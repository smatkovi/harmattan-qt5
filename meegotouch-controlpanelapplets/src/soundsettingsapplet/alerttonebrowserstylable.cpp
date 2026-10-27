/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#include "alerttonebrowserstylable.h"
#include "alerttonebrowser.h"

#include <QGraphicsLinearLayout>

AlertToneBrowserStylable::AlertToneBrowserStylable (
    AlertTone       *tone,
    QGraphicsWidget *parent) :
    DcpStylableWidget (parent),
    m_browser (0)
{
    setContentsMargins (0., 0., 0., 0.);

    // FIXME: maybe we should do this later...
    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout (Qt::Vertical, this);
    layout->setContentsMargins (0., 0., 0., 0.);

    m_browser = new AlertToneBrowser (tone, this);
    layout->addItem (m_browser);

    connect (m_browser, SIGNAL (closePage ()),
             this, SIGNAL (closePage ()));

    setLayout (layout);
}

AlertToneBrowserStylable::~AlertToneBrowserStylable ()
{
}

bool
AlertToneBrowserStylable::pagePans () const
{
    if (m_browser)
        return m_browser->pagePans ();
    
    return true;
}

QString
AlertToneBrowserStylable::title () const
{
    if (m_browser)
        return m_browser->title ();

    return QString ();
}

void
AlertToneBrowserStylable::accept ()
{
    m_browser->performAccept ();
}

void
AlertToneBrowserStylable::cancel ()
{
    m_browser->performCancel ();
}

