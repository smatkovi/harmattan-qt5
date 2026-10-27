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
#include "aboutwidget.h"

#include <QFile>
#include <QTimer>
#include <QPixmap>
#include <QTextStream>
#include <QVariant>
#include <QSettings>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>

#include <MImageWidget>
#include <MLabel>
#include <MSeparator>
#include <MWidgetController>
#include <MLinearLayoutPolicy>
#include <MLayout>
#include <MGConfItem>

#include "logowidget.h"
#include "../styles.h"

#ifdef HAVE_CONTENT_ACTION
#include <contentaction.h>
#endif

#define DEBUG
#define WARNING
#include "../debug.h"

#include <MWidgetCreator>
M_REGISTER_WIDGET_NO_CREATE(AboutWidget)

class ContentWidget: public MStylableWidget
{
  public:
    ContentWidget (QGraphicsItem *parent = 0):
        MStylableWidget (parent),
        m_title (0), m_subTitle (0)
    {
        setContentsMargins (0,0,0,0);

        QGraphicsLinearLayout *layout =
            new QGraphicsLinearLayout (Qt::Vertical);
        layout->setSpacing (0);
        layout->setContentsMargins (0, 0, 0, 0);

        m_title = new MLabel;
        m_title->setStyleName ("CommonSubTitleTopInverted");
        m_title->setWordWrap (true);
        m_title->setAlignment (Qt::AlignLeft | Qt::AlignAbsolute);
        m_title->setWrapMode (QTextOption::WrapAtWordBoundaryOrAnywhere);

        m_subTitle = new MLabel;
        m_subTitle->setStyleName ("CommonTitleBottomInverted");
        m_subTitle->setWordWrap (true);
        m_subTitle->setAlignment (Qt::AlignLeft | Qt::AlignAbsolute);
        m_subTitle->setWrapMode (QTextOption::WrapAtWordBoundaryOrAnywhere);
        m_subTitle->setText ("-");

        layout->addItem (m_title);
        layout->addItem (m_subTitle);
       
        /*
         * Fixes: NB#276291
         */
        setStyleName ("CommonLargePanelInverted");
        setLayout (layout);
    }
    void setTitle(QString const &title)
    {
        m_title->setText (title);
    }
    void setSubtitle (QString const &subTitle)
    {
        m_subTitle->setText(subTitle);
    }
  private:
    MLabel *m_title;
    MLabel *m_subTitle;
};

class CertsWidget : public MWidgetController
{
public:
    explicit CertsWidget (QGraphicsItem *parent = 0) :
        MWidgetController (parent)
    {
        certsLayout = new QGraphicsLinearLayout (Qt::Vertical);
        certsLayout->setContentsMargins (0,0,0,0);
        certsLayout->setSpacing (0);

        setContentsMargins (0,0,0,0);
        setLayout (certsLayout);
    }

    ~CertsWidget ()
    {

    }

    void
    setImages (QStringList images)
    {
        foreach (QString img, images)
        {
            SYS_DEBUG ("loading %s ...", SYS_STR (img));

            if (! QFile::exists (img))
            {
                SYS_WARNING ("%s is missing!", SYS_STR (img));
                continue;
            }

            QPixmap pixmap;

            if (pixmap.load (img))
            {
                MImageWidget *crt = new MImageWidget (&pixmap);
                crt->setContentsMargins (0,0,0,0);
                certsLayout->addItem (crt);
            }
            else
            {
                SYS_WARNING ("%s can not be loaded!", SYS_STR (img));
                continue;
            }
        }
    }
private:
    QGraphicsLinearLayout *certsLayout;
};

/******************************************************************************
 * The actual widget.
 */
AboutWidget::AboutWidget (QGraphicsWidget *parent) :
    DcpStylableWidget (parent),
    m_layout (0),
    m_AboutBusinessLogic (new AboutBusinessLogic),
    m_TitleLabel (0),
    m_Version (0),
    m_ProductName (0),
    m_WiFi (0),
    m_Ip (0),
    m_Bt (0),
    m_IMEI (0),
    m_LicenseLabel (0),
    m_imgBarcode (0),
    m_imgCerts (0)
{
    setContentsMargins (0,0,0,0);

    /* create the widgets ... */
    createContent ();

    connect (m_AboutBusinessLogic,
             SIGNAL (requestFinished (AboutBusinessLogic::requestType, QVariant)),
             SLOT (gotInfo (AboutBusinessLogic::requestType, QVariant)));

    /* Stupid h*ck .. :-S */
#ifndef UNIT_TEST
    /* start the data collection thread... */
    m_AboutBusinessLogic->start ();
#endif
}

AboutWidget::~AboutWidget ()
{
    delete m_AboutBusinessLogic;
}

/*
 * Must be called after initialize ()
 * (as some fields are optional, and customization
 *  determines what fields needs to be used...)
 */
void
AboutWidget::createContent ()
{
    MGConfItem eggsGconfItem (EGGS_GCONF_KEY);

    m_currentRow = 0;
    m_certsRow = 0;
    m_logoRow = 0;

    /*
     * Creating a layout that holds the rows of the internal widgets.
     */
    m_layout = new QGraphicsGridLayout;
    m_layout->setContentsMargins (0., 0., 0., 0.);
    m_layout->setSpacing (0.);

    /***********************
     *  HEADER             *
     *                     *
     *  LOGO       /-----\ *
     *             |BAR- | *
     *  DEVICE &&  | CODE| *
     *   SW NAME   \-----/ *
     *  ....               *
     ***********************/

    // Row 1: the title
    addHeaderContainer ();

    /* COLUMN 1 */
    // Row 2: the logo
    addLogoContainer ();

    // Row 3: the product name
    addNamesContainer ();

    /* COLUMN 2 */
    // Row 2-3: the barcode
    addBarcodeContainer ();

    // Row 4: versions
    addVersionContainer ();

    // Row 5: WiFi
    addWiFiMACContainer ();
    
    // Row 6: KekFog
    addBtMACContainer ();

    // Row 7: IMEI
    addIMEIContainer ();

    // Row 8: The IP address
    if (eggsGconfItem.value().toBool())
        addIpContainer ();

#if 0
    // Row 8: Certs container...
    addCertsContainer ();
#else
    m_certsRow = m_currentRow;
    /*
     * 2 spacer, 1 separator, 1 image
     */
    m_currentRow += 4;
#endif

    // Last Row: the license label
    addLicenseLabelContainer ();
//in Meego add also a label with names read from /etc/about_names
#ifdef MEEGO
    addNamesLabelContainer ();
#endif

    /*
     * And set the layout...
     */
    setLayout (m_layout);
}

void
AboutWidget::gotInfo (
    AboutBusinessLogic::requestType type,
    QVariant                        value)
{
    switch (type)
    {
        case AboutBusinessLogic::reqProdName:
            m_ProductName->setTitle (value.toString ());
            break;
        case AboutBusinessLogic::reqSwName:
            m_ProductName->setSubtitle (value.toString ());
            break;
        case AboutBusinessLogic::reqLicense:
            m_LicenseLabel->setText (value.toString ());
            break;
        case AboutBusinessLogic::reqCertsImageNeeded:
            addCertsContainer ();
            break;
        case AboutBusinessLogic::reqCertsImage:
            if (m_imgCerts)
                m_imgCerts->setImages (value.value<QStringList>());
            break;
        case AboutBusinessLogic::reqBarcodeImage:
            m_imgBarcode->setImage (value.value<QImage>());
            break;
        case AboutBusinessLogic::reqOsVersion:
            m_Version->setSubtitle (value.toString ());
            break;
        case AboutBusinessLogic::reqImei:
            m_IMEI->setSubtitle (value.toString ());
            break;
        case AboutBusinessLogic::reqWifiAddr:
            m_WiFi->setSubtitle (value.toString ());
            break;
        case AboutBusinessLogic::reqBtAddr:
            m_Bt->setSubtitle (value.toString ());
            break;
        default:
            break;
    }
}

void
AboutWidget::addHeaderContainer ()
{
    if (!m_layout)
        return;

    /*
     * The label that we use as title.
     */
    //% "About product"
    m_TitleLabel = new MLabel (qtTrId("qtn_prod_about_product"));
    m_TitleLabel->setStyleName (APP_TITLE_LABEL_STYLE_NAME);
    m_layout->addItem (m_TitleLabel, m_currentRow++, 0, 1, 2);
    m_layout->setAlignment (m_TitleLabel, Qt::AlignLeft | Qt::AlignAbsolute);

#ifdef APP_TITLE_USES_SPACER
    addStretcher (APP_TITLE_DIVIDER_STYLE_NAME);
#endif
}

void
AboutWidget::addLogoContainer ()
{
    if (!m_layout)
        return;

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setSpacing (0);
    layout->setContentsMargins (0,0,0,0);

    /* Store this row... maybe there is a barcode... */
    m_logoRow = m_currentRow++;

    /*
     * The logo
     */
    MImageWidget *logo;
    logo = new LogoWidget;
    //logo = new MImageWidget;

    layout->addItem (logo);
    layout->addStretch ();
    /* Only for first row: */
    m_layout->addItem (layout, m_logoRow, 0, 1, 1);
    m_layout->setAlignment (layout, Qt::AlignLeft);

    connect (logo, SIGNAL(eggs(bool)),
            this, SLOT(eggs(bool)));
}

void
AboutWidget::addCertsContainer ()
{
    if (!m_layout)
        return;

    if (m_imgCerts)
        return;

    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setSpacing (0);
    layout->setContentsMargins (0,0,0,0);

    m_currentRow = m_certsRow;

    m_imgCerts = new CertsWidget;
    m_imgCerts->setObjectName ("AboutAppletCertificates");
    m_imgCerts->setMinimumHeight (64.0);

    layout->addItem (m_imgCerts);
    layout->setAlignment (m_imgCerts, Qt::AlignCenter);

    addStretcher ("CommonLargeSpacer");
    addStretcher ("CommonHorizontalSeparatorInverted");
    addStretcher ("CommonLargeSpacer");
    m_layout->addItem (layout, m_currentRow++, 0, 1, 2);
}

void
AboutWidget::addBarcodeContainer ()
{
    if (!m_layout)
        return;

    if (m_imgBarcode)
        return;

    m_imgBarcode = new MImageWidget;
    m_imgBarcode->setObjectName ("AboutAppletBarcodeImage");

    m_layout->addItem (m_imgBarcode,
                       m_logoRow, 1, 2, 1,
                       Qt::AlignRight);
}


void
AboutWidget::addNamesContainer ()
{
    if (!m_layout)
        return;

    m_ProductName = new ContentWidget;
    m_ProductName->setTitle ("");
    /* Only for first row : */
    m_layout->addItem (m_ProductName, m_currentRow++, 0, 1, 1);
    m_layout->setAlignment (m_ProductName, Qt::AlignLeft);
}

void
AboutWidget::addVersionContainer ()
{
    if (!m_layout)
        return;

    m_Version = new ContentWidget;
    m_Version->setTitle ("");
    //% "OS version"
    m_Version->setTitle (qtTrId ("qtn_prod_version"));
    m_layout->addItem (m_Version, m_currentRow++, 0, 1, 2);
}

void
AboutWidget::addWiFiMACContainer ()
{
    if (!m_layout)
        return;

    m_WiFi = new ContentWidget;
    m_WiFi->setTitle ("");
    //% "WLAN MAC address"
    m_WiFi->setTitle (qtTrId ("qtn_prod_wlan_mac_address"));
    m_layout->addItem (m_WiFi, m_currentRow++, 0, 1, 2);
}

void
AboutWidget::addIpContainer (
        int place)
{
    QString device, addr;

    if (!m_layout)
        return;

    if (place < 0)
        place = m_currentRow++;

    m_Ip = new ContentWidget;

    device = "eth0";
    if (!m_AboutBusinessLogic->ipAddress(device, addr)) {
        device = "wlan0";
        m_AboutBusinessLogic->ipAddress(device, addr);
    }

    if (addr.isEmpty())
        addr = "None";

    m_Ip->setTitle (device);
    m_Ip->setSubtitle (addr);

    m_layout->addItem (m_Ip, place, 0, 1, 2);
}


void
AboutWidget::addBtMACContainer ()
{
    if (!m_layout)
        return;

    m_Bt = new ContentWidget;
    m_Bt->setTitle ("");
    //% "Bluetooth address"
    m_Bt->setTitle (qtTrId ("qtn_prod_bt_address"));
    m_layout->addItem (m_Bt, m_currentRow++, 0, 1, 2);
}

void
AboutWidget::addIMEIContainer ()
{
    if (!m_layout)
        return;

    m_IMEI = new ContentWidget;
    //% "IMEI"
    m_IMEI->setTitle (qtTrId ("qtn_prod_imei"));
    m_layout->addItem (m_IMEI, m_currentRow++, 0, 1, 2);
}

void
AboutWidget::addLicenseLabelContainer ()
{
    if (!m_layout)
        return;

    /*
     * The label
     */
    m_LicenseLabel = new MLabel;
    m_LicenseLabel->setWordWrap (true);
    m_LicenseLabel->setStyleName ("CommonBodyTextInverted");
    m_LicenseLabel->setObjectName ("AboutAppletLicenseLabel");
    m_LicenseLabel->setMinimumHeight (100.0);

    connect (m_LicenseLabel, SIGNAL (linkActivated (const QString &)),
             this, SLOT (linkActivated (const QString &)));

    addStretcher ("CommonLargeSpacer");
    addStretcher ("CommonHorizontalSeparatorInverted");
    addStretcher ("CommonLargeSpacer");
    /*
     * Adding the whole row to the main container.
     */
    m_layout->addItem (m_LicenseLabel, m_currentRow++, 0, 1, 2);
}



//The label with the names for Meego
#ifdef MEEGO
void
AboutWidget::addNamesLabelContainer ()
{
  if (!m_layout)
        return;

    m_NamesLabel = new MLabel(m_AboutBusinessLogic->GetAboutNames());
    m_NamesLabel->setWordWrap (true);
    m_NamesLabel->setStyleName ("CommonBodyTextInverted");
    m_NamesLabel->setObjectName ("AboutAppletNamesLabel");
    m_NamesLabel->setMinimumHeight (100.0);


    addStretcher ("CommonLargeSpacer");
    addStretcher ("CommonHorizontalSeparatorInverted");
    addStretcher ("CommonLargeSpacer");
    /*
     *  Adding the whole row to the main container.
     */
    m_layout->addItem (m_NamesLabel, m_currentRow++, 0, 1, 2);
}
#endif

void
AboutWidget::addStretcher (
        const QString &styleName)
{
    if (!m_layout)
        return;

    MSeparator *stretcher;
    stretcher = new MSeparator;
    stretcher->setStyleName (styleName);
    m_layout->addItem (stretcher, m_currentRow++, 0, 1, 2);
}

void
AboutWidget::linkActivated (const QString &link)
{
    // Seems sometimes it contains a \" string :-S
    QString aLink (link);
    aLink = aLink.replace ("\\\"", "");
    SYS_DEBUG ("link = \"%s\"", SYS_STR (aLink));

#ifdef HAVE_CONTENT_ACTION
    ContentAction::Action action =
        ContentAction::Action::defaultActionForScheme (aLink);

    SYS_DEBUG ("action.valid = %s", SYS_BOOL (action.isValid ()));
    SYS_DEBUG ("action.name = %s", SYS_STR (action.name ()));

    action.trigger ();
#else
    SYS_WARNING ("Build without contentaction, don't"
                 " know how to open the link!");
#endif
}

void
AboutWidget::eggs (
        bool on)
{
    SYS_DEBUG ("");
    if (on && !m_Ip) {
        addIpContainer (8);
    } else if (!on && m_Ip) {
        m_Ip->deleteLater ();
        m_Ip = 0;
    }
}
