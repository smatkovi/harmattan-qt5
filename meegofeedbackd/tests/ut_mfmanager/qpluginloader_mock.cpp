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

#include <QPluginLoader>

#include "mfbackendfoo.h"
#include "mfsourcefoo.h"
#include "globals.h"

QPluginLoader::QPluginLoader(QObject *parent)
    : QObject(parent)
{
    m_instance = NULL;
    m_isLoaded = false;
}

QPluginLoader::QPluginLoader(const QString &fileName, QObject *parent)
    : QObject(parent)
{
    m_instance = NULL;
    m_fileName = fileName;
    m_isLoaded = false;
}

QPluginLoader::~QPluginLoader()
{
    if (m_isLoaded) {
        unload();
    }
}

QObject *QPluginLoader::instance()
{
    if (!m_isLoaded) {
        this->load();
    }

    return m_instance;
}

QObjectList QPluginLoader::staticInstances()
{
    return QObjectList();
}

bool QPluginLoader::load()
{
    bool ok = true;

    if (m_fileName == "/usr/lib/meegofeedbackd/threaded-backends/libmeegofeedback-nohw.so") {
        if (!g_loadedInstances.contains(m_fileName)) {
            // First creation of this backend
            m_instance = new MfBackendFoo(false);
        } else {
            // This backend has been instantiated before
            m_instance = g_threadedBackendFooInstances[0];
        }
        g_threadedBackendFooInstances.append((MfBackendFoo*)m_instance);
        g_loadedInstances.append(m_fileName);
    } else if (m_fileName == "/usr/lib/meegofeedbackd/threaded-backends/libmeegofeedback-befoo1.so" ||
        m_fileName == "/usr/lib/meegofeedbackd/threaded-backends/libmeegofeedback-befoo2.so") {
        if (!g_loadedInstances.contains(m_fileName)) {
            // First creation of this backend
            m_instance = new MfBackendFoo();
        } else {
            // This backend has been instantiated before
            m_instance = g_threadedBackendFooInstances[0];
        }
        g_threadedBackendFooInstances.append((MfBackendFoo*)m_instance);
        g_loadedInstances.append(m_fileName);
    } else if (m_fileName == "/usr/lib/meegofeedbackd/non-threaded-backends/libmeegofeedback-befoo1.so" ||
               m_fileName == "/usr/lib/meegofeedbackd/non-threaded-backends/libmeegofeedback-befoo2.so") {
        if (!g_loadedInstances.contains(m_fileName)) {
            // First creation of this backend
            m_instance = new MfBackendFoo();
        } else {
            // This backend has been instantiated before
            m_instance = g_nonThreadedBackendFooInstances[0];
        }
        g_nonThreadedBackendFooInstances.append((MfBackendFoo*)m_instance);
        g_loadedInstances.append(m_fileName);
    } else if (m_fileName == "/usr/lib/meegofeedbackd/sources/libmeegofeedback-srcfoo.so") {
        if (!g_loadedInstances.contains(m_fileName)) {
            // First creation of this backend
            m_instance = new MfSourceFoo();
        } else {
            // This backend has been instanciated before
            m_instance = g_sourceFooInstances[0];
        }
        g_sourceFooInstances.append((MfSourceFoo*)m_instance);
        g_loadedInstances.append(m_fileName);
    } else {
        ok = false;
    }

    return ok;
}

bool QPluginLoader::unload()
{
    if (m_instance)
    {
        // g_loadedInstances is used as a reference count of created instances.
        // Instance will be removed only when it is the last existing instance.
        if (g_loadedInstances.count(m_fileName) <= 1) {
            delete m_instance;
        }
        if (g_loadedInstances.contains(m_fileName)) {
            g_loadedInstances.removeAt(g_loadedInstances.indexOf(m_fileName));
        }
        m_instance = 0;
        m_isLoaded = false;
    }
    return true;
}

bool QPluginLoader::isLoaded() const
{
    return m_isLoaded;
}

void QPluginLoader::setFileName(const QString &fileName)
{
    m_fileName = fileName;
}

QString QPluginLoader::fileName() const
{
    return m_fileName;
}

QString QPluginLoader::errorString() const
{
    return "Mock plugin loader error string.";
}

void QPluginLoader::setLoadHints(QLibrary::LoadHints loadHints)
{
    Q_UNUSED(loadHints);
}

QLibrary::LoadHints QPluginLoader::loadHints() const
{
    return QLibrary::ResolveAllSymbolsHint;
}
