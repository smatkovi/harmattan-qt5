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

#include "mfmanager.h"

#include <QDir>
#include <QThreadPool>

#include "mfbackendbase.h"
#include "mfconfig.h"
#include "mffeedback.h"
#include "mffeedbackhandle.h"
#include "mflog.h"
#include "mfsourcebase.h"

// TODO: Take directory from header file made at configuration time?
namespace {
    const char* const gLibDirPrefix = "/usr/lib/meegofeedbackd";
    const char* const gThreadedBackendsDir = "/threaded-backends";
    const char* const gNonThreadedBackendsDir = "/non-threaded-backends";
    const char* const gSourcesDir = "/sources";
    const int maxThreadedBackendCount = 2;
}

static MfManager *gInstance = NULL;

MfManager::MfManager(QObject *parent)
    : QObject(parent), nonThreadedBackendCount(0)
{
    if (gInstance) {
        qCritical("MfManager: Creating an instance but another one already exists");
        delete gInstance;
    }

    gInstance = this;
}

MfManager::~MfManager()
{
    deleteCache();

    // Unload the sources and delete QPluginLoaders. Note that
    // QPluginLoader will delete the source instances when unloading.
    for (int i = 0; i < sourceLoaderList.size(); ++i) {
        sourceLoaderList[i]->unload();
        delete sourceLoaderList[i];
        sourceLoaderList[i] = NULL;
    }
    sourceList.clear();

    // Unload the backends and delete QPluginLoaders. Note that
    // QPluginLoader will delete the backend instances when unloading.
    for (int i = 0; i < backendLoaderList.size(); ++i) {
        backendList[i]->cleanup();
        backendLoaderList[i]->unload();
        delete backendLoaderList[i];
        backendLoaderList[i] = NULL;
    }
    backendList.clear();

    gInstance = NULL;
}

MfManager *MfManager::instance()
{
    return gInstance;
}

bool MfManager::init(const QString &theme,
                     const QList<QMap<QString, MfBackendBase::playbackVolume> > &volumes,
                     const QMap<QString, QString> &deviceState)
{
    bool ok = true;
    QString pluginPath;
    QThreadPool *globalThreadPool = QThreadPool::globalInstance();

    // Store the current theme name
    currentThemeName = theme;

    // Load non-threaded backend plugins
    pluginPath = gLibDirPrefix;
    pluginPath.append(gNonThreadedBackendsDir);
    loadBackendPlugins(pluginPath);

    // Store the number of non-threaded backends for later use
    nonThreadedBackendCount = backendList.size();

    // Load threaded backend plugins
    pluginPath = gLibDirPrefix;
    pluginPath.append(gThreadedBackendsDir);
    loadBackendPlugins(pluginPath);

    if (backendList.size() == 0) {
        qCritical("MfManager: No backend library found!");
        return false;
    }

    // Load source plugins
    pluginPath = gLibDirPrefix;
    pluginPath.append(gSourcesDir);
    loadSourcePlugins(pluginPath);

    if (sourceList.size() == 0) {
        qCritical("MfManager: No source library found!");
    }

    // Adjust amount of QThreads in QThreadPool to match the amount of
    // threaded backend but no more than MFMANAGER_MAX_THREADS.
    if (globalThreadPool) {
        int targetThreadCount = qBound(0, backendList.size() - nonThreadedBackendCount, maxThreadedBackendCount);

        if (globalThreadPool->maxThreadCount() < targetThreadCount) {
            globalThreadPool->setMaxThreadCount(targetThreadCount);
        }
    }

    // Set initial volumes for the backends
    backendVolumes = volumes;

    // Inform source plugins about further device state changes
    for (int i = 0; i < sourceList.size(); ++i) {
        sourceList[i]->connect(this, SIGNAL(deviceStateChanged(QMap<QString, QString>)),
                               SLOT(deviceStateChanged(QMap<QString, QString>)), Qt::QueuedConnection);
    }

    // Tell initial device state to the source plugins
    for (int i = 0; i < sourceList.size(); ++i) {
        sourceList[i]->deviceStateChanged(deviceState);
    }

    // Listen to the lost/established connections and device state
    for (int i = 0; i < backendList.size(); ++i) {
        connect(backendList[i], SIGNAL(lostConnection()), this, SLOT(backendDisconnected()),
                // We must use queued connection to be sure about the sender when the signal
                // is received.
                Qt::QueuedConnection);
        connect(backendList[i], SIGNAL(establishedConnection()), this, SLOT(backendConnected()),
                // We must use queued connection to be sure about the sender when the signal
                // is received.
                Qt::QueuedConnection);

        // Inform backends of changed device state.
        backendList[i]->connect(this, SIGNAL(deviceStateChanged(QMap<QString, QString>)),
                                SLOT(deviceStateChanged(QMap<QString, QString>)), Qt::QueuedConnection);
    }

    // Tell initial device state to the backend plugins
    for (int i = 0; i < backendList.size(); ++i) {
        backendList[i]->deviceStateChanged(deviceState);
    }

    return ok;
}

void MfManager::loadSourcePlugins(const QString &libDir)
{
    QDir dir;
    QStringList fileList;
    QString fileName;
    QPluginLoader *pluginLoader;
    QObject *rootComponent = 0;
    MfSourceBase *source = 0;

    dir.setPath(libDir);

    if (!dir.exists()) {
        qCritical() << "MfManager: Lib directory" << libDir << "doesn't exist.";
        return;
    }

    fileList = dir.entryList(QDir::Files);

    for (int i = 0; i < fileList.size(); i++) {
        fileName = fileList[i];

        // Search source libraries with a pattern
        if (fileName.startsWith(QLatin1String("libmeegofeedback-")) && fileName.endsWith(QLatin1String(".so"))) {
            source = 0;
            QString filePath = dir.filePath(fileName);

            qDebug() << "MfManager: found source library file" << fileName;

            pluginLoader = new QPluginLoader(filePath, this);

            rootComponent = pluginLoader->instance();
            if (!rootComponent) {
                qWarning() << "MfManager: Unable to instantiate source plugin" << filePath;
                if (!pluginLoader->errorString().isEmpty()) {
                    qWarning() << "Detailed error message:" << pluginLoader->errorString();
                }
                goto FAILED;
            }

            source = qobject_cast<MfSourceBase*>(rootComponent);

            if (!source) {
                qWarning() << "MfManager:"
                           << filePath << "does not implement MfSourceInterface.";
                goto FAILED;
            }

            // Make sure the same source doesn't get loaded more than
            // once. It is possible to load the same plugin more than
            // once if symlinks are used. In this case QPluginLoader
            // will return the same instance that was loaded before. We
            // don't want that here.
            for (int i2 = 0; i2 < sourceList.size(); ++i2) {
                if (source == sourceList[i2]) {
                    qWarning() << "MfManager: Source library"
                               << filePath << "already loaded.";
                    goto FAILED;
                }
            }

            if (source && !source->init()) {
                qWarning() << "MfManager:" << filePath << "failed to initialize.";
                goto FAILED;
            }

            mfLog() << "Loaded source plugin" << filePath;

            // Add the source to the source list
            if (source) {
                sourceList << source;
            }

            // Add the plugin loader instance to source loader list
            if (pluginLoader) {
                sourceLoaderList << pluginLoader;
            }

            // If we reached this point, everything went just fine.
            continue;

        FAILED:
            // Calling unload() will delete the instanciated source
            // if the source hasn't been loaded before.
            if (pluginLoader) {
                if (pluginLoader->isLoaded()) {
                    pluginLoader->unload();
                }
                delete pluginLoader;
                pluginLoader = NULL;
            }
        }
    }
}

void MfManager::loadBackendPlugins(const QString &libDir)
{
    QDir dir;
    QStringList fileList;
    QString fileName;
    QPluginLoader *pluginLoader;
    QObject *rootComponent = 0;
    MfBackendBase *backend = 0;

    dir.setPath(libDir);

    if (!dir.exists()) {
        qCritical() << "MfManager: Lib directory" << libDir << "doesn't exist.";
        return;
    }

    fileList = dir.entryList(QDir::Files, QDir::Name);

    for (int i1 = 0; i1 < fileList.size(); i1++) {
        fileName = fileList[i1];

        // Search source or backend libraries with a pattern
        if (fileName.contains(QLatin1String("libmeegofeedback-")) && fileName.endsWith(QLatin1String(".so"))) {
            backend = NULL;
            QString filePath = dir.filePath(fileName);

            qDebug() << "MfManager: found backend library file" << fileName;

            pluginLoader = new QPluginLoader(filePath, this);

            rootComponent = pluginLoader->instance();
            if (!rootComponent) {
                qWarning() << "MfManager: Unable to instantiate backend plugin" << filePath;
                if (!pluginLoader->errorString().isEmpty()) {
                    qWarning() << "Detailed error message:" << pluginLoader->errorString();
                }
                goto FAILED;
            }

            backend = qobject_cast<MfBackendBase*>(rootComponent);

            if (!backend) {
                qWarning() << "MfManager:"
                           << filePath << "does not implement MfBackendBase.";
                goto FAILED;
            }
            if (!backend->detectHardware())
            {
                qWarning() << "MfManager: The hardware is not detected for the plugin"
                           << filePath << " -> Unable to load it.";
                goto FAILED;
            }

            // Make sure the same backend doesn't get loaded more than
            // once. It is possible to load the same plugin more than
            // once if symlinks are used. In this case QPluginLoader
            // will return the same instance that was loaded before. We
            // don't want that here.
            for (int i2 = 0; i2 < backendList.size(); ++i2) {
                if (backend == backendList[i2]) {
                    qWarning() << "MfManager: Backend library"
                               << filePath << "already loaded.";
                    goto FAILED;
                }
            }

            if (backend) {
                backend->connecting();
            }

            mfLog() << "Loaded backend plugin" << filePath;

            // Add the backend to the backend list
            if (backend) {
                backendList << backend;
            }

            // Add the plugin loader instance to backend loader list
            if (pluginLoader) {
                backendLoaderList << pluginLoader;
            }

            // If we reached that point, everything went just fine.
            continue;

        FAILED:
            // Calling unload() will delete the instantiated backend
            // if the backend hasn't been loaded before.
            if (pluginLoader) {
                if (pluginLoader->isLoaded()) {
                    pluginLoader->unload();
                }
                delete pluginLoader;
                pluginLoader = NULL;
            }
        }
    }
}

QPointer<MfFeedbackHandle> MfManager::searchFromCache(const QString &feedbackDir, const int &backendInterfaceIndex)
{
    QString keyValue;

    // Construct unique key for the handle
    keyValue = feedbackDir;
    keyValue.remove(THEME_DIR_PREFIX);
    keyValue += '#';
    keyValue += QString::number(backendInterfaceIndex, 16);

    return feedbackHandleHash.value(keyValue, 0);
}

void MfManager::addToCache(MfFeedbackHandle* feedbackHandle)
{
    QString keyValue;

    if (feedbackHandle != NULL) {
        // Construct unique key for the handle
        keyValue = feedbackHandle->path();
        keyValue.remove(THEME_DIR_PREFIX);
        keyValue += '#';
        keyValue += QString::number(backendList.indexOf(feedbackHandle->backend()), 16);

        // Add handle to cache
        if (feedbackHandleHash.contains(keyValue) == false) {
            feedbackHandleHash.insert(keyValue, feedbackHandle);
        } else {
            qCritical() << "MfManager: MfFeedbackHandle with key value" << keyValue << "already in cache!";
        }
    }
}

void MfManager::removeFromCache(MfFeedbackHandle* feedbackHandle)
{
    QString keyValue;

    if (feedbackHandle != NULL) {
        // Construct unique key for the handle
        keyValue = feedbackHandle->path();
        keyValue.remove(THEME_DIR_PREFIX);
        keyValue += '#';
        keyValue += QString::number(backendList.indexOf(feedbackHandle->backend()), 16);

        // Remove handle from cache
        feedbackHandleHash.remove(keyValue);
    }
}

void MfManager::deleteCache()
{
    QHash<QString, QPointer<MfFeedbackHandle> >::iterator cacheIter = feedbackHandleHash.begin();
    while (cacheIter != feedbackHandleHash.end()) {
        delete (*cacheIter);
        cacheIter = feedbackHandleHash.erase(cacheIter);
    }
}

void MfManager::loadTheme(const QString& themeName)
{
    // Store the current theme name
    currentThemeName = themeName;
    emit themeChanged(themeName);
}

void MfManager::setVolume(int volumeId, const QString& backendName, MfBackendBase::playbackVolume volume)
{
    if (volumeId < backendVolumes.size()) {
        backendVolumes[volumeId][backendName] = volume;
    }
}

void MfManager::backendConnected()
{
    MfBackendBase* backend = qobject_cast<MfBackendBase*>(sender());

    if (!backend)
        return;

    loadHandlesForABackend(backend);
}

void MfManager::backendDisconnected()
{
    MfBackendBase* backend = qobject_cast<MfBackendBase*>(sender());

    if (!backend)
        return;

    unloadHandlesForABackend(backend);
}

void MfManager::loadHandlesForABackend(MfBackendBase* backend)
{
    if (!backend)
        return;

    emit themeChanged(currentThemeName);
}

void MfManager::unloadHandlesForABackend(MfBackendBase* backend)
{
    if (!backend)
        return;

    qDebug() << "Delete handles for backend" << backend->name() << "from the cache...";
    QHash<QString, QPointer<MfFeedbackHandle> >::iterator cacheIter = feedbackHandleHash.begin();
    while (cacheIter != feedbackHandleHash.end()) {
        // Delete handles associated with a specific backend
        if (!(*cacheIter).isNull() && (*cacheIter)->backend() == backend)
        {
            (*cacheIter)->unload();
            delete (*cacheIter);
            cacheIter = feedbackHandleHash.erase(cacheIter);
        } else {
            cacheIter++;
        }
    }
}

#ifdef MF_DEBUG
void MfManager::printCache()
{
    int nullPointerCount = 0;
    QList<QPointer<MfFeedbackHandle> > cachedHandles;

    qDebug() << "========================\nCache content (total" << feedbackHandleHash.size()
             << "cached):\nNumber of backends:"
                << backendList.size();

    cachedHandles = feedbackHandleHash.values();

    for (int i1 = 0; i1 < backendList.size(); ++i1) {
        qDebug() << "Cached handles for backend"
                    << backendList[i1]->name();

        // Iterate through the cached handle list and remove every NULL
        // pointer (keep count) and find every handle that is cached
        // for this particular backend. Remove every found pointer
        // to increase performance a bit in later runs.
        QList<QPointer<MfFeedbackHandle> >::iterator handlePointerIter;
        for (handlePointerIter = cachedHandles.begin();
             handlePointerIter != cachedHandles.end(); ++handlePointerIter)
        {
            MfFeedbackHandle *feedbackHandle = 0;
            if ((*handlePointerIter).isNull() == true) {
                nullPointerCount++;
                handlePointerIter = cachedHandles.erase(handlePointerIter);
                handlePointerIter--;
            } else {
                if (backendList.indexOf((*handlePointerIter)->backend()) == i1) {
                    feedbackHandle = (*handlePointerIter);
                    handlePointerIter = cachedHandles.erase(handlePointerIter);
                    handlePointerIter--;
                }
            }

            // Found a cached handle for this backend, print information
            if (feedbackHandle != 0) {
                if (feedbackHandle->state() == MfFeedbackHandle::Empty) {
                    qDebug() << "   State: Empty   -"
                                << feedbackHandle->refCount()
                                << feedbackHandle->path();
                } else if (feedbackHandle->state() == MfFeedbackHandle::Ignored) {
                    qDebug() << "   State: Ignored -"
                                << feedbackHandle->refCount()
                                << feedbackHandle->path();
                } else if (feedbackHandle->state() == MfFeedbackHandle::Ready) {
                    qDebug() << "   State: Ready   -"
                                << feedbackHandle->refCount()
                                << feedbackHandle->path();
                }
            }
        }
        qDebug() << "\n";
    }

    qDebug() << "In addition there were" << nullPointerCount << "of NULL handles in cache";
}
#endif
