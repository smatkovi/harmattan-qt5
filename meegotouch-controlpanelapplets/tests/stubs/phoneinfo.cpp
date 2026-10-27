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

#include "phoneinfo.h"

#include <QString>

#define DEBUG
#define WARNING
#include "../../src/debug.h"

using namespace Cellular;

PhoneInfo::PhoneInfo ()
{
    SYS_DEBUG ("Creating %p", this);
}

PhoneInfo::~PhoneInfo ()
{
    SYS_DEBUG ("Destroying %p", this);
}

QString 
PhoneInfo::imeiNumber() const
{
    return FAKE_IMEI_NUMBER;
}

