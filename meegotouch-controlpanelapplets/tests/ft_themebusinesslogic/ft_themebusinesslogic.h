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
#ifndef FT_THEMEBUSINESSLOGIC_H
#define FT_THEMEBUSINESSLOGIC_H

#include <QtTest/QtTest>
#include <QObject>

class MApplication;
class ThemeBusinessLogic;
class ThemeDescriptor;


class FtThemeBusinessLogicPrivate : public QObject 
{
    Q_OBJECT

public slots:
    void themeChanged (QString themeCodeName);
    void themeChangeStarted (QString themeCodeName);

signals:
    void changeTheme (QString themeCodeName);

public:
    QString m_ThemeCodeNameUnderProcess;
    QString m_ThemeCodeName;

};

class Ft_ThemeBusinessLogic : public QObject 
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testCurrentTheme ();
    void testAvailableThemes ();
    void testChangeTheme ();

private:
    void checkDescriptorIsValid (ThemeDescriptor *desc);

    FtThemeBusinessLogicPrivate *m_Priv;
    MApplication              *m_App;
    ThemeBusinessLogic          *m_Api;
};

#endif
