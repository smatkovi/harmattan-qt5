/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of systemui.
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

#include "statusarearenderer.h"
#include "xeventlistenerwidget.h"
#include <MStyle>
#include "statusareastyle.h"
#if QT_VERSION < 0x050000
#include <QMeeGoLivePixmap>
#include <QMeeGoGraphicsSystemHelper>
#endif
#include <QX11Info>
#include "x11wrapper.h"
#include <QPainter>
#include <X11/Xutil.h>

// Update the pixmap 5 times per second at most
static const int ACCUMULATION_INTERVAL = 200;

unsigned char StatusAreaRenderer::xErrorCode = Success;
XErrorHandler StatusAreaRenderer::oldXErrorHandler = NULL;

StatusAreaRenderer::StatusAreaRenderer(QObject *parent) :
    QObject(parent),
    scene(new QGraphicsScene),
    statusArea(new StatusArea),
    statusAreaLivePixmap(NULL),
#if QT_VERSION >= 0x050000
    statusAreaPixmap(0),
#endif
    statusAreaPropertyWindow(0),
    windowManagerWindow(0),
    windowManagerWindowWidget(NULL),
    statusBarVisible(false),
#ifdef HAVE_QMSYSTEM
    displayState(new MeeGo::QmDisplayState()),
#endif
    renderScene(true),
    previousRootWindowEventMask(0)
{
    scene->setParent(this);
    scene->setObjectName("statusareascene");
    scene->addItem(statusArea);
    statusArea->setObjectName("statusarea");

    // Get signaled when the scene changes
    connect(scene, SIGNAL(changed(QList<QRectF>)), this, SLOT(accumulateSceneChanges(QList<QRectF>)));

#ifdef HAVE_QMSYSTEM
    // Switch rendering on and off based on the display state
    connect(displayState, SIGNAL(displayStateChanged(MeeGo::QmDisplayState::DisplayState)), this, SLOT(setSceneRender(MeeGo::QmDisplayState::DisplayState)));

    // The following part is important so that widgets will know from the very beginning whether they are on display or not
    MeeGo::QmDisplayState::DisplayState state = displayState->get();
    if (state != MeeGo::QmDisplayState::Off) {
        // Set rendering state off to make all widgets think they are not on display
        setSceneRender(MeeGo::QmDisplayState::Off);
    }
    // Let the widgets know whether they are actually on display or not
    setSceneRender(state);
#endif

    // Get the required X atoms
    Display *display = QX11Info::display();
    statusBarVisibleAtom = X11Wrapper::XInternAtom(display, "_MEEGOTOUCH_STATUSBAR_VISIBLE", False);
    windowManagerWindowAtom = X11Wrapper::XInternAtom(display, "_NET_SUPPORTING_WM_CHECK", False);
    netSupportedAtom = X11Wrapper::XInternAtom(display, "_NET_SUPPORTED", False);
    shareDrawableAtom = X11Wrapper::XInternAtom(display, "XSERVER_SECURITY_POLICY_SHARE_DRAWABLE", True);

    setSizeFromStyle();
    if(!createSharedPixmapHandle() || !createBackPixmap()) {
        qWarning("Shared Pixmap was not created. Status area will not render");
    } else {
        createStatusAreaPropertyWindow();
        setSharedPixmapHandleToWindowProperty();
        setStatusAreaPropertyWindowIdToRootWindowProperty();

        if(setupStatusBarVisibleListener()) {
            updateStatusBarVisibleProperty();
        } else {
            wmWindowUnavailable();
        }

        // Work around bug 285985 - When mcompositor restarts and sets the
        // _NET_SUPPORTED atom on the root window, Qt will kill our
        // ProprtyChangeMask bit. Let the X event loop handler finish
        // and restart out quest for WM availability
        connect(this, SIGNAL(netSupportedPropertyChanged()),
                this, SLOT(wmWindowUnavailable()),
                Qt::QueuedConnection);
    }

    connect(&accumulationTimer, SIGNAL(timeout()), this, SLOT(renderAccumulatedRegion()));
}

void StatusAreaRenderer::setSizeFromStyle()
{
    const StatusAreaStyle *style = static_cast<const StatusAreaStyle *>(MTheme::style("StatusAreaStyle"));
    if (style != NULL) {
        statusAreaHeight = style->preferredSize().height();
        statusAreaWidth = style->preferredSize().width();
        MTheme::releaseStyle(style);
    }
}

bool StatusAreaRenderer::createSharedPixmapHandle()
{
    Display *display = QX11Info::display();
    Pixmap pixmap = X11Wrapper::XCreatePixmap(display, QX11Info::appRootWindow(), statusAreaWidth, statusAreaHeight, DefaultDepth(QX11Info::display(), QX11Info::appScreen()));
#if QT_VERSION < 0x050000
    statusAreaPixmap = QPixmap::fromX11Pixmap(pixmap, QPixmap::ExplicitlyShared);

    if (!statusAreaPixmap.isNull()) {
#else
    statusAreaPixmap = pixmap;

    if (statusAreaPixmap != 0) {
#endif
        if (shareDrawableAtom != None) {
            // Allow sharing the pixmap even if the X security module is loaded
            XClientMessageEvent event;
            memset(&event, 0, sizeof(XClientMessageEvent));
            event.type = ClientMessage;
#if QT_VERSION < 0x050000
            event.window = statusAreaPixmap.handle();
#else
            event.window = statusAreaPixmap;
#endif
            event.message_type = shareDrawableAtom;
            event.format = 8;
            event.data.b[0] = True;
            X11Wrapper::XSendEvent(display, DefaultRootWindow(display), False, 0, (XEvent *)&event);
        }
        return true;
    } else {
        return false;
    }
}

bool StatusAreaRenderer::createBackPixmap()
{
#if QT_VERSION >= 0x050000
    // pixels as the X server keeps a pixmap of the default (24-bit) depth
    backPixmap = QImage(statusAreaWidth, statusAreaHeight, QImage::Format_RGB32);
    return !backPixmap.isNull();
#else
    if (statusAreaLivePixmap) {
        delete statusAreaLivePixmap;
        statusAreaLivePixmap = NULL;
    }

    if (QMeeGoGraphicsSystemHelper::isRunningMeeGo()) {
        statusAreaLivePixmap = QMeeGoLivePixmap::livePixmapWithSize(statusAreaWidth, statusAreaHeight, QMeeGoLivePixmap::Format_RGB16);
        backPixmap = QPixmap::fromX11Pixmap(statusAreaLivePixmap->handle(), QPixmap::ExplicitlyShared);
    } else {
        backPixmap = QPixmap(statusAreaWidth, statusAreaHeight);
    }

    if (!backPixmap.isNull()) {
        return true;
    } else {
        return false;
    }
#endif
}

uint StatusAreaRenderer::sharedPixmapHandle()
{
#if QT_VERSION < 0x050000
    return static_cast<quint32> (statusAreaPixmap.handle());
#else
    return static_cast<quint32> (statusAreaPixmap);
#endif
}

void StatusAreaRenderer::createStatusAreaPropertyWindow()
{
    Display *dpy = QX11Info::display();
    XSetWindowAttributes attr;
    statusAreaPropertyWindow = X11Wrapper::XCreateWindow(dpy, DefaultRootWindow(dpy),
                                       0, 0,
                                       2, 2,
                                       0,
                                       None,
                                       InputOnly,
                                       None,
                                       None,
                                       &attr);
}

void StatusAreaRenderer::setStatusAreaPropertyWindowIdToRootWindowProperty()
{
    if (statusAreaPropertyWindow == 0) {
        return;
    }

    /* Set the property window id to root window property */
    Atom propertyWindowAtom = X11Wrapper::XInternAtom(QX11Info::display(), "_MEEGOTOUCH_STATUSBAR_PROPERTY_WINDOW", False);
    Window rootWindow = QX11Info::appRootWindow(QX11Info::appScreen());

    X11Wrapper::XChangeProperty(QX11Info::display(),    //display
                    rootWindow,                         //window
                    propertyWindowAtom,                 //property
                    XA_WINDOW,                          //type
                    32,                                 //format
                    PropModeReplace,
                    (unsigned char*)&statusAreaPropertyWindow,  //data
                    1);
}

void StatusAreaRenderer::setSharedPixmapHandleToWindowProperty()
{
    if (statusAreaPropertyWindow == 0) {
        return;
    }
    // Create atom for pixmap handle property
    Atom pixmapHandleAtom = X11Wrapper::XInternAtom(QX11Info::display(), "_MEEGOTOUCH_STATUSBAR_PIXMAP", False);
    unsigned int handle = sharedPixmapHandle();

    X11Wrapper::XChangeProperty(QX11Info::display(),    //display
                    statusAreaPropertyWindow,           //window
                    pixmapHandleAtom,                   //property
                    XA_PIXMAP,                          //type
                    32,                                 //format
                    PropModeReplace,                    //mode
                    (unsigned char*)&handle,            //data
                    1);                                 //nelements
}

StatusAreaRenderer::~StatusAreaRenderer()
{
    XEventListener::unregisterEventFilter(this);

    scene->removeItem(statusArea);
    delete statusArea;

#if QT_VERSION < 0x050000
    if (statusAreaLivePixmap != NULL) {
        delete statusAreaLivePixmap;
    }
#endif

    if (statusAreaPropertyWindow != 0) {
        X11Wrapper::XDeleteProperty(QX11Info::display(),
                        QX11Info::appRootWindow(QX11Info::appScreen()),
                        X11Wrapper::XInternAtom(QX11Info::display(), "_MEEGOTOUCH_STATUSBAR_PROPERTY_WINDOW", False));

        X11Wrapper::XDeleteProperty(QX11Info::display(),
                        statusAreaPropertyWindow,
                        X11Wrapper::XInternAtom(QX11Info::display(), "_MEEGOTOUCH_STATUSBAR_PIXMAP", False));
    }

#if QT_VERSION < 0x050000
    if (!statusAreaPixmap.isNull()) {
        Pixmap pixmap = statusAreaPixmap.handle();
        if (pixmap != 0) {
            X11Wrapper::XFreePixmap(QX11Info::display(), pixmap);
        }
    }
#else
    if (statusAreaPixmap != 0) {
        X11Wrapper::XFreePixmap(QX11Info::display(), statusAreaPixmap);
    }
#endif

    delete windowManagerWindowWidget;

#ifdef HAVE_QMSYSTEM
    delete displayState;
#endif
}

void StatusAreaRenderer::accumulateSceneChanges(const QList<QRectF> &region)
{
    foreach(const QRectF & r, region) {
        accumulatedRegion = accumulatedRegion.united(r);
    }

    if (statusBarVisible && renderScene && !accumulationTimer.isActive()) {
        accumulationTimer.setSingleShot(true);
        accumulationTimer.start(ACCUMULATION_INTERVAL);
    }
}

void StatusAreaRenderer::renderAccumulatedRegion()
{
#if QT_VERSION >= 0x050000
    if (!accumulatedRegion.isEmpty() && statusAreaPixmap != 0 && !backPixmap.isNull()) {
        QRectF pixmapRect(backPixmap.rect());
        // Don't draw areas that are outside the pixmap
        if (accumulatedRegion.intersects(pixmapRect)) {
            QRectF sourceRect = accumulatedRegion.intersected(pixmapRect);
            QPainter painter(&backPixmap);
            painter.fillRect(sourceRect, QColor(Qt::black));
            scene->render(&painter, sourceRect, sourceRect);
            painter.end();
            copyToSharedPixmap(sourceRect.toAlignedRect().intersected(backPixmap.rect()));
        }
        accumulatedRegion = QRectF(0.0f, 0.0f, 0.0f, 0.0f);
    }
}

void StatusAreaRenderer::copyToSharedPixmap(const QRect &rect)
{
    Display *display = QX11Info::display();
    XImage *xi = XCreateImage(display, DefaultVisual(display, QX11Info::appScreen()), DefaultDepth(QX11Info::display(), QX11Info::appScreen()),
                              ZPixmap, 0, (char *) backPixmap.bits(),
                              backPixmap.width(), backPixmap.height(), 32,
                              backPixmap.bytesPerLine());
    if (xi == NULL) {
        return;
    }
    xi->byte_order = Q_BYTE_ORDER == Q_LITTLE_ENDIAN ? LSBFirst : MSBFirst;
    GC gc = XCreateGC(display, statusAreaPixmap, 0, NULL);
    // the clients see the change as damage of the shared pixmap
    XPutImage(display, statusAreaPixmap, gc, xi, rect.x(), rect.y(), rect.x(), rect.y(),
              rect.width(), rect.height());
    XFreeGC(display, gc);
    XFlush(display);
    // the data is the QImage's
    xi->data = NULL;
    XDestroyImage(xi);
#else
    if (!accumulatedRegion.isEmpty() && !statusAreaPixmap.isNull() && !backPixmap.isNull()) {
        if (statusAreaLivePixmap && !QMeeGoGraphicsSystemHelper::isRunningMeeGo()) {
            QMeeGoGraphicsSystemHelper::switchToMeeGo();
        }

        // Don't draw areas that are outside the pixmap
        if(accumulatedRegion.intersects(statusAreaPixmap.rect())) {
            QPainter painter;
            QRectF sourceRect = accumulatedRegion.intersected(statusAreaPixmap.rect());
            QImage *image = NULL;

            if (statusAreaLivePixmap) {
                image = statusAreaLivePixmap->lock();
                painter.begin(image);
            } else {
                painter.begin(&backPixmap);
            }

            if(painter.isActive()) {
                painter.fillRect(sourceRect, QColor(Qt::black));
                scene->render(&painter, sourceRect, sourceRect);
            }
            painter.end();

            if (statusAreaLivePixmap) {
                statusAreaLivePixmap->release(image);
            }

            painter.begin(&statusAreaPixmap);
            painter.drawPixmap(sourceRect, backPixmap, sourceRect);
            painter.end();
        }
        accumulatedRegion = QRectF(0.0f, 0.0f, 0.0f, 0.0f);
    }
#endif
}

bool StatusAreaRenderer::setupStatusBarVisibleListener()
{
    Atom type;
    int format;
    unsigned long length, after;
    uchar *data = 0;

    windowManagerWindow = 0;
    delete windowManagerWindowWidget;
    windowManagerWindowWidget = NULL;
    if (X11Wrapper::XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), windowManagerWindowAtom,
                           0, 1024, False, XA_WINDOW, &type, &format, &length, &after, &data) == Success) {
        if (type == XA_WINDOW && format == 32) {
            windowManagerWindow = *((Window*) data);
        }
        X11Wrapper::XFree(data);
    }

    if (windowManagerWindow != 0) {
        long wmWindowInputMask = PropertyChangeMask | StructureNotifyMask;
        trapXErrors();
        X11Wrapper::XSelectInput(QX11Info::display(), windowManagerWindow, wmWindowInputMask);
        X11Wrapper::XSync(QX11Info::display(), False);
        untrapXErrors();

        if (xErrorCode == Success) {
            XEventListener::registerEventFilter(this, wmWindowInputMask);
        } else {
            // XSelectInput failed, meaning that the window manager window
            // property on the root window is stale
            windowManagerWindow = 0;
        }
    }

    if (windowManagerWindow != 0) {
        windowManagerWindowWidget = new XEventListenerWidget(NULL, windowManagerWindow);
    }

    return (windowManagerWindow != 0);
}

bool StatusAreaRenderer::updateStatusBarVisibleProperty()
{
    Atom type;
    int format;
    unsigned long length, after;
    uchar *data = 0;

    bool oldStatusBarVisible = statusBarVisible;
    bool success = false;
    if (X11Wrapper::XGetWindowProperty(QX11Info::display(), windowManagerWindow, statusBarVisibleAtom,
                           0, 1024, False, XA_CARDINAL, &type, &format, &length, &after, &data) == Success) {
        if (type == XA_CARDINAL) {
            statusBarVisible = *data;
            success = true;
        }
        X11Wrapper::XFree(data);
    }

    if(!success) {
        // Assume status bar is visible when WM window or _MEEGOTOUCH_STATUSBAR_VISIBLE property are not available
        statusBarVisible = true;
    }

    if (statusBarVisible && !oldStatusBarVisible) {
        // Render any changes that have accumulated while the status bar was not visible
        renderAccumulatedRegion();
    }

    return success;
}

bool StatusAreaRenderer::xEventFilter(const XEvent &event)
{
    bool propertyReadSuccess = true;

    if (event.xproperty.window == windowManagerWindow && event.xproperty.atom == statusBarVisibleAtom) {
        // Status bar visibility property changed, get new visibility value
        propertyReadSuccess = updateStatusBarVisibleProperty();
    } else if (event.xproperty.window == QX11Info::appRootWindow() && event.xproperty.atom == windowManagerWindowAtom) {
        // Window manager window available, stop tracking root window properties
        stopTrackingRootWindowProperties();
        propertyReadSuccess = setupStatusBarVisibleListener();
        if(propertyReadSuccess) {
            updateStatusBarVisibleProperty();
        }
    } else if (event.xproperty.window == QX11Info::appRootWindow() && event.xproperty.atom == netSupportedAtom) {
        // _NET_SUPPORTED atom change detected, notify interested parties
        emit netSupportedPropertyChanged();
    } else if (event.type == DestroyNotify && event.xdestroywindow.window == windowManagerWindow) {
        // Window manager window destroyed - window manager is unavailable
        propertyReadSuccess = false;
    }

    // If getting the visibility atom failed anywhere above, consider the
    // window manager (still) unavailable
    if(!propertyReadSuccess) {
        wmWindowUnavailable();
    }

    return false;
}

void StatusAreaRenderer::startTrackingRootWindowProperties()
{
    // Start listening root window property changes to be notified when wm window is available
    XWindowAttributes attributes;
    previousRootWindowEventMask = 0;
    if (X11Wrapper::XGetWindowAttributes(QX11Info::display(), QX11Info::appRootWindow(), &attributes) == Success) {
        previousRootWindowEventMask = attributes.your_event_mask;
    }
    X11Wrapper::XSelectInput(QX11Info::display(), QX11Info::appRootWindow(), previousRootWindowEventMask | PropertyChangeMask);
    XEventListener::registerEventFilter(this, PropertyChangeMask);
}

void StatusAreaRenderer::stopTrackingRootWindowProperties()
{
    X11Wrapper::XSelectInput(QX11Info::display(), QX11Info::appRootWindow(), previousRootWindowEventMask);
}

void StatusAreaRenderer::wmWindowUnavailable()
{
    windowManagerWindow = 0;
    delete windowManagerWindowWidget;
    windowManagerWindowWidget = NULL;

    // Assume status bar is visible when WM window is not available
    statusBarVisible = true;

    startTrackingRootWindowProperties();

    // Check if the WM is already available
    if(setupStatusBarVisibleListener() && updateStatusBarVisibleProperty()) {
        stopTrackingRootWindowProperties();
    }
}

#ifdef HAVE_QMSYSTEM
void StatusAreaRenderer::setSceneRender(MeeGo::QmDisplayState::DisplayState state)
{
    bool oldRenderScene = renderScene;

    switch(state) {
    case MeeGo::QmDisplayState::Dimmed:
    case MeeGo::QmDisplayState::Off:
        renderScene = false;
        break;
    case MeeGo::QmDisplayState::On:
        renderScene = true;
        break;
    default:
        renderScene = true;
    }

    if (renderScene != oldRenderScene) {
        MOnDisplayChangeEvent::State state = renderScene ?
           MOnDisplayChangeEvent::FullyOnDisplay : MOnDisplayChangeEvent::FullyOffDisplay;

        MOnDisplayChangeEvent event(state, QRectF());
        foreach(QGraphicsItem *item, scene->items()) {
            scene->sendEvent(item, &event);
        }

        if (renderScene) {
            // Render any changes that have accumulated while the display was off
            renderAccumulatedRegion();
        }
    }
}
#endif

int StatusAreaRenderer::handleXError(Display* dpy, XErrorEvent *event)
{
    xErrorCode = event->error_code;
    return oldXErrorHandler ? oldXErrorHandler(dpy, event) : 0;
}

void StatusAreaRenderer::trapXErrors()
{
    xErrorCode = Success;
    oldXErrorHandler = X11Wrapper::XSetErrorHandler(handleXError);
}

void StatusAreaRenderer::untrapXErrors()
{
    X11Wrapper::XSetErrorHandler(oldXErrorHandler);
}
