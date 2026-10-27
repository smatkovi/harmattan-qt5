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
#include "wallpapereditorsheet.h"
#include "wallpaperbusinesslogic.h"
#include "wallpapereditorwidget.h"

#include <MBasicSheetHeader>
#include <QAction>

#undef DEBUG
#define WARNING
#include "../debug.h"

WallpaperEditorSheet::WallpaperEditorSheet (
        WallpaperBusinessLogic *wallpaperBusinessLogic)
{
    createCentralWidget (wallpaperBusinessLogic);
    createHeaderWidget ();
}

void 
WallpaperEditorSheet::createCentralWidget(
        WallpaperBusinessLogic *wallpaperBusinessLogic)
{
    m_EditorWidget = new WallpaperEditorWidget (wallpaperBusinessLogic, this);
    setCentralWidget (m_EditorWidget);
}

void 
WallpaperEditorSheet::createHeaderWidget()
{
    MBasicSheetHeader *basicHeader = new MBasicSheetHeader (this);
    QAction           *doneAction;
    QAction           *cancelAction;

    basicHeader->setStyleName ("CommonSheetHeaderPanelInverted");
    doneAction = new QAction(qtTrId("qtn_comm_command_done"), basicHeader);
    cancelAction = new QAction(qtTrId("qtn_comm_cancel"), basicHeader);
    
    connect(doneAction, SIGNAL(triggered()), SLOT(doneActivated()));
    connect (cancelAction, SIGNAL(triggered()), SLOT(cancelActivated()));

    basicHeader->setPositiveAction (doneAction);
    basicHeader->setNegativeAction (cancelAction);

    setHeaderWidget(basicHeader);
}

void
WallpaperEditorSheet::doneActivated ()
{
    MBasicSheetHeader *header = static_cast<MBasicSheetHeader*>(headerWidget());

    header->setProgressIndicatorVisible (true);
    m_EditorWidget->saveImage ();
    dismiss ();
}

void
WallpaperEditorSheet::cancelActivated ()
{
    m_EditorWidget->dropImage ();
    dismiss ();
}

