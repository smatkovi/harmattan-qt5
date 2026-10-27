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
#include "wallpapercellcreator.h"

#include "wallpaperconfiguration.h"
#include "gridimagewidget.h"
#include "wallpapermodel.h"

#define DEBUG
#define WARNING
#include <../debug.h>

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
    GridImageWidget *imageWidget = qobject_cast<GridImageWidget *>(cell);
    QVariant         data = index.data(WallpaperModel::WallpaperDescriptorRole);
    WallpaperDescriptor desc = data.value<WallpaperDescriptor>();
  
    if (desc.hasThumbnail()) {
        QPixmap thumb = desc.thumbnail ();
        QSizeF  cSize = cellSize();
        
        imageWidget->setPixmap (
            thumb.scaled((int)cSize.width(), (int)cSize.height()));
        /*
         * It seems that this showAnimated() causes a flicker on the current
         * wallpaper when the applet list view is shown... well, we should
         * have a better solution to solve this, but this one works.
         */
        if (imageWidget->id() != desc.filePath() && !desc.selected())
            imageWidget->showAnimated();
        
        imageWidget->setID (desc.filePath());

        if (desc.thumbnailPending()) {
            desc.setThumbnailPending (false);
        }

    } else {
        /*
         * Resetting the cell thumbnail pixmap. We need this always, because 
         * the cells are re-used.
         */
        if (m_PlaceholderPixmap.size() != cellSize()) {
            QSizeF  cSize = cellSize();
            createPlaceholderPixmap ((int)cSize.width(), (int)cSize.height());
        }

        imageWidget->setPixmap (m_PlaceholderPixmap);
    }
    
    /*
     * The selection. 
     */
    imageWidget->setCurrent (desc.selected());
    imageWidget->setProgress (desc.progress());
}


void
WallpaperCellCreator::createPlaceholderPixmap (
        int width, 
        int height) const
{
    m_PlaceholderPixmap = QPixmap (width, height);
    m_PlaceholderPixmap.fill (QColor("black"));

    if (Wallpaper::showThumbnailGrid) {
        QPainter painter (&m_PlaceholderPixmap);
        QPen     pen (QBrush(), 2, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);

        pen.setColor ("Grey");
        painter.setPen (pen);
           
        const int mod    = pen.width() % 2;
        const int adjust = pen.width() / 2;
        QRect rect (0, 0, width, height);
        QRect frame = rect.adjusted(adjust,adjust,-(adjust+mod),-(adjust+mod));
        painter.drawRect (frame);
    }
}
