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
#include "batterybusinesslogic.h"
#include <MGConfItem>
#include <QVariant>
#include <QString>

#define DEBUG
#define WARNING
#include "../debug.h"

static const QString psm_auto_key =
    "/system/osso/dsm/energymanagement/enable_power_saving";
static const QString psm_values_key =
    "/system/osso/dsm/energymanagement/possible_psm_thresholds";

#define SAVE_BATTERY

#ifndef SAVE_BATTERY
static const int animation_rate_charging_usb  = 500;
static const int animation_rate_charging_wall = 250;
#else
static const int animation_rate_charging_usb  = 1000;
static const int animation_rate_charging_wall = 1000;
#endif

BatteryBusinessLogic::BatteryBusinessLogic (
        QObject *parent)
    : QObject (parent),
    m_initialized (false),
    m_battery (0),
    m_devicemode (0),
    m_ChargingRate (-1),
    m_Charging (false),
    m_PowerSaveMode (false)
{
    DEBUG_CLOCK_START;
#ifdef HAVE_QMSYSTEM
    m_battery    = new MeeGo::QmBattery (this);
    m_devicemode = new MeeGo::QmDeviceMode (this);
#endif
    DEBUG_CLOCK_END("Battery businesslogic creation");
}

BatteryBusinessLogic::~BatteryBusinessLogic ()
{
}

/*
 * This function does the initialization and signal connection to QmBattery and
 * QmDeviceMode, and emits all the signals with the current values...
 *
 * FIXME: This function should be called realize() or something...
 */
void
BatteryBusinessLogic::requestValues ()
{
    DEBUG_CLOCK_START;
    if (m_initialized)
        return;

    m_initialized = true;

    #ifdef HAVE_QMSYSTEM
    SYS_DEBUG ("Connecting to the signals of the QmBattery class");
    connect (
        m_battery, SIGNAL(chargerEvent(MeeGo::QmBattery::ChargerType)),
        this, SLOT(batteryChargerEvent(MeeGo::QmBattery::ChargerType)));
    connect (
        m_battery, SIGNAL(chargingStateChanged(MeeGo::QmBattery::ChargingState)),
        this, SLOT(chargingStateChanged(MeeGo::QmBattery::ChargingState)));
    connect (
        m_battery, SIGNAL(batteryStateChanged(MeeGo::QmBattery::BatteryState)),
        this, SLOT (batteryStateChanged(MeeGo::QmBattery::BatteryState)));

    /*
     * We have two signals showing that the battery energy level has been
     * changed. We listen both of these signals.
     */
    SYS_DEBUG ("Connecting to the signals of the QmBattery class");
    connect (m_battery, SIGNAL (batteryRemainingCapacityChanged (int, int)),
            this, SLOT (batteryRemCapacityChanged (int, int)));


    // batteryBarValueReceived also emitted by chargingStateChanged ^^^
    // FIXME: Why?

    /*
     * Init the power-save-mode value, and connect to QmSystem2 signal
     */
    m_PowerSaveMode = (m_devicemode->getPSMState () ==
                       MeeGo::QmDeviceMode::PSMStateOn);
    connect (m_devicemode,
             SIGNAL (devicePSMStateChanged (MeeGo::QmDeviceMode::PSMState)),
             this, SLOT (PSMStateChanged (MeeGo::QmDeviceMode::PSMState)));
    #else
    /*
     * FIXME: To create an implementation without the QmSystem
     */
    #endif

    recalculateChargingInfo ();
    DEBUG_CLOCK_END("Data request");
}

void
BatteryBusinessLogic::setPSMThresholdValue (
        int percentage)
{
    #ifdef HAVE_QMSYSTEM
    bool ret;
    ret = m_devicemode->setPSMBatteryMode (percentage);

    if (!ret) {
        SYS_WARNING (" failed to set (precentage = %d)", percentage);
    }
    #else
    /*
     * To implement the code that sets the power save threshold without the help
     * of the QmSystem library.
     */
    #endif
}

int
BatteryBusinessLogic::PSMThresholdValue ()
{
    #ifdef HAVE_QMSYSTEM
    return m_devicemode->getPSMBatteryMode ();
    #else
    /*
     * FIXME: To implement a variant that does not use QmSystem
     */
    return 0;
    #endif
}

QStringList
BatteryBusinessLogic::PSMThresholdValues ()
{
    MGConfItem  possible_values (psm_values_key);
    QStringList retval;

    retval = possible_values.value ().toStringList ();
    if (retval.size() == 0) {
        SYS_WARNING ("The GConf key %s is not set. Falling back to default.",
                SYS_STR(psm_values_key));
        retval << "10" << "20" << "30" << "40" << "50";
    }

    return retval;
}

bool
BatteryBusinessLogic::PSMValue ()
{
    return m_PowerSaveMode;
}

/*!
 * Sets used  power saving option, available are: set power saving off/on
 * or use a threshold to enable setting saving mode automatically 
 */
void
BatteryBusinessLogic::setPSMOption (PowerSaveOpt saveOption)
{
    bool ret = false;
    MGConfItem PSMAutoKey (psm_auto_key);
    
    if (saveOption == PSMAutoAutomatic)
    {
        PSMAutoKey.set (true);
        ret = m_devicemode->setPSMState (MeeGo::QmDeviceMode::PSMStateOff);
    } else {
    #ifdef HAVE_QMSYSTEM
        ret = m_devicemode->setPSMState (
            saveOption == PSMAutoOn ?
            MeeGo::QmDeviceMode::PSMStateOn :
            MeeGo::QmDeviceMode::PSMStateOff);
        PSMAutoKey.set (false);

        if (ret)
        {
            m_PowerSaveMode = (saveOption == PSMAutoOn);
        }
    #else
    /*
     * FIXME: To implement the setting of the power save mode without the help
     * of the QmSystem library.
     */
    #endif
    }

    if (ret) {
        // Change succeed, we don't need to wait for QmSystem reply, we can emit
        // the PSMValueChanged asap to update the UI.
        emit updateUIonPowerSaveModeChange (saveOption);
    } else {
        SYS_WARNING ("Failed to set PSM mode to %s",
                saveOption == PSMAutoOn ? 
                "QmDeviceMode::PSMStateOn" : "QmDeviceMode::PSMStateOff");
    }
}

/*!
 * Reads the PSMAuto value, a boolean that sets if the device should go into
 * power save mode automatically. This method is only reads the GConf database.
 */
bool
BatteryBusinessLogic::PSMAutoValue ()
{
    MGConfItem PSMAutoKey (psm_auto_key);
    SYS_DEBUG ("value = %s", SYS_BOOL (PSMAutoKey.value ().toBool ()));
    return PSMAutoKey.value ().toBool ();
}


void
BatteryBusinessLogic::remainingCapacityRequired()
{
    #ifdef HAVE_QMSYSTEM
    emit remainingBatteryCapacityChanged(
            m_battery->getRemainingCapacityPct());
    #else
    /*
     * FIXME: To create an implementation that works without the QmSystem.
     */
    #endif
}

#ifdef HAVE_QMSYSTEM
/*!
 * We have three functions her that notify us about the changes in the charging
 * state. We use all of the three slots the same way, but we keep them
 * separately so we can actually see why do we need to recalculate the charging
 * info.
 */
void
BatteryBusinessLogic::batteryChargerEvent (
        MeeGo::QmBattery::ChargerType type)
{
    Q_UNUSED (type);

    SYS_DEBUG ("");
    recalculateChargingInfo ();
}
#endif

#ifdef HAVE_QMSYSTEM
void
BatteryBusinessLogic::chargingStateChanged (
        MeeGo::QmBattery::ChargingState state)
{
    Q_UNUSED (state);
    
    SYS_DEBUG ("");
    recalculateChargingInfo ();
}
#endif

#ifdef HAVE_QMSYSTEM
void 
BatteryBusinessLogic::batteryStateChanged (
        MeeGo::QmBattery::BatteryState batteryState)
{
    Q_UNUSED (batteryState);
    
    SYS_DEBUG ("");
    recalculateChargingInfo ();
}
#endif

#ifdef HAVE_QMSYSTEM
/*!
 * This slot will be called when the device power save mode is changed. The
 * method will send the PSMValueReceived() signal.
 */
void
BatteryBusinessLogic::PSMStateChanged (
        MeeGo::QmDeviceMode::PSMState state)
{
    m_PowerSaveMode =
        (state == MeeGo::QmDeviceMode::PSMStateOn);
    
    SYS_DEBUG ("Emitting PSMValueReceived (%s)", SYS_BOOL (m_PowerSaveMode));
    emit PSMValueReceived (m_PowerSaveMode);
}
#endif

#ifdef HAVE_QMSYSTEM
void
BatteryBusinessLogic::batteryRemCapacityChanged (
		int percentage, 
		int bars)
{
    Q_UNUSED (bars);
    
    SYS_DEBUG ("Emitting batteryBarValueReceived(%d)",
            batteryBarValue (percentage));
    emit batteryBarValueReceived (batteryBarValue (percentage));

    /* Fixes: NB#265208 */
    if (isBatteryFull())
        emit batteryFull ();
    else
        emit remainingBatteryCapacityChanged (percentage);
}
#endif

int
BatteryBusinessLogic::remainingTalkTime ()
{
    int ret = -1;

    /*
     * When device is charging we're return -1 from here...
     */
    if (isCharging ())
        return ret;

#ifdef HAVE_QMSYSTEM
    MeeGo::QmBattery::RemainingTimeMode mode =
        PSMValue () ? MeeGo::QmBattery::PowersaveMode
                    : MeeGo::QmBattery::NormalMode;

    ret = m_battery->getRemainingTalkTime (mode);
#endif

    return ret;
}

int
BatteryBusinessLogic::remainingIdleTime ()
{
    int ret = -1;

    /*
     * When device is charging we're return -1 from here...
     */
    if (isCharging ())
        return ret;

#ifdef HAVE_QMSYSTEM
    MeeGo::QmBattery::RemainingTimeMode mode =
        PSMValue () ? MeeGo::QmBattery::PowersaveMode
                    : MeeGo::QmBattery::NormalMode;

    ret = m_battery->getRemainingIdleTime (mode);
#endif

    return ret;
}

int
BatteryBusinessLogic::remainingChargingTime ()
{
    #ifdef HAVE_QMSYSTEM
    return m_battery->getRemainingChargingTime ();
    #else
    // FIXME: Then we don't know?
    return -1;
    #endif
}

/*!
 * \param percentage The energy level percentage or -1 to ask the backend.
 *
 * Returns the bar value (the value that used as an icon index representing for
 * the battery energy level) for the given percentage level. If the argument is
 * -1 this method will ask the QmBattery for the current energy level.
 *
 */
int
BatteryBusinessLogic::batteryBarValue (
        int percentage)
{
    int index;

    /*
     * If we got -1 we have to find the remaining capacity all by ourselves.
     */
    #ifdef HAVE_QMSYSTEM
    if (percentage == -1) {
        percentage = m_battery->getRemainingCapacityPct ();
    }
    #else
    /*
     * FIXME: To create an implementation that works without the QmSystem
     */
    #endif

    // in case of overflow (eg. in sbox) when we get value that greater than 100
    // percent we use 10 percent intentionaly
    if (percentage < 0)
        percentage = 0;
    else if (percentage > 100)
        percentage = 10;

    if (percentage >= 84)
        index = 8;
    else if (percentage < 84 && percentage >= 73)
        index = 7;
    else if (percentage < 73 && percentage >= 62)
        index = 6;
    else if (percentage < 62 && percentage >= 51)
        index = 5;
    else if (percentage < 51 && percentage >= 39)
        index = 4;
    else if (percentage < 39 && percentage >= 28)
        index = 3;
    else if (percentage < 28 && percentage >= 17)
        index = 2;
    else if (percentage < 17 && percentage >= 5)
        index = 1;
    else // if (percentage < 0)
        index = 0;

    return index;
}

/*!
 * This function is called whenever some change is detected around the battery
 * and its charger. This method will try to find out if the battery is actually
 * charging and if it is the method will calculate the charging animation speed.
 *
 * What we managed to find aboput the charging is:
 * 1) The battery is not charging (despite any other info returned) if the
 *    battery is full. See NB#172929 for further details.
 */
void
BatteryBusinessLogic::recalculateChargingInfo ()
{
    #ifdef HAVE_QMSYSTEM
    MeeGo::QmBattery::ChargingState chargingState;
    MeeGo::QmBattery::BatteryState  batteryState;
    MeeGo::QmBattery::ChargerType   chargerType;
    bool                            charging;
    bool                            couldBeCharging;
    int                             chargingRate;
   
    chargerType = m_battery->getChargerType ();
    chargingState = m_battery->getChargingState ();
    batteryState = m_battery->getBatteryState ();

    /*
     * Carefully calculating the charging rate, the animation rate of the
     * charging indicator.
     */
    couldBeCharging =
        chargingState == MeeGo::QmBattery::StateCharging &&
        // This is actually not necessary, but we even check it in the unit
        // test. Just to be sure.
        chargerType != MeeGo::QmBattery::None;

    charging = 
        batteryState != MeeGo::QmBattery::StateFull &&
        chargerType != MeeGo::QmBattery::None &&
        chargingState != MeeGo::QmBattery::StateNotCharging &&
        chargingState != MeeGo::QmBattery::StateChargingFailed;

    chargingRate = 0;
    if (charging) 
        chargingRate = chargerType == MeeGo::QmBattery::Wall ?
            animation_rate_charging_wall : animation_rate_charging_usb;

    SYS_DEBUG ("*** charging       = %s", SYS_BOOL(charging));
    SYS_DEBUG ("*** m_Charging     = %s", SYS_BOOL(m_Charging));
    SYS_DEBUG ("*** chargingRate   = %d", chargingRate);
    SYS_DEBUG ("*** m_ChargingRate = %d", m_ChargingRate);


    /*
     * If the charging rate has been changed we need to notify the ui with a
     * signal.
     */
    m_Charging = couldBeCharging;
    SYS_DEBUG ("*** chargingRate %d -> %d", m_ChargingRate, chargingRate);
    m_ChargingRate = chargingRate;       
    SYS_DEBUG ("Emitting batteryCharging(%d)", m_ChargingRate);
    emit batteryCharging (m_ChargingRate);

    /*
     * Then we need to notify everyone about the bar value. 
     * FIXME: Why exactly do we need that?
     */
    SYS_DEBUG ("Emitting batteryBarValueReceived(%d)", batteryBarValue (-1));
    emit batteryBarValueReceived (batteryBarValue (-1));

    if (batteryState == MeeGo::QmBattery::StateFull)
    {
        m_Charging = false;
        SYS_WARNING ("battery full");
        emit batteryFull ();
        return;
    }
    else
    {
        /*
         * And the remaining battery capacity has to be recalculated.
         */
        remainingCapacityRequired();
    }
    #else
    /*
     * FIXME: To implement a variant that does not use QmSystem.
     */
    #endif
}

bool
BatteryBusinessLogic::isCharging()
{
    return m_Charging;
}

bool 
BatteryBusinessLogic::isBatteryFull ()
{
    #ifdef HAVE_QMSYSTEM
    return m_battery->getBatteryState () == MeeGo::QmBattery::StateFull;
    #else
    return false;
    #endif
}

BatteryBusinessLogic::Condition
BatteryBusinessLogic::getCondition ()
{
    Condition ret = BUnknown;

#ifdef HAVE_QMSYSTEM
    switch (m_battery->getBatteryCondition ())
    {
        case MeeGo::QmBattery::ConditionGood:
            ret = BGood;
            break;
        case MeeGo::QmBattery::ConditionPoor:
            ret = BPoor;
            break;
        default:
            break;
    }
#endif

    return ret;
}

unsigned int
BatteryBusinessLogic::getBatteryLevel()
{
    unsigned int  bateryLevelPct = 0;
#ifdef HAVE_QMSYSTEM
    bateryLevelPct = m_battery->getRemainingCapacityPct();
#endif
    return  bateryLevelPct;
}


