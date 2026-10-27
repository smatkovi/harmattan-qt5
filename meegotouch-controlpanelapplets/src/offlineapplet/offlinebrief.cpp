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
#include "offlinebrief.h"

#include <DcpWidgetTypes>
#include <MApplication>
#include <MBanner>
#include <MMessageBox>
#include <MLabel>

#define DEBUG
#include "../debug.h"

#ifdef HAVE_QMSYSTEM
OfflineBrief::OfflineBrief () :
    m_DevMode (new QmDeviceMode()),
    m_infoBanner (0)
{
    connect (m_DevMode,
             SIGNAL (deviceModeChanged (MeeGo::QmDeviceMode::DeviceMode)),
             SLOT (devModeChanged (MeeGo::QmDeviceMode::DeviceMode)));
    m_LastMode = m_DevMode->getMode();
}
#else
OfflineBrief::OfflineBrief (),
    m_infoBanner (0)
{
    /*
     * FIXME: To install a version that works without the help of the QmSystem
     * library.
     */
}
#endif

OfflineBrief::~OfflineBrief()
{
    #ifdef HAVE_QMSYSTEM
    delete m_DevMode;
    #endif
}

#ifdef HAVE_QMSYSTEM
void
OfflineBrief::devModeChanged (
        MeeGo::QmDeviceMode::DeviceMode mode)
{
#ifdef DEBUG
    switch (mode)
    {
        case MeeGo::QmDeviceMode::Flight:
            SYS_DEBUG ("mode = Flight mode");
            break;
        case MeeGo::QmDeviceMode::Normal:
            SYS_DEBUG ("mode = Normal");
            break;
        default:
            SYS_DEBUG ("mode = Error!!!");
            break;
    }
#endif
    m_LastMode = mode;
    emit valuesChanged();
}
#endif

QString
OfflineBrief::titleText () const
{
    //% "Flight mode"
    return qtTrId ("qtn_sett_main_flightmode");
}

QString
OfflineBrief::valueText() const
{
#if 0
    return currentText();
#endif

    return "";
}

QString
OfflineBrief::currentText() const
{
    #ifdef HAVE_QMSYSTEM
    switch (m_LastMode)
    {
        case QmDeviceMode::Flight:
            //% "Deactivate offline mode"
            return qtTrId("qtn_offl_deactivate");
        case QmDeviceMode::Normal:
        default:
            //% "Activate offline mode"
            return qtTrId("qtn_offl_activate");
    }
    #endif

    return QString("No QmSystem");
}

bool
OfflineBrief::toggle () const
{
    return (m_LastMode == QmDeviceMode::Flight);
}

void
OfflineBrief::setToggle (
        bool toggle)
{
    SYS_DEBUG ("toggle = %s", SYS_BOOL (toggle));
    /*
     * Don't do anything if we already in the desired mode
     */
    if (toggle && m_LastMode == QmDeviceMode::Flight)
        return;
    else if ((! toggle) && m_LastMode == QmDeviceMode::Normal)
        return;

#ifdef HAVE_QMSYSTEM
    if (! toggle)
    {
        MMessageBox* dialog =
                             //% "Exit offline mode?"
            new MMessageBox (qtTrId ("qtn_offl_exiting_title"),
                             //% "Connections will be restored."
                             qtTrId ("qtn_offl_exiting"),
                             M::YesButton | M::NoButton);
        /*
         * This will set the 'Normal' mode if dialog accepted
         */
        connect (dialog, SIGNAL (accepted ()),
                 SLOT (processDialogResult ()));
        /*
         * This will switch back the button for the proper state
         */
        connect (dialog, SIGNAL (rejected ()),
                 this, SIGNAL (valuesChanged ()));
        dialog->appear (MApplication::activeWindow (),
                        MSceneWindow::DestroyWhenDone);
    }
    else
    {
        bool success = m_DevMode->setMode (QmDeviceMode::Flight);
        SYS_DEBUG ("m_DevMode->setMode (Flight) success: %s", SYS_BOOL (success));
        if (success)
        {
            m_infoBanner = new MBanner;
            m_infoBanner->setStyleName ("InformationBanner");
            m_infoBanner->setObjectName ("InfoBanner");

            //% "Closing all connections. Switching to offline mode."
            m_infoBanner->setTitle (qtTrId ("qtn_offl_entering"));
            m_infoBanner->appear (
                MApplication::activeWindow (),
                MSceneWindow::DestroyWhenDone);
            // Set to NULL, as will destroy itself
            m_infoBanner = 0;
        }
    }
#endif
}

/*
 * FIXME: This slot is only called when the dialog is accepted...
 *        Re-name this at once...
 */
void
OfflineBrief::processDialogResult ()
{
#ifdef HAVE_QMSYSTEM
    bool success = m_DevMode->setMode (QmDeviceMode::Normal);
    SYS_DEBUG ("m_DevMode->setMode (Normal) success: %s", SYS_BOOL (success));

    emit valuesChanged();
#endif
}

int
OfflineBrief::widgetTypeID () const
{
    return DcpWidgetType::Toggle;
}

