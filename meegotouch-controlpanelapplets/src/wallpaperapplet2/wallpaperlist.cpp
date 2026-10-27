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
#include "wallpaperlist.h"

#include <QTimer>
#include <MOrientationChangeEvent>
#include <MApplication>
#include <MApplicationWindow>
#include <MSortFilterProxyModel>
#include <MListFilter>

#include "wallpaperconfiguration.h"
#include "wallpaperbusinesslogic.h"
#include "wallpapermodel.h"
#include "wallpaperdescriptor.h"
#include "wallpapercellcreator.h"


#define DEBUG
#define WARNING
#include "../debug.h"

static const int loadPicturesDelay = 10;

static int columnsLandscape = 5;
static int columnsPortrait = 3;

#include <MWidgetCreator>
M_REGISTER_WIDGET_NO_CREATE(WallpaperList)

WallpaperList::WallpaperList (
        WallpaperBusinessLogic *logic,
        QGraphicsItem          *parent) :
    MList (parent),
    m_BusinessLogic (logic),
    m_Model (0),
    m_CellCreator (new WallpaperCellCreator),
    m_DataSourceType (WallpaperList::DataSourceUnknown),
    m_Panning (false)
{
    connect (this, SIGNAL(itemClicked(const QModelIndex &)),
            this, SLOT(slotItemClicked(const QModelIndex &)));
    /*
     * When the panning stops we load thumbnail images, when it starts we stop
     * doing the image loading so the panning will be smooth.
     */
    connect (this, SIGNAL(panningStarted()), 
            this, SLOT(slotPanningStarted()));
    connect (this, SIGNAL(panningStopped()), 
            this, SLOT(slotPanningStopped()));
    connect (m_BusinessLogic, SIGNAL(wallpaperChanged()), 
            this, SLOT(loadPictures()));

    MApplicationWindow *window;
    int                 columns;
    qreal               width, height;


    // The return value for geometry() is not right.
    //width = geometry().width() / columns;
    window = MApplication::activeApplicationWindow();
    if (window) {
        qreal  aspectRatio;

        columns = window->orientation() == M::Landscape ?
            columnsLandscape : columnsPortrait;
        aspectRatio = 
            (qreal) window->visibleSceneSize().height() / 
            (qreal) window->visibleSceneSize().width();
        width = window->visibleSceneSize().width() / columns;
        height = aspectRatio * width;
    
        m_CellCreator->setCellSize (QSizeF(width, width/*height*/));
        setColumns (columns);
    } 

    #if 0
    SYS_DEBUG ("*** geometry().width()  = %g", geometry().width());
    SYS_DEBUG ("*** geometry().height() = %g", geometry().height());
    SYS_DEBUG ("*** columns             = %d", columns);
    SYS_DEBUG ("*** width               = %g", width);
    #endif
}

WallpaperList::~WallpaperList()
{
    /*
     * The user might press the back button before the image is loaded in the
     * other thread...
     */
    m_BusinessLogic->endEdit ();
}

/*
 * If we got new pictures we might need to get their thumbnails.
 */
void
WallpaperList::rowsInserted (
        const QModelIndex &parent, 
        int                start, 
        int                end)
{
    Q_UNUSED (parent);

    SYS_DEBUG ("Added from %d to %d", start, end);
    filtering()->proxy()->sort(Qt::DisplayRole);
    QTimer::singleShot (loadPicturesDelay, this, SLOT(loadPictures()));
}

/*!
 * Well, if we drop images from the top of the list then we might have images
 * coming into the screen with no thumbnails downloaded.
 */
void
WallpaperList::rowsRemoved (
        const QModelIndex &parent, 
        int                start, 
        int                end)
{
    Q_UNUSED (parent);

    SYS_DEBUG ("Added from %d to %d", start, end);
    QTimer::singleShot (loadPicturesDelay, this, SLOT(loadPictures()));
}
        
void 
WallpaperList::currentChanged (
        const QModelIndex &current)
{
    SYS_WARNING ("-------------------------------------------------------");
    if (Wallpaper::currentWallpaperAtTop) {
        QModelIndex first = filtering()->proxy()->index (0, 0);

        SYS_DEBUG ("Calling sort...");
        filtering()->proxy()->sort(Qt::DisplayRole);
        scrollTo (first, EnsureVisibleHint, Animated);
    } else {
        QModelIndex myIndex = filtering()->proxy()->mapFromSource (current);
        QModelIndex first =	firstVisibleItem ();
        QModelIndex last  = lastVisibleItem ();

        SYS_DEBUG ("%d -> %d", current.row(), myIndex.row());

        /*
         * Workaround for unnecessary scrolling problem in MList. We don't want
         * to scroll if it is not necessary, especially when the list just
         * appeared.
         */
        if (myIndex.row() < 10 && 
                myIndex.row() >= first.row() && myIndex.row() <= last.row()) {
            SYS_DEBUG ("We are in.");
        } else {
            scrollTo (myIndex, EnsureVisibleHint, Animated);
        }
    }

    /*
     * It is possible that the scrolling request is not going to actually
     * scroll, so we need to load the pictures "manually".
     */
    QTimer::singleShot (loadPicturesDelay, this, SLOT(loadPictures()));
}

void
WallpaperList::setDataSourceType (
        WallpaperList::DataSourceType sourceType)
{
    Q_ASSERT (m_DataSourceType == DataSourceUnknown);
    
    m_Model = new WallpaperModel (m_BusinessLogic, this);
    setItemModel (m_Model);
    setCellCreator (m_CellCreator);

    m_DataSourceType = sourceType;

    /*
     * Enabling the filtering will create a proxy model that is used to sort the
     * items. The filtering is not used in this applet.
     */
    filtering()->setEnabled (true);
    filtering()->proxy()->sort(Qt::DisplayRole);

    QTimer::singleShot (loadPicturesDelay, this, SLOT(loadPictures()));
    #if 0
    connect (m_Model, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(dataChanged(const QModelIndex &, const QModelIndex &)));
    #endif
    connect (m_Model, SIGNAL(rowsInserted(const QModelIndex &, int, int)),
            this, SLOT(rowsInserted(const QModelIndex &, int, int)));
    connect (m_Model, SIGNAL(rowsRemoved(const QModelIndex &, int, int)),
            this, SLOT(rowsRemoved(const QModelIndex &, int, int)));
    connect (m_Model, SIGNAL(currentChanged(const QModelIndex &)),
            this, SLOT(currentChanged(const QModelIndex &)));

    //currentChanged (m_Model->currentIndex());
}

/******************************************************************************
 * Item clicked, panning started and stopped.
 */
void 
WallpaperList::slotItemClicked (
        const QModelIndex &index)
{
    /*
     * Funy thing, sometimes we get this signal while the list is panning. It is
     * very annoying for the user, so we just filter these signals here.
     */
    if (m_Panning) {
        SYS_WARNING ("+++ Dropping the event, we are panning.");
        return;
    }

    QVariant data = index.data(WallpaperModel::WallpaperDescriptorRole);
    WallpaperDescriptor desc = data.value<WallpaperDescriptor>();

    SYS_DEBUG ("***********************************************************");
    SYS_DEBUG ("*** filename          = %s", SYS_STR(desc.filePath()));
    SYS_DEBUG ("*** thumbnailPending  = %s", SYS_BOOL(desc.thumbnailPending()));
    SYS_DEBUG ("*** hasThumbnail      = %s", SYS_BOOL(desc.hasThumbnail()));
    SYS_DEBUG ("*** thumbnail         = %dx%d", 
            desc.thumbnail().width(), desc.thumbnail().height());

    if (!m_BusinessLogic->hasEditedImage()) {
        m_Model->showProgressByFilepath (desc.filePath());
        emit imageActivated(desc);
    }
}

void 
WallpaperList::slotPanningStarted ()
{
    m_Panning = true;
    //QTimer::singleShot (loadPicturesDelay, this, SLOT(stopLoadingPictures()));
    stopLoadingPictures ();
}

void 
WallpaperList::slotPanningStopped ()
{
    m_Panning = false;
    //QTimer::singleShot (loadPicturesDelay, this, SLOT(loadPictures()));
    loadPictures ();
}


void 
WallpaperList::loadPictures ()
{
    /*
     * We used to get panningStopped() signals when we got hidden, so we will
     * not initiate loading of the images when we are not visible.
     */
    if (!isVisible() || !m_Model)
        return;

    SYS_DEBUG ("from %d, %d to %d, %d",
            firstVisibleItem().column(),
            firstVisibleItem().row(),
            lastVisibleItem().column(),
            lastVisibleItem().row());
    m_Model->loadThumbnails (firstVisibleItem(), lastVisibleItem());
}

void
WallpaperList::stopLoadingPictures ()
{
    if (!m_Model)
        return;

    m_Model->stopLoadingThumbnails ();
}

void 
WallpaperList::orientationChangeEvent (
        MOrientationChangeEvent *event)
{
    MApplicationWindow *window;
    int                 columns;
    qreal               width, height;

    columns = event->orientation() == M::Landscape ?
        columnsLandscape : columnsPortrait;

    // The return value for geometry() is not right.
    //width = geometry().width() / columns;
    window = MApplication::activeApplicationWindow();
    if (window) {
        qreal  aspectRatio;
        if (window->orientation() == M::Landscape)
            aspectRatio = 
                (qreal) window->visibleSceneSize().height() / 
                (qreal) window->visibleSceneSize().width();
        else
            aspectRatio = 
                (qreal) window->visibleSceneSize().width() /
                (qreal) window->visibleSceneSize().height();

        width = window->visibleSceneSize().width() / columns;
        height = aspectRatio * width;
    } else {
        // FIXME: These are the vired in values.
        width = event->orientation() == M::Landscape ?
            854  / columns : 480 / columns;
        height = event->orientation() == M::Landscape ?
            480  / columns : 854 / columns;
    }

    #if 0
    SYS_DEBUG ("*** geometry().width()  = %g", geometry().width());
    SYS_DEBUG ("*** geometry().height() = %g", geometry().height());
    SYS_DEBUG ("*** columns             = %d", columns);
    SYS_DEBUG ("*** width               = %g", width);
    #endif

    m_CellCreator->setCellSize (QSizeF(width, width/*height*/));
    setColumns (columns);
}

void
WallpaperList::hideEvent (
        QHideEvent *event)
{
    Q_UNUSED (event);
    /*
     * When we got hidden we stop all the image loading. We have to give some
     * CPU for the other page/widget.
     */
    if (m_Model)
        m_Model->stopLoadingThumbnails ();
}

void 
WallpaperList::showEvent (
        QShowEvent *event)
{
    Q_UNUSED (event);
    SYS_DEBUG ("");
    /*
     * When we page back from the other widget we might want to load the images
     * we stopped loading when we become hidden.
     */
    QTimer::singleShot (loadPicturesDelay, this, SLOT(loadPictures()));
}

