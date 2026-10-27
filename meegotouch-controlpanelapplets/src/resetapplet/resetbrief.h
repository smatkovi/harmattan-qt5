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
#ifndef RESETBRIEF_H
#define RESETBRIEF_H

#include <QPointer>
#include "dcpbrief.h"

#include "resetbusinesslogic.h"


class ResetBrief: public DcpBrief
{
    Q_OBJECT

public:
    ResetBrief (ResetBusinessLogic *businessLogic);

    virtual QString valueText() const;
    virtual int widgetTypeID() const;

private:
    QPointer<ResetBusinessLogic> m_ResetBusinessLogic;
};

#endif
