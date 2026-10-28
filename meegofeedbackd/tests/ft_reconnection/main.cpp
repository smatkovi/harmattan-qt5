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

#include <QTest>
#include <QCoreApplication>

#include <mffeedback.h>
#include <mffeedbackhandle.h>
#include <mfsession.h>
#include <mfutil.h>

#include "mfconfig_mock.h"
#include "mfmanagertest.h"
#include "mfmanagerhandlestest.h"

void runSimpleReconnectionTest()
{
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    MfManagerTest* manager = new MfManagerTest();

    // Multiple reconnection attempts are tested here
    durationLimit = -1;

    // The manager inits the backends: vibra init is fine, audio fails.
    manager->init("base", emptyVolumeMapList, emptyDeviceStatusMap);

    qDebug() << "Test case 1a: Reconnection of the audio backend";

    QCoreApplication::exec();
    // Verify the reconnection trials of the audio backend
    if (MfBackendDummyAudio::initTrials != 3) {
        qFatal("Reconnection (audio backend) did not work: %d != %d", MfBackendDummyAudio::initTrials, 3);
    }
    // Verify the initialized state of the vibra backend
    if (MfBackendDummyVibra::initTrials != 1) {
        qFatal("Initialization (vibra backend) did not work: %d != %d", MfBackendDummyVibra::initTrials, 1);
    }

    qDebug() << "Test case 1b: Reconnection of the vibra backend";

    // Produce a lost connection
    manager->vibraBackend->play(NULL, MfBackendBase::VolumeHigh);
    QCoreApplication::exec();
    // Verify the same state of the audio backend
    if (MfBackendDummyAudio::initTrials != 3) {
        qFatal("The audio backend did not preserve its state: %d != %d", MfBackendDummyAudio::initTrials, 3);
    }
    // Verify the reconnection trials of the vibra backend
    if (MfBackendDummyVibra::initTrials != 3) {
        qFatal("Reconnection (vibra backend) did not work: %d != %d", MfBackendDummyVibra::initTrials, 3);
    }

    delete manager;
    manager = NULL;
}

void runReconnectionWithHandlesTest()
{
    MfManagerHandlesTest* manager = new MfManagerHandlesTest();
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    MfSession* session = NULL;

    // Reconnect on the first attempt immediately
    durationLimit = 0;

    manager->init("base", emptyVolumeMapList, emptyDeviceStatusMap);
    session = new MfSession("neverland_entrance");
    // Process events because the session setup
    // signals/slots with queued connection.
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    qDebug() << "Test case 2a: Audio and video handles are in place";

    if (!manager->checkHandleCache(manager->audioBackend)) {
        qFatal("Audio feedbacks are missing from the handle cache.");
    }
    if (!manager->checkHandleCache(manager->vibraBackend)) {
        qFatal("Vibra feedbacks are missing from the handle cache.");
    }

    qDebug() << "Test case 2b: Reloading audio feedbacks";

    manager->audioBackend->play(NULL, MfBackendBase::VolumeHigh);
    QTimer::singleShot(50, QCoreApplication::instance(), SLOT(quit()));
    QCoreApplication::instance()->exec();
    if (manager->checkHandleCache(manager->audioBackend)) {
        qFatal("Audio feedbacks are remained in the handle cache.");
    }
    QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
    QCoreApplication::instance()->exec();
    if (!manager->checkHandleCache(manager->audioBackend)) {
        qFatal("Audio feedbacks are missing from the handle cache.");
    }
    if (!manager->checkHandleCache(manager->vibraBackend)) {
        qFatal("Vibra feedbacks are missing from the handle cache.");
    }

    qDebug() << "Test case 2c: Reloading vibra feedbacks";

    manager->audioBackend->play(NULL, MfBackendBase::VolumeHigh);
    manager->vibraBackend->play(NULL, MfBackendBase::VolumeHigh);
    QTimer::singleShot(50, QCoreApplication::instance(), SLOT(quit()));
    QCoreApplication::instance()->exec();
    if (manager->checkHandleCache(manager->vibraBackend)) {
        qFatal("Vibra feedbacks are remained in the handle cache.");
    }
    QTimer::singleShot(1000, QCoreApplication::instance(), SLOT(quit()));
    QCoreApplication::instance()->exec();
    if (!manager->checkHandleCache(manager->audioBackend)) {
        qFatal("Audio feedbacks are missing from the handle cache.");
    }
    if (!manager->checkHandleCache(manager->vibraBackend)) {
        qFatal("Vibra feedbacks are missing from the handle cache.");
    }

    delete session;
    session = NULL;
    delete manager;
    manager = NULL;
}

int main(int argc, char **argv)
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    QCoreApplication fooApp(argc, argv);

    runSimpleReconnectionTest();
    runReconnectionWithHandlesTest();

    qDebug() << "Test successful without errors.";

    return 0;
}
