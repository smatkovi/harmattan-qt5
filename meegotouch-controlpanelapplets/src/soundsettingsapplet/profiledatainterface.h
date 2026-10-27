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
#ifndef PROFILEDATAINTERFACE_H
#define PROFILEDATAINTERFACE_H

#include <QObject>

class ProfileBackend;

class ProfileDataInterface : public QObject
{
    Q_OBJECT

public:
    
    /*
     * The order is fixed according to the UI specification.
     */
    enum ProfileId {
        ProfileIdSilent   = 0,
        ProfileIdBeep,
        ProfileIdRinging,
        ProfileIdLoud,
        // none must be last to teel how many profiles there are available
        ProfileIdNone 
    };

    struct ProfileData {
        ProfileId                 profileId;
        QString                   profileName;
        bool                      vibrationEnabled;
        bool                      isActive;
        int                       volumeLevel;
    };

    ProfileDataInterface ();
    virtual ~ProfileDataInterface ();

    static QString mapId (int id);

signals:
    void currentProfile (int id);
    void vibrationValue (int id, bool value);

public:
    QString mapId2StatusIconId (int id);
    QString mapId2IconId (int id);
    QString getCurrentProfileIconId ();
    QString getCurrentProfileName ();
    int getCurrentProfile ();
    QList<ProfileData> getProfilesData ();
    void setVibration (int id, bool value);

public slots:
    void setProfile (int value);

private slots:
    void currentProfileNameChanged (const QString &prof);

private:
    static ProfileId mapId (const QString &id);
    static QString id2Name (const QString &id);

private:
    ProfileBackend  *m_backend;

friend class Ut_ProfileDataInterface;
};

#endif
