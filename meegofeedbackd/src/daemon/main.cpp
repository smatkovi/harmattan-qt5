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

#include "mfcmdline.h"
#include "mfconfiguration.h"
#include "mfdevicestatelistener.h"
#include "mfmanager.h"
#include "mfserver.h"
#include "mfsignalhandler.h"
#include <signal.h>

#include <mflog.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>

static bool mfDaemonize() {
    const int noChdir = 1;
    const int noClose = 0;
    if (daemon(noChdir, noClose) < 0) {
        qCritical("Unable to fork");
        return false;
    }
    return true;
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    MfConfiguration configuration;
    MfDeviceStateListener deviceStateListener;
    MfManager *manager = new MfManager();
    MfServer *server = new MfServer();
    MfSignalHandler sigTermHandler;
    MfSignalHandler sigIntHandler;
    bool ok;
    bool daemonMode = false;
    bool log = false;
    bool verbose = false;
    bool syslog = false;

    ok = mfProcessCmdLineArguments(daemonMode, log, syslog, verbose, app.arguments());
    if (!ok) {
        // It's not really an error.
        return 0;
    }

    mfLogInit(log, verbose, syslog);

    ok = sigTermHandler.init(SIGTERM);
    if (!ok) goto CLEANUP;

    ok = QObject::connect(&sigTermHandler, SIGNAL(signalReceived(int)), &app, SLOT(quit()));
    if (!ok) {
        qCritical("Failed to connect MfSignalHandler signals");
        goto CLEANUP;
    }

    ok = sigIntHandler.init(SIGINT);
    if (!ok) goto CLEANUP;

    ok = QObject::connect(&sigIntHandler, SIGNAL(signalReceived(int)), &app, SLOT(quit()));
    if (!ok) {
        qCritical("Failed to connect MfSignalHandler signals");
        goto CLEANUP;
    }

    ok = configuration.init();
    if (!ok) goto CLEANUP;

    // Use direct connection, to be sure, that basic feedback list is updated immediately
    ok = manager->connect(&configuration, SIGNAL(themeChanged(QString)),
                          SLOT(loadTheme(QString)), Qt::DirectConnection);
    if (!ok) goto CLEANUP;

    // Use direct connection, to be sure, that volume is updated immediately
    ok = manager->connect(&configuration, SIGNAL(volumeChanged(int, QString, MfBackendBase::playbackVolume)),
                          SLOT(setVolume(int, QString, MfBackendBase::playbackVolume)), Qt::DirectConnection);
    if (!ok) goto CLEANUP;

    deviceStateListener.init();

    ok = manager->connect(&deviceStateListener, SIGNAL(deviceStateChanged(QMap<QString, QString>)),
                          SIGNAL(deviceStateChanged(QMap<QString, QString>)));
    if (!ok) goto CLEANUP;

    ok = manager->init(configuration.theme(), configuration.volumes(), deviceStateListener.deviceState());
    if (!ok) goto CLEANUP;

    ok = server->init();
    if (!ok) goto CLEANUP;

    if (daemonMode && !mfDaemonize()) {
        return 1;
    }

    app.exec();
    qDebug("Exiting nicely...");

 CLEANUP:

    if (server != NULL) {
        delete server;
        server = 0;
    }

    if (manager != NULL) {
        delete manager;
        manager = NULL;
    }

    // Remove the /tmp/mfeedbackd directory if it is empty
    QDir tempDir("/tmp/mfeedbackd");
    if (tempDir.exists() &&
        tempDir.entryInfoList(QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs).isEmpty()) {
        tempDir.rmdir("/tmp/mfeedbackd");
    }

    if (ok)
        return 0;
    else
        return 1;
}
