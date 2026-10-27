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
#ifndef PROFILE_BACKEND_H
#define PROFILE_BACKEND_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>

class ProfileBackend : public QObject
{
    Q_OBJECT

public:
    static ProfileBackend *getInstance ();
    virtual ~ProfileBackend ();

    bool setActiveProfile (QString profileName);
    QString getActiveProfile () const;

    QStringList getProfiles ();

    bool setVolumeLevel (QString profileName, int level);
    int getVolumeLevel (QString profileName);

    bool setVibration (QString profileName, bool vibration);
    bool getVibration (QString profileName);

protected:
    ProfileBackend (QObject *parent = 0);

signals:
    void activeProfileChanged (QString currentProfile);
    void vibrationSettingChanged (QString profile, bool vibration);
    void volumeSettingChanged (QString profile, int volumeLevel);

/* functions which are available from the static methods */
protected:
    void changeActiveProfile (const char *profile);
    void changeProfileValue (const char *profile,
                             const char *key,
                             const char *value);

private:
    static void currentProfileChanged (const char     *profile,
                                      ProfileBackend *self);
    static void profileValueChanged (const char     *profile,
                                     const char     *key,
                                     const char     *value,
                                     const char     *type,
                                     ProfileBackend *self);

    void initialize ();

private:
    bool                    m_initialized;
    QString                 m_activeProfile;
    QHash<QString, bool>    m_profileVibrations;
    QHash<QString, int>     m_profileVolumes;

    Q_DISABLE_COPY(ProfileBackend)

    friend class Ut_ProfileBackend;
};

#endif

