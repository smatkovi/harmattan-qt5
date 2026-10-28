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

#ifndef GLOBALS_H
#define GLOBALS_H

#include <QList>
#include <QPointer>
#include <QStringList>

#include "mfsourcefoo.h"
#include "mfbackendfoo.h"

extern QList< QPointer<MfBackendFoo> > g_threadedBackendFooInstances;
extern QList< QPointer<MfBackendFoo> > g_nonThreadedBackendFooInstances;
extern QList< QPointer<MfSourceFoo> > g_sourceFooInstances;

// What files are present in meegofeedbackd's lib dirs
extern QStringList g_threadedBackendsDirectoryEntryList;
extern QStringList g_nonThreadedBackendsDirectoryEntryList;
extern QStringList g_sourcesDirectoryEntryList;

// Used by mocked QPluginLoader to keep track of loaded instances
extern QStringList g_loadedInstances;
#endif
