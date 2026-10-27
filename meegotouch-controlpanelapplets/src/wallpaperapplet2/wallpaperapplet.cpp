/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
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

#include "wallpaperapplet.h"
#include "wallpaperwidget.h"
#include "wallpaperviewwidget.h"
#include "wallpaperconfiguration.h"
#include "wallpaperlistsheet.h"
#include <stdlib.h>

#include <MAction>

Q_EXPORT_PLUGIN2(wallpaperapplet2, WallpaperApplet)

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

#define DEBUG
#define WARNING
#include "../debug.h"


WallpaperApplet::WallpaperApplet() :
    m_WallpaperBusinessLogic (0)
{
}

WallpaperApplet::~WallpaperApplet() 
{
    //SYS_WARNING ("Destroying applet...");
}

void 
WallpaperApplet::init()
{
    /*
     * TODO: FIXME: this has to be done only once!
     */
    SYS_DEBUG ("Calling duplicate finder...");
    system ("/usr/bin/wallpaper_duplicate_finder.sh");
}

DcpStylableWidget *
WallpaperApplet::pageMain(
        int widgetId)
{
    SYS_DEBUG ("widgetId = %d", widgetId);
    if (!m_WallpaperBusinessLogic)
        m_WallpaperBusinessLogic = new WallpaperBusinessLogic;

    switch (widgetId) {
        case MainWidget:
            if (m_MainWidget == 0) 
                m_MainWidget = new WallpaperWidget (m_WallpaperBusinessLogic);
            return m_MainWidget;

        case EditorWidget:
            if (m_EditorWidget == 0) 
                m_EditorWidget = new WallpaperViewWidget (
                        m_WallpaperBusinessLogic);

            return m_EditorWidget;

        default:
            SYS_WARNING ("Unknown widgetId: %d", widgetId);
    }

    return 0;
}

DcpStylableWidget *
WallpaperApplet::constructStylableWidget (
        int widgetId)
{
    SYS_DEBUG ("");
    if (Wallpaper::useSheetForMainView)
        return 0;

    return pageMain (widgetId);
}

MSheet *
WallpaperApplet::constructSheet (
        int widgetId)
{
    SYS_DEBUG ("*** widgetId = %d", widgetId);
    Q_UNUSED (widgetId);

    if (!Wallpaper::useSheetForMainView)
        return 0;

    if (!m_WallpaperBusinessLogic)
        m_WallpaperBusinessLogic = new WallpaperBusinessLogic;

    return new WallpaperListSheet (m_WallpaperBusinessLogic);
}

QString
WallpaperApplet::title() const
{
    //% "Wallpaper"
    return qtTrId ("qtn_wall_wallpaper");
}

QVector<MAction*>
WallpaperApplet::viewMenuItems()
{
    QVector<MAction*>   vector;

    return vector;
}

DcpBrief *
WallpaperApplet::constructBrief (
        int partId)
{
    Q_UNUSED (partId);

    return 0;
}

