/***************************************************************************
**
** Qt 5 has no X11 pixmaps behind QPixmap (QPixmap::fromX11Pixmap() and
** QPixmap::handle() are gone): these go between an X drawable and a
** QImage with Xlib.
**
** This file is part of mcompositor.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef MX11PIXMAP_H
#define MX11PIXMAP_H

#include <QImage>
#include <QSize>

// X's Drawable and Pixmap (XIDs), without Xlib's macros here
typedef unsigned long MX11Drawable;

//! The size of an X drawable, empty if it is gone.
QSize mX11DrawableSize(MX11Drawable drawable);

//! The contents of an X drawable, a null image if it is gone.
QImage mImageFromX11Drawable(MX11Drawable drawable);

//! A new X pixmap of the default depth (or 32 bits when the image has
//! alpha and there is a 32 bit visual) with the image in it, None on
//! failure.  The caller frees it with XFreePixmap().
MX11Drawable mX11PixmapFromImage(const QImage &image);

#endif
