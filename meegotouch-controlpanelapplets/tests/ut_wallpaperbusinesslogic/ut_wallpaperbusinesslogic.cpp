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

#include "filesystemstub.h"
#include "ut_wallpaperbusinesslogic.h"
//#include "wallpapergconf.h"

#include "wallpaperbusinesslogic.h"
#include "wallpaperitrans.h"
//#include "wallpapergconf.h"
#include "wallpaperutils.h"
#include "wallpaperconfiguration.h"

#include <MApplication>
#include <MGConfItem>

#define DEBUG
#include "../../src/debug.h"

static const QString PortraitKey = 
    "/desktop/meego/background/portrait/picture_filename";
static const QString LandscapeKey = 
    "/desktop/meego/background/landscape/picture_filename";

/******************************************************************************
 * GConf stub.
 */
#include <gconf/gconf-client.h>
#include <gconf/gconf-value.h>

static QHash<QString, QString> MyGConfDatabase;

GConfValue *
gconf_client_get (
        GConfClient* client,
        const gchar* key,
        GError** err)
{
    Q_UNUSED (client);
    Q_UNUSED (err);
    Q_ASSERT (qstrlen (key) >= 10);

    QString myKey = key;
    GConfValue *retval;

    SYS_WARNING ("*** key = %s", SYS_STR(myKey));
    if (MyGConfDatabase.contains(myKey)) {
        const char *r = strdup(MyGConfDatabase[myKey].toUtf8().constData());

        retval = gconf_value_new (GCONF_VALUE_STRING);
        gconf_value_set_string (retval, r);
        SYS_DEBUG ("returning '%s'", r);
    } else {
        SYS_DEBUG ("returning false");
        retval = gconf_value_new (GCONF_VALUE_BOOL);
    }

    return retval;
}

/******************************************************************************
 * QImage stub.
 */
#include <QImage>

bool 	
QImage::load (
        const QString  &fileName, 
        const char     *format)
{
    Q_UNUSED (fileName);
    Q_UNUSED (format);
    SYS_DEBUG ("*** fileName = %s", SYS_STR(fileName));
    return true;
}

#include <QPixmap>
bool
QPixmap::load (
        const QString  &fileName, 
        const char     *format, 
        Qt::ImageConversionFlags flags)
{
    Q_UNUSED (fileName);
    Q_UNUSED (format);
    Q_UNUSED (flags);
    SYS_DEBUG ("*** fileName = %s", SYS_STR(fileName));
    return true;
}

/******************************************************************************
 * SignalSink implementation.
 */
SignalSink::SignalSink ()
{
    reset ();
}

void
SignalSink::reset ()
{
    m_WallpaperChangedCame = false;
}

void
SignalSink::wallpaperChanged ()
{
    SYS_DEBUG ("");
    m_WallpaperChangedCame = true;
}

/******************************************************************************
 * Ut_WallpaperBusinessLogic implementation. 
 */
void 
Ut_WallpaperBusinessLogic::init()
{
}

void 
Ut_WallpaperBusinessLogic::cleanup()
{
}


static int argc = 1;
static char* app_name = (char*) "./ut_wallpaperbusinesslogic";

void 
Ut_WallpaperBusinessLogic::initTestCase()
{
    bool connectSuccess;

    MyGConfDatabase["/meegotouch/theme/name"] = "MyThemeName";
    MyGConfDatabase[Wallpaper::CurrentPortraitKey] = 
        "/ak/current.jpg";
    MyGConfDatabase[Wallpaper::OriginalPortraitKey] = "";

    m_App = new MApplication (argc, &app_name);


    m_Api = new WallpaperBusinessLogic;
    connectSuccess = connect (
            m_Api, SIGNAL (wallpaperChanged()),
            &m_SignalSink, SLOT (wallpaperChanged()));
    QVERIFY (connectSuccess);


    //QVERIFY (m_Api->m_EditedImage == 0);
}

void 
Ut_WallpaperBusinessLogic::cleanupTestCase()
{
    delete m_Api;

    m_App->deleteLater ();
}

/*!
 * Checks the low level WallpaperITrans class, its tag methods and overloaded
 * operators.
 */
void
Ut_WallpaperBusinessLogic::testITrans ()
{
    WallpaperITrans trans1, trans2, trans4, trans5;
    
    SYS_DEBUG ("Checking default values of WallpaperITrans...");
    QVERIFY (trans1.x() == 0);
    QVERIFY (trans1.y() == 0);
    QVERIFY (trans1.scale() == 1.0);
    QVERIFY (trans1.orientation() == M::Landscape);

    SYS_DEBUG ("Checking the set functions...");
    trans2.setOffset (QPointF(10, 11));
    trans2.setScale (2.0);
    trans2.setOrientation (M::Portrait);
    trans2.setExpectedSize (QSize(864, 480));

    QVERIFY (trans2.x() == 10);
    QVERIFY (trans2.y() == 11);
    QVERIFY (trans2.offset() == QPointF(10, 11));
    QVERIFY (trans2.scale() == 2.0);
    QVERIFY (trans2.orientation() == M::Portrait);
    QVERIFY (trans2.expectedWidth() == 864);
    QVERIFY (trans2.expectedHeight() == 480);

    trans2 += QPointF(1.0, 1.0);
    QVERIFY (trans2.offset() == QPointF(11, 12));

    SYS_DEBUG ("Testing the copy constructor.");
    WallpaperITrans trans3 (trans2);
    QVERIFY (trans3.x() == 11);
    QVERIFY (trans3.y() == 12);
    QVERIFY (trans3.scale() == 2.0);
    QVERIFY (trans3.orientation() == M::Portrait);
    QVERIFY (trans3.expectedWidth() == 864);
    QVERIFY (trans3.expectedHeight() == 480);

    SYS_DEBUG ("testing operator=...");
    trans1 = trans2;
    QVERIFY (trans1.x() == 11);
    QVERIFY (trans1.y() == 12);
    QVERIFY (trans1.scale() == 2.0);
    QVERIFY (trans1.orientation() == M::Portrait);
    QVERIFY (trans1.expectedWidth() == 864);
    QVERIFY (trans1.expectedHeight() == 480);
    QVERIFY (trans1.expectedSize() == QSize(864, 480));

    SYS_DEBUG ("Testing operator*...");
    SYS_DEBUG ("*** trans1.scale() = %d", trans2 * 2);
    QVERIFY (trans2 * 2 == 4);


    QVERIFY (trans4.noTransformation ());
    QVERIFY (trans4.rotation() == 0.0);
    trans4.setRotation(90.0);
    QVERIFY (!trans4.noTransformation ());
    QVERIFY (trans4.rotation() == 90.0);
    trans4.modRotation(1200.0);
    QVERIFY (trans4.rotation() > 90.0);

    QVERIFY (trans5.noTransformation ());
    trans5.modScale(1200);
    QVERIFY (!trans5.noTransformation ());
    QVERIFY (trans5.scale() > 1.0);

    trans5.setScale (0.1);
    trans5.modScale (-1200);
    trans5.modScale (-1200);
    trans5.modScale (-1200);
    QVERIFY (trans5.scale() > 0.0);
}

/*!
 * Checks if the GConfItem objects are created with the right keys.
 */
void
Ut_WallpaperBusinessLogic::testGConfItems ()
{
    QVERIFY (m_Api->m_PPItem != NULL);
    QVERIFY (m_Api->m_PPItem->key() == Wallpaper::CurrentPortraitKey);

    QVERIFY (m_Api->m_POItem != NULL);
    QVERIFY (m_Api->m_POItem->key() == Wallpaper::OriginalPortraitKey);

    QVERIFY (m_Api->m_PHItem != NULL);
    QVERIFY (m_Api->m_PHItem->key() == Wallpaper::PortraitHistoryKey);
}

/*!
 */
void
Ut_WallpaperBusinessLogic::testCurrentWallpaper ()
{
    QString currentFilePath;
    QString originalFilePath;


    m_Api->currentWallpaper (currentFilePath, originalFilePath);
    QVERIFY (currentFilePath == "/ak/current.jpg");
    QVERIFY (originalFilePath.isEmpty());

    SYS_DEBUG ("*** currentFilePath  = %s", SYS_STR(currentFilePath));
    SYS_DEBUG ("*** originalFilePath = %s", SYS_STR(originalFilePath));
}

void
Ut_WallpaperBusinessLogic::testSceneSize ()
{
    QSize size = m_Api->sceneSize ();
    QSize expected = QSize (480, 854);

    QVERIFY (size == expected);
}

void
Ut_WallpaperBusinessLogic::testEditedImage ()
{
    QVERIFY (!m_Api->hasEditedImage ());

    m_Api->endEdit ();
    QVERIFY (!m_Api->hasEditedImage ());

}

/******************************************************************************
 * Private functions.
 */
QTEST_APPLESS_MAIN(Ut_WallpaperBusinessLogic)


