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

#include "themeapplet.h"
#include "themewidget.h"
#include "themebrief.h"

#include <MAction>

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

#undef DEBUG
#define WARNING
#include "../debug.h"

Q_EXPORT_PLUGIN2(themeapplet, ThemeApplet)

ThemeApplet::ThemeApplet() :
    m_ThemeBusinessLogic (ThemeBusinessLogic::instance())
{
}

ThemeApplet::~ThemeApplet() 
{
    if (! m_ThemeBusinessLogic.isNull ())
        delete m_ThemeBusinessLogic;
}

void 
ThemeApplet::init()
{
}

DcpWidget *
ThemeApplet::pageMain()
{
    static int i = 0;

    SYS_DEBUG ("Entering %d", i);
    if (m_MainWidget == NULL) {
        m_MainWidget = new ThemeWidget (m_ThemeBusinessLogic);
        SYS_DEBUG ("Widget created");
    }

    SYS_DEBUG ("returning m_MainWidget: %d", i);
    ++i;
    return m_MainWidget;
}

DcpWidget *
ThemeApplet::constructWidget (
        int widgetId)
{
    static int i = 0;
    Q_UNUSED(widgetId);

    SYS_DEBUG ("We are called: %d", i);
    ++i;
    return pageMain ();
}

QString
ThemeApplet::title() const
{
    //% "Theme"
    return qtTrId ("qtn_teme_theme"); // This is not official logical id
}

QVector<MAction*>
ThemeApplet::viewMenuItems()
{
    QVector<MAction*> vector;

    return vector;
}

DcpBrief *
ThemeApplet::constructBrief (
        int partId)
{
    Q_UNUSED (partId);
    SYS_DEBUG ("Creating brief");
    return new ThemeBrief (m_ThemeBusinessLogic);
}

