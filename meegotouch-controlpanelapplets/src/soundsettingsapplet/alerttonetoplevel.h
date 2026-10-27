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
#ifndef _ALERT_TONE_TOPLEVEL_H_
#define _ALERT_TONE_TOPLEVEL_H_

class MAction;
#include <dcpwidget.h>
#include <meegocontrolexport.h>

class MC_EXPORT AlertToneToplevel : public DcpWidget
{
Q_OBJECT

public:
    AlertToneToplevel (QGraphicsWidget *parent = 0);

    void setTitle (QString str);
    QString title () const;

    Q_DECL_DEPRECATED void setViewMenuItems (QVector<MAction *>viewMenuItems);
    Q_DECL_DEPRECATED QVector<MAction *>viewMenuItems () const;

    void emit_changeWidget (int widgetId);

private:
    QString             m_title;
    QVector<MAction *>  m_viewMenuItems;
};

#endif

