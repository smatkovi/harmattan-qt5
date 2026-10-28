/***************************************************************************
**
** The Nokia N9's Events view, as the first page of the home screen's
** three: the notifications of the system UI. Written anew for the Qt 5
** port: the N9's home (1.3.2) and its event feeds (libmeegotouchevents)
** were not released.
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation
** and appearing in the file LICENSE.LGPL included in the packaging
** of this file.
**
****************************************************************************/

#ifndef EVENTSPAGE_H
#define EVENTSPAGE_H

#include <MWidget>

class MLabel;
class MLayout;
class MLinearLayoutPolicy;
class MPannableViewport;
class MStylableWidget;

class EventsPage : public MWidget
{
    Q_OBJECT

public:
    EventsPage(QGraphicsItem *parent = 0);
    virtual ~EventsPage();

private:
    MPannableViewport *viewport;
    // styled (EventsList): the margins clear of the status bar
    MStylableWidget *list;
    MLayout *layout;
    MLinearLayoutPolicy *policy;
    MLabel *header;
    MLabel *empty;
};

#endif
