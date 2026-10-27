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
#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QPointer>
#include "dcpwidget.h"

class MContainer;
class MGridLayoutPolicy;

class TestWidget : public DcpWidget
{
    Q_OBJECT

public:
    TestWidget (
            int                 numId, 
            QGraphicsWidget    *parent = 0);
    ~TestWidget ();

    void retranslateUi ();

private slots:
    void pagingClicked ();
    void backClicked ();

private:
    void createWidgets ();
    MContainer *createContainer ();
    void createButtons ();

    MContainer                 *m_LocalContainer;
    MGridLayoutPolicy          *m_LocalLayoutPolicy;
    int                           m_NumId;
};

#endif
