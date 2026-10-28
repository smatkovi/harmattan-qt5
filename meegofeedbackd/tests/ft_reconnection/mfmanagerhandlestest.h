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

#ifndef MFMANAGERHANDLESTEST_H
#define MFMANAGERHANDLESTEST_H

#include <mffeedbackhandle.h>
#include <mfmanager.h>
#include <mfbackendbase.h>

#include "../../meegofeedback-dummy-audio/mfbackenddummyaudio.h"
#include "../../meegofeedback-dummy-vibra/mfbackenddummyvibra.h"

#include <QObject>
#include <QPointer>
#include <QDebug>

class MfManagerHandlesTest : public MfManager
{
    Q_OBJECT
public:
    MfManagerHandlesTest() {};
    ~MfManagerHandlesTest() {
        qDeleteAll(backendList);
    };

    void loadBackendPlugins(const QString &libDir)
    {
        if (libDir == "/usr/lib/meegofeedbackd/non-threaded-backends") {
            audioBackend = (MfBackendDummyAudio *)new MfBackendDummyAudio();
            audioBackend->init(0);
            backendList.append(dynamic_cast<MfBackendBase*>(audioBackend));
        } else if (libDir == "/usr/lib/meegofeedbackd/threaded-backends") {
            vibraBackend = new MfBackendDummyVibra();
            vibraBackend->init(0);
            backendList.append(dynamic_cast<MfBackendBase*>(vibraBackend));
        }
    };

    void loadSourcePlugins(const QString &libDir)
    {
        Q_UNUSED(libDir);
    };

    bool checkHandleCache(MfBackendBase* backend)
    {
        QHash<QString, QPointer<MfFeedbackHandle> >::const_iterator cacheIter = feedbackHandleHash.begin();
        while (cacheIter != feedbackHandleHash.end()) {
            if ((*cacheIter)->backend() == backend)
            {
                return true;
            }
            cacheIter++;
        }
        return false;
    }

    MfBackendDummyAudio *audioBackend;
    MfBackendDummyVibra *vibraBackend;
};

#endif
