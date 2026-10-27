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

#include "themebusinesslogic.h"
#include "themedescriptor.h"

#include <MGConfItem>
#include <MTheme>
#include <QDBusConnection>
#include <QDBusError>
#include <QString>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QTimer>

#define DEBUG
#define WARNING
#include "../debug.h"

#if !defined(UNIT_TEST) || defined(FUNCTIONAL_TEST)
#  define entryList(themeDir, filters) \
      themeDir.entryList (filters)
#else
#  include "qdirstub.h"
#endif

ThemeBusinessLogic *ThemeBusinessLogic::sm_Instance = 0;

// The directory where all the available themes are installed.
static const QString themeDirName (MEEGO_THEMEDIR);

// The GConf key where meegotouch expects us to place the theme name.
static const QString themeGConfKey ("/meegotouch/theme/name");

ThemeBusinessLogic *
ThemeBusinessLogic::instance ()
{
    if (!ThemeBusinessLogic::sm_Instance)
        ThemeBusinessLogic::sm_Instance = 
            new ThemeBusinessLogic;

    return ThemeBusinessLogic::sm_Instance;
}

ThemeBusinessLogic::ThemeBusinessLogic ()
{
    bool   success;

    connect (MTheme::instance(), SIGNAL(themeChangeCompleted()),
            this, SLOT(themeChangeCompleted()));

    /*
     * Connecting to DBus.
     */
    QDBusConnection bus = QDBusConnection::sessionBus ();
    
    success = bus.connect("", "", 
            THEME_DBUS_INTERFACE, THEME_DBUS_REMOVED_SIGNAL, 
            this, SLOT(themeRemoved(QString)));
    if (!success) {
        QDBusError lastError = bus.lastError();
        SYS_WARNING ("Connecting to DBus %s failed: %s", 
                THEME_DBUS_REMOVED_SIGNAL,
                SYS_STR(lastError.message()));
    }
    
    success = bus.connect("", "", 
            THEME_DBUS_INTERFACE, THEME_DBUS_ADDED_SIGNAL, 
            this, SLOT(themeAdded(QString)));
    if (!success) {
        QDBusError lastError = bus.lastError();
        SYS_WARNING ("Connecting to DBus %s failed: %s", 
                THEME_DBUS_ADDED_SIGNAL,
                SYS_STR(lastError.message()));
    }
}

ThemeBusinessLogic::~ThemeBusinessLogic ()
{
    sm_Instance = 0;
}

/*!
 * Returns the code name of the current theme. This code name can be used as a
 * string ID to the GConf database.
 */
QString
ThemeBusinessLogic::currentThemeCodeName ()
{
    MTheme *theme = MTheme::instance();

    Q_ASSERT (theme != 0);

    return theme->currentTheme();
}

/*!
 * Returns the official name of the current theme. This name can be used as a UI
 * string.
 */
QString
ThemeBusinessLogic::currentThemeName ()
{
    QString                   codeName = currentThemeCodeName();
    QString                   retval;

    availableThemes();
    foreach (ThemeDescriptor *descr, m_AvailableThemes) {
        if (descr->codeName() == codeName)
            retval = descr->name();
    }

    return retval;
}

/*!
 * Returns the official name of the current theme. This name can be used as a UI
 * string.
 */
QString
ThemeBusinessLogic::currentThemeIconName ()
{
    QString codeName = currentThemeCodeName();
    QString retval;

    availableThemes ();
    foreach (ThemeDescriptor *descr, m_AvailableThemes) {
        if (descr->codeName() == codeName)
            retval = descr->iconName();
    }

    return retval;
}

/*!
 * Returns all the available themes. Invisible themes are filtered out.
 */
QList<ThemeDescriptor *>
ThemeBusinessLogic::availableThemes ()
{
    QDir themeDir (themeDirName);

    /*
     * If we already got the list we are not going to read the filesystem again.
     */
    if (m_AvailableThemes.size() > 0) {
        return m_AvailableThemes;
    }

    /*
     * Here we do the actual reading.
     */
    foreach (QString themeFile, 
            entryList(themeDir, (QDir::Dirs | QDir::NoDotAndDotDot))) {
        ThemeDescriptor *descr;

        /*
         * If the theme is about to be removed we will not process it at all.
         */
        if (m_DisabledThemeNames.indexOf(themeFile) >= 0)
            continue;

        descr = new ThemeDescriptor (
                this,
                themeDirName + QDir::separator() + themeFile,
                themeFile);

        if (descr->isValid() && descr->isVisible()) {
            SYS_DEBUG ("themeFile = %s", SYS_STR(themeFile));
            m_AvailableThemes << descr;
        } else {
            delete descr;
        }
    }

    return m_AvailableThemes;
}

/*!
 * This function can be used to change the current theme.
 */
void
ThemeBusinessLogic::changeTheme (
        QString themeCodeName)
{
    SYS_DEBUG ("Activating theme '%s'", SYS_STR(themeCodeName));
    emit themeChangeStarted (themeCodeName);
    
    m_ChangingTheme = themeCodeName;
    SYS_DEBUG ("*** m_ChangingTheme = %s", SYS_STR(m_ChangingTheme));
    #ifdef DELAY_THEME_CHANGE
    QTimer::singleShot(1000, this, SLOT(performThemeChange()));
    #else
    performThemeChange ();
    #endif
}

void 
ThemeBusinessLogic::performThemeChange ()
{
    SYS_DEBUG ("CHANGING THEME");
    SYS_DEBUG ("*** m_ChangingTheme = %s", SYS_STR(m_ChangingTheme));
    MGConfItem  gconfItem (themeGConfKey);
    gconfItem.set (m_ChangingTheme);
    SYS_DEBUG ("*** m_ChangingTheme = %s", SYS_STR(m_ChangingTheme));
}

void 
ThemeBusinessLogic::themeChangeCompleted ()
{
    QString     themeCodeName = currentThemeCodeName ();

    SYS_DEBUG ("This            : %p", this);
    SYS_DEBUG ("Theme changed to: '%s'", SYS_STR(themeCodeName));
    SYS_DEBUG ("We wanted       : '%s'", SYS_STR(m_ChangingTheme));
    #ifdef WARNING
    if (themeCodeName != m_ChangingTheme) {
        SYS_WARNING ("We wanted '%s' instead of '%s'", 
                SYS_STR(m_ChangingTheme),
                SYS_STR(themeCodeName));
    }
    #endif
    emit themeChanged (themeCodeName);
    m_ChangingTheme = "";
    emit refreshNeeded ();
}

ThemeDescriptor *
ThemeBusinessLogic::themeByCodename (
        const QString &codeName)
{
    availableThemes ();
    for (int n = 0; n < m_AvailableThemes.size(); ++n)
        if (m_AvailableThemes[n]->codeName() == codeName)
            return m_AvailableThemes[n];

    return 0;
}

/*!
 * \param themeCodeName The code name of the theme for which we need the
 *   screenshot.
 * \param orientation Every theme should be two screenshots.
 * \returns The preview screenshot image file name for a given theme.
 *
 * Please note, that the preview can not handled through the normal MTheme
 * support, because we need the image _before_ the theme is set to be current.
 */
QString 
ThemeBusinessLogic::themePreviewFileName (
        const QString  &themeCodeName,
        M::Orientation  orientation) const
{
    QString oString;

    switch (orientation) {
        case M::Portrait:
            oString = "portrait";
            break;

        case M::Landscape:
            oString = "landscape";
    }

    return themeDirName + QDir::separator() + themeCodeName + 
        "/meegotouch/images/meegotouch-theme-preview-" + oString + ".jpg";
}

void
ThemeBusinessLogic::themeAdded (
        QString themeName)
{
    ThemeDescriptor *descr;
    int              index;

    SYS_DEBUG ("themeName = %s", SYS_STR(themeName));
    descr = new ThemeDescriptor (
            this, themeDirName + QDir::separator() + themeName, themeName);
    
    if (!descr->isValid() && !descr->isVisible()) {
        SYS_WARNING ("Theme %s is invalid or invisible.", SYS_STR(themeName));
        return;
    }

    /*
     * If this theme is on the removed theme list we drop that, it is installed
     * again.
     */
    index = m_DisabledThemeNames.indexOf(themeName);
    if (index >= 0)
        m_DisabledThemeNames.removeAt (index);

    /*
     * If we already have this theme we will not add to the list. No matter who
     * says what, there will be no duplications.
     */
    if (themeByCodename(themeName) != 0) {
        SYS_DEBUG ("");
        return;
    }

    emit themeAboutToBeAdded (m_AvailableThemes.size());
    m_AvailableThemes << descr;
    emit themeAdded (m_AvailableThemes);
    emit refreshNeeded ();
}

void
ThemeBusinessLogic::themeRemoved (
        QString themeName)
{
    SYS_DEBUG ("themeName = %s", SYS_STR(themeName));

    /*
     * If we already notified about the theme removal we don't need to do
     * anything.
     */
    if (m_DisabledThemeNames.indexOf(themeName) >= 0)
        return;

    /*
     * We need to remember which themes are under removal.
     */
    m_DisabledThemeNames << themeName;

    /*
     *
     */
    for (int n = 0; n < m_AvailableThemes.size(); ++n) {
    SYS_DEBUG ("m_AvailableThemes[n]->codeName() = %s",
            SYS_STR(m_AvailableThemes[n]->codeName()));
        if (m_AvailableThemes[n]->codeName() == themeName) {
            ThemeDescriptor *desc = m_AvailableThemes[n];
            SYS_DEBUG ("Theme %d is about to be removed.", n);
            
            emit themeAboutToBeRemoved (n);
            m_AvailableThemes.removeAt (n);
            delete desc;
            emit themeRemoved(m_AvailableThemes);
            
            emit refreshNeeded ();
        }
    }
}

