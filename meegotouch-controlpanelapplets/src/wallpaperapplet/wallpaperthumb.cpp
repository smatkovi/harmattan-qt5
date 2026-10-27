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
#include "wallpaperthumb.h"

static WallpaperThumb *wt_instance = 0;

WallpaperThumb::WallpaperThumb ()
{
    m_thumbnailer = 0;
}

WallpaperThumb::~WallpaperThumb ()
{
    delete m_thumbnailer;
    m_thumbnailer = 0;

    wt_instance = 0;
}

WallpaperThumb *
WallpaperThumb::getInstance ()
{
    if (! wt_instance)
        wt_instance = new WallpaperThumb;

    return wt_instance;
}

Thumbnailer *
WallpaperThumb::thumbnailer ()
{
    if (! m_thumbnailer)
        m_thumbnailer = new Thumbnailer;

    return m_thumbnailer;
}

