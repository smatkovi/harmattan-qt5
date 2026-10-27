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
#include "usbapplet.h"

#include <MAction>
#include <DcpStylableWidget>

#include "usbview.h"

#undef DEBUG
#include "../debug.h"

Q_EXPORT_PLUGIN2(usbapplet, UsbApplet)

void
UsbApplet::init (void)
{
    m_logic = 0;
}

DcpStylableWidget *
UsbApplet::constructStylableWidget (int widgetId)
{
    Q_UNUSED (widgetId);

#ifdef HAVE_QMSYSTEM
    if (! m_logic)
    {
        m_logic = new QmUSBMode (this);
    }
#endif

    if (!m_MainWidget) {
#ifdef HAVE_QMSYSTEM
        m_MainWidget = new UsbView (m_logic);
#else
        m_MainWidget = new UsbView (0);
#endif
    }

    return m_MainWidget;
}

QString
UsbApplet::title (void) const
{
    //% "USB"
    return qtTrId ("qtn_usb_title");
}

QVector<MAction*>
UsbApplet::viewMenuItems ()
{
    QVector<MAction*> vector;

    return vector;
}

DcpBrief *
UsbApplet::constructBrief (int partId)
{
    Q_UNUSED (partId);

    return 0;
}

