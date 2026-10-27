/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
**
** If you have questions regarding the use of this file, please contact
** Nokia at directui@nokia.com.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/
#ifndef _ALERT_TONE_APPLET_MAPS_H_
#define _ALERT_TONE_APPLET_MAPS_H_

#include <QString>
#include <QVariant>

namespace AlertToneAppletMaps
{
    QVariant mapToUiString (const QString &str);
    QVariant mapToUiString (int integer);
// these are only giving logical ids:
    const char *map (const QString &str);
    const char *map (const int &i);
};

#endif
