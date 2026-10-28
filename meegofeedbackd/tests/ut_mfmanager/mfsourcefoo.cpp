/* This file is part of meegofeedbackd
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mfsourcefoo.h"
#include <QDebug>

MfSourceFoo::MfSourceFoo()
{
    initCalled = 0;
    deviceStateChangedCalled = 0;
}

MfSourceFoo::~MfSourceFoo()
{
}

bool MfSourceFoo::init()
{
    initCalled++;
    return true;
}

void MfSourceFoo::deviceStateChanged(const QMap<QString, QString> &newState)
{
    Q_UNUSED(newState);
    deviceStateChangedCalled++;
}

