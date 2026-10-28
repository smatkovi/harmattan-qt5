/* This file is part of meegofeedback-reactionmaps
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MXCLIENTMESSAGELISTENER_H
#define MXCLIENTMESSAGELISTENER_H

#ifdef Q_WS_X11

#include <QList>
#include <X11/Xlib.h>

class MReactionMapPrivate;

class MXClientMessageListener
{
public:
    MXClientMessageListener();
    ~MXClientMessageListener();

    static bool xMessageEventFilter(void *message, long *result);

    QList<MReactionMapPrivate *> listeners;

private:
    void handleClientMessage(const XClientMessageEvent *clientMessage);

    static MXClientMessageListener *instance;
    Atom minimizeAnimationAtom;
};
#endif

#endif
