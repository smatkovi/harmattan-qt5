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

#include "mfbackenddummy.h"

#include <QtDebug>

MfBackendDummy::MfBackendDummy()
{
}

MfBackendDummy::~MfBackendDummy()
{
    qDebug("MfBackendDummy: Being destroyed");
}

QString MfBackendDummy::name() const
{
    return QString("dummy");
}

bool MfBackendDummy::detectHardware() const
{
    return true;
}

bool MfBackendDummy::init(int durationLimit)
{
    Q_UNUSED(durationLimit)
    return true;
}

void MfBackendDummy::play(void *feedbackHandle, playbackVolume volume)
{
    Q_UNUSED(volume)
    if (feedbackHandle != 0) {
        qDebug() << "MfBackendDummy: playing " << *((QString*) feedbackHandle);
    }
}

bool MfBackendDummy::loadHandle(const QDir &feedbackDir, void **feedbackHandle)
{
    if (feedbackHandle == 0) {
        qDebug() << "MfBackendDummy: NULL feedbackHandle given to loadHandle";
        return false;
    }

    qDebug() << "MfBackendDummy: Loading feedback" << feedbackDir.dirName();
    (*feedbackHandle) = (void*)(new QString(feedbackDir.dirName()));

    return true;
}

void MfBackendDummy::unloadHandle(void **feedbackHandle)
{
    if (feedbackHandle == 0 ||
        (*feedbackHandle) == 0) {
        return;
    }

    QString *str = (QString *) (*feedbackHandle);

    qDebug() << "MfBackendDummy: Unloading feedback" << *str;

    delete str;
    str = 0;
}

bool MfBackendDummy::isReady()
{
    return true;
}

void MfBackendDummy::deviceStateChanged(const QMap<QString, QString> &newState)
{
    Q_UNUSED(newState);
}

Q_EXPORT_PLUGIN2(meegofeedback-dummy, MfBackendDummy);
