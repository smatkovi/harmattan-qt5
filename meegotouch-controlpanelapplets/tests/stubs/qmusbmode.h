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
#ifndef QMUSBMODE_H
#define QMUSBMODE_H
#define QMUSBMODE_STUB_H
#ifndef UNIT_TEST
#  error "This header should only used in unit tests."
#endif

#include <QObject>

namespace MeeGo
{
    class QmUSBMode : public QObject
    {
        Q_OBJECT;
    public:

        enum Mode {
            Connected = 0,
            DataInUse,
            Disconnected,
            MassStorage,
            ChargingOnly,
            OviSuite,
            ModeRequest,
            Ask,
            Undefined,
            SDK
        };

        QmUSBMode (QObject *parent = 0);
        ~QmUSBMode ();

        Mode getMode ();
        Mode getDefaultMode ();
        bool setMode (Mode mode);
        bool setDefaultMode (Mode mode);

    signals:
        void modeChanged (MeeGo::QmUSBMode::Mode);

    private:
        Mode   m_Mode;
        Mode   m_DefaultMode;
    };
}
#endif
