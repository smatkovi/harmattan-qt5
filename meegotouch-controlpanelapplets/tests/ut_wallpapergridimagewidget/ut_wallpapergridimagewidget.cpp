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

#include "ut_wallpapergridimagewidget.h"

#include <MApplication>

#include "gridimagewidget.h"

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Ut_WallpaperGridImageWidget implementation. 
 */
void 
Ut_WallpaperGridImageWidget::init()
{
}

void 
Ut_WallpaperGridImageWidget::cleanup()
{
}

static int argc = 1;
static char *app_name = (char*) "./Ut_WallpaperGridImageWidget";

void 
Ut_WallpaperGridImageWidget::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
}

void 
Ut_WallpaperGridImageWidget::cleanupTestCase()
{
    m_App->deleteLater ();
}

void
Ut_WallpaperGridImageWidget::testID ()
{
    GridImageWidget *widget = new GridImageWidget ();

    QVERIFY (widget->id().isEmpty());

    widget->setID ("testID");
    QVERIFY (widget->id() == "testID");

    delete widget;
}
    
void
Ut_WallpaperGridImageWidget::testProgress ()
{
    GridImageWidget *widget = new GridImageWidget ();

    QVERIFY (widget->m_ProgressIndicator == 0);
    widget->setProgress();
    QVERIFY (widget->m_ProgressIndicator != 0);
    
    delete widget;
}


QTEST_APPLESS_MAIN(Ut_WallpaperGridImageWidget)

