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
#ifndef PROFILECONTAINER_H
#define PROFILECONTAINER_H

#include <MWidgetController>

class MButton;
class MLabel;
class MImageWidget;

/*!
 * A widget that shows and controls a profile's vibration on/off
 * property.
 */
class ProfileContainer: public MWidgetController
{
    Q_OBJECT
    Q_PROPERTY (bool checked READ isChecked WRITE setChecked)

public:
    ProfileContainer (
            int            id,
            const QString &title,
            bool           vibra,
            MWidget       *parent = 0);
    ~ProfileContainer ();

    int id ();
    bool isChecked () const;
    void setChecked (bool buttonChecked);
    void setText (const QString &text);
    QString text () const;

signals:
    void toggled (bool checked);

private:
    const int       m_ProfileId;
    MButton        *m_Button;
    MLabel         *m_Label;

#ifdef UNIT_TEST
friend class Ut_ProfileContainer;
#endif
};

#endif
