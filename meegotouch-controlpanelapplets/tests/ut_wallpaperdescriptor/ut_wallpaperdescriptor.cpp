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

#include "ut_wallpaperdescriptor.h"

#include "wallpaperdescriptor.h"
#include "wallpaperitrans.h"

#include <QPixmap>
#include <QUrl>
#include <QString>

#include <MApplication>
#include <MGConfItem>

#define DEBUG
#define WARNING
#include "../../src/debug.h"

static const int ThumbnailWidth  = 170;
static const int ThumbnailHeight = 171;
static const QString fileToLoad ("a file to load.png");

/******************************************************************************
 * QuillImageFilter stub.
 */
#include <QuillImageFilter>
QuillImage 
QuillImageFilter::apply (
        const QuillImage &image) const
{
    SYS_DEBUG ("image %dx%d", image.width(), image.height());

    return image;
}

QSize 
QuillImageFilter::newFullImageSize (
        const QSize &fullImageSize) const
{
    return QSize(1024, 1025);
}

/******************************************************************************
 * Ut_WallpaperDescriptor implementation. 
 */
void 
Ut_WallpaperDescriptor::init()
{
}

void 
Ut_WallpaperDescriptor::cleanup()
{
}


static int argc = 1;
static char* app_name = (char*) "./Ut_WallpaperDescriptor";

void 
Ut_WallpaperDescriptor::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_Desc = 0;
}

void 
Ut_WallpaperDescriptor::cleanupTestCase()
{
    if (m_Desc)
        delete m_Desc;

    m_App->deleteLater ();
}

/*!
 * Checking if the newly created descriptor has the proper default values.
 */
void 
Ut_WallpaperDescriptor::testDefaults ()
{
    WallpaperDescriptor desc;

    QVERIFY (desc.filePath().isEmpty());
    QVERIFY (desc.originalFilePath().isEmpty());
    QVERIFY (!desc.thumbnailPending());
    QVERIFY (!desc.hasThumbnail());
    QVERIFY (!desc.selected());
    QVERIFY (!desc.progress());
    QVERIFY (desc.historyIndex() == -1);
}

/*
 * Tests all but the default constructor. The default constructor is used all
 * over the place and tested also in testDefaults().
 */
void
Ut_WallpaperDescriptor::testConstructors ()
{
    WallpaperDescriptor desc1 ("/testfile1");
    WallpaperDescriptor desc2 (desc1);
    WallpaperDescriptor desc3;

    desc3 = desc2;

    desc1.setOriginalFilePath ("/origfilepath");
    desc1.setThumbnailPending (true);
    desc1.setSelected (true);
    desc1.setProgress (true);
    desc1.setHistoryIndex (5);


    QVERIFY (desc2.filePath() == "/testfile1");
    QVERIFY (desc2.originalFilePath() == "/origfilepath");
    QVERIFY (desc2.thumbnailPending());
    QVERIFY (desc2.selected());
    QVERIFY (desc2.progress());
    QVERIFY (desc2.historyIndex() == 5);
    
    QVERIFY (desc3.filePath() == "/testfile1");
    QVERIFY (desc3.originalFilePath() == "/origfilepath");
    QVERIFY (desc3.thumbnailPending());
    QVERIFY (desc3.selected());
    QVERIFY (desc3.progress());
    QVERIFY (desc3.historyIndex() == 5);
}

void
Ut_WallpaperDescriptor::testThumbnail ()
{
    WallpaperDescriptor desc1 ("/testfile1");
    WallpaperDescriptor desc2 (desc1);
    WallpaperDescriptor desc3;
    QPixmap             thumbnail;
    QPixmap             pixmap (ThumbnailWidth, ThumbnailHeight);

    desc3 = desc2;

    /*
     * Initially no thumbnails.
     */
    QVERIFY (checkHasNoThumbnail(&desc1));
    QVERIFY (checkHasNoThumbnail(&desc2));
    QVERIFY (checkHasNoThumbnail(&desc3));

    /*
     * Then we set one.
     */
    desc1.setThumbnail (pixmap);
    QVERIFY (checkHasThumbnail(&desc1));
    QVERIFY (checkHasThumbnail(&desc2));
    QVERIFY (checkHasThumbnail(&desc3));

    /*
     * Then unset...
     */
    desc1.unsetThumbnail ();
    QVERIFY (checkHasNoThumbnail(&desc1));
    QVERIFY (checkHasNoThumbnail(&desc2));
    QVERIFY (checkHasNoThumbnail(&desc3));
}

void
Ut_WallpaperDescriptor::testUrlMimeType_data ()
{
    QTest::addColumn<QString>("filePath");
    QTest::addColumn<QString>("mimeType");
    QTest::addColumn<QString>("url");

    QTest::newRow("") << "/a.bmp" << "image/bmp" << "file:///a.bmp";
    QTest::newRow("") << "/a.gif" << "image/gif" << "file:///a.gif";
    QTest::newRow("") << "/a.jpg" << "image/jpeg" << "file:///a.jpg";
    QTest::newRow("") << "/a.jpe" << "image/jpeg" << "file:///a.jpe";
    QTest::newRow("") << "/a.jpeg" << "image/jpeg" << "file:///a.jpeg";
    QTest::newRow("") << "/a.tif" << "image/tiff" << "file:///a.tif";
    QTest::newRow("") << "/a.png" << "image/png" << "file:///a.png";
}

void
Ut_WallpaperDescriptor::testUrlMimeType ()
{
    QFETCH(QString, filePath);
    QFETCH(QString, mimeType);
    QFETCH(QString, url);
    
    WallpaperDescriptor desc (filePath);

    desc.setFilePath (filePath);
    QVERIFY (desc.mimeType() == mimeType);
    QVERIFY (desc.url().toString() == url);
}

void
Ut_WallpaperDescriptor::testOrder ()
{
    WallpaperDescriptor desc1 ("/a file");
    WallpaperDescriptor desc2 ("/b file");
    WallpaperDescriptor inhistory1 ("/a file");
    WallpaperDescriptor inhistory2 ("/a file");
    WallpaperDescriptor selected ("/a file");
    
    inhistory1.setHistoryIndex (1);
    inhistory2.setHistoryIndex (2);
    selected.setSelected (true);

    /*
     * Selected one always comes first.
     */
    QVERIFY (selected < desc1);
    QVERIFY (selected < desc2);
    QVERIFY (selected < inhistory1);
    QVERIFY (selected < inhistory2);

    /*
     * History shall go before all non-history items, history index shall sort
     * the items that has history...
     */
    QVERIFY (inhistory1 < desc1);
    QVERIFY (inhistory1 < desc2);
    QVERIFY (inhistory1 < inhistory2);
    
    /*
     * Alphabetical order.
     */
    QVERIFY (desc1 < desc2);

}

void
Ut_WallpaperDescriptor::testLoad ()
{
    WallpaperDescriptor desc1 (fileToLoad);
    QSize               expectedSize (170, 171);
    QSize               originalSize;
    QuillImage          image;

    image = desc1.load (expectedSize, originalSize);
    
    QVERIFY (originalSize == QSize(1024, 1025));
    QVERIFY (image.width() == 170 && image.height() == 171);
}

/******************************************************************************
 * Low level helper functions.
 */
bool
Ut_WallpaperDescriptor::checkHasNoThumbnail (
        WallpaperDescriptor *d)
{
    QPixmap  thumbnail = d->thumbnail ();

    if (d->hasThumbnail()) {
        SYS_WARNING ("Descriptor should have no thumbnail");
        return false;
    }

    if (thumbnail.width() != 0 || thumbnail.height() != 0) {
        SYS_WARNING ("Size %dx%d not valid.", 
                thumbnail.width(),
                thumbnail.height());
        return false;
    }

    return true;
}

bool
Ut_WallpaperDescriptor::checkHasThumbnail (
        WallpaperDescriptor *d)
{
    QPixmap  thumbnail = d->thumbnail ();

    if (!d->hasThumbnail())
        return false;

    if (thumbnail.width() != ThumbnailWidth || 
            thumbnail.height() != ThumbnailHeight)
        return false;

    return true;
}

QTEST_APPLESS_MAIN(Ut_WallpaperDescriptor)



