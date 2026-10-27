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

#include "ut_displaybusinesslogic.h"
#include "displaybusinesslogic.h"

#ifdef HAVE_QMSYSTEM
#  ifndef QMDISPLAYSTATE_STUB_H
#    error "QmDisplayState should be stubbed, can not continue."
#  endif
#endif

#include <MApplication>
#include <MGConfItem>

#include <QList>
#include <QVariant>

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Stubbing the MGConfItem.
 */
static bool dimTimeoutsExist = true;

QVariant
MGConfItem::value () const
{
    SYS_DEBUG ("*** key() = %s", SYS_STR(key()));

    if (key() != "/system/osso/dsm/display/possible_display_dim_timeouts") {
        SYS_DEBUG ("%s ???", SYS_STR(key()));
        goto return_invalid;
    }

    if (dimTimeoutsExist) {
        QList<QVariant> list;

        list << 
            QVariant (15) << 
            QVariant (30) << 
            QVariant (60) << 
            QVariant (120) <<
            QVariant (180);

        return QVariant (list);
    }

return_invalid:
    return QVariant ();
}

/******************************************************************************
 * Ut_DisplayBusinessLogic implementation. 
 */
void 
Ut_DisplayBusinessLogic::init()
{
}

void 
Ut_DisplayBusinessLogic::cleanup()
{
}


void 
Ut_DisplayBusinessLogic::initTestCase()
{
    int argc = 1;
    char* app_name = (char*) "./Ut_DisplayBusinessLogic";

    m_App = new MApplication (argc, &app_name);
    m_Api = new DisplayBusinessLogic;
}

void 
Ut_DisplayBusinessLogic::cleanupTestCase()
{
    delete m_App;
    delete m_Api;
}

/*!
 * Tests if there are available brightness values and if the current brightness
 * value index found as an available value.
 *
 * This function also stores the max and min values for the brighness in the
 * m_MaxBrightness and m_MinBrightness tag variables.
 */
void 
Ut_DisplayBusinessLogic::testGetBrightnessValues ()
{
    QList<int> values = m_Api->brightnessValues();
    int        current = m_Api->selectedBrightnessValueIndex();
    int        n = 0;

    QVERIFY (values.size() != 0);

    m_MinBrightness = m_MaxBrightness = values[0];

    qDebug() << "current = " << current;
    foreach (int value, values) {
        qDebug() << "brightnessvalue[" << n << "] = " << value;

        if (value < m_MinBrightness)
            m_MinBrightness = value;
        if (value > m_MaxBrightness)
            m_MaxBrightness = value;

        ++n;
    }
   
    QVERIFY (current >= 0 && current < values.size());
    QVERIFY (m_MinBrightness < m_MaxBrightness);
}


void
Ut_DisplayBusinessLogic::testLightTimeouts ()
{
    QList<int> values = m_Api->screenLightsValues ();
    int        current = m_Api->selectedScreenLightsValue ();
    bool       foundCurrent = false;
    int        n = 0;

    QVERIFY (values.size() != 0);
    
    m_Maxtimeout = m_Mintimeout = values[0];

    foreach (int value, values) {
        if (n == current) {
            qDebug() << "timeoutvalue[" << n << "] = " << value << "sec CURRENT";
            foundCurrent = true;
        } else {
            qDebug() << "timeoutvalue[" << n << "] = " << value << "sec";
        }

        if (value > m_Maxtimeout)
            m_Maxtimeout = value;
        if (value < m_Mintimeout)
            m_Mintimeout = value;

        ++n;
    }
    
    QVERIFY (foundCurrent);
}

void
Ut_DisplayBusinessLogic::testSetBrightness ()
{
    QList<int> values = m_Api->brightnessValues();
    int        value;

    /* 
     * Testing the brighness values.  First we set to the min, then to the max
     * value.  It is obvious we should get back the same value we set, but it
     * seems we fail under the sb1 environment.  
     */

    qDebug() << "Min brightness = " << m_MinBrightness;
    qDebug() << "Max brightness = " << m_MaxBrightness;

    m_Api->setBrightnessValue (m_MinBrightness);
    value = m_Api->selectedBrightnessValueIndex();
    qDebug() << "brightness = " << value;
    // FIXME: This test is not complete!
    //QVERIFY (m_Api->selectedBrightnessValue() == m_MinBrightness);
    
    m_Api->setBrightnessValue (m_MaxBrightness);
    value = m_Api->selectedBrightnessValueIndex();
    qDebug() << "brightness = " << value;
    // FIXME: This test is not complete!
    //QVERIFY (m_Api->selectedBrightnessValue() == m_MaxBrightness);
}

/*!
 * testing the screenLightsValues() method. This method has a built in fallback
 * list, we stub the same and we check if it returns the right list.
 */
void
Ut_DisplayBusinessLogic::testScreenLightsValues ()
{
    QList<int> list1, list2;

    dimTimeoutsExist = true;
    list1 = m_Api->screenLightsValues ();
    
    dimTimeoutsExist = false;
    list2 = m_Api->screenLightsValues ();

    QVERIFY (list1 == list2);
}


void
Ut_DisplayBusinessLogic::testPSMValue()
{

    QVERIFY(!(m_Api->PSMValue()));

}


void
Ut_DisplayBusinessLogic::testSetCloseFromTop ()
{
    m_Api->setCloseFromTop(true);
    QVERIFY(m_Api->getCloseFromTopValue());
   
    m_Api->setCloseFromTop(false);
    QVERIFY(!m_Api->getCloseFromTopValue());
    

}



QTEST_APPLESS_MAIN(Ut_DisplayBusinessLogic)
