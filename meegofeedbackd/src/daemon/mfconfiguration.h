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

#ifndef MFCONFIGURATION_H
#define MFCONFIGURAIION_H

#include <QObject>
#include <QMap>
#include <QStringList>
#include <mfbackendbase.h>

class MfConfigurationPrivate;

class MfConfiguration : public QObject
{
    Q_OBJECT

public:
    MfConfiguration();
    virtual ~MfConfiguration();

    bool init();

    QString theme();
    QList<QMap<QString, MfBackendBase::playbackVolume> > volumes();

signals:
    void themeChanged(QString newTheme);
    void volumeChanged(int volumeId, const QString& backendName, MfBackendBase::playbackVolume newVolume);

private:
    MfConfigurationPrivate *d;
};

#endif
