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
#include "wallpaperlistsheet.h"
#include "wallpaperconfiguration.h"
#include "wallpaperbusinesslogic.h"
#include "wallpaperwidget.h"

#include <MBasicSheetHeader>
#include <QAction>
#include <MPannableViewport>

WallpaperListSheet::WallpaperListSheet (
        WallpaperBusinessLogic *wallpaperBusinessLogic)
{
    setStyleName ("Inverted");
    createCentralWidget (wallpaperBusinessLogic);
    createHeaderWidget ();
    
    connect (wallpaperBusinessLogic, SIGNAL(workerStarted()),
            this, SLOT(workerStarted()));
    connect (wallpaperBusinessLogic, SIGNAL(workerEnded()),
            this, SLOT(workerEnded()));
}

void 
WallpaperListSheet::createCentralWidget(
        WallpaperBusinessLogic *wallpaperBusinessLogic)
{
    WallpaperWidget *mainWidget;
    MPannableViewport *pannableViewport = new MPannableViewport;
    pannableViewport->setObjectName("pannableViewport");

    mainWidget = new WallpaperWidget (wallpaperBusinessLogic);

    pannableViewport->setWidget(mainWidget);

    setCentralWidget (pannableViewport);
}

void 
WallpaperListSheet::createHeaderWidget()
{
    MBasicSheetHeader *basicHeader = new MBasicSheetHeader (this);
    QAction           *cancelAction;

    basicHeader->setStyleName ("Inverted");
    cancelAction = new QAction(qtTrId("qtn_comm_cancel"), basicHeader);
    
    connect (cancelAction, SIGNAL(triggered()), SLOT(dismiss()));

    basicHeader->setNegativeAction (cancelAction);

    setHeaderWidget(basicHeader);
}

     
void
WallpaperListSheet::workerStarted ()
{
    MBasicSheetHeader *header = (MBasicSheetHeader *) headerWidget();
    header->setProgressIndicatorVisible (true);
    header->negativeAction()->setEnabled (false);
}


void
WallpaperListSheet::workerEnded ()
{
    MBasicSheetHeader *header = (MBasicSheetHeader *) headerWidget();
    header->setProgressIndicatorVisible (false);
    header->negativeAction()->setEnabled (true);
}
