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

#include "ut_themebusinesslogic.h"
#include "mdesktopentry.h"
#include "themebusinesslogic.h"
#include "themedescriptor.h"

#include <MApplication>
#include <MGConfItem>
#include <MTheme>

#ifndef MDESKTOPENTRY_STUB_H
# error "The MDesktopEntry class is not stubbed, can't continue."
#endif

#define DEBUG
#include "../../src/debug.h"

/******************************************************************************
 * Fake MGconfItem implementation. 
 */
QString  GConfLastKey;
QString  GConfLastValue;

void
MGConfItem::set (
        const QVariant &val)
{
    GConfLastKey   = key();
    GConfLastValue = val.toString();
    SYS_DEBUG ("*** value = %s", SYS_STR(val.toString()));
    SYS_DEBUG ("*** key   = %s", SYS_STR(key()));
}

/******************************************************************************
 * Ut_ThemeBusinessLogicPrivate implementation. 
 */
UtThemeBusinessLogicPrivate::UtThemeBusinessLogicPrivate ()
{
    reset ();
}


void
UtThemeBusinessLogicPrivate::reset ()
{
    m_ThemeAboutToBeAdded = false;
    m_ThemeAdded          = false;
    m_ThemeAboutToBeRemoved = false;
    m_ThemeRemoved        = false;
}

void
UtThemeBusinessLogicPrivate::themeChanged (
        QString themeCodeName)
{
    m_ThemeCodeName = themeCodeName;
}

void
UtThemeBusinessLogicPrivate::themeChangeStarted (
        QString themeCodeName)
{
    m_ThemeCodeNameUnderProcess = themeCodeName;
}

void
UtThemeBusinessLogicPrivate::themeAboutToBeRemoved (
        int index)
{
    SYS_DEBUG ("");
    Q_UNUSED (index);
    m_ThemeAboutToBeRemoved = true;
}
 
void 
UtThemeBusinessLogicPrivate::themeRemoved (
        QList<ThemeDescriptor *> list)
{
    SYS_DEBUG ("");
    Q_UNUSED (list);
    Q_ASSERT (m_ThemeAboutToBeRemoved);
    m_ThemeRemoved = true;
}

void
UtThemeBusinessLogicPrivate::themeAboutToBeAdded (
        int index)
{
    SYS_DEBUG ("");
    Q_UNUSED (index);
    m_ThemeAboutToBeAdded = true;
}
 
void
UtThemeBusinessLogicPrivate::themeAdded (
        QList<ThemeDescriptor *> list)
{
    SYS_DEBUG ("");
    Q_UNUSED (list);
    Q_ASSERT (m_ThemeAboutToBeAdded);
    m_ThemeAdded = true;
}

/******************************************************************************
 * Ut_ThemeBusinessLogic implementation. 
 */
void 
Ut_ThemeBusinessLogic::init()
{
}

void 
Ut_ThemeBusinessLogic::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./Ut_ThemeBusinessLogic";

void 
Ut_ThemeBusinessLogic::initTestCase()
{
    bool connectSuccess;

    m_Priv = new UtThemeBusinessLogicPrivate;
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

    connectSuccess = connect (
            m_Api, SIGNAL(themeAboutToBeRemoved(int)),
            m_Priv, SLOT(themeAboutToBeRemoved(int)));
    QVERIFY (connectSuccess);
    
    connectSuccess = connect (
            m_Api, SIGNAL(themeRemoved(QList<ThemeDescriptor *>)),
            m_Priv, SLOT(themeRemoved(QList<ThemeDescriptor *>)));
    QVERIFY (connectSuccess);
    
    connectSuccess = connect (
            m_Api, SIGNAL(themeAboutToBeAdded (int)),
            m_Priv, SLOT(themeAboutToBeAdded (int)));
    QVERIFY (connectSuccess);
    
    connectSuccess = connect (
            m_Api, SIGNAL(themeAdded (QList<ThemeDescriptor *>)),
            m_Priv, SLOT(themeAdded (QList<ThemeDescriptor *>)));
    QVERIFY (connectSuccess);
}

void 
Ut_ThemeBusinessLogic::cleanupTestCase()
{
    delete m_App;
    delete m_Api;
    delete m_Priv;
}

/*!
 * This method will test the available themes stubbed, exactly as it is in the 
 * qdirstub.h and in the mdesktopentry stub databse.
 */
void
Ut_ThemeBusinessLogic::testAvailableThemes ()
{
    QList<ThemeDescriptor *>  list = m_Api->availableThemes ();
    ThemeDescriptor          *desc;
    QString                   themeCodeName;

    SYS_DEBUG ("*** we have %d valid themes.", list.size());
    for (int n = 0; n < list.size(); ++n) {
        SYS_DEBUG ("*** %dth list item", n);
        SYS_DEBUG ("*** name()     = %s", SYS_STR(list[n]->name()));
        SYS_DEBUG ("*** codeName() = %s", SYS_STR(list[n]->codeName()));
        SYS_DEBUG ("*** iconName() = %s", SYS_STR(list[n]->iconName()));
    
        desc = list[n];

        QVERIFY(desc->isValid());
        QVERIFY(desc->isVisible());
        QVERIFY(!desc->name().isEmpty());
        QVERIFY(!desc->codeName().isEmpty());
        QVERIFY(!desc->iconName().isEmpty());
    }

    /**************************************************************************
     * Checking the items as they are defined in the MDesktopEntry stub.
     *
     * In the mdesktopentry stub implementation currently there is this much 
     * available valid and visible themes.
     */
    QVERIFY (list.size() == 2);
    
    desc = list[0];
    QVERIFY(desc->name() == NAMEDesktopFilePerfect);
    QVERIFY(desc->codeName() == CODENAMEDesktopFilePerfect);
    QVERIFY(desc->iconName() == ICONDesktopFilePerfect);

    // In order to check the current name we have to stub the MTheme class?
    desc = list[1];
    QVERIFY(desc->name() == NAMEDesktopFileCurrent);
    QVERIFY(desc->iconName() == ICONDesktopFileCurrent);
    
    /**************************************************************************
     * Let's simulate a dbus api signal that is used to detect the theme
     * uninstall.
     */
    m_Priv->reset ();
    themeCodeName = list[1]->codeName();
    m_Api->themeRemoved (CODENAMEDesktopFilePerfect);
    list = m_Api->availableThemes ();

    QVERIFY (m_Priv->m_ThemeRemoved);
    QVERIFY (list.size() == 1);
    QVERIFY (list[0]->codeName() == themeCodeName);

    /**************************************************************************
     * Let's simulate a dbus api signal that is used to detect the theme
     * install.
     */
    m_Api->themeAdded (CODENAMEDesktopFilePerfect);
    list = m_Api->availableThemes ();

    QVERIFY (m_Priv->m_ThemeAdded);
    QVERIFY (list.size() == 2);
}


/*!
 * Checks if the information on the current theme is available and valid.
 */
void
Ut_ThemeBusinessLogic::testCurrentTheme ()
{
    QString themeCodeName;
    QString themeName;
    QString themeIconName;

    // We are stubbed, how can we test??
    themeCodeName = m_Api->currentThemeCodeName ();
    themeName = m_Api->currentThemeName ();
    themeIconName = m_Api->currentThemeIconName();
    
    SYS_DEBUG ("*** currentThemeCodeName() = %s", SYS_STR(themeCodeName));
    SYS_DEBUG ("*** currentThemeName()     = %s", SYS_STR(themeName));
    SYS_DEBUG ("*** currentThemeIconName() = %s", SYS_STR(themeIconName));

    QVERIFY (!themeCodeName.isEmpty());
    QVERIFY (themeCodeName == MTheme::instance()->currentTheme());
    QVERIFY (themeName == NAMEDesktopFileCurrent);
    QVERIFY (themeIconName == ICONDesktopFileCurrent);
}

void
Ut_ThemeBusinessLogic::testSetTheme ()
{
    QString fakeName = "FakeThemeCodeName";
    
    /*
     * Initiating the theme change.
     */
    m_Api->changeTheme (fakeName);
    QVERIFY (GConfLastKey == "/meegotouch/theme/name");
    QVERIFY (GConfLastValue == fakeName);
    QVERIFY (m_Priv->m_ThemeCodeNameUnderProcess == fakeName);

    /*
     * Finishing the theme name. FIXME: It would be better to test this by
     * sending a signal, so we could test that the businesslogic connected the
     * signal.
     */
    m_Api->themeChangeCompleted ();
    QVERIFY (m_Priv->m_ThemeCodeName == m_Api->currentThemeCodeName ());
}

QTEST_APPLESS_MAIN(Ut_ThemeBusinessLogic)

