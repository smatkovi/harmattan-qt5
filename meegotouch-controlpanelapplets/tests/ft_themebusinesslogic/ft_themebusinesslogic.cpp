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

#include "ft_themebusinesslogic.h"
#include "themebusinesslogic.h"
#include "themedescriptor.h"

#include <MApplication>
#include <MTheme>
#include <QPixmap>

#define DEBUG
#include "../../src/debug.h"

static const int themeWaitMSec = 1000;

/******************************************************************************
 * Ft_ThemeBusinessLogicPrivate implementation. 
 */
void
FtThemeBusinessLogicPrivate::themeChanged (
        QString themeCodeName)
{
    m_ThemeCodeName = themeCodeName;
}

void
FtThemeBusinessLogicPrivate::themeChangeStarted (
        QString themeCodeName)
{
    m_ThemeCodeNameUnderProcess = themeCodeName;
}

/******************************************************************************
 * Ft_ThemeBusinessLogic implementation. 
 */
void 
Ft_ThemeBusinessLogic::init()
{
}

void 
Ft_ThemeBusinessLogic::cleanup()
{
}


static int argc = 1;
static char* app_name = (char*) "./Ft_ThemeBusinessLogic";

void 
Ft_ThemeBusinessLogic::initTestCase()
{
    bool connectSuccess;

    m_Priv = new FtThemeBusinessLogicPrivate;
    m_App = new MApplication (argc, &app_name);
    m_Api = ThemeBusinessLogic::instance();

    connectSuccess = connect (
            m_Api, SIGNAL(themeChanged(QString)),
            m_Priv, SLOT(themeChanged(QString)));
    QVERIFY (connectSuccess);

    connectSuccess = connect (
            m_Api, SIGNAL(themeChangeStarted(QString)),
            m_Priv, SLOT(themeChangeStarted(QString)));
    QVERIFY (connectSuccess);
        
    connectSuccess = connect (
            m_Priv, SIGNAL(changeTheme(QString)),
            m_Api, SLOT(changeTheme(QString)));
    QVERIFY (connectSuccess);
}

void 
Ft_ThemeBusinessLogic::cleanupTestCase()
{
    delete m_App;
    delete m_Api;
    delete m_Priv;
}

/*!
 * Checks if the information on the current theme is available and valid.
 */
void
Ft_ThemeBusinessLogic::testCurrentTheme ()
{
    QString themeCodeName, themeName;

    // We are checking this in the unit test already, but it is simple...
    themeCodeName = m_Api->currentThemeCodeName ();
    SYS_DEBUG ("*** currentThemeCodeName = %s", SYS_STR(themeCodeName));
    QVERIFY (!themeCodeName.isEmpty());

    themeName = m_Api->currentThemeName ();
    SYS_DEBUG ("*** currentThemeName     = %s", SYS_STR(themeName));
    QVERIFY (!themeName.isEmpty());
}

/*!
 * Checks if there are available themes (at least one) and checks if the name,
 * icon name and codename is not an empty string for all the visible themes.
 * Moreover all the available themes must be valid.
 *
 * Checks if the invisible themes are filtered out.
 */
void
Ft_ThemeBusinessLogic::testAvailableThemes ()
{
    QList<ThemeDescriptor *> list = m_Api->availableThemes ();

    QVERIFY (list.size() > 0);

    foreach (ThemeDescriptor *desc, list) {
        checkDescriptorIsValid (desc);
    }
}

/*!
 * Takes all the themes that are visible and changes the current theme to this
 * themes one by one. Checks if the themeChanged() signal is arrived with this
 * new theme.
 */
void
Ft_ThemeBusinessLogic::testChangeTheme ()
{
    QList<ThemeDescriptor *> list = m_Api->availableThemes ();

    foreach (ThemeDescriptor *desc, list) {
        if (!desc->isVisible()) {
            continue;
        }

        qDebug() << "Changing theme to " << desc->codeName();
        m_Api->changeTheme (desc->codeName());
        QTest::qWait (themeWaitMSec);

        qDebug() << "Theme is          " << m_Priv->m_ThemeCodeNameUnderProcess;
        QVERIFY (m_Priv->m_ThemeCodeNameUnderProcess == desc->codeName());
    }
}

/*
 * Tests if the theme descriptor is indeed valid, visible and it has the
 * properties that we can use to represent the theme.
 */
void 
Ft_ThemeBusinessLogic::checkDescriptorIsValid (
        ThemeDescriptor *desc)
{
    QString name, codeName, iconName;
    bool    valid, visible;

    QVERIFY (desc != 0);

    name = desc->name ();
    codeName = desc->codeName ();
    iconName = desc->iconName ();
    valid = desc->isValid ();
    visible = desc->isVisible ();
    
    #if 0
    SYS_DEBUG ("--------------------------------------");
    SYS_DEBUG ("*** name      = %s", SYS_STR (name));
    SYS_DEBUG ("*** codeName  = %s", SYS_STR (desc->codeName()));
    SYS_DEBUG ("*** iconName  = %s", SYS_STR (desc->iconName()));
    SYS_DEBUG ("*** isValid   = %s", SYS_BOOL(valid));
    SYS_DEBUG ("*** isVisible = %s", SYS_BOOL (visible));
    #endif

    /*
     * The descriptor has to be valid and visible.
     */
    QVERIFY (valid);
    QVERIFY (visible);
    
    // Checking the name.
    QVERIFY (!name.isEmpty());
    QVERIFY (!name.startsWith(" "));
    QVERIFY (!name.endsWith(" "));

    QVERIFY (!codeName.isEmpty());
    QVERIFY (!codeName.startsWith(" "));
    QVERIFY (!codeName.endsWith(" "));
    QVERIFY (!codeName.contains("\""));

    QVERIFY (!iconName.isEmpty());
    QVERIFY (!iconName.startsWith(" "));
    QVERIFY (!iconName.endsWith(" "));
    QVERIFY (!iconName.contains("\""));

    MTheme  *theme = MTheme::instance();
    QPixmap *pixmap = theme->pixmapCopy (iconName);
    if (pixmap->width() < 64 ||
            pixmap->height() < 64) {
        SYS_WARNING (
"\n"
"----------------------------------------------------------------------------\n"
"                         Theme Icon Warning \n"
"The theme icon %s has the size %dx%d while it should be \n"
"at least 64x64. This is probably a bug in the theme daemon. We are not \n"
"reporting failure now, because this is a known bug in the theme daemon. \n"
"----------------------------------------------------------------------------\n"
       , SYS_STR(iconName), pixmap->width(), pixmap->height());
    }

    delete pixmap;
}


QTEST_APPLESS_MAIN(Ft_ThemeBusinessLogic)

