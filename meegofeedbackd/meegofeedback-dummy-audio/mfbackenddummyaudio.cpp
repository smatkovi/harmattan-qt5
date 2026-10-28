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

#include "mfbackenddummyaudio.h"

#include <QStringList>
#include <QtDebug>
#include <QCoreApplication>

#ifdef MF_RECONNECTION_TESTING
int MfBackendDummyAudio::initTrials = 0;
#endif

MfBackendDummyAudio::MfBackendDummyAudio(): MfBackendBase(),
    volume(VolumeMedium)
{
    currentState = Connected;
}

MfBackendDummyAudio::~MfBackendDummyAudio()
{
    qDebug("MfBackendDummyAudio: Being destroyed");
}

QString MfBackendDummyAudio::name() const
{
    return QString("dummy_audio");
}

bool MfBackendDummyAudio::detectHardware() const
{
    return true;
}

bool MfBackendDummyAudio::init(int durationLimit)
{
    Q_UNUSED(durationLimit)
#ifdef MF_RECONNECTION_TESTING
    // In this case it is a test for the handles
    if (durationLimit == 0)
    {
        return true;
    }
    // Otherwise simple reconnection test
    initTrials++;
    if (initTrials < 3) {
        printf("Reconnection trials (audio backend)\n");
        return false;
    }
    if (initTrials == 3) {
        QCoreApplication::instance()->quit();
        return false;
    }
#endif
    return true;
}

void MfBackendDummyAudio::play(void *feedbackHandle, playbackVolume volume)
{
    Q_UNUSED(volume)
    if (feedbackHandle == NULL) {
        currentState = Disconnected;
        emit lostConnection();
    } else {
        #ifndef MF_THEME_TESTING
        qDebug() << "MfBackendDummyAudio: playing " << *((QString*) feedbackHandle);
        #else
        playList << *((QString*) feedbackHandle);
        volumeList << volume;
        #endif
    }
}

bool MfBackendDummyAudio::loadHandle(const QDir &feedbackDir, void **feedbackHandle)
{
    Q_UNUSED(feedbackDir)

    if (feedbackHandle == 0) {
        qCritical("MfBackendDummyAudio: NULL feedbackHandle given to loadHandle.");
        return false;
    }

#ifdef MF_RECONNECTION_TESTING
    (*feedbackHandle) = malloc(1);
    return true;
#else
    (*feedbackHandle) = 0;

    // Check existence of file
    if (!feedbackDir.exists("audio.wav")) {
        // We cannot load what does not exist.
        #ifndef MF_THEME_TESTING
        qDebug() << "MfBackendDummyAudio: Failed to load" << feedbackDir.absolutePath() + "/audio.wav";
        #endif
        return false;
    } else {
        QFileInfo fileInfo(feedbackDir.filePath("audio.wav"));

        // If file size is 0 bytes, return true but set
        // feedbackHandle as 0. This means tha the
        // feedback is ignored in purpose.
        if (fileInfo.size() == 0) {
            return true;
        }
    }

    #ifndef MF_THEME_TESTING
    qDebug() << "MfBackendDummyAudio: Loading feedback" << (feedbackDir.absolutePath() + "/audio.wav");
    #endif
    (*feedbackHandle) = (void*)(new QString(feedbackDir.absolutePath()));
    return true;
#endif
}

void MfBackendDummyAudio::unloadHandle(void **feedbackHandle)
{
    if (feedbackHandle == 0 ||
        (*feedbackHandle) == 0) {
        return;
    }

#ifdef MF_RECONNECTION_TESTING
    free((*feedbackHandle));
    *feedbackHandle = NULL;
    return;
#else

    QString *str = (QString *) (*feedbackHandle);

    #ifndef MF_THEME_TESTING
    qDebug() << "MfBackendDummyAudio: Unloading feedback" << *str;
    #endif

    delete str;
    str = 0;
#endif
}

bool MfBackendDummyAudio::isReady()
{
    return true;
}

void MfBackendDummyAudio::deviceStateChanged(const QMap<QString, QString> &newState)
{
    Q_UNUSED(newState);
}

#if !defined(MF_THEME_TESTING) && !defined(MF_RECONNECTION_TESTING)
Q_EXPORT_PLUGIN2(meegofeedback-dummy-audio, MfBackendDummyAudio);
#endif
