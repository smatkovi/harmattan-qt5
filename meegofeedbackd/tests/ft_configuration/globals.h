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

#include <QMap>
#include <QString>
#include "gconf_mock.h"

/* Store pointers to the notify function and data for theme change */
extern GConfClientNotifyFunc g_gconfClientNotifyThemeFuncPtr;
extern gpointer g_gconfClientNotifyThemeUserData;

/* Store pointers to the notify function and data for volume change */
extern GConfClientNotifyFunc g_gconfClientNotifyVolumeFuncPtr;
extern gpointer g_gconfClientNotifyVolumeUserData;

/* Stores the GConfEntry list that will be returned when gconf_client_all_entries is called */
extern GSList *g_gconfClientAllEntriesList0;
extern GSList *g_gconfClientAllEntriesList1;
extern GSList *g_gconfClientAllEntriesList2;

extern QMap<QString, QString> g_lastLoadedTheme;

#endif
