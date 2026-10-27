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

#include "profilecontainer.h"

#include <QGraphicsLinearLayout>
#include <MButton>
#include <MLabel>
#include <MSeparator>
#include <MImageWidget>
#include <QDebug>

#undef DEBUG
#include "../debug.h"

ProfileContainer::ProfileContainer (
        int            id, 
        const QString &title, 
        bool           vibra, 
        MWidget     *parent) :
    MWidgetController (parent),
    m_ProfileId (id),
    m_Button(0),
    m_Label(0)
{
    QGraphicsLinearLayout *layout;
    
    /*
     * The layout.
     */
    layout = new QGraphicsLinearLayout (Qt::Horizontal, this);
    layout->setContentsMargins (0., 0., 0., 0.);
    SYS_DEBUG ("Creating container for %s", SYS_STR(title)); 

    /*
     * The switch button.
     */
    m_Button = new MButton;
    m_Button->setCheckable (true);
    m_Button->setViewType (MButton::switchType);
    m_Button->setStyleName ("CommonRightSwitchInverted");
    m_Button->setChecked (vibra);
    connect (m_Button, SIGNAL (toggled (bool)),
             this, SIGNAL (toggled (bool)));

    /*
     * The label.
     */
    m_Label = new MLabel (title);
    m_Label->setStyleName ("CommonSingleTitleInverted");

    /*
     * Adding these things together.
     */
    layout->addItem (m_Label);
    layout->addItem (m_Button);

    layout->setAlignment (m_Label, Qt::AlignVCenter | Qt::AlignLeft);
    layout->setAlignment (m_Button, Qt::AlignVCenter | Qt::AlignLeft);

    setLayout (layout);

    setContentsMargins (0., 0., 0., 0.);
    setStyleName ("CommonPanelInverted");

    setActive (false);
}

ProfileContainer::~ProfileContainer()
{
}

int 
ProfileContainer::id ()
{
    return m_ProfileId;
}

bool ProfileContainer::isChecked () const
{
    return m_Button->isChecked();
}

void ProfileContainer::setChecked (bool buttonChecked)
{
    m_Button->setChecked(buttonChecked);
}

void ProfileContainer::setText (const QString &text)
{
    m_Label->setText(text);
}

QString ProfileContainer::text () const
{
    return m_Label->text();
}


