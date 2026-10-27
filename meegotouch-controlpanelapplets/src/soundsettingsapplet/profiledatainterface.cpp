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
#include "profiledatainterface.h"
#include "profilebackend.h"

namespace ProfileName {
  const QString ringing = "general";  /*!< Profile name 'ringing' */
  const QString silent = "silent";    /*!< Profile name 'silent' */
  const QString beep = "meeting";     /*!< Profile name 'beep' */
}

#include <QStringList>
#include <QList>

//#define DEBUG
#include "../debug.h"


/******************************************************************************
 * ProfileData support methods implementation.
 */
bool ProfileDataLessThan (
        const ProfileDataInterface::ProfileData &p1,
        const ProfileDataInterface::ProfileData &p2)
{
    // Use this to sort the profiles in volume level order
    // return p1.volumeLevel < p2.volumeLevel;
    // Modified, see NB#240711
    return p1.profileId > p2.profileId;
}

/******************************************************************************
 * ProfileDataInterface class implementation.
 */
ProfileDataInterface::ProfileDataInterface ()
{
    m_backend = ProfileBackend::getInstance ();

    connect (m_backend, SIGNAL (activeProfileChanged (QString)),
             SLOT (currentProfileNameChanged (QString)));
}

ProfileDataInterface::~ProfileDataInterface ()
{
}


/*!
 * returns the status-menu icon ID representing the current profile.
 */
QString
ProfileDataInterface::mapId2StatusIconId (int id)
{
    QString iconId;

    switch (id) {
        case ProfileIdRinging:
            iconId = QString ("icon-m-status-menu-normal");
            break;

        case ProfileIdSilent:
            iconId = QString ("icon-m-status-menu-profile-silent");
            break;

        case ProfileIdBeep:
            iconId = QString ("icon-m-status-menu-profile-beep");
            break;
    }

    return iconId;
}

void
ProfileDataInterface::currentProfileNameChanged (
        const QString &prof)
{
    SYS_DEBUG ("prof = %s", SYS_STR (prof));
    emit currentProfile(mapId(prof));
}

int
ProfileDataInterface::getCurrentProfile ()
{
    SYS_DEBUG ("");

    QString prof = m_backend->getActiveProfile ();
    return mapId (prof);
}

QList<ProfileDataInterface::ProfileData>
ProfileDataInterface::getProfilesData ()
{
    SYS_DEBUG ("");
    QList<ProfileData> data;
    QStringList ids = m_backend->getProfiles ();

    // send...
    for (int i = 0; i < ids.count(); ++i) {
        ProfileDataInterface::ProfileData d;
        //get name...
        QString id = ids.at(i);

        SYS_DEBUG ("profile-id : '%s'", SYS_STR (id));
        d.profileId = mapId (id);
        if (d.profileId == ProfileIdNone)
            continue;

        d.profileName       = id2Name (id);
        d.vibrationEnabled  = m_backend->getVibration (id);
        d.volumeLevel       = m_backend->getVolumeLevel (id);
        data.append(d);
    }

    /*
     * We return the profiles in a specific order. Please check NB#188710 for
     * further details.
     */
    qSort (data.begin(), data.end(), ProfileDataLessThan);
    return data;
}

void
ProfileDataInterface::setProfile (
        int value)
{
    bool success;

    SYS_DEBUG ("value = %d", value);

    success = m_backend->setActiveProfile (mapId(value));
    if (!success) {
        SYS_WARNING ("Failed setting profile.");
        return;
    }

    emit currentProfile (value);
}

void
ProfileDataInterface::setVibration (
        int    id,
        bool   value)
{
    bool success;

    SYS_DEBUG ("id = %d, value = %s", id, SYS_BOOL (value));

    success = m_backend->setVibration(mapId(id), value);
    if (!success) {
        SYS_WARNING ("Failed setting the vibration.");
    }
}

/*!
 * returns The icon ID representing the given profile.
 */
QString
ProfileDataInterface::mapId2IconId (int id)
{
    QString iconId = "";

    switch (id) {
        case ProfileIdRinging:
            iconId = QString("icon-m-profile-normal");
            break;
        case ProfileIdSilent:
            iconId = QString("icon-m-profile-silent");
            break;
        case ProfileIdBeep:
            iconId = QString("icon-m-profile-beep");
            break;
    }

    return iconId;
}

QString
ProfileDataInterface::getCurrentProfileName ()
{
    QString prof = m_backend->getActiveProfile ();

    return id2Name (prof);
}

QString
ProfileDataInterface::id2Name (
        const QString &id)
{
    QString localised = "";
    if (ProfileName::ringing == id) {
        //% "Ringing"
        localised = qtTrId ("qtn_prof_ringing");
    } else if (ProfileName::silent == id) {
        //% "Silent"
        localised = qtTrId ("qtn_prof_silent");
    } else if (ProfileName::beep == id) {
        //% "Beep"
        localised = qtTrId ("qtn_prof_beep");
    }

    SYS_DEBUG ("returning '%s'", SYS_STR (localised));
    return localised;
}

ProfileDataInterface::ProfileId
ProfileDataInterface::mapId (
        const QString &id)
{
    ProfileId intId = ProfileIdNone;

    if (ProfileName::ringing == id) {
        intId = ProfileIdRinging;
    } else if (ProfileName::silent == id) {
        intId = ProfileIdSilent;
    } else if (ProfileName::beep == id) {
        intId = ProfileIdBeep;
    }

    return intId;
}

QString
ProfileDataInterface::mapId (
        int id)
{
    QString stringId = "";

    switch (id) {
        case ProfileIdRinging:
            stringId = ProfileName::ringing;
            break;
        case ProfileIdSilent:
            stringId = ProfileName::silent;
            break;
        case ProfileIdBeep:
            stringId = ProfileName::beep;
            break;
    }

    return stringId;
}

