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

#include "mfbackendbase.h"

#include <QtConcurrentRun>
#include <QDebug>

#include "mfconfig.h"

const QString MfBackendBase::volumeHighID = "high";
const QString MfBackendBase::volumeMediumID = "medium";
const QString MfBackendBase::volumeLowID = "low";
const QString MfBackendBase::volumeOffID = "off";

bool initWrapper(MfBackendBase* backend, int durationLimit);

bool initWrapper(MfBackendBase* backend, int durationLimit)
{
    return backend->init(durationLimit);
}

MfBackendBase::MfBackendBase() :
    failedConnections(0), currentState(Disconnected)
{
    // Fill the intervals for connection
    connectionIntervals << 500;
    connectionIntervals << 1000;
    connectionIntervals << 5000;
    connectionIntervals << 10000;
    connectionIntervals << 30000;
    connectionIntervals << 120000;

    // Handle the lost connection
    connect(this, SIGNAL(lostConnection()), this, SLOT(tryConnect()));
    // Set up a timer
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(connecting()));
    // Listen to the future watcher
    connect(&resultConnection, SIGNAL(finished()), this, SLOT(connectionDone()));
}

MfBackendBase::~MfBackendBase()
{
}

MfBackendBase::State MfBackendBase::state() const
{
    return currentState;
}

bool MfBackendBase::loadHandleWrapper(const QDir &feedbackDir, void **feedbackHandle)
{
    if (currentState != Connected)
    {
        qCritical("MfBackendBase: Unable to load handles in the current state.");
        return false;
    }
    if (feedbackHandle == NULL) {
        qCritical("MfBackendBase: NULL feedbackHandle given to loadHandle.");
        return false;
    }

    (*feedbackHandle) = NULL;

    return loadHandle(feedbackDir, feedbackHandle);
}

void MfBackendBase::unloadHandleWrapper(void **feedbackHandle)
{
    if (feedbackHandle == NULL ||
        (*feedbackHandle) == NULL) {
        return;
    }
    unloadHandle(feedbackHandle);
}

void MfBackendBase::playWrapper(void *feedbackHandle, playbackVolume volume)
{
    if (!feedbackHandle || volume == VolumeOff || currentState != Connected)
        return;

    play(feedbackHandle, volume);
}

void MfBackendBase::tryConnect()
{
    int interval;

    // Change the state to connecting
    currentState = Connecting;
    failedConnections++;

     if (failedConnections < connectionIntervals.size()) {
         interval = connectionIntervals[failedConnections];
     } else {
         interval = connectionIntervals.last();
     }
     if (!timer.isActive()) {
         timer.start(interval);
     }
}

void MfBackendBase::connectionDone()
{
    if (!resultConnection.result()) {
        qDebug() << "Unsuccessful connection: " << name();
        tryConnect();
        return;
    }
    qDebug() << "Successful connection: " << name();

    currentState = Connected;
    failedConnections = 0;
    emit establishedConnection();
}

void MfBackendBase::cleanup()
{
    if (resultConnection.isRunning()) {
        resultConnection.waitForFinished();
    }
    if (timer.isActive()) {
        timer.stop();
    }
    disconnect();
}

void MfBackendBase::connecting()
{
    qDebug() << "Schedule a connection attempt of " << name() << "...";
    runConnection = QtConcurrent::run(initWrapper, this, MfConfig::feedbackDurationLimit());
    resultConnection.setFuture(runConnection);
}
