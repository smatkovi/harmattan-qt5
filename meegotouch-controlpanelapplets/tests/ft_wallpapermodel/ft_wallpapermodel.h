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
#ifndef FT_WALLPAPERBUSINESSLOGIC_H
#define FT_WALLPAPERBUSINESSLOGIC_H

#include <QtTest/QtTest>
#include <QObject>

class MApplication;
class WallpaperBusinessLogic;
class WallpaperModel;
class WallpaperDescriptor;

class SignalSink : public QObject
{
Q_OBJECT

public:
    SignalSink ();

    void reset ();
    
public slots:
    void wallpaperChanged ();

public:
    bool   m_WallpaperChangedCame;
};

class Ft_WallpaperModel : public QObject 
{
Q_OBJECT

private slots:
    void init ();
    void cleanup ();
    void initTestCase ();
    void cleanupTestCase ();

    void testRowsColumns ();
    void testData ();

private:
    bool isWallpaperDescriptorValid (WallpaperDescriptor *desc);
    
    WallpaperBusinessLogic    *m_BusinessLogic;
    WallpaperModel            *m_Model;
    MApplication              *m_App;
    SignalSink                 m_SignalSink;
};

#endif

