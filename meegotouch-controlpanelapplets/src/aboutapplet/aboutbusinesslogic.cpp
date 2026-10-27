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
#include "aboutbusinesslogic.h"
#include <QFile>
#include <QTimer>
#include <QSystemInfo>
#include <QNetworkInterface>
#include <QSystemDeviceInfo>
#include <QSystemNetworkInfo>

QTM_USE_NAMESPACE

#include <QDir>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QtDBus>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusObjectPath>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define OS_NAME_FALLBACK "MeeGo"

#define DEBUG
#define WARNING
#include "../debug.h"

/*
 * Values used for getting bluetooth address by dbus
 */
#define DBUS_BLUEZ_SERVICE "org.bluez"
#define DBUS_BLUEZ_OBJECT_PATH "/"
#define DBUS_BLUEZ_MANAGER_INTERFACE "org.bluez.Manager"
#define DBUS_BLUEZ_ADAPTER_INTERFACE "org.bluez.Adapter"
#define DBUS_BLUEZ_GET_DEFAULT_ADAPTER_METHOD "DefaultAdapter"
#define DBUS_BLUEZ_GET_PROPERTIES_METHOD "GetProperties"

#ifndef LICENSE_PATH
/* Define an empty string if fallback license file is not defined */
#define LICENSE_PATH ""
#endif

#ifdef MEEGO
#define LICENSE_PATH "/etc/meego-license"
#define NAMES_PATH "/etc/about_names"
#endif

static const QString barcodeGenerator ("/usr/bin/about-contents-barcode");
static const QString configPath ("/usr/share/about-contents/");
static const QString configFile (configPath + "contents.ini");
static const QString certConfigFile (configPath + "typelabel.ini");

AboutBusinessLogic::AboutBusinessLogic() : QThread (0),
    m_generator (0)
{
    setTerminationEnabled (true);
    qRegisterMetaType<AboutBusinessLogic::requestType>(
        "AboutBusinessLogic::requestType");

    connect (this, SIGNAL (started ()),
             SLOT (initializeAndStart ()));
}

AboutBusinessLogic::~AboutBusinessLogic()
{
    delete m_generator;
    m_generator = 0;

    if (m_ManagerDBusIf)
        delete m_ManagerDBusIf;
    if (m_AdapterDBusIf)
        delete m_AdapterDBusIf;
}


bool
AboutBusinessLogic::ipAddress (
            const QString   &device,
            QString         &addrString)
{
    int                 sfd;
    struct ifreq        ifr;
    struct sockaddr_in *sin = (struct sockaddr_in *) &ifr.ifr_addr;
    bool                retval = false;

    memset(&ifr, 0, sizeof ifr);

    if (0 > (sfd = socket(AF_INET, SOCK_STREAM, 0))) {
        SYS_WARNING ("Socket creation error");
        goto finalize;
    }

    strcpy (ifr.ifr_name, device.toAscii().constData());
    sin->sin_family = AF_INET;

    if (0 == ioctl(sfd, SIOCGIFADDR, &ifr)) {
        SYS_DEBUG ("%s: %s", ifr.ifr_name, inet_ntoa(sin->sin_addr));
        addrString = inet_ntoa(sin->sin_addr);
        retval = true;
    }

finalize:
     return retval;
}

void
AboutBusinessLogic::run ()
{
    m_queue.prepend (reqStartAll);
    setPriority (QThread::LowPriority);
}

QString
AboutBusinessLogic::osVersion (QString lsbFilePath )
{
    QString retval;

    QSystemInfo systemInfo;
#ifdef MEEGO
   
    QFile rel_file ("/etc/meego-release");  
      if (rel_file.open (QIODevice::ReadOnly))  
      {  
          QString contents (rel_file.readAll ().constData ());  
           rel_file.close ();  
    
         QRegExp distrib_name ("BUILD: (\\S*)");  
          int pos = distrib_name.indexIn (contents);  
          if (pos > -1)  
              retval = distrib_name.cap (1);  

              return retval;
      } 

#endif

    retval = systemInfo.version (QSystemInfo::Firmware);
    /*
     * The fw-version format is something like that:
     * SupportedHWID_PROGRAM_ReleaseVersion_suffixes
     *
     * Try to get only the version number:
     */
    int index = retval.indexOf ('_', retval.indexOf ('_') + 1) + 1;
    if (index > 0)
        retval = retval.mid (index);

    if (retval.isEmpty()) {
        /*
         * This is a fallback method... (works fine on Ubuntu)
         * Try to get the version number from the lsb-release
         */
        QFile lsbrel_file (lsbFilePath);
        if (lsbrel_file.open (QIODevice::ReadOnly)) {
            QString contents (lsbrel_file.readAll ().constData ());
            lsbrel_file.close ();

            QRegExp version ("DISTRIB_RELEASE=(\\S*)");
            int pos = version.indexIn (contents);
            if (pos > -1) {
                retval = version.cap (1);
                goto finalize;
            }
        }
    }

finalize:
    if (retval.isEmpty()) {
        retval = "-";
    } else {
        retval = "<b>PR1.2</b><br>(" + retval + ")";
    }

    return retval;
}

QString
AboutBusinessLogic::osName (QString lsbFilePath)
{
    /*
     * Customization overrides any value...
     */
    if (! m_swName.isEmpty ())
        return m_swName;

    //% "MeeGo"
    QString retval = qtTrId ("qtn_prod_sw_version");


#ifdef MEEGO

    QFile meegorel_file ("/etc/meego-release");  
     if (meegorel_file.open (QIODevice::ReadOnly))  
     {  
         QString contents (meegorel_file.readAll ().constData ());  
         meegorel_file.close ();  
   
         retval = contents.section('\n',0,0);  
     }
#endif

    /*
     * Try to get the version number from the lsb-release
     */
    QFile lsbrel_file (lsbFilePath);
    if (lsbrel_file.open (QIODevice::ReadOnly))
    {
        QString contents (lsbrel_file.readAll ().constData ());
        lsbrel_file.close ();

        QRegExp distrib_name ("DISTRIB_ID=(\\S*)");
        int pos = distrib_name.indexIn (contents);
        if (pos > -1)
            retval = distrib_name.cap (1);
    }

    return retval;

}

/*!
 * Returns the MAC address for the first interface it founds. This method
 * contains a static list of interface names that the method will try to find.
 */
QString
AboutBusinessLogic::WiFiAddress ()
{
    QString address;
    QSystemNetworkInfo netInfo;

    address = netInfo.interfaceForMode (
            QSystemNetworkInfo::WlanMode).hardwareAddress ();

    if (address.isNull ())
        address = netInfo.interfaceForMode (
            QSystemNetworkInfo::WimaxMode).hardwareAddress ();

    return address;
}

/*!
 * Returns the IMEI address as a string.
 */
QString
AboutBusinessLogic::IMEI ()
{
    QString imei;

    // Get IMEI from QtMobility SystemDeviceInfo obj.
    QSystemDeviceInfo deviceInfo;

    imei = deviceInfo.imei ();

    return imei;
}

QString
AboutBusinessLogic::productName ()
{
    /*
     * Customization maybe overrides the qt-mobility value...
     */
    if (! m_prodName.isEmpty ())
        return m_prodName;

    QSystemDeviceInfo sdi;

    /*
     * From now model should be used
     * (as productName will returns the internal prod.name,
     * eg.: RX-51 for Nokia N900)
     */
    QString vendor = sdi.manufacturer ();
    QString model = sdi.model ();

    if (model.contains (vendor))
        return model;

    return vendor + " " + model;
}

/*!
 * Initiates a query to receive the default bluetooth adapter name. Later on an
 * other query is going to be initiated to get the address of the default
 * adapter.
 */
void
AboutBusinessLogic::initiateBluetoothQueries ()
{
    if (m_ManagerDBusIf)
        return;

    m_ManagerDBusIf = new QDBusInterface (
            DBUS_BLUEZ_SERVICE,
            DBUS_BLUEZ_OBJECT_PATH,
            DBUS_BLUEZ_MANAGER_INTERFACE,
            QDBusConnection::systemBus ());

    m_ManagerDBusIf->callWithCallback (
            QString (DBUS_BLUEZ_GET_DEFAULT_ADAPTER_METHOD),
            QList<QVariant> (), this,
            SLOT (defaultBluetoothAdapterReceived(QDBusObjectPath)),
            SLOT (DBusMessagingFailure (QDBusError)));
}

/*!
 * This slot is called when the default adapter name of the bluetooth has been
 * received through the dbus. This method will initiate a new dbus query to get
 * the address of this adapter.
 */
void
AboutBusinessLogic::defaultBluetoothAdapterReceived (
        QDBusObjectPath adapter)
{
    QDBusInterface  *m_AdapterDBusIf;

    SYS_DEBUG ("Defaultadapter: %s", SYS_STR (adapter.path()));
    m_AdapterDBusIf = new QDBusInterface (
            DBUS_BLUEZ_SERVICE,
            adapter.path(),
            DBUS_BLUEZ_ADAPTER_INTERFACE,
            QDBusConnection::systemBus());

    m_AdapterDBusIf->callWithCallback (
            QString (DBUS_BLUEZ_GET_PROPERTIES_METHOD),
            QList<QVariant>(), this,
            SLOT (defaultBluetoothAdapterAddressReceived(QMap<QString, QVariant>)),
            SLOT (DBusMessagingFailure (QDBusError)));

    delete m_ManagerDBusIf;
}

/*!
 * This slot is called when the address of the default bluetooth adapter has
 * been received.
 */
void
AboutBusinessLogic::defaultBluetoothAdapterAddressReceived (
        QMap<QString, QVariant> properties)
{
    QString btAddress;

    btAddress = properties["Address"].toString ();

    SYS_DEBUG ("address = %s", SYS_STR (btAddress));
    delete m_AdapterDBusIf;

    emit requestFinished (reqBtAddr, btAddress);
}

/*!
 * This slot is called when an error is occured during the dbus communication.
 * The error message is printed as a warning message.
 */
void
AboutBusinessLogic::DBusMessagingFailure (
        QDBusError error)
{
    Q_UNUSED (error);
    SYS_WARNING ("%s: %s", SYS_STR (error.name()),
                 SYS_STR (error.message()));
}

QString
AboutBusinessLogic::licenseText ()
{
    static const char *legalTextId = "qtn_prod_legal";
    QString localizedText (qtTrId (legalTextId));

    if (m_licenseFile.isEmpty ())
        return localizedText;

    if (m_licenseFile.at (0) != '/')
        m_licenseFile = configPath + m_licenseFile;

    QFile licenseFile (m_licenseFile);
    if (!licenseFile.open (QIODevice::ReadOnly | QIODevice::Text))
    {
        SYS_WARNING ("Unable to open %s", SYS_STR (m_licenseFile));
        return localizedText;
    }

    QTextStream inStream (&licenseFile);

    return inStream.readAll ();
}

void
AboutBusinessLogic::initializeAndStart ()
{
    /*
     * Load the customization file...
     */
    QSettings content (configFile, QSettings::IniFormat);

    m_swName = content.value ("swname").toString ();
    m_licenseFile =
        content.value ("abouttext", QString (LICENSE_PATH)).toString ();

    if (QFile::exists (certConfigFile))
    {
        /*
         * Type certifications may come from different customization
         */
        QSettings certsContent (certConfigFile, QSettings::IniFormat);
        QString images = certsContent.value ("swtypeimage").toString ();
        if (! images.isEmpty ())
          m_certsImages = images.split (",");
        else
          m_certsImages = certsContent.value ("swtypeimage").toStringList ();

        SYS_WARNING ("count = %d", m_certsImages.count ());
    }

    bool certsExists = false;
    for (int i = 0; i < m_certsImages.count (); i++)
    {
        QString toCheck = m_certsImages[i];

        if ((! toCheck.isEmpty ()) &&
            (toCheck.at (0) != '/'))
            m_certsImages[i] = configPath + toCheck;

        if (QFile::exists (m_certsImages[i]))
            certsExists = true;
    }

    if (certsExists)
    {
        /*
         * To avoid flickering we need to construct the
         * certificates image container as soon as possible
         */
        emit requestFinished (reqCertsImageNeeded, QVariant ());
    }

    /*
     * Lets run the barcode-generator if there is one...
     */
    if (QFile::exists (barcodeGenerator))
    {
        m_generator = new QProcess;
        connect (m_generator, SIGNAL (finished (int, QProcess::ExitStatus)),
                 this, SLOT (barcodeReady ()));
        SYS_DEBUG ("Requesting barcode image");
        m_generator->start (barcodeGenerator);
    }

    processNextRequest ();
}

QImage
AboutBusinessLogic::barcodeImage ()
{
    QString barcode =
        QDir::homePath () + "/.barcode_image.png";

    /* Load the picture ... */
    QImage img (barcode);
    SYS_DEBUG ("Image path = %s",
               SYS_STR (barcode));
    return img;
}

void
AboutBusinessLogic::barcodeReady ()
{
    SYS_DEBUG ("");
    if (QFile::exists (QDir::homePath () + "/.barcode_image.png"))
    {
        emit requestFinished (reqBarcodeImage, barcodeImage ());
    }
}

void
AboutBusinessLogic::processNextRequest ()
{
    if (m_queue.isEmpty ())
    {
        return;
    }

    requestType current = m_queue.takeFirst ();
    SYS_DEBUG ("*** current req = %d", (int) current);

    switch (current)
    {
        case reqProdName:
            emit requestFinished (current, productName ());
            break;
        case reqSwName:
            emit requestFinished (current, osName ());
            break;
        case reqLicense:
            emit requestFinished (current, licenseText ());
            break;
        case reqCertsImage:
            emit requestFinished (current, m_certsImages);
            break;
        case reqOsVersion:
            emit requestFinished (current, osVersion ());
            break;
        case reqImei:
            emit requestFinished (current, IMEI ());
            break;
        case reqWifiAddr:
            emit requestFinished (current, WiFiAddress ());
            break;
        case reqBtAddr:
            /* defaultBluetoothAdapterAddressReceived
             * will emit the requestFinished ... */
            initiateBluetoothQueries ();
            break;
        case reqLast:
            /* no-op */
            break;
        default:
        case reqStartAll:
        {
            m_queue.clear ();
            /* Fill the queue with all requests */
            for (int i = (int) reqProdName; i < (int) reqLast; i++)
                m_queue.append ((requestType) i);
        }
        break;
    }

    QTimer::singleShot (0, this, SLOT (processNextRequest ()));
}


#ifdef MEEGO
QString
AboutBusinessLogic::GetAboutNames ()
{
       QString retval;

       QFile names_file(NAMES_PATH);
       if (names_file.open (QIODevice::ReadOnly))
       {
       retval = (names_file.readAll().constData());
       names_file.close();
       }

       else
       {
       retval = '-';  
       }

      return retval;

}
#endif
