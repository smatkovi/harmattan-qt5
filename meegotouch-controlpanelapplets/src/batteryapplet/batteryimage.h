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
#ifndef BATTERYIMAGE_H
#define BATTERYIMAGE_H

#include <QObject>
#include <MImageWidget>
#include <QMetaType>

class QTimer;
class QStringList;

/*!
 * An animated icon that shows the state of the battery. The icon has free
 * dictinct states:
 * (1) Using the battery.
 * (2) Charging the battery.
 * (3) Power save mode.
 * (4) Charging complete
 *
 * The state however is controlled by several attributes:
 * (1) Charging boolean
 * (2) PSM enabled boolean
 * and a slot:
 * (1) Charge complete slot
 *
 * And thus we have four combinations. here is a map that this class implements:
 * 
 * Charg.       PSM.       State
 *   n           n         (1) Using the battery.
 *   n           y         (3) Power save mode.
 *   y           n         (2) Charging the battery.
 *   y           y         (2) Charging the battery.
 *
 * if chargeComplete () slot called, than icon changes to charge complete.
 *
 * This is because indicating the charging is more important than indicating the
 * power save mode, although we could anymate the power save mode icon too.
 */
class BatteryImage : public MImageWidget
{
    Q_OBJECT

    typedef enum {
        ICON_NORMAL = 0,
        ICON_POWERSAVE,
        ICON_CHARGING,
        IconTypeUnset,
    } BatteryIconType;


public:
    BatteryImage (QGraphicsItem *parent = 0);
    ~BatteryImage ();
    bool charging () const;
    void setOnScreen (bool onscreen);

public slots:
    void updateBatteryLevel (int level);

    void startCharging (int rate);
    void stopCharging ();
    void setPSMValue (bool PSMEnabled);

private slots:
    void updateImage ();
    void chargeComplete ();

private:
    void  setIconSet (bool forceReload = false);

    void  stopTimer();
    bool  maybeStartTimer();
    const QPixmap *getPixmap (const QString &name);
    void loadImages (BatteryImage::BatteryIconType type);
    
private:
    bool                    m_PowerSave;
    bool                    m_ChargeComplete;
    int                     m_ChargingSpeed;
    QTimer                 *m_timer;
    int                     m_batteryLevel;
    QList<const QPixmap *>  m_Images;
    BatteryIconType         m_iconCurrentSet;
    int                     m_ImageIndex;
    bool                    m_Onscreen;
    #ifdef UNIT_TEST
    friend class Ut_BatteryImage;
    #endif
};

#endif
