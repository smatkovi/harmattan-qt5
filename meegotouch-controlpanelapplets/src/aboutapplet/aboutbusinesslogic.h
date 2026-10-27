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
#ifndef ABOUTBUSINESSLOGIC_H
#define ABOUTBUSINESSLOGIC_H

#include <QMap>
#include <QList>
#include <QString>
#include <QStringList>
#include <QPointer>
#include <QImage>
#include <QThread>
#include <QVariant>
#include <QDBusError>
#include <QDBusInterface>

class QDBusObjectPath;
class QProcess;

/*!
 * This class allows to get some information about system
 */
class AboutBusinessLogic : public QThread
{
    Q_OBJECT
    Q_ENUMS (requestType);

public:
    AboutBusinessLogic ();
    ~AboutBusinessLogic ();

    typedef enum {
        reqProdName = 0,
        reqSwName,
        reqLicense,
        reqCertsImage,
        reqOsVersion,
        reqImei,
        reqWifiAddr,
        reqBtAddr,
        reqLast,
        /* special requests */
        reqBarcodeImage,
        reqCertsImageNeeded,
        reqStartAll = 100
    } requestType;


#ifdef MEEGO
    QString GetAboutNames ();
#endif

    bool ipAddress (
            const QString   &device,
            QString         &addrString);
    
protected:
    virtual void run ();

signals:
    void requestFinished (AboutBusinessLogic::requestType type,
                          QVariant value);

private slots:
    void initializeAndStart ();
    void processNextRequest ();

    void defaultBluetoothAdapterAddressReceived (
        QMap<QString, QVariant> properties);
    void defaultBluetoothAdapterReceived (QDBusObjectPath adapter);
    void DBusMessagingFailure (QDBusError error);
    void barcodeReady ();

private:
    void initiateBluetoothQueries ();
    QString osVersion (QString lsbFilePath = "/etc/lsb-release");
    QString osName (QString lsbFilePath = "/etc/lsb-release");
    QString WiFiAddress ();
    QString IMEI ();
    QString productName ();
    QString licenseText ();

    QImage barcodeImage ();

    QPointer<QDBusInterface> m_ManagerDBusIf;
    QPointer<QDBusInterface> m_AdapterDBusIf;

    QList<requestType>  m_queue;

    QString m_licenseFile;
    QString m_swName;
    QString m_prodName;
    QStringList m_certsImages;

    QProcess *m_generator;

#ifdef UNIT_TEST
    friend class Ut_AboutBusinessLogic;
    friend class Ut_AboutApplet;
#endif
};

Q_DECLARE_METATYPE(AboutBusinessLogic::requestType);

#endif
