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

#include <QX11Info>
#include <QElapsedTimer>
#include <x11bool.h>
#include "mxcbevent.h"
#include <X11/Xlibint.h>
#include <cstring>
#include <unistd.h>

bool mXEventFromXcb(Display *dpy, xcb_generic_event_t *event, XEvent *xevent)
{
    const int type = event->response_type & ~0x80;
    if (!dpy || type < 2)    // errors and replies
        return false;

    typedef Bool (*WireToEvent)(Display *, XEvent *, xEvent *);
    WireToEvent convert = XESetWireToEvent(dpy, type, 0);
    XESetWireToEvent(dpy, type, convert);
    if (!convert)
        return false;

    // the 32 bytes of the wire event (xcb puts the full sequence number
    // after them); Xlib reckons the 16-bit sequence number against its
    // own request count, which Qt's requests through xcb leave behind:
    // the one it read last, so that nothing changes
    xEvent wire;
    std::memcpy(&wire, event, sizeof wire);
    wire.u.u.sequenceNumber = static_cast<CARD16>(dpy->last_request_read);
    std::memset(xevent, 0, sizeof *xevent);
    if (!convert(dpy, xevent, &wire))
        return false;
    xevent->xany.display = dpy;
    return true;
}

namespace {
struct Peek {
    Display *dpy;
    MXEventPredicate predicate;
    XPointer arg;
    XEvent *found;
    bool accepted;
};
}

static bool peekOne(xcb_generic_event_t *event, void *data)
{
    Peek *p = static_cast<Peek *>(data);
    XEvent xevent;
    if (!mXEventFromXcb(p->dpy, event, &xevent))
        return false;
    if (p->predicate(p->dpy, &xevent, p->arg)) {
        if (p->found)
            *p->found = xevent;
        p->accepted = true;
        return true;    // no further
    }
    return false;
}

bool mPeekXEvents(Display *dpy, MXEventPredicate predicate, XPointer arg,
                  XEvent *found)
{
    Peek p = { dpy, predicate, arg, found, false };
    QX11Info::peekEventQueue(peekOne, &p);
    return p.accepted;
}

bool mWaitForXEvent(Display *dpy, XEvent *found, MXEventPredicate predicate,
                    XPointer arg, int timeout_ms)
{
    // what the event answers has to reach the server first
    XFlush(dpy);
    QElapsedTimer timer;
    timer.start();
    // Qt's reader thread queues the events meanwhile
    while (!mPeekXEvents(dpy, predicate, arg, found)) {
        if (timer.elapsed() > timeout_ms)
            return false;
        usleep(5000);
    }
    return true;
}
