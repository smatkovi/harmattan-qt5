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

#include <MWidgetController>
#include <MLabel>
#include <MImageWidget>
#include <QGraphicsGridLayout>
#include <QGraphicsLinearLayout>
#include <MLocale>

#include "../styles.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

/******************************************************************************
 *
 */
RightArrowItem::RightArrowItem (
        MBasicListItem::ItemStyle  style, 
        QGraphicsItem             *parent) : 
    MBasicListItem (style, parent),
    m_TitleStyleName ("CommonTitleInverted")
{
}

QGraphicsLayout *
RightArrowItem::createLayout()
{
    QGraphicsGridLayout *layout;
    MWidget             *spacer;
    MLabel              *titleLabel;
    MLabel              *subTitleLabel = 0;
    MImageWidget        *iconWidget    = 0;
    MImageWidget        *drillIconWidget;
    Qt::Alignment        textAlignment = getTextAlignment ();
    
    layout = new QGraphicsGridLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    titleLabel = titleLabelWidget ();
    titleLabel->setWordWrap (true);

    drillIconWidget = new MImageWidget ("icon-m-common-drilldown-arrow-inverse");
    drillIconWidget->setStyleName ("CommonDrillDownIcon");

    titleLabel->setStyleName(m_TitleStyleName);

    switch (itemStyle()) {
        case TitleWithSubtitle:
            SYS_DEBUG ("TitleWithSubtitle");
            /*
             * The title label.
             */
            layout->addItem (titleLabel, 0, 0);
            layout->setAlignment(titleLabel, 
                    Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The sub-title label.
             */
            subTitleLabel = subtitleLabelWidget ();
            subTitleLabel->setStyleName("CommonSubTitleInverted");
            layout->addItem (subTitleLabel, 1, 0);
            layout->setAlignment (subTitleLabel, 
                    Qt::AlignLeft | Qt::AlignVCenter);

            spacer = new MWidget;
            layout->addItem (spacer, 2, 0);

            /*
             * The drill down icon.
             */
            layout->addItem(drillIconWidget, 0, 1, 3, 1);
            layout->setAlignment (drillIconWidget, Qt::AlignVCenter | Qt::AlignRight);
            break;

        case IconWithTitleAndSubtitle:
            SYS_DEBUG ("IconWithTitleAndSubtitle");
            /*
             * The left side icon.
             */
            iconWidget = imageWidget();
            layout->addItem (iconWidget, 0, 0, 2, 1);
            layout->setAlignment (iconWidget, Qt::AlignVCenter | Qt::AlignRight);
            /*
             * The title label.
             */
            layout->addItem (titleLabel, 0, 1);
            layout->setAlignment (titleLabel, 
                    Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The sub-title label.
             */
            subTitleLabel = subtitleLabelWidget ();
            subTitleLabel->setStyleName("CommonSubTitleInverted");
            layout->addItem (subTitleLabel, 1, 1, 
                    Qt::AlignLeft | Qt::AlignVCenter);
            /*
             * The drill down icon.
             */
            layout->addItem(drillIconWidget, 0, 2, 2, 1);
            layout->setAlignment (drillIconWidget, 
                    Qt::AlignVCenter | Qt::AlignRight);
            break;

        case IconWithTitle:
            SYS_DEBUG ("IconWithTitle");

            /*
             * The left side icon.
             */
            iconWidget = imageWidget();
            layout->addItem (iconWidget, 0, 0);
            layout->setAlignment (iconWidget, 
                    Qt::AlignVCenter | Qt::AlignRight);
            /*
             * The title label.
             */
            titleLabel->setAlignment (textAlignment | Qt::AlignVCenter);
            layout->addItem (titleLabel, 0, 1);
            layout->setAlignment (titleLabel, 
                    Qt::AlignLeft | Qt::AlignVCenter);
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

void 
RightArrowItem::setTitleStyleName (
        const QString &styleName)
{
    m_TitleStyleName = styleName;
}

Qt::Alignment
RightArrowItem::getTextAlignment () const
{
    Qt::LayoutDirection textDir;

    /*
     * We use this label text as an example to decide what should be the text
     * alignment.
     */
    textDir = MLocale::directionForText (qtTrId ("qtn_sond_pick_music"));

    if (textDir == Qt::RightToLeft) {
        return (Qt::AlignAbsolute | Qt::AlignRight);
    }

    return Qt::AlignLeft;
}
