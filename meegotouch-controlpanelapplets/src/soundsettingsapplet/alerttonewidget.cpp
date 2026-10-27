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
#include "alerttonewidget.h"
#include "alerttoneappletmaps.h"
#include "soundsettingsapplet.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

AlertToneWidget::AlertToneWidget (
        AlertTone           *tone, 
        int                  idx, 
        QGraphicsItem       *parent) :
    RightArrowItem (MBasicListItem::TitleWithSubtitle, parent),
	m_tone (tone),
	m_idx (idx)
{
    /*
     * Connect to signals
     */
	connect (m_tone, SIGNAL (changed ()), SLOT (alertToneChanged ()));
	connect (m_tone, SIGNAL (refreshed ()), SLOT (alertToneChanged ()));
	connect (this, SIGNAL (clicked ()), SLOT (clicked ()));
	
    setProperty ("title", AlertToneAppletMaps::mapToUiString (m_tone->key ()));
	setProperty ("subtitle", QVariant (m_tone->niceName ()));
}

void
AlertToneWidget::alertToneChanged ()
{
    SYS_DEBUG ("");
	setProperty ("subtitle", QVariant (m_tone->niceName ()));
}

void
AlertToneWidget::clicked ()
{
    int dcpWidgetId = 
        (SoundSettingsApplet::AlertToneBrowser_id * 65536) + m_idx;

    emit showWidget (dcpWidgetId);
}

void
AlertToneWidget::retranslateUi ()
{
	setProperty ("title", AlertToneAppletMaps::mapToUiString (m_tone->key ()));
}

