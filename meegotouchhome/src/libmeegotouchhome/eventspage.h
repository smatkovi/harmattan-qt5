/***************************************************************************
**
** The Nokia N9's Events view, as the first page of the home screen's
** three: the notifications of the system UI (its notification manager
** hands them to this page as to a sink, com.meego.core.MNotificationSink),
** newest first, a tap runs a notification's action and dismisses it.
** Written anew for the Qt 5 port: the N9's home (1.3.2) and its event
** feeds (libmeegotouchevents) were not released.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef EVENTSPAGE_H
#define EVENTSPAGE_H

#include <MWidget>
#include <QMap>
#include <notificationsystem/notificationsink.h>
#include <notificationsystem/notification.h>
#include <notificationsystem/notificationparameters.h>

class MBasicListItem;
class MLabel;
class MLayout;
class MLinearLayoutPolicy;
class MPannableViewport;
class MStylableWidget;

//! What the notification manager hands the page
class EventsSink : public NotificationSink
{
    Q_OBJECT

public:
    EventsSink(QObject *parent = 0) : NotificationSink(parent) {}

    virtual void addNotification(const Notification &notification) { emit added(notification); }
    virtual void removeNotification(uint notificationId) { emit removed(notificationId); }
    virtual void addGroup(uint groupId, const NotificationParameters &parameters) { emit groupAdded(groupId, parameters); }
    virtual void removeGroup(uint groupId) { emit groupRemoved(groupId); }

    // the page asks the manager to dismiss these
    void requestRemoval(uint notificationId) { emit notificationRemovalRequested(notificationId); }
    void requestGroupClearing(uint groupId) { emit notificationGroupClearingRequested(groupId); }

signals:
    void added(const Notification &notification);
    void removed(uint notificationId);
    void groupAdded(uint groupId, const NotificationParameters &parameters);
    void groupRemoved(uint groupId);
};

class EventsPage : public MWidget
{
    Q_OBJECT

public:
    EventsPage(QGraphicsItem *parent = 0);
    virtual ~EventsPage();

private Q_SLOTS:
    //! Registers the page with the system UI's notification manager
    void registerSink();
    void addNotification(const Notification &notification);
    void removeNotification(uint notificationId);
    void addGroup(uint groupId, const NotificationParameters &parameters);
    void removeGroup(uint groupId);
    //! An event's item was tapped
    void itemClicked();

private:
    //! The list again: the groups and the notifications without one, newest first
    void updateList();
    EventsSink *sink;
    MPannableViewport *viewport;
    MStylableWidget *list;
    MLayout *layout;
    MLinearLayoutPolicy *policy;
    MLabel *header;
    MLabel *empty;
    //! By id
    QMap<uint, Notification> notifications;
    QMap<uint, NotificationParameters> groups;
    QList<MBasicListItem *> items;
};

#endif
