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
#include "warrantyapplet.h"
#include "warrantywidget.h"

#include <MAction>

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

#undef DEBUG
#include "../debug.h"

Q_EXPORT_PLUGIN2(warrantyapplet, WarrantyApplet)

WarrantyApplet::WarrantyApplet()
{
}

WarrantyApplet::~WarrantyApplet() 
{
    if (!m_WarrantyBusinessLogic.isNull ())
        delete m_WarrantyBusinessLogic.data ();
}

void 
WarrantyApplet::init()
{
}

DcpStylableWidget *
WarrantyApplet::constructStylableWidget (int widgetId)
{
    if (m_WarrantyBusinessLogic.isNull ())
        m_WarrantyBusinessLogic = new WarrantyBusinessLogic;

    SYS_DEBUG ("widgetId = %d", widgetId);

    switch (widgetId) {
        case 0:
            if (m_MainWidget == 0) 
                m_MainWidget = new WarrantyWidget (m_WarrantyBusinessLogic);
            return m_MainWidget;

        default:
            SYS_WARNING ("Unknown widgetId: %d", widgetId);
    }

    return 0;
}

QString
WarrantyApplet::title() const
{
    //% "Warranty"
    return qtTrId ("qtn_warr_title");
}

QVector<MAction*>
WarrantyApplet::viewMenuItems()
{
    QVector<MAction*>   vector;

    return vector;
}

