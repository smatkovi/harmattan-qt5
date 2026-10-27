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
#ifndef WALLPAPERMODEL_H
#define WALLPAPERMODEL_H

#include <QPointer>
#include <QList>
#include <QHash>
#include <QAbstractTableModel>
#include <Thumbnailer>
using namespace Thumbnails;

#include <QFileSystemWatcher>

#include <MAbstractCellCreator>
#include "wallpaperdescriptor.h"
#include <MImageWidget> 
#include <QSizeF>
#include <QTimer>

#ifdef HAVE_QMSYSTEM
#  include <qmusbmode.h>
using namespace MeeGo;
#endif

class WallpaperBusinessLogic;
class WallpaperDescriptor;


class WallpaperModel: public QAbstractTableModel
{
    Q_OBJECT

    public:
        typedef enum {
            WallpaperDescriptorRole = Qt::UserRole,
        } WallpaperModelRole;

        WallpaperModel (
                WallpaperBusinessLogic *logic,
                QObject                *parent = 0);
        
        ~WallpaperModel ();
    
        /*
         * Methods for accessing data.
         */
        virtual int rowCount (
                const QModelIndex &parent = QModelIndex()) const;
        virtual QVariant data (
                const QModelIndex &index, 
                int role = Qt::DisplayRole) const;
        virtual int columnCount (const QModelIndex&) const;
       
        /*
         * Thumbnail handling.
         */
        void loadThumbnails (
                const QModelIndex& firstVisibleRow, 
                const QModelIndex& lastVisibleRow);
        void stopLoadingThumbnails ();

        /*
         *
         */
        QModelIndex currentIndex ();

        void showProgressByFilepath (const QString  &path);
        
    signals:
        void currentChanged (const QModelIndex &current);

    private slots:
        /*
         * Business-logic connections.
         */
        void wallpaperChanged ();

        /*
         * Slots for the thumbnailer connection.
         */
        void thumbnailLoadingFinished (
                int          left);

        void thumbnailError (
                QString      message,
                QUrl         fileUri);

        void thumbnailReady (
                QUrl         fileUri, 
                QUrl         thumbnailUri, 
                QPixmap      pixmap, 
                QString      flavor);

        /*
         * Watching the file-system.
         */
        void directoryChanged (const QString &path);
        void fileChanged (const QString &path);
        void fileTimerTimeout ();

        #ifdef HAVE_QMSYSTEM
        void usbModeChanged (MeeGo::QmUSBMode::Mode mode);
        #endif
        
        void loadFromDirectory ();

        void emitCurrentChanged ();
        
        void hideProgress ();

    protected:
        void ensureSelection ();
        void startWatchFiles ();

    private:
        QString selectedPath () const;
        bool trySelect (const QString filePath);
        bool tryAddAndSelect (const QString filePath);
        bool selectByFilepath (const QString &filepath);
        void emitChangedSignal (int idx);
        int  nThumbnails () const;
        bool appendThumbnailRequest (
                QList<QUrl>          &uris,
                QStringList          &requestedFiles,
                QStringList          &mimeTypes,
                WallpaperDescriptor  &desc);

        void sneakyFileChange (const QString &filePath);
        void mySort () const;

    private:
        WallpaperBusinessLogic                 *m_BusinessLogic;
        QStringList                             m_FilePathList;
        QHash<QString, WallpaperDescriptor>     m_FilePathHash; 
        QString                                 m_SysImagesDir;
        QString                                 m_ImagesDir;
        QPointer<Thumbnailer>                   m_Thumbnailer;
        QPointer<QFileSystemWatcher>            m_FileWatcher;
        int                                     m_ThumbnailMagicNumber;
        QHash<QString, qint64>                  m_PendingFiles;
        QTimer                                  m_FileSystemTimer;
        QTimer                                  m_FileTimer;
        QStringList                             m_ChangedFiles;
        mutable bool                            m_OrderDirty;
        mutable QHash<QString, int>             m_OrderHash;
        #ifdef HAVE_QMSYSTEM
        MeeGo::QmUSBMode                       *m_UsbMode;
        #endif
        
        friend class Ut_WallpaperModel;
};


#endif
