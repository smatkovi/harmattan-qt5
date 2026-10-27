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
#ifndef USBAPPLET_H
#define USBAPPLET_H

#include <QObject>
#include <QPointer>
#include <DcpAppletIf>

#ifdef HAVE_QMSYSTEM
#  include <qmusbmode.h>
using namespace MeeGo;
#endif

#include "usbview.h"

class MAction;
class DcpStylableWidget;
class UsbBrief;

class UsbApplet : public QObject, public DcpAppletIf 
{
	Q_OBJECT
	Q_INTERFACES (DcpAppletIf)

public:
    virtual void        init ();
	virtual DcpStylableWidget  *constructStylableWidget (int widgetId);
    virtual QString     title () const;
    virtual DcpBrief   *constructBrief (int partId);
    QVector<MAction *>  viewMenuItems ();

private:
    QPointer<UsbView>   m_MainWidget;
    UsbBrief           *m_brief;
    #ifdef HAVE_QMSYSTEM
    MeeGo::QmUSBMode   *m_logic;
    #endif

    #ifdef UNIT_TEST
    friend class Ut_UsbApplet;
    #endif
};

#endif

