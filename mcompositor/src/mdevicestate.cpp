/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of mcompositor.
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

// MCE's display state and touch screen lock, whatever the GL (Nokia's
// GLES2 builds were the device builds; the Qt 5 port's is GLX everywhere,
// where the display going off was never heard of: the windows were never
// told they were obscured, damage tracked and pinging went on)
#include <QtDBus>
#if defined(__has_include)
#if __has_include(<mce/dbus-names.h>)
#include <mce/dbus-names.h>
#include <mce/mode-names.h>
#endif
#endif
#ifndef MCE_SERVICE
// (mce-dev's names, where its headers are not installed)
#define MCE_SERVICE                "com.nokia.mce"
#define MCE_REQUEST_PATH           "/com/nokia/mce/request"
#define MCE_REQUEST_IF             "com.nokia.mce.request"
#define MCE_SIGNAL_PATH            "/com/nokia/mce/signal"
#define MCE_SIGNAL_IF              "com.nokia.mce.signal"
#define MCE_DISPLAY_SIG            "display_status_ind"
#define MCE_DISPLAY_STATUS_GET     "get_display_status"
#define MCE_TKLOCK_MODE_SIG        "tklock_mode_ind"
#define MCE_TKLOCK_MODE_GET        "get_tklock_mode"
#define MCE_DISPLAY_OFF_STRING     "off"
#define MCE_TK_UNLOCKED            "unlocked"
#endif

#include "mdevicestate.h"

void MDeviceState::mceDisplayStatusIndSignal(QString mode)
{
    if (mode == MCE_DISPLAY_OFF_STRING) {
        display_off = true;
        emit displayStateChange(true);
    } else {  // "on" or "dimmed", or "lpm": an mce with a low power mode
              // shows a stand-by screen then, whose client draws on X
        if (display_off) {
            display_off = false;
            emit displayStateChange(false);
        }
    }
}

void MDeviceState::mceTouchScreenLockSignal(QString mode)
{
    if (tsmode_call) {
        delete tsmode_call;
        tsmode_call = 0;
    }
    touchScreenLockMode = mode;
}

void MDeviceState::gotDisplayStatus(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QString> reply = *watcher;
    if (reply.isError()) {
        qDebug() << __func__ << "getting display state failed:"
                 << reply.error().message();
        goto away;
    }
    mceDisplayStatusIndSignal(reply);

away:
    delete display_call;
    display_call = 0;
}

void MDeviceState::gotTouchScreenLockMode(QDBusPendingCallWatcher *watcher)
{
    QDBusPendingReply<QString> reply = *watcher;
    if (reply.isError()) {
        qDebug() << __func__ << "getting touch screen lock mode failed:"
                 << reply.error().message();
        goto away;
    }
    touchScreenLockMode = reply;

away:
    delete tsmode_call;
    tsmode_call = 0;
}

void MDeviceState::callPropChanged()
{
    QString val = call_prop->value().toString();
    incoming_call = false;
    if (val == "active") {
        ongoing_call = true;
        emit callStateChange(true);
    } else if (val == "alerting") {
        incoming_call = true;
        ongoing_call = false;
        emit incomingCall();
    } else {
        ongoing_call = false;
        emit callStateChange(false);
    }
}

void MDeviceState::topPropChanged()
{
    screen_topedge = top_prop->value().toString();
    emit screenTopEdgeChange(screen_topedge);
}

void MDeviceState::flatPropChanged()
{
    is_flat = flat_prop->value().toBool();
    emit isFlatChange(is_flat);
}

MDeviceState::MDeviceState(QObject* parent)
    : QObject(parent),
      ongoing_call(false),
      incoming_call(false),
      screen_topedge("top")
{
    display_off = false;

    call_prop = new ContextProperty("Phone.Call");
    connect(call_prop, SIGNAL(valueChanged()), this, SLOT(callPropChanged()));
    top_prop = new ContextProperty("Screen.TopEdge");
    connect(top_prop, SIGNAL(valueChanged()), this, SLOT(topPropChanged()));
    topPropChanged();
    flat_prop = new ContextProperty("Position.IsFlat");
    connect(flat_prop, SIGNAL(valueChanged()), this, SLOT(flatPropChanged()));
    flatPropChanged();

    touchScreenLockMode = MCE_TK_UNLOCKED;
    systembus_conn = new QDBusConnection(QDBusConnection::systemBus());
    if (!systembus_conn->isConnected())
        qWarning("Failed to connect to the D-Bus system bus");

    systembus_conn->connect(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
                            MCE_DISPLAY_SIG, this,
                            SLOT(mceDisplayStatusIndSignal(QString)));
    systembus_conn->connect(MCE_SERVICE, MCE_SIGNAL_PATH, MCE_SIGNAL_IF,
                            MCE_TKLOCK_MODE_SIG, this,
                            SLOT(mceTouchScreenLockSignal(QString)));

    // get the initial state of touch screen lock
    tsmode_call = new QDBusPendingCallWatcher(
        systembus_conn->asyncCall(
            QDBusMessage::createMethodCall(MCE_SERVICE,
                                           MCE_REQUEST_PATH,
                                           MCE_REQUEST_IF,
                                           MCE_TKLOCK_MODE_GET)), this);
    connect(tsmode_call, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(gotTouchScreenLockMode(QDBusPendingCallWatcher*)));

    // get the initial state of the display
    display_call = new QDBusPendingCallWatcher(
        systembus_conn->asyncCall(
            QDBusMessage::createMethodCall(MCE_SERVICE,
                                           MCE_REQUEST_PATH,
                                           MCE_REQUEST_IF,
                                           MCE_DISPLAY_STATUS_GET)), this);
    connect(display_call, SIGNAL(finished(QDBusPendingCallWatcher*)),
            this, SLOT(gotDisplayStatus(QDBusPendingCallWatcher*)));
}

MDeviceState::~MDeviceState()
{
    delete call_prop;
    call_prop = 0;
}
