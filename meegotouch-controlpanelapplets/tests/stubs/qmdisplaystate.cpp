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

#include "qmdisplaystate.h"

#define DEBUG
#include "../../src/debug.h"

namespace MeeGo
{

QmDisplayState::QmDisplayState ()
{
    SYS_DEBUG ("Constructor: %p", this);
    m_State = On;
    m_DisplayBrightnessValue = 1;
    m_DisplayDimTimeout = 1;
    m_DisplayBlankTimeout = 1;
    m_BlankingWhenCharging = true;
}

QmDisplayState::~QmDisplayState ()
{
    SYS_DEBUG ("Destructor : %p", this);
}

bool
QmDisplayState::set (
        DisplayState state)
{
    SYS_DEBUG ("*** state = %d", state);
    m_State = state;
    return true;
}

int
QmDisplayState::getMaxDisplayBrightnessValue ()
{
    SYS_DEBUG ("*** returning 10");
    return 10;
}

/*!
 * The default brightnessvalue is 1, the smallest accepted value.
 */
int 
QmDisplayState::getDisplayBrightnessValue()
{
    SYS_DEBUG ("returning %d", m_DisplayBrightnessValue);
    return m_DisplayBrightnessValue;
}

void
QmDisplayState::setDisplayBrightnessValue(
        int brightness)
{
    SYS_DEBUG ("** brightness = %d", brightness);
    m_DisplayBrightnessValue = brightness;
}

int 
QmDisplayState::getDisplayDimTimeout()
{
    SYS_DEBUG ("returning %d", m_DisplayDimTimeout);
    return m_DisplayDimTimeout;
}

void
QmDisplayState::setDisplayDimTimeout(
        int timeout)
{
    SYS_DEBUG ("*** timeout = %d", timeout);
    m_DisplayDimTimeout = timeout;
}

void
QmDisplayState::setDisplayBlankTimeout(
        int timeout)
{
    SYS_DEBUG ("*** timeout = %d", timeout);
    m_DisplayBlankTimeout = timeout;
}

void
QmDisplayState::setBlankingWhenCharging(
        bool blanking)
{
    SYS_DEBUG ("*** blanking = %s", SYS_BOOL(blanking));
    m_BlankingWhenCharging = blanking;
}

bool
QmDisplayState::getBlankingWhenCharging()
{
    SYS_DEBUG ("returning %s", SYS_BOOL(m_BlankingWhenCharging));
    return m_BlankingWhenCharging;
}

} // namespace MeeGo


