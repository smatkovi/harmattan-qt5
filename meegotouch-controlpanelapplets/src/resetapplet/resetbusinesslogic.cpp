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
#include "resetbusinesslogic.h"
#include <QString>
#include <MGConfItem>

#define WANT_LOCK_CODE

#include <QVariant>
#include <QDBusInterface>
#include <QDBusMessage>
#ifdef HAVE_DEVICELOCK
#include <devicelock/devicelock.h>
#endif

#ifdef HAVE_QMSYSTEM
#include <qmusbmode.h>
#endif

#define DEBUG
#define WARNING
#include "../debug.h"

ResetBusinessLogic::ResetBusinessLogic() :
    m_devlock (0)
{
#ifdef HAVE_DEVICELOCK
    m_devlock =
        new QDBusInterface (QString (DEVLOCK_SERVICE),
                            QString (DEVLOCK_PATH),
                            QString ("com.nokia.devicelock"),
                            QDBusConnection::systemBus ());
    connect (m_devlock, SIGNAL (passwordPromptResult (bool)),
             this, SLOT (passwordResult (bool)));
#endif
}

ResetBusinessLogic::~ResetBusinessLogic()
{
#ifdef HAVE_DEVICELOCK
    delete m_devlock;
#endif
}

/*!
 * Performs the 'Restore original settings' action.
 */
void
ResetBusinessLogic::performRestoreSettings ()
{
    char *command = (char *) "/usr/sbin/clean-device.sh --rfs";
    int   retval;

    SYS_DEBUG ("*** executing command: %s", command);
    retval = system (command);
    if (retval != 0) {
        SYS_WARNING ("The command '%s' failed: %m", command);
    }
}

/*!
 * Performs the 'Clear device' action.
 */
void 
ResetBusinessLogic::performClearData ()
{
    char *command = (char *) "/usr/sbin/clean-device.sh --cud-reset";
    int   retval;

    SYS_DEBUG ("*** executing command: %s", command);
    retval = system (command);
    if (retval != 0) {
        SYS_WARNING ("The command '%s' failed: %m", command);
    }
}

/*!
 * Asks user for device-lock code when it is set
 */
void
ResetBusinessLogic::getAccess ()
{

    if (isUsbConnected ())
        return;

    bool success = false;

#if defined(HAVE_DEVICELOCK) && defined (WANT_LOCK_CODE)
    QDBusMessage message;
    message = m_devlock->call (
            QDBus::Block, 
            QString ("setState"),
            DeviceLock::DeviceLockEnums::Device,
            DeviceLock::DeviceLockEnums::PasswordPromptIfEnabled);

    QString errorString = message.errorMessage();
    QString errorName = message.errorName ();
    SYS_DEBUG ("*** errorMessage = %s", SYS_STR(errorString));
    SYS_DEBUG ("*** errorName    = %s", SYS_STR(errorName));

    /*
     * For now i don't care about errors, i assume when some
     * error happening means there is no device-lock daemon
     * running on the device...
     */
    success = message.arguments().at(0).toBool ();
    SYS_DEBUG ("*** reply = %s", SYS_BOOL (success));
#endif

// TODO: FIXME: It seems we're getting 'true'
// here when the lock code does not exists!!!
//  Bug filed: NB#270754

    /*
     * No password needed [ie. device-lock is disabled]
     */
    if (success == false)
        emit gotAccess ();
}

void
ResetBusinessLogic::passwordResult (bool result)
{
    SYS_DEBUG ("*** result = %s", SYS_BOOL (result));
    if (result == true)
        emit gotAccess ();
}

bool
ResetBusinessLogic::isUsbConnected ()
{
    bool retVal = false;

#ifdef HAVE_QMSYSTEM
    MeeGo::QmUSBMode usbMode;
    retVal = usbMode.getMode () == MeeGo::QmUSBMode::MassStorage;
#endif

    return retVal;
}

