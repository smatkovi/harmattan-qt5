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
#include "warrantybusinesslogic.h"

#include <QString>
#include <QSettings>
#include <QVariant>

#ifdef HAVE_QMSYSTEM
#  include <qmsystemstate.h>
#endif

static const QString configPath ("/usr/share/about-contents/");
static const QString configFile (configPath + "warranty.ini");

#define DEBUG
#define WARNING
#include "../debug.h"

WarrantyBusinessLogic::WarrantyBusinessLogic() 
{
}

WarrantyBusinessLogic::~WarrantyBusinessLogic()
{
}

int
WarrantyBusinessLogic::getExpirationDays ()
{
    int retVal = -1;

#ifdef HAVE_QMSYSTEM
    QSettings warrantyConf (configFile, QSettings::IniFormat);
    int warrantyDays = warrantyConf.value ("days", QVariant (365)).toInt ();

    MeeGo::QmSystemState  systemState;
    int elapsedDays = systemState.getPowerOnTimeInSeconds () / (60 * 60 * 24);

    retVal = warrantyDays - elapsedDays;

    SYS_DEBUG ("warrantyDays = %d, elapsedDays = %d", warrantyDays, elapsedDays);

    if (retVal < 0)
        retVal = 0;
#endif

    return retVal;
}

