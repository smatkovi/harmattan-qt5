/***************************************************************************
**
** Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of libmeegotouch.
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

#include <QMetaMethod>
#include "mlistitem.h"
#include "mlistitem_p.h"

#include "mwidgetcreator.h"

M_REGISTER_WIDGET(MListItem)
      
MListItemPrivate::MListItemPrivate()
    : MWidgetControllerPrivate()
{
}

MListItemPrivate::~MListItemPrivate()
{
}

void MListItemPrivate::updateLongTapConnections()
{
    Q_Q(MListItem);
    if (q->receivers(SIGNAL(longTapped(QPointF))) > 0)
        q->grabGestureWithCancelPolicy(Qt::TapAndHoldGesture, Qt::GestureFlags(), MWidget::MouseEventCancelOnGestureFinished);
    else
        q->ungrabGesture(Qt::TapAndHoldGesture);
}

MListItem::MListItem(QGraphicsItem *parent)
    : MWidgetController(new MListItemPrivate, new MListItemModel, parent)
{
}

MListItem::~MListItem()
{
}

void MListItem::click()
{
    emit clicked();
}

void MListItem::longTap(const QPointF &pos)
{
    emit longTapped(pos);
}

// as SIGNAL() writes a signal: Qt 5 passes connectNotify() a QMetaMethod
static QByteArray signalString(const QMetaMethod &signal)
{
    return QByteArray("2") + signal.methodSignature();
}

void MListItem::connectNotify(const QMetaMethod &signal)
{
    Q_D(MListItem);

    if (signalString(signal) == SIGNAL(longTapped(QPointF))) {
        d->updateLongTapConnections();
    }
}

void MListItem::disconnectNotify(const QMetaMethod &signal)
{
    Q_D(MListItem);

    if (signalString(signal) == SIGNAL(longTapped(QPointF))) {
        d->updateLongTapConnections();
    }
}
