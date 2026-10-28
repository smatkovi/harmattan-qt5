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

#include "ut_mfcmdline.h"

void Ut_MfCmdLine::init()
{
}

void Ut_MfCmdLine::cleanup()
{
}

void Ut_MfCmdLine::testDaemonLog()
{
    bool ok = false;
    bool daemonMode = false;
    bool log = false;
    bool syslog = false;
    bool verbose = false;
    QStringList arguments;

    arguments << "meegofeedbackd";
    arguments << "--daemon";
    arguments << "--log";

    ok = mfProcessCmdLineArguments(daemonMode,
                                   log,
                                   syslog,
                                   verbose,
                                   arguments);
    QVERIFY(ok);
    QVERIFY(daemonMode);
    QVERIFY(log);
}

void Ut_MfCmdLine::testLogDaemon()
{
    bool ok = false;
    bool daemonMode = false;
    bool log = false;
    bool syslog = false;
    bool verbose = false;
    QStringList arguments;

    arguments << "meegofeedbackd";
    arguments << "--log";
    arguments << "--daemon";

    ok = mfProcessCmdLineArguments(daemonMode,
                                   log,
                                   syslog,
                                   verbose,
                                   arguments);
    QVERIFY(ok);
    QVERIFY(daemonMode);
    QVERIFY(log);
    QVERIFY(!syslog);
    QVERIFY(!verbose);
}

void Ut_MfCmdLine::testDaemon()
{
    bool ok = false;
    bool daemonMode = false;
    bool log = false;
    bool syslog = false;
    bool verbose = false;
    QStringList arguments;

    arguments << "meegofeedbackd";
    arguments << "--daemon";

    ok = mfProcessCmdLineArguments(daemonMode,
                                   log,
                                   syslog,
                                   verbose,
                                   arguments);
    QVERIFY(ok);
    QVERIFY(daemonMode);
    QVERIFY(!log);
    QVERIFY(!syslog);
    QVERIFY(!verbose);
}

void Ut_MfCmdLine::testLog()
{
    bool ok = false;
    bool daemonMode = false;
    bool log = false;
    bool syslog = false;
    bool verbose = false;
    QStringList arguments;

    arguments << "meegofeedbackd";
    arguments << "--log";

    ok = mfProcessCmdLineArguments(daemonMode,
                                   log,
                                   syslog,
                                   verbose,
                                   arguments);
    QVERIFY(ok);
    QVERIFY(!daemonMode);
    QVERIFY(log);
    QVERIFY(!syslog);
    QVERIFY(!verbose);
}

void Ut_MfCmdLine::testSysLog()
{
    bool ok = false;
    bool daemonMode = false;
    bool log = false;
    bool syslog = false;
    bool verbose = false;
    QStringList arguments;

    arguments << "meegofeedbackd";
    arguments << "--syslog";

    ok = mfProcessCmdLineArguments(daemonMode,
                                   log,
                                   syslog,
                                   verbose,
                                   arguments);
    QVERIFY(ok);
    QVERIFY(!daemonMode);
    QVERIFY(!log);
    QVERIFY(syslog);
    QVERIFY(!verbose);
}

void Ut_MfCmdLine::testVerbose()
{
    bool ok = false;
    bool daemonMode = false;
    bool log = false;
    bool syslog = false;
    bool verbose = false;
    QStringList arguments;

    arguments << "meegofeedbackd";
    arguments << "--verbose";

    ok = mfProcessCmdLineArguments(daemonMode,
                                   log,
                                   syslog,
                                   verbose,
                                   arguments);
    QVERIFY(ok);
    QVERIFY(!daemonMode);
    QVERIFY(!log);
    QVERIFY(!syslog);
    QVERIFY(verbose);
}

void Ut_MfCmdLine::testHelp()
{
    bool ok = false;
    bool daemonMode = false;
    bool log = false;
    bool syslog = false;
    bool verbose = false;
    QStringList arguments;

    arguments << "meegofeedbackd";
    arguments << "--help";

    ok = mfProcessCmdLineArguments(daemonMode,
                                   log,
                                   syslog,
                                   verbose,
                                   arguments);
    QVERIFY(!ok);
    QVERIFY(!daemonMode);
    QVERIFY(!log);
    QVERIFY(!syslog);
    QVERIFY(!verbose);
}

QTEST_MAIN(Ut_MfCmdLine)
