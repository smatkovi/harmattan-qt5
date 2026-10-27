/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelapplets.
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
#include <MApplicationExtensionArea>

MApplicationExtensionArea::MApplicationExtensionArea (
        const QString       &interface,
        QGraphicsItem       *parent)
{
    Q_UNUSED (interface);
    Q_UNUSED (parent);
}

MApplicationExtensionArea::~MApplicationExtensionArea ()
{

}

void
MApplicationExtensionArea::setInProcessFilter (
       const QRegExp       &inProcessFilter)
{
    Q_UNUSED (inProcessFilter);
}
void
MApplicationExtensionArea::setOutOfProcessFilter (
        const QRegExp       &outOfProcessFilter)
{
    Q_UNUSED (outOfProcessFilter);
}

bool
MApplicationExtensionArea::init ()
{
    return true;
}

