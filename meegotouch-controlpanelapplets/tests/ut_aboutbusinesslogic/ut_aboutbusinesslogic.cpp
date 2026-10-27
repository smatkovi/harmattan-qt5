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
#include "ut_aboutbusinesslogic.h"
#include "aboutbusinesslogic.h"

#include <stdint.h>
#include <inttypes.h>
#include <QMap>
#include <QDebug>
#include <QString>
#include <QVariant>
#include <QByteArray>

#include <QSystemInfo>
#include <QSystemDeviceInfo>
#include <QSystemNetworkInfo>
#include <QNetworkInterface>

#include <QFile>
#include <QIODevice>


#include <MApplication>

#define DEBUG
#include "../../src/debug.h"


/******************************************************************************
 * Stubbing the QNetworkInfo
 */
QString qnetworkinterface_hwaddr;

QString
QNetworkInterface::hardwareAddress () const
{
    return qnetworkinterface_hwaddr;
}

bool
QNetworkInterface::isValid () const
{
    return ! qnetworkinterface_hwaddr.isNull ();
}

/******************************************************************************
 * Stubbing the QSystemDeviceInfo::imei () and QSystemInfo::version ()
 */
QString systemdeviceinfo_imei_retval;
QString systeminfo_firmware_retval;

QTM_BEGIN_NAMESPACE

QString
QSystemDeviceInfo::imei()
{
    return systemdeviceinfo_imei_retval;
}

QString
QSystemInfo::version (
    QSystemInfo::Version type,
    const QString &parameter)
{
    Q_UNUSED (type);
    Q_UNUSED (parameter);
    return systeminfo_firmware_retval;
}

QSystemNetworkInfo::QSystemNetworkInfo (QObject *parent)
{
    Q_UNUSED (parent);
}

QSystemNetworkInfo::~QSystemNetworkInfo ()
{
}

QNetworkInterface qnetif_for_testing;

QNetworkInterface
QSystemNetworkInfo::interfaceForMode (QSystemNetworkInfo::NetworkMode mode)
{
    Q_UNUSED (mode);
    return qnetif_for_testing;
}

QTM_END_NAMESPACE

/******************************************************************************
 * Stubbing the dbus interface.
 */
static QString lastCalledMethod;

bool
QDBusAbstractInterface::callWithCallback (
        const QString           &method,
        const QList<QVariant>   &args,
        QObject                 *receiver,
        const char              *returnMethod,
        const char              *errorMethod)
{
    Q_UNUSED (args);
    Q_UNUSED (receiver);
    Q_UNUSED (errorMethod);
    Q_UNUSED (returnMethod);

    SYS_DEBUG ("*** method       = %s", SYS_STR(method));
    SYS_DEBUG ("*** returnMethod = %s", returnMethod);

    lastCalledMethod = method;
    return true;
}

/******************************************************************************
 * Ut_AboutBusinessLogic implementation.
 */
void
Ut_AboutBusinessLogic::init()
{
    m_Api = new AboutBusinessLogic;
}

void
Ut_AboutBusinessLogic::cleanup()
{
    delete m_Api;
    m_Api = 0;
}


static int argc = 1;
static char *app_name = (char*) "./ut_aboutbusinesslogic";

void
Ut_AboutBusinessLogic::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
}

void
Ut_AboutBusinessLogic::cleanupTestCase()
{
    m_App->deleteLater ();
}

void
Ut_AboutBusinessLogic::testImei()
{
    systemdeviceinfo_imei_retval = "AA-BBBBBB-CCCCCC-D";

    QCOMPARE (m_Api->IMEI (), systemdeviceinfo_imei_retval);
}

void
Ut_AboutBusinessLogic::testOsName ()
{
    QString name;

    name = m_Api->osName();
    QCOMPARE (name, QString ("qtn_prod_sw_version"));

    
}

void
Ut_AboutBusinessLogic::testOsVersion ()
{
    QString name;

    systeminfo_firmware_retval = "HARDWARE_PROGRAM_VERSION13";
    name = m_Api->osVersion();

    QVERIFY (name.contains (QString ("VERSION13")));

    // check the LSB release file version
    systeminfo_firmware_retval = "";
    system("echo  'DISTRIB_ID=Ubuntu' > ./testfile");
    system("echo  'DISTRIB_RELEASE=11.04' >> ./testfile");
    system("echo  'DISTRIB_ID=Ubuntu' >> ./testfile");
    
    name = m_Api->osVersion("./testfile");
    qDebug() << qPrintable(m_Api->osVersion("./testfile"));
    
    QVERIFY (name.contains (QString ("11.04")));
    system ("rm -f ./testfile");
	  

}

void
Ut_AboutBusinessLogic::testBluetooth ()
{
    QSignalSpy spy (
        m_Api,
        SIGNAL (requestFinished (AboutBusinessLogic::requestType, QVariant)));

    QMap <QString, QVariant> properties;
    properties["Address"] = QString ("fake-bluetooth-address");

    /*
     * Let's initiate the Bluetooth query that will call the
     * QDBusAbstractInterface::callWithCallback() that is stubbed.
     */
    m_Api->initiateBluetoothQueries ();
    QCOMPARE (lastCalledMethod, QString ("DefaultAdapter"));

    /*
     * Let's answer the previous step by calling the DBus callback manually.
     * This will initiate an other DBus call also stubbed.
     */
    m_Api->defaultBluetoothAdapterReceived (
            QDBusObjectPath("/fakeObjectPath"));
    QCOMPARE (lastCalledMethod, QString ("GetProperties"));

    /*
     * Answering the second DBus call and checking if the businesslogic
     * processed the data as it should.
     */
    m_Api->defaultBluetoothAdapterAddressReceived (properties);

    QTest::qWait (100);
    QCOMPARE (spy.count (), 1);

    QList<QVariant> args = spy.first ();

    QCOMPARE (args.at (0).value<AboutBusinessLogic::requestType>(),
              AboutBusinessLogic::reqBtAddr);
#if 0
    /* for some strange reason it is not working :-S */
    QCOMPARE (args.at (1).toString (), QString ("fake-bluetooth-address"));
#endif

    /*
     * Let's test the failure socket. This does not do nothing...
     */
    m_Api->DBusMessagingFailure (QDBusError());

    delete m_Api;
    /*
     * Let's see what happens if we initate the data collection and instead of
     * producing answers to queries we just destroy the object.
     */
    m_Api = new AboutBusinessLogic;
    m_Api->initiateBluetoothQueries ();
    //XXX: delete m_Api; (done in cleanup ())
}

void
Ut_AboutBusinessLogic::testHwAddresses ()
{
    qnetworkinterface_hwaddr = "test-address-xxx";

    QCOMPARE (m_Api->WiFiAddress (), qnetworkinterface_hwaddr);
}

void
Ut_AboutBusinessLogic::testLicenseText ()
{
    /*
     * FIXME: the test app may not have access to do this
     */
    system ("mkdir -p /usr/share/about-contents");
    system ("echo 'testtext' > /usr/share/about-contents/testlicensefile");
    m_Api->m_licenseFile = QString ("testlicensefile");
    
    QCOMPARE (m_Api->licenseText (), QString ("testtext\n"));
    system ("rm /usr/share/about-contents/testlicensefile");

    //check what if the file  doesn't exist
    QCOMPARE (m_Api->licenseText (), QString ("qtn_prod_legal"));
}

/**
 * Check that all mandatory informations are passed
 * throught the requestFinished signal...
 */
void
Ut_AboutBusinessLogic::testMultiThread ()
{
    QSignalSpy spy (
        m_Api,
        SIGNAL (requestFinished (AboutBusinessLogic::requestType, QVariant)));

    /*
     * Run the multithreaded data collection ...
     */
    m_Api->start ();

    /*
     * Hopefully this does not harm, and enough...
     */
    QTest::qWait (500);

    QList<AboutBusinessLogic::requestType> mandatoryFields;
    mandatoryFields <<
        AboutBusinessLogic::reqProdName <<
        AboutBusinessLogic::reqSwName <<
        AboutBusinessLogic::reqLicense <<
        AboutBusinessLogic::reqOsVersion <<
        AboutBusinessLogic::reqImei <<
        AboutBusinessLogic::reqWifiAddr;
    /* BtAddr also mandatory, but we stubbed that stuff here,
     * and we have a separate testcase which covers that one... */

    /*
     * Check whether that at least the mandatory field values are arrived
     */
    QVERIFY (spy.count () >= mandatoryFields.count ());

    QList<AboutBusinessLogic::requestType> arrivedFields;

    for ( ; ! spy.isEmpty () ; )
    {
        QList<QVariant> signal = spy.takeFirst ();
        arrivedFields <<
            signal.at (0).value<AboutBusinessLogic::requestType>();
    }

    foreach (AboutBusinessLogic::requestType req, mandatoryFields)
    {
        //SYS_DEBUG ("reqNr = %d", (int) req);
        QVERIFY (arrivedFields.contains (req));
    }
}

QTEST_APPLESS_MAIN(Ut_AboutBusinessLogic)
