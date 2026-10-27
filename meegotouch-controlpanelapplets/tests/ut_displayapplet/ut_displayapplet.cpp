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

#include "ut_displayapplet.h"
#include "displayapplet.h"
#include "displaywidget.h"

#include <dcpwidgettypes.h>

#include <MApplication>
#include <MComboBox>
#include <MAction>
#include <QVector>

#define DEBUG
#include "../../src/debug.h"


/******************************************************************************
 * Ut_DisplayApplet implementation. 
 */
void 
Ut_DisplayApplet::init()
{
}

void 
Ut_DisplayApplet::cleanup()
{
}

static int argc = 1;
static char *app_name = (char*) "./ut_displayapplet";

void 
Ut_DisplayApplet::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_Applet = new DisplayApplet;
    
    QVERIFY (!m_Applet->m_MainWidget);
    m_Applet->init ();
    
    /*
     * Testing if the widget is not created yet.
     */
    QVERIFY (!m_Applet->m_MainWidget);
}

void 
Ut_DisplayApplet::cleanupTestCase()
{
    delete m_Applet;
    m_App->deleteLater ();
}

void 
Ut_DisplayApplet::testTitle ()
{
#if 0
    QString title = m_Applet->title();
    QVERIFY (title.isEmpty());
#endif
}

void
Ut_DisplayApplet::testConstructWidget ()
{
    DcpWidget *widget;
    bool       backAccepted;

    /*
     * Testing if the applet creates a widget the first time.
     */
    widget = m_Applet->constructWidget (0);
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
Ut_DisplayApplet::testMenuItems ()
{
    QVector<MAction*> items = m_Applet->viewMenuItems ();
    
    QVERIFY (items.size() == 0);
}

QTEST_APPLESS_MAIN(Ut_DisplayApplet)

