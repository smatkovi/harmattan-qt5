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
#ifndef UT_WALLPAPERLIST_H
#define UT_WALLPAPERLIST_H

#include <QtTest/QtTest>
#include <QObject>

#include "wallpaperdescriptor.h"

class MApplication;
class WallpaperBusinessLogic;
class WallpaperList;
class WallpaperDescriptor;

class Ut_WallpaperList : public QObject
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testItemClicked ();
    void testPanningStartStop ();
    void testShowHide ();

private:
    MApplication              *m_App;
    WallpaperBusinessLogic    *m_BusinessLogic;
    WallpaperList             *m_List;
};

#endif


