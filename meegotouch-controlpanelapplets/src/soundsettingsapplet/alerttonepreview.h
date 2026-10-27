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
#ifndef _ALERT_TONE_PREVIEW_H_
#define _ALERT_TONE_PREVIEW_H_

#include <gst/gst.h>
#include <QObject>
#include <QString>
#include "qprofilevalue.h"

class AlertTonePreview : public QObject
{
Q_OBJECT

public:
    AlertTonePreview (const QString &fname);
    ~AlertTonePreview ();
    QString fname () const;

    static void freeResources ();

private:
    GstElement     *m_gstPipeline;
    GstElement     *m_gstFilesrc;
    GstElement     *m_gstVolume;
    QProfileValue   m_profileVolume;
    QString         m_Filename;

    void gstInit ();
    void rewind ();
    double profileToGstVolume ();
    static void gstSignalHandler (GstBus *bus,
                                  GstMessage *msg,
                                  AlertTonePreview *atp);
    void getResources ();

private slots:
	void profileVolumeChanged();
    void audioResourceAcquired();
    void audioResourceLost();

#ifdef UNIT_TEST
    friend class Ut_AlertTonePreviewTests;
    friend class Ut_AlertToneBrowserTests;
#endif
};

#endif /* !_ALERT_TONE_PREVIEW_H_ */
