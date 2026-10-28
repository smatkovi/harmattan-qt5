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

#include "mfconfig.h"

#include <QDebug>
#include <QSettings>

qint64 MfConfig::_feedbackLatencyLimit = -1;
int MfConfig::_feedbackDurationLimit = -1;
bool MfConfig::_loaded = false;

qint64 MfConfig::feedbackLatencyLimit()
{
    if (!_loaded) {
        load();
    }

    return _feedbackLatencyLimit;
}

int MfConfig::feedbackDurationLimit()
{
    if (!_loaded) {
        load();
    }

    return _feedbackDurationLimit;
}

void MfConfig::load()
{
    QSettings settings("/etc/meegofeedbackd/daemon.conf", QSettings::NativeFormat);

    _loaded = true;
    _feedbackLatencyLimit = (qint64)settings.value("feedback-latency-limit", QVariant(2000)).toInt();
    _feedbackDurationLimit = settings.value("feedback-duration-limit", QVariant(1000)).toInt();
    qDebug() << "Loaded feedback latency limit:" << _feedbackLatencyLimit;
    qDebug() << "Loaded feedback duration limit:" << _feedbackDurationLimit;
}
