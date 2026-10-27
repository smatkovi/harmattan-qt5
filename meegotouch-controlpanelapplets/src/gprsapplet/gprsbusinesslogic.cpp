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
#include "gprsbusinesslogic.h"

#include <QFile>
#include <QSystemInfo>
#include <QNetworkInterface>
#include <QSystemDeviceInfo>
#include <QSystemNetworkInfo>

QTM_USE_NAMESPACE

#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QtDBus>
#include <QDBusInterface>
#include <QDBusObjectPath>

#include <QDBusVariant>
#include <QVariant>

#include <QtDBus/QtDBus>
#include <QtDBus/qdbusreply.h>
#include <QtDBus/QDBusConnection>



#define OS_NAME_FALLBACK "MeeGo"

#undef DEBUG
#define WARNING
#include "../debug.h"

/*
 * Values used for getting apn data by dbus
 */
#define DBUS_GPRS_SERVICE "org.ofono"
#define DBUS_GPRS_OBJECT_PATH "/context1"
#define DBUS_GPRS_OBJECT_PATH2 "/"
#define DBUS_GPRS_CONTEXT_INTERFACE "org.ofono.ConnectionContext"
#define DBUS_GPRS_MANAGER_INTERFACE "org.ofono.Manager"
#define DBUS_GPRS_NETWORKREGISTRATION_INTERFACE "org.ofono.NetworkRegistration"
#define DBUS_GPRS_GET_PROPERTIES_METHOD "GetProperties"

#define SET_PROPERTY_TIMEOUT 300000

gprsBusinessLogic::gprsBusinessLogic()
{

}

gprsBusinessLogic::~gprsBusinessLogic()
{
    if (m_AdapterDBusIf)
        delete m_AdapterDBusIf;
    if (m_AdapterDBusIf2)
        delete m_AdapterDBusIf2;
    if (m_AdapterDBusIfNetworkRegistration)
        delete m_AdapterDBusIfNetworkRegistration;

    if (m_modem)
        delete m_modem;
/*
    if(m_modem_context)
        delete m_modem_context;
*/
}


/*!
 * This slot is called when an error is occured during the dbus communication.
 * The error message is printed as a warning message.
 */
void
gprsBusinessLogic::DBusMessagingFailure (
        QDBusError error)
{
    SYS_WARNING ("%s: %s", SYS_STR (error.name()), SYS_STR (error.message()));
}

QString
gprsBusinessLogic::initiateGprsQueries ()
{
   QString retval;

   //QDBusInterface  *m_AdapterDBusIf;
   //QDBusInterface  *m_AdapterDBusIf2;

   m_modem = new OfonoModem(OfonoModem::AutomaticSelect, QString(), this);
   m_modemPath=m_modem->path();
   m_contextPath="";
   m_contextPath += m_modemPath;
   m_contextPath += DBUS_GPRS_OBJECT_PATH;

   SYS_DEBUG ("Defaultadapter: %s", SYS_STR (adapter.path()));
   m_AdapterDBusIf = new QDBusInterface (
          DBUS_GPRS_SERVICE,
           m_contextPath,
           DBUS_GPRS_CONTEXT_INTERFACE,
           QDBusConnection::systemBus());
  m_AdapterDBusIfNetworkRegistration = new QDBusInterface (
          DBUS_GPRS_SERVICE,
           m_modemPath,
           DBUS_GPRS_NETWORKREGISTRATION_INTERFACE,
           QDBusConnection::systemBus());

   m_AdapterDBusIf->callWithCallback (
           QString ("GetProperties"),
           QList<QVariant>(), this,
           SLOT (defaultGprsAdapterAddressReceived(QMap<QString, QVariant>)),
           SLOT (DBusMessagingFailure (QDBusError)));

   // get modem info required to network registration
   SYS_DEBUG ("Defaultadapter: %s", SYS_STR (adapter.path()));
   m_AdapterDBusIf2 = new QDBusInterface (
           DBUS_GPRS_SERVICE,
           DBUS_GPRS_OBJECT_PATH2,
           DBUS_GPRS_MANAGER_INTERFACE,
           QDBusConnection::systemBus());
/*
   m_AdapterDBusIf2->call (
           QString ("GetModems()"),
           QDBus::Block,
	   -1);
*/

  //TODO: get context path, this part was not yet in libofono
  /*
   QString connManPath;
   m_modem_context = new Ofonoconnmancontext(QString("/n9000"),this);
   connManPath = m_modem_context->path();
   SYS_DEBUG ("ConnManPath = %s", SYS_STR(connManPath));
*/

 return retval += "ok";
}

void
gprsBusinessLogic::defaultGprsAdapterAddressReceived (
        QMap<QString, QVariant> properties)
{
    SYS_DEBUG("");
    m_GprsAccesPointName = properties["AccessPointName"].toString();
    m_GprsUser = properties["Username"].toString();
    m_GprsPasswd = properties["Password"].toString();
    m_GprsActive = properties["Active"].toString();
    SYS_DEBUG ("Active = %s", SYS_STR(m_GprsActive));

    delete m_AdapterDBusIf;
    m_AdapterDBusIfNetworkRegistration->callWithCallback (
           QString ("GetProperties"),
           QList<QVariant>(), this,
           SLOT (defaultGprsNetworkRegistrationInfoReceived(QMap<QString, QVariant>)),
           SLOT (DBusMessagingFailure (QDBusError)));


    //emit refreshNeeded ();
}

void
gprsBusinessLogic::defaultGprsNetworkRegistrationInfoReceived (
        QMap<QString, QVariant> properties)
{
    SYS_DEBUG("");
    m_OperatorName = properties["Name"].toString();
    SYS_DEBUG ("Operator = %s", SYS_STR(m_OperatorName));
    delete m_AdapterDBusIfNetworkRegistration;

    emit refreshNeeded ();
}

QString
gprsBusinessLogic::GprsApnName()
{
    if (m_GprsAccesPointName.isNull () == true)
        return QString("");

    return m_GprsAccesPointName;
}

QString
gprsBusinessLogic::GprsUser()
{
    if (m_GprsUser.isNull () == true)
        return QString("");

    return m_GprsUser;
}

QString
gprsBusinessLogic::GprsPasswd()
{
    if (m_GprsPasswd.isNull () == true)
        return QString("");

    return m_GprsPasswd;
}

QString
gprsBusinessLogic::OperatorName()
{
    if (m_OperatorName.isNull () == true)
        return QString("");

    return m_OperatorName;
}

QString
gprsBusinessLogic::GprsActive()
{
    if (m_GprsActive.isNull () == true)
        return QString("");

    return m_GprsActive;
}


void gprsBusinessLogic::setProperty(const QString& property, const QVariant& apn)
{
    QDBusMessage request;
    request = QDBusMessage::createMethodCall("org.ofono",
                                            m_contextPath, DBUS_GPRS_CONTEXT_INTERFACE,
                                            "SetProperty");
    QVariantList arguments;
    arguments << property << QVariant::fromValue(QDBusVariant(apn));

    //if (!apn.isNull())
     //arguments << QVariant(apn);
    request.setArguments(arguments);

    bool result = QDBusConnection::systemBus().callWithCallback(request, this,
                                        SLOT(setPropertyResp()),
                                        SLOT(DBusMessagingFailure (QDBusError)),
                                        SET_PROPERTY_TIMEOUT);
/*
    if (!result) {

        // FIXME: should indicate that sending a message failed
        setError(QString(), QString("Sending a message failed"));
        emit setPropertyFailed(name);

        return;
       }
       */
}

void gprsBusinessLogic::setPropertyResp()
{

}
