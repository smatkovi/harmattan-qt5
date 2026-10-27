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

#include "ut_usbapplet.h"
#include "usbapplet.h"
#include "usbview.h"

#ifdef HAVE_QMSYSTEM
#  include "qmusbmode.h"
#  ifndef QMUSBMODE_STUB_H
#    error "The QmUSBMode is not stubbed, can not continue."
#  endif
using namespace MeeGo;
#endif

#include <dcpwidgettypes.h>

#include <mdesktopentry.h>
#include <MApplication>
#include <MWindow>
#include <MAction>
#include <MBanner>
#include <MLabel>

#include <QVector>
#include <QList>
#include <QMap>

#define DEBUG
#include "../../src/debug.h"

#ifdef HAVE_QMSYSTEM
static QmUSBMode::Mode usbModes[3] =
       { QmUSBMode::Ask,
         QmUSBMode::MassStorage,
         QmUSBMode::OviSuite };

inline int
usbModeIndex (QmUSBMode::Mode mode)
{
    int ret = 0;
    for (int i = 0; i < 3; i++)
        if (usbModes[i] == mode)
        {
            ret = i;
            break;
        }
    return ret;
}
#endif

/******************************************************************************
 * The translations for the USBApplet contains special characters (like %1) so
 * we need to stub the qtTrId() method here.
 */
static QMap<QString, QString> catalog;
static bool                   catalogInitialized = false;

void
initializeCatalog ()
{
    if (catalogInitialized)
        return;

    catalog["qtn_usb_active_mode"] = "qtn_usb_active_mode %1";
}

QString
qtTrId (
        const char  *id,
        int          n)
{
    QString retVal;

    Q_UNUSED (n);
    initializeCatalog ();

    retVal = catalog[id];
    if (retVal.isEmpty())
        retVal = id;

    //SYS_DEBUG ("*** id          = %s", id);
    //SYS_DEBUG ("*** translation = %s", SYS_STR(retVal));

    return retVal;
}

/******************************************************************************
 * Stub for MBanner
 */
static QString mbannerSubtitle;

MBanner::MBanner ()
{
}

MBanner::~MBanner ()
{
}

void
MBanner::setTitle (const QString &text)
{
    mbannerSubtitle = text;
    mbannerSubtitle.replace ("<p>","");
    mbannerSubtitle.replace ("</p>","");
}

void
MBanner::setIconID (const QString &id)
{
    Q_UNUSED (id);
}

void
MSceneWindow::appear (MWindow *window, MSceneWindow::DeletionPolicy policy)
{
    Q_UNUSED (window);
    Q_UNUSED (policy);
}


/******************************************************************************
 * Ut_UsbApplet implementation.
 */
void
Ut_UsbApplet::init()
{
    m_Applet = new UsbApplet;

    QVERIFY (!m_Applet->m_MainWidget);
    m_Applet->init ();

    /*
     * Testing if the widget is not created yet.
     */
    QVERIFY (!m_Applet->m_MainWidget);
}

void
Ut_UsbApplet::cleanup()
{
    delete m_Applet;
    m_Applet = 0;
}

static int argc = 1;
static char *app_name = (char*) "./ut_usbapplet";

void
Ut_UsbApplet::initTestCase()
{
    m_App = new MApplication (argc, &app_name);
    m_window = new MWindow;
}

void
Ut_UsbApplet::cleanupTestCase()
{
    delete m_window;
    m_window = 0;
    m_App->deleteLater ();
    m_App = 0;
}

void
Ut_UsbApplet::testTitle ()
{
    QString title = m_Applet->title();
    QVERIFY (!title.isEmpty());
}

void
Ut_UsbApplet::testConstructWidget ()
{
#ifdef HAVE_QMSYSTEM
    UsbView                 *widget;
    bool                     backAccepted;
    QList<QmUSBMode::Mode>   availableModes;
    QStringList              availableModesStr;

    availableModes <<
            QmUSBMode::Connected <<
            QmUSBMode::DataInUse <<
            QmUSBMode::Disconnected <<
            QmUSBMode::MassStorage <<
            QmUSBMode::ChargingOnly <<
            QmUSBMode::OviSuite <<
            QmUSBMode::Ask <<
            QmUSBMode::ModeRequest <<
            QmUSBMode::Undefined;

    availableModesStr <<
            "Connected" <<
            "DataInUse" <<
            "Disconnected" <<
            "MassStorage" <<
            "ChargingOnly" <<
            "OviSuite" <<
            "Ask" <<
            "ModeRequest" <<
            "Undefined";

    /*
     * Testing if the applet creates a widget the first time.
     */
    widget = (UsbView *) m_Applet->constructStylableWidget (0);
    QVERIFY (widget);
    QVERIFY (m_Applet->m_MainWidget == widget);

    /*
     * Testing if the widget accepts the back. Our applets always accept back.
     */
    backAccepted = widget->back();
    QVERIFY (backAccepted);

    /*
     * For all default modes and modes we try to change to all the modes.
     */
    foreach (QmUSBMode::Mode mode, availableModes) {
        foreach (QmUSBMode::Mode defaultMode, availableModes) {
            m_Applet->m_logic->setDefaultMode (defaultMode);
            m_Applet->m_logic->setMode (mode);

#ifdef LOTDEBUG
            SYS_DEBUG ("set default and current mode: %s, %s",
                       SYS_STR (availableModesStr.at (
                        availableModes.indexOf (defaultMode))),
                       SYS_STR (availableModesStr.at (
                        availableModes.indexOf (mode))));
#endif

            for (int id = 0; id < 3; ++id) {
                mbannerSubtitle = "";
                m_Applet->m_logic->setMode (mode);
                widget->usbModeActivated (id);

#ifdef LOTDEBUG
                SYS_DEBUG ("usbModeActivated: %s",
                    SYS_STR (availableModesStr.at (
                        availableModes.indexOf (usbModes[id]))));
#endif

                if (usbModes[id] == defaultMode) {
                    // If we want to change to the current mode nothing happens.
                    QVERIFY (mbannerSubtitle.isEmpty ());
                } else if (mode == QmUSBMode::MassStorage ||
                           mode == QmUSBMode::OviSuite)  {

                    switch (defaultMode) {
                        case QmUSBMode::OviSuite:
                        case QmUSBMode::MassStorage:
                        case QmUSBMode::Ask:
                            /* check only ^^ these cases for notification... */
                            break;
                        default:
                            continue;
                    }

                    // If we are connected and try to change the mode we got the
                    // error message and we stay in the same mode we were in.
                    QCOMPARE (mbannerSubtitle,
                              qtTrId ("qtn_usb_change_incorrect"));
                    QCOMPARE (m_Applet->m_logic->getDefaultMode(), defaultMode);
                    QCOMPARE (m_Applet->m_logic->getMode(), mode);
                } else {
                    // Otherwise we should have reached the new mode.
                    //QVERIFY (mbannerSubtitle.isEmpty());
                    //QCOMPARE ((int)m_Applet->m_logic->getDefaultMode(), id);
                    // FIXME: There are the QmUSBMode::ChargingOnly case might
                    // be tested here along with the signal.
                }

            }
        }
    }


    /*
     * Testing if the applet knows about the destruction of the widget.
     */
    delete widget;
    QVERIFY (!m_Applet->m_MainWidget);
#endif
}

void
Ut_UsbApplet::testMenuItems ()
{
    QVector<MAction*> items = m_Applet->viewMenuItems ();

    QCOMPARE (items.size (), 0);
}

void
Ut_UsbApplet::testConstructBrief ()
{
    QVERIFY (m_Applet->constructBrief (0) == 0);
}

void
Ut_UsbApplet::testCurrentText ()
{
    UsbView *usbWidget =
        qobject_cast<UsbView*>(m_Applet->constructStylableWidget (0));
    QVERIFY (usbWidget);

#ifndef HAVE_QMSYSTEM
    QCOMPARE (usbWidget->currentText (), QString (""));
#else // if HAVE_QMSYSTEM
    usbWidget->m_logic->setMode (QmUSBMode::MassStorage);
    QCOMPARE (usbWidget->currentText (), qtTrId ("qtn_usb_storage_active"));

    usbWidget->m_logic->setMode (QmUSBMode::OviSuite);
    QCOMPARE (usbWidget->currentText (), qtTrId ("qtn_usb_sync_active"));

#ifndef MEEGO
    usbWidget->m_logic->setMode (QmUSBMode::SDK);
    QCOMPARE (usbWidget->currentText (), QString ("SDK"));
#endif

    usbWidget->m_logic->setMode (QmUSBMode::ChargingOnly);
    QCOMPARE (usbWidget->currentText (), qtTrId ("qtn_usb_charging"));

    usbWidget->m_logic->setMode (QmUSBMode::Disconnected);
    QCOMPARE (usbWidget->currentText (), QString (""));
#endif
}

void
Ut_UsbApplet::testInfoWidget ()
{
    UsbView *usbWidget =
        qobject_cast<UsbView*>(m_Applet->constructStylableWidget (0));
    QVERIFY (usbWidget);

    // infoLabel should not exists at this point
    QVERIFY (! usbWidget->m_infoLabel);

#ifdef HAVE_QMSYSTEM
    usbWidget->m_logic->setMode (QmUSBMode::MassStorage);
    usbWidget->updateInfoLabel ();
    QVERIFY (usbWidget->m_infoLabel);
    QCOMPARE (usbWidget->m_infoLabel->text (),
              qtTrId ("qtn_usb_storage_active"));

    usbWidget->m_logic->setMode (QmUSBMode::OviSuite);
    usbWidget->updateInfoLabel ();
    QVERIFY (usbWidget->m_infoLabel);
    QCOMPARE (usbWidget->m_infoLabel->text (),
              qtTrId ("qtn_usb_sync_active"));

    usbWidget->m_logic->setMode (QmUSBMode::ChargingOnly);
    usbWidget->updateInfoLabel ();
    QVERIFY (usbWidget->m_infoLabel);
    QCOMPARE (usbWidget->m_infoLabel->text (),
              qtTrId ("qtn_usb_charging"));

    usbWidget->m_logic->setMode (QmUSBMode::Disconnected);
    usbWidget->updateInfoLabel ();
    QVERIFY (! usbWidget->m_infoLabel);
#endif

}

QTEST_APPLESS_MAIN(Ut_UsbApplet)

