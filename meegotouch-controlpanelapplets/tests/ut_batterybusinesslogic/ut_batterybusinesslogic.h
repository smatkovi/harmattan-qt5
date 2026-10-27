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
#ifndef UT_BATTERYBUSINESSLOGIC_H
#define UT_BATTERYBUSINESSLOGIC_H

#include <QtTest/QtTest>
#include <QObject>
#include <QStringList>

#include <batterybusinesslogic.h>
class SignalSink : public QObject 
{
    Q_OBJECT
public:
    SignalSink ();
    
    void reset ();
    void print ();
    bool chargingWithAnimation (int animationRate);
    bool notCharging ();
    bool hasBarValue (int barValue);
//    bool hasRemainingTimes (bool charging);
    bool hasRemainingCapacity(bool charging);
    
public slots:
    void remainingCapacityChanged(int value);
    void batteryCharging (int animationRate);
    void batteryBarValueReceived (int barValue);
    void PSMValueReceived (bool PSMValue);

public:
    bool        m_PSMValue;
    bool        m_PSMValueReceived;
    int         m_RemainingCapacity;
    bool        m_RemainingCapacityReceived;
    int         m_AnimationRate;
    bool        m_AnimationRateReceived;
    int         m_BarValue;
    bool        m_BarValueReceived;
};

class Ut_BatteryBusinessLogic : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void testPSMAutoValue ();
    void testPSMValue ();
    void testSpontaneousPSMValue ();
    void testSpontaneousChargerEvent ();
    void testSpontaneousChargingComplete ();
    void testSetPSMThreshold ();

private:
    BatteryBusinessLogic    *m_Logic;
    SignalSink               m_SignalSink;
};

#endif
