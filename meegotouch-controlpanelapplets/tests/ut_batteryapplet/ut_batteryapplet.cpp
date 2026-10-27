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

#include "ut_batteryapplet.h"
#include "batteryapplet.h"
#include "batterywidget.h"

#include <dcpwidgettypes.h>
#include <dcpstylablewidget.h>
#include <MApplication>
#include <MAction>
#include <QVector>

#define DEBUG
#include "../../src/debug.h"


/******************************************************************************
 * Ut_BatteryApplet implementation. 
 */
void 
Ut_BatteryApplet::init()
{
}

void 
Ut_BatteryApplet::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_BatteryApplet";

void 
Ut_BatteryApplet::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_Applet = new BatteryApplet;
    
    QVERIFY (!m_Applet->m_MainWidget);
    m_Applet->init ();
    
    /*
     * Testing if the widget is not created yet.
     */
    QVERIFY (!m_Applet->m_MainWidget);
}

void 
Ut_BatteryApplet::cleanupTestCase()
{
    delete m_Applet;
    m_App->deleteLater ();
}

void 
Ut_BatteryApplet::testTitle ()
{
    QString title = m_Applet->title();
    QVERIFY (!title.isEmpty());
}

void
Ut_BatteryApplet::testConstructWidget ()
{
    DcpStylableWidget *widget;
    bool       backAccepted;

    /*
     * Testing if the applet creates a widget the first time.
     */
    widget = m_Applet->constructStylableWidget (0);
    QVERIFY (widget);
    QVERIFY (m_Applet->m_MainWidget == widget);
    
    /*
     * Testing if the widget accepts the back. Our applets always accept back.
     */
    backAccepted = widget->back();
    QVERIFY (backAccepted);

    /*
     * Testing if the applet knows about the destruction of the widget.
     */
    delete widget;
    QVERIFY (!m_Applet->m_MainWidget);
}

void 
Ut_BatteryApplet::testMenuItems ()
{
    QVector<MAction*> items = m_Applet->viewMenuItems ();
   
    SYS_DEBUG ("*** menu has %d items", items.size());
    /*
     * Seems that someone changed the code, we have no menu items.
     */
    QVERIFY (items.size() == 0);
}

QTEST_APPLESS_MAIN(Ut_BatteryApplet)

