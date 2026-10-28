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

#include "mffeedback.h"

#include <QDir>
#include <QDebug>
#include <QtConcurrentRun>

#include "mfconfig.h"
#include "mffeedbackhandle.h"
#include "mfmanager.h"
#include "mfutil.h"
#include "mfbackendbase.h"
#include "mfcommon.h"

MfManager* MfFeedback::manager = 0;

MfFeedback::MfFeedback(QObject *parent)
    : QObject(parent), feedbackVolumePriority(0)
{
    if (manager == 0) {
        manager = MfManager::instance();
        if (manager == 0) {
            qFatal("MfDeedback: Could not get MfManager instance!");
        }
    }

    // Set correct amount of placeholders for handles
    for (int i = 0; i < manager->backendList.size(); ++i) {
        feedbackHandles << QPointer<MfFeedbackHandle>();
    }

    // Set correct amount of QFuture placeholders for concurrent playing
    for (int i = 0; i < (manager->backendList.size() - manager->nonThreadedBackendCount); ++i) {
        concurrentPlayers << QFuture<void>();
    }

    connect(this, SIGNAL(startPlay(qint64)), this,
            SLOT(play(qint64)), Qt::QueuedConnection);
}

MfFeedback::~MfFeedback()
{
    unload();
    feedbackHandles.clear();
    concurrentPlayers.clear();
}

MfFeedbackHandle* MfFeedback::loadHandle(const QString &feedbackDir, const int backendInterfaceIndex)
{
    MfFeedbackHandle *feedbackHandle = 0;

    // Search from cache first
    feedbackHandle = manager->searchFromCache(feedbackDir, backendInterfaceIndex);

    // Create a new one if not found from cache
    if (feedbackHandle == 0) {
        // MfManager must be the parent QObject of all feedback handles,
        // otherwise handle might get deleted at the wrong time
        feedbackHandle = new MfFeedbackHandle(manager);
        feedbackHandle->load(feedbackDir, manager->backendList[backendInterfaceIndex]);

        // If a valid feedback was found, add it to the cache, otherwise
        // discard handle.
        if (feedbackHandle->state() != MfFeedbackHandle::Empty) {
            manager->addToCache(feedbackHandle);
        } else {
            delete(feedbackHandle);
            feedbackHandle = 0;
        }
    }

    // Increase usage
    if (feedbackHandle != 0) {
        feedbackHandle->attach();
    }

    return feedbackHandle;
}

void MfFeedback::load(const QStringList &feedbackDirList)
{
    if (feedbackDirList.isEmpty() == true) {
        qCritical() << "MfFeedback: Empty feedback directory list given to load()!";
        return;
    }

    // Set feedback name
    feedbackName = feedbackDirList[0].section(QDir::separator(), -1);

    // Determine feedback volume priority
    bool ok;
    QString volumePriorityStr = feedbackName.section("_", 0, 0);
    if (volumePriorityStr.startsWith(volumePriorityPrefix)) {
        volumePriorityStr.remove(0, strlen(volumePriorityPrefix));
        feedbackVolumePriority = volumePriorityStr.toInt(&ok);
        if (!ok) {
            feedbackVolumePriority = 0;
        }
    }

    // Determine a new handle for each backend
    for (int i1 = 0; i1 < feedbackHandles.size(); ++i1) {
        MfFeedbackHandle *newFeedbackHandle = 0;

        // Store previous feedback handle to dereference it later.
        // New feedback handle could be the same handle as the old one and
        // dereferencing it now could lead to unloading and loading
        // of the feedback unnecessarily because of the reference counting.
        MfFeedbackHandle *oldFeedbackHandle = feedbackHandles[i1];

        // Go through the possible location list until a valid one is found
        // or a valid one doesn't exist at all.
        for (int i2 = 0; i2 < feedbackDirList.size() && newFeedbackHandle == 0; ++i2) {
            newFeedbackHandle = loadHandle(feedbackDirList[i2], i1);
        }

        // Add handle to be a part of the feedback
        feedbackHandles[i1] = newFeedbackHandle;

        // Dereference old feedback handle
        if (oldFeedbackHandle != 0) {
            // Remove handle from cache if it is about to be
            // deleted.
            if (oldFeedbackHandle->refCount() <= 1) {
                manager->removeFromCache(oldFeedbackHandle);
            }
            oldFeedbackHandle->detach();
        }
    }
}

void MfFeedback::unload()
{
    for (int i1 = 0; i1 < feedbackHandles.size(); ++i1) {
        if (feedbackHandles[i1].isNull() == false) {
            // Remove handle from cache if it is about to be
            // deleted.
            if (feedbackHandles[i1]->refCount() <= 1) {
                manager->removeFromCache(feedbackHandles[i1]);
            }
            feedbackHandles[i1]->detach();
        }
    }

    feedbackName.clear();
    feedbackVolumePriority = 0;
}

QString MfFeedback::name() const
{
    return feedbackName;
}

void MfFeedback::emitPlay(qint64 timestamp)
{
    emit startPlay(timestamp);
}

void MfFeedback::play(qint64 timestamp)
{
    int playerCount = 0;

    if (timestamp != -1) {
        long int timeDiff = getTimestamp()-timestamp;

        if (timeDiff > MfConfig::feedbackLatencyLimit()) {
            qDebug() << "Skip the feedback because of too much latency: " <<
                    timeDiff << "usec";
            return;
        }
    }

    // Play feedbacks using non-threaded backend interfaces
    for (int i = 0; i < manager->nonThreadedBackendCount; ++i) {
        if (feedbackHandles[i].isNull() == false &&
            feedbackHandles[i]->state() == MfFeedbackHandle::Ready) {
            MfBackendBase::playbackVolume volume = MfBackendBase::VolumeMedium;
            if (feedbackVolumePriority < manager->backendVolumes.size() &&
                manager->backendVolumes[feedbackVolumePriority].contains(manager->backendList[i]->name())) {
                volume = manager->backendVolumes[feedbackVolumePriority][manager->backendList[i]->name()];
            }
            feedbackHandles[i]->play(volume);
        }
    }

    // Play the rest of the feedbacks in separate threads
    for (int i = manager->nonThreadedBackendCount; i < feedbackHandles.size(); ++i) {
        if (feedbackHandles[i].isNull() == false &&
            feedbackHandles[i]->state() == MfFeedbackHandle::Ready) {
            MfBackendBase::playbackVolume volume = MfBackendBase::VolumeMedium;
            if (feedbackVolumePriority < manager->backendVolumes.size() &&
                manager->backendVolumes[feedbackVolumePriority].contains(manager->backendList[i]->name())) {
                volume = manager->backendVolumes[feedbackVolumePriority][manager->backendList[i]->name()];
            }
            concurrentPlayers[playerCount] = QtConcurrent::run(feedbackHandles[i].data(), &MfFeedbackHandle::play, volume);
            playerCount++;
        }
    }

    // Wait until the playing has stopped to avoid concurrency problems
    // in main thread.
    for (int i = 0; i < playerCount; ++i) {
        concurrentPlayers[i].waitForFinished();
    }
}

const QList<QPointer<MfFeedbackHandle> >& MfFeedback::fbHandles()
{
    const QList<QPointer<MfFeedbackHandle> >& handles = feedbackHandles;
    return handles;
}


#ifdef MF_DEBUG
void MfFeedback::printData() const
{
    qDebug() << "Feedback:" << name();
    for (int i2 = 0; i2 < feedbackHandles.size(); ++i2) {
        if (feedbackHandles[i2].isNull() == true) {
            qDebug() << "   (NULL)";
        } else {
            if (feedbackHandles[i2]->state() == MfFeedbackHandle::Empty) {
                qDebug() << "   State: Empty   -"
                            << feedbackHandles[i2]->refCount()
                            << feedbackHandles[i2]->path();
            } else if (feedbackHandles[i2]->state() == MfFeedbackHandle::Ignored) {
                qDebug() << "   State: Ignored -"
                            << feedbackHandles[i2]->refCount()
                            << feedbackHandles[i2]->path();
            } else if (feedbackHandles[i2]->state() == MfFeedbackHandle::Ready) {
                qDebug() << "   State: Ready   -"
                            << feedbackHandles[i2]->refCount()
                            << feedbackHandles[i2]->path();
            }
        }
    }
}
#endif
