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
#ifndef THEMECELLCREATOR_H__
#define THEMECELLCREATOR_H__

#include <QList>
#include <MAdvancedListItem>
#include <MAbstractCellCreator>

class CustomListItem : public MAdvancedListItem 
{
    Q_OBJECT
    public:
        CustomListItem (
            ItemStyle itemStyle = IconWithTitleProgressIndicatorAndTwoSideIcons, 
            QGraphicsItem *parent = 0);
};

class ThemeCellCreator : public MAbstractCellCreator<MAdvancedListItem>
{
    public:
        virtual MWidget *createCell (
                const QModelIndex &index, 
                MWidgetRecycler   &recycler) const;
        
        virtual void updateCell (
                const QModelIndex &index, 
                MWidget           *cell) const;

        virtual void updateListItemMode (
              const QModelIndex &index, 
              MAdvancedListItem *listItem) const;

        void highlightByText(QString text);

    private:
        QString   m_HighlightText;
};

#endif
