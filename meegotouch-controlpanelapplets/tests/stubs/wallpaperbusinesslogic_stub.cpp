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
#include "wallpaperbusinesslogic.h"
#include "wallpaperdescriptor.h"
    
WallpaperBusinessLogic::WallpaperBusinessLogic (
        QObject *parent) :
    QObject (parent)
{
    Q_UNUSED (parent);
}

WallpaperBusinessLogic::~WallpaperBusinessLogic ()
{

}

void
WallpaperBusinessLogic::currentWallpaper (
        QString   &currentFilePath,
        QString   &originalFilePath)
{
    Q_UNUSED (currentFilePath);
    Q_UNUSED (originalFilePath);
}

void 
WallpaperBusinessLogic::workerThreadFinishedSave ()
{
}

void
WallpaperBusinessLogic::gconfTimerSlot ()
{
}


void 
WallpaperBusinessLogic::workerThreadFinishedLoad ()
{
}

void
WallpaperBusinessLogic::portraitGConfChanged ()
{
}

void
WallpaperBusinessLogic::portraitHistoryChanged ()
{
}

QStringList
WallpaperBusinessLogic::history () const
{
    return QStringList();
}

bool
WallpaperBusinessLogic::hasEditedImage () const
{
    return false;
}

bool
WallpaperBusinessLogic::setWallpaper ()
{
    return true;
}

bool
WallpaperBusinessLogic::setWallpaper (
        QPixmap   &pixmap)
{
    Q_UNUSED (pixmap);
    return true;
}

QSize
WallpaperBusinessLogic::sceneSize () const
{
    return QSize (480, 854);
}


void
WallpaperBusinessLogic::startEdit (
        WallpaperDescriptor desc)
{
    m_EditedImage = desc;
}

void
WallpaperBusinessLogic::endEdit ()
{
    m_EditedImage = WallpaperDescriptor();
}

WallpaperDescriptor 
WallpaperBusinessLogic::editedImage () const
{
    return m_EditedImage;
}

/******************************************************************************
 * Gallery connection.
 */
#ifdef HAVE_GALLERYCORE
void 
WallpaperBusinessLogic::galleryActivated ()
{
}

void 
WallpaperBusinessLogic::onGridPageItemSelected (
        QUrl    url)
{
    Q_UNUSED (url);
}

void 
WallpaperBusinessLogic::onGridPageCancelled ()
{
}

void 
WallpaperBusinessLogic::croppingDone (
        QImage croppedImage)
{
    Q_UNUSED (croppedImage);
}

void 
WallpaperBusinessLogic::croppingCancelled ()
{
}

void
WallpaperBusinessLogic::setProgressIndicator (bool busy)
{
    Q_UNUSED (busy);
}
#endif

