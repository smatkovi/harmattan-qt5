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
#ifndef _ALERT_TONE_WIDGET_H_
#define _ALERT_TONE_WIDGET_H_

#include "alerttone.h"
#include "drilldownitem.h"

class AlertToneWidget : public RightArrowItem
{
Q_OBJECT

public:
	AlertToneWidget (
            AlertTone             *tone,
            int                    idx,
            QGraphicsItem         *parent = 0);

private slots:
    void alertToneChanged ();
    void clicked ();

signals:
    void showWidget (int widgetId);

protected:
    void retranslateUi ();

private:
	AlertTone   *m_tone;
	int          m_idx;

#ifdef UNIT_TEST
    friend class Ut_AlertToneWidgetTests;
#endif
};

#endif

