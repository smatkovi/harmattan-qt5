/***************************************************************************
**
** Qt 5: Qt's xcb layer reads the X events (in a thread of its own) and
** Xlib's event queue stays empty, so XIfEvent() would wait forever and
** XCheckIfEvent() would find nothing.  These look into the events Qt has
** read but not delivered yet instead; Qt still delivers them as usual
** (XCheckIfEvent() with a predicate that returns False and XIfEvent()
** for an event that is handled again later work alike).
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

#ifndef MXCBEVENT_H
#define MXCBEVENT_H

#include <X11/Xlib.h>
#include <xcb/xcb.h>

typedef Bool (*MXEventPredicate)(Display *, XEvent *, XPointer);

//! An xcb event as Xlib's XEvent, through Xlib's conversion from the wire
//! format (of the core protocol and of the extensions registered with the
//! Display); false for errors, replies and unknown events.
bool mXEventFromXcb(Display *dpy, xcb_generic_event_t *event, XEvent *xevent);

//! Like XCheckIfEvent() with a predicate that returns False: shows the
//! predicate each event that is waiting.  True when it returned True.
bool mPeekXEvents(Display *dpy, MXEventPredicate predicate, XPointer arg,
                  XEvent *found = 0);

//! Like XIfEvent(), but the event stays queued: waits at most @timeout_ms
//! for an event the predicate accepts.  False on timeout.
bool mWaitForXEvent(Display *dpy, XEvent *found, MXEventPredicate predicate,
                    XPointer arg, int timeout_ms = 5000);

#endif
