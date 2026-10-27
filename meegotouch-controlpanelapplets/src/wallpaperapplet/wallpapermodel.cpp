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

#include "gridimagewidget.h"

#include <QTimer>
#include <MImageWidget>
#include <MProgressIndicator>

#define DEBUG
#define WARNING
#include <../debug.h>

/*
 * The intentional delay between the loading of two picture files. This gives a
 * little chance for other applications.
 */
static const int loadPictureDelay = 0;

/******************************************************************************
 * WallpaperImageLoader implementation.
 */

/*!
 * When this slot is called the loading of the thumbnails in a specific
 * interval will be initated.
 */
void
WallpaperImageLoader::loadPictures (
            const QModelIndex& firstVisibleRow, 
            const QModelIndex& lastVisibleRow)
{
    int    from = firstVisibleRow.row();
    int    to = lastVisibleRow.row();

    SYS_DEBUG ("Between %d, %d - %d, %d", 
            firstVisibleRow.column(),
            firstVisibleRow.row(), 
            lastVisibleRow.column(),
            lastVisibleRow.row());

    for (int n = from; n <= to; ++n) {
        if (! firstVisibleRow.model ())
            break;
        //QModelIndex index(firstVisibleRow.sibling (n, 0));
        QModelIndex index = firstVisibleRow.model()->index(n, 0);
        if(!index.isValid())
            continue;

        QVariant data = index.data(WallpaperModel::WallpaperDescriptorRole);
        WallpaperDescriptor *desc = data.value<WallpaperDescriptor*>();
       
        SYS_DEBUG ("loaded = %s image = %s", 
                SYS_BOOL(desc->isThumbnailLoaded()),
                SYS_STR(desc->filename()));
        /*
         * We only create jobs for those items that has no thumbnail in the
         * memory yet.
         */
        if (!desc->isThumbnailLoaded()) {
            Job job;
            job.desc = desc;
            job.row = index;

            m_ThumbnailLoadingJobs << job;
        }
    }

    if (m_ThumbnailLoadingJobs.count() != 0)
        QTimer::singleShot(0, this, SLOT(processJobQueue()));
}

/*!
 * When this slot is called all the thumbnail loading jobs will be cancelled.
 * This slot should be called when (1) the panning on the widget has been
 * initiated and when (2) the widget became hidden. In these cases the loading
 * of the thumbnails will be initiated again when (1) the panning stops or 
 * (2) when the widgets shown again.
 */
void 
WallpaperImageLoader::stopLoadingPictures()
{
    SYS_DEBUG ("");
    m_ThumbnailLoadingJobs.clear();
}

/*!
 * Processes one thumbnailing job and then calls itself to process the next 
 * one. While processing the job this method will only initate the thumbnailing,
 * it will not wait until the actual thumbnail is ready.
 */
void 
WallpaperImageLoader::processJobQueue ()
{
    SYS_DEBUG ("jobs = %d", m_ThumbnailLoadingJobs.size());
    if (m_ThumbnailLoadingJobs.isEmpty())
        return;

    //SYS_DEBUG ("Initiating Thumbnailer");
    Job job = m_ThumbnailLoadingJobs.takeFirst();
    job.desc->initiateThumbnailer ();

    // Continue loading and letting UI thread do something
    if(m_ThumbnailLoadingJobs.count() > 0)
        QTimer::singleShot(loadPictureDelay, this, SLOT(processJobQueue()));
}

/******************************************************************************
 * WallpaperContentItemCreator implementation.
 */
MWidget *
WallpaperCellCreator::createCell (
        const QModelIndex &index, 
        MWidgetRecycler   &recycler) const
{
    GridImageWidget *cell;

    cell = qobject_cast <GridImageWidget *> (
            recycler.take(GridImageWidget::staticMetaObject.className()));

    if (!cell) {
        cell = new GridImageWidget ();
    }

    updateCell(index, cell);

    return cell;
}

void 
WallpaperCellCreator::setCellSize (
        const QSizeF &size)
{
    m_CellSize = size;
}

QSizeF 
WallpaperCellCreator::cellSize() const
{
    return m_CellSize;
}

void 
WallpaperCellCreator::updateCell (
        const QModelIndex &index, 
        MWidget           *cell) const
{
    static QPixmap placeholderPixmap;

    GridImageWidget *imageWidget = qobject_cast<GridImageWidget *>(cell);
    QVariant data = index.data(WallpaperModel::WallpaperDescriptorRole);
    WallpaperDescriptor *desc = data.value<WallpaperDescriptor *>();
  
    if (!imageWidget || !desc)
        return;


    if (desc->isThumbnailLoaded()) {
        QPixmap thumb = desc->thumbnailPixmap();
        QSizeF  cSize = cellSize();
        imageWidget->setPixmap (
                thumb.scaled(
                    (int)cSize.width(), (int)cSize.height()));
    } else {
        if (placeholderPixmap.size() != cellSize()) {
            QSizeF  cSize = cellSize();

            placeholderPixmap = QPixmap(
                    (int)cSize.width(), (int)cSize.height());
            placeholderPixmap.fill (QColor("black"));
        }

        imageWidget->setPixmap (placeholderPixmap);
    }

    // The spinner.
    if (desc->loading()) {
        imageWidget->progressIndicator(true)->show();
    } else {
        MProgressIndicator *indicator;

        indicator = imageWidget->progressIndicator(false);
        if (indicator)
            indicator->hide();
    }
    
    // The selection. 
    imageWidget->setCurrent (desc->isCurrent());
    //imageWidget->setSelected (true);
}

/******************************************************************************
 * WallpaperModel implementation.
 */

WallpaperModel::WallpaperModel (
        WallpaperBusinessLogic *logic,
        QObject                *parent) :
    QAbstractTableModel (parent),
    m_BusinessLogic (logic)
{
    Q_ASSERT (logic != 0);
    
    /*
     * FIXME: Maybe we should delay this?
     */
    m_DescriptorList = logic->availableWallpapers();
    
    for (int n = 0; n < m_DescriptorList.size(); ++n) {
        connect (m_DescriptorList[n], SIGNAL (changed (WallpaperDescriptor *)),
                this, SLOT(descriptorChanged (WallpaperDescriptor *)));
    }

    connect (logic, SIGNAL(wallpaperChanged()),
            this, SLOT(wallpaperChanged()));
    connect (logic, SIGNAL(fileListChanged()),
            this, SLOT(fileListChanged()));
}

WallpaperModel::~WallpaperModel ()
{
    for (int n = 0; n < m_DescriptorList.size(); ++n) {
        if (!m_DescriptorList[n]->isCurrent())
            delete m_DescriptorList[n];
    }
}

int 
WallpaperModel::rowCount(
		const QModelIndex &parent) const
{
    Q_UNUSED (parent);
    
    return m_DescriptorList.size();
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
    QVariant             var;

    Q_UNUSED (role);
    Q_UNUSED (index);

    Q_ASSERT (index.row() >= 0);
    Q_ASSERT (index.row() < m_DescriptorList.size());

    switch (role) {
        case Qt::DisplayRole:
            /*
             * We use the base-name when sorting the images, the full path would
             * make the current image at the fix position because the current
             * image in a separate directory.
             */
            var.setValue (m_DescriptorList[index.row()]->basename());
            break;

        case WallpaperModel::WallpaperDescriptorRole:
            var.setValue (m_DescriptorList[index.row()]);
            break;

        default:
            var.setValue (QString("Unsupported role"));
    }

    return var;
}

int 
WallpaperModel::columnCount (
        const QModelIndex&) const
{
    return 1;
}


/*!
 * This slot is called when the wallpaper descriptor has been changed. 
 */
void 
WallpaperModel::descriptorChanged (
        WallpaperDescriptor *desc)
{
    for (int n = 0; n < m_DescriptorList.size(); ++n) {
        if (m_DescriptorList[n] == desc) {
            QModelIndex first;
            first = index (n, 0);
            emit dataChanged (first, first);

            break;
        }
    }
}

/*
 * This slot is activated when the wallpaper has been changed. We emit a
 * dataChanged() signal here, so the UI can sense that and refresh itself.
 */
void 
WallpaperModel::wallpaperChanged ()
{
    /*
     * The current wallpaper is always the first (no matter where it is shown in
     * the list), we need to refresh that.
     */
    QModelIndex first = index (0, 0);
    emit dataChanged (first, first);
}

void 
WallpaperModel::fileListChanged ()
{
    SYS_DEBUG ("");
    m_DescriptorList = m_BusinessLogic->availableWallpapers();
    for (int n = 0; n < m_DescriptorList.size(); ++n) {
        connect (m_DescriptorList[n], SIGNAL (changed (WallpaperDescriptor *)),
                this, SLOT(descriptorChanged (WallpaperDescriptor *)));
    }

    reset();
}
