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
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
** Authors: David Kedves <dkedves@blumsoft.eu>
**          Laszlo Pere <lpere@blumsoft.eu>
**
****************************************************************************/

#include "warrantywidget.h"

#include <MLabel>
#include <MLayout>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QVariant>
#include <MLinearLayoutPolicy>
#include <QGraphicsLinearLayout>
#include <MSeparator>

#include "../styles.h" 

#define DEBUG
#define WARNING
#include "../debug.h"

static const QString configPath ("/usr/share/about-contents/");
static const QString configFile (configPath + "contents.ini");

#include <MWidgetCreator>
M_REGISTER_WIDGET_NO_CREATE (WarrantyWidget);

WarrantyWidget::WarrantyWidget (
        WarrantyBusinessLogic  *warrantyBusinessLogic, 
        QGraphicsWidget        *parent) :
    DcpStylableWidget (parent),
    m_WarrantyBusinessLogic (warrantyBusinessLogic),
    m_labelExpiration (0)
{
    initialize ();

    createContent ();
}

WarrantyWidget::~WarrantyWidget ()
{
}

void
WarrantyWidget::initialize ()
{
    QSettings content (configFile, QSettings::IniFormat);

    m_warrantyTimer =
        content.value ("warrantytimer", true).toBool ();
    m_warrantyText =
        content.value ("warrantytext", "qtn_warr_terms").toString ();

    SYS_DEBUG ("show warranty timer = %s", SYS_BOOL (m_warrantyTimer));
    SYS_DEBUG ("warranty_text = %s", SYS_STR (m_warrantyText));

    if (! m_warrantyText.isEmpty ())
    {
        if (m_warrantyText.contains ("qtn_"))
            m_warrantyText = qtTrId (m_warrantyText.toAscii ().constData ());
        else
        {
            QFile warrantyFile (configPath + m_warrantyText);
            if (warrantyFile.open (QIODevice::ReadOnly | QIODevice::Text))
            {
                QTextStream inStream (&warrantyFile);
                m_warrantyText = inStream.readAll ();
            }
            else
            {
                SYS_WARNING ("Warranty text cannot be loaded!");
                m_warrantyText = "";
            }
        }
    }
}

void
WarrantyWidget::createContent ()
{
    MLayout             *layout;
    MLinearLayoutPolicy *policy;

    layout = new MLayout;
    layout->setContentsMargins (0,0,0,0);

    policy = new MLinearLayoutPolicy (layout, Qt::Vertical);
    policy->setContentsMargins (0., 0., 0., 0.);
    policy->setSpacing (0.);

    /*
     * Add the title-bar
     */
    addHeaderContainer (policy);
    #ifdef APP_TITLE_USES_SPACER
    addStretcher (policy, APP_TITLE_DIVIDER_STYLE_NAME);
    #endif

    if (m_warrantyTimer) {
        // The label that shows the expiration date
        m_labelExpiration = new MLabel;
        m_labelExpiration->setObjectName ("WarrantyAppletExpirationLabel");
        m_labelExpiration->setStyleName ("CommonTitleInverted");
        m_labelExpiration->setWordWrap (true);
    }

    // The label that shows the terms of the warranty
    m_labelTerms = new MLabel;
    m_labelTerms->setObjectName ("WarrantyAppletTermsLabel");
    m_labelTerms->setStyleName ("CommonBodyTextInverted");
    m_labelTerms->setWordWrap (true);
    
    /*
     * Adding the two labels with a new container
     */
    addLabelContainer (policy, m_labelExpiration, m_labelTerms);

    policy->addStretch ();

    retranslateUi ();

    layout->setPolicy (policy);
    setLayout (layout);
}

void 
WarrantyWidget::addStretcher (
        MLinearLayoutPolicy *mainLayout,
        const QString       &styleName)
{
    MSeparator *stretcher;

    Q_ASSERT (mainLayout);

    stretcher = new MSeparator ();
    stretcher->setStyleName (styleName);
    mainLayout->addItem (stretcher);
}

void 
WarrantyWidget::addHeaderContainer (
        MLinearLayoutPolicy *mainLayout)
{
    Q_ASSERT (mainLayout);

    /*
     * The label that we use as title.
     */
    //% "Warranty"
    m_TitleLabel = new MLabel (qtTrId("qtn_warr_title"));
    m_TitleLabel->setStyleName (APP_TITLE_LABEL_STYLE_NAME);

    /*
     * Adding the whole row to the main container.
     */
    mainLayout->addItem (m_TitleLabel);
    mainLayout->setStretchFactor (m_TitleLabel, 0);
}

void 
WarrantyWidget::addLabelContainer (
        MLinearLayoutPolicy *mainLayout,
        MLabel              *label1,
        MLabel              *label2)
{
    Q_ASSERT (mainLayout);

    /*
     * Adding the label to the layout.
     */
    mainLayout->addItem (label1);
    mainLayout->setAlignment (label1, Qt::AlignLeft);

    MSeparator *separator = new MSeparator;
    separator->setStyleName ("CommonSpacer");

    mainLayout->addItem (separator);

    mainLayout->addItem (label2);
    mainLayout->setAlignment (label2, Qt::AlignLeft);
}

/*
 * gconftool-2 -t string -s /meegotouch/i18n/language hu_HU
 */
void
WarrantyWidget::retranslateUi ()
{
    if (m_labelExpiration)
    {
        int expirationDays = m_WarrantyBusinessLogic->getExpirationDays ();

        if (expirationDays > 0) {
          //% "Product warranty will expire in <b>%Ln</b> day."
          m_labelExpiration->setText (
                  qtTrId ("qtn_warr_expiration", expirationDays)
                          .arg (expirationDays));
        } else if (expirationDays == 0) {
          //% "Product warranty is expired."
          m_labelExpiration->setText (qtTrId ("qtn_warr_expired"));
        } else {
          //% "Warranty timer error."
          m_labelExpiration->setText (qtTrId ("qtn_warr_error_timer"));
        }
    }

    /* This stuff is for eng.english generation */
#if 0
    //% "(insert terms of warranty here)"
    qtTrId ("qtn_warr_terms")
#endif
    m_labelTerms->setText (m_warrantyText);
}

