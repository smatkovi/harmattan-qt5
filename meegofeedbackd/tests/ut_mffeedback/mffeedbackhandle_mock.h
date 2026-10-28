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

#ifndef MFFEEDBACKHANDLE_H
#define MFFEEDBACKHANDLE_H

#include "mfbackendbase.h"

#include <QObject>
#include <QStringList>

class MfFeedbackHandle : public QObject {
    Q_OBJECT

public:
    MfFeedbackHandle(QObject *parent);
    virtual ~MfFeedbackHandle();

    void load(const QString& path, MfBackendBase* backendInterface);

    QString path() const;

    bool operator== (const MfFeedbackHandle &other) const;

    void attach();
    void detach();

    enum State {
        Empty,      // Initial state
        Ready,      // Handle loaded and ready to play
        Ignored,    // Handle ignored on purpose
    };
    State state() const;

    int refCount() const;
    MfBackendInterface* backendInterface() const;

    static QStringList playedPaths;

public slots:
    void play(MfBackendBase::playbackVolume volume);
private:
    void unload();

    QString feedbackDirPath;
    int refCnt;
    State currentState;
};

#endif
