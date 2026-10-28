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

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QtDebug>
#include <QtGlobal>
#include <QString>
#include <QVector>
#include <QList>
#include <QIODevice>
#include <QCoreApplication>
#include <QTimer>

#include <mffeedback.h>
#include <mffeedbackhandle.h>
#include <mfsession.h>
#include <mfutil.h>

#include "mfmanagertest.h"

struct feedbackHandleState {
    int volumeId;
    MfFeedbackHandle::State state;
    QString path;
};

struct feedbackCacheEntry {
    int backendIndex;
    int refCount;
    QString path;
};

QCoreApplication* fooApp;

void runThemeTest(MfManagerTest* manager)
{
    MfSession* session = 0;
    QFile resultsFile("true_results.txt");
    QList<MfBackendBase::playbackVolume> expectedVolumes;

    // Set volumes to known values:
    // Low for general feedbacks (feedbacks without prefix)
    // Medium for input related feedbacks (feedbacks with prefix: "1_")
    // High for system related feedbacks (feedbacks with prefix: "2_")
    manager->setVolume(0, "dummy_audio", MfBackendBase::VolumeLow);
    manager->setVolume(0, "dummy_vibra", MfBackendBase::VolumeLow);
    manager->setVolume(1, "dummy_audio", MfBackendBase::VolumeMedium);
    manager->setVolume(1, "dummy_vibra", MfBackendBase::VolumeMedium);
    manager->setVolume(2, "dummy_audio", MfBackendBase::VolumeHigh);
    manager->setVolume(2, "dummy_vibra", MfBackendBase::VolumeHigh);
    expectedVolumes.append(MfBackendBase::VolumeLow);
    expectedVolumes.append(MfBackendBase::VolumeMedium);
    expectedVolumes.append(MfBackendBase::VolumeHigh);

    if (resultsFile.open(QIODevice::ReadOnly)) {
        QTextStream fileStream(&resultsFile);
        QString themeName;
        QString appName;

        QStringList feedbackNames;
        QVector<QStringList> feedbackDirLists;
        QVector<QList<feedbackHandleState> > feedbackHandleStates;
        int feedbackCount = 0;

        while (!fileStream.atEnd()) {
            QString lineStr;

            lineStr = fileStream.readLine();

            if (lineStr.indexOf(',') >= 0) {
                if (feedbackNames.isEmpty() == false) {

                    QVector<QStringList> utilFeedbackDirLists;
                    QStringList utilFeedbackNames;

                    utilFeedbackDirLists = getFeedbackDirLists(themeName, appName);
                    utilFeedbackNames = getFeedbackNames(appName);

                    // Make sure results file had all right amount of data
                    if (feedbackNames.size() != feedbackDirLists.size() ||
                        feedbackNames.size() != feedbackHandleStates.size() ||
                        feedbackNames.size() != feedbackCount) {
                        qFatal("Some data was missing from expected results file!");
                    }

                    // Make sure that session has expected amount of feedbacks
                    if (feedbackNames.size() != session->feedbackHash.size()) {
                        qFatal("MfSession's feedback hash size is different from expected size! (%d != %d)",
                               feedbackNames.size(), session->feedbackHash.size());
                    }

                    // Make sure that mfutil's getFeedbackDirLists() method returned fallback
                    // paths for expected amount of feedbacks
                    if (feedbackNames.size() != utilFeedbackDirLists.size()) {
                        qFatal("getFeedbackDirLists(): vector of string list size is different from expected size!");
                    }

                    // Make sure that mfutil's getFeedbackNames() method returned expected
                    // amount of feedback names
                    if (feedbackNames.size() != utilFeedbackNames.size()) {
                        qFatal("getFeedbackNames(): size is different from expected size! (%d != %d)",
                               feedbackNames.size(), utilFeedbackNames.size());
                    }

                    // Validate each feedback
                    for (int i1 = 0; i1 < feedbackNames.size(); ++i1) {
                        MfFeedback *feedback;
                        int utilFeedbackIndex = 0;

                        // Find the index of feedback in mfutil's functions
                        utilFeedbackIndex = utilFeedbackNames.indexOf(feedbackNames[i1]);
                        if (utilFeedbackIndex < 0) {
                            qFatal("getFeedbackNames(): Result did not contain expected feedback name: %s", qPrintable(feedbackNames[i1]));
                        }

                        // Validate feedback fallback paths
                        if (utilFeedbackDirLists[utilFeedbackIndex].size() < 1 ||
                            utilFeedbackDirLists[utilFeedbackIndex][0].section(QDir::separator(), -1) != feedbackNames[i1]) {
                            qFatal("getFeedbackDirLists(): Result did not contain expected feedback name: %s", qPrintable(feedbackNames[i1]));
                        }

                        if (utilFeedbackDirLists[utilFeedbackIndex].size() !=
                            feedbackDirLists[i1].size()) {
                            qFatal("getFeedbackDirLists(): Feedback \"%s\" did not have expected amount of fallback paths!", qPrintable(feedbackNames[i1]));
                        }

                        for (int i2 = 0; i2 < feedbackDirLists[i1].size(); ++i2) {
                            if (feedbackDirLists[i1][i2] !=
                                utilFeedbackDirLists[utilFeedbackIndex][i2]) {
                                qFatal("getFeedbackDirLists(): Feedback \"%s\" contains unexpected fallback paths!", qPrintable(feedbackNames[i1]));
                            }
                        }

                        // Validate actual MfFeedbacks and MfFeedbackHandles
                        if (session->feedbackHash.contains(feedbackNames[i1]) == false) {
                            qFatal("MfSession's feedback hash did not contain feedback: %s", qPrintable(feedbackNames[i1]));
                        }

                        feedback = session->feedbackHash[feedbackNames[i1]];

                        // Clear play lists of backends
                        manager->audioBackend->playList.clear();
                        manager->vibraBackend->playList.clear();
                        manager->audioBackend->volumeList.clear();
                        manager->vibraBackend->volumeList.clear();

                        // Play feedback
                        feedback->emitPlay(-1);
                        QCoreApplication::processEvents();

                        const QList<QPointer<MfFeedbackHandle> >& feedbackHandles = feedback->fbHandles();
                        if (feedbackHandles.size() != feedbackHandleStates[i1].size()) {
                            qFatal("MfFeedback had wrong amount of MfFeedbackHandles for feedback: %s", qPrintable(feedbackNames[i1]));
                        }

                        for (int i2 = 0; i2 < feedbackHandles.size(); ++i2) {
                            // Check that resulted feedback handles have correct paths
                            if (feedbackHandles[i2].isNull() == true &&
                                feedbackHandleStates[i1][i2].path != "NULL") {
                                qFatal("Feedback handle %i failed to load when success was expected (Feedback: %s)", i2, qPrintable(feedbackNames[i1]));
                            }
                            if (feedbackHandles[i2].isNull() == false &&
                                feedbackHandles[i2]->path() !=
                                feedbackHandleStates[i1][i2].path) {
                                qFatal("Feedback handle %i had unexpected path (Feedback: %s)", i2, qPrintable(feedbackNames[i1]));
                            }
                            if (feedbackHandles[i2].isNull() == false &&
                                feedbackHandles[i2]->state() !=
                                feedbackHandleStates[i1][i2].state) {
                                qFatal("Feedback handle %i had unexpected state (Feedback: %s)", i2, qPrintable(feedbackNames[i1]));
                            }

                            // Check that the correct feedback got played with correct volume
                            if (i2 == 0) {
                                if (feedbackHandles[i2].isNull() == true ||
                                    (feedbackHandles[i2].isNull() == false &&
                                     feedbackHandles[i2]->state() == MfFeedbackHandle::Ignored)) {
                                    if (manager->audioBackend->playList.size() > 0) {
                                        qFatal("Dummy audio backend did not get correct handle played (Feedback: %s)", qPrintable(feedbackNames[i1]));
                                    }
                                } else {
                                    if (!(manager->audioBackend->playList.size() == 1 &&
                                          manager->audioBackend->playList[0] ==
                                          QDir(feedbackHandles[i2]->path()).absolutePath())) {
                                        qFatal("Dummy audio backend did not get correct handle played (Feedback: %s)", qPrintable(feedbackNames[i1]));
                                    }
                                    if (!(manager->audioBackend->playList.size() == 1 &&
                                          manager->audioBackend->volumeList[0] ==
                                          expectedVolumes[feedbackHandleStates[i1][i2].volumeId])) {
                                        qFatal("Dummy audio backend did not play with correct volume (Feedback: %s)", qPrintable(feedbackNames[i1]));
                                    }
                                }
                            } else if (i2 == 1) {
                                if (feedbackHandles[i2].isNull() == true ||
                                    (feedbackHandles[i2].isNull() == false &&
                                     feedbackHandles[i2]->state() == MfFeedbackHandle::Ignored)) {
                                    if (manager->vibraBackend->playList.size() > 0) {
                                        qFatal("Dummy vibra backend did not get correct handle played (Feedback: %s)", qPrintable(feedbackNames[i1]));
                                    }
                                } else {
                                    if (!(manager->vibraBackend->playList.size() == 1 &&
                                        manager->vibraBackend->playList[0] ==
                                        QDir(feedbackHandles[i2]->path()).absolutePath())) {
                                        qFatal("Dummy vibra backend did not get correct handle played (Feedback: %s)", qPrintable(feedbackNames[i1]));
                                    }
                                    if (!(manager->vibraBackend->playList.size() == 1 &&
                                          manager->vibraBackend->volumeList[0] ==
                                          expectedVolumes[feedbackHandleStates[i1][i2].volumeId])) {
                                        qFatal("Dummy vibra backend did not play with correct volume (Feedback: %s)", qPrintable(feedbackNames[i1]));
                                    }
                                }
                            }
                        }
                    }
                }

                if (lineStr == "end_of_file,end_of_file")
                    break;

                themeName = lineStr.section(',', 0, 0);
                QString oldAppName = appName;
                appName = lineStr.section(',', 1, 1);
                qDebug() << "Test case: theme:" << themeName << "app:" << appName;
                manager->loadTheme(themeName);
                // Process events because the theme loading uses
                // signals/slots with queued connection.
                QCoreApplication::processEvents();
                if (session != NULL && oldAppName == appName) {
                    qDebug() << "Application name remained the same, reusing MfSession instance.";
                } else {
                    if (session != NULL) {
                        delete session;
                        session = NULL;
                    }
                    session = new MfSession(appName);
                    // Process events because the session setup
                    // signals/slots with queued connection.
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                }
                feedbackNames.clear();
                feedbackDirLists.clear();
                feedbackHandleStates.clear();
                feedbackCount = 0;
            } else {
                if (lineStr.at(0).isNumber() == true &&
                    lineStr.at(1) == QChar(' ')) {
                    // Line describes feedback handle state
                    QStringList stateStrList;
                    stateStrList = lineStr.split(' ');

                    if ((stateStrList.size() == 3 &&
                        stateStrList[2] != "NULL") ||
                        (stateStrList.size() != 4 &&
                        stateStrList.size() != 3)) {
                        qFatal("Parse error reading results file 1: \"%s\"", qPrintable(lineStr));
                    }

                    // Add new feedback placeholder if needed
                    if (feedbackHandleStates.size() < feedbackCount) {
                        feedbackHandleStates.append(QList<feedbackHandleState>());
                    }

                    // Make sure that the results file is parsed correctly
                    if (feedbackHandleStates.size() == feedbackCount &&
                        stateStrList[0].toInt() == feedbackHandleStates[feedbackCount-1].size()) {
                        struct feedbackHandleState handleState;

                        // Get expected volume ID of feedback
                        handleState.volumeId = stateStrList[1].toInt();

                        // Get expected feedback handle path
                        handleState.path = stateStrList[2];

                        if (handleState.path != "NULL") {
                            // Determine expected feedback handle state
                            if (stateStrList[3] == "Ready") {
                                handleState.state = MfFeedbackHandle::Ready;
                            } else if (stateStrList[3] == "Ignored") {
                                handleState.state = MfFeedbackHandle::Ignored;
                            } else {
                                handleState.state = MfFeedbackHandle::Empty;
                            }
                        }

                        feedbackHandleStates[feedbackCount-1] << handleState;
                    } else {
                        qFatal("Parse error reading results file 2: \"%s\"", qPrintable(lineStr));
                    }
                } else if (lineStr.at(0) == '.') {
                    // Line describes one fallback path

                    // Add new placeholder for fallback paths if needed
                    if (feedbackDirLists.size() < feedbackCount) {
                        feedbackDirLists.append(QStringList());
                    }

                    // Add path to list of paths
                    if (feedbackDirLists.size() == feedbackCount) {
                        feedbackDirLists[feedbackCount-1] << lineStr;
                    } else {
                        qFatal("Parse error reading results file 3: \"%s\"", qPrintable(lineStr));
                    }
                } else {
                    // Line describes feedback name
                    feedbackCount++;
                    feedbackNames << lineStr;
                }
            }
        }

        resultsFile.close();
        qDebug() << "Test successful without errors.";
    } else {
        qFatal("true_results.txt is not found");
    }

    if (session != 0) {
        delete session;
        session = 0;
    }
}

void runFeedbackCacheTest(MfManagerTest* manager)
{
    QFile resultsFile("feedback_hashing_results.txt");

    if (resultsFile.open(QIODevice::ReadOnly)) {
        QTextStream fileStream(&resultsFile);
        QString themeName;
        QVector<struct feedbackCacheEntry> cacheContent;
        QVector<QPointer<MfSession> > sessions;

        while (!fileStream.atEnd()) {
            QString lineStr;

            lineStr = fileStream.readLine();

            if (lineStr.indexOf(',') >= 0) {
                if (!cacheContent.isEmpty()) {
                    // Check that cache size matches
                    QHash<QString, QPointer<MfFeedbackHandle> > fbCache = manager->feedbackCache();
                    if (fbCache.size() !=
                        cacheContent.size()) {
                        qFatal("Cache size mismatch!");
                    }

                    // Check that all expected handles are found from cache with expected values
                    for (int i1 = 0; i1 < cacheContent.size(); i1++) {
                        QPointer<MfFeedbackHandle> cachedHandle;
                        cachedHandle = manager->searchFromCache(cacheContent[i1].path, cacheContent[i1].backendIndex);

                        if (cachedHandle.isNull() == true) {
                            qFatal("Handle with path \"%s\" and backendIndex: %i not found from cache!",
                                   qPrintable(cacheContent[i1].path), cacheContent[i1].backendIndex);
                        }

                        if (cachedHandle->refCount() != cacheContent[i1].refCount)
                        {
                            qFatal("Handle with path \"%s\" and backendIndex: %i has unexpected reference count!",
                                   qPrintable(cacheContent[i1].path), cacheContent[i1].backendIndex);
                        }
                    }
                }

                if (lineStr == "end_of_file,end_of_file")
                    break;

                themeName = lineStr.section(',', 0, 0);
                qDebug() << lineStr;
                qDeleteAll(sessions);
                for (int i = 1; !lineStr.section(',', i, i).isEmpty(); ++i)
                {
                    sessions += new MfSession(lineStr.section(',', i, i));
                    // Process events because the session setup
                    // signals/slots with queued connection.
                    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
                }
                qDebug() << "Test case: theme:" << themeName << "apps:" << lineStr.section(',', 1);
                manager->loadTheme(themeName);
                // Process events because the theme loading uses
                // signals/slots with queued connection.
                QCoreApplication::processEvents();
                cacheContent.clear();
            } else {
                // Parse cache entry
                QStringList entryStrList;
                struct feedbackCacheEntry entry;

                entryStrList = lineStr.split(' ');

                if (entryStrList.size() != 3) {
                    qFatal("Parse error reading results file!");
                }

                entry.backendIndex = entryStrList[0].toInt();
                entry.refCount = entryStrList[1].toInt();
                entry.path = entryStrList[2];

                cacheContent << entry;
            }
        }

        resultsFile.close();

        qDeleteAll(sessions);
        sessions.clear();
        qDebug() << "Test successful without errors.";
    } else {
        qFatal("true_results.txt is not found");
    }
}

int main(int argc, char **argv)
{
    fooApp = new QCoreApplication(argc, argv);
    QList<QMap<QString, MfBackendBase::playbackVolume> > emptyVolumeMapList;
    QMap<QString, QString> emptyDeviceStatusMap;
    MfManagerTest* manager = new MfManagerTest();
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    // Preallocate volume map list
    // The magical number 2 comes from MfConfigurationPrivate (maxVolumeId)
    for (int i = 0; i <= 2; ++i) {
        emptyVolumeMapList << QMap<QString, MfBackendBase::playbackVolume>();
    }

    manager->init("base", emptyVolumeMapList, emptyDeviceStatusMap);

    // Change directory to the directory where the application is located
    // to make sure that theming logic finds the necessary paths and files
    if (QDir(QCoreApplication::applicationDirPath()).exists("true_results.txt"))
    {
        chdir(QCoreApplication::applicationDirPath().toAscii());
    } else {
        chdir(QCoreApplication::applicationFilePath().toAscii());
    }

    runThemeTest(manager);
    runFeedbackCacheTest(manager);

    delete manager;
    manager = 0;

    return 0;
}
