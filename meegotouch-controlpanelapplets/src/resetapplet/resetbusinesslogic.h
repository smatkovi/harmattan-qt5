/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
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
#ifndef RESETBUSINESSLOGIC_H
#define RESETBUSINESSLOGIC_H

#include <QList>
#include <QObject>

#include <QPointer>
class QDBusInterface;

/*!
 * Provides the 'restore factory settings' and 'clear user data' operations for
 * the reset applet. These operations are implemented in the 'clean-device'
 * package, so one might want to install them:
 *
 * sudo apt-get install clean-device
 *
 * (This package is added to the dependency list of the
 * duicontrolpanel-resetapplet package.) The class will execute the following
 * shell command lines to perform the operations.
 * 
 * /usr/sbin/clean-device.sh --rfs
 * /usr/sbin/clean-device.sh --cud
 */
class ResetBusinessLogic : public QObject
{
    Q_OBJECT

public:
    ResetBusinessLogic ();
    ~ResetBusinessLogic ();

    bool isUsbConnected ();

    /*
     * Asks user for device-lock code when it set
     */
    void getAccess ();

signals:
    void gotAccess ();

public slots:
    void performRestoreSettings ();
    void performClearData ();

private slots:
    void passwordResult (bool result);

private:
    QDBusInterface  *m_devlock;
};

#endif
