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
#include <MApplication>
#include <MApplicationWindow>
#include <QString>
#include <QStringList>
#include "ut_soundsettingsapplet.h"
#include "soundsettingsapplet.h"
#include "alerttonebrowserstylable.h"
#include "alerttoneappletwidget.h"
#include <typeinfo>

/******************************************************************************
 * Ut_SoundSettingsApplet implementation.
 */
int   argc = 1;
char *argv[] = {
    (char *) "./ut_soundsettingsapplet",
    NULL };

void
Ut_SoundSettingsAppletTests::initTestCase ()
{
    m_App = new MApplication (argc, argv);
}

void
Ut_SoundSettingsAppletTests::cleanupTestCase ()
{
    delete m_App;
    m_App = 0;
}

void
Ut_SoundSettingsAppletTests::soundsettingsappletConstructor ()
{
    MApplicationWindow      aw;
    SoundSettingsApplet     ssa;
}

void
Ut_SoundSettingsAppletTests::soundsettingsappletconstructWidget ()
{
    DcpStylableWidget * newWidget;
    SoundSettingsApplet  ssa;
    ssa.init();

    newWidget = ssa.constructStylableWidget (3)  ;
    QVERIFY ( typeid (AlertToneAppletWidget) == typeid( * newWidget ) );

    newWidget = ssa.constructStylableWidget (65536);
    QVERIFY ( typeid (AlertToneBrowserStylable) == typeid( * newWidget) );

    AlertToneBrowserStylable *aWidget =
        qobject_cast<AlertToneBrowserStylable *> (newWidget);
    QVERIFY (aWidget);

    // for more coverage
    QCOMPARE (aWidget->pagePans (), true);
    QVERIFY (! aWidget->title ().isNull ());

    QCOMPARE( ssa.m_stack.size() , 2 );
}

void
Ut_SoundSettingsAppletTests::soundsettingsappletBrief ()
{
    SoundSettingsApplet applet;

    applet.init ();

    QCOMPARE ((int) applet.constructBrief (0), 0);
}

void
Ut_SoundSettingsAppletTests::soundsettingsappletinit()
{
    MApplicationWindow aw;
    SoundSettingsApplet  ssa;
    ssa.init();

    QVERIFY(ssa.m_alertTones.size() != 0 );
}

void
Ut_SoundSettingsAppletTests::soundsettingsapplettitle()
{

    SoundSettingsApplet  ssa;
    ssa.init();
    QCOMPARE( ssa.title(), QString ("qtn_sond_sounds")  );
}

void
Ut_SoundSettingsAppletTests::soundsettingsappletviewMenuItems ()
{
    MApplicationWindow aw;
    SoundSettingsApplet  ssa;

    ssa.init();
    ssa.constructStylableWidget (3)  ;
     //ssa.viewMenuItems().size();
}

void Ut_SoundSettingsAppletTests::soundsettingsappletToplevelDestroyed()
{
    DcpStylableWidget * newWidget;
    SoundSettingsApplet  ssa;
    ssa.init();

    newWidget = ssa.constructStylableWidget (3);
    newWidget = ssa.constructStylableWidget (65536);

    QCOMPARE( ssa.m_stack.size() , 2 );
    delete  ssa.m_stack.at(0);
    QCOMPARE( ssa.m_stack.size() , 2 );
    delete  ssa.m_stack.at(1);
    QCOMPARE( ssa.m_stack.size() , 1 );
}

QTEST_APPLESS_MAIN(Ut_SoundSettingsAppletTests)
