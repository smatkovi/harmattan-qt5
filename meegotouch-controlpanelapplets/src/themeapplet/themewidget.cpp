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

#include "themewidget.h"
#include "themedescriptor.h"
#include "themelistmodel.h"
#include "themecellcreator.h"
#include "themedialog.h"
#include "drilldownitem.h"

#include <MApplication>
#include <MApplicationWindow>
#include <QGraphicsLinearLayout>
#include <QDBusInterface>
#include <MTextEdit>
#include <MList>
#include <MListFilter>
#include <MSortFilterProxyModel>
#include <MBasicListItem>
#include <MImageWidget>
#include <QTimer>

#define DEBUG
#define WARNING
#include "../debug.h"

ThemeWidget::ThemeWidget (
        ThemeBusinessLogic *themeBusinessLogic, 
        QGraphicsWidget    *parent) :
    DcpWidget (parent),
    m_ThemeBusinessLogic (themeBusinessLogic),
    m_ThemeListModel(0),
    m_List(0),
    m_OviItem(0)
{
    createWidgets ();
}

ThemeWidget::~ThemeWidget ()
{
    SYS_DEBUG ("");
    m_LiveFilterEditor->setParentLayoutItem (0);
}

void
ThemeWidget::createWidgets ()
{
    QGraphicsLinearLayout *mainLayout;
    
    /*
     * Creating and setting up the main layout
     */
    mainLayout = new QGraphicsLinearLayout(Qt::Vertical);
    mainLayout->setContentsMargins (0., 0., 0., 0.);
    mainLayout->setSpacing (0.);
    
    /*
     * Creating the list with the available themes.
     */
    m_List = new MList();
    m_List->setObjectName ("ThemeList");


    // We only connect the themeChangeStarted if we have a chance to sense the
    // end of the theme change too. just to be sure.
    if (connect (m_ThemeBusinessLogic, SIGNAL (themeChanged (QString)),
                 SLOT (enableList ()))) {
        connect (m_ThemeBusinessLogic, SIGNAL (themeChangeStarted (QString)),
                 SLOT (disableList ()));
    }
    
    // Cellcreator
    m_CellCreator = new ThemeCellCreator;
    m_List->setCellCreator (m_CellCreator);
    m_List->setSelectionMode (MList::SingleSelection);

    // This function will create the m_LiveFilterEditor widget.
    readLocalThemes ();

    /*
     * An item to activate the OVI link.
     */
    m_OviItem = new RightArrowItem (MBasicListItem::IconWithTitle);
    m_OviItem->setObjectName("OviItem");

    // Currently we use the default.
    //m_OviItem->setLayoutPosition (M::VerticalCenterPosition);
    m_OviItem->imageWidget()->setImage ("icon-m-common-ovi");

    /*
     * Adding everything to the layout.
     */
    m_LiveFilterEditor->setParentLayoutItem (mainLayout);
    mainLayout->addItem (m_OviItem);
    mainLayout->addItem (m_List);

    /*
     * Connecting to the signals.
     */
    connect (m_OviItem, SIGNAL(clicked()),
            this, SLOT(oviActivated()));
    connect (m_LiveFilterEditor, SIGNAL(textChanged()),
            this, SLOT(textChanged ()));
    connect (m_List, SIGNAL(panningStarted()),
            this, SLOT(hideEmptyTextEdit()));
    connect (m_ThemeBusinessLogic, SIGNAL(refreshNeeded()),
            this, SLOT(refreshNeeded ()));

    setLayout(mainLayout);
    retranslateUi ();
}

/*!
 * A slot to disable the list of themes. We use this method to disable the user
 * interaction with the list while the theme is being changed.
 */
void
ThemeWidget::disableList ()
{
    m_List->setEnabled(false);
}

/*!
 * A slot to enable the theme list that was disabled by the disableList()
 * method.
 */
void
ThemeWidget::enableList ()
{
    m_List->setEnabled(true);
    selectCurrentTheme ();
}

void
ThemeWidget::retranslateUi ()
{
    //% "Get more from Ovi Store"
    m_OviItem->setTitle(qtTrId("qtn_teme_store"));
}

/**
 * If you consider to call this function multiple times
 * (for example because you want to update the theme list),
 * pay attention to freeing ThemeDescriptor objects and
 * m_ThemeListModel.
 *
 * Apparently multiple calls of setItemModel() with the same
 * model doesn't update MList.
 *
 * You also have to deal with disconnection and reconnection of
 * itemClicked() signal because MList::selectItem() emits that!
 */
void
ThemeWidget::readLocalThemes ()
{
    /*
     * Creating the model and connecting it to the businesslogic so we can show
     * the spinner while the theme change is in progress.
     */
    m_ThemeListModel = new ThemeListModel (m_ThemeBusinessLogic);

    m_ThemeListModel->setObjectName ("ThemeListModel");
    SYS_DEBUG ("*** m_ThemeListModel = %p", m_ThemeListModel);
    m_List->setItemModel (m_ThemeListModel);

    if (m_ThemeBusinessLogic) {
        connect (m_ThemeBusinessLogic, SIGNAL(themeChangeStarted(QString)),
                m_ThemeListModel, SLOT(themeChangeStarted(QString)));
        connect (m_ThemeBusinessLogic, SIGNAL(themeChanged(QString)),
                m_ThemeListModel, SLOT(themeChanged(QString)));
    }

    /*
     * Enabling the live filter feature for the list. From this moment on the
     * list will use a QSortFilterProxyModel object as model. 
     */
    m_List->filtering()->setEnabled (true);
    m_List->filtering()->setFilterRole (ThemeListModel::SearchRole);

    m_Proxy = m_List->filtering()->proxy();
    m_Proxy->setSortRole (ThemeListModel::SearchRole);
    m_Proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
    // Seems that the sort() method simply will not sort when the
    // ThemeListModel::SearchRole is used.
    m_Proxy->sort(Qt::DisplayRole);
    m_Proxy->setFilterKeyColumn(0);

    m_LiveFilterEditor = m_List->filtering()->editor();

    connect(m_List, SIGNAL(itemClicked(QModelIndex)),
            this, SLOT(themeActivated(QModelIndex)));

    SYS_DEBUG ("*** calling selectCurrentTheme()");
    selectCurrentTheme ();
}

/*!
 * This slot selects the current theme from the theme list. We needed to
 * implement this feature as a slot, so we can select the current theme when the
 * user pressed the cancel button in the dialog. We need to go back to the
 * original theme then.
 */
void 
ThemeWidget::selectCurrentTheme ()
{
    QString        currentThemeCodeName; 
    QModelIndex    currentIndex;
    
    /*
     * Selecting the current theme from the list. The index must be mapped to
     * the proxy model, since we are reading from our model but the list shows a
     * proxy that is sorted.
     */
    currentThemeCodeName = m_ThemeBusinessLogic->currentThemeCodeName();
    currentIndex = m_ThemeListModel->indexOfCodeName(currentThemeCodeName);
    currentIndex = m_Proxy->mapFromSource (currentIndex);

    /*
     * Unfortunately we need to clear the selection and re-select the current
     * item, othherwise the enabling/disabling of the list causes strange
     * bugs... sometimes the item is selected, but the selection is not shown.
     */
    #if 0
    if (m_List->selectionModel()->isSelected(currentIndex)) {
        SYS_DEBUG ("Index already selected");
        return;
    }
    #endif
    m_List->selectionModel()->clear();
    m_List->selectionModel()->select (
            currentIndex, 
            QItemSelectionModel::ClearAndSelect);
    m_List->scrollTo (currentIndex, MList::PositionAtCenterHint);
}

void 
ThemeWidget::themeActivated (
        const QModelIndex &index)
{
    QString           codeName;
    ThemeDescriptor  *descr = 0;
      
    SYS_DEBUG ("*** index at %d, %d", index.row(), index.column());

    if (m_ThemeDialog) {
        SYS_DEBUG ("We already have a dialog, returning.");
        return;
    }

    codeName = m_Proxy->data(index, ThemeListModel::CodeNameRole).toString();

    /*
     * If the user selects the current theme we don't do anything.
     */
    if (codeName == m_ThemeBusinessLogic->currentThemeCodeName())
        return;

    #if 0
    /*
     * For debugging purposes it is possible to leave out the dialog and change
     * the theme here.
     */
    m_ThemeBusinessLogic->changeTheme (codeName);
    return;
    #endif

    descr = m_ThemeBusinessLogic->themeByCodename (codeName);
    if (descr == 0) {
        SYS_CRITICAL("codename not found: %s", SYS_STR(codeName));
        return;
    }

    m_ThemeDialog = new ThemeDialog (m_ThemeBusinessLogic, descr);
    connect (m_ThemeDialog, SIGNAL(themeChangeCancelled()),
            this, SLOT(selectCurrentTheme()));
    m_ThemeDialog->showDialog ();
}

void
ThemeWidget::oviActivated ()
{
    static const char OviIf[] = "com.nokia.OviStoreClient";
    QDBusInterface OviStore (OviIf, "/", OviIf);

    OviStore.call ("LaunchWithKeyword", QString ("themes"));
}

void 
ThemeWidget::textChanged ()
{
    if (!m_List->filtering()->editor()->isOnDisplay()) {
        m_List->filtering()->editor()->show();
        m_List->filtering()->editor()->setFocus();
    }

    QGraphicsLinearLayout *mainLayout = dynamic_cast<QGraphicsLinearLayout *>(layout ());
    if (mainLayout)
    {
      if (m_LiveFilterEditor->text ().isEmpty () == true)
      {
          /*
           * We already have a better solution for this in the soundsettings
           * applet...
           */
          mainLayout->removeItem (m_LiveFilterEditor);
          m_LiveFilterEditor->setPos (QPointF (0.,-200.));
      }
      else
      {
          mainLayout->insertItem (0, m_LiveFilterEditor);
      }  
      mainLayout->invalidate ();
    }

    m_CellCreator->highlightByText (m_LiveFilterEditor->text());
    // Seems that the sort() method simply will not sort when the
    // ThemeListModel::SearchRole is used.
    m_Proxy->sort(Qt::DisplayRole);
    
    /*
     * As the search string changes the current theme might appear in the list
     * (if the current theme was filtered before). In this case we need to
     * select this item in the list, because the selection is lost when the
     * selected item is filtered out.
     */
    selectCurrentTheme ();
    m_ThemeListModel->refresh();

    update ();
}

/*
 * This slot will re-sort the model and select the current theme again. Need to
 * be called when the theme package has been removed or installed, connected to
 * a signal of the businesslogic.
 */
void
ThemeWidget::refreshNeeded ()
{
    if (!m_Proxy)
        return;

    m_Proxy->sort(Qt::DisplayRole);
    selectCurrentTheme ();
    m_ThemeListModel->refresh();
    update ();
}

void 
ThemeWidget::hideEmptyTextEdit ()
{
    if (m_List->filtering()->editor()->text().isEmpty())
        m_List->filtering()->editor()->hide();
}

