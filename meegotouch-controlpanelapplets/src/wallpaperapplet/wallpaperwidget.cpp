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
#include "wallpaperwidget.h"
#include "wallpaperlist.h"

#include <MApplicationPage>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <QTimer>
#include <MContentItem>
#include <MAction>
#include <QGraphicsLinearLayout>
#include <QDBusInterface>
#include <MLabel>

#include "wallpapereditorsheet.h"

//#define DEBUG
//#define WARNING
#include "../debug.h"

static const int MaxColumns = 2;
static const qreal navBarTransparency = 0.85;

#include <MWidgetCreator>
M_REGISTER_WIDGET_NO_CREATE(WallpaperWidget)

/******************************************************************************
 *
 */
WallpaperWidget::WallpaperWidget (
        WallpaperBusinessLogic *wallpaperBusinessLogic, 
        QGraphicsWidget        *parent) :
    DcpWidget (parent),
    m_WallpaperBusinessLogic (wallpaperBusinessLogic),
    m_NavigationBarTransparency (-1)
{
    // For testability
    setObjectName ("WallpaperWidget");

    MWindow *win = MApplication::activeWindow ();

    if (win) {
        m_NavigationBarTransparency = 
            win->property("navigationBarOpacity").toReal();
        win->setProperty ("navigationBarOpacity", navBarTransparency);
    }

    /*
     * Unfortunatelly the content has to be created late, otherwise the list
     * only shows one row.
     * FIXME: I'm n ot sure how late should this be done, I thing we should use
     * the void MApplicationPage::createContent(), but I don't know how to do
     * that.
     */
    //createContent ();
    QTimer::singleShot(100, this, SLOT(createContent()));
}

WallpaperWidget::~WallpaperWidget ()
{
    if (m_NavigationBarTransparency >= 0) {
        MWindow *win = MApplication::activeWindow ();

        if (win) {
            win->setProperty ("navigationBarOpacity", 
                    m_NavigationBarTransparency);
        }
    }
}

void
WallpaperWidget::createContent ()
{
    MLayout              *layout;
    MLinearLayoutPolicy  *mainLayout;

    /*
     * Creating a layout that holds the rows of the internal widgets.
     */
    layout = new MLayout (this);
    mainLayout = new MLinearLayoutPolicy (layout, Qt::Vertical);
    mainLayout->setContentsMargins (0., 0., 0., 0.);
    mainLayout->setSpacing (0.);
    setLayout (layout);

#ifndef MEEGO
    /*
     * Adding the header/title
     */
    addHeaderContainer (mainLayout);
#endif

    /*
     * The list of the available images.
     */
    m_ImageList = new WallpaperList (m_WallpaperBusinessLogic, this);
    m_ImageList->setObjectName("WallpaperImageList");
    connect (m_ImageList, SIGNAL(imageActivated(WallpaperDescriptor *)),
            this, SLOT(slotImageActivated(WallpaperDescriptor *)));

    m_ImageList->setDataSourceType (WallpaperList::DataSourceLocal);


    /*
     * Adding all widgets into the layout.
     */
    mainLayout->addItem (m_ImageList);
    mainLayout->setStretchFactor (m_ImageList, 1);

    connect (m_WallpaperBusinessLogic, SIGNAL(imageEditRequested()),
            this, SLOT(slotImageActivated()));
}

void 
WallpaperWidget::addHeaderContainer (
        MLinearLayoutPolicy *mainLayout)
{
    QGraphicsLinearLayout *layout;

    Q_ASSERT (mainLayout);

    layout = new QGraphicsLinearLayout (Qt::Horizontal);
    layout->setContentsMargins (0., 0., 0., 0.);
    layout->setSpacing (0.);

    /*
     * The label that we use as title.
     */
    //% "Wallpaper"
    m_TitleLabel = new MLabel (qtTrId ("qtn_wall_wallpaper"));
    m_TitleLabel->setStyleName ("CommonApplicationHeaderInverted");
    layout->addItem (m_TitleLabel);
    layout->setAlignment (m_TitleLabel, Qt::AlignLeft);

    /*
     * Adding the whole row to the main container.
     */
    mainLayout->addItem (layout);
    mainLayout->setStretchFactor (layout, 0);
}

void
WallpaperWidget::retranslateUi ()
{
    if (m_TitleLabel)
        //% "Wallpaper"
        m_TitleLabel->setText (qtTrId("qtn_wall_wallpaper"));
}


/*
 * This slot is called when the user activates an image in the list. The
 * WallpaperList has a signal for that.
 */
void 
WallpaperWidget::slotImageActivated (
        WallpaperDescriptor *desc)
{
    SYS_DEBUG ("*** desc = %s", SYS_STR(desc->basename()));
    m_WallpaperBusinessLogic->startEdit (desc);
}

/*
 * When this slot is activated the image has to be already loaded, so that the
 * editor widget can be shown without delay.
 */
void 
WallpaperWidget::slotImageActivated ()
{
    SYS_DEBUG ("");
    /*
     * With this define
     */
#if 0
    WallpaperEditorSheet  *sheet;
    sheet = new WallpaperEditorSheet (m_WallpaperBusinessLogic);
    sheet->appear(scene(), MSceneWindow::DestroyWhenDone);
#else
    emit changeWidget (1);
#endif
}


void 
WallpaperWidget::oviActivated ()
{
    static const char OviIf[] = "com.nokia.OviStoreClient";
    QDBusInterface OviStore (OviIf, "/", OviIf);

    OviStore.call ("LaunchWithKeyword", QString ("graphics"));
}

void
WallpaperWidget::polishEvent ()
{
    QGraphicsWidget  *parent;
    MApplicationPage *page = 0;
    MAction          *action;
    
    /*
     * We need to find the MApplicationPage among our parents.
     */
    parent = parentWidget();
    while (parent) {
        page = qobject_cast <MApplicationPage *>(parent);
        if (page)
            break;
        parent = parent->parentWidget();
    }

    if (!page)
        return;

    /**************************************************************************
     * Hiding the home button. 
     */
    page->setComponentsDisplayMode (
            MApplicationPage::HomeButton,
            MApplicationPageModel::Hide);

    /*
     * Adding the ovi action.
     */
    action = new MAction("icon-m-toolbar-content-ovi-music-white", "", this);
    action->setLocation(MAction::ToolBarLocation);
    page->addAction(action);
    connect(action, SIGNAL(triggered()), this, SLOT(oviActivated()));
}

