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
#include "ut_offlineapplet.h"
#include <DcpWidgetTypes>
#include <DcpWidget>
#include <MMessageBox>
#include <MDialog>
#include <MApplication>
#include <MBanner>

#include "offlineapplet.h"
#include "offlinebrief.h"

#ifdef HAVE_QMSYSTEM
#  include "qmdevicemode_stub.h"
#endif

#include "signalchecker.h"

#define DEBUG
#include "../../src/debug.h"

static const char *signalValuesChanged = SIGNAL (valuesChanged ());

int   argc = 1;
char *argv[] = {
    (char *) "./ut_offlineapplet",
    NULL };
/******************************************************************************
 * Stub for MMessageBox
 */
static QString mmessageBoxText;
static int mmessageBoxApereance;

MMessageBox::MMessageBox (
    const QString          &title,
    const QString          &text,
    M::StandardButtons      buttons)
{
    SYS_DEBUG ("");
    Q_UNUSED (title);
    Q_UNUSED (buttons);

    mmessageBoxText = text;
}

int
MDialog::result () const
{
    return MDialog::Accepted;
}

void
MDialog::appear (MSceneWindow::DeletionPolicy policy)
{
    Q_UNUSED (policy);

    mmessageBoxApereance = true;
}

void
MDialog::appear (MWindow *window, MSceneWindow::DeletionPolicy policy)
{
    Q_UNUSED (window);
    Q_UNUSED (policy);

    mmessageBoxApereance = true;
}

/******************************************************************************
 * Stub for MNotification
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
}

/******************************************************************************
 * Ut_offlineApplet implementation.
 */
void
Ut_OfflineApplet::init()
{
    mmessageBoxText = "";
    mmessageBoxApereance = false;
    mbannerSubtitle = "";
}

void
Ut_OfflineApplet::cleanup()
{
}

void
Ut_OfflineApplet::initTestCase()
{
    m_App = new MApplication(argc, argv);
    m_App->setQuitOnLastWindowClosed (false);

    m_Applet = new OfflineApplet;
}

void
Ut_OfflineApplet::cleanupTestCase()
{
    delete m_Applet;
    m_Applet = 0;

    m_App->deleteLater ();
    m_App = 0;
}

void
Ut_OfflineApplet::testTitle ()
{
    QString title = m_Applet->title();
    QVERIFY (title.isEmpty());
}

void
Ut_OfflineApplet::testMenu ()
{
    QVector<MAction *> menu = m_Applet->viewMenuItems();
    QVERIFY (menu.isEmpty());
}

void
Ut_OfflineApplet::testConstructWidget ()
{
    DcpWidget* widget = m_Applet->constructWidget(1);
    QVERIFY (!widget);
}

void
Ut_OfflineApplet::testBriefConstruct ()
{
    #ifdef HAVE_QMSYSTEM
    DcpBrief* widget;

    gQmDeviceModeStub->stubReset ();

    gQmDeviceModeStub->stubSetReturnValue<MeeGo::QmDeviceMode::DeviceMode> (
        "getMode", MeeGo::QmDeviceMode::Normal);
    widget = m_Applet->constructBrief(1);

    QVERIFY (widget);
    QCOMPARE (int(widget->widgetTypeID()), int(DcpWidgetType::Toggle));
    QVERIFY (widget->valueText ().isEmpty ());
    QCOMPARE (widget->titleText (), qtTrId ("qtn_sett_main_flightmode"));
    QCOMPARE (widget->toggle (), false);

    OfflineBrief *test = qobject_cast<OfflineBrief *>(widget);
    QVERIFY (test);
    QCOMPARE (test->currentText (), qtTrId ("qtn_offl_activate"));

    delete widget;
    #endif
}

void
Ut_OfflineApplet::testCurrentText ()
{
    #ifdef HAVE_QMSYSTEM
    OfflineBrief brief;

    brief.m_LastMode = MeeGo::QmDeviceMode::Normal;
    QCOMPARE (brief.currentText(), qtTrId("qtn_offl_activate"));

    brief.m_LastMode = MeeGo::QmDeviceMode::Flight;
    QCOMPARE (brief.currentText(), qtTrId("qtn_offl_deactivate"));

    QCOMPARE (brief.titleText (), qtTrId ("qtn_sett_main_flightmode"));

    brief.m_LastMode = MeeGo::QmDeviceMode::Error;
    QVERIFY (brief.currentText().isEmpty() == false);
    #endif
}


void
Ut_OfflineApplet::testBriefInit ()
{
    #ifdef HAVE_QMSYSTEM
    OfflineBrief *brief;

    gQmDeviceModeStub->stubReset ();

    gQmDeviceModeStub->stubSetReturnValue<MeeGo::QmDeviceMode::DeviceMode> (
        "getMode", MeeGo::QmDeviceMode::Normal);

    brief = new OfflineBrief();
    QVERIFY (brief);
    QCOMPARE (brief->toggle (), false);
    delete brief;

    gQmDeviceModeStub->stubSetReturnValue<MeeGo::QmDeviceMode::DeviceMode> (
        "getMode", MeeGo::QmDeviceMode::Flight);
    brief = new OfflineBrief();
    QVERIFY (brief);
    QCOMPARE (brief->toggle (), true);
    delete brief;
    #endif
}

void
Ut_OfflineApplet::testBriefValueText ()
{
    #ifdef HAVE_QMSYSTEM
    OfflineBrief *brief;

    gQmDeviceModeStub->stubReset ();
    gQmDeviceModeStub->stubSetReturnValue<MeeGo::QmDeviceMode::DeviceMode> (
        "getMode", MeeGo::QmDeviceMode::Normal);

    brief = new OfflineBrief();
    QVERIFY (brief);
    QCOMPARE (brief->toggle (), false);
    // QCOMPARE (brief->valueText(), qtTrId("qtn_offl_activate"));

    SignalChecker m_sChecker(brief);
    m_sChecker.addSignalChecker(signalValuesChanged);

    m_sChecker.increaseSigCounter(signalValuesChanged);
    brief->devModeChanged(MeeGo::QmDeviceMode::Flight);
    QCOMPARE (brief->toggle (), true);
    // QCOMPARE (brief->valueText(), qtTrId("qtn_offl_deactivate"));
    m_sChecker.check();

    m_sChecker.increaseSigCounter(signalValuesChanged);
    brief->devModeChanged(MeeGo::QmDeviceMode::Normal);
    QCOMPARE (brief->toggle (), false);
    // QCOMPARE (brief->valueText(), qtTrId("qtn_offl_activate"));
    delete brief;
    #endif
}

void
Ut_OfflineApplet::testBriefSetToggle ()
{
    #ifdef HAVE_QMSYSTEM
    OfflineBrief *brief;

    gQmDeviceModeStub->stubReset ();
    gQmDeviceModeStub->stubSetReturnValue<MeeGo::QmDeviceMode::DeviceMode> (
        "getMode", MeeGo::QmDeviceMode::Normal);

    brief = new OfflineBrief();
    QVERIFY (brief);
    QCOMPARE (brief->toggle (), false);
    // QCOMPARE (brief->valueText(), qtTrId("qtn_offl_activate"));

    SignalChecker m_sChecker(brief);
    m_sChecker.addSignalChecker(signalValuesChanged);

    // This should not change the text
    brief->setToggle(true);
    QCOMPARE (mbannerSubtitle, qtTrId("qtn_offl_entering"));

    QCOMPARE (brief->toggle (), false);
    // QCOMPARE (brief->valueText(), qtTrId("qtn_offl_activate"));
    QCOMPARE (gQmDeviceModeStub->stubCallCount("setMode"), 1);
    QCOMPARE (gQmDeviceModeStub->stubLastParameters<MeeGo::QmDeviceMode::DeviceMode> (0), MeeGo::QmDeviceMode::Flight);

    m_sChecker.increaseSigCounter(signalValuesChanged);
    brief->devModeChanged(MeeGo::QmDeviceMode::Flight);
    QCOMPARE (brief->toggle (), true);
    // QCOMPARE (brief->valueText(), qtTrId("qtn_offl_deactivate"));
    m_sChecker.check();

    delete brief;
    #endif
}

void
Ut_OfflineApplet::testProcessDialogResult()
{
    #ifdef HAVE_QMSYSTEM
    OfflineBrief *brief;

    gQmDeviceModeStub->stubReset ();
    gQmDeviceModeStub->stubSetReturnValue<MeeGo::QmDeviceMode::DeviceMode> (
        "getMode", MeeGo::QmDeviceMode::Flight);

    brief = new OfflineBrief();
    QVERIFY (brief);

    brief->setToggle(false);
    QCOMPARE(mmessageBoxText, qtTrId("qtn_offl_exiting"));
    QVERIFY(mmessageBoxApereance);
    brief->processDialogResult();
    QCOMPARE (gQmDeviceModeStub->stubCallCount("setMode"), 1);
    QCOMPARE (gQmDeviceModeStub->stubLastParameters<MeeGo::QmDeviceMode::DeviceMode> (0), MeeGo::QmDeviceMode::Normal);

    delete brief;
    #endif
}

QTEST_APPLESS_MAIN(Ut_OfflineApplet)


