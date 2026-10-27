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
#include "percentagecontainer.h"

#include <QGraphicsLinearLayout>
#include <MImageWidget>
#include <MLabel>
#include <MSeparator>
#include <MStylableWidget>
#include <MLocale>

#define DEBUG
#define WARNING
#include "../debug.h"

PercentageContainer::PercentageContainer(
        const QString  &text,
        MImageWidget   *image,
        MWidget        *parent) :
    MWidgetController (parent),
    m_MainLayout (0),
    m_Image (image),
    m_TextLabel (0),
    m_SubTextLabel (0)
{
    SYS_DEBUG ("*** text = %s", SYS_STR(text));

    setStyleName ("CommonTextFrameInverted");
    setObjectName ("PercentageContainer");

    m_TextLabel = new MLabel (text);
    m_TextLabel->setStyleName ("CommonSingleTitleInverted");
    m_TextLabel->setWordWrap (true);

    setMainLayout ();
}


void
PercentageContainer::setText (
        const QString &text)
{
    m_TextLabel->setText (text);
}

void 
PercentageContainer::updateCapacity (
        const int value)
{
    QString text;
    MLocale locale;

    float percentage = ((float) value) / 100.0f;

    //% "Battery level \%L1\%"
    text = qtTrId ("qtn_ener_battery_level").arg (locale.formatPercent (percentage));
    SYS_DEBUG ("*** text = %s", SYS_STR(text));
    
    m_TextLabel->setText(text);
}

void 
PercentageContainer::updateRemainingChargingTime (
        int    ChTime)
{
    toggleSubLabelVisibility (ChTime > 0);

    if (ChTime > 0) {
        int minutes = ChTime / 60;
        m_SubTextLabel->setText (
            qtTrId ("qtn_ener_charging_estimate",
                    minutes).arg (minutes));
    }
}

/*
 * This method will create all the internal widgets. 
 */
void 
PercentageContainer::setMainLayout()
{
    QGraphicsLinearLayout *layout;

    m_MainLayout = new QGraphicsLinearLayout (Qt::Vertical);
    m_MainLayout->setContentsMargins (0., 0., 0., 0.);
    m_MainLayout->setSpacing (0.);

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0., 0., 0., 0.);
    layout->setSpacing (0.);

    // add the widgets
    layout->addItem (m_Image);
    layout->setAlignment (m_Image, Qt::AlignLeft | Qt::AlignVCenter);
    layout->addItem (m_TextLabel);
    layout->setAlignment (m_TextLabel, Qt::AlignLeft | Qt::AlignVCenter);

    m_MainLayout->addItem (layout);

    // set the layout
    setLayout (m_MainLayout);
}

void
PercentageContainer::toggleSubLabelVisibility (bool visible)
{
    if (!visible && m_SubTextLabel) {
        m_SubTextLabel->deleteLater();
        m_SubTextLabel = 0;
        m_SubTextSeparator->deleteLater();
        m_SubTextSeparator = 0;
        return;
    } 
    
    if (visible && !m_SubTextLabel) {
        m_SubTextLabel = new MLabel;
        m_SubTextLabel->setWordWrap (true);
        m_SubTextLabel->setObjectName ("SubTextLabel");
        m_SubTextLabel->setStyleName ("CommonSubTitleInverted");
        m_SubTextSeparator = new MSeparator;
        // Using this one instead of "CommonSpacer", margins look even.
        m_SubTextSeparator->setStyleName ("CommonLargeSpacer");

        m_MainLayout->addItem (m_SubTextLabel);
        m_MainLayout->addItem (m_SubTextSeparator);
    }
}

void
PercentageContainer::updateRemainingTime (
        int     remainingTalk,
        int     remainingIdle,
        int     batteryPercent)
{
    QString  labelText;

    toggleSubLabelVisibility (
            remainingTalk > 0 || 
            remainingIdle > 0 || 
            batteryPercent > -1);

    if (batteryPercent > -1) {
        //% "Battery level %1"
        MLocale locale;
        float percentage = ((float) batteryPercent) / 100.0f;
        labelText = qtTrId ("qtn_ener_battery_level").arg (
                        locale.formatPercent (percentage));
    }

    if (remainingTalk > 0) {
        if (!labelText.isEmpty())
            labelText += "<br>";

        //% "Remaining talk time: %1"
        labelText += qtTrId ("qtn_ener_remaining_talk").arg (
                formatTime(remainingTalk));
    }

    if (remainingIdle > 0) {
        if (!labelText.isEmpty())
            labelText += "<br>";

        //% "Remaining standby time: %1"
        labelText += 
            qtTrId ("qtn_ener_remaining_standby").arg(
                    formatTime(remainingIdle));
    }

    if (m_SubTextLabel)
        m_SubTextLabel->setText (labelText);
}

QString
PercentageContainer::formatTime (int remTime)
{
    QString retval;
    SYS_DEBUG ("time = %d sec", remTime);
    /*
     * Only hours and minutes are shown if less than 24 hours remains.
     * Only minutes are shown if less than one hour remains.
     */
    int value = remTime;

    if (value > 60*60*24)
    {
        value /= 60*60*24;
        //% "%1 day <%1 days>"
        retval = qtTrId ("qtn_comm_time_day", value).arg (value);
    }
    else if (value > 60*60)
    {
        int minutes = (value / 60) % 60;

        /* hours */
        value /= 60*60;
        //% "%1 hour <%1 hours>"
        retval = qtTrId ("qtn_comm_time_hour", value).arg (value);
        if (minutes > 0)
        {
            retval += " ";
            //% "%1 minute <%1 minutes>"
            retval += qtTrId ("qtn_comm_time_minute", minutes).arg (minutes);
        }
    }
    else
    {
        value /= 60;
        /* We don't want to show 0 minutes left */
        if (value < 1)
            value = 1;
        //% "%1 minute <%1 minutes>"
        retval = qtTrId ("qtn_comm_time_minute", value).arg (value);
    }

    SYS_DEBUG ("timeStr = %s", SYS_STR (retval));

    return retval;
}

