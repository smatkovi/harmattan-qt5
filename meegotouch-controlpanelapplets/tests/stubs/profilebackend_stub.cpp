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
#include "profilebackend.h"

#define DEBUG
#define WARNING
#include "../debug.h"

ProfileBackend::ProfileBackend (QObject *parent) :
    QObject (parent),
    m_initialized (false)
{
    initialize ();
}

ProfileBackend::~ProfileBackend ()
{
}

ProfileBackend *
ProfileBackend::getInstance ()
{
    static ProfileBackend *backend;

    if (! backend)
        backend = new ProfileBackend;

    return backend;
}

void
ProfileBackend::initialize ()
{
    // initialization should be done only once...
    if (m_initialized)
        return;

    m_initialized = true;

    // make some fake profiles...
    m_profileVibrations["general"]  = true;
    m_profileVibrations["meeting"]  = false;
    m_profileVibrations["outdoors"] = true;
    m_profileVibrations["silent"]   = false;

    m_profileVolumes["general"]     = 60;
    m_profileVolumes["meeting"]     = 20;
    m_profileVolumes["outdoors"]    = 100;
    m_profileVolumes["silent"]      = 0;

    m_activeProfile = "silent";
}

bool
ProfileBackend::setActiveProfile (QString profileName)
{
    m_activeProfile = profileName;

    return true;
}

QString
ProfileBackend::getActiveProfile () const
{
    return m_activeProfile;
}

QStringList
ProfileBackend::getProfiles ()
{
    return m_profileVolumes.keys ();
}

bool
ProfileBackend::setVolumeLevel (QString profileName, int level)
{
    m_profileVolumes[profileName] = level;

    return true;
}

int
ProfileBackend::getVolumeLevel (QString profileName)
{
    // this returns -1 if the wanted profile is not exists
    return m_profileVolumes.value (profileName, -1);
}

bool
ProfileBackend::setVibration (QString profileName, bool vibration)
{
    m_profileVibrations[profileName] = vibration;

    return true;
}

bool
ProfileBackend::getVibration (QString profileName)
{
    return m_profileVibrations.value (profileName, false);
}

void
ProfileBackend::currentProfileChanged (
    const char      *profile,
    ProfileBackend  *self)
{
    Q_UNUSED (profile);
    Q_UNUSED (self);
    /* no op */
}

void
ProfileBackend::profileValueChanged (
    const char      *profile,
    const char      *key,
    const char      *value,
    const char      *type,
    ProfileBackend  *self)
{
    Q_UNUSED (profile);
    Q_UNUSED (key);
    Q_UNUSED (value);
    Q_UNUSED (type);
    Q_UNUSED (self);

    /* no op */
}

void
ProfileBackend::changeActiveProfile (const char *profile)
{
    Q_UNUSED (profile);
    /* no op */
}

void
ProfileBackend::changeProfileValue (
    const char      *profile,
    const char      *key,
    const char      *value)
{
    Q_UNUSED (profile);
    Q_UNUSED (key);
    Q_UNUSED (value);
    /* no op */
}

