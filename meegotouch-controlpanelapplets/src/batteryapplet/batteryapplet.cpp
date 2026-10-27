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
#include "batteryapplet.h"
#include "batterywidget.h"

#include "dcpbattery.h"
#include "batterywidget.h"

#include <dcpwidget.h>

#include <QtGui>
#include <MAction>

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

#undef DEBUG
#include "../debug.h"

Q_EXPORT_PLUGIN2(batteryapplet, BatteryApplet)

BatteryApplet::BatteryApplet ():
	m_MainWidget (0)
{
}

BatteryApplet::~BatteryApplet ()
{
    SYS_DEBUG ("Destroying %p", this);
    SysDebug::closeDebugFile ();
}

void BatteryApplet::init()
{
}

DcpStylableWidget *
BatteryApplet::constructStylableWidget (
		int widgetId)
{
    Q_UNUSED (widgetId);
    /*
     * Please note that the m_MainWidget is a QPointer that will nullify itself
     * when the widget is destroyed. Then we need to create a new one when we
     * asked for it.
     */
    if (m_MainWidget == NULL)
        m_MainWidget = new BatteryWidget();

    return m_MainWidget;
}

QString BatteryApplet::title() const
{
    //% "Battery"
    return qtTrId ("qtn_ener_battery");
}

QVector<MAction*>
BatteryApplet::viewMenuItems ()
{
    QVector<MAction*> vector;

    return vector;
}

DcpBrief*
BatteryApplet::constructBrief (int partId)
{
    Q_UNUSED(partId);
    return 0;
}

