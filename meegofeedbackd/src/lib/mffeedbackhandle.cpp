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

#include "mffeedbackhandle.h"

#include <QDir>
#include <QtDebug>

#include "mfconfig.h"
#include "mfmanager.h"

MfFeedbackHandle::MfFeedbackHandle(QObject *parent)
    : QObject(parent), backendIf(0), handle(0), feedbackDirPath(""), refCnt(0), currentState(Empty)
{
}

MfFeedbackHandle::~MfFeedbackHandle()
{
    if (currentState == Ready) {
        unload();
    }
}

QString MfFeedbackHandle::path() const
{
    return feedbackDirPath;
}

void MfFeedbackHandle::load(const QString& path, MfBackendBase* backend)
{
    QDir feedbackDir(path);

    if (backend == 0) {
        qDebug() << "Failed to load feedback. Provided backend was NULL.";
        return;
    }

    if (!feedbackDir.exists()) {
        qDebug() << "Failed to load feedback. Directory does not exist:"
                 << feedbackDir.path();
        return;
    }

    // Unload pre-existing handle
    if (currentState == Ready) {
        unload();
    }

    if (backend->loadHandleWrapper(path, &handle) == false) {
        qDebug() << "Failed to load feedback. "
                 << "Backend" << backend->name()
                 << "was not able to load content from directory:"
                 << feedbackDir.path();
        return;
    }

    feedbackDirPath = path;
    this->backendIf = backend;
    if (handle != 0) {
        currentState = Ready;
    } else {
        // When loading succeeds but handle is NULL,
        // the feedback is ignored on purpose.
        currentState = Ignored;
    }
    return;
}

void MfFeedbackHandle::unload()
{
    if (backendIf != 0) {
        backendIf->unloadHandleWrapper(&handle);
    } else {
        qCritical() << "Failed to unload feedback. Backend is missing!";
    }

    // We do not delete the reference to the backend because
    // it can be reloaded.
    handle = 0;
    feedbackDirPath = "";
    currentState = Empty;
}

bool MfFeedbackHandle::operator== (const MfFeedbackHandle &other) const
{
    return (path() == other.path() &&
            backend() == other.backend());
}

void MfFeedbackHandle::attach()
{
    refCnt++;
}

void MfFeedbackHandle::detach()
{
    refCnt--;

    if (refCnt <= 0) {
        delete this;
    }
}


void MfFeedbackHandle::play(MfBackendBase::playbackVolume volume)
{
    if (backendIf != 0 && backendIf->state() == MfBackendBase::Connected
        && handle != 0 && volume != MfBackendBase::VolumeOff) {
        backendIf->playWrapper(handle, volume);
    }
}

MfFeedbackHandle::State MfFeedbackHandle::state() const
{
    return currentState;
}

int MfFeedbackHandle::refCount() const
{
    return refCnt;
}

MfBackendBase* MfFeedbackHandle::backend() const
{
    return backendIf;
}

