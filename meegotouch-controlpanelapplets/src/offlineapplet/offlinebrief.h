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

#ifndef OFFLINEBRIEF_H
#define OFFLINEBRIEF_H

#include <DcpBrief>

#ifdef HAVE_QMSYSTEM
#  include <qmdevicemode.h>
using namespace MeeGo;
#endif

class MBanner;

class OfflineBrief: public DcpBrief
{
Q_OBJECT

public:
    OfflineBrief ();
    ~OfflineBrief ();
    virtual QString titleText () const;
    virtual QString valueText () const;

    virtual bool toggle () const;
    virtual void setToggle (bool toggle);
    virtual int widgetTypeID () const;

private:
    QString currentText () const;

private slots:
    #ifdef HAVE_QMSYSTEM
    void devModeChanged (MeeGo::QmDeviceMode::DeviceMode mode);
    #endif
    void processDialogResult ();

private:
    #ifdef HAVE_QMSYSTEM
    MeeGo::QmDeviceMode::DeviceMode  m_LastMode;
    MeeGo::QmDeviceMode             *m_DevMode;
    #endif

    MBanner     *m_infoBanner;

#ifdef UNIT_TEST
    friend class Ut_OfflineApplet;
#endif

};


#endif
