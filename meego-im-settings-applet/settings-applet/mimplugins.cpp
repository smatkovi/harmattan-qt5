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

#include "mimplugins.h"

#include <QDebug>

#include <minputmethodplugin.h>
#include "mimpluginsloader.h"

MImPlugins::MImPlugins(QObject *parent) :
    QObject(parent),
    pluginMap(),
    loader(new MImPluginsLoader(this, this))
{
    connect(loader, SIGNAL(done()), this, SIGNAL(loadDone()));
}

MImPlugins::~MImPlugins()
{
    // No need to do anything here.
    // Since loader is a QObject with non-NULL parent, Qt will handle it.
}

MAbstractInputMethodSettings *MImPlugins::settingsFor(const QString &pluginName)
{
    MInputMethodPlugin *plugin = 0;
    plugin = pluginMap.value(pluginName,0);
    if (plugin == 0) {
        return 0;
    }
    return plugin->createInputMethodSettings();
}

void MImPlugins::startLoading()
{
    loader->start();
}

void MImPlugins::addPlugin(const QString &pluginName, MInputMethodPlugin *plugin)
{
    if (pluginMap.contains(pluginName)) {
        qWarning() << __PRETTY_FUNCTION__
                << "Trying to add plugin that already exists:"
                << pluginName;
    } else {
        pluginMap.insert(pluginName, plugin);
    }
    // emit signal even for already-loaded plugins
    // (to see them when returning to applet):
    emit loaded(pluginName);
}
