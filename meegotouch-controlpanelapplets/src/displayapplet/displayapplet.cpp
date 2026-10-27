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
#include "displayapplet.h"
#include "displaywidget.h"
#include "brightnessbrief.h"

#include "dcpdisplay.h"
#include <dcpwidget.h>

#include <QtPlugin>

#include <MAction>

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

#undef DEBUG
#include "../debug.h"

Q_EXPORT_PLUGIN2(displayapplet, DisplayApplet)

void 
DisplayApplet::init()
{
}

DcpWidget* DisplayApplet::constructWidget(int widgetId)
{
    Q_UNUSED(widgetId);
    return pageMain();
}

DcpWidget* DisplayApplet::pageMain()
{
    if (m_MainWidget == NULL)
        m_MainWidget = new DisplayWidget;

    return m_MainWidget;
}

#if 0
QString
DisplayApplet::title() const
{
    //% "Display"
    return qtTrId ("qtn_disp_display");
}
#endif

QVector<MAction*> DisplayApplet::viewMenuItems()
{
    QVector<MAction*> vector;

    return vector;
}

DcpBrief* DisplayApplet::constructBrief(int partId)
{
    DcpBrief* result = 0;
    switch (partId) {
        case BrightnessPart:
            result = new BrightnessBrief;
            break;
        default:
            break;
    };
    return result;
}

int DisplayApplet::partID (const QString& partStr)
{
    return partStr == "Brightness" ? BrightnessPart : DefaultPart;
}

