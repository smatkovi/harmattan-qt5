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

#include "ut_themedescriptor.h"
#include "mdesktopentry.h"
#include "themedescriptor.h"

#include <MApplication>
#include <MTheme>

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Ut_ThemeDescriptor implementation. 
 */
void 
Ut_ThemeDescriptor::init()
{
}

void 
Ut_ThemeDescriptor::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_ThemeDescriptor";

void 
Ut_ThemeDescriptor::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    /*
     * FIXME: First I have to stub the themedescriptor somehow.
     */
}

void 
Ut_ThemeDescriptor::cleanupTestCase()
{
    m_App->deleteLater ();
}

/*!
 * This test will check a descriptor that has all the data perfectly available.
 */
void
Ut_ThemeDescriptor::testPerfectDescriptor ()
{
    ThemeDescriptor *desc, *desc1;

    desc = new ThemeDescriptor (
            NULL,
            DIRNAMEDesktopFilePerfect, 
            CODENAMEDesktopFilePerfect);
    desc1 = new ThemeDescriptor (
            NULL,
            DIRNAMEDesktopFilePerfect, 
            CODENAMEDesktopFilePerfect);

    SYS_DEBUG ("*** valid     = %s", SYS_BOOL(desc->isValid()));
    SYS_DEBUG ("*** visible   = %s", SYS_BOOL(desc->isVisible()));
    SYS_DEBUG ("*** name      = %s", SYS_STR(desc->name()));
    SYS_DEBUG ("*** codeName  = %s", SYS_STR(desc->codeName()));
    SYS_DEBUG ("*** iconName  = %s", SYS_STR(desc->iconName()));
    
    QVERIFY(desc->isValid());
    QVERIFY(desc->isVisible());
    QVERIFY(desc->name() == NAMEDesktopFilePerfect);
    QVERIFY(desc->codeName() == CODENAMEDesktopFilePerfect);
    QVERIFY(desc->iconName() == ICONDesktopFilePerfect);

    /*
     * Testing the operator== variants here.
     */
    QVERIFY(*desc == QString(CODENAMEDesktopFilePerfect));
    QVERIFY(*desc == QString(NAMEDesktopFilePerfect));
    QVERIFY(*desc == *desc1);

    delete desc;
}

/*!
 * This test will check the theme descriptor that is created from a desktop file
 * which is actually missing. More precisely we test what happens when the 
 * MDesktopEntry becomes invalid.
 */
void
Ut_ThemeDescriptor::testMissingFileDescriptor ()
{
    ThemeDescriptor *desc;

    desc = new ThemeDescriptor (
            NULL,
            DIRNAMEDesktopFileMissing, 
            CODENAMEDesktopFileMissing);

    SYS_DEBUG ("*** valid     = %s", SYS_BOOL(desc->isValid()));
    SYS_DEBUG ("*** visible   = %s", SYS_BOOL(desc->isVisible()));
    SYS_DEBUG ("*** name      = %s", SYS_STR(desc->name()));
    SYS_DEBUG ("*** codeName  = %s", SYS_STR(desc->codeName()));
    SYS_DEBUG ("*** iconName  = %s", SYS_STR(desc->iconName()));
    
    QVERIFY(!desc->isValid());
    QVERIFY(!desc->isVisible());
    QVERIFY(desc->name().isEmpty());
    QVERIFY(!desc->codeName().isEmpty());
    QVERIFY(desc->iconName().isEmpty());

    delete desc;
}

/*!
 * Testing what happens when the theme is not visible.
 */
void
Ut_ThemeDescriptor::testHiddenThemeDescriptor ()
{
    ThemeDescriptor *desc;

    desc = new ThemeDescriptor (
            NULL,
            DIRNAMEDesktopFileHidden, 
            CODENAMEDesktopFileHidden);

    SYS_DEBUG ("*** valid     = %s", SYS_BOOL(desc->isValid()));
    SYS_DEBUG ("*** visible   = %s", SYS_BOOL(desc->isVisible()));
    SYS_DEBUG ("*** name      = %s", SYS_STR(desc->name()));
    SYS_DEBUG ("*** codeName  = %s", SYS_STR(desc->codeName()));
    SYS_DEBUG ("*** iconName  = %s", SYS_STR(desc->iconName()));
    
    QVERIFY(desc->isValid());
    QVERIFY(!desc->isVisible());
    QVERIFY(desc->name() == NAMEDesktopFileHidden);
    QVERIFY(!desc->codeName().isEmpty());
    QVERIFY(desc->iconName() == ICONDesktopFileHidden);

    delete desc;
}

/*!
 * testing what happens when the MDesktopEntry does not hold the keys we expect
 * it should hold. All of the keys will return an empty string.
 */
void
Ut_ThemeDescriptor::testEmptyThemeDescriptor ()
{
    ThemeDescriptor *desc;

    desc = new ThemeDescriptor (
            NULL,
            DIRNAMEDesktopFileEmpty, 
            CODENAMEDesktopFileEmpty);

    SYS_DEBUG ("*** valid     = %s", SYS_BOOL(desc->isValid()));
    SYS_DEBUG ("*** visible   = %s", SYS_BOOL(desc->isVisible()));
    SYS_DEBUG ("*** name      = %s", SYS_STR(desc->name()));
    SYS_DEBUG ("*** codeName  = %s", SYS_STR(desc->codeName()));
    SYS_DEBUG ("*** iconName  = %s", SYS_STR(desc->iconName()));

    QVERIFY(!desc->isValid());
    QVERIFY(!desc->isVisible());
    QVERIFY(desc->name().isEmpty());
    QVERIFY(!desc->codeName().isEmpty());
    QVERIFY(desc->iconName().isEmpty());

    delete desc;
}

QTEST_APPLESS_MAIN(Ut_ThemeDescriptor)



