/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#include "mimpluginenablerwidget.h"

#include <QGraphicsLinearLayout>
#include <MButton>
#include <MLabel>
#include <MImageWidget>
#include <MSeparator>

#include "mimsettingscommon.h"
#include "mimsettingslistitem.h"

namespace {
    const char * const ObjectNameEnableSwypeButton("EnableSwypeButton");
    const char * const EnabledArrowIcon("icon-m-common-drilldown-arrow-inverse");
    const char * const DisabledArrowIcon("icon-m-common-drilldown-arrow-inverse-disabled");
}

MImPluginEnablerWidget::MImPluginEnablerWidget(const QString &aPluginName,
                                               const QString &aDisplayName,
                                               MImOnScreenPlugins &osp,
                                               MImOnScreenPlugins::SubView aSubview,
                                               QGraphicsItem *parent)
    : MContainer(parent)
    , pluginName(aPluginName)
    , displayName(aDisplayName)
    , onScreenPlugins(osp)
    , subview(aSubview)
    , enableButton(0)
    , pluginTitle(0)
    , drillDownItem(0)
    , drillDownArrow(0)
{
    setContentsMargins(0., 0., 0., 0.);
    setStyleName(MImSettingsCommon::StylePluginContainer);
    setHeaderVisible(false);

    QGraphicsLinearLayout *topLayout = new QGraphicsLinearLayout(Qt::Vertical);
    topLayout->setContentsMargins(0., 0., 0., 0.);
    topLayout->setSpacing(0.);
    centralWidget()->setLayout(topLayout);

    MSeparator *separator(new MSeparator);
    separator->setStyleName(MImSettingsCommon::StylePluginHeaderDivider);
    topLayout->addItem(separator);

    pluginTitle = new MLabel;
    pluginTitle->setStyleName(MImSettingsCommon::StyleSingleTitle);

    enableButton = new MButton(this);
    enableButton->setObjectName(ObjectNameEnableSwypeButton);
    enableButton->setStyleName(MImSettingsCommon::StyleSwitch);
    enableButton->setViewType(MButton::switchType);
    enableButton->setCheckable(true);
    connect(enableButton, SIGNAL(toggled(bool)),
            this, SLOT(enablePlugin(bool)));

    QGraphicsLinearLayout *hLayout = new QGraphicsLinearLayout(Qt::Horizontal);
    hLayout->setContentsMargins(0., 0., 0., 0.);
    hLayout->setSpacing(0.);
    hLayout->addItem(pluginTitle);
    hLayout->addItem(enableButton);
    hLayout->setAlignment(enableButton, Qt::AlignCenter);

    topLayout->addItem(hLayout);
    topLayout->setStretchFactor(hLayout, 0);

    drillDownItem = new MImSettingsListItem(MBasicListItem::IconWithTitle);
    drillDownArrow = new MImageWidget;
    drillDownArrow->setStyleName(MImSettingsCommon::StyleImageDrillDown);
    drillDownItem->setImageWidget(drillDownArrow);
    drillDownItem->setObjectName("ShowSwypeItem");
    drillDownItem->setStyleName(MImSettingsCommon::StyleSettingsListItem);

    connect(drillDownItem, SIGNAL(clicked()),  this, SLOT(showSettings()));

    topLayout->addItem(drillDownItem);
    topLayout->setStretchFactor(drillDownItem, 0);

    connect(&onScreenPlugins, SIGNAL(enabledPluginsChanged()),
            this, SLOT(updateEnabledState()));

    updateEnabledState();
    setDrillDownEnabled(enableButton->isChecked());
    retranslateUi();
}

MImPluginEnablerWidget::~MImPluginEnablerWidget()
{
    // No need to do anything here.
}

void MImPluginEnablerWidget::updateEnabledState()
{
    bool enabled = onScreenPlugins.isSubViewEnabled(subview);
    enableButton->setChecked(enabled);
}

void MImPluginEnablerWidget::enablePlugin(bool enabled)
{
    QList<MImOnScreenPlugins::SubView> selected
            = onScreenPlugins.enabledSubViews();
    bool pluginSelected = selected.contains(subview);
    bool activate = false;
    if (enabled && !pluginSelected) {
        selected.push_back(subview);
        activate = true;
    } else if (!enabled && pluginSelected) {
        selected.removeAll(subview);
    }
    onScreenPlugins.setEnabledSubViews(selected);

    if (activate) {
        onScreenPlugins.setActiveSubView(subview);
    }

    setDrillDownEnabled(enabled);
}

void MImPluginEnablerWidget::showSettings()
{
    emit showSettings(pluginName);
}

void MImPluginEnablerWidget::retranslateUi()
{
    pluginTitle->setText(displayName);
    drillDownItem->setTitle(
            QString(qtTrId(MImSettingsCommon::QtnCommonSettings)));
}

void MImPluginEnablerWidget::setDrillDownEnabled(bool enabled)
{
    drillDownItem->setEnabled(enabled);
    if (enabled)
        drillDownArrow->setImage(EnabledArrowIcon);
    else
        drillDownArrow->setImage(DisabledArrowIcon);
}
