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

#include "mffeedbackhandle_mock.h"

#include <QDir>
#include <QtDebug>

#include "mfmanager_mock.h"

QStringList MfFeedbackHandle::playedPaths;

MfFeedbackHandle::MfFeedbackHandle(QObject *parent)
    : QObject(parent), feedbackDirPath(""), refCnt(0), currentState(Empty)
{
}

MfFeedbackHandle::~MfFeedbackHandle()
{
}

QString MfFeedbackHandle::path() const
{
    return feedbackDirPath;
}

void MfFeedbackHandle::load(const QString& path, MfBackendBase* backendInterface)
{
    if (path == "cached") {
        currentState = Ready;
        feedbackDirPath = path;
        return;
    }

    if (backendInterface == 0) {
        qFatal("Feedback handle loaded with null backend interface!");
    }

    currentState = Empty;
    if (path.at(path.length()-1).isNumber() == false ||
        (path.at(path.length()-1).isNumber() == true &&
        path.endsWith(backendInterface->name()) == true)) {
        if (path.contains("ready")) {
            currentState = Ready;
        } else if (path.contains("ignored")) {
            currentState = Ignored;
        }
    }
    feedbackDirPath = path;
}

void MfFeedbackHandle::unload()
{
}

bool MfFeedbackHandle::operator== (const MfFeedbackHandle &other) const
{
    Q_UNUSED(other);

    return false;
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
    Q_UNUSED(volume)
    playedPaths << feedbackDirPath;
}

MfFeedbackHandle::State MfFeedbackHandle::state() const
{
    return currentState;
}

int MfFeedbackHandle::refCount() const
{
    return refCnt;
}

MfBackendInterface* MfFeedbackHandle::backendInterface() const
{
    return 0;
}

