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

#include "mfconnection.h"

#include <QDir>
#include <QLocalSocket>
#include <QtDebug>
#include <QTimer>

#include "mffeedback.h"
#include "mfsession.h"
#include "mfutil.h"

namespace {
    const QString FeedbackProtocolVersionPrefix("FeedbackProtocolVersion#");
}

MfConnection::MfConnection(QLocalSocket *connection, QObject *parent)
    : QObject(parent), state(StateClientSetup), socket(connection), session(NULL),
      clientFeedbackProtocolVersion(Unknown)
{
    // Let's reparent the connect and take the ownership instead of
    // the QLocalServer to avoid double delete of the object
    socket->setParent(this);

    connect(socket, SIGNAL(readyRead()), SLOT(readSocketData()));
    // The deleteLater() signal triggers the deletion of the MfConnection
    // object along with the associated socket (QLocalSocket) instance.
    // Let's use the killMe wrapper function because the deletion can be triggered during
    // session setup if the sample upload in the backend uses a
    // local main loop and goes to deadlock.
    connect(socket, SIGNAL(disconnected()), SLOT(killMe()));

    if (socket->bytesAvailable() > 0) {
        // Avoid the setup during the ctor call
        setup();
    }
}

MfConnection::~MfConnection()
{
    if (session) {
        qDebug() << "MfConnection: Session with client"
                 << session->applicationName() << "has ended.";
    } else {
        qDebug() << "MfConnection: Destroyed. Session creation didn't happen.";
    }
}

void MfConnection::readSocketData()
{
    if (state == StatePlayback) {
        while (socket->bytesAvailable() > 0) {
            fetchPlaybackRequest();
        }
    } else if (state == StateDoomed || state == StateWaitingSession) {
        // Discard all data when in doomed state or waiting
        // for session creation to finish.
        socket->readAll();
    } else {
        setup();
    }
}

void MfConnection::setupReady()
{
    // Session setup has ended
    // We're now ready to receive playback requests
    state = StatePlayback;
    qDebug() << "Session setup finished for" << clientName << ". Ready for playback";
}

void MfConnection::killMe()
{
    if (state == StateWaitingSession)
    {
        // Unable to delete the instance before the session creation is done
        QTimer::singleShot(10, this, SLOT(killMe()));
    } else {
        // Schedule the deletion
        deleteLater();
    }
}

void MfConnection::fetchPlaybackRequest()
{
    QDataStream socketStream(socket);
    QString feedbackName;

    socketStream >> feedbackName;

    if (socketStream.status() != QDataStream::Ok) {
        terminate();
        return;
    }

    if (timeStamping()) {
        writeTimestamp("daemon_play");
    }
    if (!session->feedbackHash.contains(feedbackName)) {
        // Feedback not found, do nothing
        return;
    }

    session->feedbackHash.value(feedbackName)->emitPlay(getTimestamp());
}

void MfConnection::setup()
{
    QDataStream socketStream(socket);
    QString feedbackProtocolVersion;

    // Get protocol version string from stream
    socketStream >> feedbackProtocolVersion;

    if (feedbackProtocolVersion.startsWith(FeedbackProtocolVersionPrefix)) {
        // String contains protocol version, see if it something
        // that this version supports.
        int protocolVersion;
        bool conversionOk;

        feedbackProtocolVersion.remove(0, FeedbackProtocolVersionPrefix.length());
        protocolVersion = feedbackProtocolVersion.toInt(&conversionOk);
        if (conversionOk == true && protocolVersion == Version_1) {
            clientFeedbackProtocolVersion = Version_1;
            socketStream >> clientName;
        }
    } else {
        // String does not contain protocol version, assume protocol
        // version 1. This is to preserve backwars compatibility with
        // previous versions of clients without the protocol version number.
        clientFeedbackProtocolVersion = Version_1;
        clientName = feedbackProtocolVersion;
    }

    if (socketStream.status() != QDataStream::Ok ||
        clientFeedbackProtocolVersion == Unknown) {
        // Unknown protocol version. Leave the socket open (to prevent
        // endless reconnection attempts) and ignore data from now on.
        state = StateDoomed;
    } else {
        state = StateWaitingSession;
        session = new MfSession(clientName, this);
        connect(session, SIGNAL(setupReady()), this, SLOT(setupReady()));
    }
}

void MfConnection::terminate()
{
    socket->disconnectFromServer();
    state = StateTerminated;
    killMe();
}

