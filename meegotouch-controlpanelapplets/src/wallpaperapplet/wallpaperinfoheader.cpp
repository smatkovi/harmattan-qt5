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

#include "wallpaperinfoheader.h"

#include <MLabel>

#include <QGraphicsLinearLayout>
#include <QPixmap>

#undef DEBUG
#include "../debug.h"

WallpaperInfoHeader::WallpaperInfoHeader (
        QGraphicsWidget *parent) :
    MStylableWidget (parent)
{
    QGraphicsLinearLayout    *HBox;

    SYS_DEBUG ("");

    m_Label = new MLabel;
    m_Label->setObjectName ("WallpaperInfoHeaderLabel");
    m_Label->setWordWrap (true);

    HBox = new QGraphicsLinearLayout (Qt::Horizontal);
    HBox->addItem (m_Label);

    setLayout (HBox);
    retranslateUi ();
}

WallpaperInfoHeader::~WallpaperInfoHeader ()
{
}


void
WallpaperInfoHeader::retranslateUi ()
{
    //% "Turn device to set wallpaper in another orientation"
    m_Label->setText(qtTrId ("qtn_wall_info_turn_device"));
}


