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

#undef DEBUG
#undef WARNING

int s_brightnessvalue;

DisplayBusinessLogic::DisplayBusinessLogic (
        QObject* parent) :
        QObject (parent)
{
    qDebug (" DisplayBusinessLogic::DisplayBusinessLogic called" );
    s_brightnessvalue = 2;
}

DisplayBusinessLogic::~DisplayBusinessLogic ()
{
}

/*!
 * Returns a list of brightness values the underlying hw system accepts.
 */
QList<int> 
DisplayBusinessLogic::brightnessValues ()
{
    QList<int> values;
    int        max = 5;


    for (int i = 1; i <= max; ++i)
    {
        qDebug ("*** BrightnessValues[%d] = %d", i - 1, i);
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
    return s_brightnessvalue;
}

int
DisplayBusinessLogic::selectedBrightnessValue ()
{
    return 0;
}

/*!
 * Returns a list that contains the available touch screen backlight time-out 
 * values. All the elements are measuring timeout values in seconds.
 */
QList<int>
DisplayBusinessLogic::screenLightsValues ()
{
    QList<int> values;
    return values;
}

/*!
 * FIXME: The name of the method should be modified: this method actually
 * returns the index of the screen dim timeout value.
 */
int 
DisplayBusinessLogic::selectedScreenLightsValue ()
{
    return 0;
}

const QStringList&
DisplayBusinessLogic::colorProfileValues ()
{
    static QStringList values;

    return values;
}

const QMap<QString, QString>&
DisplayBusinessLogic::colorProfileMap ()
{
	static QMap<QString, QString> map;
	return map;
}

int
DisplayBusinessLogic::selectedColorProfileValue ()
{
	return 0;
}

/*!
 * \param value The slider value, that starts from 0, the qmsystem value starts
 *   from 1, so we add +1 to this parameter.
 */
void 
DisplayBusinessLogic::setBrightnessValue (
        int value)
{
    s_brightnessvalue = value;
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
    Q_UNUSED (index);
}

void
DisplayBusinessLogic::setColorProfile (
		int     index)
{
    Q_UNUSED(index);
}

void
DisplayBusinessLogic::colorProfileIsSet (QDBusMessage msg)
{
	Q_UNUSED(msg);
}

void
DisplayBusinessLogic::setLowPowerMode (
        bool enable)
{
    Q_UNUSED (enable);
}

void
DisplayBusinessLogic::setDoubleTapWakes (
        bool enable)
{
    Q_UNUSED (enable);
}

bool
DisplayBusinessLogic::getLowPowerMode ()
{
    return true;
}

bool
DisplayBusinessLogic::getDoubleTapWakes ()
{
    return true;
}

void
DisplayBusinessLogic::lpmValueChanged ()
{

}

void
DisplayBusinessLogic::doubleTapValueChanged ()
{
}

#ifdef HAVE_QMSYSTEM
/*!
 * This slot will be called when the device power save mode is changed. The
 * method will send the PSMValueReceived() signal.
 */
void
DisplayBusinessLogic::PSMStateChanged (
        MeeGo::QmDeviceMode::PSMState state)
{
    Q_UNUSED (state);
}
#endif

bool
DisplayBusinessLogic::PSMValue ()
{
//just giving back false here
    return false;
}

void
DisplayBusinessLogic::setCloseFromTop (bool enable)
{
    Q_UNUSED (enable);
}

bool
DisplayBusinessLogic::getCloseFromTopValue ()
{
    bool retval = false;
    return retval;
}

void
DisplayBusinessLogic::setupMceDBusIf ()
{
}

void
DisplayBusinessLogic::initiateMceQueries ()
{
}

void
DisplayBusinessLogic::availableColorProfilesReceivedSlot (
        QStringList list)
{
	Q_UNUSED(list);
}

void
DisplayBusinessLogic::currentColorProfileChanged ()
{
}

void
DisplayBusinessLogic::DBusMessagingFailure (
		QDBusError error)
{
	Q_UNUSED (error);
}


