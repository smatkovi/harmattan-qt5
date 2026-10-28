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

#include "mflog.h"

#include <syslog.h>

#include <QMutex>
#include <QMutexLocker>

#define MfLogMsg 4

static bool MfLogVerbose = false;

QtMsgType logMsgType = QtDebugMsg;

namespace {
    QMutex logMutex1;

    void disabledMessageHandler(QtMsgType type, const char *msg) {
        Q_UNUSED(type)
        Q_UNUSED(msg)
        QMutexLocker lock(&logMutex1);
    }
} //namespace

namespace {
    QMutex logMutex2;

    void syslogMessageHandler(QtMsgType type, const char *msg) {
        QMutexLocker lock(&logMutex2);

        if (type == QtDebugMsg) {
            if (MfLogVerbose) {
                syslog(LOG_WARNING, "%s", msg);
            }
        } else if (type == QtWarningMsg) {
            syslog(LOG_WARNING, "%s", msg);
        } else if (type == QtCriticalMsg) {
            syslog(LOG_ERR, "%s", msg);
        } else if (type == QtFatalMsg) {
            syslog(LOG_CRIT, "%s", msg);
            closelog();
            abort();
        } else {
            // It's an MfLogMsg
            syslog(LOG_INFO, "%s", msg);
        }
    }
} //namespace

namespace {
    QMutex logMutex3;

    void normalMessageHandler(QtMsgType type, const char *msg) {
        QMutexLocker lock(&logMutex3);

        if (type == QtDebugMsg) {
            if (MfLogVerbose) {
                printf("%s\n", msg);
            }
        } else {
            printf("%s\n", msg);
        }
    }
} //namespace


void mfLogInit(bool enable, bool verbose, bool syslog)
{
    MfLogVerbose = verbose;
    logMsgType = (QtMsgType) MfLogMsg;
    if (!enable) {
        qInstallMsgHandler(&disabledMessageHandler);
        return;
    }
    if (syslog) {
        qInstallMsgHandler(&syslogMessageHandler);
        openlog("meegofeedbackd", 0, LOG_DAEMON);
    } else {
        qInstallMsgHandler(&normalMessageHandler);
    }
}

QDebug mfLog()
{
    return QDebug(logMsgType);
}
