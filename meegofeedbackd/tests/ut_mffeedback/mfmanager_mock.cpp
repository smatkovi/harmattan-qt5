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

#include "mfmanager_mock.h"

#include <QDir>
#include <QDebug>

#include "mffeedbackhandle_mock.h"

// TODO: Take directory from header file made at configuration time?
static const char gLibDir[] = "/usr/lib/meegofeedbackd";

static MfManager *gInstance = NULL;

MfManager::MfManager(QObject *parent)
    : QObject(parent)
{
    if (gInstance) {
        qCritical("MfManager: Creating an instance but another one already exists");
        delete gInstance;
    }

    cachedHandle = new MfFeedbackHandle(this);
    cachedHandle->load("cached", 0);
    cachedHandle->attach();
    gInstance = this;
}

MfManager::~MfManager()
{
    cachedHandle->detach();
    if (cachedHandle.isNull() == false) {
        delete cachedHandle;
    }

    for (int i = 0; i < backendList.size(); ++i) {
        delete backendList[i];
        backendList[i] = NULL;
    }
    backendList.clear();

    gInstance = NULL;
}

MfManager *MfManager::instance()
{
    return gInstance;
}

bool MfManager::init(const QString &theme, const QList<QMap<QString, MfBackendBase::playbackVolume> > &volumes)
{
    Q_UNUSED(theme);
    Q_UNUSED(volumes);

    // Store the current theme name
    currentThemeName = theme;

    // Instantiate the dummy backend plugin
    // twice to act as if there were two actual
    // backends.
    QObject *backendInstance;

    backendInstance = new MfBackendFoo();
    backendList.append(qobject_cast<MfBackendBase*>(backendInstance));

    nonThreadedBackendCount = 1;

    backendInstance = new MfBackendFoo();
    backendList.append(qobject_cast<MfBackendBase*>(backendInstance));

    return true;
}

void MfManager::loadPlugins(const QString &libDir)
{
    Q_UNUSED(libDir);
}

QPointer<MfFeedbackHandle> MfManager::searchFromCache(const QString &feedbackDir, const int &backendInterfaceIndex)
{
    Q_UNUSED(backendInterfaceIndex);
    if (feedbackDir.at(feedbackDir.length()-1).isNumber() == false ||
        (feedbackDir.at(feedbackDir.length()-1).isNumber() == true &&
        feedbackDir.endsWith(QString::number(backendInterfaceIndex)) == true)) {
        if (feedbackDir.contains("cached") == true) {
            cacheHits++;
            return cachedHandle;
        }
    }

    return 0;
}

void MfManager::addToCache(MfFeedbackHandle* feedbackHandle)
{
    Q_UNUSED(feedbackHandle);
}

void MfManager::removeFromCache(MfFeedbackHandle* feedbackHandle)
{
    Q_UNUSED(feedbackHandle);
}

void MfManager::deleteCache()
{
}

void MfManager::loadTheme(const QString& themeName)
{
    Q_UNUSED(themeName);
}


