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
#ifndef USBVIEW_H
#define USBVIEW_H

#include <DcpStylableWidget>

#ifdef HAVE_QMSYSTEM
#  include <qmusbmode.h>
#endif

#include <QList>

class MLabel;
class MButton;
class MLinearLayoutPolicy;

class UsbView : public DcpStylableWidget
{
Q_OBJECT

public:
#ifdef HAVE_QMSYSTEM
    UsbView (MeeGo::QmUSBMode *logic);
#else
    UsbView (void *logic);
#endif
    ~UsbView ();

private slots:
    void usbModeActivated (int idx);
    void updateInfoLabel ();
    void buttonToggled (bool state);

private:
    typedef enum {
        UsbModeAsk           = 0,
        UsbModeMassStorage   = 1,
        UsbModeOviSuite      = 2,
        UsbModeSDK           = 3,
        UsbModeLastMode      = 4,
    } UsbModeType;

    void initWidget ();
    QString usbModeUIString (UsbModeType type) const;
    QString usbModeButtonStyle (UsbModeType type) const;
    int selectedButtonIndex () const;
    void setSelectedButtonIndex (int index);

    /*
     * Private methods that add widgets.
     */
    MLabel *addTitleLabel (
            QGraphicsWidget     *parent,
            MLinearLayoutPolicy *targetPolicy,
            const char          *labelStyleName);

    void addSubTitle (
            QGraphicsWidget     *parent,
            MLinearLayoutPolicy *targetPolicy,
            const QString       &subTitle);

    void addSubtitle ();
    void addButtons ();

    QString currentText () const;

#ifdef HAVE_QMSYSTEM
    MeeGo::QmUSBMode    *m_logic;
#endif
    MLinearLayoutPolicy *m_policy;
    MLabel              *m_infoLabel;
    int                  m_infoOrder;
    QList<MButton *>     m_Buttons;
    bool                 m_DeveloperMode;

friend class Ut_UsbApplet;
};

#endif

