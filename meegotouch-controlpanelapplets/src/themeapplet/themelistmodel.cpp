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

#include "themelistmodel.h"
#include "themedescriptor.h"

#include <QModelIndex>

//#define DEBUG
#include "../debug.h"

ThemeListModel::ThemeListModel (
        ThemeBusinessLogic *logic,
        QObject            *parent)
    : QAbstractTableModel (parent),
    m_ThemeBusinessLogic (logic)
{
    /*
     * If we have a businesslogic we can read the available themes.
     */
    if (logic) {
        QList<ThemeDescriptor *> list;
        list = logic->availableThemes ();

        if (list.size() > 0) {
            beginInsertRows (QModelIndex(), 0, list.size() - 1);
            m_ThemeDescList = list;
            endInsertRows ();
        }

        /*
         *
         */
        connect (logic, SIGNAL(themeAboutToBeRemoved(int)),
                    this, SLOT(themeAboutToBeRemoved(int)));
        connect (logic, SIGNAL(themeRemoved(QList<ThemeDescriptor *>)),
                this, SLOT(themeRemoved(QList<ThemeDescriptor *>)));

        connect (logic, SIGNAL(themeAboutToBeAdded(int)),
                    this, SLOT(themeAboutToBeAdded(int)));
        connect (logic, SIGNAL(themeAdded(QList<ThemeDescriptor *>)),
                this, SLOT(themeAdded(QList<ThemeDescriptor *>)));
    }
}

/*
 * We call this function when the whole model needs to be refreshed on the
 * screen, for example when the highlight is changed because the user typed a
 * search string.
 */
void
ThemeListModel::refresh()
{
    if (m_ThemeDescList.size() != 0) {
        QModelIndex first = index (0, 0);
        QModelIndex last  = index (m_ThemeDescList.size() - 1, 0);

        SYS_DEBUG ("Emitting dataChanged()");
        emit dataChanged (first, last);
    }
}

int
ThemeListModel::rowCount(
        const QModelIndex &parent) const
{
    int retval = m_ThemeDescList.size();

    if (parent.isValid())
        retval = 0;

    //SYS_DEBUG ("Returning %d", retval);
    return retval;
}

int 
ThemeListModel::columnCount (
        const QModelIndex & parent) const
{
    int retval = 2;

    if (parent.isValid())
        retval = 0;

    //SYS_DEBUG ("Returning %d", retval);
    return retval;
}
        
/*!
 * Please note: In this function we return a value based on the role parameter
 * and we are not using the column number from the index. Well, it works...
 */
QVariant
ThemeListModel::data (
		const QModelIndex &index, 
		int                role) const
{    
    if (index.row() < 0 || index.row() >= m_ThemeDescList.size())
        return QVariant();

    ThemeDescriptor *desc = m_ThemeDescList[index.row()];
   
    switch (role) {
        case Qt::DisplayRole:
            return QVariant (desc->name());

        case ThemeListModel::SearchRole:
            return QVariant (desc->name());

        case ThemeListModel::CodeNameRole:
            return QVariant(desc->codeName());

        case ThemeListModel::NameRole:
            return QVariant(desc->name());
        
        case ThemeListModel::IconNameRole:
            return QVariant(desc->iconName());
            
        case ThemeListModel::ChangingNameRole:
            return QVariant (m_ChangingTheme);

        default:
            SYS_WARNING ("Unhandled role: %d", role);
            return QVariant();
    }
}

 
QModelIndex
ThemeListModel::indexOfCodeName(
        const QString &codeName) const 
{
    int i = 0;

    foreach (ThemeDescriptor *desc, m_ThemeDescList) {
        if (desc->codeName() == codeName) {
            return index (i, 0);
        }

        ++i;
    }

    SYS_WARNING("code name not found in list model: %s", SYS_STR(codeName));
    return QModelIndex();
}

void 
ThemeListModel::themeChangeStarted (
		QString themeCodeName)
{
    QModelIndex index = indexOfCodeName (themeCodeName);
    
    SYS_DEBUG ("*** themeCodeName = %s", SYS_STR(themeCodeName));
    m_ChangingTheme = themeCodeName;

    if (index.isValid())
        emit dataChanged (index, index);
}

void
ThemeListModel::themeChanged (
		QString themeCodeName)
{
    QModelIndex index = indexOfCodeName (themeCodeName);

    SYS_DEBUG ("***********************************************************");
    SYS_DEBUG ("*** themeCodeName   = %s", SYS_STR(themeCodeName));
    SYS_DEBUG ("*** index.isValid() = %s", SYS_BOOL(index.isValid()));
    m_ChangingTheme = "";

    if (index.isValid())
        emit dataChanged (index, index);
}

/*!
 * \returns The name of the theme that is about to become the current theme. 
 *
 * In the UI this theme should have the spinner shown.
 */
QString 
ThemeListModel::changingTheme () const
{
    return m_ChangingTheme;
}

void
ThemeListModel::themeAboutToBeRemoved (
        int index)
{
    SYS_DEBUG ("");
    beginRemoveRows (QModelIndex(), index, index);
}
 
void
ThemeListModel::themeRemoved (
        QList<ThemeDescriptor *> list)
{
    SYS_DEBUG ("");
    m_ThemeDescList = list;
    endRemoveRows ();
}

void
ThemeListModel::themeAboutToBeAdded (
        int index)
{
    SYS_DEBUG ("");
    beginInsertRows (QModelIndex(), index, index);
}
 
void
ThemeListModel::themeAdded (
        QList<ThemeDescriptor *> list)
{
    SYS_DEBUG ("");
    m_ThemeDescList = list;
    endInsertRows ();
}


