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
#include "aboutapplet.h"
#include "aboutwidget.h"
#include <MAction>

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

#include "../debug.h"

Q_EXPORT_PLUGIN2(aboutapplet, AboutApplet)

AboutApplet::AboutApplet()
{
}

AboutApplet::~AboutApplet()
{
    SysDebug::closeDebugFile ();
}

void
AboutApplet::init()
{
}

DcpStylableWidget *
AboutApplet::constructStylableWidget (int widgetId)
{
    SYS_DEBUG ("widgetId = %d", widgetId);

    switch (widgetId) {
        case 0:
            if (m_MainWidget.isNull ())
                m_MainWidget = new AboutWidget;
            return m_MainWidget;
            break;
        default:
            SYS_WARNING ("Unknown widgetId: %d", widgetId);
    }

    return 0;

}

QString
AboutApplet::title() const
{
    //% "About product"
    return qtTrId ("qtn_prod_about_product");
}

QVector<MAction *>
AboutApplet::viewMenuItems ()
{
    QVector<MAction *> vector;

    return vector;
}

