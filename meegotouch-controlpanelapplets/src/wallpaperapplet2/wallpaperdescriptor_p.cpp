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
#include "wallpaperdescriptor_p.h"
#include "wallpaperutils.h"
#include "wallpaperconfiguration.h"

#define DEBUG
#define WARNING
#include "../debug.h"

WallpaperDescriptorPrivate::WallpaperDescriptorPrivate () :
    m_ReferenceCounter (1),
    m_Selected (false),
    m_Progress (false),
    m_ThumbnailPending (false),
    m_HasThumbnail (false),
    m_HistoryIndex (-1)
{
}

WallpaperDescriptorPrivate::WallpaperDescriptorPrivate (
        const QString &filePath) :
    m_ReferenceCounter (1),
    m_FilePath (filePath),
    m_Selected (false),
    m_Progress (false),
    m_ThumbnailPending (false),
    m_HasThumbnail (false),
    m_HistoryIndex (-1)
{
    m_TimeStamp = Wallpaper::fileTimeStamp (filePath);
}


void
WallpaperDescriptorPrivate::ref ()
{
    ++m_ReferenceCounter;
}

int 
WallpaperDescriptorPrivate::unRef ()
{
    return --m_ReferenceCounter;
}

/******************************************************************************
 * Standard set/get methods.
 */
void 
WallpaperDescriptorPrivate::setFilePath (
        const QString &filePath)
{
    m_FilePath = filePath;
    m_TimeStamp = Wallpaper::fileTimeStamp (filePath);
}
        
QString 
WallpaperDescriptorPrivate::filePath () const
{
    return m_FilePath;
}

void 
WallpaperDescriptorPrivate::setOriginalFilePath (
        const QString &filePath)
{
    m_OriginalFilePath = filePath;
}
        
QString 
WallpaperDescriptorPrivate::originalFilePath () const
{
    return m_OriginalFilePath;
}

void
WallpaperDescriptorPrivate::setThumbnailPending (
        bool pending)
{
    m_ThumbnailPending = pending;
}

bool
WallpaperDescriptorPrivate::thumbnailPending () const
{
    return m_ThumbnailPending;
}

void 
WallpaperDescriptorPrivate::setThumbnail (
        const QPixmap &thumbnail)
{
    m_HasThumbnail = true;
    m_Thumbnail = thumbnail;
}

void
WallpaperDescriptorPrivate::unsetThumbnail ()
{
    m_HasThumbnail = false;
    m_ThumbnailPending = false;
    m_Thumbnail = QPixmap();
}

QPixmap
WallpaperDescriptorPrivate::thumbnail () const
{
    return m_Thumbnail;
}

bool
WallpaperDescriptorPrivate::hasThumbnail () const
{
    return m_HasThumbnail;
}

void
WallpaperDescriptorPrivate::setSelected (
        bool selected)
{
    m_Selected = selected;
}

bool 
WallpaperDescriptorPrivate::selected () const
{
    return m_Selected;
}

void
WallpaperDescriptorPrivate::setProgress (
        bool progress)
{
    m_Progress = progress;
}

bool 
WallpaperDescriptorPrivate::progress () const
{
    return m_Progress;
}

void
WallpaperDescriptorPrivate::setHistoryIndex (
        int   index)
{
    m_HistoryIndex = index;
}

int 
WallpaperDescriptorPrivate::historyIndex () const
{
    return m_HistoryIndex;
}

QDateTime 
WallpaperDescriptorPrivate::timeStamp () const
{
    return m_TimeStamp;
}


/******************************************************************************
 * Loading...
 */
QuillImage
WallpaperDescriptorPrivate::load (
        const QSize    &expectedSize,
        QSize          &originalSize)
{
    QuillImage        retval;
    QSize             mySize;
    QuillImageFilter *loadFilter;
    QString           fileName;

    if (!m_OriginalFilePath.isEmpty() &&
            Wallpaper::imageFile(m_OriginalFilePath))
        fileName = m_OriginalFilePath;
    else
        fileName = m_FilePath;

    loadFilter = QuillImageFilterFactory::createImageFilter(
            QuillImageFilter::Role_Load);
    loadFilter->setOption(
            QuillImageFilter::FileName,
            QVariant(fileName));

    mySize = loadFilter->newFullImageSize(QSize());
    
    originalSize = mySize;
   
    SYS_DEBUG ("*** expectedSize = %s", SYS_SIZE(expectedSize));
    SYS_DEBUG ("*** mySize       = %s", SYS_SIZE(mySize));

    if (Wallpaper::disableBiggerThanScreen || 
            Wallpaper::smallerSize(mySize, expectedSize)) {
        mySize.scale (expectedSize, Qt::KeepAspectRatio);
        SYS_DEBUG ("*** chosen small = %s", SYS_SIZE(mySize));
    } else {
        mySize.scale (expectedSize, Qt::KeepAspectRatioByExpanding);
        SYS_DEBUG ("*** chosen big   = %s", SYS_SIZE(mySize));
    }

    retval = loadFilter->apply (QImage(mySize, QImage::Format_RGB16));
    delete loadFilter;

    return retval;
}

