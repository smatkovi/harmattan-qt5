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
#include "wallpapermodel.h"

#include "wallpaperbusinesslogic.h"
#include "wallpaperdescriptor.h"

#define DEBUG
#define WARNING
#include <../debug.h>

#define BG_PATH "/usr/share/backgrounds/"
static const QString bg1 (BG_PATH "bg1.png");
static const QString bg2 (BG_PATH "bg2.png");
static const QString bg3 (BG_PATH "bg3.png");

static const QString bgselected (bg1);

/******************************************************************************
 * WallpaperModel implementation.
 */
WallpaperModel::WallpaperModel (
        WallpaperBusinessLogic *logic,
        QObject                *parent) :
    QAbstractTableModel (parent),
    m_BusinessLogic (logic),
    m_ThumbnailMagicNumber (1),
    m_OrderDirty (false)
{
    loadFromDirectory ();

    ensureSelection ();
}

WallpaperModel::~WallpaperModel ()
{
}

int 
WallpaperModel::rowCount(
		const QModelIndex &parent) const
{
    Q_UNUSED (parent);
    
    return m_FilePathList.size();
}

/*!
 * Returns the data from a given row in the specified format. The supported
 * roles are:
 *   Qt::DisplayRole: The title of the wallpaper is returned.
 *   otherwise: the WallpaperDescriptor object pointer is returned.
 */
QVariant
WallpaperModel::data (
        const QModelIndex &index, 
        int                role) const
{
    QVariant             retval;

    Q_ASSERT (index.row() >= 0);
    Q_ASSERT (index.row() < m_FilePathList.size());

    switch (role) {
        case Qt::DisplayRole:
            if (m_OrderDirty)
                mySort ();
            /*
             * We use this role to sort the wallpapers in the list widget.
             */
            SYS_DEBUG ("Qt::DisplayRole");
            if (!m_FilePathHash.contains(m_FilePathList[index.row()])) {
                SYS_WARNING ("MISSING DESCRIPTOR AT %d: %s", index.row(),
                        SYS_STR(m_FilePathList[index.row()]));
            }
            
            retval.setValue (m_OrderHash[m_FilePathList[index.row()]]);
            break;

        case WallpaperModel::WallpaperDescriptorRole:
            SYS_DEBUG ("WallpaperModel::WallpaperDescriptorRole");
            if (!m_FilePathHash.contains(m_FilePathList[index.row()])) {
                SYS_WARNING ("MISSING DESCRIPTOR AT %d: %s", index.row(),
                        SYS_STR(m_FilePathList[index.row()]));
            }
            retval.setValue (m_FilePathHash[m_FilePathList[index.row()]]);
            break;

        default:
            SYS_WARNING ("Unsupported role");
            retval.setValue (QString("Unsupported role"));
    }

    SYS_DEBUG ("Returning %s", retval.typeName());
    return retval;
}

int 
WallpaperModel::columnCount (
        const QModelIndex&) const
{
    return 1;
}

/*
 * This slot is activated when the wallpaper has been changed. We emit a
 * dataChanged() signal here, so the UI can sense that and refresh itself.
 */
void 
WallpaperModel::wallpaperChanged ()
{
}

void 
WallpaperModel::loadFromDirectory ()
{
    if (! m_FilePathList.isEmpty ())
        return;

    m_FilePathList <<
        bg1 <<
        bg2 <<
        bg3;

    QModelIndex  parent;
    /*
     * Inserting the new items.
     */
    beginInsertRows (parent, 0, m_FilePathList.size() - 1);

    foreach (QString newPath, m_FilePathList) {
        WallpaperDescriptor desc (newPath);

        m_OrderDirty = true;
        m_FilePathHash[newPath] = desc;
    }

    endInsertRows ();
}

/******************************************************************************
 * Methods to handle thumbnails.
 */
/*!
 * This slot is called when the thumbnail has been generated.
 */
void 
WallpaperModel::thumbnailReady (
            QUrl         fileUri, 
            QUrl         thumbnailUri, 
            QPixmap      pixmap, 
            QString      flavor)
{
    Q_UNUSED (flavor);
    Q_UNUSED (fileUri);
    Q_UNUSED (thumbnailUri);
    Q_UNUSED (pixmap);
}

/*!
 * This slot is called when the thumbler has been detected some error. We could
 * do something, but until we find some smart thing we can do we just print an
 * error message.
 */
void
WallpaperModel::thumbnailError (
            QString      message,
            QUrl         fileUri)
{
    Q_UNUSED (message);
    Q_UNUSED (fileUri);
}

/*!
 * Called when the generation of the thumbnail is finished. This is the point
 * where we can destroy the thumbler object.
 */
void 
WallpaperModel::thumbnailLoadingFinished (
            int          left)
{
    SYS_DEBUG ("*** left = %d", left);
    Q_UNUSED (left);
}


void
WallpaperModel::loadThumbnails (
            const QModelIndex& firstVisibleRow, 
            const QModelIndex& lastVisibleRow)
{
    Q_UNUSED (firstVisibleRow);
    Q_UNUSED (lastVisibleRow);
}

void
WallpaperModel::stopLoadingThumbnails ()
{
}

/******************************************************************************
 * Methods to handle selection and current wallpaper. 
 */
QString
WallpaperModel::selectedPath () const
{
    foreach (WallpaperDescriptor desc, m_FilePathHash) {
        if (desc.selected())
            return desc.filePath();
    }

    return QString();
}

void 
WallpaperModel::ensureSelection ()
{
    trySelect (bg1);
}

bool 
WallpaperModel::trySelect (
        const QString filePath)
{
    bool retval = false;

    SYS_DEBUG ("*** filePath = '%s'", SYS_STR(filePath));
    if (filePath.isEmpty())
        goto finalize;
   
    if (!m_FilePathList.contains(filePath))
        goto finalize;

    retval = true; 

finalize:
    if (retval)
        selectByFilepath (filePath);

    SYS_DEBUG ("Returning %s", SYS_BOOL(retval));
    return retval;
}

bool 
WallpaperModel::tryAddAndSelect (
        const QString filePath)
{
    Q_UNUSED (filePath);
    return false;
}

/*!
 * \returns true if the item with the given path is indeed found
 *
 * Selects the item given by its file path and deselects all the other items.
 * Emits a signal for every changed item, so the widget will re-draw the parts
 * that needed to be updated.
 */
bool
WallpaperModel::selectByFilepath (
        const QString &filePath)
{
    bool retval = false;

    SYS_DEBUG ("*** filePath = '%s'", SYS_STR(filePath));

    for (int n = 0; n < m_FilePathList.size(); ++n) {
        QString              thisPath = m_FilePathList[n];
        WallpaperDescriptor  desc = m_FilePathHash[thisPath];
        bool                 selected = desc.selected();
        bool                 changed = false;
        bool                 thisIsSelected;

        if (selected && thisPath != filePath) {
            changed = true;
            thisIsSelected = false;
        } else if (!selected && thisPath == filePath) {
            thisIsSelected = true;
            changed = true;
            retval = true;
        }

        if (changed) {
            QModelIndex          first;

            desc.setSelected (thisIsSelected);
            first = index (n, 0);
            SYS_DEBUG ("emit dataChanged(%d, %d)", n, n);
            if (!thisIsSelected)
                emit dataChanged (first, first);
        }
    }

    //if (retval)
        QTimer::singleShot(100, this, SLOT(emitCurrentChanged()));
    
    return retval;
}

/******************************************************************************
 * File-system watcher and USB mode methods.
 */
#ifdef HAVE_QMSYSTEM
void 
WallpaperModel::usbModeChanged (
        MeeGo::QmUSBMode::Mode mode)
{
    Q_UNUSED (mode);
}
#endif

void 	
WallpaperModel::directoryChanged (
        const QString  &path)
{
    Q_UNUSED (path);
}

void
WallpaperModel::fileTimerTimeout ()
{
}

void
WallpaperModel::fileChanged (
        const QString  &path)
{
    Q_UNUSED (path);
}

void
WallpaperModel::startWatchFiles ()
{
}

/******************************************************************************
 * Low level private methods.
 */
void
WallpaperModel::emitChangedSignal (
        int    idx)
{
    QModelIndex         first;
    first = index (idx, 0);

    SYS_DEBUG ("emit dataChanged (%d, %d);", idx, idx);
    emit dataChanged (first, first);
}

void
WallpaperModel::emitCurrentChanged ()
{
    QModelIndex index;

    SYS_WARNING ("Emitting currentChanged()");

    index = currentIndex();
    emit dataChanged (index, index);
    emit currentChanged (index);
}


QModelIndex
WallpaperModel::currentIndex () 
{
    QModelIndex retval;

    for (int n = 0; n < m_FilePathList.size(); ++n) {
        WallpaperDescriptor  desc = m_FilePathHash[m_FilePathList[n]];
        bool                 selected = desc.selected();

        if (!selected)
            continue;

        retval = index(n, 0);
        break;
    }

    return retval;
}
        
void
WallpaperModel::showProgressByFilepath (
        const QString  &path)
{
    Q_UNUSED (path);
}

void
WallpaperModel::hideProgress ()
{
}

int
WallpaperModel::nThumbnails () const
{
    return m_FilePathList.count ();
}

bool
WallpaperModel::appendThumbnailRequest (
        QList<QUrl>          &uris,
        QStringList          &requestedFiles,
        QStringList          &mimeTypes,
        WallpaperDescriptor  &desc)
{
    Q_UNUSED (uris);
    Q_UNUSED (requestedFiles);
    Q_UNUSED (mimeTypes);
    Q_UNUSED (desc);

    return true;
}

void
WallpaperModel::sneakyFileChange (
        const QString &filePath)
{
    Q_UNUSED (filePath);
}

void 
WallpaperModel::mySort () const
{
    QList<WallpaperDescriptor> list;
    QStringList                history = m_BusinessLogic->history();

    if (!m_OrderDirty) {
        SYS_WARNING ("This method should not called when the order is not "
                "dirty.");
        return;
    }
    
    foreach (WallpaperDescriptor desc, m_FilePathHash) {
        desc.setHistoryIndex (history.indexOf(desc.filePath()));
        list << desc;
    }

    qSort (list.begin(), list.end());

    m_OrderHash.clear();
    for (int n = 0; n < list.size(); ++n)
        m_OrderHash[list[n].filePath()] = n;

    m_OrderDirty = false;
}

