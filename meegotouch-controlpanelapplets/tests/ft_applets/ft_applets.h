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
#ifndef APPLET_LOADER_H
#define APPLET_LOADER_H

#include <QObject>
#include <QtTest/QtTest>

class DcpAppletIf;
class MApplication;
class MApplicationWindow;
class MApplicationPage;

class Ft_Applets : public QObject
{
    Q_OBJECT

public:
    enum view {
        NoView = 0,
        HaveView,
        HaveStylableView
    };

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testsoundsettingsapplet ();
    void testthemeapplet ();
    void testbatteryapplet ();
    void testdisplayapplet ();
    void testusbapplet ();
    void testresetapplet ();
    void testaboutapplet ();
    void testwallpaperapplet ();
    void testwarrantyapplet ();
    void testofflineapplet ();

private:
    void doAppletTest (const char *soName,
                       bool hasBrief = true,
                       view viewType = HaveView);
    void doViewTest (DcpAppletIf *applet,
                     bool stylableView = false);

private:
    MApplication            *m_app;
    MApplicationWindow      *m_window;
};

#endif
