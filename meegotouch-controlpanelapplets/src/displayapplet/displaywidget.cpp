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
#include "displaywidget.h"
#include "displaybusinesslogic.h"
#include "dcpdisplay.h"
#include "../styles.h"

#undef DEBUG
#define WARNING
#include "../debug.h"

/*
 * define this if you want to show the top-close switch
 */
#undef TOP_CLOSE_ENABLED

#include <QVariant>
#include <MButton>
#include <MWidgetController>
#include <MComboBox>
#include <MLabel>
#include <MLayout>
#include <MSlider>
#include <MLinearLayoutPolicy>
#include <QGraphicsLinearLayout>
#include <MSeparator>
#include <MHelpButton>
#include <MGConfItem>

DisplayWidget::DisplayWidget (QGraphicsWidget *parent) :
        DcpWidget (parent),
        m_logic (0),
        m_MainLayout (0),
        m_TitleLabel (0),
        m_SubTitleLabel (0),
        m_brightnessSlider (0),
        m_screenTimeout (0),
        m_screenlightLabel (0),
        m_lowPowerSwitch (0),
        m_DoubleTapSwitch (0),
        m_colorProfilesCBox (0)
{
    setReferer (DcpDisplay::None);
    setContentsMargins (0, 0, 0, 0);
    m_logic = new DisplayBusinessLogic;

    /*
     * Lets have these configurable via gconf keys
     */
    MGConfItem eggs(EGGS_GCONF_KEY);

    if (eggs.value().toBool()) {
        m_lowPowerChangeable = true; 
        m_topCloseChangeable = true; 
    } else {
        MGConfItem lps ("/meegotouch/settings/low_power_switchable");
        m_lowPowerChangeable = lps.value (true).toBool ();

        MGConfItem tcs ("/meegotouch/settings/top_swipe_switchable");
        m_topCloseChangeable = tcs.value (false).toBool ();
    }

    connect (m_logic, SIGNAL(lowPowerModeChanged(bool)),
            this, SLOT(lowPowerModeChanged(bool)));
    connect (m_logic, SIGNAL(doubleTapModeChanged(bool)),
            this, SLOT(doubleTapModeChanged(bool)));

    initWidget ();
}

DisplayWidget::~DisplayWidget ()
{
    delete m_logic;
    m_logic = 0;
}

void DisplayWidget::initWidget ()
{
    MLayout     *layout;

    /*
     * Creating a layout that holds the rows of the internal widgets.
     */
    layout = new MLayout (this);
    m_MainLayout = new MLinearLayoutPolicy (layout, Qt::Vertical);
    m_MainLayout->setContentsMargins (0., 0., 0., 0.);
    m_MainLayout->setSpacing (0.);
    setLayout (layout);

    /*
     * FIXME: I removed these widgets because the layout guide has been changed
     * (what an unexpected thing). If the layout guide remains we will have to
     * remove the methods themselves.
     */
    // Row 1: The title label
#ifndef MEEGO
    addHeaderContainer ();
#ifdef APP_TITLE_USES_SPACER
    addStretcher (APP_TITLE_DIVIDER_STYLE_NAME);
#endif
#endif

#if 0
    addSecHeaderContainer ();
    addSliderContainer ();
#endif

    addScreenTimeoutContainer ();

    if (m_lowPowerChangeable)
    {
        addLowPowerContainer ();
    }
    addDoubleTapContainer ();
    
    addColorProfilesCBox ();

#ifndef MEEGO
    if (m_topCloseChangeable)
    {
        addFromTopCloseContainer ();
    }
#endif

    m_MainLayout->addStretch();
}

void
DisplayWidget::addHeaderContainer ()
{
    QGraphicsLinearLayout *layout;

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0,0,0,0);

    /*
     * The label that we use as title.
     */
    //% "Display"
    m_TitleLabel = new MLabel (qtTrId ("qtn_disp_display"));
    m_TitleLabel->setObjectName ("TitleLabel");
    m_TitleLabel->setStyleName (APP_TITLE_LABEL_STYLE_NAME);
    layout->addItem (m_TitleLabel);
    layout->setAlignment (m_TitleLabel, Qt::AlignLeft);
    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (layout);
    m_MainLayout->setStretchFactor (layout, 0);
}

#if 0
void
DisplayWidget::addSecHeaderContainer ()
{
    MWidgetController            *container;
    QGraphicsLinearLayout *layout;

    /*
     * Creating a lcontainer and a layout.
     */
    container = new MWidgetController (this);
    container->setStyleName ("CommonHeaderPanelInverted");
    container->setContentsMargins (0,0,0,0);

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    container->setLayout (layout);
    layout->setContentsMargins (0,0,0,0);

    /*
     * The label that we use as title.
     */
    //% "Brightness"
    m_SubTitleLabel = new MLabel (qtTrId("qtn_disp_bright"));
    m_SubTitleLabel->setObjectName ("SubTitleLabel");
    m_SubTitleLabel->setStyleName ("CommonGroupHeaderInverted");
    layout->addItem (m_SubTitleLabel);
    layout->setAlignment (m_SubTitleLabel, Qt::AlignLeft);
    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (container);
    m_MainLayout->setStretchFactor (container, 0);
}

void
DisplayWidget::addSliderContainer ()
{
    MWidgetController            *container;
    QGraphicsLinearLayout *layout;

    /*
     * Creating a lcontainer and a layout.
     */
    container = new MWidgetController (this);
    container->setContentsMargins (0,0,0,0);
    container->setStyleName ("CommonLargePanelInverted");

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0,0,0,0);
    container->setLayout (layout);

    /*
     * The slider to set the brighness of the display.
     */
    m_brightnessSlider = new MSlider;
    m_brightnessSlider->setContentsMargins (0,0,0,0);
    m_brightnessSlider->setStyleName ("CommonSliderInverted");
    m_brightnessSlider->setObjectName ("BrightnessSlider");
    m_brightnessSlider->setMinLabelIconID ("icon-s-image-edit-low-brightness");
    m_brightnessSlider->setMinLabelVisible (true);
    m_brightnessSlider->setMaxLabelIconID ("icon-s-image-edit-high-brightness");
    m_brightnessSlider->setMaxLabelVisible (true);
    m_brightness_vals = m_logic->brightnessValues ();
    m_brightnessSlider->setRange (0, m_brightness_vals.size () - 1);
    m_brightnessSlider->setValue (m_logic->selectedBrightnessValueIndex ());

    connect (m_brightnessSlider, SIGNAL (valueChanged (int)),
             SLOT (sliderUpdated (int)));

    /*
     * Adding the slider to the panel. Please note that the theme might set the
     * size of the slider, so we need stretchers to move the short slider to the
     * center and make the panel as wide as possible.
     */
    layout->addStretch ();
    layout->addItem (m_brightnessSlider);
    layout->addStretch ();
    //layout->setAlignment (m_brightnessSlider, Qt::AlignHCenter);
    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (container);
    m_MainLayout->setStretchFactor (container, 0);
}
#endif

void
DisplayWidget::addScreenTimeoutContainer ()
{
    /*
     * Get the values from the business-logic
     */
    m_screenlight_vals = m_logic->screenLightsValues ();

    // Screen dim timeout selector
    m_screenTimeout = new MComboBox;

    //% "Backlight time out"
    m_screenTimeout->setTitle (qtTrId ("qtn_disp_screenoff"));
    m_screenTimeout->setStyleName ("CommonComboBoxInverted");
    m_screenTimeout->setLayoutPosition (M::VerticalCenterPosition);

    updateScreenTimeoutCombo ();

    connect (m_screenTimeout, SIGNAL (currentIndexChanged (int)),
             this, SLOT (screenTimeoutChanged (int)));

    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (m_screenTimeout);
    m_MainLayout->setStretchFactor (m_screenTimeout, 0);
}

/* 
 * Construct and init color profiles selector
 */
void
DisplayWidget::addColorProfilesCBox ()
{
    /*
     * Get the values from the business-logic
     */
    m_colorProfilesCBox = new MComboBox;

    //% "Color profile"
    m_colorProfilesCBox->setTitle (qtTrId ("qtn_disp_profile"));
    m_colorProfilesCBox->setStyleName ("CommonComboBoxInverted");
    m_colorProfilesCBox->setLayoutPosition (M::VerticalCenterPosition);

    updateColorProfilesCBox ();

    connect (m_logic, SIGNAL (availableColorProfilesReceived()),
             this, SLOT (updateColorProfilesCBox ()));
    connect (m_logic, SIGNAL (currentColorProfileReceived()),
             this, SLOT (currentColorProfileReceivedSlot ()));
    connect (m_colorProfilesCBox, SIGNAL (currentIndexChanged (int)),
             this, SLOT (colorProfilesCBoxCurrentIndexChanged (int)));

    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (m_colorProfilesCBox);
    m_MainLayout->setStretchFactor (m_colorProfilesCBox, 0);
}

void
DisplayWidget::addLowPowerContainer ()
{
    MWidgetController     *container;
    QGraphicsLinearLayout *layout;

    /*
     * Creating a lcontainer and a layout.
     */
    container = new MWidgetController (this);
    container->setContentsMargins (0,0,0,0);
    container->setStyleName ("CommonLargePanelInverted");

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0,0,0,0);
    layout->setSpacing (0);
    container->setLayout (layout);

    MLabel *lowPowerLabel = new MLabel;
    lowPowerLabel->setWordWrap (true);
    lowPowerLabel->setObjectName ("LowPowerLabel");
    lowPowerLabel->setStyleName ("CommonSingleTitleInverted");
    //% "Low power mode"
    lowPowerLabel->setText (qtTrId ("qtn_disp_lowpower"));
    layout->addItem (lowPowerLabel);
    layout->setAlignment (lowPowerLabel, Qt::AlignVCenter);

    MHelpButton *helpButton =
        new MHelpButton ("CTX_IDUG_MEEGO_SCREENSAVER.html");
    helpButton->setViewType (MButton::iconType);
    helpButton->setIconID ("icon-s-description-inverse");
    helpButton->setStyleName ("CommonMainIcon");
    layout->addItem (helpButton);
    layout->setAlignment (helpButton, Qt::AlignVCenter | Qt::AlignRight);

    m_lowPowerSwitch = new MButton;
    m_lowPowerSwitch->setCheckable (true);
    m_lowPowerSwitch->setViewType (MButton::switchType);
    m_lowPowerSwitch->setStyleName ("CommonRightSwitchInverted");
    m_lowPowerSwitch->setChecked (m_logic->getLowPowerMode ());

    connect (m_lowPowerSwitch, SIGNAL (toggled (bool)),
             m_logic, SLOT (setLowPowerMode (bool)));

    layout->addItem (m_lowPowerSwitch);
    layout->setAlignment (m_lowPowerSwitch, Qt::AlignVCenter | Qt::AlignRight);

    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (container);
    m_MainLayout->setStretchFactor (container, 0);
}

void
DisplayWidget::addDoubleTapContainer ()
{
    MWidgetController            *container;
    QGraphicsLinearLayout *layout;

    /*
     * Creating a lcontainer and a layout.
     */
    container = new MWidgetController (this);
    container->setContentsMargins (0,0,0,0);
    container->setStyleName ("CommonLargePanelInverted");

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0,0,0,0);
    layout->setSpacing (0);
    container->setLayout (layout);

    MLabel *lowPowerLabel = new MLabel;
    lowPowerLabel->setWordWrap (true);
    lowPowerLabel->setObjectName ("LowPowerLabel");
    lowPowerLabel->setStyleName ("CommonSingleTitleInverted");
    //% "Double tap wake-up"
    lowPowerLabel->setText (qtTrId ("qtn_disp_doubletap"));
    layout->addItem (lowPowerLabel);
    // See bug NB#289944
    layout->setAlignment (lowPowerLabel, 
            Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter);
    lowPowerLabel->setAlignment (
            Qt::AlignAbsolute | Qt::AlignLeft | Qt::AlignVCenter);

    m_DoubleTapSwitch = new MButton;
    m_DoubleTapSwitch->setCheckable (true);
    m_DoubleTapSwitch->setViewType (MButton::switchType);
    m_DoubleTapSwitch->setStyleName ("CommonRightSwitchInverted");
    m_DoubleTapSwitch->setChecked (m_logic->getDoubleTapWakes());

    connect (m_DoubleTapSwitch, SIGNAL (toggled (bool)),
             m_logic, SLOT (setDoubleTapWakes(bool)));

    layout->addItem (m_DoubleTapSwitch);
    layout->setAlignment (m_DoubleTapSwitch, Qt::AlignVCenter | Qt::AlignRight);

    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (container);
    m_MainLayout->setStretchFactor (container, 0);
}

void
DisplayWidget::addFromTopCloseContainer ()
{
#ifndef MEEGO
    MWidgetController            *container;
    QGraphicsLinearLayout *layout;

    /*
     * Creating a lcontainer and a layout.
     */
    container = new MWidgetController (this);
    container->setContentsMargins (0,0,0,0);
    container->setStyleName ("CommonLargePanelInverted");

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0,0,0,0);
    layout->setSpacing (0);
    container->setLayout (layout);

    MLabel *descLabel = new MLabel;
    descLabel->setWordWrap (true);
    descLabel->setObjectName ("CloseFromTopLabel");
    descLabel->setStyleName ("CommonSingleTitleInverted");
    //% "Swipe from top to close"
    descLabel->setText (qtTrId ("qtn_comm_swipe_to_close"));
    layout->addItem (descLabel);
    layout->setAlignment (descLabel, Qt::AlignVCenter);

    m_CloseSwitch = new MButton;
    m_CloseSwitch->setCheckable (true);
    m_CloseSwitch->setViewType (MButton::switchType);
    m_CloseSwitch->setStyleName ("CommonRightSwitchInverted");
    m_CloseSwitch->setChecked (m_logic->getCloseFromTopValue ());

    connect (m_CloseSwitch, SIGNAL (toggled (bool)),
             m_logic, SLOT (setCloseFromTop (bool)));

    layout->addItem (m_CloseSwitch);
    layout->setAlignment (m_CloseSwitch, Qt::AlignVCenter | Qt::AlignRight);

    /*
     * Adding the whole row to the main container.
     */
    m_MainLayout->addItem (container);
    m_MainLayout->setStretchFactor (container, 0);
#endif
}

void
DisplayWidget::updateScreenTimeoutCombo ()
{
    bool fillNeeded = (m_screenTimeout->count () == 0);

    for (int i = 0; i < m_screenlight_vals.size (); i++)
    {
        bool minute = false;
        int value = m_screenlight_vals.at (i);

        if (value >= 60)
        {
            minute = true;
            value /= 60;
        }

        QString itemText;
        if (minute)
        {
            //% "%1 minutes"
            itemText = qtTrId ("qtn_comm_time_minute", value).arg (value);
        }
        else
        {
            //% "%1 seconds"
            itemText = qtTrId ("qtn_comm_time_second", value).arg (value);
        }

        if (fillNeeded)
            m_screenTimeout->insertItem (i, itemText);
        else
            m_screenTimeout->setItemText (i, itemText);
    }

    if (fillNeeded)
    {
        /*
         * set the currently selected index
         */
        m_screenTimeout->setProperty ("currentIndex",
                                      m_logic->selectedScreenLightsValue ());
    }
}

void
DisplayWidget::updateColorProfilesCBox ()
{
    SYS_DEBUG("update color profiles");
    bool fillNeeded = (m_colorProfilesCBox->count () == 0);
    
    const QStringList& colorProf = m_logic->colorProfileValues ();
    const QMap<QString, QString>& colorProfMap = m_logic->colorProfileMap ();

    for (int i = 0; i < colorProf.size (); i++)
    {
        QString textId = colorProfMap.value(colorProf.at (i));
        QString itemText = qtTrId (textId.toLatin1().constData());

        if (fillNeeded)
            m_colorProfilesCBox->insertItem (i, itemText);
        else
            m_colorProfilesCBox->setItemText (i, itemText);
    }

    /* set the currently selected index */
    if (fillNeeded) {
        int idx = m_logic->selectedColorProfileValue ();

        if (idx >= 0) 
            m_colorProfilesCBox->setProperty ("currentIndex", idx);
    }
}

void
DisplayWidget::currentColorProfileReceivedSlot ()
{
    m_colorProfilesCBox->setProperty (
            "currentIndex",
            m_logic->selectedColorProfileValue ());
}

void
DisplayWidget::addStretcher (
        const QString &styleName)
{
    MSeparator *stretcher;

    stretcher = new MSeparator ();
    stretcher->setStyleName (styleName);
    m_MainLayout->addItem (stretcher);
}

void
DisplayWidget::sliderUpdated (int val)
{
    m_logic->setBrightnessValue (val);
}

void
DisplayWidget::screenTimeoutChanged (int index)
{
    SYS_DEBUG ("selected = %d (%s)", index, SYS_STR (m_screenTimeout->currentText ()));
    SYS_DEBUG ("new value = %d", m_screenlight_vals.at (index));

    m_logic->setScreenLightTimeouts (index);
}

void
DisplayWidget::colorProfilesCBoxCurrentIndexChanged (int index)
{
    SYS_DEBUG ("selected = %d (%s)", index, SYS_STR (m_colorProfilesCBox->currentText ()));

    m_logic->setColorProfile (index);
}

void
DisplayWidget::retranslateUi ()
{
    if (m_TitleLabel)
        m_TitleLabel->setText (qtTrId("qtn_disp_display"));

    if (m_SubTitleLabel)
        m_SubTitleLabel->setText (qtTrId("qtn_disp_bright"));

    if (m_screenTimeout)
    {
        updateScreenTimeoutCombo ();
        m_screenTimeout->setTitle (qtTrId ("qtn_disp_screenoff"));
    }

    if (m_colorProfilesCBox)
    {
	updateColorProfilesCBox ();
        m_colorProfilesCBox->setTitle (qtTrId ("qtn_disp_profile"));
    }
}

void
DisplayWidget::lowPowerModeChanged (
        bool lpm)
{
    SYS_DEBUG ("*** lpm = %s", SYS_BOOL(lpm));
    if (m_lowPowerSwitch)
        m_lowPowerSwitch->setChecked (lpm);
}

void
DisplayWidget::doubleTapModeChanged (
        bool dt)
{
    SYS_DEBUG ("*** dt = %s", SYS_BOOL(dt));
    if (m_DoubleTapSwitch)
        m_DoubleTapSwitch->setChecked (dt);
}

