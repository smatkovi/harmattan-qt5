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
#ifndef BATTERYBUSINESSLOGIC_H
#define BATTERYBUSINESSLOGIC_H

#include <QObject>
#include <QStringList>

#ifdef HAVE_QMSYSTEM
#  include <qmdevicemode.h>
#  include <qmbattery.h>
#endif

/*!
 * This class implements the backend connection the battery applet needs to
 * sense the battery status, the charging state an the power save state. The
 * class uses the QmSystem as a backend when available.
 *
 * Please note that currently no alternative, the class will not work without
 * the QmSystem library.
 */
class BatteryBusinessLogic : public QObject
{
    Q_OBJECT
    Q_ENUMS (Condition);

public:
    BatteryBusinessLogic (QObject *parent = 0);
    ~BatteryBusinessLogic ();
    typedef enum {
        PSMAutoOff=0,
        PSMAutoOn,
        PSMAutoAutomatic
    } PowerSaveOpt;

    typedef enum {
        BGood,
        BPoor,
        BUnknown
    } Condition;
        
    QStringList PSMThresholdValues ();
    int PSMThresholdValue ();
    bool PSMAutoValue ();
    bool PSMValue ();
    bool isCharging ();
    bool isBatteryFull ();
    Condition getCondition ();
    unsigned int getBatteryLevel();
    int remainingChargingTime ();
    int remainingTalkTime ();
    int remainingIdleTime ();

public slots:
    void setPSMThresholdValue (int percentage);
    void setPSMOption (PowerSaveOpt);
    void requestValues ();
    void remainingCapacityRequired ();

signals:
    void remainingBatteryCapacityChanged (int value);
    // animation_rate is 0 when not charging...
    void batteryCharging (int animation_rate);
    void batteryBarValueReceived (int bar_value);
    void PSMValueReceived (bool enabled);
    void batteryFull ();
    void updateUIonPowerSaveModeChange (PowerSaveOpt);

#ifdef HAVE_QMSYSTEM
private slots:
    /*
     * These slots are implemented to connect to the signals of the QmSystem. If
     * the QmSystem library is not available we don't use them.
     */
    void batteryChargerEvent (MeeGo::QmBattery::ChargerType type);
    void chargingStateChanged (MeeGo::QmBattery::ChargingState state);
    void batteryStateChanged (MeeGo::QmBattery::BatteryState batteryState);
    void batteryRemCapacityChanged (int percentage, int bars);
    void PSMStateChanged (MeeGo::QmDeviceMode::PSMState state);
#endif

private:
    int batteryBarValue (int percentage);
    void recalculateChargingInfo ();

    bool              m_initialized;
    #ifdef HAVE_QMSYSTEM 
    MeeGo::QmBattery    *m_battery;
    MeeGo::QmDeviceMode *m_devicemode;
    #endif

    // If the charger is connected, the battery is not full and the charging is
    // not failed this integer shows the speed of the charging animation.
    int               m_ChargingRate;

    // If the charger is connected. Might be not really charging if the battery
    // is full.
    bool              m_Charging;

    // Cache this information as frequently used,
    // and updated by a QmSystem2 signal
    bool              m_PowerSaveMode;
    
#ifdef UNIT_TEST
    friend class Ut_BatteryBusinessLogic;
#endif
};

#endif
