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

#include "ut_aboutapplet.h"
#include "aboutapplet.h"
#include "aboutwidget.h"

#include <QVariant>
#include <QString>
#include <QStringList>
#include <QImage>

#include <dcpstylablewidget.h>
#include <dcpwidgettypes.h>

#include <QSystemInfo>
#include <QSystemDeviceInfo>

QTM_USE_NAMESPACE

#include <mdesktopentry.h>
#include <MApplication>
#include <MAction>
#include <MLabel>
#include <QVector>

#define DEBUG
#include "../../src/debug.h"

#ifdef HAVE_CONTENT_ACTION
#include <contentaction.h>

static bool contentActionTriggered = false;

namespace ContentAction
{

void
Action::trigger () const
{
    contentActionTriggered = true;
}

}
#endif

/******************************************************************************
 * Ut_AboutApplet implementation.
 */
void
Ut_AboutApplet::init()
{
}

void
Ut_AboutApplet::cleanup()
{
}


static int argc = 1;
static char *app_name = (char*) "./ut_aboutapplet";

void
Ut_AboutApplet::initTestCase()
{
    m_App = new MApplication (argc, &app_name);

    /*
     * We create an AboutApplet here. The applet will create a businesslogic,
     * for which we check that the data collection is not started.
     */
    m_Applet = new AboutApplet;

    QVERIFY (!m_Applet->m_MainWidget);
    m_Applet->init ();

    /*
     * Testing if the widget is not created yet.
     */
    QVERIFY (!m_Applet->m_MainWidget);

#if 0
    /*
     * TODO: FIXME: remove this, but output can be helpful
     * for development & testing purposes
     */
    QSystemDeviceInfo deviceInfo;
    SYS_DEBUG ("\n****** QSystemDeviceInfo *****\n"
               "***** imei: %s\n***** manufacturer: %s\n"
               "***** model: %s\n***** productName: %s\n",
               SYS_STR(deviceInfo.imei ()), SYS_STR(deviceInfo.manufacturer ()),
               SYS_STR(deviceInfo.model ()), SYS_STR(deviceInfo.productName ()));

    QSystemInfo systemInfo;
    SYS_DEBUG ("\n****** QSystemInfo versions *****\n"
               "***** Os: %s\n***** QtCore: %s\n***** Firmware: %s\n",
               SYS_STR(systemInfo.version (QSystemInfo::Os)),
               SYS_STR(systemInfo.version (QSystemInfo::QtCore)),
               SYS_STR(systemInfo.version (QSystemInfo::Firmware)));
#endif
}

void
Ut_AboutApplet::cleanupTestCase()
{
    delete m_Applet;
    m_App->deleteLater ();
}

void
Ut_AboutApplet::testTitle ()
{
    QString title = m_Applet->title();
    QVERIFY (!title.isEmpty());
}

void
Ut_AboutApplet::testConstructWidget ()
{
    AboutWidget     *widget;
    bool             backAccepted;

    /*
     * Testing if the applet creates a widget the first time.
     */
    widget = (AboutWidget *) m_Applet->constructStylableWidget (0);
    QVERIFY (widget);
    QVERIFY (m_Applet->m_MainWidget == widget);
    QVERIFY (!m_Applet->constructStylableWidget (1));

    /*
     * Testing if the widget accepts the back. Our applets always accept back.
     */
    backAccepted = widget->back();
    QVERIFY (backAccepted);

    /*
     * Testing if the applet knows about the destruction of the widget.
     */
    delete widget;
    QVERIFY (!m_Applet->m_MainWidget);
}

void
Ut_AboutApplet::testMenuItems ()
{
    QVector<MAction*> items = m_Applet->viewMenuItems ();

    QVERIFY (items.size() == 0);
}

void
Ut_AboutApplet::testLinkActivated ()
{
    AboutWidget     *widget;

    widget = (AboutWidget *) m_Applet->constructStylableWidget (0);
    QVERIFY (widget);

#ifdef HAVE_CONTENT_ACTION
    contentActionTriggered = false;
    widget->linkActivated (QString ("http://www.nokia.com"));

    QCOMPARE (contentActionTriggered, true);
#endif
}

void
Ut_AboutApplet::testWidgets ()
{
    AboutWidget     *widget;

    widget = qobject_cast<AboutWidget*>(m_Applet->constructStylableWidget (0));
    QVERIFY (widget);

    widget->gotInfo (AboutBusinessLogic::reqCertsImageNeeded, QVariant ());
    QVERIFY (widget->m_imgCerts);
    widget->gotInfo (AboutBusinessLogic::reqProdName, QVariant ("product-name"));
    QVERIFY (widget->m_ProductName);
    widget->gotInfo (AboutBusinessLogic::reqSwName, QVariant ("sw-name"));
    QVERIFY (widget->m_ProductName);
    widget->gotInfo (AboutBusinessLogic::reqLicense, QVariant ("long-long-license"));
    QCOMPARE (widget->m_LicenseLabel->text (), QString ("long-long-license"));
    QStringList testCertImageList;
    testCertImageList << "a.jpg" << "b.jpg";
    // and add some valid one (it is not a problem if it fails, but less coverage)
    testCertImageList << "/usr/share/themes/blanco/meegotouch/icons/icon-l-settings.png";
    widget->gotInfo (AboutBusinessLogic::reqCertsImage, testCertImageList);

    QImage testImage;
    widget->gotInfo (AboutBusinessLogic::reqBarcodeImage, testImage);
    QVERIFY (widget->m_imgBarcode);

    // to cover all:
    widget->gotInfo (AboutBusinessLogic::reqOsVersion, QVariant ("os-version"));
    widget->gotInfo (AboutBusinessLogic::reqImei, QVariant ("imei"));
    widget->gotInfo (AboutBusinessLogic::reqWifiAddr, QVariant ("wifi-mac"));
    widget->gotInfo (AboutBusinessLogic::reqBtAddr, QVariant ("bt-mac"));

    delete widget;
}

QTEST_APPLESS_MAIN(Ut_AboutApplet)
