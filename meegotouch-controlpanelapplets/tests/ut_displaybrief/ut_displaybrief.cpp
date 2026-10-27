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

#include "ut_displaybrief.h"
#include "displayapplet.h"
#include "displaywidget.h"
#include "brightnessbrief.h"


#include <dcpwidgettypes.h>

#include <MApplication>
#include <MComboBox>
#include <MAction>
#include <QVector>


#define DEBUG
#include "../../src/debug.h"

/*
 * Ut_DisplayBrief implementation. 
 */
void 
Ut_DisplayBrief::init()
{
}

void 
Ut_DisplayBrief::cleanup()
{
}

static int argc = 1;
static char *app_name = (char*) "./ut_displayapplet";

void 
Ut_DisplayBrief::initTestCase()
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
Ut_DisplayBrief::cleanupTestCase()
{
    delete brief1;
    delete m_Applet;
    m_App->deleteLater ();
}


 void
Ut_DisplayBrief::testConstructBrief()
{
    brief1 = (BrightnessBrief *) m_Applet->constructBrief(1);
    QVERIFY (brief1);
    QVERIFY (brief1->widgetTypeID() == DcpWidgetType::Slider);
   
}


 void
Ut_DisplayBrief::testValue()
{
    QVERIFY( brief1->value() == 2 );
	
}
 void	
Ut_DisplayBrief::testMinValue()
{
    QVERIFY( brief1->minValue() == 0);
}

 void
Ut_DisplayBrief::testMaxValue()
{
    QVERIFY( brief1->maxValue() == 4);
}

 void
Ut_DisplayBrief::testSetValue()
{

  brief1->setValue(3);
  QVERIFY(brief1->value() == 3);

}

QTEST_APPLESS_MAIN(Ut_DisplayBrief)

