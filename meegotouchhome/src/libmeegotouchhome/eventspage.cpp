/***************************************************************************
**
** The Nokia N9's Events view (see eventspage.h).
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "eventspage.h"

#include <MBasicListItem>
#include <MImageWidget>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MPannableViewport>
#include <MRemoteAction>
#include <MStylableWidget>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusServiceWatcher>
#include <QGraphicsLinearLayout>
#include <QImage>
#include <notificationsystem/notificationsinkadaptor.h>
#include <notificationsystem/notificationgroup.h>

namespace {
const char *const ManagerService = "com.meego.core.MNotificationManager";
const char *const ManagerPath = "/notificationsinkmanager";
const char *const SinkPath = "/homeeventsfeed";

//! An event: light on black, as the N9's
class EventItem : public MBasicListItem
{
public:
    EventItem(uint id, bool group) : MBasicListItem(MBasicListItem::IconWithTitleAndSubtitle),
        id(id), group(group)
    {
        setStyleName("CommonBasicListItemInverted");
        titleLabelWidget()->setStyleName("CommonTitleInverted");
        subtitleLabelWidget()->setStyleName("CommonSubTitleInverted");
    }

    uint id;
    bool group;
    QString action;
};
}

EventsPage::EventsPage(QGraphicsItem *parent) :
    MWidget(parent),
    sink(new EventsSink(this)),
    viewport(new MPannableViewport),
    list(new MStylableWidget),
    layout(new MLayout),
    policy(new MLinearLayoutPolicy(layout, Qt::Vertical))
{
    QGraphicsLinearLayout *pageLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addItem(viewport);

    layout->setContentsMargins(0, 0, 0, 0);
    list->setStyleName("EventsList");
    list->setLayout(layout);

    // (no translation catalogue for the port's own texts yet)
    header = new MLabel("Events");
    header->setStyleName("CommonHeaderInverted");
    empty = new MLabel("No events");
    empty->setStyleName("CommonBodyTextInverted");
    empty->setAlignment(Qt::AlignCenter);

    // vertical only: a sideways pan belongs to the home's pages
    viewport->setObjectName("EventsViewport");
    viewport->setPanDirection(Qt::Vertical);
    viewport->setWidget(list);

    // the notification manager's sink interface
    qDBusRegisterMetaType<Notification>();
    qDBusRegisterMetaType<NotificationParameters>();
    qDBusRegisterMetaType<NotificationGroup>();
    connect(sink, SIGNAL(added(Notification)), SLOT(addNotification(Notification)));
    connect(sink, SIGNAL(removed(uint)), SLOT(removeNotification(uint)));
    connect(sink, SIGNAL(groupAdded(uint, NotificationParameters)),
            SLOT(addGroup(uint, NotificationParameters)));
    connect(sink, SIGNAL(groupRemoved(uint)), SLOT(removeGroup(uint)));
    new NotificationSinkAdaptor(sink);
    QDBusConnection::sessionBus().registerObject(SinkPath, sink);

    // now, and whenever the system UI (re)starts: it hands a new sink the
    // notifications it has
    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(ManagerService, QDBusConnection::sessionBus(),
                                                           QDBusServiceWatcher::WatchForRegistration, this);
    connect(watcher, SIGNAL(serviceRegistered(QString)), SLOT(registerSink()));
    registerSink();

    updateList();
}

EventsPage::~EventsPage()
{
}

void EventsPage::registerSink()
{
    QDBusConnection bus = QDBusConnection::sessionBus();
    QDBusMessage call = QDBusMessage::createMethodCall(ManagerService, ManagerPath, ManagerService, "registerSink");
    call << bus.baseService() << QString(SinkPath);
    bus.asyncCall(call);
}

void EventsPage::addNotification(const Notification &notification)
{
    notifications.insert(notification.notificationId(), notification);
    updateList();
}

void EventsPage::removeNotification(uint notificationId)
{
    notifications.remove(notificationId);
    updateList();
}

void EventsPage::addGroup(uint groupId, const NotificationParameters &parameters)
{
    groups.insert(groupId, parameters);
    updateList();
}

void EventsPage::removeGroup(uint groupId)
{
    groups.remove(groupId);
    updateList();
}

void EventsPage::updateList()
{
    while (policy->count() > 0)
        policy->removeAt(0);
    qDeleteAll(items);
    items.clear();

    policy->addItem(header);

    // what to show, newest first (by the timestamp the notification manager
    // gives each, then the id): each group with a notification in it, by its
    // newest, and the notifications without one
    QMap<uint, int> groupCounts;
    QMap<uint, quint64> groupNewest;
    QMultiMap<quint64, QPair<uint, bool> > byAge;  // age -> (id, group)
    foreach (const Notification &notification, notifications) {
        const uint groupId = notification.groupId();
        const quint64 age = (quint64(notification.parameters().value("timestamp").toUInt()) << 32)
                            | notification.notificationId();
        if (groupId && groups.contains(groupId)) {
            ++groupCounts[groupId];
            groupNewest[groupId] = qMax(groupNewest.value(groupId), age);
        } else {
            byAge.insert(age, qMakePair(notification.notificationId(), false));
        }
    }
    foreach (uint groupId, groupNewest.keys())
        byAge.insert(groupNewest.value(groupId), qMakePair(groupId, true));
    QList<QPair<uint, bool> > shown;  // id, group
    for (QMultiMap<quint64, QPair<uint, bool> >::const_iterator i = byAge.constEnd(); i != byAge.constBegin();) {
        --i;
        shown.append(i.value());
    }

    for (int i = 0; i < shown.count(); ++i) {
        const uint id = shown.at(i).first;
        const bool group = shown.at(i).second;
        const NotificationParameters &parameters = group ? groups[id] : notifications[id].parameters();

        EventItem *item = new EventItem(id, group);
        QString summary = parameters.value("summary").toString();
        const int count = group ? groupCounts.value(id) : 1;
        if (count > 1)
            summary = QString("%1 (%2)").arg(summary).arg(count);
        item->setTitle(summary);
        item->setSubtitle(parameters.value("body").toString());
        // the notification's own image, or its event type's icon
        QString icon = parameters.value("imageId").toString();
        if (icon.isEmpty())
            icon = parameters.value("iconId").toString();
        if (icon.startsWith('/'))
            item->imageWidget()->setImage(QImage(icon));
        else if (!icon.isEmpty())
            item->imageWidget()->setImage(icon);
        item->action = parameters.value("action").toString();
        connect(item, SIGNAL(clicked()), SLOT(itemClicked()));
        items.append(item);
        policy->addItem(item);
    }

    if (shown.isEmpty())
        policy->addItem(empty);
    empty->setVisible(shown.isEmpty());
}

void EventsPage::itemClicked()
{
    EventItem *item = dynamic_cast<EventItem *>(sender());
    if (!item)
        return;
    if (!item->action.isEmpty()) {
        MRemoteAction action(item->action);
        action.trigger();
    }
    // seen: the notification manager removes it (and tells the sinks)
    if (item->group)
        sink->requestGroupClearing(item->id);
    else
        sink->requestRemoval(item->id);
}
