/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets
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
#ifndef _Q_PROFILE_VALUE_H_
#define _Q_PROFILE_VALUE_H_

#include <QPointer>
#include <QFileSystemWatcher>
#include <QString>
#include <meegocontrolexport.h>
#include "qtrackedvariant.h"

/*!
 * Base class to access (both read and write) for all the information stored in
 * the profile database to control audio and haptic feedback for specific events
 * (e.g. incoming call, incoming email, etc.).
 */
class MC_EXPORT QProfileValue : public QTrackedVariant
{
Q_OBJECT

public:
    QProfileValue (const QString &key, bool setAllProfiles = false);
    ~QProfileValue ();

    /*!
     * Enum for handling the different range-types
     */
    enum RangeType
    {
        Interval,
        List,
        Invalid
    };

    QList<QVariant> possibleValues (RangeType *p_rangeType = NULL);

protected:
    virtual void fetchFromBackend ();
    virtual void realSetValue (const QVariant &newValue);
    virtual bool stopWatchFiles ();
    virtual bool startWatchFile (const QString &filename);

private slots:
    void fileChanged (const QString &filename);

private:
    static int nTrackedValues;
    static void notifyValue (
            const char *profile, 
            const char *key, 
            const char *val, 
            const char *type, 
            QProfileValue *self);
    void addNotify ();
    void delNotify ();
    QStringList getType (QString &theKey, QString &theProfile);

private:
    bool                         m_setAllProfiles;
    QPointer<QFileSystemWatcher> m_FileWatcher;
    bool                         m_MissingFile;

#ifdef UNIT_TEST
    friend class Ut_ProfileValueTests;
#endif
};

#endif
