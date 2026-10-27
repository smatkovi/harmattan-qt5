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
#include "displaybusinesslogic.h"

#include <QTimer>
#include <MGConfItem>
#include <QSettings>
#include <QVariant>
#include <QString>
#include <stdlib.h>
#include <QDBusInterface>
#include <QDBusObjectPath>

#include <mce/dbus-names.h>

//#define DEBUG
#define WARNING
#include "../debug.h"

static const QString GConfDir ("/system/osso/dsm/display");


using namespace MeeGo;

/*
 * gconftool --recursive-list /system/osso/dsm/display
 * gconftool --set /system/osso/dsm/display/use_low_power_mode --type boolean false
 *
 * gconftool --recursive-list /system/osso/dsm/locks
 * gconftool --set /system/osso/dsm/locks/tklock_double_tap_gesture --type integer 0
 */
static const QString LowPowerKey = GConfDir + "/use_low_power_mode";
static const QString DimTimeoutsKey (
        "/system/osso/dsm/display/possible_display_dim_timeouts");
static const QString DoubleTapKey (
        "/system/osso/dsm/locks/tklock_double_tap_gesture");
static const QString colorProfileKey (
        "/system/osso/dsm/display/color_profile");

static const QString mcompositorConfName ("mcompositor");
static const QString closeFromTopKey ("swipe-action-down");
static const QString closeFromTopEnabled ("close");
static const QString closeFromTopDisabled ("away");

static int TIMEGAP = 5; // time gap between blanking and dimming

DisplayBusinessLogic::DisplayBusinessLogic (
        QObject* parent) :
    QObject (parent),
    m_Display (new MeeGo::QmDisplayState),
    m_compositorConf (0),
    m_psmValue (false),
    m_MceDBusIf(0),
    m_CurrentColorProfile(""),
    m_ColorProfileToSet("")
{
    m_possibleDimValues = new MGConfItem (DimTimeoutsKey);
    m_lowPower = new MGConfItem (LowPowerKey);
    m_DoubleTap = new MGConfItem (DoubleTapKey);
    m_devicemode = new MeeGo::QmDeviceMode (this);

    /*
     * DBus is slow....
     */
    m_CurrentColorProfileItem = new MGConfItem (colorProfileKey);
    connect (m_CurrentColorProfileItem, SIGNAL (valueChanged ()),
             SLOT (currentColorProfileChanged ()));
    m_CurrentColorProfile = m_CurrentColorProfileItem->value().toString();
    m_AvailColorProfiles << "Neutral" << "Vivid" << "Muted";

#ifndef MEEGO
    m_compositorConf = new QSettings (
        mcompositorConfName, mcompositorConfName);
#endif

    connect (m_lowPower, SIGNAL (valueChanged ()),
             SLOT (lpmValueChanged ()));
    connect (m_DoubleTap, SIGNAL (valueChanged ()),
             SLOT (doubleTapValueChanged ()));
    connect (m_devicemode,
             SIGNAL (devicePSMStateChanged (MeeGo::QmDeviceMode::PSMState)),
             SLOT (PSMStateChanged (MeeGo::QmDeviceMode::PSMState)));

    m_psmValue =
        (m_devicemode->getPSMState () == MeeGo::QmDeviceMode::PSMStateOn);
    
    initiateMceQueries ();
}

DisplayBusinessLogic::~DisplayBusinessLogic ()
{
    delete m_Display;
    m_Display = 0;

#ifndef MEEGO
    delete m_compositorConf;
    m_compositorConf = 0;
#endif

    delete m_possibleDimValues;
    m_possibleDimValues = 0;

    delete m_lowPower;
    m_lowPower = 0;

    delete m_DoubleTap;
    m_DoubleTap = 0;

    if(m_MceDBusIf)
    delete m_MceDBusIf;
}

/*!
 * Returns a list of brightness values the underlying hw system accepts.
 */
QList<int>
DisplayBusinessLogic::brightnessValues ()
{
    QList<int> values;
    int        max;

    /*
     * If the QmSystem is available we use it to get the maximum brightness
     * value, if not we read this value from the GConf database.
     */
    max = m_Display->getMaxDisplayBrightnessValue();

    // Well, this must be some kind of last minute check...
    max = (max > 0 ? max : 5);

    /*
     * What we use here is a list of accepted values from 1 to the maximum
     * value.
     */
    for (int i = 1; i <= max; ++i) {
        SYS_DEBUG ("*** BrightnessValues[%d] = %d", i - 1, i);
        values << i;
    }

    return values;
}

/*!
 * Returns the current brighness value for the screen. Ooops, this function
 * actually returns the index. FIXME: Should be checked and renamed.
 */
int
DisplayBusinessLogic::selectedBrightnessValueIndex ()
{
    QList<int> values = brightnessValues();
    int        index;

    index = values.indexOf (selectedBrightnessValue());
    if (index < 0) {
        SYS_WARNING ("Value not in available values list.");
        index = values.size() / 2;
        setBrightnessValue (values.at(index));
    }

    SYS_DEBUG ("*** index                       = %d", index);
    return index;
}

int
DisplayBusinessLogic::selectedBrightnessValue ()
{
    /*
     * When we have the QmSystem we use that, otherwise we read the data from
     * the GConf database.
     */
    return m_Display->getDisplayBrightnessValue();
}

/*!
 * Returns a list that contains the available touch screen backlight time-out
 * values. All the elements are measuring timeout values in seconds.
 */
QList<int>
DisplayBusinessLogic::screenLightsValues ()
{
    QList<int> values;

    QList<QVariant> gconf_values =
        m_possibleDimValues->value().toList ();

    if (gconf_values.isEmpty ())
    {
        values << 15 << 30 << 60 << 120 << 180;
        SYS_WARNING ("GConf doesn't have possible dim "
                     "values, using the default list");
    }
    else
    {
        SYS_DEBUG ("Possible dim values from GConf:");
        foreach (QVariant i, gconf_values)
        {
            values << i.toInt ();
            SYS_DEBUG ("%d", i.toInt ());
        }
    }

    return values;
}

/*!
 * FIXME: The name of the method should be modified: this method actually
 * returns the index of the screen dim timeout value.
 */
int
DisplayBusinessLogic::selectedScreenLightsValue ()
{
    int index;

    QList<int> values = screenLightsValues ();
    index = values.indexOf (m_Display->getDisplayDimTimeout ());

    if (index < 0) {
        index = 1;
        setScreenLightTimeouts (index);
    }

    return index;
}

/*!
 * Returns reference to inner list that contains the available color profiles.
 * The elements currently are translation ids.
 */
const QStringList&
DisplayBusinessLogic::colorProfileValues ()
{
    return m_AvailColorProfiles;
}

/*!
 * Returns reference to a map from color profile ids to translation ids.
 */
const QMap<QString, QString>&
DisplayBusinessLogic::colorProfileMap ()
{
    //% "Normal"
    static const char* QtnDispProfileNormal = QT_TRID_NOOP("qtn_disp_profile_normal");
    //% "Vivid"
    static const char* QtnDispProfileVivid = QT_TRID_NOOP("qtn_disp_profile_vivid");
    //% "Muted"
    static const char* QtnDispProfileMuted = QT_TRID_NOOP("qtn_disp_profile_muted");

    if(m_ColorProfileTextIds.empty()) {
        m_ColorProfileTextIds.insert("Neutral", QtnDispProfileNormal);
        m_ColorProfileTextIds.insert("Vivid", QtnDispProfileVivid);
        m_ColorProfileTextIds.insert("Muted", QtnDispProfileMuted);
    }
    return m_ColorProfileTextIds;
}

/*!
 * FIXME: The name of the method should be modified: this method actually
 * returns the index of the currently set color profile value.
 */
int
DisplayBusinessLogic::selectedColorProfileValue ()
{
    int index = m_AvailColorProfiles.indexOf(m_CurrentColorProfile);
    return index;
}

/*!
 * \param value The slider value, that starts from 0, the qmsystem value starts
 *   from 1, so we add +1 to this parameter.
 */
void
DisplayBusinessLogic::setBrightnessValue (
        int value)
{
    SYS_DEBUG ("*** value + 1 = %d", value + 1);

    m_Display->setDisplayBrightnessValue (value + 1);
}

/*!
 * This function will take an index of the 'screen lights off' list that stores
 * the screen dim delay values and sets the dim timeout and the blank timeout
 * accordingly.
 */
void
DisplayBusinessLogic::setScreenLightTimeouts (
        int     index)
{
    /*
     * We got the index, not the value.
     */
    QList<int> values = screenLightsValues ();
    int seconds = values[index];

    SYS_DEBUG ("*** index   = %d", index);
    SYS_DEBUG ("*** seconds = %d", seconds);
    m_Display->setDisplayDimTimeout (seconds);
    /*
     * Originally it was seconds + TIMEGAP here, but as I see the blank timeout
     * is relative to the dim timeout value.
     */
    m_Display->setDisplayBlankTimeout (TIMEGAP);
}

/*!
 * This function will take an index of the 'color profiles' list that stores
 * the color profile values and sets the color profile accordingly.
 */
void
DisplayBusinessLogic::setColorProfile (
        int     index)
{
    if (index < 0 || m_AvailColorProfiles.size() <= index){
        SYS_WARNING (
                "Index (%d) for selecting available color profile is "
                "out of range. Available range is: 0-%d",
                index, m_AvailColorProfiles.size() - 1 );
    	return;
    }
    
    QString profile = m_AvailColorProfiles.at(index);

    if(profile == m_CurrentColorProfile)
        return; // Nothing to do

    m_ColorProfileToSet = profile;

    setupMceDBusIf ();

    QList<QVariant> params;
    params.append(QVariant(QString(m_AvailColorProfiles.at(index))));

    m_MceDBusIf->callWithCallback (
            QString (MCE_COLOR_PROFILE_CHANGE_REQ),
            params, this,
            SLOT (colorProfileIsSet(QDBusMessage)),
            SLOT (DBusMessagingFailure (QDBusError)));
}

/*!
 * This slot is called when the current color profile is set
 * through the dbus.
 */
void
DisplayBusinessLogic::colorProfileIsSet (QDBusMessage msg)
{
    if(msg.type() == QDBusMessage::ReplyMessage){
        SYS_DEBUG("Current color profile changed.");
        if(!m_ColorProfileToSet.isEmpty())
	        m_CurrentColorProfile = m_ColorProfileToSet;
    } else {
        SYS_DEBUG("Could not set color profile.");
        m_ColorProfileToSet.clear();
    }
}

void
DisplayBusinessLogic::setLowPowerMode (
        bool enable)
{
    SYS_DEBUG ("enable = %s", SYS_BOOL (enable));
    m_lowPower->set (QVariant(enable));
}

void
DisplayBusinessLogic::setDoubleTapWakes (
        bool enable)
{
    SYS_DEBUG ("enable = %s", SYS_BOOL (enable));
    int ival = enable ? 1 : 0;

    m_DoubleTap->set (QVariant(ival));
}

bool
DisplayBusinessLogic::getLowPowerMode ()
{
    return m_lowPower->value(QVariant(false)).toBool();
}

bool
DisplayBusinessLogic::getDoubleTapWakes ()
{
    return m_DoubleTap->value().toInt() != 0;
}

void
DisplayBusinessLogic::lpmValueChanged ()
{
    bool value = m_lowPower->value().toBool();

    emit lowPowerModeChanged (value);
}

void
DisplayBusinessLogic::doubleTapValueChanged ()
{
    int value = m_DoubleTap->value().toInt();

    emit doubleTapModeChanged (value != 0);
}

/*!
 * This slot will be called when the device power save mode is changed. The
 * method will send the PSMValueReceived() signal.
 */
void
DisplayBusinessLogic::PSMStateChanged (
        MeeGo::QmDeviceMode::PSMState state)
{
    m_psmValue = (state == MeeGo::QmDeviceMode::PSMStateOn);

    SYS_DEBUG ("*** state = %d", (int)state);
    SYS_DEBUG ("Emitting PSMValueReceived (%s)", SYS_BOOL(m_psmValue));
    emit PSMValueReceived (m_psmValue);
}

bool
DisplayBusinessLogic::PSMValue ()
{
    return m_psmValue;
}

void
DisplayBusinessLogic::setCloseFromTop (bool enable)
{
    SYS_DEBUG ("enable = %s", SYS_BOOL (enable));
#ifndef MEEGO
    QString val = enable ? closeFromTopEnabled : closeFromTopDisabled;

    bool oldVal = getCloseFromTopValue ();

    m_compositorConf->setValue (closeFromTopKey, val);
    m_compositorConf->sync ();

    // HACK: BEGIN
    if (enable != oldVal)
    {
        // Notify the mcompositor about the change
        SYS_DEBUG ("Notifing mcompositor...");
        system ("kill -SIGHUP $(pgrep mcompositor)");
    }
    // HACK: END
#endif
}

bool
DisplayBusinessLogic::getCloseFromTopValue ()
{
    bool retval = false;

    /*
     * TODO: FIXME:
     * Implement a tracker for this key to listen for
     * value-changes... (but most probably no-one will
     * modify this key...)
     */
#ifndef MEEGO
    QString setting = m_compositorConf->value (
        closeFromTopKey, QVariant (closeFromTopDisabled)).toString ();

    retval = (setting == closeFromTopEnabled);
#endif

    SYS_DEBUG ("returning %s", SYS_BOOL (retval));
    return retval;
}

/*!
 * Initilize MCE DBus interface.
 */
void
DisplayBusinessLogic::setupMceDBusIf ()
{
    if (!m_MceDBusIf){
        m_MceDBusIf = new QDBusInterface (
                MCE_SERVICE,
                MCE_REQUEST_PATH,
                MCE_REQUEST_IF,
                QDBusConnection::systemBus ());
    }
}

/*!
 * Initiates a query to receive the list of available color profiles.
 */
void
DisplayBusinessLogic::initiateMceQueries ()
{
    setupMceDBusIf ();

    m_MceDBusIf->callWithCallback (
            QString (MCE_COLOR_PROFILE_IDS_GET),
            QList<QVariant> (), this,
            SLOT (availableColorProfilesReceivedSlot(QStringList)),
            SLOT (DBusMessagingFailure (QDBusError)));
}

/*!
 * This slot is called when the current color profile is received
 * through the dbus.
 */
void
DisplayBusinessLogic::currentColorProfileChanged ()
{
    QString profile = m_CurrentColorProfileItem->value().toString();

    if (profile != m_CurrentColorProfile) {
        m_CurrentColorProfile = profile;

        SYS_DEBUG("Received current color profile: %s", 
            SYS_STR (profile));
    
        emit currentColorProfileReceived ();
    }
}

/*!
 * This slot is called when the list of available color profiles received
 * through the dbus.
 */
void
DisplayBusinessLogic::availableColorProfilesReceivedSlot (
        QStringList list)
{
    if (m_AvailColorProfiles == list) 
        return;


    SYS_DEBUG("Received available color profiles: %s",
            SYS_STR (list.join(", ")));

    m_AvailColorProfiles = list;
    emit availableColorProfilesReceived ();
}

/*!
 * This slot is called when an error is occured during the dbus communication.
 * The error message is printed as a warning message.
 */
void
DisplayBusinessLogic::DBusMessagingFailure (
        QDBusError error)
{
    Q_UNUSED (error);
    SYS_WARNING ("%s: %s", SYS_STR (error.name()),
            SYS_STR (error.message()));
}

