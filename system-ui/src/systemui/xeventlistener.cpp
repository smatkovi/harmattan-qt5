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
#include "xeventlistener.h"

#include <QSharedPointer>
#if QT_VERSION >= 0x050000
#include <QAbstractNativeEventFilter>
#include <QCoreApplication>
#include <QX11Info>
#include <X11/Xlibint.h>
#include <xcb/xcb.h>
#include <cstring>
#endif
#include <X11/X.h>

static long maskMap[LASTEvent];

static QSharedPointer<XEventListener> eventListenerInstance;
QHash<XEventListenerFilterInterface *, long> XEventListener::filterList;
#if QT_VERSION < 0x050000
QAbstractEventDispatcher::EventFilter XEventListener::previousEventFilter = NULL;
#else
/*
 * Qt 5 has no QAbstractEventDispatcher::setEventFilter() and hands the X
 * events over in xcb's format: each becomes an XEvent through Xlib's own
 * conversion from the wire format, as in libmeegotouch's MApplication,
 * and goes to xEventFilter().
 */
class XEventListenerNativeFilter : public QAbstractNativeEventFilter
{
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, long *)
    {
        if (eventType != "xcb_generic_event_t")
            return false;
        Display *dpy = QX11Info::display();
        xcb_generic_event_t *event = static_cast<xcb_generic_event_t *>(message);
        const int type = event->response_type & ~0x80;
        if (!dpy || type < 2 || type >= LASTEvent)    // errors, replies, extensions
            return false;

        typedef Bool (*WireToEvent)(Display *, XEvent *, xEvent *);
        WireToEvent convert = XESetWireToEvent(dpy, type, 0);
        XESetWireToEvent(dpy, type, convert);
        if (!convert)
            return false;
        // Xlib reckons the 16-bit sequence number against its own request
        // count: the one it read last, so that nothing changes
        xEvent wire;
        std::memcpy(&wire, event, sizeof wire);
        wire.u.u.sequenceNumber = static_cast<CARD16>(dpy->last_request_read);
        XEvent xevent;
        std::memset(&xevent, 0, sizeof xevent);
        if (!convert(dpy, &xevent, &wire))
            return false;
        xevent.xany.display = dpy;
        return XEventListener::xEventFilter(&xevent);
    }
};
#endif

XEventListener::XEventListener()
{
}

XEventListener::~XEventListener()
{
#if QT_VERSION < 0x050000
    if (previousEventFilter) {
        QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance();
        if (dispatcher) {
            dispatcher->setEventFilter(previousEventFilter);
        }
        previousEventFilter = NULL;
    }
#endif
}

void XEventListener::setupEventMasks()
{
    maskMap[KeyPress] = KeyPressMask;
    maskMap[KeyRelease] = KeyReleaseMask;
    maskMap[ButtonPress] = ButtonPressMask;
    maskMap[ButtonRelease] = ButtonReleaseMask;
    maskMap[MotionNotify] = PointerMotionMask;
    maskMap[VisibilityNotify] = VisibilityChangeMask;
    maskMap[CreateNotify] = StructureNotifyMask|SubstructureNotifyMask;
    maskMap[DestroyNotify] = StructureNotifyMask|SubstructureNotifyMask;
    maskMap[UnmapNotify] = StructureNotifyMask|SubstructureNotifyMask;
    maskMap[MapNotify] = StructureNotifyMask|SubstructureNotifyMask;
    maskMap[PropertyNotify] = PropertyChangeMask;
    maskMap[ClientMessage] = 0xffffffff; // ClientMessages can't be masked
}

void XEventListener::setupListener()
{
    // Initialize event listener on first registration
    if (eventListenerInstance.isNull()) {
        setupEventMasks();
#if QT_VERSION < 0x050000
        previousEventFilter = QAbstractEventDispatcher::instance()->setEventFilter(xEventFilter);
#else
        QCoreApplication::instance()->installNativeEventFilter(new XEventListenerNativeFilter);
#endif
        eventListenerInstance = QSharedPointer<XEventListener>(new XEventListener);
    }
}

void XEventListener::registerEventFilter(XEventListenerFilterInterface *filter, long eventMask)
{
    setupListener();

    filterList.insert(filter, eventMask);
}

void XEventListener::unregisterEventFilter(XEventListenerFilterInterface *filter)
{
    filterList.remove(filter);
}

bool XEventListener::xEventFilter(void *message)
{
    XEvent *xevent = static_cast<XEvent*>(message);
    bool eventHandled = false;

    if (!filterList.isEmpty()) {
        QHashIterator<XEventListenerFilterInterface *, long> iterator(filterList);
        while(iterator.hasNext()) {
            iterator.next();
            if (maskMap[xevent->type] & iterator.value()) {
                if (eventHandled |= iterator.key()->xEventFilter(*xevent)) {
                    break;
                }
            }
        }
    }

#if QT_VERSION < 0x050000
    if (!eventHandled && previousEventFilter) {
        eventHandled = previousEventFilter(message);
    }
#endif

    return eventHandled;
}


