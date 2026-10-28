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

// QtGlobal contains defines for Q_WS_X11
#include <QtGlobal>
#ifdef Q_WS_X11

#include "mreactionmap_p.h"

#include <QCoreApplication>
#include <QX11Info>

// Include after Qt headers because mxclientmessagelistener.h includes
// X headers
#include "mxclientmessagelistener.h"

namespace {
    static QCoreApplication::EventFilter previousEventFilter;
}

MXClientMessageListener *MXClientMessageListener::instance = NULL;

MXClientMessageListener::MXClientMessageListener()
{
    instance = this;

    minimizeAnimationAtom = XInternAtom(QX11Info::display(),
                                        "_MEEGOTOUCH_MINIMIZE_ANIMATION", False);

    // Setup event filter to follow _MEEGOTOUCH_MINIMIZE_ANIMATION client messages
    previousEventFilter = QCoreApplication::instance()->setEventFilter(xMessageEventFilter);
}

MXClientMessageListener::~MXClientMessageListener()
{
    instance = NULL;

    QCoreApplication *app = QCoreApplication::instance();
    if (app) {
        if (previousEventFilter) {
            app->setEventFilter(previousEventFilter);
            previousEventFilter = NULL;
        } else {
            app->setEventFilter(NULL);
        }
    }
}

bool MXClientMessageListener::xMessageEventFilter(void *message, long *result)
{
    XEvent *xevent = static_cast<XEvent*>(message);

    if (xevent->type == ClientMessage) {
        instance->handleClientMessage(&xevent->xclient);
    }

    bool handled = false;
    if (previousEventFilter && previousEventFilter != xMessageEventFilter) {
        handled = previousEventFilter(message, result);
    }

    return handled;
}

void MXClientMessageListener::handleClientMessage(const XClientMessageEvent *clientMessage)
{
    quint32 windowId;
    bool active;
    bool gotMessage = false;

    if (clientMessage->message_type == minimizeAnimationAtom) {
        switch (clientMessage->data.l[0]) {
        // Magic values 0, 1 and 100 are undocumented MCompositor values.
        case 0:   // minimizing was cancelled
            gotMessage = true;
            active = true;
            windowId = clientMessage->window;
            break;
        case 1:   // minimizing started
            gotMessage = true;
            active = false;
            windowId = clientMessage->window;
            break;
        case 100: // minimizing finished
            gotMessage = true;
            active = true;
            windowId = clientMessage->window;
            break;
        default:
            break;
        }
    }

    if (gotMessage) {
        foreach(MReactionMapPrivate* reactionMap, listeners) {
            if (reactionMap->topLevelWidgetWinId == windowId) {
                if (reactionMap->active != active) {
                    reactionMap->active = active;
                    reactionMap->reactionMapConnection->updateReactionMapData(reactionMap);
                }
                break;
            }
        }
    }
}

#endif


