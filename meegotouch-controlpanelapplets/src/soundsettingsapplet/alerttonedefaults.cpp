/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#include <QDir>
#include <QDebug>
#include <QTimer>
#include <QGraphicsWidget>
#include <QShowEvent>
#include <MApplicationPage>
#include <MLabel>
#include <MBasicListItem>

#include <MSortFilterProxyModel>
#include <MListFilter>

#include "static.h"
#include "alerttonedefaults.h"
#include "alerttonedefaultsmodel.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

static const char *SelectionStartTag = "<font color='blue'>";
static const char *SelectionEndTag   = "</font>";


MyListItem::MyListItem (
        MBasicListItem::ItemStyle style, 
        QGraphicsItem *parent) : 
    MBasicListItem (style, parent)
{
}

void
MyListItem::setup()
{
    setStyleName ("CommonPanelInverted");
    titleLabelWidget()->setStyleName ("CommonSingleTitleInverted");
}

/******************************************************************************
 * AlertToneDefaultsCellCreator implementation.
 */
class AlertToneDefaultsCellCreator : public MAbstractCellCreator<MyListItem>
{
public:
    virtual MWidget *createCell (
                const QModelIndex &index, 
                MWidgetRecycler   &recycler) const;

	virtual void updateCell (
            const QModelIndex &index, 
            MWidget           *cell) const;

    void highlightByText(QString text);

private:
        QString   m_HighlightText;
};

MWidget *
AlertToneDefaultsCellCreator::createCell(
        const QModelIndex &index, 
        MWidgetRecycler   &recycler) const
{
    MyListItem *cell;
    
    cell = qobject_cast <MyListItem *> (
            recycler.take(MyListItem::staticMetaObject.className()));

    if (!cell) {
        cell = new MyListItem (MyListItem::SingleTitle);
    }

    updateCell(index, cell);

    return cell;
}

void 
AlertToneDefaultsCellCreator::updateCell(
        const QModelIndex &index, MWidget *cell) const
{
    MyListItem *item;
    QString       title;

    item = qobject_cast<MyListItem *>(cell);
    title = index.data().toString();
   
    if (m_HighlightText.isEmpty()) {
        item->setTitle (title);
    } else if (title.startsWith(m_HighlightText, Qt::CaseInsensitive)) {
        int matchingIndex = title.indexOf (
                m_HighlightText, 0, Qt::CaseInsensitive);

        if (matchingIndex != -1) {
            title.insert (matchingIndex + m_HighlightText.length(), 
                    SelectionEndTag);
            title.insert (matchingIndex, SelectionStartTag);
        }

        item->setTitle (title);
    }

    /*
     * Setting the item mode. These lines will make the item corners round where
     * they need to be round.
     */
    if (index.row() == 0)
        item->setLayoutPosition (M::VerticalTopPosition);
    else if (index.row() == index.model()->rowCount() - 1)
        item->setLayoutPosition (M::VerticalBottomPosition);
    else
        item->setLayoutPosition (M::VerticalCenterPosition);

    item->setup ();
    //item->setStyleName ("CommonPanelInverted");
    //item->titleLabelWidget()->setStyleName ("CommonTitleInverted");
}

void 
AlertToneDefaultsCellCreator::highlightByText (
        QString text)
{
    m_HighlightText = text;
}


/******************************************************************************
 * AlertToneDefaults implementation.
 */
AlertToneDefaults::AlertToneDefaults (
        AlertTone        *tone, 
        QGraphicsWidget  *parent) :
	MList(parent),
	m_tone(tone),
    m_PanningStarted (false),
    m_ShowingSpinner (false)
{
    setSelectionMode (SingleSelection);
	m_DefaultsModel = new AlertToneDefaultsModel;

	setObjectName("AlertToneDefaults");
    m_CellCreator = new AlertToneDefaultsCellCreator;
	setCellCreator(m_CellCreator);
	setItemModel(m_DefaultsModel);


    /*
     * We have to give the widget a chance to process the model events,
     * otherwise the scroollTo() method will not work.
     */
	QTimer::singleShot(10, this, SLOT(toneChanged()));

    connect (m_DefaultsModel, SIGNAL(finished()), 
            this, SLOT(loadingFinished()));

	connect (tone, SIGNAL(changed()), 
            this, SLOT(toneChanged()));
	connect (this, SIGNAL(itemClicked(const QModelIndex &)), 
            this, SLOT(itemClicked(const QModelIndex &)));
	connect (this, SIGNAL(panningStarted()), 
            this, SLOT(userPanningStarted()));
   
    /*
     * Please note that the loadingFinished() will hide the spinner.
     */
    connect (m_DefaultsModel, SIGNAL(loading()), 
            this, SLOT(checkSpinner()));
    m_FileFromMyMusicSelected = false;
}

AlertToneDefaults::~AlertToneDefaults ()
{
    delete m_DefaultsModel;
    m_DefaultsModel = 0;
}

/*!
 * This slot will be activated when the tone setting is changed.
 */
void
AlertToneDefaults::toneChanged()
{
    QString  fileName = m_tone->fileName();
    QString  niceName = m_tone->niceName();
    
    /*
     * m_tone->value() returns the full file path of the tone file.
     */
    SYS_DEBUG ("*** fileName = %s", SYS_STR(fileName));
    SYS_DEBUG ("*** niceName = %s", SYS_STR(niceName));
    if (isVisible()) {
        selectAndScroll (fileName, niceName);
    } else {
        m_FileNameToSelect = fileName;
        m_NiceNameToSelect = niceName;
    }
}

/*!
 * This slot will be activated when the loading of the available tones from the
 * filesystem is finished.
 */
void
AlertToneDefaults::loadingFinished()
{
    SYS_DEBUG ("*** m_FileNameToSelect = %s", SYS_STR(m_FileNameToSelect));
    SYS_DEBUG ("*** isVisible() = %s", SYS_BOOL(isVisible()));

    /*
     * If we are not shown on the screen we will handle this finished thingy
     * from the showEvent() callback. We have to abort this method so the 
     * m_FileNameToSelect and other variables are kept in their state.
     */
    if (!isVisible())
        return;

    if (!m_FileNameToSelect.isEmpty()) {
        selectAndScroll (m_FileNameToSelect, m_NiceNameToSelect);
    }

    checkSpinner ();
}

/*!
 * This slot is activated when the user clicks on a particular list item. This
 * method will abort the postponed selecting (because the user don't want us to
 * select the current item once the loading of the items are finished since
 * he/she already selected a new item). This method will also emit a signal with
 * the filename the user just selected. 
 * 
 * There should be no other way to find the selected item but to watch this
 * signal.
 */
void
AlertToneDefaults::itemClicked(const QModelIndex &item)
{
    QModelIndex  proxyIndex = filtering()->proxy()->mapToSource (item); 
    QString      fileName;

    /*
     * It is possible that the user clicks on an item before all the ites are
     * loaded. In this case we shall not select the current item when the
     * loading is finished.
     * 
     * NOTE: We need to interpret this index as an index of the proxy model,
     * this fixes NB#199741 - Not able to set alert tones from filtered tones 
     * list.
     */
    m_FileNameToSelect = "";

    fileName = m_DefaultsModel->fileName (proxyIndex);
    SYS_DEBUG ("*** fileName    = %s", SYS_STR(fileName));
	SYS_DEBUG ("Emitting defaultItemClicked (%s)", SYS_STR(fileName));

	emit defaultItemClicked (fileName);
}

/*!
 * \param filter The filter text for the live filtering.
 * 
 * Sets the filter text for the live filtering and refreshes the model. The
 * model has to be refreshed so that the highlight will properly refresh in
 * every item of the list.
 */
void 
AlertToneDefaults::setFilterText (
        const QString &filter)
{
    m_CellCreator->highlightByText (filter);
    m_DefaultsModel->refresh ();
}

/*!
 * \param idx The numerical index of the item to select and scroll to.
 * \returns true if the item is found, false if the operation has been failed.
 * 
 * A lower level method to select an item from the list and scroll the list so
 * that the item becomes visible for the user. This method has no failsafe, it
 * will not add the any new items to the list if the index is not valid.
 */
bool
AlertToneDefaults::selectAndScroll (
        int    idx)
{
    SYS_DEBUG ("*** idx       = %d", idx);
    SYS_DEBUG ("*** isVisible = %s", SYS_BOOL(isVisible()));
    SYS_DEBUG ("*** m_PanningStarted = %s", SYS_BOOL(m_PanningStarted));
    if (idx < 0)
        return false;

    /*
     * If the list is not visible it will not handle our requests anyway.
     */
    if (!isVisible()) {
        return true;
    }

    /*
     * We need to use the proxy model here, fixes #199749 - Current selected
     * tone is getting deselected when user taps on it.
     */
    const QModelIndex mIndex = m_DefaultsModel->index (idx, 0); 
    const QModelIndex index = filtering()->proxy()->mapFromSource (mIndex);

    selectionModel()->select (index, QItemSelectionModel::ClearAndSelect);
    SYS_DEBUG ("*** newIndex = %d", index.row()); 
    /*
     * Once the user started a panning we don't want to scroll to the just
     * selected item, the user knows what to do.
     */
    // Had to disable this feature because of NB#248294
    //if (!m_PanningStarted) 
    {
        SYS_DEBUG ("calling MList::scrollTo(%d)", idx);
        MList::scrollTo (
                index, MList::PositionAtCenterHint, MList::NonAnimated);
    }

    m_PanningStarted = false;
    return true;
}

/*!
 * \param fileName The file name to find or insert
 * \param niceName The user readable name to find or insert
 * \returns true if the item found, selected and scrolled, false otherwise
 * 
 * Will try to find a particular item, select it and scroll the list there. If
 * this method returns false it means that the item is not found but. If the
 * model is ready this method inserts the item and registers a delayed method
 * call to try the selecting and scroll once the widget had the chance to
 * process its signals. If the model is not ready the m_FileNameToSelect and 
 * m_NiceNameToSelect is set so that the loadingFinished() slot will re-call
 * this method. 
 *
 * No matter if the item is added now or will be added later this method will be
 * returning false and that means the m_FileNameToSelect, m_NiceNameToSelect
 * will hold the information about the delayed action.
 */
bool
AlertToneDefaults::selectAndScroll (
        const QString &fileName,
        const QString &niceName)
{
    int      idx;
    bool     success;

    SYS_DEBUG ("--------------------------------------------------");
    SYS_DEBUG ("*** fileName    = %s", SYS_STR(fileName));
    SYS_DEBUG ("*** niceName    = %s", SYS_STR(niceName));
    SYS_DEBUG ("*** isVisible() = %s", SYS_BOOL(isVisible()));

    /*
     * When we are invisible (e.g. the selectandscroll is coming from the
     * contentpicker) we have no chance to manipulate the list thus we will not
     * remember the selection. The setting of the AlertTone fails.
     * We handle this with the mechanism designed for the loading process: we
     * are going to delay the list manipulation.
     */
    if (!isVisible()) {
        m_PanningStarted = false;
        m_FileNameToSelect = fileName;
        m_NiceNameToSelect = niceName;
    }

    filtering()->proxy()->sort(AlertToneDefaultsModel::NiceNameColumn);

    idx = m_DefaultsModel->findItemByFileName (fileName);

    SYS_DEBUG ("m_FileFromMyMusicSelected = %s", 
            SYS_BOOL(m_FileFromMyMusicSelected));
    SYS_DEBUG ("idx                       = %d", idx);
    if(!m_FileFromMyMusicSelected && idx >= 0) {
        SYS_DEBUG ("Moving");
        QModelIndex mIndex = m_DefaultsModel->index (idx, 0);
        m_DefaultsModel->moveItem(idx, 0);
        success = selectAndScroll (0);
    } else {
        SYS_DEBUG ("Selecting");
        success = selectAndScroll (idx);
    }


    SYS_DEBUG ("idx                       = %d", idx);
    SYS_DEBUG ("success                   = %s", SYS_BOOL(idx));
    if (!success) {
        m_FileNameToSelect = fileName;
        m_NiceNameToSelect = niceName;
        if (m_DefaultsModel->isFinished()) {
            idx = m_DefaultsModel->addSingleItem (
                    niceName,
                    fileName, 
                    true);
            /*
             * We have to give the widget a chance to process the model events,
             * otherwise the scroollTo() method will not work.
             */
            SYS_WARNING ("singleShot (loadingFinished())");
            QTimer::singleShot (200, this, SLOT(loadingFinished()));
        } else {
            SYS_DEBUG ("Calling clear()");
            selectionModel()->clear();
        }
    } else {
        m_FileNameToSelect = "";
        m_NiceNameToSelect = "";
        m_FileFromMyMusicSelected = false;
    }

    return success;
}

/*!
 * This slot is called when the user starts the panning. We need to know if the
 * user initiated such an action because we don't want to scroll the widget once
 * the user started to pan.
 */
void 
AlertToneDefaults::userPanningStarted ()
{
    SYS_DEBUG ("");

    if (m_ShowingSpinner) {
        SYS_DEBUG ("Setting m_PanningStarted to true");
        m_PanningStarted = true;
    }
}

void
AlertToneDefaults::polishEvent ()
{
    SYS_DEBUG ("");
    // Here we sort the model. Seems that it is not the best place for sorting,
    // but it at least works.
    filtering()->proxy()->sort(AlertToneDefaultsModel::NiceNameColumn);
    checkSpinner ();
}

void
AlertToneDefaults::showEvent (
        QShowEvent *event)
{
    SYS_DEBUG ("*** isVisible() = %s", SYS_BOOL(isVisible()));
    SYS_DEBUG ("*** m_FileNameToSelect = %s", SYS_STR(m_FileNameToSelect));

    event->accept ();

    if(!isVisible())
        m_FileFromMyMusicSelected = true;
    else
        m_FileFromMyMusicSelected = false;

    /*
     * If there is a pending selection we need to handle it but not now,
     * unfortunately we need to do it when the widget becomes visible.
     * FIXME: maybe we can solve this without calling the singleShot, but please
     * keep an eye on bug NB#208326!
     */
    if (!m_FileNameToSelect.isEmpty())
        QTimer::singleShot(100, this, SLOT(loadingFinished()));
}

/*
 * This slot is called when the widget sends the layoutChanged signal (sent when
 * the widget already have an MApplicationPage parent) and when the model
 * finished loading its content. 
 *
 * The method will enable/disable the spinner that shows the progress of the
 * loading in the application menu area.
 */
void 
AlertToneDefaults::checkSpinner ()
{
    QGraphicsWidget  *parent;
    MApplicationPage *page = 0;

    SYS_DEBUG ("*** m_ShowingSpinner = %s", SYS_BOOL(m_ShowingSpinner));
    SYS_DEBUG ("*** isFinished()     = %s", SYS_BOOL(m_DefaultsModel->isFinished()));
    
    if (m_ShowingSpinner == !m_DefaultsModel->isFinished())
        return;

    /*
     * We need to find the MApplicationPage among our parents.
     */
    parent = parentWidget();
    while (parent) {
        page = qobject_cast <MApplicationPage *>(parent);
        if (page)
            break;
        parent = parent->parentWidget();
    }

    if (!page) {
        return;
    }

    m_ShowingSpinner = !m_DefaultsModel->isFinished();
    SYS_DEBUG ("calling page->setProgressIndicatorVisible (%s)", 
            SYS_BOOL(m_ShowingSpinner));
    page->setProgressIndicatorVisible (m_ShowingSpinner);
}

