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

#include "mfserver.h"
#include "mfconnection.h"
#include "mfutil.h"

#include <sys/stat.h>

#include <QFile>
#include <QLocalSocket>
#include <QObject>
#include <QtDebug>


MfServer::MfServer(QObject *parent) : QObject(parent)
{
}

bool MfServer::init()
{
    bool ok;
    QString tempDir = mfTempDir();
    QString socketFilePath;

    if (tempDir.size() == 0) {
        return false;
    }

    socketFilePath = tempDir;
    socketFilePath.append("/player.sock");

    // Use queued connection to avoid multiple initialization at once
    ok = connect(&socketServer, SIGNAL(newConnection()), SLOT(onNewConnection()),
                 Qt::QueuedConnection);
    if (!ok) return false;

    ok = socketServer.listen(socketFilePath);
    if (!ok) {
        // Remove temp file and try again.
        if (QFile::remove(socketFilePath)) {
            ok = socketServer.listen(socketFilePath);
        }
    }
    if (!ok) {
        qCritical() << "MfServer: Unable to listen for connections on"
                    << socketFilePath;
        return false;
    }

    // Enable other users to connect to this socket server
    if (chmod(socketFilePath.toAscii().constData(),
              S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH) == -1) {
        qCritical() << "MfServer: Unable to set socket file permissions";
        return false;
    }

    return true;
}

void MfServer::onNewConnection()
{
    QLocalSocket* socket = socketServer.nextPendingConnection();
    qDebug("MfServer: Connection arrived.");

    new MfConnection(socket, this);
}
