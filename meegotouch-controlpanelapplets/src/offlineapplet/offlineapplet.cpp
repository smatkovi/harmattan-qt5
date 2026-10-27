/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
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

#include <QtGui>
#include <DcpWidget>
#include "offlineapplet.h"
#include "offlinebrief.h"

Q_EXPORT_PLUGIN2(offlineapplet, OfflineApplet)

void OfflineApplet::init()
{
};

DcpWidget* OfflineApplet::constructWidget(int widgetId)
{
    // No views for button-like applets by default
    Q_UNUSED(widgetId);
    return 0;
}

QString OfflineApplet::title() const
{
    return QString();
}

QVector<MAction*> OfflineApplet::viewMenuItems()
{
    QVector<MAction*> vector;
    return vector;
}

DcpBrief* OfflineApplet::constructBrief(int)
{
    return new OfflineBrief();
}

