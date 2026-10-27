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

#include "ut_wallpaperworkerthread.h"

#include <MApplication>
#include "wallpaperworkerthread.h"

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Ut_WallpaperWorkerThread implementation. 
 */
void 
Ut_WallpaperWorkerThread::init()
{
}

void 
Ut_WallpaperWorkerThread::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_WallpaperWorkerThread";

void 
Ut_WallpaperWorkerThread::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
}

void 
Ut_WallpaperWorkerThread::cleanupTestCase()
{
    m_App->deleteLater ();
}

void 
Ut_WallpaperWorkerThread::testConstructors ()
{
    QPixmap                pixmap (170, 171);
    WallpaperDescriptor    desc;
    QSize                  size (234, 345);
    QString                originalFileName ("/original.png");
    QString                outputFileName ("/output.png");
    WallpaperWorkerThread *thread;

    thread = new WallpaperWorkerThread (
            pixmap, originalFileName, outputFileName, size);
    QVERIFY (thread->m_Task == WallpaperWorkerThread::TaskSaveImage);
    QVERIFY (thread->image().width() == 170);
    QVERIFY (thread->image().height() == 171);
    QVERIFY (thread->originalFileName() == originalFileName);
    QVERIFY (thread->outputFileName() == outputFileName);
    QVERIFY (thread->m_Size == size);
    delete thread;

    thread = new WallpaperWorkerThread (
            originalFileName, outputFileName);
    QVERIFY (thread->m_Task == WallpaperWorkerThread::TaskCopyFile);
    QVERIFY (thread->originalFileName() == originalFileName);
    QVERIFY (thread->outputFileName() == outputFileName);
    delete thread;

    thread = new WallpaperWorkerThread (desc, size);
    QVERIFY (thread->m_Task == WallpaperWorkerThread::TaskLoadImage);
    QVERIFY (thread->size() == size);
    delete thread;
}

QTEST_APPLESS_MAIN(Ut_WallpaperWorkerThread)

