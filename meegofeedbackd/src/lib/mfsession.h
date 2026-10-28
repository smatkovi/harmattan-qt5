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

#ifndef MFSESSION_H
#define MFSESSION_H

#include <QObject>
#include <QString>
#include <QHash>
#include "mffeedback.h"

class MfSession : public QObject
{
    Q_OBJECT
public:
    QHash<QString, MfFeedback*> feedbackHash;

    explicit MfSession(const QString &applicationName, QObject *parent = 0);
    virtual ~MfSession();

    const QString &applicationName() {
        return appName;
    }

    bool isSetupReady() {
        return setupFinished;
    }

signals:
    void setupReady();

private slots:
    void setup();

public slots:
    void themeChanged(const QString& themeName);

private:
    QString appName;
    bool setupFinished;

    #ifdef MF_DEBUG
    void printHash();
    #endif
};

#endif
