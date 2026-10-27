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
#include "alerttonetoplevel.h"

AlertToneToplevel::AlertToneToplevel (QGraphicsWidget *parent) :
    DcpWidget (parent)
{

}

void
AlertToneToplevel::emit_changeWidget (int widgetId)
{
    emit changeWidget (widgetId);
}

void
AlertToneToplevel::setTitle (QString str)
{
    m_title = str;
}

QString
AlertToneToplevel::title () const
{
    return qtTrId (m_title.toUtf8 ().constData ());
}

/*!
 * These methods that are handling menu items are obsolete...
 */
void
AlertToneToplevel::setViewMenuItems (QVector<MAction *> viewMenuItems)
{
    m_viewMenuItems = viewMenuItems;
}

QVector<MAction *>
AlertToneToplevel::viewMenuItems () const
{
    return m_viewMenuItems;
}

