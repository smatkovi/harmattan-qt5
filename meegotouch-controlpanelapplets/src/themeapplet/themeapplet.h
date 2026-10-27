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
#ifndef THEMEAPPLET_H
#define THEMEAPPLET_H

#include "dcpappletif.h"
#include "themewidget.h"
#include "themebusinesslogic.h"

#include <QObject>
#include <QPointer>
#include <DcpAppletIf>

class ThemeApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES(DcpAppletIf)
#if QT_VERSION >= 0x050000
    // Qt 5's plugins say what they are in the class (Q_EXPORT_PLUGIN2 is gone)
    Q_PLUGIN_METADATA(IID "com.nokia.m.core.DcpAppletIf/1.0")
#endif

public:
    ThemeApplet ();
    ~ThemeApplet ();

    virtual void init();
    virtual DcpWidget *constructWidget (int widgetId);
    virtual DcpWidget *pageMain ();
    virtual QString title() const;
    virtual QVector<MAction *> viewMenuItems();
    virtual DcpBrief* constructBrief(int partId);

private:
    QPointer<ThemeWidget> m_MainWidget;
    QPointer<ThemeBusinessLogic> m_ThemeBusinessLogic;
    #ifdef UNIT_TEST
    friend class Ut_ThemeApplet;
    #endif
};
#endif
