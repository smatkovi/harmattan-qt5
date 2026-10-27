/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mimpluginsloader.h"

#include <QString>
#include <QStringList>
#include <QPluginLoader>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QTimer>

#include <mimsettings.h>
#include <minputmethodplugin.h>

#include "mimplugins.h"

namespace
{
    const char * const DefaultPluginLocation(M_IM_PLUGINS_DIR);

    const QString ConfigRoot = MALIIT_CONFIG_ROOT;
    const QString MImPluginPaths = ConfigRoot + "paths";
    const QString MImPluginDisabled = ConfigRoot + "disabledpluginfiles";

    // Period between successive plugin loads, to allow UI to update:
    const int NextLoadDelayMs(50);
}

MImPluginsLoader::MImPluginsLoader(MImPlugins *pl, QObject *parent)
    : QObject(parent)
    , plugins(pl)
    , workQueue()
{
}

MImPluginsLoader::~MImPluginsLoader()
{
    // Nothing to do here.
}

void MImPluginsLoader::start()
{
    QStringList paths(MImSettings(MImPluginPaths).value(QStringList(DefaultPluginLocation)).toStringList());
    QStringList blacklist(MImSettings(MImPluginDisabled).value().toStringList());

    foreach (QString path, paths) {
        QDir pluginsDir(path);
        foreach (const QString &fileName, pluginsDir.entryList(QDir::Files)) {
            if (blacklist.contains(fileName)) {
                qWarning() << __PRETTY_FUNCTION__ << fileName << "is on the blacklist, skipped.";
            } else {
                workQueue << pluginsDir.absoluteFilePath(fileName);
            }
        }
    }
    QTimer::singleShot(NextLoadDelayMs, this, SLOT(loadNext()));
}

void MImPluginsLoader::loadNext()
{
    if (workQueue.isEmpty()) {
        emit done();
    } else {
        loadPlugin(workQueue.takeFirst());
        QTimer::singleShot(NextLoadDelayMs, this, SLOT(loadNext()));
    }
}

void MImPluginsLoader::loadPlugin(const QString &fileName)
{
    QPluginLoader load(fileName);
    QObject *pluginInstance = load.instance();

    if (!pluginInstance) {
        qWarning() << __PRETTY_FUNCTION__ << "Error loading plugin from "
                   << fileName << load.errorString();
    } else {
        MInputMethodPlugin *plugin = qobject_cast<MInputMethodPlugin *>(pluginInstance);
        if (plugin) {
            if (!plugin->supportedStates().isEmpty()) {
                QString pluginName = QFileInfo(load.fileName()).fileName();
                plugins->addPlugin(pluginName, plugin);
            } else {
                qWarning() << __PRETTY_FUNCTION__
                           << "Plugin does not support any state: " << fileName;
            }
        } else {
            qWarning() << __PRETTY_FUNCTION__
                       << "Plugin is not MInputMethodPlugin: " << fileName;
        }
    }
}
