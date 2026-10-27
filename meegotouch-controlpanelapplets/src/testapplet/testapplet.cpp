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

#include "testapplet.h"
#include "testwidget.h"
#include "testbrief.h"

#include <MAction>

#undef DEBUG
#include "../debug.h"

Q_EXPORT_PLUGIN2(testapplet, TestApplet)


TestApplet::TestApplet() 
{
}

TestApplet::~TestApplet() 
{
}

void 
TestApplet::init()
{
}

DcpWidget *
TestApplet::pageMain(
        int Id)
{
    static int i = 0;

    SYS_DEBUG ("Entering %dnth times for id %d", i, Id);
    ++i;

    switch (Id) {
        case 0:
            if (m_MainWidget == NULL) {
                m_MainWidget = new TestWidget (Id);
                SYS_DEBUG ("Widget created");
            }

            return m_MainWidget;

        case 1:
            if (m_Page1 == NULL) {
                m_Page1 = new TestWidget (Id);
                 SYS_DEBUG ("Widget created");
            }

            return m_Page1;
        
        case 2:
            if (m_Page2 == NULL) {
                m_Page2 = new TestWidget (Id);
                 SYS_DEBUG ("Widget created");
            }

            return m_Page2;

        case 3:
            if (m_Page3 == NULL) {
                m_Page3 = new TestWidget (Id);
                 SYS_DEBUG ("Widget created");
            }

            return m_Page3;

        case 4:
            if (m_Page4 == NULL) {
                m_Page4 = new TestWidget (Id);
                 SYS_DEBUG ("Widget created");
            }

            return m_Page4;

        case 5:
            if (m_Page5 == NULL) {
                m_Page5 = new TestWidget (Id);
                 SYS_DEBUG ("Widget created");
            }

            return m_Page5;

        default:
            SYS_WARNING ("Invalid Id: %d", Id);
            Q_ASSERT (false);
    }

    return m_MainWidget;
}

DcpWidget *
TestApplet::constructWidget (
        int widgetId)
{
    Q_ASSERT (widgetId >= 0);
    Q_ASSERT (widgetId <= 5);

    SYS_DEBUG ("-----------------------------------");
    SYS_DEBUG ("*** widgetId = %d", widgetId);

    if (widgetId == 0 && m_MainWidget != 0) {
        SYS_WARNING ("m_MainWidget already exists!");
    } else if (widgetId == 1 && m_Page1 != 0) {
        SYS_WARNING ("m_Page1 already exists");
    } else if (widgetId == 2 && m_Page2 != 0) {
        SYS_WARNING ("m_Page2 already exists");
    } else if (widgetId == 3 && m_Page3 != 0) {
        SYS_WARNING ("m_Page3 already exists");
    } else if (widgetId == 4 && m_Page4 != 0) {
        SYS_WARNING ("m_Page4 already exists");
    } else if (widgetId == 5 && m_Page5 != 0) {
        SYS_WARNING ("m_Page5 already exists");
    }

    return pageMain (widgetId);
}

QString
TestApplet::title() const
{
    //% "Test"
    return qtTrId ("qtn_test_test"); // This is not official logical id
}

QVector<MAction*>
TestApplet::viewMenuItems()
{
    QVector<MAction*> vector;
    //% "User guide"
    MAction* helpAction = new MAction (qtTrId ("qtn_comm_userguide"), 
            pageMain (0));

    vector.append(helpAction);
    helpAction->setLocation(MAction::ApplicationMenuLocation);

    return vector;
}

DcpBrief *
TestApplet::constructBrief (
        int partId)
{
    Q_UNUSED (partId);
    return new TestBrief ();
}


