/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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

#ifndef _ALERT_TONE_H_
#define _ALERT_TONE_H_

#include <meegocontrolexport.h>
#include "qprofilevalue.h"

/*!
 * Class that provides information about an alert tone set to be used when a
 * specific event occurs (e.g. incoming call, new email, etc.). Tracks the
 * changes and emits signals when some change detected.
 *
 * The AlertTone is created with a specific key called the profile key that
 * identifies the event to which the AlertTone belongs. Here is an example on
 * the available keys:
\code
AlertTone  *alertTone;

alertTone = new AlertTone ("ringing.alert.tone");
\endcode
 *
 * Please check the documentation for the profile-data package or the profile
 * daemon files under /etc/profiled/
 */
class MC_EXPORT AlertTone: public QProfileValue
{
Q_OBJECT

public:
    AlertTone(const QString &key);

    static QList<AlertTone *> alertTones();

    QString fileName();
    QString niceName();
    QString trackerId();

protected:
    virtual void fetchFromBackend();
    virtual void realSetValue(const QVariant &newValue);

signals:
    /*! This signal is emitted when the value is not changed but a new detail
     * becomes available so the UI should be refreshed. Currently there is one
     * such a case, the asynchronous tracker connection that reports the song
     * title (nice name of the sound file) once the tracker answers to the
     * qutomatic query.
     */
    void refreshed();
    
private slots:
    void dataReceived (
            const QString   &filename, 
            const QString   &title,
            const QString   &trackerId);

private:
    void maybeUpdate();
    QString m_niceName;
    QString m_trackerId;

#ifdef UNIT_TEST
    friend class Ut_AlertToneTests;
#endif
};

#endif /* !_ALERT_TONE_H_ */
