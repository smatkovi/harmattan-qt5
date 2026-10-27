/***************************************************************************
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

// Qt's headers before Xlib's (its Status and Bool macros)
#include <QDBusConnection>
#include <QDBusMessage>
#include <QGLWidget>
#include <QX11Info>
#include <QtPlugin>

#include "mswipegesture.h"
#include <mcompositemanager.h>
#include <mcompositewindow.h>
#include <mwindowpropertycache.h>
#include <X11/Xlib.h>
#include <cstring>
#include <x11bool.h>

// the base position of the window while it follows the finger
static const char *BasePosProperty = "mswipegesture-base-pos";

static MCompositeManager *manager()
{
    return static_cast<MCompositeManager *>(qApp);
}

MSwipeGesture::MSwipeGesture(QObject *parent)
    : MCompositeManagerExtension(parent),
      tracking(false), edge(NoEdge), app(None), angle(0)
{
    MCompositeManager *m = manager();
    Display *dpy = QX11Info::display();
    const int w = DisplayWidth(dpy, DefaultScreen(dpy));
    const int h = DisplayHeight(dpy, DefaultScreen(dpy));
    // a strip of about 1/32 of the short side (15 px on the N9's 854x480,
    // 34 on the Pro1X's 2160x1080)
    edgeWidth = m->configInt("swipe-edge-px", 0);
    if (edgeWidth <= 0)
        edgeWidth = qMax(8, qMin(w, h) / 32);
    threshold = m->configInt("swipe-threshold-percent", 40) / 100.0;
    flickThreshold = m->configInt("swipe-flick-percent", 12) / 100.0;
    flickTime = m->configInt("swipe-flick-ms", 250);
    closeFromTop = m->configInt("swipe-down-to-close", 1) != 0;

    snapBack.setDuration(180);
    snapBack.setEasingCurve(QEasingCurve::OutCubic);
    connect(&snapBack, SIGNAL(valueChanged(QVariant)), SLOT(moveWindow(QVariant)));
    connect(&snapBack, SIGNAL(finished()), SLOT(snapBackFinished()));

    listenXEventType(ButtonPress);
    listenXEventType(ButtonRelease);
    listenXEventType(MotionNotify);
    grab();
}

MSwipeGesture::~MSwipeGesture()
{
    XUngrabButton(QX11Info::display(), Button1, AnyModifier, QX11Info::appRootWindow());
}

void MSwipeGesture::grab()
{
    // every press stops the pointer until we have looked at it: from an
    // edge it is ours, anywhere else it goes on to the window as if
    // nothing had happened (ReplayPointer)
    XGrabButton(QX11Info::display(), Button1, AnyModifier, QX11Info::appRootWindow(),
                False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                GrabModeSync, GrabModeAsync, None, None);
}

bool MSwipeGesture::x11Event(XEvent *event)
{
    switch (event->type) {
    case ButtonPress:
        if (event->xbutton.window != QX11Info::appRootWindow() || tracking)
            return false;
        if (!press(event->xbutton))
            XAllowEvents(QX11Info::display(), ReplayPointer, event->xbutton.time);
        XFlush(QX11Info::display());
        return true;
    case MotionNotify:
        if (!tracking)
            return false;
        motion(event->xmotion);
        return true;
    case ButtonRelease:
        if (!tracking)
            return false;
        release(event->xbutton);
        return true;
    }
    return false;
}

QPointF MSwipeGesture::logical(const QPointF &p, QSizeF *size) const
{
    Display *dpy = QX11Info::display();
    const qreal w = DisplayWidth(dpy, DefaultScreen(dpy));
    const qreal h = DisplayHeight(dpy, DefaultScreen(dpy));
    // MeeGo Touch rotates the scene by the orientation angle (clockwise)
    switch (angle) {
    case 90:
        if (size) *size = QSizeF(h, w);
        return QPointF(p.y(), w - p.x());
    case 180:
        if (size) *size = QSizeF(w, h);
        return QPointF(w - p.x(), h - p.y());
    case 270:
        if (size) *size = QSizeF(h, w);
        return QPointF(h - p.y(), p.x());
    default:
        if (size) *size = QSizeF(w, h);
        return p;
    }
}

QPointF MSwipeGesture::screenOffset(const QPointF &o) const
{
    switch (angle) {
    case 90:
        return QPointF(-o.y(), o.x());
    case 180:
        return QPointF(-o.x(), -o.y());
    case 270:
        return QPointF(o.y(), -o.x());
    default:
        return o;
    }
}

bool MSwipeGesture::press(const XButtonEvent &e)
{
    MCompositeManager *m = manager();
    if (snapBack.state() == QAbstractAnimation::Running)
        return false;
    Window current = currentAppWindow();
    if (current == None || current == m->desktopWindow())
        // home has its own swipes (its pages)
        return false;
    MCompositeWindow *cw = MCompositeWindow::compositeWindow(current);
    if (!cw || !cw->propertyCache() || cw->propertyCache()->dontIconify())
        // the lock screens
        return false;

    angle = cw->propertyCache()->orientationAngle() % 360;
    const QPointF p = logical(QPointF(e.x_root, e.y_root), &logicalSize);
    if (p.x() < edgeWidth)
        edge = LeftEdge;
    else if (p.x() >= logicalSize.width() - edgeWidth)
        edge = RightEdge;
    else if (p.y() < edgeWidth && closeFromTop)
        edge = TopEdge;
    else
        return false;

    // ours until the finger goes up
    XAllowEvents(QX11Info::display(), AsyncPointer, e.time);
    tracking = true;
    app = current;
    window = cw;
    start = p;
    offset = QPointF();
    pressTime.start();
    window->setProperty(BasePosProperty, window->pos());
    return true;
}

void MSwipeGesture::setPeek(bool on)
{
    MCompositeManager *m = manager();
    if (on) {
        // what is below has to be painted: composite, and show home
        m->enableCompositing(true);
        MCompositeWindow *desktop = MCompositeWindow::compositeWindow(m->desktopWindow());
        if (desktop)
            desktop->setVisible(true);
    } else {
        m->recheckVisibility();
    }
}

void MSwipeGesture::moveWindow(const QVariant &value)
{
    if (!window)
        return;
    offset = value.toPointF();
    window->setPos(window->property(BasePosProperty).toPointF() + screenOffset(offset));
    manager()->glWidget()->update();
}

void MSwipeGesture::motion(const XMotionEvent &e)
{
    if (!window) {
        tracking = false;
        return;
    }
    const QPointF p = logical(QPointF(e.x_root, e.y_root), 0);
    QPointF o;
    switch (edge) {
    case LeftEdge:
        o = QPointF(qMax<qreal>(0, p.x() - start.x()), 0);
        break;
    case RightEdge:
        o = QPointF(qMin<qreal>(0, p.x() - start.x()), 0);
        break;
    case TopEdge:
        o = QPointF(0, qMax<qreal>(0, p.y() - start.y()));
        break;
    default:
        return;
    }
    if (offset.isNull() && !o.isNull())
        setPeek(true);
    moveWindow(o);
}

void MSwipeGesture::closeCurrent()
{
    Display *dpy = QX11Info::display();
    XEvent ev;
    memset(&ev, 0, sizeof ev);
    ev.xclient.type = ClientMessage;
    ev.xclient.display = dpy;
    ev.xclient.window = app;
    ev.xclient.message_type = XInternAtom(dpy, "_NET_CLOSE_WINDOW", False);
    ev.xclient.format = 32;
    ev.xclient.data.l[0] = CurrentTime;
    ev.xclient.data.l[1] = 2;   // from a pager-like source: the user
    XSendEvent(dpy, QX11Info::appRootWindow(), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &ev);
    XFlush(dpy);
}

void MSwipeGesture::release(const XButtonEvent &)
{
    tracking = false;
    if (!window)
        return;
    const qreal size = edge == TopEdge ? logicalSize.height() : logicalSize.width();
    const qreal distance = qAbs(edge == TopEdge ? offset.y() : offset.x());
    const bool quick = pressTime.elapsed() < flickTime;
    if (edge == TopEdge && distance < edgeWidth) {
        // a tap on the status bar, which the swipe took from the
        // application: what the status bar does, open the status menu
        window->setPos(window->property(BasePosProperty).toPointF());
        setPeek(false);
        window = 0;
        QDBusConnection::sessionBus().asyncCall(QDBusMessage::createMethodCall(
            "com.meego.core.MStatusIndicatorMenu", "/statusindicatormenu",
            "com.meego.core.MStatusIndicatorMenu", "open"));
        return;
    }
    const bool done = distance >= threshold * size
                      || (quick && distance >= flickThreshold * size);
    if (!done) {
        // a peek: back where it was
        snapBack.setStartValue(offset);
        snapBack.setEndValue(QPointF());
        snapBack.start();
        return;
    }
    // back in place for the compositor's own animations
    window->setPos(window->property(BasePosProperty).toPointF());
    setPeek(false);
    if (edge == TopEdge)
        closeCurrent();
    else
        manager()->exposeSwitcher();
    window = 0;
}

void MSwipeGesture::snapBackFinished()
{
    setPeek(false);
    window = 0;
}

MCompositeManagerExtension *MSwipeGestureFactory::create()
{
    return new MSwipeGesture;
}

QString MSwipeGestureFactory::extensionName()
{
    return "swipe";
}
