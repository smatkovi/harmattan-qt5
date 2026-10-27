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

#include "themedescriptor.h"

#include <QString>
#include <mdesktopentry.h>

#undef DEBUG
#define WARNING
#include "../debug.h"


static const QString requiredType = "X-MeeGoTouch-Metatheme";
static const QString typeKey = "Desktop Entry/Type";

static const QString nameKey = "Desktop Entry/Name";
static const QString iconNameKey = "X-MeeGoTouch-Metatheme/X-Icon";
static const QString visibleKey = "X-MeeGoTouch-Metatheme/X-Visible";

ThemeDescriptor::ThemeDescriptor (
        QObject       *parent,
        const QString &directoryPath,
        const QString &codeName) :
    QObject (parent),
    m_Valid (false),
    m_Visible (false),
    m_CodeName (codeName),
    m_DesktopEntry (0)
{
    QString indexFileName (directoryPath + "/index.theme");

    SYS_DEBUG ("*** indexFileName = %s", SYS_STR(indexFileName));

    /*
     * If the theme index file is missing the MDesktopEntry will report an
     * invalid desktop file, so we don't have to check if the directory actually
     * holds an index file.
     */
    m_DesktopEntry = new MDesktopEntry (indexFileName);
    if (!m_DesktopEntry->isValid()) {
        delete m_DesktopEntry;
        m_DesktopEntry = 0;
        return;
    }

    if (m_DesktopEntry->value(typeKey) != requiredType) {
        SYS_DEBUG ("The type not right: '%s', should be '%s'", 
                SYS_STR(m_DesktopEntry->value(typeKey)),
                SYS_STR(requiredType));
        delete m_DesktopEntry;
        m_DesktopEntry = 0;
        return;
    }

    m_Visible = m_DesktopEntry->value (visibleKey) == "true";
    m_Name = m_DesktopEntry->value (nameKey);
    m_IconName =  m_DesktopEntry->value (iconNameKey).
        trimmed().remove('\"');
    
    m_Valid = true;
}

ThemeDescriptor::~ThemeDescriptor ()
{
    if (m_DesktopEntry)
        delete m_DesktopEntry;
}


bool 
ThemeDescriptor::operator== (
        const ThemeDescriptor &other) const
{
    return m_Valid && other.m_Valid && m_CodeName == other.m_CodeName;
}

bool 
ThemeDescriptor::operator== (
        const QString &name) const
{
    return m_Valid && 
        (m_CodeName == name || m_Name == name);
}

/*!
 * Returns true if the theme is valid, false if the desktop file is missing, the
 * format of the desktop file is invalid or the theme has some property not set
 * in theme desktop file.
 */
bool
ThemeDescriptor::isValid () const
{
    return m_Valid;
}

/*!
 * Returns true if the theme is visible according to the desktop file.
 */
bool
ThemeDescriptor::isVisible () const
{
    return m_Visible;
}

/*!
 * Returns the name of the theme as it is registered in the desktop file.
 */
QString
ThemeDescriptor::name() const
{
    return m_Name;
}

/*!
 * Returns the code name (directory basename) for the theme.
 */
QString
ThemeDescriptor::codeName() const
{
    return m_CodeName;
}

/*!
 * Returns the symbolic icon name for the theme.
 */
QString
ThemeDescriptor::iconName() const
{
    return m_IconName;
}


