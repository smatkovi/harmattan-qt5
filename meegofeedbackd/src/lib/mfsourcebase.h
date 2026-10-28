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

#ifndef MFSOURCEBASE_H
#define MFSOURCEBASE_H

#include <QObject>
#include <QMap>
#include <QString>

#include "mfsourceinterface.h"

class MfSourceBase : public QObject, public MfSourceInterface {
    Q_OBJECT
    Q_INTERFACES(MfSourceInterface)
public:
    /*!
     * \brief Class ctor.
     */
    MfSourceBase();

    /*!
     * \brief Class dtor.
     */
    virtual ~MfSourceBase();

    virtual bool init() = 0;

public slots:
    // This slot is called every time any status changes in the device.
    // Status is presented as key and value pairs. Currently supported
    // keys:
    // KEY: "display" POSSIBLE VALUES: "on", "off"
    virtual void deviceStateChanged(const QMap<QString, QString> &newState) = 0;
};

#endif
