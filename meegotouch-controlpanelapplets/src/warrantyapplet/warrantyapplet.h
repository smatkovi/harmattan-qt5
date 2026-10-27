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
#ifndef WARRANTYAPPLET_H
#define WARRANTYAPPLET_H

#include "dcpappletif.h"
#include "warrantybusinesslogic.h"

#include <QObject>
#include <QPointer>
#include <DcpAppletIf>

class WarrantyWidget;

class Q_DECL_EXPORT WarrantyApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES (DcpAppletIf)

public:
    WarrantyApplet ();
    ~WarrantyApplet ();

    virtual void init ();
    virtual DcpStylableWidget *constructStylableWidget (int widgetId);
    virtual QString title () const;
    virtual QVector<MAction *> viewMenuItems ();

private:
    QPointer<WarrantyWidget>        m_MainWidget;
    QPointer<WarrantyBusinessLogic> m_WarrantyBusinessLogic;
    #ifdef UNIT_TEST
    friend class Ut_WarrantyApplet;
    #endif
};

#endif
