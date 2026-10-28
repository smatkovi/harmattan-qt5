/***************************************************************************
**
** The Nokia N9's Events view (see eventspage.h).
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#include "eventspage.h"

#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <MPannableViewport>
#include <MStylableWidget>
#include <QGraphicsLinearLayout>

EventsPage::EventsPage(QGraphicsItem *parent) :
    MWidget(parent),
    viewport(new MPannableViewport),
    list(new MStylableWidget),
    layout(new MLayout),
    policy(new MLinearLayoutPolicy(layout, Qt::Vertical))
{
    QGraphicsLinearLayout *pageLayout = new QGraphicsLinearLayout(Qt::Vertical, this);
    pageLayout->setContentsMargins(0, 0, 0, 0);
    pageLayout->addItem(viewport);

    layout->setContentsMargins(0, 0, 0, 0);
    list->setStyleName("EventsList");
    list->setLayout(layout);

    // (no translation catalogue for the port's own texts yet)
    header = new MLabel("Events");
    header->setStyleName("CommonHeaderInverted");
    policy->addItem(header);
    empty = new MLabel("No events");
    empty->setStyleName("CommonBodyTextInverted");
    empty->setAlignment(Qt::AlignCenter);
    policy->addItem(empty);

    // vertical only: a sideways pan belongs to the home's pages
    viewport->setObjectName("EventsViewport");
    viewport->setPanDirection(Qt::Vertical);
    viewport->setWidget(list);
}

EventsPage::~EventsPage()
{
}
