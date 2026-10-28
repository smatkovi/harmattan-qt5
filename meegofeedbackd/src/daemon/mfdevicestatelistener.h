/* This file is part of meegofeedbackd
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

#ifndef MFDEVICESTATELISTENER_H
#define MFDEVICESTATELISTENER_H

#include <QObject>
#include <QString>
#include <QMap>
#include <contextproperty.h>

class MfDeviceStateListener: public QObject
{
    Q_OBJECT

public:

    MfDeviceStateListener(QObject* parent = 0);
    ~MfDeviceStateListener();

    void init();

    QMap<QString, QString> deviceState();

signals:
    // This signal is emitted every time any status changes in the device.
    // Status is presented as key and value pairs. Currently supported
    // keys:
    // KEY: "display" POSSIBLE VALUES: "on", "off"
    void deviceStateChanged(const QMap<QString, QString>& newState);

private slots:
    void sessionStateChanged();

private:
    bool isDisplayActive;
    ContextProperty *screenBlankedProperty;
};

#endif
