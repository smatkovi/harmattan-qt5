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
#ifndef UT_THEMEWIDGET_H
#define UT_THEMEWIDGET_H

#include <QtTest/QtTest>
#include <QObject>
#include <QPointer>
#include "themebusinesslogic.h"
#include "themewidget.h"

class MApplication;
class ThemeWidget;


class Ut_ThemeWidget : public QObject 
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testThemeActivated ();
private:
    MApplication                 *m_App;
    QPointer<ThemeBusinessLogic>  m_ThemeBusinessLogic;
    QPointer<ThemeWidget>         m_ThemeWidget;
};

#endif
