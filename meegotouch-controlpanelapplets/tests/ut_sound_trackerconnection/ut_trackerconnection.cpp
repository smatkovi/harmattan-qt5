/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#include "ut_trackerconnection.h"
#include "trackerconnection.h"

#include <QSignalSpy>
#include <QVariant>
#include <QList>
#include <QString>
#include <QStringList>

#define DEBUG
#define WARNING
#include "../debug.h"

/*********************************************
 * Stub for QtSparql if it is available
 */
#ifdef HAVE_QTSPARQL
static bool stubSparqlResultHasError = false;

#include <QSparqlConnection>
#include <QSparqlQuery>
#include <QSparqlResult>
#include <QSparqlResultRow>
#include <QSparqlError>

#if 0
QSparqlResult *
QSparqlConnection::syncExec (const QSparqlQuery& query)
{
    Q_UNUSED (query);
    return new QSparqlResult;
}
#endif

void
QSparqlResult::waitForFinished ()
{
    return;
}

bool
QSparqlResult::hasError() const
{
    return stubSparqlResultHasError;
}
#endif

/*********************************************
 * Begin UNIT TEST
 */
void
Ut_TrackerConnection::initTestCase ()
{
    SYS_DEBUG ("");
}

void
Ut_TrackerConnection::cleanupTestCase ()
{
    SYS_DEBUG ("");
}

void
Ut_TrackerConnection::init ()
{
    SYS_DEBUG ("");
}

void
Ut_TrackerConnection::cleanup ()
{
    SYS_DEBUG ("");
}

void
Ut_TrackerConnection::testConstructDestruct ()
{
    TrackerConnection *tc = 0;

    tc = TrackerConnection::instance ();

    QVERIFY (tc);
    QCOMPARE (tc, tc->s_Instance);

    delete tc;

    QVERIFY (! tc->s_Instance);
}

void
Ut_TrackerConnection::testRegisterFileCopy ()
{
    TrackerConnection *tc = TrackerConnection::instance ();

    QString origFile = "/sdcard/blah-blah.mp3";
    QString copyFile = "~/blah-blah-copy.mp3";

    tc->registerFileCopy (origFile, copyFile);

    QCOMPARE (tc->m_FileCopies[copyFile], origFile);
}

void
Ut_TrackerConnection::testTrackerIDfromFileName ()
{
    TrackerConnection *tc = TrackerConnection::instance ();

    /*
     * test an already cashed version
     */
    QString trackA = "/sdcard/mymusic.wma";
    QString trackerIdforTrackA = "xyz556";
    tc->m_TrackerIdCache[trackA] = trackerIdforTrackA;

    QCOMPARE (tc->trackerIdFromFileName (trackA), trackerIdforTrackA);

    QString trackB = "/sdcard/mymusic2.ogg";

    QCOMPARE (tc->trackerIdFromFileName (trackB), QString (""));

    // TODO: test here whether the tracker is initiated to look for this song
}

void
Ut_TrackerConnection::testTrackerIDtoFileName ()
{
    TrackerConnection *tc = TrackerConnection::instance ();

    QCOMPARE (tc->trackerIdToFilename (""), QString (""));

#ifdef HAVE_QTSPARQL
    // TODO write test here, but first stub the QtSparql
#endif
}

void
Ut_TrackerConnection::testProcessRequest ()
{
    TrackerConnection *tc = TrackerConnection::instance ();
    QSignalSpy spy (tc, SIGNAL (dataReady (const QString &,    // filename
                                           const QString &,    // title
                                           const QString &))); // trackerid

    /*
     * tracker will not used here has it has no chanse for this
     */
    QString somePath = "/sdcard/xyz/";
    QString someMusicTitle = "someMusic";
    QString someMusic = someMusicTitle + ".flac";

    tc->processRequest (somePath + someMusic);

    QTest::qWait (50);

    QCOMPARE (spy.count (), 1);
    QList<QVariant> args = spy.takeLast ();

    QCOMPARE (args.at (0).toString (), somePath + someMusic);
    QCOMPARE (args.at (1).toString (), someMusicTitle);
    QCOMPARE (args.at (2).toString (), QString (""));

#ifdef HAVE_QTSPARQL
    // TODO write test here, but first stub the QtSparql
#endif
}

QTEST_MAIN(Ut_TrackerConnection)

