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

#include "mimsettingsapplet.h"

#include <QDebug>
#include <MLibrary>
#include <MAction>
#include <MLocale>

#include <DcpStylableWidget>

#include "mimsettingswidget.h"
#include "mimsettingsbrief.h"
#include "mimsettingsconf.h"
#include "mimsettingscommon.h"
#include "mimswypesettingswidget.h"
#include "mimonscreenplugins.h"
#include "mimplugins.h"

M_LIBRARY

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(mimsettingsapplet, MImSettingsApplet)
#endif

MImSettingsApplet::MImSettingsApplet()
    : QObject()
    , onscreenPlugins(0)
    , plugins(0)
{
    // Leave initialization to init();
}

MImSettingsApplet::~MImSettingsApplet()
{
    MImSettingsConf::destroyInstance();
    // No need to delete plugins: it's a QObject with non-NULL parent.
}

void MImSettingsApplet::init()
{
    // Don't do anything here to avoid unnecessary init for brief creation
}

DcpStylableWidget* MImSettingsApplet::constructStylableWidget(int widgetId)
{
    realInit();
    DcpStylableWidget *widget = NULL;

    // Create the main settings page
    // widgetId tells the settings page number for a certain settings plug-in.
    // If we will have multiple settings pages in the future, they must
    // be constructed according to the widgetId. So far we have only one page.
    switch (widgetId) {
    case MImSettingsCommon::MainView:
        widget = new MImSettingsWidget(*onscreenPlugins, this, plugins);
        break;
    case MImSettingsCommon::SwypeSettings:
        if (storedPluginName.isEmpty())
        {
            qWarning() << "Settings for empty plugin requested";
        } else {
            widget = new MImSwypeSettingsWidget(plugins->settingsFor(storedPluginName));
        }
        break;
    default:
        qWarning() << "Unknown widget Id" << widgetId;
    }
    return widget;
}

QVector<MAction*> MImSettingsApplet::viewMenuItems()
{
    QVector<MAction*> vector;

    return vector;
}

DcpBrief* MImSettingsApplet::constructBrief(int partId)
{
    Q_UNUSED(partId);
    return new MImSettingsBrief();
}

void MImSettingsApplet::realInit()
{
    if (onscreenPlugins == 0) {
        MImSettingsConf::createInstance();
        onscreenPlugins = new MImOnScreenPlugins(this);
        plugins = new MImPlugins(this);
    }
}
