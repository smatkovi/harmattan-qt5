/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2010-2011 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimsettingswidget.h"

#include <QGraphicsLinearLayout>
#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QDebug>
#include <QTimer>

#include <algorithm>

#include <MContainer>
#include <MLocale>
#include <MPopupList>
#include <MLabel>
#include <MSeparator>

#include <MAbstractCellCreator>
#include <MBanner>
#include <MBasicListItem>
#include <MImageWidget>
#include <MList>
#include <MButton>
#include <MProgressIndicator>

#include "mimsettingslistitem.h"
#include "mimsettingscommon.h"
#include "mimsubviewmodel.h"
#include "mimsettingsapplet.h"

#include "mimsettingsconf.h"
#include "mimplugins.h"
#include "mimpluginenablerwidget.h"

namespace {
    // Object names for settings widgets
    const QString ObjectNameActiveInputMethodWidget("ActiveInputMethodWidget");
    const QString ObjectNameSelectedKeyboardsItem("SelectedKeyboardsItem");

    const QString DefaultPlugin("libmeego-keyboard.so");
    const int FirstPluginContainerIndex = 4; // After the header and items for active and available keyboards

    const QString SwypePlugin("libswype-keyboard.so");
    const QString SwypeSubviewName("Swype");

    // Delay before loading plugins: must be long enough for the
    // enter display transition to finish properly:
    const int PluginLoadDelayMs(1000);

    bool mImSubviewLessThan(const MImSubview &sv1, const MImSubview &sv2)
    {
        return sv1.displayName < sv2.displayName;
    }

    // Function object that tells if a subview's plugin equals
    // the one given in constructor.
    class PluginEquals
    {
    public:
        PluginEquals(const QString &pluginId)
            : m_pluginId(pluginId) {}
        inline bool operator() (const MImSubview &sv)
        {
            return sv.pluginId == m_pluginId;
        }
        const QString &m_pluginId;
    };

};

class MSubViewCellCreator: public MAbstractCellCreator<MBasicListItem>
{
public:
    /*! \reimp */
    virtual MWidget *createCell(const QModelIndex &index,
                                MWidgetRecycler &recycler) const;
    virtual void updateCell(const QModelIndex &index, MWidget *cell) const;
    /*! \reimp_end */
};

MWidget *MSubViewCellCreator::createCell(const QModelIndex &index,
                                         MWidgetRecycler &recycler) const
{
    MBasicListItem *cell = qobject_cast<MBasicListItem *>(recycler.take("MBasicListItem"));
    if (!cell) {
        cell = new MBasicListItem(MBasicListItem::SingleTitle);
    }
    updateCell(index, cell);
    return cell;
}

void MSubViewCellCreator::updateCell(const QModelIndex &index, MWidget *cell) const
{
    MBasicListItem *contentItem = qobject_cast<MBasicListItem *>(cell);

    contentItem->setTitle(index.data(Qt::DisplayRole).toString());
}

MImSettingsWidget::MImSettingsWidget(MImOnScreenPlugins &osp,
                                     MImSettingsApplet *applet,
                                     MImPlugins *p)
    : DcpStylableWidget(),
      mainLayout(0),
      headerLabel(0),
      activeSubViewItem(0),
      availableSubViewItem(0),
      swypePluginEnabler(0),
      availableSubViewList(),
      keyboardDialog(),
      keyboardList(),
      onscreenPlugins(osp),
      selectedSubViews(),
      settingsContainers(),
      plugins(p),
      spinner(0),
      doLoadPlugins(true),
      ownerApplet(applet)
{
    connect(plugins, SIGNAL(loaded(QString)),
            this, SLOT(pluginLoaded(QString)));
    connect(plugins, SIGNAL(loadDone()), this, SLOT(pluginLoadDone()));
    updateSelectedSubViews();
    initWidget();
}


// pressing the back button means accept for us
bool MImSettingsWidget::back()
{
    return true;
}

QString MImSettingsWidget::title() const
{
    return QString(qtTrId(MImSettingsCommon::QtnTxtsTextInput));
}

void MImSettingsWidget::enterDisplayEvent()
{
    DcpStylableWidget::enterDisplayEvent();
    if (doLoadPlugins) {
        doLoadPlugins = false;
        // Wait a little before loading the plugins.
        QTimer::singleShot(PluginLoadDelayMs, plugins, SLOT(startLoading()));
    }
}

MImSettingsWidget::~MImSettingsWidget()
{
}

void MImSettingsWidget::initWidget()
{
    mainLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    mainLayout->setContentsMargins(0., 0., 0., 0.);

    // Header
    headerLabel = new MLabel;
    headerLabel->setStyleName(MImSettingsCommon::StyleHeader);
    mainLayout->addItem(headerLabel);
    mainLayout->setStretchFactor(headerLabel, 0);

    MSeparator *titleSeparator(new MSeparator);
    titleSeparator->setStyleName(MImSettingsCommon::StyleHeaderDivider);
    titleSeparator->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainLayout->addItem(titleSeparator);

    // Active input method selector
    activeSubViewItem = new MImSettingsListItem;
    MImageWidget *activeSubViewArrow = new MImageWidget();
    activeSubViewArrow->setStyleName(MImSettingsCommon::StyleImageWidget);
    activeSubViewItem->setImageWidget(activeSubViewArrow);
    activeSubViewItem->setObjectName(ObjectNameActiveInputMethodWidget);
    activeSubViewItem->setStyleName(MImSettingsCommon::StyleSettingsListItem);
    mainLayout->addItem(activeSubViewItem);
    mainLayout->setStretchFactor(activeSubViewItem, 0);
    connect(activeSubViewItem, SIGNAL(clicked()), this, SLOT(showAvailableSubViewList()));

    // All available subviews.
    availableSubViewItem = new MImSettingsListItem;
    MImageWidget *availableSubViewArrow = new MImageWidget;
    availableSubViewArrow->setStyleName(MImSettingsCommon::StyleImageWidget);
    availableSubViewItem->setImageWidget(availableSubViewArrow);
    availableSubViewItem->setObjectName(ObjectNameSelectedKeyboardsItem);
    availableSubViewItem->setStyleName(MImSettingsCommon::StyleSettingsListItem);
    mainLayout->addItem(availableSubViewItem);
    mainLayout->setStretchFactor(availableSubViewItem, 0);
    connect(availableSubViewItem, SIGNAL(clicked()), this, SLOT(showSelectedKeyboardsDialog()));

    // Progress indicator
    spinner = new MProgressIndicator(this, MProgressIndicator::spinnerType);
    spinner->setUnknownDuration(true);
    spinner->setStyleName(MImSettingsCommon::StyleSpinner);
    mainLayout->addItem(spinner);
    mainLayout->setAlignment(spinner, Qt::AlignCenter);
    mainLayout->setStretchFactor(spinner, 0);

    // Final strech item
    QGraphicsWidget *stretchItem = new QGraphicsWidget;
    stretchItem->setPreferredSize(QSizeF());
    mainLayout->addItem(stretchItem);
    mainLayout->setStretchFactor(stretchItem, 1);

    setLayout(mainLayout);
    retranslateUi();
    connect(&onscreenPlugins, SIGNAL(activeSubViewChanged()), this, SLOT(syncActiveSubView()));
    connect(&onscreenPlugins, SIGNAL(enabledPluginsChanged()), this, SLOT(updateSelectedSubViews()));
}

void MImSettingsWidget::showAvailableSubViewList()
{
    if (!availableSubViewList) {
        availableSubViewList = new MPopupList();
        availableSubViewList.data()->setTitle(qtTrId(MImSettingsCommon::QtnTxtsActiveInputMethod));
        connect(availableSubViewList.data(), SIGNAL(clicked(const QModelIndex &)),
                this, SLOT(setActiveSubView(const QModelIndex &)));
    }
    // always update available subview model avoid missing some updated subviews
    updateAvailableSubViewModel();
    availableSubViewList.data()->appear(MSceneWindow::DestroyWhenDone);
}

void MImSettingsWidget::updateAvailableSubViewModel()
{
    if (!availableSubViewList)
        return;

    availableSubViewList.data()->setItemModel(new MImSubviewModel(selectedSubViews, availableSubViewList.data()));

    updateActiveSubViewIndex();
}

void MImSettingsWidget::updateActiveSubViewIndex()
{
    if (!availableSubViewList)
        return;

    const MImOnScreenPlugins::SubView &activeSubView = onscreenPlugins.activeSubView();

    QAbstractItemModel *model = availableSubViewList.data()->itemModel();

    for (int row = 0; row < model->rowCount(); ++row) {
        const QModelIndex &index = model->index(row, 0);
        const MImOnScreenPlugins::SubView &subview = index.data(MImSubviewModel::SubViewRole).value<MImOnScreenPlugins::SubView>();
        if (activeSubView == subview) {
            availableSubViewList.data()->setCurrentIndex(index);
            availableSubViewList.data()->scrollTo(index);
            break;
        }
    }
}

void MImSettingsWidget::setActiveSubView(const QModelIndex &index)
{
    if (!index.isValid() || !availableSubViewList
        || !availableSubViewList.data()->selectionModel()->isSelected(index))
        return;

    availableSubViewList.data()->setCurrentIndex(index);

    const QVariant &data = index.data(MImSubviewModel::SubViewRole);
    const MImOnScreenPlugins::SubView &subview = data.value<MImOnScreenPlugins::SubView>();
    onscreenPlugins.setActiveSubView(subview);
}

void MImSettingsWidget::showSelectedKeyboardsDialog()
{
    if (!keyboardDialog) {
        keyboardDialog = new MDialog;

        keyboardList = new MList(keyboardDialog.data());
        keyboardList.data()->setCellCreator(new MSubViewCellCreator);
        keyboardList.data()->setSelectionMode(MList::MultiSelection);

        keyboardDialog.data()->setCentralWidget(keyboardList.data());
        keyboardDialog.data()->addButton(M::DoneButton);

        connect(keyboardList.data(), SIGNAL(itemClicked(const QModelIndex &)),
                this, SLOT(updateSelectedKeyboards(const QModelIndex &)));
        connect(keyboardDialog.data(), SIGNAL(accepted()),
                this, SLOT(selectKeyboards()));
    }

    updateSelectedKeyboardsModel();
    updateSelectedKeyboards();

    keyboardDialog.data()->appear(MSceneWindow::DestroyWhenDone);
}

void MImSettingsWidget::updateSelectedKeyboardsModel()
{
    if (!keyboardList)
        return;

    QList<MImSubview> subViews;
    foreach (const MImSettingsConf::MImSubView &subview, MImSettingsConf::instance().subViews()) {
        if (subview.pluginName != SwypePlugin) {
            subViews.push_back(MImSubview(subview.subViewId,
                                          subview.subViewTitle,
                                          subview.pluginName));
        }
    }

    // sort based on subview title
    qSort(subViews.begin(), subViews.end(), mImSubviewLessThan);

    keyboardList.data()->setItemModel(new MImSubviewModel(subViews, keyboardList.data()));

    updateSelectedKeyboardsSelection();
}

void MImSettingsWidget::updateSelectedKeyboardsSelection()
{
    if (!keyboardList)
        return;

    QAbstractItemModel *model = keyboardList.data()->itemModel();
    QItemSelectionModel *selection = keyboardList.data()->selectionModel();
    selection->clear();

    for (int i = 0; i < model->rowCount(); i++) {
        const QModelIndex &index = model->index(i, 0);
        const QVariant &data = model->data(index, MImSubviewModel::SubViewRole);
        const MImOnScreenPlugins::SubView &subView = data.value<MImOnScreenPlugins::SubView>();

        if (onscreenPlugins.isSubViewEnabled(subView)) {
            selection->select(index, QItemSelectionModel::Select);
        }
    }
}

void MImSettingsWidget::updateSelectedKeyboards(const QModelIndex &)
{
    if (!keyboardList || !keyboardDialog)
        return;

    const QModelIndexList &indexList = keyboardList.data()->selectionModel()->selectedIndexes();

    QString title = qtTrId(MImSettingsCommon::QtnTxtsInstalledKeyboards).arg(indexList.size());
    keyboardDialog.data()->setTitle(title);
}

void MImSettingsWidget::selectKeyboards()
{
    if (!keyboardList)
        return;

    QModelIndexList indexList = keyboardList.data()->selectionModel()->selectedIndexes();

    QList<MImOnScreenPlugins::SubView> selected;
    foreach (const QModelIndex &i, indexList) {
        selected.push_back(i.data(MImSubviewModel::SubViewRole).value<MImOnScreenPlugins::SubView>());
    }

    if (onscreenPlugins.isEnabled(SwypePlugin)) {
        selected.push_back(MImOnScreenPlugins::SubView(SwypePlugin, SwypeSubviewName));
    }
    onscreenPlugins.setEnabledSubViews(selected);

    updateSelectedSubViews();

    // "No keyboard is selected" notification
    if (selected.isEmpty()) {
        MBanner *noKeyboardsNotification = new MBanner;

        // It is needed to set the proper style name to have properly wrapped, multiple lines
        // with too much content. The MBanner documentation also emphasises to specify the
        // style name for the banners explicitly in the code.
        noKeyboardsNotification->setStyleName(MImSettingsCommon::StyleInfoBanner);
        noKeyboardsNotification->setTitle(qtTrId(MImSettingsCommon::QtnTxtsNoKeyboardsNotification));
        noKeyboardsNotification->appear(MSceneWindow::DestroyWhenDone);
    }
    //update titles
    retranslateUi();
}

void MImSettingsWidget::showPluginSettings(const QString& pluginName)
{
    // Owner applet needs to know plugin's name:
    ownerApplet->storePluginName(pluginName);
    // Now it's OK to change widget.
    emit changeWidget(MImSettingsCommon::SwypeSettings);
}

void MImSettingsWidget::retranslateUi()
{
    if (!activeSubViewItem || !headerLabel || !availableSubViewItem)
        return;

    headerLabel->setText(qtTrId(MImSettingsCommon::QtnTxtsTextInput));
    activeSubViewItem->setTitle(qtTrId(MImSettingsCommon::QtnTxtsActiveInputMethod));
    updateActiveSubViewTitle();
    updateSelectedSubviewsTitle();

    settingsContainers.retranslate();

    if (!availableSubViewList.isNull()) {
        availableSubViewList.data()->setTitle(qtTrId(MImSettingsCommon::QtnTxtsActiveInputMethod));
    }
}

void MImSettingsWidget::syncActiveSubView()
{
    // update setting
    updateActiveSubViewIndex();
    updateActiveSubViewTitle();
    updateSelectedSubviewsTitle();
}

void MImSettingsWidget::updateActiveSubViewTitle()
{
    if (!activeSubViewItem)
        return;

    const MImOnScreenPlugins::SubView &activeSubView = onscreenPlugins.activeSubView();

    foreach (const MImSubview &subView, selectedSubViews) {
        if (subView.pluginId == activeSubView.plugin
                && subView.id == activeSubView.id) {
            activeSubViewItem->setSubtitle(subView.displayName);
            break;
        }
    }
}


void MImSettingsWidget::updateSelectedSubViews()
{
    selectedSubViews.clear();
    foreach (const MImSettingsConf::MImSubView &subview, MImSettingsConf::instance().subViews()) {
        if (onscreenPlugins.isSubViewEnabled(MImOnScreenPlugins::SubView(subview.pluginName, subview.subViewId))) {
            selectedSubViews.push_back(MImSubview(subview.subViewId, subview.subViewTitle, subview.pluginName));
        }
    }
    updatePluginContainerVisibility();
}

void MImSettingsWidget::pluginLoaded(const QString &pluginName)
{
    if (pluginName == SwypePlugin) {
        createSwypeSettingsItem();
        return;
    }
    settingsContainers.addPluginSettings(pluginName,
                                         plugins->settingsFor(pluginName));
}

void MImSettingsWidget::pluginLoadDone()
{
    if (spinner) {
        mainLayout->removeItem(spinner);
        delete spinner;
        spinner = 0;
    }
    if (swypePluginEnabler)
        swypePluginEnabler->setVisible(true);
    updatePluginContainerVisibility();
}

void MImSettingsWidget::updateSelectedSubviewsTitle()
{
    // Filter out Swype plugin form selected subviews list in order to
    // hide it from the "Installed keyboards" list:
    QList<MImSubview> filtered;
    std::remove_copy_if(selectedSubViews.begin(), selectedSubViews.end(),
                        std::back_inserter(filtered),
                        PluginEquals(SwypePlugin));

    QString title = qtTrId(MImSettingsCommon::QtnTxtsInstalledKeyboards).arg(filtered.size());
    availableSubViewItem->setTitle(title);
    if (selectedSubViews.empty()) {
        availableSubViewItem->setSubtitle(qtTrId(MImSettingsCommon::QtnTxtsNoKeyboards));
    } else {
        QStringList names;
        foreach (const MImSubview &subView, filtered) {
            if (MLocale::directionForText(subView.displayName) == Qt::RightToLeft) {
                // Surround RTL name with RIGHT-TO-LEFT EMBEDDING (0x202B) mark
                // and POP DIRECTIONAL FORMATTING (0x202C) mark
                names.push_back(QChar(0x202B) + subView.displayName + QChar(0x202C));
            } else {
                // Surround LTR name with LEFT-TO-RIGHT EMBEDDING (0x202B) mark
                // and POP DIRECTIONAL FORMATTING (0x202C) mark
                names.push_back(QChar(0x202A) + subView.displayName + QChar(0x202C));
            }
        }

        MLocale locale;
        if (locale.localeScripts()[0] == "Arab") {
            // RIGHT-TO-LEFT MARK mark in front, use Arabic comma as separator
            availableSubViewItem->setSubtitle(QChar(0x200F) + names.join(QChar(0x060C) + QLatin1String(" ")));
        } else if (locale.localeScripts()[0] == "Hebr") {
            // RIGHT-TO-LEFT MARK mark in front, use normal comma as separator
            availableSubViewItem->setSubtitle(QChar(0x200F) + names.join(", "));
        } else {
            // LEFT-TO-RIGHT MARK mark in front, use normal comma as separator
            availableSubViewItem->setSubtitle(QChar(0x200E) + names.join(", "));
        }
    }
}

void MImSettingsWidget::createSwypeSettingsItem()
{
    if (swypePluginEnabler)
        return;
    swypePluginEnabler =
            new MImPluginEnablerWidget(SwypePlugin,
                                       SwypeSubviewName,
                                       onscreenPlugins,
                                       MImOnScreenPlugins::SubView(SwypePlugin, SwypeSubviewName));
    swypePluginEnabler->setVisible(false);
    mainLayout->insertItem(FirstPluginContainerIndex, swypePluginEnabler);
    mainLayout->setStretchFactor(swypePluginEnabler, 0);
    connect(swypePluginEnabler, SIGNAL(showSettings(QString)),
            this, SLOT(showPluginSettings(QString)));
}

void MImSettingsWidget::updatePluginContainerVisibility()
{
    if (mainLayout) {
        for (MImSettingsContainerStore::const_iterator iter = settingsContainers.constBegin()
            ; iter != settingsContainers.constEnd(); ++iter) {
            updatePluginContainerVisibility(iter.key(), iter.value());
        }
        mainLayout->activate();
    }
}

void MImSettingsWidget::updatePluginContainerVisibility(const QString &plugin,
                                                        MContainer *container)
{
    int index = -1;  // where to put the container (negative -> don't insert)
    if (onscreenPlugins.isEnabled(plugin)) {
        if (!container->isVisible()) {
            index = mainLayout->count() - 1; //insert before the final stretch
            if (plugin == DefaultPlugin) {
                index = FirstPluginContainerIndex;
                if (swypePluginEnabler) {
                    ++index;
                }
            }
        }
    } else {
        container->setVisible(false);
        mainLayout->removeItem(container);
    }
    if (index >= 0) {
        mainLayout->insertItem(index, container);
        mainLayout->setStretchFactor(container, 0);
        container->setVisible(true);
    }
}
