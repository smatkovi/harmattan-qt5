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

#include "qmusbmode.h"

#define DEBUG
#include "../../src/debug.h"

namespace MeeGo
{

QmUSBMode::QmUSBMode (
        QObject *parent) :
    QObject (parent),
    m_Mode (Connected),
    m_DefaultMode (Connected)
{
    SYS_DEBUG ("Constructing %p", this);
}

QmUSBMode::~QmUSBMode ()
{
    SYS_DEBUG ("Destructing %p", this);
}

QmUSBMode::Mode
QmUSBMode::getMode()
{
    return m_Mode;
}

QmUSBMode::Mode
QmUSBMode::getDefaultMode ()
{
    return m_DefaultMode;
}

bool
QmUSBMode::setMode (
        Mode mode)
{
    m_Mode = mode;
    return true;
}


bool
QmUSBMode::setDefaultMode (
        Mode mode)
{
    m_DefaultMode = mode;
    return true;
}

}; // namespace MeeGo

