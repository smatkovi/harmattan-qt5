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
#ifndef THEMEBUSINESSLOGIC_H
#define THEMEBUSINESSLOGIC_H

#include <QList>
#include <QStringList>
#include <QObject>
#include <QDir>
#include <MApplication>

#include "themedescriptor.h"

class QString;
class ThemeDescriptor;

#define THEME_DBUS_INTERFACE "com.nokia.theme"
#define THEME_DBUS_ADDED_SIGNAL "added"
#define THEME_DBUS_REMOVED_SIGNAL "removed"

/*
  This class will connect to some DBus signals to detect when the theme packages
  are installed/removed. Here are some examples how to test and use this feature
  from the shell:

  dbus-send --session --type=signal / com.nokia.theme.added string:blanco
  dbus-send --session --type=signal / com.nokia.theme.removed string:blanco

 */
class ThemeBusinessLogic : public QObject
{
    Q_OBJECT

public:
    static ThemeBusinessLogic *instance ();
    ~ThemeBusinessLogic ();
    
    QString currentThemeCodeName ();
    QString currentThemeName ();
    QString currentThemeIconName ();
    QList<ThemeDescriptor *> availableThemes ();

    ThemeDescriptor *themeByCodename (const QString &codeName);

    QString themePreviewFileName (
            const QString  &themeCodeName,
            M::Orientation orientation) const;

protected:
    ThemeBusinessLogic ();

public slots:
    /*!
     * Sets the current theme to the theme with the given id.
     */
    void changeTheme (QString themeCodeName);
    void themeAdded (QString themeName);
    void themeRemoved (QString themeName);

private slots:
    void performThemeChange ();
    void themeChangeCompleted ();

signals:
    void themeChangeStarted (QString themeCodeName);
    void themeChanged (QString themeCodeName);
    void themeAboutToBeRemoved (int index);
    void themeRemoved (QList<ThemeDescriptor *> list);
    void themeAboutToBeAdded (int index);
    void themeAdded (QList<ThemeDescriptor *> list);
    void refreshNeeded ();

private:
    static ThemeBusinessLogic   *sm_Instance;

    QList<ThemeDescriptor *>     m_AvailableThemes;
    QStringList                  m_DisabledThemeNames;
    QString                      m_ChangingTheme;
#ifdef UNIT_TEST
    friend class Ut_ThemeBusinessLogic;
#endif
};

#endif
