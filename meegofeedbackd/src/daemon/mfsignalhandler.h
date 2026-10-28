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

#ifndef MFSIGNALHANDLER_H
#define MFSIGNALHANDLER_H

#include <QObject>
#include <QVector>
#include <QMap>

class QSocketNotifier;

/*!
 * \class MfSignalHandler
 * \brief Class that listens to native signals and translate them into QT signals
 */
class MfSignalHandler : public QObject {
    Q_OBJECT
public:
    MfSignalHandler();
    virtual ~MfSignalHandler();
    bool init(int signal = 0);

    //this function should be called prior to init
    void catchSignal(int signal);

    //returns amount of registered handlers, intended for unit tests
    static int count();

    //blocks all native signals for calling thread
    static void blockAll();
signals:
    void signalReceived(int sig);

private slots:
    void retranslate();

private:
    QSocketNotifier *notify;
    QVector<int> watchOver;
    int fd[2];
    static QMap<int, int> signal2fd;
    static void onSignal(int signal);

    Q_DISABLE_COPY(MfSignalHandler)
};

#endif

