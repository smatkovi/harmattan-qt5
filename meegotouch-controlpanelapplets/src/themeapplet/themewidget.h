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
#ifndef THEMEWIDGET_H
#define THEMEWIDGET_H

#include <QPointer>
#include <QModelIndex>
#include <dcpwidget.h>
#include <themedialog.h>

#include "themebusinesslogic.h"
#include "themelistmodel.h"
#include "themecellcreator.h"

class MList;
class MTextEdit;
class MSortFilterProxyModel;
class MBasicListItem;
class RightArrowItem;

class ThemeWidget : public DcpWidget
{
    Q_OBJECT

public:
    ThemeWidget (
            ThemeBusinessLogic *themeBusinessLogic, 
            QGraphicsWidget    *parent = 0);
    ~ThemeWidget ();

    void retranslateUi ();

private slots:
    void selectCurrentTheme ();
    void themeActivated(const QModelIndex &index);
    void oviActivated ();
    void textChanged ();
    void hideEmptyTextEdit ();
    void refreshNeeded ();
    void enableList ();
    void disableList ();

private:
    void createWidgets ();
    void readLocalThemes ();

    QPointer<ThemeBusinessLogic>  m_ThemeBusinessLogic;
    ThemeListModel               *m_ThemeListModel;
    MSortFilterProxyModel        *m_Proxy;
    ThemeCellCreator             *m_CellCreator;

    MTextEdit                    *m_LiveFilterEditor;
    MList                        *m_List;
    RightArrowItem                *m_OviItem;
    QPointer<ThemeDialog>         m_ThemeDialog;
    #ifdef UNIT_TEST
    friend class Ut_ThemeWidget;
    #endif
};

#endif
