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

#ifndef MFFEEDBACK_H
#define MFFEEDBACK_H

#include <QObject>
#include <QList>
#include <QString>
#include <QPointer>
#include <QFuture>

class MfFeedbackHandle;
class MfManager;

class MfFeedback : public QObject {
    Q_OBJECT

public:
    MfFeedback(QObject *parent = 0);
    virtual ~MfFeedback();

    // Loads feedback handles. Multiple invocations will reload
    // the feedback handles. Feedback name is determined from the
    // first dir of feedback directories.
    void load(const QStringList &feedbackDirList);

    QString name() const;

    #ifdef MF_DEBUG
    void printData() const;
    #endif

    const QList<QPointer<MfFeedbackHandle> > &fbHandles();

    void emitPlay(qint64 timestamp);

signals:
    void startPlay(qint64 timestamp);

private slots:
    void play(qint64 timestamp);

protected:
    void unload();
    MfFeedbackHandle* loadHandle(const QString &feedbackDir, const int backendInterfaceIndex);

    static MfManager *manager;
    QString feedbackName;
    int feedbackVolumePriority;

    // The size of feedbackHandles must always match the size of
    // loaded backend interfaces.
    QList<QPointer<MfFeedbackHandle> > feedbackHandles;

    QList<QFuture<void> > concurrentPlayers;
};

#endif
