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

#include "mfsession.h"

#include <QAtomicInt>
#include <QDebug>
#include <QDir>
#include <QTimer>

#include "mfmanager.h"
#include "mfutil.h"

// A variable to have one session at once
static QAtomicInt setupInProgress = 0;

MfSession::MfSession(const QString &applicationName, QObject *parent)
    : QObject(parent), appName(applicationName), setupFinished(false)
{
    MfManager *manager = MfManager::instance();

    // Schedule the session setup
    QTimer::singleShot(1, this, SLOT(setup()));

    // Update to the latest theme changes
    connect(manager, SIGNAL(themeChanged(const QString&)),
            // Avoid direct connection for state changes
            this, SLOT(themeChanged(const QString&)), Qt::QueuedConnection);
}

MfSession::~MfSession()
{
    // Clear the feedback map
    QHash<QString, MfFeedback*>::const_iterator iter;
    for (iter = feedbackHash.begin();
         iter != feedbackHash.end(); ++iter)
    {
        delete iter.value();
    }
    feedbackHash.clear();

    #ifdef MF_DEBUG
    MfManager *manager = MfManager::instance();
    manager->printCache();
    #endif
}

void MfSession::setup()
{
    if (!setupInProgress.testAndSetAcquire(0, 1)) {
        // If a session setup is in progress wait some time
        QTimer::singleShot(10, this, SLOT(setup()));
        return;
    }

    MfManager *manager = MfManager::instance();
    QVector<QStringList> feedbackDirLists = getFeedbackDirLists(manager->currentTheme(), appName);
    QStringList feedbackDirs;

    for (int i = 0; i < feedbackDirLists.size(); ++i) {
        feedbackDirs << feedbackDirLists[i][0];
    }

    if (feedbackDirLists.isEmpty()) {
        // Can't live without feedbacks
        qFatal("It is not possible to load any feedbacks.");
    }

    // Map the feedbacks
    foreach (const QStringList& feedbackDirList, feedbackDirLists) {
        MfFeedback *feedback = new MfFeedback();
        feedback->load(feedbackDirList);
        feedbackHash[feedback->name()] = feedback;
    }

#ifdef MF_DEBUG
    manager->printCache();
    printHash();
#endif
    emit setupReady();
    setupFinished = true;

    if (!setupInProgress.testAndSetRelease(1, 0)) {
        // Should never happen
        qCritical("MfSession::setup: Atomic setupInProgress cleared unexpectedly!");
    }
}

void MfSession::themeChanged(const QString& themeName)
{
    QVector<QStringList> feedbackDirLists = getFeedbackDirLists(themeName, appName);

    // Reload the feedbacks
    foreach (const QStringList& feedbackDirList, feedbackDirLists) {
        QString feedbackName = feedbackDirList[0].section(QDir::separator(), -1);

        if (feedbackHash.contains(feedbackName)) {
            // Reload feedback
            feedbackHash[feedbackName]->load(feedbackDirList);
        } else {
            // This can only happen if someone has changed the theming
            // directory within the process's lifetime. The only use case
            // is a theme package update, but a living instance of the application
            // does not know the new theme logic, therefore, it does not
            // make sence to upload new feedbacks and delete obsolete ones.
            qDebug() << "At MfSession::themeChanged, Found new non-hashed feedback!";
        }
    }
}

#ifdef MF_DEBUG
void MfSession::printHash()
{
    QStringList feedbackNamesList;
    const MfFeedback* feedback;

    feedbackNamesList = feedbackHash.keys();
    qSort(feedbackNamesList);

    qDebug() << "========================\nHash content for application:" << appName;

    for (int i1 = 0; i1 < feedbackNamesList.size(); ++i1) {
        feedback = feedbackHash[feedbackNamesList[i1]];
        feedback->printData();
    }
}
#endif
