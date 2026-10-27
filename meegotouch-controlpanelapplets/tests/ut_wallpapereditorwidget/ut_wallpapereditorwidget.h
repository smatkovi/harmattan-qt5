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

#ifndef UT_WALLPAPEREDITORWIDGET_H
#define UT_WALLPAPEREDITORWIDGET_H

#include <QtTest/QtTest>
#include <QObject>

class WallpaperViewWidget;
class WallpaperBusinessLogic;
class WallpaperDescriptor;
class MApplication;

class Ut_WallpaperEditorWidget : public QObject 
{
Q_OBJECT
public:
    typedef enum {
        DescriptorNone,
        DescriptorCurrent,
        DescriptorNotCurrent,
    } DescriptorType;

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testCreateContentFail ();
    void test1 ();
    void test2 ();

    void testCreateContentCurrent ();
    void testCreateContentNonCurrent ();
    void testMouseEvents ();
    
private:
    void createObjects (DescriptorType descType = DescriptorNone);
    void dropObjects ();
    
    MApplication              *m_App;
    WallpaperBusinessLogic    *m_BusinessLogic;
    WallpaperViewWidget       *m_Widget;
};

#endif


