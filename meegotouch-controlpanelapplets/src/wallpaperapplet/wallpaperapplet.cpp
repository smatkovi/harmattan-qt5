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
#include "wallpapereditorwidget.h"

#include <MAction>
#include "wallpaperthumb.h"

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

//#define DEBUG
#define WARNING
#include "../debug.h"

Q_EXPORT_PLUGIN2(wallpaperapplet, WallpaperApplet)

WallpaperApplet::WallpaperApplet() :
    m_WallpaperBusinessLogic (0)
{
    SYS_WARNING ("Creating applet...");
}

WallpaperApplet::~WallpaperApplet() 
{
    SYS_WARNING ("Destroying applet and the Thumbnailer");
    delete WallpaperThumb::getInstance ();
}

void 
WallpaperApplet::init()
{
}

DcpWidget *
WallpaperApplet::pageMain(
        int widgetId)
{
    SYS_DEBUG ("widgetId = %d", widgetId);
    if (!m_WallpaperBusinessLogic)
        m_WallpaperBusinessLogic = new WallpaperBusinessLogic (this);

    switch (widgetId) {
        case MainWidget:
            if (m_MainWidget == 0) 
                m_MainWidget = new WallpaperWidget (m_WallpaperBusinessLogic);
            return m_MainWidget;

        case EditorWidget:
            if (m_EditorWidget == 0)
                m_EditorWidget = new WallpaperEditorWidget (
                        m_WallpaperBusinessLogic);
            return m_EditorWidget;

        default:
            SYS_WARNING ("Unknown widgetId: %d", widgetId);
    }

    return 0;
}

DcpWidget *
WallpaperApplet::constructWidget (
        int widgetId)
{
    return pageMain (widgetId);
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

