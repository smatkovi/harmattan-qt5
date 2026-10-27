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
#ifndef THEMEDESCRIPTOR_H
#define THEMEDESCRIPTOR_H

#include <QObject>

class QString;
class MDesktopEntry;

/*!
 * A simple class to handle all the information we need to know about one
 * specific theme.
 */
class ThemeDescriptor : public QObject
{
    Q_OBJECT

public:
    ThemeDescriptor (
            QObject       *parent,
            const QString &directoryPath,
            const QString &codeName);
    ~ThemeDescriptor ();

    bool operator== (const ThemeDescriptor &other) const;
    bool operator== (const QString &name) const;
    bool isValid () const;
    bool isVisible () const;
    QString name() const;
    QString codeName() const;
    QString iconName() const;
    
private:
    bool              m_Valid;
    bool              m_Visible;
    QString           m_CodeName;
    QString           m_Name;
    QString           m_IconName;
    MDesktopEntry    *m_DesktopEntry;
};

#endif
