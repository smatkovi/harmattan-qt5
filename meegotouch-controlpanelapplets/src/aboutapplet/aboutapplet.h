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
#ifndef ABOUTAPPLET_H
#define ABOUTAPPLET_H

#include "aboutwidget.h"

#include <dcpappletif.h>
#include <QObject>
#include <QPointer>

class AboutWidget;

class Q_DECL_EXPORT AboutApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES (DcpAppletIf)

public:
    AboutApplet ();
    ~AboutApplet ();

    virtual void init ();
    virtual DcpStylableWidget *constructStylableWidget (int widgetId);
    virtual QString title () const;
    virtual QVector<MAction *> viewMenuItems ();

private:
    QPointer<AboutWidget>        m_MainWidget;

    #ifdef UNIT_TEST
    friend class Ut_AboutApplet;
    #endif
};
#endif
