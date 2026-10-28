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

#include "mfsignalhandler.h"

#include <QSocketNotifier>
#include <QString>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

#include <QDebug>

QMap<int, int> MfSignalHandler::signal2fd;

namespace {
    inline void reportError(QString prefix)
    {
        char buf[512] = "";
        const char *errorMsg = strerror_r(errno, buf, sizeof(buf) - 1);
        qCritical() << prefix << errorMsg << " (errno=" << errno << ")";
    }

    typedef QVector<int>::const_iterator Iter;
}

MfSignalHandler::MfSignalHandler() : notify(0) {
    enum { DEFAULT_CAPACITY = 5 };
    watchOver.reserve(DEFAULT_CAPACITY);
    fd[0] = 0;
    fd[1] = 0;
}

MfSignalHandler::~MfSignalHandler() {
    struct sigaction sa;
    sigset_t mask, old_mask;

    delete notify;
    notify = 0;
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, &old_mask);
    for(Iter it(watchOver.begin()); it != watchOver.end(); ++it) {
        sigaction(*it, &sa, 0);
        signal2fd.remove(*it);
    }
    pthread_sigmask(SIG_SETMASK, &old_mask, 0);

    close(fd[0]);
    close(fd[1]);
}

bool MfSignalHandler::init(int signal) {
    struct sigaction sa;
    sigset_t mask, old_mask;

    if (signal) {
        catchSignal(signal);
    }

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fd)) {
        reportError("Unable to open socket: ");
        return false;
    }

    sa.sa_handler = onSignal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, &old_mask);
    for(Iter it(watchOver.begin()); it != watchOver.end(); ++it) {
        qDebug() << __FILE__ << ":" << __LINE__ <<
            "Install handler for signal" << *it;
        if (sigaction(*it, &sa, 0)) {
            pthread_sigmask(SIG_SETMASK, &old_mask, 0);
            reportError(QString("Unable to install signal %1 handler: ").arg(*it));
            return false;
        }
        signal2fd.insert(*it, fd[0]);
    }
    pthread_sigmask(SIG_SETMASK, &old_mask, 0);

    notify = new QSocketNotifier(fd[1], QSocketNotifier::Read);
    connect(notify, SIGNAL(activated(int)), this, SLOT(retranslate()));
    return true;
}

void MfSignalHandler::catchSignal(int signal) {
    watchOver.push_back(signal);
}

void MfSignalHandler::retranslate() {
    int signal;
    notify->setEnabled(false);
    ssize_t bytesRead;

    bytesRead = ::read(fd[1], &signal, sizeof(signal));
    if (bytesRead != sizeof(signal)) {
        qCritical() << __PRETTY_FUNCTION__ << "Bad read";
    }

    qDebug() << __PRETTY_FUNCTION__ << "Translate signal"
             << signal << "to QT signal";

    emit signalReceived(signal);
    notify->setEnabled(true);
}

void MfSignalHandler::onSignal(int signal) {
    QMap<int, int>::const_iterator it(signal2fd.find(signal));
    ssize_t bytesWritten;

    if (signal2fd.end() != it) {
        bytesWritten = ::write(it.value(), &signal, sizeof(signal));
        if (bytesWritten != sizeof(signal)) {
            qCritical() << __PRETTY_FUNCTION__ << "Bad write";
        }
    }
}

int MfSignalHandler::count() {
    return signal2fd.size();
}

void MfSignalHandler::blockAll() {
    sigset_t mask;
    sigfillset(&mask);
    pthread_sigmask(SIG_BLOCK, &mask, 0);
}

