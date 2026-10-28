/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimremotewindow.h"

#include <QDebug>
#include <QX11Info>
#if QT_VERSION >= 0x050000
#include <QWindow>
#endif

#include "mimxapplication.h"
#include "mimxerrortrap.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>

#ifdef Q_WS_MAEMO_5
# include <X11/Xatom.h>
#endif

namespace {

#if QT_VERSION >= 0x050000
// Qt 5's QPixmap has no X pixmap behind it: a copy of the window's
// contents instead, read again when they change
QPixmap copyOfX11Pixmap(Pixmap xpixmap)
{
    Display *dpy = QX11Info::display();
    Window root;
    int x, y;
    unsigned int width, height, border, depth;
    if (!XGetGeometry(dpy, xpixmap, &root, &x, &y, &width, &height, &border, &depth))
        return QPixmap();
    XImage *xi = XGetImage(dpy, xpixmap, 0, 0, width, height, AllPlanes, ZPixmap);
    if (!xi)
        return QPixmap();
    QImage image;
    if (xi->bits_per_pixel == 32)
        image = QImage(reinterpret_cast<const uchar *>(xi->data), width, height,
                       xi->bytes_per_line,
                       depth == 32 ? QImage::Format_ARGB32_Premultiplied
                                   : QImage::Format_RGB32).copy();
    XDestroyImage(xi);
    return QPixmap::fromImage(image);
}
#endif

Atom wmStateAtom()
{
#ifdef Q_WS_MAEMO_5
// MB doesn't update WM_STATE when window is iconified, so, as a
// workaround, we monitor _MB_CURRENT_APP_WINDOW. This points to wid
// when on FullScreen/Active
# define ICONIFIED_ATOM_NAME "_MB_CURRENT_APP_WINDOW"
#else
# define ICONIFIED_ATOM_NAME "WM_STATE"
#endif
    static const Atom stateAtom = XInternAtom(QX11Info::display(), ICONIFIED_ATOM_NAME, false);

    return stateAtom;
}

}

MImRemoteWindow::MImRemoteWindow(WId window, MImXApplication *application) :
    QObject(0),
    wid(window),
    xpixmap(0),
    damage(0),
    pixmap(),
    redirected(false),
    mApplication(application),
    foreignWindow(0),
    imWidget(0)
{
}

MImRemoteWindow::~MImRemoteWindow()
{
    XSelectInput(QX11Info::display(), wid, 0);
    unredirect();
#if QT_VERSION >= 0x050000
    if (imWidget && imWidget->windowHandle()
        && imWidget->windowHandle()->transientParent() == foreignWindow)
        imWidget->windowHandle()->setTransientParent(0);
    delete foreignWindow;
#endif
}

void MImRemoteWindow::setIMWidget(const QWidget *widget)
{
    // Qt 5 creates the native window only when asked for it (winId()); its
    // effectiveWinId() is 0 until then
#if QT_VERSION >= 0x050000
    const WId imWindow = const_cast<QWidget *>(widget)->winId();
#else
    const WId imWindow = widget->effectiveWinId();
#endif
    XSetTransientForHint(QX11Info::display(), imWindow, wid);
#if QT_VERSION >= 0x050000
    // Qt 5's xcb sets WM_TRANSIENT_FOR itself whenever it maps a window
    // (from QWindow::transientParent(), and removes one it did not set):
    // the application's window, as a foreign QWindow, is the transient
    // parent of the input method's
    imWidget = const_cast<QWidget *>(widget);
    if (!foreignWindow)
        foreignWindow = QWindow::fromWinId(wid);
    if (imWidget->windowHandle())
        imWidget->windowHandle()->setTransientParent(foreignWindow);
#endif

    // Using PropertyChangeMask is a work-around for NB#172722 (a WONTFIX):
    XSelectInput(QX11Info::display(),
                 wid,
                 StructureNotifyMask | PropertyChangeMask);
#if QT_VERSION >= 0x050000
    // Qt 5 talks to the server through xcb and no longer flushes Xlib's
    // own request buffer
    XFlush(QX11Info::display());
#endif
}

bool MImRemoteWindow::wasIconified(XEvent *ev) const
{
    if (PropertyNotify != ev->type) {
        return false;
    }

    if (ev->xproperty.atom == wmStateAtom()) {
        return isIconified();
    }

    return false;
}

bool MImRemoteWindow::isIconified() const
{
    Atom type;
    int format;
    unsigned long length;
    unsigned long after;
    unsigned long *state;
    uchar *data = 0;

#ifdef Q_WS_MAEMO_5
# define REAL_ATOM XA_WINDOW
# define IS_ICONIFIED(_X) _X != wid
#else
# define REAL_ATOM AnyPropertyType
# define IS_ICONIFIED(_X) _X == IconicState
#endif

    int queryResult = XGetWindowProperty(QX11Info::display(), wid, wmStateAtom(), 0, 2,
                                         false, REAL_ATOM, &type, &format, &length,
                                         &after, &data);
    state = reinterpret_cast<unsigned long *>(data);

    bool result = (queryResult == Success && data && format == 32 && IS_ICONIFIED(*state));

    if (data) {
        XFree(data);
    }

    return result;
}


bool MImRemoteWindow::wasUnmapped(XEvent *ev) const
{
    return (ev->type == UnmapNotify &&
            ev->xunmap.event == wid);
}

void MImRemoteWindow::setupDamage()
{
    destroyDamage();

    damage = XDamageCreate(QX11Info::display(), wid, XDamageReportNonEmpty);
}

void MImRemoteWindow::destroyDamage()
{
    if (damage == 0)
        return;

    XDamageDestroy(QX11Info::display(), damage);
    damage = 0;
}

void MImRemoteWindow::redirect()
{
    if (redirected)
        return;

    if (mApplication->manualRedirection()) {
        MImXErrorTrap xerror(mApplication->compositeExtension(), X_CompositeRedirectWindow);
        XCompositeRedirectWindow(QX11Info::display(),
                                 wid,
                                 CompositeRedirectManual);
        if (xerror.untrap() == BadAccess)
            qDebug() << "Window " << wid << " was already redirected";
    }

    redirected = true;

    setupPixmap();
    setupDamage();
#if QT_VERSION >= 0x050000
    XFlush(QX11Info::display());    // see setIMWidget()
#endif

    Q_EMIT contentUpdated(QRegion(QRect(QPoint(), pixmap.size())));
}

void MImRemoteWindow::unredirect()
{
    if (!redirected)
        return;

    redirected = false;

    destroyDamage();
    destroyPixmap();

    if (mApplication->manualRedirection()) {
        MImXErrorTrap xerror(mApplication->compositeExtension(), X_CompositeUnredirectWindow);
        XCompositeUnredirectWindow(QX11Info::display(),
                                   wid,
                                   CompositeRedirectManual);
        if (xerror.untrap() == BadAccess)
            qDebug() << "Window " << wid << " was not redirected";
    }
}

void MImRemoteWindow::handleEvent(XEvent *event)
{
    handleConfigureNotifyEvent(event);
    handleDamageEvent(event);
}

void MImRemoteWindow::handleConfigureNotifyEvent(XEvent *event)
{
    if (event->type != ConfigureNotify)
        return;

    XConfigureEvent *e = &event->xconfigure;

    if (e->window != wid)
        return;

    resetPixmap();
}

void MImRemoteWindow::resetPixmap()
{
    if (redirected)
        setupPixmap();
}

void MImRemoteWindow::update(const QRegion &region)
{
    Q_EMIT contentUpdated(region);
}

void MImRemoteWindow::handleDamageEvent(XEvent *event)
{
    if (event->type != mApplication->damageExtension().eventBase() + XDamageNotify)
        return;

    XDamageNotifyEvent *e = reinterpret_cast<XDamageNotifyEvent*>(event);

    if (damage != e->damage)
        return;

    XserverRegion parts = XFixesCreateRegion(QX11Info::display(), 0, 0);
    XDamageSubtract(QX11Info::display(), e->damage, None, parts);

    QRegion region;

    int nrects;
    XRectangle *rects = XFixesFetchRegion(QX11Info::display(), parts, &nrects);
    if (rects) {
        for (int i = 0; i < nrects; ++i) {
            region += QRect(rects[i].x, rects[i].y, rects[i].width, rects[i].height);
        }
    }
    free(rects);

    XFixesDestroyRegion(QX11Info::display(), parts);

    // setup remote pixmap when it failed before (with Qt 5 always: the
    // pixmap is a copy)
#if QT_VERSION >= 0x050000
    setupPixmap();
#else
    if (pixmap.isNull())
        setupPixmap();
#endif

    Q_EMIT contentUpdated(region);
}

void MImRemoteWindow::setupPixmap()
{
    destroyPixmap();

    MImXErrorTrap error(mApplication->compositeExtension(), X_CompositeNameWindowPixmap);
    xpixmap = XCompositeNameWindowPixmap(QX11Info::display(), wid);
    if (error.untrap() == BadMatch) {
        qDebug() << "Cannot get offscreen reference for Window " << wid;
        xpixmap = 0;
        return;
    }

    if (xpixmap != 0) {
#if QT_VERSION >= 0x050000
        pixmap = copyOfX11Pixmap(xpixmap);
#else
        pixmap = QPixmap::fromX11Pixmap(xpixmap, QPixmap::ExplicitlyShared);
#endif
    }
}

void MImRemoteWindow::destroyPixmap()
{
    if (MIMApplication::instance())
        pixmap = QPixmap();

    if (xpixmap != 0) {
        XFreePixmap(QX11Info::display(), xpixmap);
        xpixmap = 0;
    }
}

const QPixmap &MImRemoteWindow::windowPixmap() const
{
    // setup remote pixmap when it failed before
    if (redirected && pixmap.isNull()) {
        const_cast<MImRemoteWindow*>(this)->setupPixmap();
    }

    return pixmap;
}
