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

#include <QDebug>
#include <QtGlobal>
#include <QCoreApplication>
#include <QByteArray>

#include <mfconfiguration.h>
#include "mfmanagertest.h"
#include "mffeedbacktest.h"
#include "mfsession.h"
#include "gconf_mock.h"
#include "globals.h"

int verifySessionTheme(MfSession *session, const QString &expectedTheme)
{
    int retVal = 0;

    if (g_lastLoadedTheme.contains(session->applicationName()) == false) {
        qWarning() << "Failed test for application:" << session->applicationName() << endl
                   << "Theme not loaded";
        retVal = 1;
    } else if (g_lastLoadedTheme[session->applicationName()] != expectedTheme) {
        qWarning() << "Failed test for application:" << session->applicationName() << endl
                   << "Expected theme name:" << expectedTheme
                   << "actual theme name:" << g_lastLoadedTheme[session->applicationName()];
        retVal = 1;
    }

    return retVal;
}

int testThemeChange()
{
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    MfConfiguration configuration;
    MfManagerTest* manager;
    MfSession *session1, *session2, *session3;
    QByteArray mockedThemeName;
    GConfEntry mockedEntry;
    GConfValue mockedValue;
    int retVal = 0;

    mockedEntry.value = &mockedValue;

    // Clear MfFeedback's reference to old MfManager
    MfFeedbackTest::clearManagerPointer();

    manager = new MfManagerTest();

    configuration.init();

    manager->init(configuration.theme(), emptyVolumeMapList, emptyDeviceStatusMap);

    // Use direct connection, to be sure, that basic feedback list is updated immediately
    manager->connect(&configuration, SIGNAL(themeChanged(QString)),
                     SLOT(loadTheme(QString)), Qt::DirectConnection);

    // Create new session
    session1 = new MfSession("Testapp1");
    // Process events because the session setup
    // signals/slots with queued connection.
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // Loaded theme shold now be MeegoDefaultTheme
    retVal |= verifySessionTheme(session1, MeegoDefaultTheme);

    // Change theme to "sometheme"
    mockedThemeName = "sometheme";
    mockedEntry.value->type = GCONF_VALUE_STRING;
    mockedEntry.value->payload = mockedThemeName.data();
    g_gconfClientNotifyThemeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyThemeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    // Loaded theme shold now be "sometheme"
    retVal = verifySessionTheme(session1, "sometheme");

    // Create two new applications
    session2 = new MfSession("Testapp2");
    // Process events because the session setup
    // signals/slots with queued connection.
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    session3 = new MfSession("Testapp3");
    // Process events because the session setup
    // signals/slots with queued connection.
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

    // Their loaded theme should be "sometheme" as well
    retVal |= verifySessionTheme(session2, "sometheme");
    retVal |= verifySessionTheme(session3, "sometheme");

    // Change theme to "othertheme"
    mockedThemeName = "othertheme";
    mockedEntry.value->type = GCONF_VALUE_STRING;
    mockedEntry.value->payload = mockedThemeName.data();
    g_gconfClientNotifyThemeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyThemeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    // Delete one session
    delete session2;
    session2 = 0;

    // Remaining session's loaded theme should be "othertheme"
    retVal |= verifySessionTheme(session1, "othertheme");
    retVal |= verifySessionTheme(session3, "othertheme");

    // Set GConfValue type to something other than GCONF_VALUE_STRING.
    // Theme should not change and no segfault should happen.
    mockedThemeName = "foobar";
    mockedEntry.value->type = GCONF_VALUE_BOOL;
    mockedEntry.value->payload = mockedThemeName.data();
    g_gconfClientNotifyThemeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyThemeUserData);

    // Remaining session's loaded theme should still be "othertheme"
    retVal |= verifySessionTheme(session1, "othertheme");
    retVal |= verifySessionTheme(session3, "othertheme");

    // Set GConfValue as NULL
    // No segfault should happen
    mockedEntry.value = NULL;
    g_gconfClientNotifyThemeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyThemeUserData);

    delete session1;
    session1 = 0;

    delete session3;
    session3 = 0;

    // Change theme to "nobodyhere"
    // No segfault should happen
    mockedThemeName = "nobodyhere";
    mockedEntry.value = &mockedValue;
    mockedEntry.value->type = GCONF_VALUE_STRING;
    mockedEntry.value->payload = mockedThemeName.data();
    g_gconfClientNotifyThemeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyThemeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    delete manager;
    manager = 0;

    return retVal;
}

int verifyVolumes(MfManagerTest *manager,
                  const QList<QMap <QString, MfBackendBase::playbackVolume> > volumes)
{
    int retVal = 0;

    if (volumes != manager->backendVolumes) {
        qWarning() << "Initial volumes are were as expected. Expected:";
        for (int i = 0; i < volumes.size(); ++i) {
            qWarning() << "Volumes for volume ID:" << i << ":";
            QList<QString> keys = volumes[i].keys();
            for (int j = 0; j < keys.size(); ++j) {
                qWarning() << " " << keys[j] << ":" << volumes[i][keys[j]];
            }
        }

        qWarning() << "Actual volumes:";
        for (int i = 0; i < manager->backendVolumes.size(); ++i) {
            qWarning() << "Volumes for volume ID:" << i << ":";
            QList<QString> keys = manager->backendVolumes[i].keys();
            for (int j = 0; j < keys.size(); ++j) {
                qWarning() << " " << keys[j] << ":" << manager->backendVolumes[i][keys[j]];
            }
        }

        retVal = 1;
    }

    return retVal;
}

int testVolumeChange()
{
    MfConfiguration configuration;
    MfManagerTest *manager;
    QByteArray mockedVolumeData, mockedBackendData;
    QByteArray mockedInitialVolumeData[6], mockedInitialBackendData[6];
    GConfEntry mockedEntry, initialEntries[6];
    GConfValue mockedValue, initialValues[6];
    QList<QMap <QString, MfBackendBase::playbackVolume> > expectedVolumes;
    QMap<QString, QString> emptyDeviceStatusMap;
    int retVal = 0;

    mockedEntry.value = &mockedValue;
    for (int i = 0; i < 6; ++i) {
        initialEntries[i].value = &initialValues[i];
    }

    // Preallocate volume map list
    for (int i = 0; i < 3; ++i) {
        expectedVolumes << QMap<QString, MfBackendBase::playbackVolume>();
    }

    // Clear MfFeedback's reference to old MfManager
    MfFeedbackTest::clearManagerPointer();

    configuration.init();

    // Set initial volumes and see that they are set
    mockedInitialBackendData[0] = "/meegotouch/input_feedback/volume/dummy_audio";
    mockedInitialBackendData[1] = "/meegotouch/input_feedback/volume/dummy_vibra";
    mockedInitialBackendData[2] = "/meegotouch/input_feedback/volume/priority1/dummy_audio";
    mockedInitialBackendData[3] = "/meegotouch/input_feedback/volume/priority1/dummy_vibra";
    mockedInitialBackendData[4] = "/meegotouch/input_feedback/volume/priority2/dummy_audio";
    mockedInitialBackendData[5] = "/meegotouch/input_feedback/volume/priority2/dummy_vibra";
    mockedInitialVolumeData[0] = "high";
    mockedInitialVolumeData[1] = "off";
    mockedInitialVolumeData[2] = "medium";
    mockedInitialVolumeData[3] = "medium";
    mockedInitialVolumeData[4] = "low";
    mockedInitialVolumeData[5] = "high";
    expectedVolumes[0]["dummy_audio"] = MfBackendBase::VolumeHigh;
    expectedVolumes[0]["dummy_vibra"] = MfBackendBase::VolumeOff;
    expectedVolumes[1]["dummy_audio"] = MfBackendBase::VolumeMedium;
    expectedVolumes[1]["dummy_vibra"] = MfBackendBase::VolumeMedium;
    expectedVolumes[2]["dummy_audio"] = MfBackendBase::VolumeLow;
    expectedVolumes[2]["dummy_vibra"] = MfBackendBase::VolumeHigh;
    for (int i = 0; i < 6; ++i) {
        initialEntries[i].key = mockedInitialBackendData[i].data();
        initialEntries[i].value->payload = mockedInitialVolumeData[i].data();
        initialEntries[i].value->type = GCONF_VALUE_STRING;
    }
    g_gconfClientAllEntriesList0 = g_slist_append(g_gconfClientAllEntriesList0, (gpointer)(&initialEntries[0]));
    g_gconfClientAllEntriesList0 = g_slist_append(g_gconfClientAllEntriesList0, (gpointer)(&initialEntries[1]));
    g_gconfClientAllEntriesList1 = g_slist_append(g_gconfClientAllEntriesList1, (gpointer)(&initialEntries[2]));
    g_gconfClientAllEntriesList1 = g_slist_append(g_gconfClientAllEntriesList1, (gpointer)(&initialEntries[3]));
    g_gconfClientAllEntriesList2 = g_slist_append(g_gconfClientAllEntriesList2, (gpointer)(&initialEntries[4]));
    g_gconfClientAllEntriesList2 = g_slist_append(g_gconfClientAllEntriesList2, (gpointer)(&initialEntries[5]));

    manager = new MfManagerTest();
    manager->init(configuration.theme(), configuration.volumes(), emptyDeviceStatusMap);

    // Verify volume levels
    retVal |= verifyVolumes(manager, expectedVolumes);

    delete manager;
    manager = 0;

    // Set some garbage as initial volumes.
    mockedInitialBackendData[1] = "/meegotouch/input_feedback/volume/dummy_gobra";
    mockedInitialBackendData[2] = "/meegotouch/input_feedback/volume/priority1/dummy_audio";
    mockedInitialBackendData[4] = "/meegotouch/input_feedback/volume/priority2/dummy_audio";
    initialEntries[0].value = NULL;
    initialEntries[4].value->type = GCONF_VALUE_INT;
    initialEntries[5].value->payload = NULL;
    mockedInitialVolumeData[1] = "zap";
    mockedInitialVolumeData[4] = "foo";
    expectedVolumes[0].remove("dummy_audio");
    expectedVolumes[0].remove("dummy_vibra");
    expectedVolumes[0]["dummy_gobra"] = MfBackendBase::VolumeMedium;
    expectedVolumes[2]["dummy_vibra"] = MfBackendBase::VolumeMedium;
    expectedVolumes[2].remove("dummy_audio");

    manager = new MfManagerTest();
    manager->init(configuration.theme(), configuration.volumes(), emptyDeviceStatusMap);

    // Verify volume levels
    retVal |= verifyVolumes(manager, expectedVolumes);

    // Use direct connection, to be sure, that volume is updated immediately
    manager->connect(&configuration, SIGNAL(volumeChanged(int, QString, MfBackendBase::playbackVolume)),
                     SLOT(setVolume(int, QString, MfBackendBase::playbackVolume)), Qt::DirectConnection);

    // Set volume ID 0 Audio level to low
    mockedBackendData = "/meegotouch/input_feedback/volume/dummy_audio";
    mockedVolumeData = "low";
    mockedEntry.key = mockedBackendData.data();
    mockedEntry.value->type = GCONF_VALUE_STRING;
    mockedEntry.value->payload = mockedVolumeData.data();
    g_gconfClientNotifyVolumeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyVolumeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    // Verify volume levels
    expectedVolumes[0]["dummy_audio"] = MfBackendBase::VolumeLow;
    retVal |= verifyVolumes(manager, expectedVolumes);

    // Set volume ID 1 Vibra level to high
    mockedBackendData = "/meegotouch/input_feedback/volume/priority1/dummy_vibra";
    mockedVolumeData = "high";
    mockedEntry.key = mockedBackendData.data();
    mockedEntry.value->type = GCONF_VALUE_STRING;
    mockedEntry.value->payload = mockedVolumeData.data();
    g_gconfClientNotifyVolumeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyVolumeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    // Verify volume levels
    expectedVolumes[1]["dummy_vibra"] = MfBackendBase::VolumeHigh;
    retVal |= verifyVolumes(manager, expectedVolumes);

    // Set volume ID 1 Vibra level to some unsupported value, volume
    // should be Medium after this for vibra.
    mockedBackendData = "/meegotouch/input_feedback/volume/priority1/dummy_vibra";
    mockedVolumeData = "FooBar";
    mockedEntry.key = mockedBackendData.data();
    mockedEntry.value->type = GCONF_VALUE_STRING;
    mockedEntry.value->payload = mockedVolumeData.data();
    g_gconfClientNotifyVolumeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyVolumeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    // Verify volume levels
    expectedVolumes[1]["dummy_vibra"] = MfBackendBase::VolumeMedium;
    retVal |= verifyVolumes(manager, expectedVolumes);

    // Set GConfValue type to something other than GCONF_VALUE_STRING.
    // Volume should have remained the same and no segfault should happen.
    mockedBackendData = "/meegotouch/input_feedback/volume/priority1/dummy_vibra";
    mockedEntry.key = mockedBackendData.data();
    mockedEntry.value->type = GCONF_VALUE_BOOL;
    mockedEntry.value->payload = NULL;
    g_gconfClientNotifyVolumeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyVolumeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    // Verify volume levels
    retVal |= verifyVolumes(manager, expectedVolumes);

    // Set GConfValue to NULL.
    // Volume should have remained the same and no segfault should happen.
    mockedBackendData = "/meegotouch/input_feedback/volume/dummy_audio";
    mockedEntry.key = mockedBackendData.data();
    mockedEntry.value = NULL;
    g_gconfClientNotifyVolumeFuncPtr(0, 0, &mockedEntry, g_gconfClientNotifyVolumeUserData);

    // Make sure signal gets processed
    QCoreApplication::instance()->processEvents();

    // Verify volume levels
    retVal |= verifyVolumes(manager, expectedVolumes);

    delete manager;
    manager = 0;

    g_slist_free(g_gconfClientAllEntriesList0);
    g_slist_free(g_gconfClientAllEntriesList1);
    g_slist_free(g_gconfClientAllEntriesList2);
    g_gconfClientAllEntriesList0 = NULL;
    g_gconfClientAllEntriesList1 = NULL;
    g_gconfClientAllEntriesList2 = NULL;

    return retVal;
}

int main(int argc, char **argv)
{
    QCoreApplication fooApp(argc, argv);
    int result1 = 0, result2 = 0;

    // Change directory to the directory where the application is located
    // to make sure that theming logic finds the necessary paths and files
    if (QDir(QCoreApplication::applicationDirPath()).exists("ft_configuration"))
    {
        chdir(QCoreApplication::applicationDirPath().toAscii());
    } else {
        chdir(QCoreApplication::applicationFilePath().toAscii());
    }

    result1 = testThemeChange();
    if (result1 != 0) {
        qWarning() << "Theme test failed";
    } else {
        qWarning() << "Theme test successful";
    }

    result2 = testVolumeChange();
    if (result2 != 0) {
        qWarning() << "Volume test failed";
    } else {
        qWarning() << "Volume test successful";
    }

    if (result1 || result2) {
        qFatal("Theme or volume test FAILED!");
    }

    fooApp.exit(0);
}
