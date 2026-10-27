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
#include "drilldownitem.h"

#include <MWidget>
#include <MLabel>
#include <MImageWidget>
#include <QGraphicsGridLayout>

#undef DEBUG
#define WARNING
#include "../debug.h"

RightArrowItem::RightArrowItem (
        MBasicListItem::ItemStyle  style, 
        QGraphicsItem             *parent) : 
    MBasicListItem (style, parent)
{
}

QGraphicsLayout *
RightArrowItem::createLayout()
{
    QGraphicsGridLayout *layout;
    MLabel              *titleLabel;
    MLabel              *subTitleLabel;
    MImageWidget        *iconWidget;
    MImageWidget        *drillIconWidget;
    
    layout = new QGraphicsGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    titleLabel =  titleLabelWidget ();
    subTitleLabel = subtitleLabelWidget ();
    iconWidget = imageWidget();

    drillIconWidget = new MImageWidget ();
    drillIconWidget->setImage ("icon-m-common-drilldown-arrow");
    
    drillIconWidget->setStyleName ("CommonDrillDownIcon");
    titleLabel->setStyleName ("CommonTitleInverted");
    subTitleLabel->setStyleName ("CommonSubTitleInverted");

    switch (itemStyle()) {
        case TitleWithSubtitle:
            /*
             * The title label.
             */
            layout->addItem (titleLabel, 0, 0);
            layout->setAlignment(titleLabel, Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The sub-title label.
             */
            layout->addItem (subTitleLabel, 1, 0);
            layout->setAlignment (subTitleLabel, Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The drill down icon.
             */
            layout->addItem(drillIconWidget, 0, 1, 2, 1);
            layout->setAlignment (drillIconWidget, Qt::AlignVCenter | Qt::AlignRight);
            break;

        case IconWithTitleAndSubtitle:
            /*
             * The left side icon.
             */
            layout->addItem (iconWidget, 0, 0, 2, 1);
            layout->setAlignment (iconWidget, Qt::AlignVCenter | Qt::AlignRight);
            /*
             * The title label.
             */
            layout->addItem (titleLabel, 0, 1);
            layout->setAlignment (titleLabel, Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The sub-title label.
             */
            layout->addItem (subTitleLabel, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The drill down icon.
             */
            layout->addItem(drillIconWidget, 0, 2, 2, 1);
            layout->setAlignment (drillIconWidget, Qt::AlignVCenter | Qt::AlignRight);
            break;

        case IconWithTitle:
            /*
             * The left side icon.
             */
            layout->addItem (iconWidget, 0, 0);
            layout->setAlignment (iconWidget, Qt::AlignVCenter | Qt::AlignRight);
            /*
             * The title label.
             */
            layout->addItem (titleLabel, 0, 1);
            layout->setAlignment (titleLabel, Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The drill down icon.
             */
            layout->addItem(drillIconWidget, 0, 2);
            layout->setAlignment (drillIconWidget, Qt::AlignVCenter | Qt::AlignRight);
            break;

        default:
            SYS_WARNING ("itemStyle not supported.");
    }

    setStyleName ("CommonPanelInverted");
    return layout;
}

