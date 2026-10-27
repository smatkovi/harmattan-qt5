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

#include "qmdevicemode.h"

#define DEBUG
#define WARNING
#include "../../src/debug.h"

using namespace MeeGo;

QmDeviceMode::QmDeviceMode (
        QObject *parent) :
    QObject (parent),
    m_PSMSTate (PSMStateOff), 
    m_PSMBatteryMode (-1)
{
    SYS_DEBUG ("Creating %p", this);
}

QmDeviceMode::~QmDeviceMode ()
{
    SYS_DEBUG ("Destroying %p", this);
}


QmDeviceMode::PSMState
QmDeviceMode::getPSMState () const
{
    SYS_DEBUG ("returning %s", SYS_STR(PSMStateName(m_PSMSTate)));
    return m_PSMSTate;
}

bool
QmDeviceMode::setPSMState (
        PSMState state)
{
    SYS_DEBUG ("Setting state %s -> %s",
            SYS_STR(PSMStateName(m_PSMSTate)),
            SYS_STR(PSMStateName(state)));

    if (m_PSMSTate == state)
        return false;

    m_PSMSTate = state;

    SYS_DEBUG ("Emitting devicePSMStateChanged(%s)", 
            SYS_STR(PSMStateName(state)));
    emit devicePSMStateChanged (state);

    return true;
}

int
QmDeviceMode::getPSMBatteryMode()
{
    return m_PSMBatteryMode;
}

bool
QmDeviceMode::setPSMBatteryMode (
        int percentages)
{
    m_PSMBatteryMode = percentages;
    return true;
}

/******************************************************************************
 * Low level functions to help the testing. These are not in the original!
 */
QString
QmDeviceMode::PSMStateName (
        PSMState state) const
{
    switch (state) {
        case PSMError:
            return "QmDeviceMode::PSMError";
        case PSMStateOff:
            return "QmDeviceMode::PSMStateOff";
        case PSMStateOn:
            return "QmDeviceMode::PSMStateOn";
    }

    return "NOSUCHASTATE";
}
