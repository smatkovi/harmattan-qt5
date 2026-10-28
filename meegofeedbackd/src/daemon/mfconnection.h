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

#ifndef MFCONNECTION_H
#define MFCONNECTION_H

#include <QObject>
#include <QPointer>
#include <QString>

class MfSession;
class QLocalSocket;

class MfConnection : public QObject
{
    Q_OBJECT
public:
    explicit MfConnection(QLocalSocket *connection, QObject *parent = 0);
    virtual ~MfConnection();

    enum FeedbackProtocolVersion {
        Unknown   = 0,
        Version_1 = 1
    };

private slots:
    void readSocketData();
    void setupReady();
    void killMe();

private:
    void fetchPlaybackRequest();
    void setup();
    void terminate();
    void goDoomed();

    enum {
        StateClientSetup,
        StateWaitingSession,
        StatePlayback,
        StateTerminated,
        StateDoomed
    } state;

    QPointer<QLocalSocket> socket;
    MfSession* session;
    QString clientName;
    FeedbackProtocolVersion clientFeedbackProtocolVersion;
};

#endif
