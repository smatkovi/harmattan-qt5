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

#include "themebrief.h"
#include <dcpwidgettypes.h>

#undef DEBUG
#include "../debug.h"

ThemeBrief::ThemeBrief (
        ThemeBusinessLogic *businessLogic) :
    m_ThemeBusinessLogic (businessLogic)
{
    connect (businessLogic, SIGNAL(themeChanged(QString)),
            this, SLOT(themeChanged(QString)));
}

int
ThemeBrief::widgetTypeID() const
{
    return DcpWidgetType::Image;
}

QString 
ThemeBrief::valueText() const
{
    Q_ASSERT (m_ThemeBusinessLogic != 0);
    return m_ThemeBusinessLogic->currentThemeName ();
}

QString 
ThemeBrief::icon () const
{
    Q_ASSERT (m_ThemeBusinessLogic != 0);

    SYS_DEBUG ("Returning %s", 
            SYS_STR(m_ThemeBusinessLogic->currentThemeIconName ()));
    return m_ThemeBusinessLogic->currentThemeIconName ();
}

void 
ThemeBrief::themeChanged (
        QString themeCodeName)
{
    Q_UNUSED (themeCodeName);
    SYS_DEBUG ("*** themeCodeName = %s", SYS_STR(themeCodeName));

    emit valuesChanged();
}

