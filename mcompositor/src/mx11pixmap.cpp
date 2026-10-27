/***************************************************************************
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

#include "mx11pixmap.h"

#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>

static bool drawableGeometry(MX11Drawable drawable, unsigned *w, unsigned *h,
                             unsigned *depth)
{
    Window root;
    int x, y;
    unsigned border;
    if (!drawable)
        return false;
    return XGetGeometry(QX11Info::display(), drawable, &root, &x, &y,
                        w, h, &border, depth) != 0;
}

QSize mX11DrawableSize(MX11Drawable drawable)
{
    unsigned w, h, depth;
    if (!drawableGeometry(drawable, &w, &h, &depth))
        return QSize();
    return QSize(w, h);
}

QImage mImageFromX11Drawable(MX11Drawable drawable)
{
    unsigned w, h, depth;
    if (!drawableGeometry(drawable, &w, &h, &depth) || !w || !h)
        return QImage();
    XImage *xi = XGetImage(QX11Info::display(), drawable, 0, 0, w, h,
                           AllPlanes, ZPixmap);
    if (!xi)
        return QImage();

    QImage image;
    if (xi->bits_per_pixel == 32 && xi->byte_order == LSBFirst
        && Q_BYTE_ORDER == Q_LITTLE_ENDIAN) {
        // the pixels as QImage keeps them (0xAARRGGBB in host order)
        image = QImage(w, h, depth == 32 ? QImage::Format_ARGB32_Premultiplied
                                         : QImage::Format_RGB32);
        for (unsigned y = 0; y < h; ++y)
            memcpy(image.scanLine(y), xi->data + y * xi->bytes_per_line, w * 4);
    } else if (xi->bits_per_pixel == 16 && depth == 16
               && xi->byte_order == LSBFirst && Q_BYTE_ORDER == Q_LITTLE_ENDIAN) {
        image = QImage(w, h, QImage::Format_RGB16);
        for (unsigned y = 0; y < h; ++y)
            memcpy(image.scanLine(y), xi->data + y * xi->bytes_per_line, w * 2);
    } else {
        // anything else, slowly, pixel by pixel through the visual's masks
        image = QImage(w, h, QImage::Format_RGB32);
        unsigned long rm = xi->red_mask, gm = xi->green_mask, bm = xi->blue_mask;
        int rs = rm ? __builtin_ctzl(rm) : 0, gs = gm ? __builtin_ctzl(gm) : 0,
            bs = bm ? __builtin_ctzl(bm) : 0;
        int rmax = rm >> rs, gmax = gm >> gs, bmax = bm >> bs;
        for (unsigned y = 0; y < h; ++y) {
            QRgb *line = (QRgb *) image.scanLine(y);
            for (unsigned x = 0; x < w; ++x) {
                unsigned long p = XGetPixel(xi, x, y);
                line[x] = qRgb(rmax ? ((p & rm) >> rs) * 255 / rmax : 0,
                               gmax ? ((p & gm) >> gs) * 255 / gmax : 0,
                               bmax ? ((p & bm) >> bs) * 255 / bmax : 0);
            }
        }
    }
    XDestroyImage(xi);
    return image;
}

MX11Drawable mX11PixmapFromImage(const QImage &source)
{
    Display *dpy = QX11Info::display();
    int screen = QX11Info::appScreen();
    if (source.isNull())
        return None;

    int depth = DefaultDepth(dpy, screen);
    Visual *visual = DefaultVisual(dpy, screen);
    XVisualInfo vi;
    if (source.hasAlphaChannel()
        && XMatchVisualInfo(dpy, screen, 32, TrueColor, &vi)) {
        depth = 32;
        visual = vi.visual;
    }
    if (depth != 24 && depth != 32)
        // not on the devices this is for
        return None;

    QImage image = source.convertToFormat(depth == 32
                                          ? QImage::Format_ARGB32_Premultiplied
                                          : QImage::Format_RGB32);
    XImage *xi = XCreateImage(dpy, visual, depth, ZPixmap, 0,
                              (char *) image.bits(), image.width(),
                              image.height(), 32, image.bytesPerLine());
    if (!xi)
        return None;
    xi->byte_order = Q_BYTE_ORDER == Q_LITTLE_ENDIAN ? LSBFirst : MSBFirst;

    Pixmap pixmap = XCreatePixmap(dpy, RootWindow(dpy, screen),
                                  image.width(), image.height(), depth);
    GC gc = XCreateGC(dpy, pixmap, 0, 0);
    XPutImage(dpy, pixmap, gc, xi, 0, 0, 0, 0, image.width(), image.height());
    XFreeGC(dpy, gc);
    // the data is the QImage's
    xi->data = 0;
    XDestroyImage(xi);
    return pixmap;
}
