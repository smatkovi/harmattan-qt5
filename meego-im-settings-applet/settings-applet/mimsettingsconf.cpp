/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimsettingsconf.h"

#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

#include <minputmethodnamespace.h>

namespace
{
    const char * const DBusMIMPluginManagerServiceName = "com.meego.inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerPath = "/com/meego/inputmethodpluginmanager1";
    const char * const DBusMIMPluginManagerInterface = "com.meego.inputmethodpluginmanager1";
}

MImSettingsConf *MImSettingsConf::imSettingsConfInstance = 0;

MImSettingsConf::MImSettingsConf()
{
    connectToIMPluginManagerDBus();
}

MImSettingsConf::~MImSettingsConf()
{
    // No clean-up needed here.
}

void MImSettingsConf::createInstance()
{
    Q_ASSERT(!imSettingsConfInstance);
    if (!imSettingsConfInstance) {
        imSettingsConfInstance = new MImSettingsConf;
    }
}

void MImSettingsConf::destroyInstance()
{
    Q_ASSERT(imSettingsConfInstance);
    delete imSettingsConfInstance;
    imSettingsConfInstance = 0;
}

QList<MImSettingsConf::MImSubView> MImSettingsConf::subViews() const
{
    QList<MImSubView> views;
    if (impluginMgrIface) {
        QDBusReply<QStringList> reply
            = impluginMgrIface->call("queryAvailablePlugins", MInputMethod::OnScreen);
        if (reply.isValid()) {
            foreach (const QString &plugin, reply.value()) {
                QDBusReply< QMap<QString, QVariant> > replySubViews
                    = impluginMgrIface->call("queryAvailableSubViews",
                                             plugin, MInputMethod::OnScreen);
                if (replySubViews.isValid()) {
                    QMap<QString, QVariant> sv = replySubViews.value();
                    QMap<QString, QVariant>::const_iterator iterator = sv.constBegin();
                    while (iterator != sv.constEnd()) {
                        MImSubView subView = { plugin, iterator.key(), iterator.value().toString()};
                        views << subView;
                        iterator++;
                    }
                }
            }
        }
    }
    return views;
}

void MImSettingsConf::connectToIMPluginManagerDBus()
{
    qDebug() << __PRETTY_FUNCTION__;
    QDBusConnection connection = QDBusConnection::sessionBus();

    if (!connection.isConnected()) {
        qWarning() << "Cannot connect to the DBus session bus";
        return;
    }
    impluginMgrIface = new QDBusInterface(DBusMIMPluginManagerServiceName, DBusMIMPluginManagerPath,
                                          DBusMIMPluginManagerInterface, connection, this);

    if (!impluginMgrIface->isValid()) {
        qWarning() << "MImSettingsConf was unable to connect to indicator server: "
                   << connection.lastError().message();
        delete impluginMgrIface;
        impluginMgrIface = 0;
    }
}
