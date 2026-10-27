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

#ifndef GPRSBUSINESSLOGIC_H
#define GPRSBUSINESSLOGIC_H

#include <QPointer>
#include <QObject>
#include <QMap>
#include <QDBusError>
#include <QDBusInterface>

#include <QDBusArgument>

#include <QtCore/QList>
#include <QtCore/QMetaType>
#include <QtDBus/QtDBus>


#include <ofonomodem.h>

class QDBusObjectPath;
class QString;
class OfonoModem;

/*!
 * This class allows to get some information gprs system
 */
class gprsBusinessLogic : public QObject
{
    Q_OBJECT

public:
    gprsBusinessLogic ();
    ~gprsBusinessLogic ();

    QString initiateGprsQueries ();
    QString GprsApnName ();
    QString GprsUser ();
    QString GprsPasswd ();
    QString GprsActive ();
    QString OperatorName();

signals:
    void refreshNeeded ();

public slots:
    void defaultGprsNetworkRegistrationInfoReceived (QMap<QString, QVariant> properties);
    void defaultGprsAdapterAddressReceived (QMap<QString, QVariant> properties);
    void DBusMessagingFailure (QDBusError error);
    void setProperty(const QString& property, const QVariant& apn);
    void setPropertyResp();

private:
    void initiateBluetoothQueries ();


private:
    QPointer<QDBusInterface> m_AdapterDBusIf;
    QPointer<QDBusInterface> m_AdapterDBusIf2;
    QPointer<QDBusInterface> m_AdapterDBusIfNetworkRegistration;

    QString       m_GprsAccesPointName;
    QString       m_GprsUser;
    QString       m_GprsPasswd;
    QString       m_GprsActive;
    QString       m_OperatorName;
    QString       m_ModemPath;
    OfonoModem    *m_modem;
    QString       m_modemPath;
    QString       m_contextPath;

    #ifdef UNIT_TEST
    friend class Ut_gprsBusinessLogic;
    friend class Ut_gprsApplet;
    #endif
};

#endif
