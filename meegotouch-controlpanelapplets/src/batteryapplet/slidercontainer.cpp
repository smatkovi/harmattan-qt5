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

#include "slidercontainer.h"

#include <QGraphicsLinearLayout>
#include <MLabel>
#include <MSlider>
#include <MLocale>

#include "percentagecontainer.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

#include <QDebug>

SliderContainer::SliderContainer (
        MWidget *parent) :
    MWidgetController (parent),
    m_PSMSlider (0),
    m_SliderExists (false)
{
    SYS_DEBUG ("");
    createWidgets (parent);
}

SliderContainer::~SliderContainer ()
{
    SYS_DEBUG ("Destroying %p", this);
}

void 
SliderContainer::createWidgets (MWidget *parent)
{
    QGraphicsLinearLayout   *layout;

    SYS_DEBUG ("");

    /*
     * A container for the two labels.
     */
    setContentsMargins (0, 0, 0, 0);
    //setStyleName ("CommonPanelInverted");

    layout = new QGraphicsLinearLayout (Qt::Vertical);
    layout->setContentsMargins (0,0,0,0);
    layout->setSpacing (0);
    setLayout (layout);

    /*
     * "Auto activate power save" label
     */
    m_AutoPSMLabel = new MLabel (parent);
    // This is needed to select the short translation.
    m_AutoPSMLabel->setWordWrap (true);
    m_AutoPSMLabel->setStyleName ("CommonTitleInverted");
    layout->addItem (m_AutoPSMLabel);
    layout->setAlignment (m_AutoPSMLabel, Qt::AlignLeft);

    /*
     * A subtitle that shows the current value of the slider.
     */
    m_PsmValueLabel = new MLabel (parent);
    m_PsmValueLabel->setStyleName ("CommonSubTitleInverted");
    m_PsmValueLabel->setObjectName ("PsmValueLabel");
    layout->addItem (m_PsmValueLabel);
    layout->setAlignment (m_PsmValueLabel, Qt::AlignLeft);

    /*
     * Power save mode auto activation slider
     */
    m_PSMSlider = new MSlider (parent);
    m_PSMSlider->setObjectName ("PSMSlider");
    m_PSMSlider->setStyleName ("CommonSliderInverted");
    m_PSMSlider->setOrientation (Qt::Horizontal);
    m_PSMSlider->setHandleLabelVisible (true);
    m_PSMSlider->setRange (0, m_SliderValues.size () - 1);
    m_PSMSlider->setMaxLabelIconID ("icon-s-common-add-inverse");
    m_PSMSlider->setMinLabelIconID ("icon-s-common-remove-inverse");
    m_PSMSlider->setMinLabelVisible (true);
    m_PSMSlider->setMaxLabelVisible (true);

    layout->addItem (m_PSMSlider);
    layout->setAlignment (m_PSMSlider, Qt::AlignHCenter);
    
    /*
     * .. and after connect the slidervalue changed signal
     */
    connect (m_PSMSlider, SIGNAL (valueChanged (int)),
            this, SLOT (sliderValueChanged (int)),
            Qt::DirectConnection);

    m_AutoPSMLabel->setText (qtTrId("qtn_ener_activation"));
}

/*!
 * This is in fact not an initialization function, the backend calls it to set
 * the slider values.
 * Must be called before updateSlider!
 */
void
SliderContainer::initSlider (
        const QStringList &values)
{
    m_SliderValues = values;

    if (m_PSMSlider)
        m_PSMSlider->setRange (0, m_SliderValues.size () - 1);
}

/*!
 * This slot is called when the backend returns the PSM value so we have to set
 * the slider accordingly.
 */
void
SliderContainer::updateSlider (int value)
{
    SYS_DEBUG ("*** value = %d", value);

    // Store the actual value for later
    // (eg for the case when slider isn't ready yet...)
    value = m_SliderValues.indexOf (QString ("%1").arg (value));

    if (value >= 0) {
        m_PSMSlider->setValue (value);
    } else {
        SYS_WARNING ("ERROR: got an invalid PSM value: %d", value);
        foreach (QString str, m_SliderValues)
            SYS_DEBUG ("Available slider value: %s", SYS_STR (str));
    }

    updateSliderValueLabel (value);
}

/*!
 * This function is called when the user drags the slider and when the slider
 * value has been changed by the applet to show the value that came from the
 * backend.
 */
void
SliderContainer::sliderValueChanged (
        int value)
{
    SYS_DEBUG ("*** slider = %p", m_PSMSlider);
    SYS_DEBUG ("*** value  = %d", value);

    updateSliderValueLabel (value);

    emit PSMThresholdValueChanged (m_SliderValues.at (value).toInt ());
}

/*!
 * Updates the label that shows the auto PSM slider value or shows the 'off'
 * string when the auto power save mode is disabled.
 */
void
SliderContainer::updateSliderValueLabel (int index)
{
    /*
     * We want to show something, but this should never happen.
     */
    if (index < 0 || index >= m_SliderValues.size())
        index = 0;

    if (index >= 0 && index < m_SliderValues.size()) {
        MLocale locale;
        QString theString = locale.formatPercent(
                m_SliderValues[index].toDouble() / 100.0, 0);

        /*
         * This is what the UI spec uses...
         * m_PSMSlider->setHandleLabel(
         *        QString ("%L1%").arg(m_SliderValues[index]));
         */
        m_PsmValueLabel->setText (theString);
        m_PSMSlider->setHandleLabel(theString);
    }
}

