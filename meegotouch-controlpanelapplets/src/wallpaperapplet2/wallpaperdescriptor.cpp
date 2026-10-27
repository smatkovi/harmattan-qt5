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

#include "wallpaperdescriptor.h"
#include "wallpaperdescriptor_p.h"

#include <QUrl>
#include <QDir>

//#define DEBUG
#define WARNING
#include "../debug.h"

#define RETURN_VAL_IF_NULL(retval) \
    if (isNull()) { \
        SYS_WARNING ("WallpaperDescriptor is a null object."); \
        return (retval); \
    }

#define RETURN_IF_NULL \
    if (isNull()) { \
        SYS_WARNING ("WallpaperDescriptor is a null object."); \
        return; \
    }

/******************************************************************************
 * WallpaperDescriptor implementation: constructors and destructors.
 */
WallpaperDescriptor::WallpaperDescriptor(
        QObject         *parent) : 
    QObject (parent),
    m_Priv (0)
{
}

WallpaperDescriptor::WallpaperDescriptor(
        const QString  &filePath,
        QObject        *parent) : 
    QObject (parent),
    m_Priv (new WallpaperDescriptorPrivate (filePath))
{
}

WallpaperDescriptor::WallpaperDescriptor(
        const QUrl     &url,
        QObject        *parent) : 
    QObject (parent)
{
    QString filePath = url.path();

    SYS_WARNING ("*** filePath = '%s'", SYS_STR(filePath));

    filePath.replace ("%20", " ");

    m_Priv = new WallpaperDescriptorPrivate (filePath);
}

WallpaperDescriptor::WallpaperDescriptor (
        const WallpaperDescriptor &orig) :
    QObject ()
{
    m_Priv = orig.m_Priv;

    if (m_Priv) 
        m_Priv->ref ();    
}

WallpaperDescriptor::~WallpaperDescriptor ()
{
    if (m_Priv) {
        if (m_Priv->unRef() == 0)
            delete m_Priv;
    }

    m_Priv = 0;
}

WallpaperDescriptor &
WallpaperDescriptor::operator= (
         const WallpaperDescriptor &rhs)
{
    if (this == &rhs)
        return *this;

    if (m_Priv) {
        if (m_Priv->unRef() == 0)
            delete m_Priv;
    }

    m_Priv = rhs.m_Priv;
    if (m_Priv)
        m_Priv->ref ();

    return *this;
}

bool 
WallpaperDescriptor::operator< (
        const WallpaperDescriptor &desc) const
{
    bool retval;

    if (desc.isNull())
        retval = false;
    else if (isNull())
        retval = true;
    else if (desc.selected())
        retval = false;
    else if (selected())
        retval = true;
    else if (historyIndex() != -1 && desc.historyIndex() != -1)
        retval = historyIndex() < desc.historyIndex();
    else if (desc.historyIndex() != -1)
        retval = false;
    else if (historyIndex() != -1)
        retval = true;
    else if (m_Priv->timeStamp() < desc.m_Priv->timeStamp())
        return true;
    else if (m_Priv->timeStamp() > desc.m_Priv->timeStamp())
        return false;
    else
        retval = filePath() < desc.filePath();

    return retval;
}



bool 
WallpaperDescriptor::isNull() const
{
    return m_Priv == 0;
}


/******************************************************************************
 * Standard set/get methods.
 */
void
WallpaperDescriptor::setFilePath (
        const QString &filePath)
{
    RETURN_IF_NULL;

    m_Priv->setFilePath (filePath);
}

QString
WallpaperDescriptor::filePath () const
{
    RETURN_VAL_IF_NULL(QString());

    return m_Priv->filePath ();
}

void
WallpaperDescriptor::setOriginalFilePath (
        const QString &filePath)
{
    RETURN_IF_NULL;

    m_Priv->setOriginalFilePath (filePath);
}

QString
WallpaperDescriptor::originalFilePath () const
{
    RETURN_VAL_IF_NULL(QString());

    return m_Priv->originalFilePath ();
}

QUrl
WallpaperDescriptor::url () const
{
    QUrl    retval;
    QString path;

    RETURN_VAL_IF_NULL(retval);

    path = m_Priv->filePath ();
    if (path.startsWith(QDir::separator())) {
        path.prepend ("file://");
        path.replace (" ", "%20");
        retval.setEncodedUrl (path.toAscii(), QUrl::TolerantMode);
    }

    return retval;
}

static const char *
mimetypes[][2] = 
{
    { ".bmp",  "image/bmp"  },
    { ".gif",  "image/gif"  },
    { ".jpg",  "image/jpeg" },
    { ".jpeg", "image/jpeg" },
    { ".jpe",  "image/jpeg" },
    { ".tif",  "image/tiff" },
    { ".png",  "image/png"  },
    { NULL,   NULL}
};

QString
WallpaperDescriptor::mimeType () const
{
    QString retval;
    QString path;

    RETURN_VAL_IF_NULL(retval);

    path = m_Priv->filePath();
    if (path.isEmpty()) 
        goto finalize;

    for (int n = 0; mimetypes[n][0] != NULL; ++n) {
        if (path.endsWith(mimetypes[n][0], Qt::CaseInsensitive)) {
            retval = QString(mimetypes[n][1]);
            break;
        }
    }

finalize:
    return retval;
}

void
WallpaperDescriptor::setThumbnailPending (
        bool pending)
{
    RETURN_IF_NULL;
    m_Priv->setThumbnailPending (pending);
}

bool
WallpaperDescriptor::thumbnailPending () const
{
    RETURN_VAL_IF_NULL(false);

    return m_Priv->thumbnailPending ();
}
     
void 
WallpaperDescriptor::setThumbnail (
        const QPixmap &thumbnail)
{
    RETURN_IF_NULL;
    m_Priv->setThumbnail (thumbnail);
}

void
WallpaperDescriptor::unsetThumbnail ()
{
    RETURN_IF_NULL;
    m_Priv->unsetThumbnail ();
}

QPixmap
WallpaperDescriptor::thumbnail () const
{
    RETURN_VAL_IF_NULL(QPixmap());

    return m_Priv->thumbnail ();
}

bool
WallpaperDescriptor::hasThumbnail () const
{
    RETURN_VAL_IF_NULL(false);

    return m_Priv->hasThumbnail ();
}

void
WallpaperDescriptor::setSelected (
        bool selected)
{
    RETURN_IF_NULL;
    m_Priv->setSelected (selected);
}

bool 
WallpaperDescriptor::selected () const
{
    RETURN_VAL_IF_NULL(false);

    return m_Priv->selected ();
}

void
WallpaperDescriptor::setProgress (
        bool progress)
{
    RETURN_IF_NULL;
    m_Priv->setProgress (progress);
}

bool 
WallpaperDescriptor::progress () const
{
    RETURN_VAL_IF_NULL(false);

    return m_Priv->progress ();
}

void
WallpaperDescriptor::setHistoryIndex (
        int index)
{
    RETURN_IF_NULL;
    m_Priv->setHistoryIndex (index);
}

int 
WallpaperDescriptor::historyIndex () const
{
    RETURN_VAL_IF_NULL(-1);

    return m_Priv->historyIndex ();
}


/******************************************************************************
 * Loading...
 */
QuillImage
WallpaperDescriptor::load (
        const QSize     &expectedSize,
        QSize           &originalSize)
{
    RETURN_VAL_IF_NULL(QuillImage());

    return m_Priv->load (expectedSize, originalSize);
}

