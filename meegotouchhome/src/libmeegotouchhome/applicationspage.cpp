/***************************************************************************
**
** The Nokia N9's Applications view (see applicationspage.h).
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "applicationspage.h"
#include "launcherbutton.h"
#include "launcherdatastore.h"

#include <MDesktopEntry>
#include <MFlowLayoutPolicy>
#include <MLayout>
#include <MPannableViewport>
#include <MStylableWidget>
#include <QGraphicsLinearLayout>

namespace {
bool byName(const LauncherButton *a, const LauncherButton *b)
{
    return QString::localeAwareCompare(a->text(), b->text()) < 0;
}
}

ApplicationsPage::ApplicationsPage(LauncherDataStore *dataStore, QGraphicsItem *parent) :
    MWidget(parent),
    dataStore(dataStore),
    viewport(new MPannableViewport),
    grid(new MStylableWidget),
    layout(new MLayout),
    policy(new MFlowLayoutPolicy(layout))
{
    QGraphicsLinearLayout *pageLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addItem(viewport);

    // the launcher's page styles keep the buttons clear of the status bar
    layout->setContentsMargins(0, 0, 0, 0);
    policy->setObjectName("LauncherPageFlowLayoutPolicy");
    grid->setStyleName("ApplicationsGrid");
    grid->setLayout(layout);

    // vertical only: a sideways pan belongs to the home's pages
    viewport->setObjectName("ApplicationsViewport");
    viewport->setPanDirection(Qt::Vertical);
    viewport->setWidget(grid);

    connect(dataStore, SIGNAL(dataStoreChanged()), SLOT(updateButtons()));
    connect(dataStore, SIGNAL(desktopEntryAdded(QSharedPointer<MDesktopEntry>)), SLOT(updateButtons()));
    connect(dataStore, SIGNAL(desktopEntryRemoved(QString)), SLOT(updateButtons()));
    connect(dataStore, SIGNAL(desktopEntryChanged(QSharedPointer<MDesktopEntry>)),
            SLOT(updateButton(QSharedPointer<MDesktopEntry>)));
    updateButtons();
}

ApplicationsPage::~ApplicationsPage()
{
}

void ApplicationsPage::scrollToTop()
{
    viewport->setPosition(QPointF(0, 0));
}

void ApplicationsPage::updateButtons()
{
    const QStringList entries = dataStore->dataForAllDesktopEntries().keys();

    // the applications gone
    foreach (const QString &path, buttons.keys()) {
        if (!entries.contains(path)) {
            LauncherButton *button = buttons.take(path);
            policy->removeItem(button);
            delete button;
        }
    }
    // the new ones
    foreach (const QString &path, entries) {
        if (!buttons.contains(path)) {
            QSharedPointer<MDesktopEntry> entry(new MDesktopEntry(path));
            if (!entry->isValid())
                continue;
            LauncherButton *button = new LauncherButton(entry);
            button->setObjectName("LauncherButton");
            buttons.insert(path, button);
        }
    }

    // all of them by name
    while (policy->count() > 0)
        policy->removeAt(0);
    QList<LauncherButton *> sorted = buttons.values();
    qSort(sorted.begin(), sorted.end(), byName);
    foreach (LauncherButton *button, sorted)
        policy->addItem(button);
}

void ApplicationsPage::updateButton(const QSharedPointer<MDesktopEntry> &entry)
{
    LauncherButton *button = buttons.value(entry->fileName());
    if (button)
        button->updateFromDesktopEntry(entry);
    updateButtons();
}
