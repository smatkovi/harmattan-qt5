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

#include "ut_mfmanager.h"

#include "mfbackendfoo.h"
#include "globals.h"

#include <mfconfig.h>
#include <mfmanager.h>
#include <mffeedback.h>

class MfManagerTest : public MfManager
{
public:
    MfManagerTest() {};
    ~MfManagerTest() {};

    void emitDeviceStateChanged(const QMap<QString, QString> &newState);
};

void MfManagerTest::emitDeviceStateChanged(const QMap<QString, QString> &newState)
{
    emit deviceStateChanged(newState);
}

void Ut_MfManager::initTestCase()
{
    qRegisterMetaType< QMap<QString,QString> >("QMap<QString,QString>");
}

void Ut_MfManager::cleanupTestCase()
{
}

void Ut_MfManager::cleanup()
{
    qDeleteAll(g_threadedBackendFooInstances);
    g_threadedBackendFooInstances.clear();
    qDeleteAll(g_nonThreadedBackendFooInstances);
    g_nonThreadedBackendFooInstances.clear();
    qDeleteAll(g_sourceFooInstances);
    g_sourceFooInstances.clear();
    g_threadedBackendsDirectoryEntryList.clear();
    g_nonThreadedBackendsDirectoryEntryList.clear();
    g_sourcesDirectoryEntryList.clear();
    g_loadedInstances.clear();
}

void Ut_MfManager::initNoSourcesOneBackend()
{
    MfManagerTest *manager;
    MfBackendFoo *fooBackend = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // There's only one non-threaded backend there and nothing more.
    g_nonThreadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so";

    // Initialization should not fail if there is backend
    // but no source. Sources are optional.
    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Get the loaded backend
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 1);
    fooBackend = g_nonThreadedBackendFooInstances.at(0);

    // Wait a while to get backend initialized. Backend initialization
    // uses queued signals/slots so this gives some time to process
    // those events.
    QTest::qWait(50);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);

    // Make sure nothing extra was created
    QCOMPARE(g_threadedBackendFooInstances.size(), 0);
    QCOMPARE(g_sourceFooInstances.size(), 0);

    delete manager;
}

void Ut_MfManager::initNoSourcesNoBackend()
{
    MfManagerTest *manager;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);

    // It can't init if there's no backend library available.
    QCOMPARE(ok, false);

    QCOMPARE(g_threadedBackendFooInstances.size(), 0);
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 0);
    QCOMPARE(g_sourceFooInstances.size(), 0);

    delete manager;
}

void Ut_MfManager::initOneSourceOneBackend()
{
    MfManagerTest *manager;
    MfBackendFoo *fooBackend = 0;
    MfSourceFoo *fooSource = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // Put one source library and one backend library there
    g_threadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so";
    g_sourcesDirectoryEntryList << "libmeegofeedback-srcfoo.so";

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Get the loaded backend
    QCOMPARE(g_threadedBackendFooInstances.size(), 1);
    fooBackend = g_threadedBackendFooInstances.at(0);

    // Wait a while to get backend initialized
    QTest::qWait(50);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded source
    QCOMPARE(g_sourceFooInstances.size(), 1);
    fooSource = g_sourceFooInstances.at(0);

    // Check that MfSourceFoo was properly initialized
    QCOMPARE(fooSource->initCalled, 1);
    QCOMPARE(fooSource->deviceStateChangedCalled, 1);

    // Make sure nothing extra was created
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 0);

    delete manager;
}

void Ut_MfManager::initOneSourceTwoBackends()
{
    MfManagerTest *manager;
    MfBackendFoo *fooBackend = 0;
    MfSourceFoo *fooSource = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // Put one source library and one backend library there
    g_threadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so";
    g_nonThreadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so";
    g_sourcesDirectoryEntryList << "libmeegofeedback-srcfoo.so";

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Wait a while to get backend initialized
    QTest::qWait(50);

    // Get the loaded backend
    QCOMPARE(g_threadedBackendFooInstances.size(), 1);
    fooBackend = g_threadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded backend
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 1);
    fooBackend = g_nonThreadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded source
    QCOMPARE(g_sourceFooInstances.size(), 1);
    fooSource = g_sourceFooInstances.at(0);

    // Check that MfSourceFoo was properly initialized
    QCOMPARE(fooSource->initCalled, 1);
    QCOMPARE(fooSource->deviceStateChangedCalled, 1);

    delete manager;
}

void Ut_MfManager::initFourBackends()
{
    MfManagerTest *manager;
    MfBackendFoo *fooBackend = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // Put one source library and one backend library there
    g_threadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so" << "libmeegofeedback-befoo2.so";
    g_nonThreadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so" << "libmeegofeedback-befoo2.so";

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Wait a while to get backend initialized
    QTest::qWait(50);

    // Get the loaded backend #1
    QCOMPARE(g_threadedBackendFooInstances.size(), 2);
    fooBackend = g_threadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded backend #2
    fooBackend = g_threadedBackendFooInstances.at(1);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded backend #3
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 2);
    fooBackend = g_nonThreadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded backend #4
    fooBackend = g_nonThreadedBackendFooInstances.at(1);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Make sure nothing extra was created
    QCOMPARE(g_sourceFooInstances.size(), 0);

    delete manager;
}

void Ut_MfManager::initTwoBackendsTwice()
{
    MfManagerTest *manager;
    MfBackendFoo *fooBackend = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // Put one source library and one backend library there
    g_threadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so" << "libmeegofeedback-befoo1.so";
    g_nonThreadedBackendsDirectoryEntryList << "libmeegofeedback-befoo2.so" << "libmeegofeedback-befoo2.so";

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Wait a while to get backend initialized
    QTest::qWait(50);

    // The two backend instances should be the same and
    // the init method should be called only once.
    QCOMPARE(g_threadedBackendFooInstances.size(), 2);
    QCOMPARE(g_threadedBackendFooInstances.at(0),
             g_threadedBackendFooInstances.at(1));
    fooBackend = g_threadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // The two backend instances should be the same and
    // the init method should be called only once.
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 2);
    QCOMPARE(g_nonThreadedBackendFooInstances.at(0),
             g_nonThreadedBackendFooInstances.at(1));
    fooBackend = g_nonThreadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Make sure nothing extra was created
    QCOMPARE(g_sourceFooInstances.size(), 0);

    delete manager;
}

void Ut_MfManager::initTwoSourcesTwice()
{
    MfManagerTest *manager;
    MfSourceFoo *fooSource = 0;
    MfBackendFoo *fooBackend = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // Put one source library and one backend library there
    g_threadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so";
    g_sourcesDirectoryEntryList << "libmeegofeedback-srcfoo.so" << "libmeegofeedback-srcfoo.so";

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Wait a while to get backend initialized
    QTest::qWait(50);

    // Get the loaded backend
    QCOMPARE(g_threadedBackendFooInstances.size(), 1);
    fooBackend = g_threadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // The two source instances should be the same and
    // the init method should be called only once.
    QCOMPARE(g_sourceFooInstances.size(), 2);
    QCOMPARE(g_sourceFooInstances.at(0),
             g_sourceFooInstances.at(1));
    fooSource = g_sourceFooInstances.at(0);

    // Check that MfSourceFoo was properly initialized
    QCOMPARE(fooSource->initCalled, 1);
    QCOMPARE(fooSource->deviceStateChangedCalled, 1);

    // Make sure nothing extra was created
    QCOMPARE(g_nonThreadedBackendsDirectoryEntryList.size(), 0);

    delete manager;
}

void Ut_MfManager::checkHardwareDetection()
{
    MfManagerTest *manager;
    MfBackendFoo *fooBackend = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // Put one source library and one backend library there
    g_threadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so" << "libmeegofeedback-nohw.so";

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Wait a while to get backend initialized
    QTest::qWait(50);

    // Get the loaded backend #1
    QCOMPARE(g_threadedBackendFooInstances.size(), 2);
    fooBackend = g_threadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded backend #2
    fooBackend = g_threadedBackendFooInstances.at(1);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 0);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 0);
    QVERIFY(fooBackend->limit != MfConfig::feedbackDurationLimit());

    // Make sure nothing extra was created
    QCOMPARE(g_sourceFooInstances.size(), 0);
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 0);

    delete manager;
}

void Ut_MfManager::checkDeviceStatePropagation()
{
    MfManagerTest *manager;
    MfBackendFoo *fooBackend = 0;
    MfSourceFoo *fooSource = 0;
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    bool ok;

    // Put one source library and one backend library there
    g_threadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so";
    g_nonThreadedBackendsDirectoryEntryList << "libmeegofeedback-befoo1.so";
    g_sourcesDirectoryEntryList << "libmeegofeedback-srcfoo.so";

    manager = new MfManagerTest();
    ok = manager->init(MeegoDefaultTheme, emptyVolumeMapList, emptyDeviceStatusMap);
    QVERIFY(ok);

    // Wait a while to get backend initialized
    QTest::qWait(50);

    // Get the loaded backend
    QCOMPARE(g_threadedBackendFooInstances.size(), 1);
    fooBackend = g_threadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded backend
    QCOMPARE(g_nonThreadedBackendFooInstances.size(), 1);
    fooBackend = g_nonThreadedBackendFooInstances.at(0);

    // Check that MfBackendFoo was properly initialized
    QCOMPARE(fooBackend->initCalled, 1);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 1);
    QVERIFY(fooBackend->limit == MfConfig::feedbackDurationLimit());

    // Get the loaded source
    QCOMPARE(g_sourceFooInstances.size(), 1);
    fooSource = g_sourceFooInstances.at(0);

    // Check that MfSourceFoo was properly initialized
    QCOMPARE(fooSource->initCalled, 1);
    QCOMPARE(fooSource->deviceStateChangedCalled, 1);

    // Emit new device state and see that every backend and source
    // got the new information.
    manager->emitDeviceStateChanged(emptyDeviceStatusMap);
    QTest::qWait(50);
    fooBackend = g_threadedBackendFooInstances.at(0);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 2);
    fooBackend = g_nonThreadedBackendFooInstances.at(0);
    QCOMPARE(fooBackend->deviceStateChangedCalled, 2);
    fooSource = g_sourceFooInstances.at(0);
    QCOMPARE(fooSource->deviceStateChangedCalled, 2);

    delete manager;
}

QTEST_MAIN(Ut_MfManager);
