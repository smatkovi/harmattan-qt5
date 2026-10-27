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

#include "ut_resetapplet.h"
#include "resetapplet.h"
#include "resetwidget.h"

#include "resetbrief.h"

#include <dcpwidgettypes.h>
#include <dcpbrief.h>

#include <mdesktopentry.h>
#include <MApplication>
#include <MAction>
#include <MDialog>
#include <QVector>

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Stubbing the system() C library function here.
 */
static QString lastExecutedCommand;
static int     commandSuccess;

int
system (
        const char *command)
{
    SYS_DEBUG ("*** command = %s", command);

    lastExecutedCommand = command;
    return commandSuccess;
}

/******************************************************************************
 * Ut_ResetApplet implementation. 
 */
void 
Ut_ResetApplet::init()
{
}

void 
Ut_ResetApplet::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./ut_resetapplet";

void 
Ut_ResetApplet::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_Applet = new ResetApplet;
    
    QVERIFY (!m_Applet->m_MainWidget);
    m_Applet->init ();
    
    /*
     * Testing if the widget is not created yet.
     */
    QVERIFY (!m_Applet->m_MainWidget);
}

void 
Ut_ResetApplet::cleanupTestCase()
{
    delete m_Applet;
    m_App->deleteLater ();
}

void 
Ut_ResetApplet::testTitle ()
{
    QString title = m_Applet->title();
    QVERIFY (!title.isEmpty());
}

void
Ut_ResetApplet::testConstructWidget ()
{
    ResetWidget *widget;
    bool         backAccepted;

    /*
     * We check if we got a NULL pointer for the invalid window ID.
     */
    widget = (ResetWidget *) m_Applet->constructWidget (3);
    QVERIFY (!widget);

    /*
     * Testing if the applet creates a widget the first time.
     */
    widget = (ResetWidget *) m_Applet->constructWidget (0);
    QVERIFY (widget);
    QVERIFY (m_Applet->m_MainWidget == widget);
    
    /*
     * Testing if the widget accepts the back. Our applets always accept back.
     */
    backAccepted = widget->back();
    QVERIFY (backAccepted);

    /*
     * Testing the ResetWidget. We could write a separate unit test, but it
     * would be very primitive...
     */
    // Restore with 'no' as answer.
    lastExecutedCommand = "";
    commandSuccess = 0;
    widget->restoreActivated ();
    widget->doTheWork ();
    QVERIFY (lastExecutedCommand == "");
    
    // Restore with 'yes' as answer.
    lastExecutedCommand = "";
    commandSuccess = 0;
    widget->restoreActivated ();
    // simulate yes
    widget->restoreConfirmed ();
    widget->doTheWork ();
    QVERIFY (lastExecutedCommand == "/usr/sbin/clean-device.sh --rfs");
    
    // Clear with 'no' as answer.
    lastExecutedCommand = "";
    commandSuccess = 0;
    widget->clearActivated ();
    widget->doTheWork ();
    QVERIFY (lastExecutedCommand == "");
    
    // Clear with 'yes' as answer.
    lastExecutedCommand = "";
    commandSuccess = 0;
    widget->clearActivated ();
    widget->clearConfirmed ();
    widget->doTheWork ();
    QVERIFY (lastExecutedCommand == "/usr/sbin/clean-device.sh --cud-reset");

    /*
     * Testing if the applet knows about the destruction of the widget.
     */
    delete widget;
    QVERIFY (!m_Applet->m_MainWidget);
}

void 
Ut_ResetApplet::testMenuItems ()
{
    QVector<MAction*> items = m_Applet->viewMenuItems ();
    
    QVERIFY (items.size() == 0);
}

void 
Ut_ResetApplet::testConstructbrief ()
{
    ResetBrief *brief1 = (ResetBrief *) m_Applet->constructBrief(0);
    QString   iconName;
    QString   text;

    QVERIFY (brief1);
    QVERIFY (brief1->widgetTypeID() == DcpWidgetType::Label);

    iconName = brief1->icon ();
    QVERIFY (iconName.isEmpty());

    text = brief1->valueText ();
    QVERIFY (text.isEmpty());

    // FIXME: Why does this method has an argument, it must be some leftover
    // code. Also: we should check if the signal is emitted.
    delete brief1;
}

/*!
 * Testing the whole functionality of the ResetBusinesslogic. We could write a
 * separate unit test for this class, but it is very primitive, so we just do
 * the job here.
 */
void 
Ut_ResetApplet::testResetBusinessLogic ()
{
    // So we get a better coverage we use an error code sometimes.
    commandSuccess = 3;

    m_Applet->m_ResetBusinessLogic->performRestoreSettings ();
    QVERIFY (lastExecutedCommand == "/usr/sbin/clean-device.sh --rfs");

    m_Applet->m_ResetBusinessLogic->performClearData ();
    QVERIFY (lastExecutedCommand == "/usr/sbin/clean-device.sh --cud-reset");
}


void
Ut_ResetApplet::testShowMassStorageWarning ()
{

      ResetWidget *widget;
      widget = (ResetWidget *) m_Applet->constructWidget (0);
      widget->showMassStorageWarning ();
	    
	    
           // infoBanner->setTitle (qtTrId ("qtn_rset_not_possible"));
 
	        QVERIFY (widget->infoBanner->title() == "qtn_rset_not_possible");
}







QTEST_APPLESS_MAIN(Ut_ResetApplet)




