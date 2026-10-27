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
#ifndef WALLPAPERCELLCREATOR_H
#define WALLPAPERCELLCREATOR_H

#include <MWidgetController>
#include <MAbstractCellCreator>

class WallpaperCellCreator : 
    public MAbstractCellCreator<MWidgetController>
{
    public:
        virtual MWidget *createCell (
                const QModelIndex &index, 
                MWidgetRecycler   &recycler) const;
      
        virtual void setCellSize (const QSizeF &size);
        virtual QSizeF cellSize() const;

        virtual void updateCell (
                const QModelIndex &index, 
                MWidget *cell) const;

    private:
        void createPlaceholderPixmap (int width, int height) const;
        
    private:
        QSizeF           m_CellSize;
        mutable QPixmap  m_PlaceholderPixmap;
};

#endif
