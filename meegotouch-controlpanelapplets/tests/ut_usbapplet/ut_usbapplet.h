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
#ifndef UT_USBAPPLET_H
#define UT_USBAPPLET_H

#include <QtTest/QtTest>
#include <QObject>

class UsbApplet;
class MWindow;
class MApplication;

class Ut_UsbApplet : public QObject 
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testTitle ();
    void testConstructBrief ();
    void testConstructWidget ();
    void testMenuItems ();
    void testCurrentText ();
    void testInfoWidget ();

private:
    MApplication              *m_App;
    MWindow                   *m_window;
    UsbApplet                 *m_Applet;
};

#endif


