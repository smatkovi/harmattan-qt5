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
#include "usbview.h"

#include <MSeparator>
#include <MLabel>
#include <MLayout>
#include <QGraphicsLinearLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MButtonGroup>
#include <MBanner>
#include <MApplication>
#include <MImageWidget>
#include <MWidgetController>
#include <MGConfItem>

#include <QTimer>
#include <QStringList>

#include "../styles.h"

#undef DEBUG
#define WARNING
#include "../debug.h"

#ifdef HAVE_QMSYSTEM
using namespace MeeGo;

#define qtTrIdShort(id) qtTrId(id).split(QChar(0x9c)).last()

#ifndef MEEGO
#define VALID_USB_MODE(modeType) \
    ((modeType == UsbModeAsk || \
      modeType == UsbModeMassStorage || \
      modeType == UsbModeOviSuite || \
      modeType == UsbModeSDK))
#else
#define VALID_USB_MODE(modeType) \
    ((modeType == UsbModeAsk || \
      modeType == UsbModeMassStorage || \
      modeType == UsbModeOviSuite))
#endif

static const QString developerModeKey ("/Meego/System/DeveloperMode");
static const QString usbModeKey ("/MeeGo/System/UsbMode");
static const QString developerModeName ("windows_network");

static QmUSBMode::Mode usbModes[] = {
    QmUSBMode::Ask,
    QmUSBMode::MassStorage,
    QmUSBMode::OviSuite,
#ifndef MEEGO
    QmUSBMode::SDK,
#endif
};

inline int
usbModeIndex (QmUSBMode::Mode mode)
{
    int ret = 0;
    for (int i = 0; i < 4; i++)
        if (usbModes[i] == mode)
        {
            ret = i;
            break;
        }
    return ret;
}

#ifdef DEBUG
static QString
usbModeName (QmUSBMode::Mode mode)
{
    switch (mode) {
        case QmUSBMode::Connected:
            return "QmUSBMode::Connected";

        case QmUSBMode::DataInUse:
            return "QmUSBMode::DataInUse";

        case QmUSBMode::Disconnected:
            return "QmUSBMode::Disconnected";

        case QmUSBMode::MassStorage:
            return "QmUSBMode::MassStorage";

        case QmUSBMode::ChargingOnly:
            return "QmUSBMode::ChargingOnly";

        case QmUSBMode::OviSuite:
            return "QmUSBMode::OviSuite";

        case QmUSBMode::ModeRequest:
            return "QmUSBMode::ModeRequest";

        case QmUSBMode::Ask:
            return "QmUSBMode::Ask";
#ifndef MEEGO
        case QmUSBMode::SDK:
            return "QmUSBMode::SDK";
#endif
        case QmUSBMode::Undefined:
            return "QmUSBMode::Undefined";

        default:
            return "QmUSBMode::UnknownMode!";
    }
}
#endif
/******************************************************************************
 * UsbView implementation.
 */
UsbView::UsbView (MeeGo::QmUSBMode *logic) :
    DcpStylableWidget (0),
    m_logic (logic),
    m_policy (0),
    m_infoLabel (0),
    m_DeveloperMode (false)
{
    MGConfItem  developerModeItem (developerModeKey);

#ifndef MEEGO
    m_DeveloperMode = developerModeItem.value().toBool();
#endif
    setObjectName ("UsbView");
    initWidget ();
    connect (m_logic, SIGNAL (modeChanged (MeeGo::QmUSBMode::Mode)),
             SLOT (updateInfoLabel ()));

}
#else
/*
 * FIXME: to implement a variant that does not use the QmSystem library.
 */
UsbView::UsbView (void *logic) :
    DcpStylableWidget (0),
    m_policy (0),
    m_infoLabel (0),
    m_DeveloperMode (false)
{
    MGConfItem  developerModeItem (developerModeKey);
#ifndef MEEGO
    m_DeveloperMode = developerModeItem.value().toBool();
#endif
    setObjectName ("UsbView");
    initWidget ();
}
#endif

UsbView::~UsbView ()
{
}

void
UsbView::initWidget ()
{
    SYS_DEBUG ("");
    MLayout          *mainLayout;
    MSeparator       *separator;

    m_infoOrder = 0;
    setStyleName ("CommonPanelInverted");

    // Creating the main layout
    mainLayout = new MLayout;
    m_policy = new MLinearLayoutPolicy (mainLayout, Qt::Vertical);

    mainLayout->setContentsMargins (0,0,0,0);
    m_policy->setContentsMargins (0,0,0,0);
    m_policy->setSpacing (0);

#ifndef MEEGO
    // Create the title-bar
    MLabel *title = addTitleLabel (
            this, m_policy, APP_TITLE_LABEL_STYLE_NAME);
    //% "USB"
    title->setText (qtTrId ("qtn_usb_title"));
    m_infoOrder++;
#endif

    /*
     * A subtitle and then a separator. The separator right below the infotext
     * is going to be added by the infotext when it is necessary. The banner
     * below the subtitle is always there.
     */
    //% "Default USB device mode"
    addSubTitle (this, m_policy,
            qtTrId("qtn_usb_default_info"));

    separator = new MSeparator;
    separator->setStyleName ("CommonLargeSpacer");
    m_policy->addItem (separator);

    /*
     * The buttons to change the USB mode.
     */
    addButtons ();

    m_policy->addStretch ();
    setLayout (mainLayout);
    updateInfoLabel ();
}

void
UsbView::usbModeActivated (int idx)
{
    SYS_DEBUG ("idx = %d", idx);
#ifdef HAVE_QMSYSTEM
    /*
     * Do nothing if we just tapped on the
     * currently selected one...
     */
    if (m_logic->getDefaultMode() == usbModes[idx])
        return;

    QmUSBMode::Mode active = m_logic->getMode ();

    /*
     * Error message, shown if user tries to change the USB mode in USB
     * setting view (3.2) while the USB cable is connecting with the device.
     */
    if (active != QmUSBMode::Disconnected &&
            active != QmUSBMode::Undefined)
    {
        /*
         * Set checked on the previously active button
         */
        setSelectedButtonIndex (usbModeIndex(m_logic->getDefaultMode()));

        MBanner *infoBanner = new MBanner;
        infoBanner->setStyleName ("InformationBanner");
        infoBanner->setObjectName ("InfoBanner");

        //% "Cannot change the USB mode when it is connected"
        infoBanner->setTitle (qtTrId ("qtn_usb_change_incorrect"));

        infoBanner->appear (MApplication::instance ()->activeWindow (),
                            MSceneWindow::DestroyWhenDone);
        return;
    }

    QmUSBMode::Mode newmode = usbModes[idx];

    SYS_DEBUG ("Setting USB mode/default mode to %s",
            SYS_STR(usbModeName(newmode)));
    m_logic->setDefaultMode (newmode);
    m_logic->setMode (newmode);
#endif
}

MLabel *
UsbView::addTitleLabel (
        QGraphicsWidget     *parent,
        MLinearLayoutPolicy *targetPolicy,
        const char          *labelStyleName)
{
    Q_UNUSED (parent);
    MLabel                  *label;

    label = new MLabel;
    label->setStyleName (labelStyleName);
    label->setWordWrap (true);

    targetPolicy->addItem (label);
    return label;
}

void
UsbView::addSubTitle (
        QGraphicsWidget     *parent,
        MLinearLayoutPolicy *targetPolicy,
        const QString       &subTitle)
{
    MWidgetController       *container;
    QGraphicsLinearLayout   *layout;
    MLabel                  *label;
    MSeparator              *separator;

    container = new MWidgetController (parent);
    container->setContentsMargins (0., 0., 0., 0.);
    container->setStyleName (SUBTITLE_PANEL_STYLE_NAME);

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0., 0., 0., 0.);
    layout->setSpacing (0.);
    /*
     *
     */
    separator = new MSeparator;
    separator->setStyleName (SUBTITLE_DIVIDER_STYLE_NAME);

    /*
     *
     */
    label = new MLabel (subTitle);
    label->setStyleName (SUBTITLE_LABEL_STYLE_NAME);


    layout->addItem (separator);
    layout->setStretchFactor (separator, 2);

    layout->addItem (label);
    layout->setAlignment (label, Qt::AlignLeft);
    layout->setStretchFactor (label, 0);

    container->setLayout (layout);
    targetPolicy->addItem (container);
}

void
UsbView::addButtons ()
{
    MButtonGroup *group = new MButtonGroup();
    int           currentModeIndex = -1;

    #ifdef HAVE_QMSYSTEM
    currentModeIndex = usbModeIndex (m_logic->getDefaultMode ());
    SYS_DEBUG ("The default mode is %s",
            SYS_STR(usbModeName(m_logic->getDefaultMode ())));
    #endif

    for (int n = 0; n < UsbModeLastMode; ++n) {
        MButton *button;

        if (n == UsbModeSDK && !m_DeveloperMode)
            continue;

        SYS_DEBUG ("*** n = %d", n);
        button = new MButton (usbModeUIString(UsbModeType(n)));

        group->addButton (button);
        button->setStyleName (usbModeButtonStyle(UsbModeType(n)));
        button->setViewType (MButton::groupType);
        button->setCheckable (true);
        m_policy->addItem (button);
        m_policy->setAlignment (button, Qt::AlignCenter);

        m_Buttons.append (button);

        if (n == currentModeIndex) {
            button->setChecked (true);
        }

        connect (button, SIGNAL (toggled(bool)),
                    this, SLOT(buttonToggled (bool)));
    }
}

QString
UsbView::currentText () const
{
    SYS_DEBUG ("");

#ifdef HAVE_QMSYSTEM
    QmUSBMode::Mode active = m_logic->getMode ();
    SYS_DEBUG ("*** active = %s", SYS_STR(usbModeName(active)));

    switch (active) {
        case QmUSBMode::MassStorage:
            //% "Mass storage in use"
            return qtTrId ("qtn_usb_storage_active");
            break;

        case QmUSBMode::OviSuite:
            //% "Sync and connect in use"
            return qtTrId ("qtn_usb_sync_active");
            break;
#ifndef MEEGO
        case QmUSBMode::SDK:
            return "SDK";
            break;
#endif
        case QmUSBMode::ModeRequest:
            // Seems that the ChargingOnly comes late (30 sec or so), and we can
            // show the chargingonly early.
        case QmUSBMode::Ask:
            // Seems that the ChargingOnly comes late (30 sec or so), and we can
            // show the chargingonly early.
        case QmUSBMode::ChargingOnly:
            //% "Current state: Charging only"
            return qtTrId ("qtn_usb_charging");
            break;

        case QmUSBMode::Disconnected:
        case QmUSBMode::Undefined:
            // Nothing to do.
            break;

        default:
            SYS_WARNING ("UNHANDLED MODE: %d", active);
            return "";
            break;
    }
#endif
    return "";
}

void
UsbView::updateInfoLabel ()
{
    SYS_DEBUG ("");
    if (! m_policy)
        return;

    QString infoText = currentText ();

    if ((! infoText.isEmpty ()) && m_infoLabel)
    {
        /*
         * Label is already there, lets update the text
         */
        m_infoLabel->setText (infoText);
        return;
    }

    if (infoText.isEmpty ()) {
        /*
         * Label needs to be removed if it is there...
         */
        if (! m_infoLabel)
            return;

        /*
         * Delete the separator, and the infoWidget
         */
        delete m_policy->itemAt (m_infoOrder + 1);
        delete m_policy->itemAt (m_infoOrder);
        m_infoLabel = 0;

        return;
    }

    SYS_DEBUG ("creating the info-widget");

    /*
     * When we shown no text we didn't need the separator, now we need one.
     */
    MSeparator       *separator;
    separator = new MSeparator;
    separator->setStyleName ("CommonLargeSpacer");

    /*
     * Create the info label widget, and initialize its contents...
     */
    QGraphicsLinearLayout *iwLayout =
        new QGraphicsLinearLayout (Qt::Horizontal);
    iwLayout->setContentsMargins (0,0,0,0);
    iwLayout->setSpacing (0);

    MImageWidget *iwIcon = new MImageWidget;
    iwIcon->setStyleName ("CommonMainIcon");
    iwIcon->setImage ("icon-m-common-usb");
    iwLayout->addItem (iwIcon);

    m_infoLabel = new MLabel;
    m_infoLabel->setStyleName ("CommonSingleTitleInverted");
    m_infoLabel->setWordWrap (true);
    m_infoLabel->setText (infoText);
    iwLayout->addItem (m_infoLabel);

    MStylableWidget *infoWidget = new MStylableWidget;
    infoWidget->setContentsMargins (0,0,0,0);
    infoWidget->setLayout (iwLayout);

    infoWidget->setStyleName ("CommonTextFrameInverted");

    /*
     * Insert it to the proper place... after the separator.
     */
    m_policy->insertItem (m_infoOrder, infoWidget);
    m_policy->insertItem (m_infoOrder + 1, separator);
}

void
UsbView::buttonToggled (
        bool state)
{
    int index;

    /*
     * For every click we got two signals: one buttons is going to be un-toggled
     * and one is going to be toggled. To handle the event we only handle one of
     * these.
     */
    if (!state)
        return;

    index = selectedButtonIndex ();
    if (VALID_USB_MODE(index)) {
        usbModeActivated (index);
    } else {
        SYS_WARNING ("Unhandled mode.");
    }
}

QString
UsbView::usbModeUIString (
        UsbModeType type) const
{
    switch (type) {
        case UsbModeAsk:
            //% "Always ask"
            return qtTrId ("qtn_usb_always_ask");

        case UsbModeMassStorage:
            //% "Mass storage"
            return qtTrId("qtn_usb_mass_storage");

        case UsbModeOviSuite:
            //% "Sync and connect"
            return qtTrId("qtn_usb_ovi_suite");

        case UsbModeSDK:
            return "SDK";

        default:
            SYS_WARNING ("Invalid USB mode: %d", type);
    }

    return QString();
}

QString
UsbView::usbModeButtonStyle (
        UsbModeType type) const
{
    switch (type) {
        case UsbModeAsk:
            return "CommonTopSplitButtonInverted";

        case UsbModeMassStorage:
            return "CommonVerticalCenterSplitButtonInverted";

        case UsbModeOviSuite:
            if (m_DeveloperMode)
                return "CommonVerticalCenterSplitButtonInverted";

            return "CommonBottomSplitButtonInverted";

        case UsbModeSDK:
            return "CommonBottomSplitButtonInverted";

        default:
            SYS_WARNING ("Invalid USB mode: %d", type);
    }

    return QString();
}

int
UsbView::selectedButtonIndex () const
{
    int index = -1;

    for (int n = 0; n < m_Buttons.size(); ++n) {
        if (m_Buttons[n]->isChecked())
            index = n;
    }

    SYS_DEBUG ("returning %d", index);
    return index;
}

void
UsbView::setSelectedButtonIndex (
        int index)
{
    for (int n = 0; n < m_Buttons.size(); ++n) {
        if (n == index) {
            if (!m_Buttons[n]->isChecked())
                m_Buttons[n]->setChecked(true);
            break;
        }
    }
}
