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

#include <MGConfItem>
#include "ut_batterybusinesslogic.h"

#define DEBUG
#include "../debug.h"

/*
 * Include the stubs here:
 */
#include "qmbattery.h"
#include "qmdevicemode.h"
#ifndef QMDEVICEMODE_STUBBED_H
#  error "QmDeviceMode is not stubbed, can't continue."
#endif
#ifndef QMBATTERY_STUBBED_H
#  error "QmBattery is not stubbed, can't continue."
#endif

// XXX: This must be the same as in batterybusinesslogi.cpp
#define SAVE_BATTERY

#ifndef SAVE_BATTERY
static const int animation_rate_charging_usb  = 500;
static const int animation_rate_charging_wall = 250;
#else
static const int animation_rate_charging_usb  = 1000;
static const int animation_rate_charging_wall = 1000;
#endif


using namespace MeeGo;

/******************************************************************************
 * SignalSink implementation. 
 */
SignalSink::SignalSink () :
    QObject ()
{
    reset ();
}


void
SignalSink::reset ()
{
    m_PSMValue = false;
    m_PSMValueReceived = false;
    m_RemainingCapacity= -1;
    m_RemainingCapacityReceived = false;
    m_AnimationRate = -1;
    m_AnimationRateReceived = false;
    m_BarValue = -1;
    m_BarValueReceived = false;
}

void
SignalSink::print ()
{
    SYS_DEBUG ("*** m_PSMValueReceived            = %s", SYS_BOOL (m_PSMValueReceived));
    if (m_PSMValueReceived) {
        SYS_DEBUG ("*** m_PSMValue                    = %s", SYS_BOOL (m_PSMValue));
    }

    SYS_DEBUG ("*** m_RemainingCapacityReceived = %s",
            SYS_BOOL(m_RemainingCapacityReceived));
    if (m_RemainingCapacityReceived) {
            SYS_DEBUG ("*** m_RemainingCapacity     = %d", m_RemainingCapacity);

    }

    SYS_DEBUG ("*** m_AnimationRateReceived       = %s", 
            SYS_BOOL(m_AnimationRateReceived));
    if (m_AnimationRateReceived) {
        SYS_DEBUG ("*** m_AnimationRate               = %d", m_AnimationRate);
    }

    SYS_DEBUG ("*** m_BarValueReceived            = %s", SYS_BOOL(m_BarValueReceived));
    if (m_BarValueReceived) {
        SYS_DEBUG ("*** m_BarValue                    = %d", m_BarValue);
    }
}

bool 
SignalSink::chargingWithAnimation (
        int animationRate)
{
    return m_AnimationRateReceived && m_AnimationRate == animationRate;
}

bool 
SignalSink::notCharging ()
{
    return m_AnimationRateReceived && m_AnimationRate == 0;
}


bool 
SignalSink::hasBarValue (
        int barValue)
{
    return m_BarValueReceived && m_BarValue == barValue;
}

/*!
 * \param charging If the sink should have information sho
 */
//bool
//SignalSink::hasRemainingTimes (
//        bool   charging)
//{
//    if (charging)
//        return m_RemainingTimeValuesReceived &&
//            m_RemainingTimeValues.size() == 2 &&
//            m_RemainingTimeValues[0] == "qtn_ener_charging" &&
//            m_RemainingTimeValues[1] == "qtn_ener_charging";

//    return m_RemainingTimeValuesReceived &&
//        m_RemainingTimeValues.size() == 2 &&
//        m_RemainingTimeValues[0] != "qtn_ener_charging" &&
//        m_RemainingTimeValues[1] != "qtn_ener_charging";
//}

bool
SignalSink::hasRemainingCapacity (
        bool   charging)
{
    Q_UNUSED (charging);

    if (m_RemainingCapacity != -1)
        return true;
    else
        return false;
}

void
SignalSink::remainingCapacityChanged(
        int value)
{
    SYS_DEBUG ("*** value = %d", value);
    m_RemainingCapacity = value;
}

void
SignalSink::batteryCharging (
        int animationRate)
{
    SYS_DEBUG ("*** animationRate = %d", animationRate);
    m_AnimationRate = animationRate;
    m_AnimationRateReceived = true;
}

void 
SignalSink::batteryBarValueReceived (
        int barValue)
{
    SYS_DEBUG ("*** barValue = %d", barValue);
    m_BarValue = barValue;
    m_BarValueReceived = true;
}

void
SignalSink::PSMValueReceived (
        bool PSMValue)
{
    SYS_DEBUG ("*** PSMValue = %s", SYS_BOOL(PSMValue));
    m_PSMValue = PSMValue;
    m_PSMValueReceived = true;
}

/******************************************************************************
 * Stubbing MGConfItem class.
 */

// The GConf key and value for the PSMAuto settings (I guess this states if the
// device whould or should not go automatically into the PSM mode.
static const char *psmAutoKey =
    "/system/osso/dsm/energymanagement/enable_power_saving";
static bool psmAutoValue = false;


static QString lastGConfKey;
QVariant GconfItemSet;

enum _testCase {
    CASE_BOOL,
    CASE_STRINGLIST
} testCase;

void
MGConfItem::set (
        const QVariant &val)
{
    SYS_DEBUG ("*** key = %s", SYS_STR(key()));
    lastGConfKey = key();

    if (key() == psmAutoKey) {
        psmAutoValue = val.toBool();
    }

    GconfItemSet = val;
}

QVariant 
MGConfItem::value () const
{
    SYS_DEBUG ("*** key = %s", SYS_STR(key()));
    lastGConfKey = key();
    
    if (key() == psmAutoKey) {
        return QVariant (psmAutoValue);
    }

    switch (testCase) {
        case CASE_BOOL:
            return QVariant (true);
            break;
        case CASE_STRINGLIST:
        {
            QStringList list;
            list << "10" << "20" << "30";
            return QVariant (list);
            break;
        }
    }

    return QVariant ();
}

/******************************************************************************
 * Ut_BatteryBusinessLogic implements the unit test.
 */

/*
 * These are run before and after every test case
 */
void Ut_BatteryBusinessLogic::init ()
{
    bool connectSuccess;
   
    SYS_DEBUG ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    SYS_DEBUG ("+++ Initializing test +++++++++++++++++++++++++++++++++++++");
    SYS_DEBUG ("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    /*
     * Creating the new businesslogic.
     */
    m_Logic = new BatteryBusinessLogic (this);

    /*
     * Conencting to its signals.
     */
    connectSuccess = connect (
            m_Logic, SIGNAL (remainingBatteryCapacityChanged(int)),
            &m_SignalSink, SLOT (remainingCapacityChanged (int)));
    QVERIFY (connectSuccess);
    
    connectSuccess = connect (
            m_Logic, SIGNAL (batteryCharging (int)),
            &m_SignalSink, SLOT (batteryCharging (int)));
    QVERIFY (connectSuccess);
    
    connectSuccess = connect (
            m_Logic, SIGNAL (batteryBarValueReceived(int)),
            &m_SignalSink, SLOT (batteryBarValueReceived(int)));
    QVERIFY (connectSuccess);
    
    connectSuccess = connect (
            m_Logic, SIGNAL (PSMValueReceived(bool)),
            &m_SignalSink, SLOT (PSMValueReceived(bool)));
    QVERIFY (connectSuccess);

    /*
     * Now that we connected the signals we can initialize the whole
     * businesslogic.
     */
    QVERIFY (!m_Logic->m_initialized);
    m_Logic->requestValues();
    QVERIFY (m_Logic->m_initialized);
    // One more time to check if it causes some problems...
    m_Logic->requestValues();
    QVERIFY (m_Logic->m_initialized);
}

void Ut_BatteryBusinessLogic::cleanup ()
{
    delete m_Logic;
}

/*
 * These are run once [start / finish]
 */
void Ut_BatteryBusinessLogic::initTestCase ()
{
}

void
Ut_BatteryBusinessLogic::cleanupTestCase ()
{
}

/*!
 * Reads and sets the PSMAuto value in the simulated GConf database.
 */
void 
Ut_BatteryBusinessLogic::testPSMAutoValue ()
{
    bool value;
    BatteryBusinessLogic::PowerSaveOpt newValue;

    value = m_Logic->PSMAutoValue ();
    QCOMPARE (value, psmAutoValue);
    QVERIFY  (lastGConfKey == psmAutoKey);

    if (value)
    {
        newValue = BatteryBusinessLogic::PSMAutoOff;} else {
        newValue = BatteryBusinessLogic::PSMAutoAutomatic;
    }
    
    m_Logic->setPSMOption(newValue);
    QCOMPARE (psmAutoValue, !value);
    QVERIFY  (lastGConfKey == psmAutoKey);
}

/*!
 * Turns on and off the power save mode then checks if the businesslogic set the
 * state in the QmDeviceMode stub and sent a signal about the change.
 */
void 
Ut_BatteryBusinessLogic::testPSMValue ()
{
    #ifdef HAVE_QMSYSTEM
    bool newValue;
    SYS_DEBUG ("");
    
    newValue = true;
    m_SignalSink.reset();
    m_Logic->setPSMOption(BatteryBusinessLogic::PSMAutoOn);
    QVERIFY (m_Logic->m_devicemode->getPSMState() == QmDeviceMode::PSMStateOn);
    QVERIFY (m_SignalSink.m_PSMValueReceived);
    QVERIFY (m_SignalSink.m_PSMValue == newValue);

    newValue = false;
    m_SignalSink.reset();
    m_Logic->setPSMOption(BatteryBusinessLogic::PSMAutoOff);
    QVERIFY (m_Logic->m_devicemode->getPSMState() == QmDeviceMode::PSMStateOff);
    QVERIFY (m_SignalSink.m_PSMValueReceived);
    QVERIFY (m_SignalSink.m_PSMValue == newValue);
    #endif
}

/*!
 * This test will check that when the device changes the power save mode
 * spontaneously (so that the change is not initiated by the
 * BatteryBusinessLogic), the businesslogic will sense it and will send the
 * proper signal.
 */
void 
Ut_BatteryBusinessLogic::testSpontaneousPSMValue ()
{
    /*
     * So we know we start from this state.
     */
    #ifdef HAVE_QMSYSTEM
    m_Logic->m_devicemode->setPSMState (QmDeviceMode::PSMStateOff);
    
    m_SignalSink.reset();
    m_Logic->m_devicemode->setPSMState (QmDeviceMode::PSMStateOn);
    QVERIFY (m_SignalSink.m_PSMValueReceived);
    QVERIFY (m_SignalSink.m_PSMValue == true);
    
    m_SignalSink.reset();
    m_Logic->m_devicemode->setPSMState (QmDeviceMode::PSMStateOff);
    QVERIFY (m_SignalSink.m_PSMValueReceived);
    QVERIFY (m_SignalSink.m_PSMValue == false);
    #endif
}

/*!
 * Testing connecting/disconnecting chargers. Please note that this test will
 * not modify the setChargingState, it will remain QmBattery::StateCharging, and
 * we actually test if our businesslogic can tolerate this kind of behaviour.
 */
void 
Ut_BatteryBusinessLogic::testSpontaneousChargerEvent ()
{
    #ifdef HAVE_QMSYSTEM
    // Just to be sure we are in a known state.
    m_Logic->m_battery->connectCharger (QmBattery::None);
    m_Logic->m_battery->modifyBatteryState (QmBattery::StateOK, 50);
    m_Logic->m_battery->setChargingState (QmBattery::StateCharging);


    SYS_DEBUG ("**********************************************************");
    SYS_DEBUG ("*** Connecting 500mA charger                           ***");
    SYS_DEBUG ("**********************************************************");
    m_SignalSink.reset();
    m_Logic->m_battery->connectCharger (QmBattery::USB_500mA);

    m_SignalSink.print();
    QVERIFY (m_SignalSink.chargingWithAnimation(animation_rate_charging_usb));
    QVERIFY (m_SignalSink.hasBarValue(4));
    QVERIFY (m_SignalSink.hasRemainingCapacity(true));
    

    SYS_DEBUG ("**********************************************************");
    SYS_DEBUG ("*** Disconnecting charger                              ***");
    SYS_DEBUG ("**********************************************************");
    m_SignalSink.reset();
    m_Logic->m_battery->connectCharger (QmBattery::None);

    m_SignalSink.print();
    QVERIFY (m_SignalSink.notCharging());
    QVERIFY (m_SignalSink.hasRemainingCapacity(false));
    

    SYS_DEBUG ("**********************************************************");
    SYS_DEBUG ("*** Connecting wall charger                            ***");
    SYS_DEBUG ("**********************************************************");
    m_SignalSink.reset();
    m_Logic->m_battery->connectCharger (QmBattery::Wall);

    m_SignalSink.print();
    QVERIFY (m_SignalSink.chargingWithAnimation(animation_rate_charging_wall));
    QVERIFY (m_SignalSink.hasBarValue(4));
    QVERIFY (m_SignalSink.hasRemainingCapacity(true));
    
    
    SYS_DEBUG ("**********************************************************");
    SYS_DEBUG ("*** Disconnecting charger                              ***");
    SYS_DEBUG ("**********************************************************");
    m_SignalSink.reset();
    m_Logic->m_battery->connectCharger (QmBattery::None);

    m_SignalSink.print();
    QVERIFY (m_SignalSink.notCharging());
    QVERIFY (m_SignalSink.hasRemainingCapacity(false));
    #endif
}

/*!
 * This test will check what happens when the charger is connected and the
 * battery gets fully charged, then it looses the fully charged state.
 * Plase note that the chargingstate will not change. The test will check if the
 * businesslogic can tolerate such a behaviour.
 */
void 
Ut_BatteryBusinessLogic::testSpontaneousChargingComplete ()
{
    #ifdef HAVE_QMSYSTEM
    // Just to be sure we are in a known state. We are charging...
    m_Logic->m_battery->modifyBatteryState (QmBattery::StateOK, 50);
    m_Logic->m_battery->connectCharger (QmBattery::Wall);
    m_Logic->m_battery->setChargingState (QmBattery::StateCharging);

    /*
     * Checking NB#172929 -  Battery applet in Settings->Device system->Battery 
     * should stop animating once battery is full
     * We alse expect to report the remaining times as charging, since the
     * charger is connected.
     */
    SYS_DEBUG ("**********************************************************");
    SYS_DEBUG ("*** The battery got charged.                           ***");
    SYS_DEBUG ("**********************************************************");
    m_SignalSink.reset();
    m_Logic->m_battery->modifyBatteryState (QmBattery::StateFull, 98);
    
    m_SignalSink.print();
    QVERIFY (m_SignalSink.notCharging());
    QVERIFY (m_SignalSink.hasRemainingCapacity(true));
    QVERIFY (m_SignalSink.hasBarValue(8));

    SYS_DEBUG ("**********************************************************");
    SYS_DEBUG ("*** The battery is loosing charge.                     ***");
    SYS_DEBUG ("**********************************************************");
    m_SignalSink.reset();
    m_Logic->m_battery->modifyBatteryState (QmBattery::StateOK, 4);
    
    m_SignalSink.print();
    QVERIFY (m_SignalSink.chargingWithAnimation(animation_rate_charging_wall));
    QVERIFY (m_SignalSink.hasBarValue(0));
    QVERIFY (m_SignalSink.hasRemainingCapacity(true));
    
    SYS_DEBUG ("**********************************************************");
    SYS_DEBUG ("*** The charging is stopped.                           ***");
    SYS_DEBUG ("**********************************************************");
    m_SignalSink.reset();
    m_Logic->m_battery->modifyBatteryState (QmBattery::StateFull, 96);
    m_Logic->m_battery->setChargingState (QmBattery::StateNotCharging);
    m_Logic->m_battery->connectCharger (QmBattery::None);
    
    m_SignalSink.print();
    QVERIFY (m_SignalSink.notCharging());
    QVERIFY (m_SignalSink.hasBarValue(8));
    QVERIFY (m_SignalSink.hasRemainingCapacity(false));
    #endif
}

/*!
 * Simply test the PSMThresholdValue setting and retrieving.
 */
void 
Ut_BatteryBusinessLogic::testSetPSMThreshold ()
{
    #ifdef HAVE_QMSYSTEM
    m_Logic->setPSMThresholdValue (10);
    QVERIFY (m_Logic->m_devicemode->getPSMBatteryMode() == 10);

    QVERIFY (m_Logic->PSMThresholdValue() == 10);
    #endif
}

QTEST_MAIN (Ut_BatteryBusinessLogic);
