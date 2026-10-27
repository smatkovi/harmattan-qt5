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
#ifndef WALLPAPERLIST_H
#define WALLPAPERLIST_H

#include <MList>
#include <QModelIndex>

class QGraphicsItem;
class WallpaperDescriptor;
class WallpaperBusinessLogic;
class WallpaperImageLoader;
class WallpaperModel;
class MOrientationChangeEvent;
class WallpaperCellCreator;

/*!
 * A list widget that shows a list of the available wallpapers.
 */
class WallpaperList : public MList 
{
    Q_OBJECT
        
public:
    WallpaperList (
            WallpaperBusinessLogic  *logic,
            QGraphicsItem           *parent = 0);

    typedef enum {
        DataSourceLocal,
        DataSourceOvi,
        DataSourceUnknown
    } DataSourceType;

    void setDataSourceType (WallpaperList::DataSourceType sourceType);

protected:
    virtual void orientationChangeEvent (MOrientationChangeEvent *event);
    void hideEvent (QHideEvent *event);
    void showEvent (QShowEvent *event);

private slots:
    void slotItemClicked (const QModelIndex &index);
    void loadPictures ();
    void modelChanged ();
    
signals:
    void imageActivated (WallpaperDescriptor *desc);

private:
    WallpaperBusinessLogic *m_BusinessLogic;
    WallpaperImageLoader   *m_ImageLoader;
    WallpaperModel         *m_Model;
    WallpaperCellCreator   *m_CellCreator;
    DataSourceType          m_DataSourceType;
    #ifdef UNIT_TEST
    friend class Ut_WallpaperList;
    #endif
};

#endif
