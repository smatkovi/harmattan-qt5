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
#ifndef WALLPAPER_THUMB_H
#define WALLPAPER_THUMB_H

/*
 * In the functional tests we use the real thing, in the unit tests we use the
 * stubbed version. 
 */
#if defined(UNIT_TEST) && !defined(FUNCTIONAL_TEST)
#  include "thumbnailerstub.h"
#else
#  include <Thumbnailer>
#endif

using namespace Thumbnails;

class WallpaperThumb
{
public:
    ~WallpaperThumb ();
    static WallpaperThumb  * getInstance ();
    Thumbnailer            * thumbnailer ();

protected:
    WallpaperThumb ();

private:
    Thumbnailer     *m_thumbnailer;
};

#endif

