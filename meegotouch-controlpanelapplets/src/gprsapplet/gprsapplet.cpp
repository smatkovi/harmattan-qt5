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

#include "gprsapplet.h"
#include "gprswidget.h"
#include "gprsbrief.h"

#include <MTheme>
#include <MAction>

#define DEBUG
#include "../debug.h"

Q_EXPORT_PLUGIN2(gprsapplet, gprsApplet)

const QString cssDir = "/usr/share/themes/base/meegotouch/libgprsapplet/style/";

gprsApplet::gprsApplet() :
    m_gprsBusinessLogic (new gprsBusinessLogic)
{
}

gprsApplet::~gprsApplet()
{
}

void 
gprsApplet::init()
{
    QString themeFile = cssDir + "gprsapplet.css";

    SYS_DEBUG ("Loading theme file: %s", SYS_STR(themeFile));
    MTheme::loadCSS (themeFile);
}

DcpWidget *
gprsApplet::pageMain(
        int widgetId)
{
    SYS_DEBUG ("widgetId = %d", widgetId);
    switch (widgetId) {
        case 0:
            if (m_MainWidget == 0) 
                m_MainWidget = new gprsWidget (m_gprsBusinessLogic);
            return m_MainWidget;

        default:
            SYS_WARNING ("Unknown widgetId: %d", widgetId);
    }

    return 0;
}

DcpWidget *
gprsApplet::constructWidget (
        int widgetId)
{
    SYS_DEBUG ("-----------------------------------");
    SYS_DEBUG ("*** widgetId = %d", widgetId);
    return pageMain (widgetId);
}

QString
gprsApplet::title() const
{
    //% "gprs APN"
    return qtTrId ("qtn_prod_gprs_connectivity");
}

QVector<MAction*>
gprsApplet::viewMenuItems()
{
    QVector<MAction*>   vector;

    return vector;
}

DcpBrief *
gprsApplet::constructBrief (
        int partId)
{
    Q_UNUSED (partId);
    return new gprsBrief (m_gprsBusinessLogic);
}
