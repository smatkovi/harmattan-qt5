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

#include "mfbackendfoo.h"

MfBackendFoo::MfBackendFoo() : initCalled(0), limit(0), hardware(true), deviceStateChangedCalled(0)
{
}

MfBackendFoo::MfBackendFoo(bool hw) : initCalled(0), limit(0), hardware(hw), deviceStateChangedCalled(0)
{
}

MfBackendFoo::~MfBackendFoo()
{
}

QString MfBackendFoo::name() const
{
    return "foo";
}

bool MfBackendFoo::detectHardware() const
{
    return hardware;
}

bool MfBackendFoo::init(int durationLimit)
{
    initCalled++;;
    limit = durationLimit;

    return true;
}

bool MfBackendFoo::loadHandle(const QDir &feedbackDir, void **feedbackHandle)
{
    Q_UNUSED(feedbackDir);
    Q_UNUSED(feedbackHandle);
    return true;
}

void MfBackendFoo::unloadHandle(void **feedbackHandle)
{
    Q_UNUSED(feedbackHandle);
}

void MfBackendFoo::play(void *feedbackHandle, playbackVolume volume)
{
    Q_UNUSED(feedbackHandle);
    Q_UNUSED(volume);
}

bool MfBackendFoo::isReady()
{
    return true;
}

void MfBackendFoo::deviceStateChanged(const QMap<QString, QString> &newState)
{
    Q_UNUSED(newState);
    deviceStateChangedCalled++;
}

