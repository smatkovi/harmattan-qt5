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

#include "mimsettingscontainerstore.h"

#include <QGraphicsWidget>
#include <QGraphicsLinearLayout>
#include <QMap>
#include <QString>
#include <QDebug>

#include <MContainer>
#include <MSeparator>
#include <MLabel>
#include <mabstractinputmethodsettings.h>

#include "mimsettingsconf.h"
#include "mimsettingscommon.h"

MImSettingsContainerStore::MImSettingsContainerStore()
    : settingsContainerMap()
    , settingsLabelMap()
    , containers()
{
}

MImSettingsContainerStore::~MImSettingsContainerStore()
{
    foreach (QPointer<MContainer> c, containers) {
        delete c.data();
    }
}

void MImSettingsContainerStore::retranslate()
{
    foreach (MLabel *label, settingsLabelMap.values()) {
        label->setText(settingsLabelMap.key(label)->title());
    }
}


MContainer *MImSettingsContainerStore::addPluginSettings(const QString &plugin,
                                           MAbstractInputMethodSettings *settings)
{
    if (!settings) {
        return 0;
    }

    QGraphicsWidget *contentWidget = settings->createContentWidget();
    if (!contentWidget) {
        return 0;
    }

    QGraphicsLinearLayout *layout = new QGraphicsLinearLayout(Qt::Vertical);
    layout->setContentsMargins(0, 0, 0, 0);

    MContainer *container = new MContainer;
    container->setVisible(false);
    container->setStyleName(MImSettingsCommon::StylePluginContainer);
    container->setHeaderVisible(false);
    container->centralWidget()->setLayout(layout);

    MLabel *header = new MLabel(settings->title());
    header->setStyleName(MImSettingsCommon::StylePluginHeader);

    MContainer *headerContainer(new MContainer);
    headerContainer->setContentsMargins(0, 0, 0, 0);
    headerContainer->setStyleName(MImSettingsCommon::StylePluginHeaderContainer);
    headerContainer->setHeaderVisible(false);

    MSeparator *headerSeparator(new MSeparator);
    headerSeparator->setStyleName(MImSettingsCommon::StylePluginHeaderDivider);

    QGraphicsLinearLayout *headerLayout(new QGraphicsLinearLayout(Qt::Horizontal));
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(0);

    headerLayout->addItem(headerSeparator);
    headerLayout->setStretchFactor(headerSeparator, 2);

    headerLayout->addItem(header);
    headerLayout->setAlignment(header, Qt::AlignLeft);
    headerLayout->setStretchFactor(header, 0);

    headerContainer->centralWidget()->setLayout(headerLayout);

    //TODO: icon for the settings.
    layout->addItem(headerContainer);
    layout->addItem(contentWidget);

    settingsLabelMap.insert(settings, header);
    settingsContainerMap.insert(plugin, container);
    containers.push_back(container);
    return container;
}
