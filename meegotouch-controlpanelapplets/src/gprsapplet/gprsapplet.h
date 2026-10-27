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
#ifndef GPRSAPPLET_H
#define GPRSAPPLET_H

#include "dcpappletif.h"
#include "gprsbusinesslogic.h"

#include <QObject>
#include <QPointer>
#include <DcpAppletIf>

class gprsWidget;

class gprsApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES(DcpAppletIf)
#if QT_VERSION >= 0x050000
    // Qt 5's plugins say what they are in the class (Q_EXPORT_PLUGIN2 is gone)
    Q_PLUGIN_METADATA(IID "com.nokia.m.core.DcpAppletIf/1.0")
#endif

public:
    gprsApplet ();
    ~gprsApplet ();

    virtual void init();
    virtual DcpWidget *constructWidget (int widgetId);
    virtual DcpWidget *pageMain (int widgetId);
    virtual QString title() const;
    virtual QVector<MAction *> viewMenuItems();
    virtual DcpBrief* constructBrief(int partId);

private:
    QPointer<gprsWidget>        m_MainWidget;
    QPointer<gprsBusinessLogic> m_gprsBusinessLogic;
    #ifdef UNIT_TEST
    friend class Ut_gprsApplet;
    #endif
};
#endif
