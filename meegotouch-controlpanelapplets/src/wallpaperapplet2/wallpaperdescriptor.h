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
#ifndef WALLPAPERDESCRIPTOR_H
#define WALLPAPERDESCRIPTOR_H

#include <QObject>
#include <QPointer>
#include <QMetaType>
#include <QPixmap>
#include <QUrl>
#include <QVector>
#include <MApplication>
#include <meegocontrolexport.h>
#include <QuillImage>

class QString;
class WallpaperDescriptorPrivate;
class QSize;

/*!
 * This class is used to represent a potential wallpaper image with all its
 * attributes. 
 */
class MC_EXPORT WallpaperDescriptor : public QObject 
{
    Q_OBJECT

    public:
        WallpaperDescriptor (
                QObject        *parent = 0);

        WallpaperDescriptor (
                const QString  &filePath,
                QObject        *parent = 0);

        WallpaperDescriptor (
                const QUrl     &url,
                QObject        *parent = 0);

        WallpaperDescriptor (
                const WallpaperDescriptor &orig);

        ~WallpaperDescriptor ();

        /*
         * Operators and operator like methods.
         */
        WallpaperDescriptor &operator= (
                 const WallpaperDescriptor &rhs);

        bool operator< (const WallpaperDescriptor &desc) const;

        bool isNull() const;

        /*
         * Standard set/get methods.
         */
        void setFilePath (const QString &filePath);
        QString filePath () const;

        void setOriginalFilePath (const QString &filePath);
        QString originalFilePath () const;
        
        QUrl url () const;
        QString mimeType () const;

        void setThumbnailPending (bool pending = true);
        bool thumbnailPending () const;

        void setThumbnail (const QPixmap &thumbnail);
        void unsetThumbnail ();
        QPixmap thumbnail () const;
        bool hasThumbnail () const;

        void setSelected (bool selected = true);
        bool selected () const;
        
        void setProgress (bool progress = true);
        bool progress () const;

        void setHistoryIndex (int index);
        int historyIndex () const;
        
        /*
         * Loading...
         */
        QuillImage load (
                const QSize &expectedSize, 
                QSize       &originalSize);

    private:
        WallpaperDescriptorPrivate *m_Priv;
};

Q_DECLARE_METATYPE(WallpaperDescriptor)

#endif

