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
#ifndef TESTAPPLET_H
#define TESTAPPLET_H

#include "dcpappletif.h"
#include "testwidget.h"

#include <QObject>
#include <QPointer>
#include <DcpAppletIf>

class TestApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES(DcpAppletIf)

public:
    TestApplet ();
    ~TestApplet ();

    virtual void init();
    virtual DcpWidget *constructWidget (int widgetId);
    virtual DcpWidget *pageMain (int Id);
    virtual QString title() const;
    virtual QVector<MAction *> viewMenuItems();
    virtual DcpBrief* constructBrief(int partId);

private:
    QPointer<TestWidget> m_MainWidget;
    QPointer<TestWidget> m_Page1;
    QPointer<TestWidget> m_Page2;
    QPointer<TestWidget> m_Page3;
    QPointer<TestWidget> m_Page4;
    QPointer<TestWidget> m_Page5;
};
#endif
