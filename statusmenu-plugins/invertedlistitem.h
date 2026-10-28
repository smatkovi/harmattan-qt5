/*
 * A row of the status indicator menu as the N9's draws them: a basic list
 * item light on black (the "Inverted" common styles; its labels are only
 * reachable from a subclass).
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 */
#ifndef INVERTEDLISTITEM_H
#define INVERTEDLISTITEM_H

#include <MBasicListItem>
#include <MLabel>

class InvertedListItem : public MBasicListItem
{
public:
    InvertedListItem() : MBasicListItem(MBasicListItem::IconWithTitleAndSubtitle)
    {
        setStyleName("CommonBasicListItemInverted");
        titleLabelWidget()->setStyleName("CommonTitleInverted");
        subtitleLabelWidget()->setStyleName("CommonSubTitleInverted");
    }
};

#endif
