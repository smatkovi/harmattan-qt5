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
#include "ut_wallpaperlist.h"
#include "wallpaperlist.h"
#include "wallpaperbusinesslogic.h"
#include "wallpaperdescriptor.h"
#include "wallpapermodel.h"

#include <QMetaType>
#include <QSignalSpy>
#include <QList>
#include <QVariant>
#include <MApplication>

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Ut_WallpaperList implementation.
 */

static int argc = 1;
static char *app_name = (char*) "./ut_wallpaperlist";

void
Ut_WallpaperList::initTestCase ()
{
    qRegisterMetaType<WallpaperDescriptor>("WallpaperDescriptor");

    m_App = new MApplication (argc, &app_name);
    m_BusinessLogic = new WallpaperBusinessLogic;
}

void
Ut_WallpaperList::cleanupTestCase ()
{
    delete m_BusinessLogic;
    m_BusinessLogic = 0;
    m_App->deleteLater ();
    m_App = 0;
}

void
Ut_WallpaperList::init ()
{
    SYS_DEBUG ("");

    m_List = new WallpaperList (m_BusinessLogic);
    m_List->setDataSourceType (WallpaperList::DataSourceLocal);

    QVERIFY (m_List->m_DataSourceType == WallpaperList::DataSourceLocal);
    QVERIFY (m_List->m_Model);
}

void
Ut_WallpaperList::cleanup ()
{
    SYS_DEBUG ("");
    delete m_List;
    m_List = 0;
}

void
Ut_WallpaperList::testItemClicked ()
{
    QModelIndex  index;
    QSignalSpy   spy (m_List, SIGNAL (imageActivated (WallpaperDescriptor)));

    index = m_List->m_Model->index (0, 0);
    QVERIFY (index.isValid ());

    /*
     * During panning, click shouldn't work
     */
    m_List->slotPanningStarted ();
    m_List->slotItemClicked (index);

    QCOMPARE (spy.count (), 0);

    m_List->slotPanningStopped ();
    m_List->slotItemClicked (index);

    QCOMPARE (spy.count (), 1);
}

void
Ut_WallpaperList::testShowHide ()
{
//XXX: this is crashing :-S
#if 0
    m_List->showEvent (0);
    QTest::qWait (300);

    // FIXME: We should find a way to test the loading of the thumbnails here.
    // Some code was removed trom the tested unit, so this method became empty.
    m_List->hideEvent (0);
    // Well, maybe the stopLoadingImages should be checked too?
#endif
}

void
Ut_WallpaperList::testPanningStartStop ()
{
    m_List->slotPanningStarted ();

    QCOMPARE (m_List->m_Panning, true);

    m_List->slotPanningStopped ();

    QCOMPARE (m_List->m_Panning, false);
}

QTEST_APPLESS_MAIN(Ut_WallpaperList)

