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
#ifndef UT_THEMEBUSINESSLOGIC_H
#define UT_THEMEBUSINESSLOGIC_H

#include <QtTest/QtTest>
#include <QObject>
#include <QList>

#include <themedescriptor.h>

class MApplication;
class ThemeBusinessLogic;

class UtThemeBusinessLogicPrivate : public QObject 
{
    Q_OBJECT

public:
    UtThemeBusinessLogicPrivate ();
    void reset ();

public slots:
    void themeChanged (QString themeCodeName);
    void themeChangeStarted (QString themeCodeName);

    void themeAboutToBeRemoved (int index);
    void themeRemoved (QList<ThemeDescriptor *> list);
    void themeAboutToBeAdded (int index);
    void themeAdded (QList<ThemeDescriptor *> list);

signals:
    void changeTheme (QString themeCodeName);

public:
    QString m_ThemeCodeNameUnderProcess;
    QString m_ThemeCodeName;
    bool    m_ThemeAboutToBeAdded;
    bool    m_ThemeAdded;
    bool    m_ThemeAboutToBeRemoved;
    bool    m_ThemeRemoved;
};

/*
 * There is a new signal notifyes us about the theme change being complete. The
 * signal sent by MTheme and should be simulated in this unit test.
 */
class Ut_ThemeBusinessLogic : public QObject 
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testCurrentTheme ();
    void testAvailableThemes ();
    void testSetTheme ();
    
private:
    UtThemeBusinessLogicPrivate *m_Priv;
    MApplication                *m_App;
    ThemeBusinessLogic          *m_Api;
};

#endif
