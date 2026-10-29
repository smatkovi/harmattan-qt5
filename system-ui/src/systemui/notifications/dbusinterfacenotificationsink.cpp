/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of systemui.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#include "dbusinterfacenotificationsink.h"
#include "dbusinterfacenotificationsinkadaptor.h"
#include "dbusinterfacenotificationsinkproxy.h"
#include "notificationmanagerinterface.h"
#include <QDBusServiceWatcher>

DBusInterfaceNotificationSink::DBusInterfaceNotificationSink(NotificationManagerInterface *interface) :
        serviceWatcher(new QDBusServiceWatcher(this)),
        notificationManager(interface)
{
    new DBusInterfaceNotificationSinkAdaptor(this);
    // A sink whose process goes (the home screen restarting, with a new
    // unique bus name) never unregisters: its proxies go with its name
    serviceWatcher->setConnection(QDBusConnection::sessionBus());
    serviceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(serviceWatcher, SIGNAL(serviceUnregistered(QString)), this, SLOT(sinkServiceUnregistered(QString)));
}

DBusInterfaceNotificationSink::~DBusInterfaceNotificationSink()
{
}

void DBusInterfaceNotificationSink::registerSink(const QString &service, const QString &path)
{
    ProxyAddress proxy(service, path);
    DBusInterface proxyInterface(new DBusInterfaceNotificationSinkProxy(service, path, QDBusConnection::sessionBus()));
    proxies.insert(proxy, proxyInterface);
    serviceWatcher->addWatchedService(service);
    QDBusConnection::sessionBus().connect(service, path, DBusInterfaceNotificationSinkProxy::staticInterfaceName(), "notificationRemovalRequested", this, SIGNAL(notificationRemovalRequested(uint)));
    QDBusConnection::sessionBus().connect(service, path, DBusInterfaceNotificationSinkProxy::staticInterfaceName(), "notificationGroupClearingRequested", this, SIGNAL(notificationGroupClearingRequested(uint)));
    sendCurrentNotifications(proxyInterface);
}

void DBusInterfaceNotificationSink::unregisterSink(const QString &service, const QString &path)
{
    removeProxy(ProxyAddress(service, path));
    foreach (const ProxyAddress &address, proxies.keys()) {
        if (address.first == service) {
            return;
        }
    }
    serviceWatcher->removeWatchedService(service);
}

void DBusInterfaceNotificationSink::removeProxy(const ProxyAddress &address)
{
    if (proxies.remove(address) == 0) {
        return;
    }
    QDBusConnection::sessionBus().disconnect(address.first, address.second, DBusInterfaceNotificationSinkProxy::staticInterfaceName(), "notificationRemovalRequested", this, SIGNAL(notificationRemovalRequested(uint)));
    QDBusConnection::sessionBus().disconnect(address.first, address.second, DBusInterfaceNotificationSinkProxy::staticInterfaceName(), "notificationGroupClearingRequested", this, SIGNAL(notificationGroupClearingRequested(uint)));
}

void DBusInterfaceNotificationSink::sinkServiceUnregistered(const QString &service)
{
    foreach (const ProxyAddress &address, proxies.keys()) {
        if (address.first == service) {
            removeProxy(address);
        }
    }
    serviceWatcher->removeWatchedService(service);
}

void DBusInterfaceNotificationSink::addNotification(const Notification &notification)
{
    if (notification.type() != Notification::SystemEvent) {
        // Do not handle system events at all
        foreach (QSharedPointer<DBusInterfaceNotificationSinkProxy> proxy, proxies) {
            proxy->addNotification(notification);
        }
    }
}

void DBusInterfaceNotificationSink::removeNotification(uint notificationId)
{
    foreach (QSharedPointer<DBusInterfaceNotificationSinkProxy> proxy, proxies) {
        proxy->removeNotification(notificationId);
    }
}

void DBusInterfaceNotificationSink::addGroup(uint groupId, const NotificationParameters &parameters)
{
    foreach (QSharedPointer<DBusInterfaceNotificationSinkProxy> proxy, proxies) {
        proxy->addGroup(groupId, parameters);
    }
}

void DBusInterfaceNotificationSink::removeGroup(uint groupId)
{
    foreach (QSharedPointer<DBusInterfaceNotificationSinkProxy> proxy, proxies) {
        proxy->removeGroup(groupId);
    }
}

void DBusInterfaceNotificationSink::sendGroupsToProxy(const QList<NotificationGroup> &groups, const DBusInterface &proxy) const
{
    foreach(const NotificationGroup &group, groups) {
        proxy->addGroup(group.groupId(), group.parameters());
    }
}

void DBusInterfaceNotificationSink::sendNotificationsToProxy(const QList<Notification> &notifications, const DBusInterface &proxy) const
{
    foreach(const Notification &notification, notifications) {
        if (notification.type() != Notification::SystemEvent) {
            // Do not handle system events at all
            proxy->addNotification(notification);
        }
    }
}

void DBusInterfaceNotificationSink::sendCurrentNotifications(const DBusInterface &proxyInterface) const
{
    if (notificationManager != NULL) {
        QList<NotificationGroup> groups = notificationManager->groups();
        sendGroupsToProxy(groups, proxyInterface);

        QList<Notification> notifications = notificationManager->notifications();
        sendNotificationsToProxy(notifications, proxyInterface);
    }
}
