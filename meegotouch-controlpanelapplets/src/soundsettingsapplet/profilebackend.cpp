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

#ifdef HAVE_LIBPROFILE
#include <libprofile.h>
#endif

#undef DEBUG
#define WARNING
#include "../debug.h"

const char *keyVolume    = "ringing.alert.volume";
const char *keyVibration = "vibrating.alert.enabled";

static ProfileBackend *backend = 0;

inline const char* toCharArray (QString &str)
{
    return str.toAscii().constData();
}

ProfileBackend::ProfileBackend (QObject *parent) :
    QObject (parent),
    m_initialized (false)
{
    initialize ();
}

ProfileBackend::~ProfileBackend ()
{
#ifdef HAVE_LIBPROFILE
    profile_tracker_quit ();
#endif
    backend = 0;
}

ProfileBackend *
ProfileBackend::getInstance ()
{
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

#ifdef HAVE_LIBPROFILE
    // get the current profile name
    m_activeProfile = profile_get_profile ();

    char **profiles = NULL;

    // get the list of available profiles
    profiles = profile_get_profiles ();

    if (profiles != NULL)
    {
        for (int i = 0; profiles[i] != NULL; i++)
        {
            char *profile = profiles[i];

            int  volumeLevel = profile_get_value_as_int (profile, keyVolume);
            bool vibration = profile_get_value_as_bool (profile, keyVibration);

            SYS_DEBUG ("*** PROFILE : \"%s\" ***", profile);

            // fill the hash tables...
            m_profileVibrations[QString (profile)] = vibration;
            m_profileVolumes[QString (profile)]    = volumeLevel;
        }

        profile_free_profiles (profiles);
    }
    else
    {
        SYS_WARNING ("There are no available profiles on the system!");
    }

    // callback to track current profile changes...
    profile_track_add_profile_cb (
        (profile_track_profile_fn_data) &ProfileBackend::currentProfileChanged,
        this, NULL);

    // callback to track profile value changes...
    profile_track_add_change_cb (
        (profile_track_value_fn_data) &ProfileBackend::profileValueChanged,
        this, NULL);

    // callback to track current profile value changes...
    // XXX: is this call really necessary? FIXME
    profile_track_add_active_cb (
        (profile_track_value_fn_data) &ProfileBackend::profileValueChanged,
        this, NULL);

    // start the tracking of changes...
    profile_connection_enable_autoconnect ();
    profile_tracker_init ();
#endif // HAVE_LIBPROFILE
}

bool
ProfileBackend::setActiveProfile (QString profileName)
{
    bool success = false;

#ifdef HAVE_LIBPROFILE
    success = profile_set_profile (toCharArray (profileName)) == 0;
#endif

    if (success)
        m_activeProfile = profileName;

    return success;
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
    bool success =
#ifdef HAVE_LIBPROFILE
      profile_set_value_as_int (toCharArray (profileName), keyVolume, level) == 0;
#else
    false;
#endif

    if (success)
        m_profileVolumes[profileName] = level;

    return success;
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
    bool success =
#ifdef HAVE_LIBPROFILE
      profile_set_value_as_bool (toCharArray (profileName),
                                 keyVibration, vibration) == 0;
#else
    false;
#endif

    if (success)
        m_profileVibrations[profileName] = vibration;

    return success;
}

bool
ProfileBackend::getVibration (QString profileName)
{
    // we're returning false here when profile is not exists...
    // XXX: better error handling??
    return m_profileVibrations.value (profileName, false);
}

// profiled callback function
void
ProfileBackend::currentProfileChanged (
    const char      *profile,
    ProfileBackend  *self)
{
    self->changeActiveProfile (profile);
}

// profiled callback function
void
ProfileBackend::profileValueChanged (
    const char      *profile,
    const char      *key,
    const char      *value,
    const char      *type,
    ProfileBackend  *self)
{
    Q_UNUSED (type);

    // we ignore everything except the vibra and volume-level
    bool ignore = true;

    if (qstrcmp (keyVolume, key) == 0)
        ignore = false;
    else if (qstrcmp (keyVibration, key) == 0)
        ignore = false;

    if (!ignore)
        self->changeProfileValue (profile, key, value);
}

void
ProfileBackend::changeActiveProfile (const char *profile)
{
    SYS_DEBUG ("profile = %s", profile);
    m_activeProfile = profile;

    emit activeProfileChanged (m_activeProfile);
}

void
ProfileBackend::changeProfileValue (
    const char      *profile,
    const char      *key,
    const char      *value)
{
#ifdef HAVE_LIBPROFILE
    QString profileName = profile;

    if (m_profileVolumes.value (profileName, -1) < 0)
    {
        SYS_WARNING ("Invalid profile: %s, ignoring...", profile);
        return;
    }

    if (qstrcmp (keyVolume, key) == 0)
    {
        int volume = profile_parse_int (value);

        m_profileVolumes[profileName] = volume;

        emit volumeSettingChanged (profileName, volume);
    }
    else if (qstrcmp (keyVibration, key) == 0)
    {
        bool vibration = profile_parse_bool (value);

        m_profileVibrations[profileName] = vibration;

        emit vibrationSettingChanged (profileName, vibration);
    }
    else
    {
        SYS_WARNING ("Error, invalid key: %s", key);
    }
#endif
}

