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

#include "ut_mffeedback.h"

#include <mffeedback.h>
#include <mfbackendinterface.h>

#include "mfbackendfoo.h"
#include "mffeedbackhandle_mock.h"

void Ut_MfFeedback::initTestCase()
{
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    manager = new MfManager(this);
    manager->init(QString("base"), emptyVolumeMapList);
}

void Ut_MfFeedback::cleanupTestCase()
{
    delete manager;
    manager = 0;
}

void Ut_MfFeedback::init()
{
    // Clear counters between tests
    MfFeedbackHandle::playedPaths.clear();
    manager->cacheHits = 0;
}

void Ut_MfFeedback::cleanup()
{
}

/* Load a feedback with empty string list and make sure
 * that nothing gets loaded or played.
 */
void Ut_MfFeedback::loadPlayEmptyDirs()
{
    QStringList emptyList;
    MfFeedback *feedback = 0;

    feedback = new MfFeedback(this);
    feedback->load(emptyList);

    // Make sure there are enough handles reserved
    const QList<QPointer<MfFeedbackHandle> > &testHandles = feedback->fbHandles();
    QCOMPARE(testHandles.size(), manager->backendList.size());

    // And that the handles are NULL
    for (int i = 0; i < testHandles.size(); ++i) {
        QCOMPARE(testHandles[i].isNull(), true);
    }

    // Name should be empty
    QCOMPARE(feedback->name().isEmpty(), true);

    // Play should succeed even in this case
    feedback->emitPlay(-1);
    QCoreApplication::processEvents();

    // Nothing should have been actually played
    QCOMPARE(MfFeedbackHandle::playedPaths.size(), 0);

    delete feedback;
    feedback = 0;
}

/* Load a feedbacks that don't exist at all and make sure
 * nothing is loaded or played.
 */
void Ut_MfFeedback::loadPlayNoHandles()
{
    MfFeedback *feedback = 0;
    QStringList dirList;

    // Small description about using directories in this test:
    // If the directory name contains "empty", it is handled as a directory
    // that doesn't contain a valid feedback. If there is "ignored" in the
    // directory name, it is handled as if there was a 0-byte feedback file
    // in the directory. If there is "ready" in the directory name, it is
    // handled as if there was a valid feedback file in the directory. If
    // there is "cached" somewhere in the directory name, it is handled as
    // if the feedback is found from cache.
    //
    // If the last character of the directory is a number (0 or 1), it tells
    // for which backend this directory is meant for. If there is no number,
    // the directory is meant for both backends.
    dirList << "/foo/empty/foo" << "/bar/empty/bar" << "/gah/empty/gah";

    feedback = new MfFeedback(this);
    feedback->load(dirList);

    // Make sure that handles are NULL
    const QList<QPointer<MfFeedbackHandle> > &testHandles = feedback->fbHandles();
    for (int i = 0; i < testHandles.size(); ++i) {
        QCOMPARE(testHandles[i].isNull(), true);
    }

    // Name should be the last part of first path
    QCOMPARE(feedback->name(), QString("foo"));

    // Play should succeed even in this case
    feedback->emitPlay(-1);
    QCoreApplication::processEvents();

    // Nothing should have been actually played
    QCOMPARE(MfFeedbackHandle::playedPaths.size(), 0);

    delete feedback;
    feedback = 0;
}

/* Load feedbacks which are not cached and play them.
 * Make sure there are no cache hits and that they
 * are actually played.
 */
void Ut_MfFeedback::loadPlayLoadedHandles()
{
    MfFeedback *feedback = 0;
    QStringList dirList;

    dirList << "/foo/empty/foo" << "/bar/ready/bar0" << "/gah/ready/gah1";

    feedback = new MfFeedback(this);
    feedback->load(dirList);

    // Make sure that handles are not NULL
    const QList<QPointer<MfFeedbackHandle> > &testHandles = feedback->fbHandles();
    for (int i = 0; i < testHandles.size(); ++i) {
        QCOMPARE(testHandles[i].isNull(), false);
    }

    // Name should be the last part of first path
    QCOMPARE(feedback->name(), QString("foo"));

    feedback->emitPlay(-1);
    QCoreApplication::processEvents();

    // Played paths should contain 2 paths
    QCOMPARE(MfFeedbackHandle::playedPaths.size(), 2);

    // Make sure the correct handles got played
    QCOMPARE(MfFeedbackHandle::playedPaths[0], QString("/bar/ready/bar0"));
    QCOMPARE(MfFeedbackHandle::playedPaths[1], QString("/gah/ready/gah1"));

    // There should have been 0 cache hits
    QCOMPARE(manager->cacheHits, 0);

    delete feedback;
    feedback = 0;
}

/* Load a feedbacks which are ignored. Make sure they are not
 * played.
 */
void Ut_MfFeedback::loadPlayIgnoredHandles()
{
    MfFeedback *feedback = 0;
    QStringList dirList;

    dirList << "/foo/empty/foo" << "/bar/ignored/bar1" << "/gah/ignored/gah0";

    feedback = new MfFeedback(this);
    feedback->load(dirList);

    // Make sure that handles are not NULL
    const QList<QPointer<MfFeedbackHandle> > &testHandles = feedback->fbHandles();
    for (int i = 0; i < testHandles.size(); ++i) {
        QCOMPARE(testHandles[i].isNull(), false);
    }

    // Name should be the last part of first path
    QCOMPARE(feedback->name(), QString("foo"));

    feedback->emitPlay(-1);
    QCoreApplication::processEvents();

    // Played paths should contain 0 paths
    QCOMPARE(MfFeedbackHandle::playedPaths.size(), 0);

    // There should have been 0 cache hits
    QCOMPARE(manager->cacheHits, 0);

    delete feedback;
    feedback = 0;
}

/* Load a feedback which has cached handles and play it.
 * Make sure that there were cache hits and that the cached
 * handles are played.
 */
void Ut_MfFeedback::loadPlayCachedHandles()
{
    MfFeedback *feedback = 0;
    QStringList dirList;

    dirList << "/foo/empty/foo" << "/bar/empty/bar" << "/gah/cached/gah";

    feedback = new MfFeedback(this);
    feedback->load(dirList);

    // Make sure that handles are not NULL
    const QList<QPointer<MfFeedbackHandle> > &testHandles = feedback->fbHandles();
    for (int i = 0; i < testHandles.size(); ++i) {
        QCOMPARE(testHandles[i].isNull(), false);
    }

    // Name should be the last part of first path
    QCOMPARE(feedback->name(), QString("foo"));

    feedback->emitPlay(-1);
    QCoreApplication::processEvents();

    // Cached handle should have been played twice
    QCOMPARE(MfFeedbackHandle::playedPaths.size(), 2);

    // Make sure it was the cached feedback that got played
    for (int i = 0; i < MfFeedbackHandle::playedPaths.size(); ++i) {
        QCOMPARE(MfFeedbackHandle::playedPaths[i], QString("cached"));
    }

    // There should have been 2 cache hits
    QCOMPARE(manager->cacheHits, 2);

    // The cached handle should now have a reference count of 3
    QCOMPARE(manager->cachedHandle->refCount(), 3);

    delete feedback;
    feedback = 0;

    // After deleting the cached handle should have a
    // reference count of 1
    QCOMPARE(manager->cachedHandle->refCount(), 1);
}

/* Load feedbacks and then reload different feedbacks.
 * Make sure that they are actually played.
 */
void Ut_MfFeedback::loadPlayReloadedHandles()
{
    MfFeedback *feedback = 0;
    QStringList dirList1, dirList2;

    dirList1 << "/foo/empty/foo" << "/bar/ready/bar0" << "/gah/cached/gah1";
    dirList2 << "/foo/empty/foo" << "/bar/ignored/foo0" << "/gah/cached/doh1";

    feedback = new MfFeedback(this);
    feedback->load(dirList1);

    // Name should be the last part of first path
    QCOMPARE(feedback->name(), QString("foo"));

    // Make sure that handles are not NULL
    const QList<QPointer<MfFeedbackHandle> > &testHandles = feedback->fbHandles();
    for (int i = 0; i < testHandles.size(); ++i) {
        QCOMPARE(testHandles[i].isNull(), false);
    }

    feedback->emitPlay(-1);
    QCoreApplication::processEvents();

    // Load second set
    feedback->load(dirList2);

    // Make sure that handles are not NULL
    for (int i = 0; i < testHandles.size(); ++i) {
        QCOMPARE(testHandles[i].isNull(), false);
    }

    feedback->emitPlay(-1);
    QCoreApplication::processEvents();

    // Played paths should contain 3 paths
    QCOMPARE(MfFeedbackHandle::playedPaths.size(), 3);

    // Make the correct handles got played
    QCOMPARE(MfFeedbackHandle::playedPaths[0], QString("/bar/ready/bar0"));
    QCOMPARE(MfFeedbackHandle::playedPaths[1], QString("cached"));
    QCOMPARE(MfFeedbackHandle::playedPaths[2], QString("cached"));

    // There should have been 2 cache hits
    QCOMPARE(manager->cacheHits, 2);

    // The cached handle should now have a reference count of 2
    QCOMPARE(manager->cachedHandle->refCount(), 2);

    delete feedback;
    feedback = 0;

    // After deleting the cached handle should have a
    // reference count of 1
    QCOMPARE(manager->cachedHandle->refCount(), 1);
}


QTEST_MAIN(Ut_MfFeedback);
