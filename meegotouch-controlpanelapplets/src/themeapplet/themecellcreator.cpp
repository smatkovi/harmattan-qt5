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

#include <QString>

#include <QModelIndex>
#include <MSortFilterProxyModel>

#include <QVariant>
#include <MAdvancedListItem>
#include <MImageWidget>
#include <MLabel>
#include <MProgressIndicator>
#include <QGraphicsLayout>

#include "themecellcreator.h"
#include "themelistmodel.h"

#undef DEBUG
#include "../debug.h"

static const char *SelectionStartTag = "<font color='blue'>";
static const char *SelectionEndTag   = "</font>";

CustomListItem::CustomListItem (
        ItemStyle         itemStyle,
        QGraphicsItem    *parent) :
    MAdvancedListItem (itemStyle, parent)
{
    setStyleName ("CommonPanelInverted");
    titleLabelWidget ()->setStyleName ("CommonTitleInverted");
}

MWidget *
ThemeCellCreator::createCell(
        const QModelIndex &index, 
        MWidgetRecycler   &recycler) const
{
    CustomListItem *cell;
    
    cell = qobject_cast <CustomListItem *> (
            recycler.take(CustomListItem::staticMetaObject.className()));

    if (!cell) {
        cell = new CustomListItem (
            MAdvancedListItem::IconWithTitleProgressIndicatorAndTwoSideIcons);
        cell->progressIndicator()->setUnknownDuration (true);
        cell->sideTopImageWidget()->hide();
        cell->sideBottomImageWidget()->hide();
    }

    updateCell(index, cell);

    return cell;
}

void
ThemeCellCreator::updateCell (
        const QModelIndex &index, 
        MWidget           *cell) const
{
    CustomListItem       *listItem;
    QString               title;
    QString               codeName;
    QString               iconName;
    QString               changingTheme;
    bool                  isChangingTheme;

    if(!cell || !index.isValid()) 
        return;

    listItem = qobject_cast<CustomListItem *>(cell);
    
    changingTheme = index.data (ThemeListModel::ChangingNameRole).toString();
    title = index.data (ThemeListModel::NameRole).toString();
    codeName = index.data (ThemeListModel::CodeNameRole).toString();
    iconName = index.data (ThemeListModel::IconNameRole).toString();
    isChangingTheme = !codeName.isEmpty() && changingTheme == codeName;

    #if 1
    SYS_DEBUG ("************** %d ***************", index.row());
    SYS_DEBUG ("title           = %s", SYS_STR(title));
    SYS_DEBUG ("changingTheme   = %s", SYS_STR(changingTheme));
    SYS_DEBUG ("codeName        = %s", SYS_STR(codeName));
    SYS_DEBUG ("iconName        = %s", SYS_STR(iconName));
    SYS_DEBUG ("isChangingTheme = %s", SYS_BOOL(isChangingTheme));
    #endif

    // The title
    if (m_HighlightText.isEmpty()) {
        listItem->setTitle(title);
    } else {
        int matchingIndex = title.indexOf (
                m_HighlightText, 0, Qt::CaseInsensitive);

        if (matchingIndex != -1) {
            title.insert (matchingIndex + m_HighlightText.length(), 
                    SelectionEndTag);
            title.insert (matchingIndex, SelectionStartTag);
        }
        listItem->setTitle (title);
    }

    // The icon
    if (listItem->imageWidget()->image() != iconName)
        listItem->imageWidget()->setImage (iconName);

    // The spinner.
    if (isChangingTheme) {
        listItem->progressIndicator()->show();
    } else {
        listItem->progressIndicator()->hide();
    }

    updateListItemMode(index, listItem);
}

void 
ThemeCellCreator::updateListItemMode (
              const QModelIndex &index, 
              MAdvancedListItem *listItem) const
{
    int row = index.row();
    int rows = index.model()->rowCount();

    if (row == 0)
        listItem->setLayoutPosition (M::VerticalTopPosition);
    else if (row < rows - 1)
        listItem->setLayoutPosition (M::VerticalCenterPosition);
    else 
        listItem->setLayoutPosition (M::VerticalBottomPosition);
}

void 
ThemeCellCreator::highlightByText (
        QString text)
{
    m_HighlightText = text;
}

