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

#ifndef MIMSETTINGSWIDGET_H
#define MIMSETTINGSWIDGET_H

#include <DcpStylableWidget>
#include <QModelIndex>
#include <QPointer>

#include "mimsubviewmodel.h"
#include "mimsettingscontainerstore.h"

class MPopupList;
class MAbstractInputMethodSettings;
class MDialog;
class MLabel;
class MList;
class MContainer;
class QGraphicsLinearLayout;
class MImSettingsListItem;
class MImOnSreenPlugins;
class MImPlugins;
class MProgressIndicator;
class MImPluginEnablerWidget;
class MImSettingsApplet;

class MImSettingsWidget : public DcpStylableWidget
{
    Q_OBJECT

public:
    MImSettingsWidget(MImOnScreenPlugins& onscreenPlugins,
                      MImSettingsApplet *applet,
                      MImPlugins *plugins);
    virtual ~MImSettingsWidget();

protected:
    void initWidget();

    //! reimp
    virtual void retranslateUi();
    virtual bool back();
    virtual QString title() const;
    virtual void enterDisplayEvent();
    //! reimp_end

private slots:
    void syncActiveSubView();
    void showAvailableSubViewList();
    void setActiveSubView(const QModelIndex &);
    void updateAvailableSubViewModel();
    void updateActiveSubViewIndex();

    void showSelectedKeyboardsDialog();
    void updateSelectedKeyboards(const QModelIndex &index = QModelIndex());
    void selectKeyboards();
    void showPluginSettings(const QString& pluginName);
    void updateSelectedSubViews();

    void pluginLoaded(const QString& pluginName);
    void pluginLoadDone();

private:
    void updateSelectedSubviewsTitle();
    void updateActiveSubViewTitle();
    void updateSelectedKeyboardsModel();
    void updateSelectedKeyboardsSelection();

    void createSwypeSettingsItem();
    void updatePluginContainerVisibility();
    void updatePluginContainerVisibility(const QString &plugin, MContainer *container);

    QGraphicsLinearLayout* mainLayout;
    MLabel *headerLabel;
    MImSettingsListItem *activeSubViewItem;
    MImSettingsListItem *availableSubViewItem;
    MImPluginEnablerWidget *swypePluginEnabler;

    // QPointer: Qt 5's QWeakPointer is no longer made from a plain QObject pointer
    QPointer<MPopupList> availableSubViewList;
    QPointer<MDialog> keyboardDialog;
    QPointer<MList> keyboardList;

    MImOnScreenPlugins& onscreenPlugins;
    QList<MImSubview> selectedSubViews;

    MImSettingsContainerStore settingsContainers;

    MImPlugins *plugins;
    MProgressIndicator *spinner;
    bool doLoadPlugins;
    MImSettingsApplet *ownerApplet;
};
#endif
