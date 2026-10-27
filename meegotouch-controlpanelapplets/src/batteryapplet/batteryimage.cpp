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

#include "batteryimage.h"

#include <QTimer>
#include <QStringList>
#include <MTheme>

#undef DEBUG
#include "../debug.h"

BatteryImage::BatteryImage (QGraphicsItem *parent) :
        MImageWidget (parent),
        m_PowerSave (false),
        m_ChargingSpeed (0),
        m_timer (NULL),
        m_batteryLevel (0),
        m_iconCurrentSet (IconTypeUnset),
        m_ImageIndex (-1),
        m_Onscreen (false)
{
    setZoomFactor (1.0);
    setIconSet ();
    setObjectName ("CommonMainIcon");
    connect (MTheme::instance (), SIGNAL (pixmapRequestsFinished ()),
             this, SLOT (updateImage ()));
}

void
BatteryImage::loadImages (
        BatteryImage::BatteryIconType type)
{
    if (!m_Images.isEmpty ()) {
        // Release the pixmaps
        foreach (const QPixmap *icon, m_Images)
            MTheme::instance ()->releasePixmap (icon);

        m_Images.clear ();
    }

    QString ID = "battery";
    switch (type) {
        case ICON_POWERSAVE:
            ID = "powersave";
            break;

        case ICON_CHARGING:
            ID = "charging";
            break;

        default:
            break;
    }

  /*
   * The second one was -low , but to be sync with the status-menu it should
   * be verylow too... (See NB#284459 for details)
   */
  if (charging ())
  {
      /*
       * In case of charging, do not scare the user with the 'verylow' icon
       * XXX: https://projects.maemo.org/bugzilla/show_bug.cgi?id=284459#c5
       */
      m_Images <<
          getPixmap (QString ("icon-m-energy-management-%1-low").arg (ID));
  }
  else
  {
      m_Images <<
          getPixmap (QString ("icon-m-energy-management-%1-verylow").arg (ID));
  }

  for (int i = 1; i <=8; i++)
  {
      m_Images <<
          getPixmap (QString ("icon-m-energy-management-%1").arg (ID) + QString::number (i));
  }

  m_iconCurrentSet = type;
}

BatteryImage::~BatteryImage ()
{
    delete m_timer;
    m_timer = NULL;

    if (!m_Images.isEmpty ()) {
        // Release the pixmaps
        foreach (const QPixmap *icon, m_Images)
            MTheme::instance ()->releasePixmap (icon);
        m_Images.clear ();
    }
}

/*!
 * Updates the battery energy level, stores the current level and updates the
 * image if it is necessary
 */
void
BatteryImage::updateBatteryLevel (
        int level)
{
    SYS_DEBUG ("level = %d", level);
    if (m_batteryLevel == level)
        return;
    m_batteryLevel = level;

    updateImage ();
}

void
BatteryImage::setIconSet (bool forceReload)
{
    BatteryImage::BatteryIconType type;

    type = ICON_NORMAL;

    if (m_PowerSave) {
        type = ICON_POWERSAVE;
    } else if (charging()) {
        type = ICON_CHARGING;
    }

    stopTimer ();
    /*
     * In case of charging begin/end we should
     * re load the icon set, as 'low' icon should
     * be used instead of 'verylow' in case of charging
     * and vice versa
     */
    if (forceReload || (m_iconCurrentSet != type)) {
        SYS_DEBUG ("*** new type = %d", type);
        loadImages (type);
    }
    maybeStartTimer ();
}

void
BatteryImage::updateImage ()
{
    if (m_ImageIndex == -1)
        m_ImageIndex = m_batteryLevel;

    //SYS_DEBUG ("*** m_batteryLevel = %d", m_batteryLevel);
    /*
     * When the battery is charing we animate when it is not we simply show the
     * battery level.
     */
    if (charging()) {
        m_ImageIndex++;
        // Cumulative charging indicator
        if (m_ImageIndex >= m_Images.size ())
            m_ImageIndex = m_batteryLevel;
    } else {
        m_ImageIndex = m_batteryLevel;
    }

    /*
     * A last check...
     */
    if (m_ImageIndex < 0 || m_ImageIndex >= m_Images.size()) {
        SYS_WARNING ("Wrong index %d, should be between 0 and %d",
                m_ImageIndex, m_Images.size());
        return;
    }

    //SYS_DEBUG ("*** m_ImageIndex = %d", m_ImageIndex);
    setPixmap (*(m_Images.at (m_ImageIndex)));

    // When the battery is almost loaded we can not animate, so I hacked
    // this here.
    if (charging() && 
            m_ImageIndex + 1 == m_Images.size () && 
            m_batteryLevel + 1 == m_Images.size ())
        m_ImageIndex = 6;
}

/*!
 * \returns true if the image in charging state
 */
bool 
BatteryImage::charging () const
{
    return m_ChargingSpeed > 0 /*&& !m_PowerSave*/;
}

void
BatteryImage::startCharging (
		int rate)
{
    SYS_DEBUG ("*** rate = %d", rate);
    m_ChargingSpeed = rate;
    setIconSet (true);
    maybeStartTimer ();
}

void
BatteryImage::stopCharging ()
{
    m_ChargingSpeed = 0;
    setIconSet (true);
}

void
BatteryImage::setPSMValue (
        bool    PSMEnabled)
{
    SYS_DEBUG ("*** PSMEnabled = %s", SYS_BOOL(PSMEnabled));
    m_PowerSave = PSMEnabled;
    setIconSet ();
    maybeStartTimer ();
}


void
BatteryImage::stopTimer ()
{
    if (m_timer && m_timer->isActive()) {
        SYS_DEBUG ("STOPPING TIMER");
        m_timer->stop();
    }
}

/*!
 * \return true if the animation is ongoing.
 *
 * Start the animation if the state of the image requires it. Updates the icon
 * once if not.
 */
bool
BatteryImage::maybeStartTimer ()
{
    if (!charging() || !m_Onscreen) {
        updateImage ();
        return false;
    }

    if (!m_timer) {
        SYS_DEBUG("Creating timer.");
        m_timer = new QTimer (this);
        connect (m_timer, SIGNAL(timeout()),
                this, SLOT(updateImage()));
    } else {
        stopTimer();
    }

    updateImage ();
    SYS_DEBUG ("STARTING TIMER");
    m_timer->start (m_ChargingSpeed);
    return true;
}

const QPixmap *
BatteryImage::getPixmap (
        const QString &name)
{
    const QPixmap  *pixmap;

    pixmap = MTheme::pixmap (name);
    return pixmap;
}

void
BatteryImage::chargeComplete ()
{
    SYS_DEBUG("battery full iconn");
    setPixmap( *(getPixmap(QString ("icon-m-energy-management-remove-charger"))) );
}

void
BatteryImage::setOnScreen (
        bool onscreen)
{
    m_Onscreen = onscreen;

    if (m_Onscreen) {
        maybeStartTimer ();
    } else {
        stopTimer ();
    }
}
