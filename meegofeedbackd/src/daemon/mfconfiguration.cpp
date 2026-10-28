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

#include "mfconfiguration.h"
#include "mfconfiguration_p.h"

MfConfiguration::MfConfiguration()
    : d(new MfConfigurationPrivate())
{
    d->gconfClient = NULL;
}

MfConfiguration::~MfConfiguration()
{
    if (d) {
        delete d;
        d = NULL;
    }
}

bool MfConfiguration::init()
{
    bool ok;

    // Forward signals from private class
    ok = connect(d, SIGNAL(themeChanged(QString)), SIGNAL(themeChanged(QString)));
    if (!ok) {
        qCritical("MfConfiguration: Unable to connect internal signals");
        return false;
    }

    ok = connect(d, SIGNAL(volumeChanged(int, QString,  MfBackendBase::playbackVolume)),
                 SIGNAL(volumeChanged(int, QString,  MfBackendBase::playbackVolume)));
    if (!ok) {
        qCritical("MfConfiguration: Unable to connect internal signals");
        return false;
    }

    d->init();

    return true;
}

QString MfConfiguration::theme()
{
    return d->getTheme();
}

QList<QMap<QString, MfBackendBase::playbackVolume> > MfConfiguration::volumes()
{
    return d->getVolumes();
}
