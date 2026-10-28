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

#ifndef MFMANAGERTEST_H
#define MFMANAGERTEST_H

#include <mfmanager.h>
#include <mfbackendbase.h>

#include "../../meegofeedback-dummy-audio/mfbackenddummyaudio.h"
#include "../../meegofeedback-dummy-vibra/mfbackenddummyvibra.h"

#include <QObject>
#include <QHash>
#include <QPointer>
#include <QDebug>

class MfFeedbackHandle;

class MfManagerTest : public MfManager
{
    Q_OBJECT
public:
    MfManagerTest() {};
    ~MfManagerTest() {
        qDeleteAll(backendList);
    };

    void loadBackendPlugins(const QString &libDir)
    {
        if (libDir == "/usr/lib/meegofeedbackd/non-threaded-backends") {
            audioBackend = (MfBackendDummyAudio *)new MfBackendDummyAudio();
            backendList.append(dynamic_cast<MfBackendBase*>(audioBackend));
        } else if (libDir == "/usr/lib/meegofeedbackd/threaded-backends") {
            vibraBackend = new MfBackendDummyVibra();
            backendList.append(dynamic_cast<MfBackendBase*>(vibraBackend));
        }
    };

    void loadSourcePlugins(const QString &libDir)
    {
        Q_UNUSED(libDir);
    };

    QHash<QString, QPointer<MfFeedbackHandle> > feedbackCache()
    {
        return feedbackHandleHash;
    };

    MfBackendDummyAudio *audioBackend;
    MfBackendDummyVibra *vibraBackend;
};

#endif
