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
#ifndef THEMELISTMODEL_H__
#define THEMELISTMODEL_H__

#include <QList>
#include <QStringList>
#include <QString>
#include <QAbstractTableModel>

#include "themebusinesslogic.h"
#include "themedescriptor.h"

class ThemeDescriptor;
class QModelIndex;

class ThemeListModel : public QAbstractTableModel
{
    Q_OBJECT

    public:
        ThemeListModel(
                ThemeBusinessLogic *businessLogic = 0,
                QObject            *parent = 0);

        void refresh ();

        int rowCount (const QModelIndex &parent = QModelIndex()) const;
        int columnCount (const QModelIndex & parent) const;

        QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const;
        QModelIndex indexOfCodeName(const QString &codeName) const;
        QString changingTheme () const;

        typedef enum {
            SearchRole  = Qt::UserRole + 1,
            CodeNameRole,
            NameRole,
            IconNameRole,
            ChangingNameRole,
        } Roles;

    public slots:
	    void themeChangeStarted (QString themeCodeName);
    	void themeChanged (QString themeCodeName);
        void themeAboutToBeRemoved (int index);
        void themeRemoved (QList<ThemeDescriptor *> list);
        void themeAboutToBeAdded (int index);
        void themeAdded (QList<ThemeDescriptor *> list);

    private:
        ThemeBusinessLogic  *m_ThemeBusinessLogic;
        // FIXME: This should be in the businesslogic.
        QString              m_ChangingTheme;
        QList<ThemeDescriptor *>      m_ThemeDescList;
};

#endif
