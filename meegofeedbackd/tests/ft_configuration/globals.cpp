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

#include "globals.h"

gpointer g_gconfClientNotifyThemeUserData = NULL;
GConfClientNotifyFunc g_gconfClientNotifyThemeFuncPtr = NULL;

gpointer g_gconfClientNotifyVolumeUserData = NULL;
GConfClientNotifyFunc g_gconfClientNotifyVolumeFuncPtr = NULL;

GSList *g_gconfClientAllEntriesList0 = NULL;
GSList *g_gconfClientAllEntriesList1 = NULL;
GSList *g_gconfClientAllEntriesList2 = NULL;

QMap<QString, QString> g_lastLoadedTheme;
