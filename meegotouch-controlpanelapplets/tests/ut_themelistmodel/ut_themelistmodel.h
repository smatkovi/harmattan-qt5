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
#ifndef UT_THEMELISTMODEL_H
#define UT_THEMELISTMODEL_H

#include <QtTest/QtTest>
#include <QObject>
#include <QList>
#include <QModelIndex>

#include <themedescriptor.h>

class MApplication;
class ThemeBusinessLogic;
class ThemeListModel;

class UtThemeListModelPrivate : public QObject 
{
    Q_OBJECT

public:
    UtThemeListModelPrivate ();
    void reset ();

public slots:
    void dataChanged (const QModelIndex &first, const QModelIndex &last);

public:
    QModelIndex m_FirstChanged;
    QModelIndex m_LastChanged;
};

/*
 * There is a new signal notifyes us about the theme change being complete. The
 * signal sent by MTheme and should be simulated in this unit test.
 */
class Ut_ThemeListModel : public QObject 
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testAvailableThemes ();
    void testRefresh ();
private:
    UtThemeListModelPrivate     *m_Priv;
    MApplication                *m_App;
    ThemeBusinessLogic          *m_Logic;
    ThemeListModel              *m_Api;
};

#endif

