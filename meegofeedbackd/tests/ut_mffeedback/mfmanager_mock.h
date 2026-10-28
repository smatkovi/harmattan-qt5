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

#include <mfbackendinterface.h>
#include <mfbackendbase.h>
#include <mfsourceinterface.h>
#include "mfbackendfoo.h"

#include <QObject>
#include <QHash>
#include <QString>
#include <QPluginLoader>
#include <QPointer>
#include <QList>
#include "mfcommon.h"

class MfFeedback;
class MfFeedbackHandle;

class MfManager : public QObject
{
    Q_OBJECT
public:
    MfManager(QObject *parent = 0);
    ~MfManager();

    static MfManager *instance();

    QString currentTheme() { return QString("base"); }

    bool init(const QString &theme, const QList<QMap<QString, MfBackendBase::playbackVolume> > &volumes);

    virtual void loadPlugins(const QString &libDir);

    QPointer<MfFeedbackHandle> searchFromCache(const QString &feedbackDir, const int &backendInterfaceIndex);
    void addToCache(MfFeedbackHandle* feedbackHandle);
    void removeFromCache(MfFeedbackHandle* feedbackHandle);

    int cacheHits;
    QPointer<MfFeedbackHandle> cachedHandle;

signals:
    void themeChanged(const QString& themeName);

public slots:
    void loadTheme(const QString& themeName);

public:
    int nonThreadedBackendCount;
    QList<MfBackendBase*> backendList;
    QList<MfSourceInterface*> sourceList;
    QList<QMap<QString, MfBackendBase::playbackVolume> > backendVolumes;

protected:
    void deleteCache();

    QString currentThemeName;
};

#endif
