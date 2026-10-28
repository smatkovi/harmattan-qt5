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

#include "ut_mfsignal.h"

#include <signal.h>

#include <QDebug>

#include <mfsignalhandler.h>

void Ut_MfSignal::finishTest() {
    counter += 1;
    eventLoop.exit();
}

void Ut_MfSignal::abortTest() {
    eventLoop.exit();
}

void Ut_MfSignal::sendSignal() {
    kill(getpid(), SIGTERM);
}

void Ut_MfSignal::testSignal() {
    counter = 0;
    {
        MfSignalHandler handler;
        bool ok = handler.init(SIGTERM);
        QVERIFY(ok);
        QVERIFY(MfSignalHandler::count() == 1);
        QObject::connect(&handler, SIGNAL(signalReceived(int)), this, SLOT(finishTest()));
        QTimer::singleShot(5000, this, SLOT(abortTest()));
        QTimer::singleShot(1000, this, SLOT(sendSignal()));
        eventLoop.exec();
        QVERIFY(counter == 1);
    }
    QVERIFY(MfSignalHandler::count() == 0);
}

void Ut_MfSignal::testNothing() {
    counter = 0;
    {
        MfSignalHandler handler;
        bool ok = handler.init(SIGTERM);
        QVERIFY(ok);
        QVERIFY(MfSignalHandler::count() == 1);
        QObject::connect(&handler, SIGNAL(signalReceived(int)), this, SLOT(finishTest()));
        QTimer::singleShot(5000, this, SLOT(abortTest()));
        eventLoop.exec();
        QVERIFY(counter == 0);
    }
    QVERIFY(MfSignalHandler::count() == 0);
}

void Ut_MfSignal::testMiss() {
    counter = 0;
    {
        MfSignalHandler handlerTerm, handlerUsr1;
        bool ok = handlerTerm.init(SIGTERM);
        QVERIFY(ok);
        QVERIFY(MfSignalHandler::count() == 1);
        handlerUsr1.catchSignal(SIGUSR1);
        ok = handlerUsr1.init();
        QVERIFY(ok);
        QVERIFY(MfSignalHandler::count() == 2);
        QObject::connect(&handlerUsr1, SIGNAL(signalReceived(int)), this, SLOT(finishTest()));
        QTimer::singleShot(5000, this, SLOT(abortTest()));
        QTimer::singleShot(1000, this, SLOT(sendSignal()));
        eventLoop.exec();
        QVERIFY(counter == 0);
    }
    QVERIFY(MfSignalHandler::count() == 0);
}

QTEST_MAIN(Ut_MfSignal)

