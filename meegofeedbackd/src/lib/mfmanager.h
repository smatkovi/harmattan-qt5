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

#ifndef MFMANAGER_H
#define MFMANAGER_H

#include <QObject>
#include <QHash>
#include <QString>
#include <QPluginLoader>
#include <QPointer>
#include <QList>

#include "mfbackendbase.h"
#include "mfcommon.h"

class MfFeedback;
class MfFeedbackHandle;
class MfSourceBase;

class MfManager : public QObject
{
    Q_OBJECT
public:
    MfManager(QObject *parent = 0);
    ~MfManager();

    static MfManager *instance();

    QString currentTheme() {return currentThemeName;}

    bool init(const QString &theme,
              const QList<QMap<QString, MfBackendBase::playbackVolume> > &volumes,
              const QMap<QString, QString> &deviceState);

    virtual void loadBackendPlugins(const QString &libDir);
    virtual void loadSourcePlugins(const QString &libDir);

    QPointer<MfFeedbackHandle> searchFromCache(const QString &feedbackDir, const int &backendInterfaceIndex);
    void addToCache(MfFeedbackHandle* feedbackHandle);
    void removeFromCache(MfFeedbackHandle* feedbackHandle);

    #ifdef MF_DEBUG
    void printCache();
    #endif

signals:
    void themeChanged(const QString& themeName);
    void deviceStateChanged(const QMap<QString, QString>& newState);

public slots:
    void loadTheme(const QString& themeName);
    void setVolume(int volumeId, const QString& backendName, MfBackendBase::playbackVolume volume);
protected:
    void deleteCache();

protected slots:
    void backendConnected();
    void backendDisconnected();
protected:
    void loadHandlesForABackend(MfBackendBase* backend);
    void unloadHandlesForABackend(MfBackendBase* backend);

public:
    int nonThreadedBackendCount;
    QList<MfBackendBase*> backendList;
    QList<MfSourceBase*> sourceList;
    QList<QMap<QString, MfBackendBase::playbackVolume> > backendVolumes;

protected:
    QString currentThemeName;
    QHash<QString, QPointer<MfFeedbackHandle> > feedbackHandleHash;
    QList<QPluginLoader*> backendLoaderList;
    QList<QPluginLoader*> sourceLoaderList;
};

#endif
