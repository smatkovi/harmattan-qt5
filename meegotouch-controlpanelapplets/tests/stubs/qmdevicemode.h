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
#ifndef QMDEVICEMODE_H
#define QMDEVICEMODE_H
#define QMDEVICEMODE_STUBBED_H

#ifndef UNIT_TEST
#  error "This header should be used only in unit tests."
#endif
#ifdef FUNCTIONAL_TEST
#  error "This header should not be used in a functional test."
#endif

#include <QObject>

namespace MeeGo
{
    class QmDeviceMode : public QObject {
        Q_OBJECT
        Q_ENUMS(PSMState);

        public:
            QmDeviceMode (QObject *parent = 0);
            ~QmDeviceMode ();

            enum PSMState {
                PSMError = -1,    //!< Power save mode could not be figured out
                PSMStateOff = 0,  //!< Power save mode is off
                PSMStateOn        //!< Power save mode is on
            };

            PSMState getPSMState () const;
            bool setPSMState (PSMState state);
        
            int getPSMBatteryMode ();
            bool setPSMBatteryMode (int percentages);
            
        signals:
            void devicePSMStateChanged(MeeGo::QmDeviceMode::PSMState state);
            
        public: // helper functions that are not in the original
            QString PSMStateName (PSMState state) const;

        private:
            PSMState  m_PSMSTate;
            int       m_PSMBatteryMode;
    };
}
#endif
