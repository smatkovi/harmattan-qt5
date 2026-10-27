/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
**
** This file is part of duicontrolpanel.
**
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef DCPAPPLET_H
#define DCPAPPLET_H

#include <QString>
#include <QtGlobal>

/*
 * An installed path under $DCP_ROOT, when it is set: to run the control
 * panel and its applets from a staging directory (DCP_ROOT=/path/to/stage
 * for /path/to/stage/usr/lib/duicontrolpanel, ...).
 */
inline QString dcpRooted(const QString &path)
{
    return QString::fromLocal8Bit(qgetenv("DCP_ROOT")) + path;
}

namespace DcpApplet {
    const QString DefaultPath = dcpRooted(DESKTOP_DIR) + ":" + dcpRooted(DESKTOP_DIR2);
    const QString Lib = dcpRooted(APPLET_LIBS);
    const QString MostUsedCategory = "MostUsedCategory";
    const QString OtherCategories = "OtherCategories";
    const int MaxMostUsed = 4;
};

#endif // DCPAPPLET_H

