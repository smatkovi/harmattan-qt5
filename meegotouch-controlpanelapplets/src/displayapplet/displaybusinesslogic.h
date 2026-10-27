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

#ifndef DISPLAYBUSINESSLOGIC_H
#define DISPLAYBUSINESSLOGIC_H

// QmSystem is mandatory
#include <qmdisplaystate.h>
#include <qmdevicemode.h>

#include <QObject>
#include <QDBusError>
#include <QDBusInterface>
#include <QStringList>

class QDBusObjectPath;
class QSettings;
class MGConfItem;

/*!
 * Implements the business logic that needed to handle the display settings.
 * This class is using the QmSystem library when it is available and uses GConf
 * database items when QmSystem is not found. The aim of using the QmSystem is
 * simply to hide the implementation details (that is the GConf keys) sdo that
 * we can create an independent implementation.
 *
 * Please note that not all the GConf keys are implemented yet.
 */
class DisplayBusinessLogic : public QObject
{
Q_OBJECT

public:
    DisplayBusinessLogic (QObject* parent = 0);
    virtual ~DisplayBusinessLogic ();

    QList<int> brightnessValues ();
    int selectedBrightnessValueIndex ();
    int selectedBrightnessValue ();
    QList<int> screenLightsValues ();
    int selectedScreenLightsValue ();
    const QStringList& colorProfileValues ();
    const QMap<QString, QString>& colorProfileMap ();
    int selectedColorProfileValue ();
    bool PSMValue ();
    bool getLowPowerMode ();
    bool getDoubleTapWakes ();
    bool getCloseFromTopValue ();

public slots:
    void setBrightnessValue (int value);
    void setScreenLightTimeouts (int index);
    void setColorProfile (int index);
    void setLowPowerMode (bool enable);
    void setDoubleTapWakes (bool enable);
    void setCloseFromTop (bool enable);
    void PSMStateChanged (MeeGo::QmDeviceMode::PSMState state);

private slots:
    void lpmValueChanged ();
    void doubleTapValueChanged ();
    void availableColorProfilesReceivedSlot (QStringList list);
    void currentColorProfileChanged ();
    void colorProfileIsSet (QDBusMessage);
    void DBusMessagingFailure (QDBusError error);

private:
    void setupMceDBusIf ();
    void initiateMceQueries ();

signals:
    void lowPowerModeChanged (bool lpm);
    void doubleTapModeChanged (bool lpm);
    void PSMValueReceived (bool enabled);
    void currentColorProfileReceived ();
    void availableColorProfilesReceived();

private:
    MeeGo::QmDisplayState   *m_Display;
    MeeGo::QmDeviceMode     *m_devicemode;

    MGConfItem              *m_possibleDimValues;
    MGConfItem              *m_lowPower;
    MGConfItem              *m_DoubleTap;
    QSettings               *m_compositorConf;
    bool                     m_psmValue;
    QDBusInterface          *m_MceDBusIf;

    MGConfItem              *m_CurrentColorProfileItem;
    QStringList              m_AvailColorProfiles;
    QMap<QString, QString>   m_ColorProfileTextIds;
    QString                  m_CurrentColorProfile;
    QString                  m_ColorProfileToSet;

#ifdef UNIT_TEST
    friend class Ut_DisplayBusinessLogic;
#endif
};

#endif
