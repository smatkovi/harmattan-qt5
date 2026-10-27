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

#include "mimswypesettingswidget.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsWidget>
#include <QDebug>

#include <MLabel>
#include <MSeparator>
#include <mabstractinputmethodsettings.h>

#include "mimsettingscommon.h"

MImSwypeSettingsWidget::MImSwypeSettingsWidget(MAbstractInputMethodSettings *s,
                                               QGraphicsWidget *parent)
    : DcpStylableWidget(parent)
    , mainLayout(0)
    , headerLabel(0)
    , settings(s)
{
    initWidget();
}

MImSwypeSettingsWidget::~MImSwypeSettingsWidget()
{
}

void MImSwypeSettingsWidget::initWidget()
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

    if (settings)
    {
        QGraphicsWidget *settingWidget = settings->createContentWidget();
        if (settingWidget) {
            mainLayout->addItem(settingWidget);
        }
    }

    // Final strech item
    QGraphicsWidget *stretchItem = new QGraphicsWidget;
    stretchItem->setPreferredSize(QSizeF());
    mainLayout->addItem(stretchItem);
    mainLayout->setStretchFactor(stretchItem, 1);

    setLayout(mainLayout);
    retranslateUi();
}

void MImSwypeSettingsWidget::retranslateUi()
{
    headerLabel->setText(title());
}

bool MImSwypeSettingsWidget::back()
{
    return true;
}

QString MImSwypeSettingsWidget::title() const
{
    if (!settings)
        return qtTrId(MImSettingsCommon::QtnCommonSettings);
    else
        return settings->title();
}

