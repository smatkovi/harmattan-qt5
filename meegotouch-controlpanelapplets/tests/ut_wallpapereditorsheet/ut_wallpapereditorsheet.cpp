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

#include "ut_wallpapereditorsheet.h"
#include "wallpaperbusinesslogic.h"
#include "wallpapereditorsheet.h"
#include "wallpaperviewwidget.h"
#include "wallpapergconf.h"

/*
 * We are reading data from this stub, we have some definition there we can use
 * in the test. This way the stub might be changed and these tests will
 * hopefully not fail.
 */
#include "mdesktopentry.h"

#include <MApplication>
#include <QList>

#define DEBUG
#include "../../src/debug.h"


/******************************************************************************
 * Ut_WallpaperEditorSheet implementation.
 */
void
Ut_WallpaperEditorSheet::init()
{
}

void
Ut_WallpaperEditorSheet::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./ut_wallpapereditorsheet";

void
Ut_WallpaperEditorSheet::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_BusinessLogic = new WallpaperBusinessLogic;
}

void
Ut_WallpaperEditorSheet::cleanupTestCase()
{
    m_App->deleteLater ();
}

void 
Ut_WallpaperEditorSheet::testConstruct ()
{
    WallpaperEditorSheet *sheet = new WallpaperEditorSheet (m_BusinessLogic);

    QVERIFY (!sheet->m_Saving);

    SYS_DEBUG ("*** doneActivated *************");
    sheet->doneActivated ();
    QVERIFY (sheet->m_Saving);

    SYS_DEBUG ("*** wallpaperSaved ************");
    sheet->wallpaperSaved ();
    QVERIFY (!sheet->m_Saving);

    SYS_DEBUG ("*** cancelActivated ***********");
    sheet->cancelActivated ();
}


QTEST_APPLESS_MAIN(Ut_WallpaperEditorSheet)

