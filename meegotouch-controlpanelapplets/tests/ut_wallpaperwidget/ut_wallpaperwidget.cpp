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

#include "ut_wallpaperwidget.h"
#include "wallpaperwidget.h"
#include "wallpaperbusinesslogic.h"
#include "wallpaperdescriptor.h"
#include "wallpapermodel.h"

#include <MApplication>

//#define DEBUG
#define WARNING
#include "../../src/debug.h"

/******************************************************************************
 * Ut_WallpaperWidget implementation. 
 */
void 
Ut_WallpaperWidget::init()
{
}

void 
Ut_WallpaperWidget::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_WallpaperWidget";

void 
Ut_WallpaperWidget::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_BusinessLogic = new WallpaperBusinessLogic;

    m_Widget = new WallpaperWidget (m_BusinessLogic);

    QTest::qWait (150);
    QVERIFY (m_Widget->m_ImageList);
    QVERIFY (m_Widget->m_TitleLabel);
}

void 
Ut_WallpaperWidget::cleanupTestCase()
{
    delete m_BusinessLogic;
    delete m_Widget;
    m_App->deleteLater ();
}

void 
Ut_WallpaperWidget::test0 ()
{
    /*
     * FIXME: Working on this.
     */
}


QTEST_APPLESS_MAIN(Ut_WallpaperWidget)

