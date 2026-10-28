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

#ifndef MFCONFIGURATION_P_H
#define MFCONFIGURATION_P_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <mfbackendbase.h>

#include <gconf/gconf-client.h>

#define MFCONFIGURATION_DIR "/meegotouch"

class MfConfigurationPrivate : public QObject
{
    Q_OBJECT
public:
    ~MfConfigurationPrivate();

    void init();
    bool handleGError(GError **error);
    void emitThemeChanged(QString newTheme);
    void emitVolumeChanged(QString keyPath, QString newVolume);

    static void stringListFromGSList(QStringList &qlist, GSList *glist);

    QString getTheme();
    QList<QMap<QString, MfBackendBase::playbackVolume> > getVolumes();
    QMap<QString, MfBackendBase::playbackVolume> getVolumesOfDir(const char* dirStr);
    MfBackendBase::playbackVolume toVolume(const QString &volume, bool *ok = 0);

    GConfClient *gconfClient;
signals:
    void themeChanged(QString newTheme);
    void volumeChanged(int volumeId, QString backendName, MfBackendBase::playbackVolume newVolume);
};

#endif
