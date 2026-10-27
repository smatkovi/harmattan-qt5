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
#include "resetwidget.h"

#include <MLayout>
#include <MBanner>
#include <MApplication>
#include <MHelpButton>
#include <QGraphicsLinearLayout>
#include <MLinearLayoutPolicy>
#include <MButton>
#include <MDialog>
#include <MMessageBox>
#include <MSeparator>
#include <MStylableWidget>
#include <MLocale>

//#define DEBUG
#include "../debug.h"

#define qtTrIdShort(id) qtTrId(id).split(QChar(0x9c)).last()

ResetWidget::ResetWidget (
        ResetBusinessLogic     *resetBusinessLogic, 
        QGraphicsWidget        *parent) :
    DcpWidget (parent),
    m_ResetBusinessLogic (resetBusinessLogic),
    m_currentPlan (None)
{
    createContent();

    connect (resetBusinessLogic, SIGNAL (gotAccess ()),
             this, SLOT (doTheWork ()));
}

ResetWidget::~ResetWidget ()
{
}


void
ResetWidget::createContent ()
{
    MLayout             *layout;
    MLinearLayoutPolicy *policy;
    MButton             *restoreButton;
    MButton             *clearButton;
    MSeparator          *spacer;

    /*
     *
     */
    layout = new MLayout;
    policy = new MLinearLayoutPolicy (layout, Qt::Vertical);
    policy->setContentsMargins (0., 0., 0., 0.);
    policy->setSpacing (0.);
    
    /*
     *
     */
    spacer = new MSeparator;
    // Using this one instead of "CommonSpacer", margins look even.
    spacer->setStyleName ("CommonLargeSpacer");
    policy->addItem (spacer);

    /*
     * The first button.
     */
    //% "Restore original settings"
    restoreButton = new MButton (qtTrId("qtn_rset_restore"));
    restoreButton->setStyleName ("CommonSingleButtonInverted");
    restoreButton->setObjectName ("ResetAppletRFSButton");
    connect (restoreButton, SIGNAL(clicked()), 
            this, SLOT(restoreActivated()));

    
    /*
     * The second button.
     */
    //% "Clear device"
    clearButton = new MButton (qtTrId("qtn_rset_clear"));
    clearButton->setStyleName ("CommonSingleButtonInverted");
    clearButton->setObjectName ("ResetAppletCUDButton");
    connect (clearButton, SIGNAL(clicked()), 
            this, SLOT(clearActivated()));

    addButtonContainer (policy, restoreButton, clearButton);
    policy->addStretch();
    /*
     *
     */
    layout->setPolicy (policy);
    setLayout (layout);
}

MHelpButton *
ResetWidget::createHelpButton (const QString &link)
{
    MHelpButton *helpButton = new MHelpButton (link);
    helpButton->setViewType (MButton::iconType);
    helpButton->setIconID ("icon-s-description-inverse");
    helpButton->setStyleName ("CommonRightIcon");
    return helpButton;
}

void
ResetWidget::addButtonContainer (
            MLinearLayoutPolicy *mainLayout,
            MButton             *button1,
            MButton             *button2)
{
    MSeparator          *spacer;
    QGraphicsLinearLayout *layout =
        new QGraphicsLinearLayout (Qt::Vertical);
    layout->setContentsMargins (0,0,0,0);
    layout->setSpacing (0);
    
    /*
     * One spacer
     */
    spacer = new MSeparator;
    spacer->setStyleName ("CommonSpacer");

    /*
     * Button 1 layout
     */
    QGraphicsLinearLayout *button1layout =
        new QGraphicsLinearLayout (Qt::Horizontal);
    button1layout->setContentsMargins (0,0,0,0);
    button1layout->setSpacing (0);

    MStylableWidget *imgSpacer1 = new MStylableWidget;
    imgSpacer1->setStyleName ("CommonLeftIcon");
    button1layout->addItem (imgSpacer1);

    button1layout->addStretch ();

    button1layout->addItem (button1);
    button1layout->setAlignment (button1, Qt::AlignCenter);

    button1layout->addStretch ();

    MHelpButton *hp1 = createHelpButton ("IDUG_MEEGO_SETT_RESTORE.html");
    button1layout->addItem (hp1);
    button1layout->setAlignment (hp1, Qt::AlignVCenter);

    layout->addItem (button1layout);
    layout->setAlignment (button1layout, Qt::AlignCenter);

    layout->addItem (spacer);

    /*
     * Button 2 layout
     */
    QGraphicsLinearLayout *button2layout =
        new QGraphicsLinearLayout (Qt::Horizontal);
    button2layout->setContentsMargins (0,0,0,0);
    button2layout->setSpacing (0);

    MStylableWidget *imgSpacer2 = new MStylableWidget;
    imgSpacer2->setStyleName ("CommonLeftIcon");
    button2layout->addItem (imgSpacer2);
    button2layout->addStretch ();

    button2layout->addItem (button2);
    button2layout->setAlignment (button2, Qt::AlignCenter);

    button2layout->addStretch ();
    MHelpButton *hp2 = createHelpButton ("IDUG_MEEGO_SETT_CLEARDEVICE.html");
    button2layout->addItem (hp2);
    button2layout->setAlignment (hp2, Qt::AlignVCenter);

    layout->addItem (button2layout);
    layout->setAlignment (button2layout, Qt::AlignCenter);

    /*
     *
     */
    mainLayout->addItem (layout);
    mainLayout->setStretchFactor (layout, 0);
}

void
ResetWidget::restoreActivated ()
{
    MDialog   *dialog;

    if (m_ResetBusinessLogic->isUsbConnected ())
    {
        showMassStorageWarning ();
        return;
    }

    //% "Restore original settings?"
    QString question = qtTrId ("qtn_rset_restore_query");
    // It is a bit ugly, but translations contains \n stuffs:
    question.replace ("\\n", "<br>");
    question.replace ("\n", "<br>");

    //% "Restore original settings?"
    dialog = new MMessageBox (qtTrId ("qtn_rset_restore_query_title"),
                              question, M::YesButton | M::NoButton);
    connect (dialog, SIGNAL (accepted ()), SLOT (restoreConfirmed ()));

    dialog->appear (MApplication::instance ()->activeWindow (),
                    MSceneWindow::DestroyWhenDone);
}

void
ResetWidget::restoreConfirmed ()
{
    SYS_DEBUG ("user choosen yes");
    m_currentPlan = ResetSettings;
    m_ResetBusinessLogic->getAccess ();
}

void
ResetWidget::clearActivated ()
{
    MDialog   *dialog;
    MLocale    locale;
    QString    question;

    if (m_ResetBusinessLogic->isUsbConnected ()) {
        showMassStorageWarning ();
        return;
    }

    //% "Clear all user data and restore original settings?"
    question = qtTrId("qtn_rset_clear_query").arg(locale.formatNumber(15));
    question.replace ("\\n", "<br>");
    question.replace ("\n", "<br>");

    //% "Clear all data?"
    dialog = new MMessageBox (qtTrId ("qtn_rset_clear_query_title"),
                              question, M::YesButton | M::NoButton);
    connect (dialog, SIGNAL (accepted ()), SLOT (clearConfirmed ()));

    dialog->appear (MApplication::instance ()->activeWindow (),
                    MSceneWindow::DestroyWhenDone);
}

void
ResetWidget::clearConfirmed ()
{
    SYS_DEBUG ("user choosen yes");
    m_currentPlan = ClearData;
    m_ResetBusinessLogic->getAccess ();
}

void
ResetWidget::doTheWork ()
{
    switch (m_currentPlan)
    {
        case ResetSettings:
            m_ResetBusinessLogic->performRestoreSettings();
            break;
        case ClearData:
            m_ResetBusinessLogic->performClearData();
            break;
        default:
            SYS_WARNING ("Got access, but no plan ?!");
            break;
    }
    m_currentPlan = None;
}

void
ResetWidget::showMassStorageWarning ()
{
    SYS_DEBUG ("");
    infoBanner = new MBanner;
    infoBanner->setStyleName ("InformationBanner");
    infoBanner->setObjectName ("InfoBanner");

    //% "Device resets are not possible while USB is connected in mass storage mode."
    infoBanner->setTitle (qtTrId ("qtn_rset_not_possible"));

    infoBanner->appear (MApplication::instance ()->activeWindow (),
                        MSceneWindow::DestroyWhenDone);
}

