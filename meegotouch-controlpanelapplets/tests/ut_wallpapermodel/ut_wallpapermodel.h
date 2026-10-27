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

#ifndef UT_WALLPAPERMODEL_H
#define UT_WALLPAPERMODEL_H

#include <QtTest/QtTest>
#include <QObject>

class MApplication;
class WallpaperBusinessLogic;
class WallpaperModel;
class WallpaperDescriptor;

class WallpaperModelSignals : public QObject 
{
Q_OBJECT

    public slots:
        void currentChanged (const QModelIndex &current);
};

class Ut_WallpaperModel : public QObject 
{
Q_OBJECT

    private slots:
        void init ();
        void cleanup ();
        void initTestCase ();
        void cleanupTestCase ();

        void testData ();
        void testSelectByFilepath ();
    private:
        MApplication              *m_App;
        WallpaperBusinessLogic    *m_BusinessLogic;
        WallpaperModel            *m_Model;
        WallpaperModelSignals      m_Signals;
};

#endif

