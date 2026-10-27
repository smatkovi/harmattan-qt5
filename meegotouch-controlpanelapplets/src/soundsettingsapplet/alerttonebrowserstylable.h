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
#ifndef _ALERTTONEBROWSERSTYLABLE_H
#define _ALERTTONEBROWSERSTYLABLE_H

#include <dcpstylablewidget.h>

class AlertTone;
class AlertToneBrowser;

class AlertToneBrowserStylable : public DcpStylableWidget
{
    Q_OBJECT

    public:
        AlertToneBrowserStylable (AlertTone *tone, QGraphicsWidget *parent = 0);
        ~AlertToneBrowserStylable ();

        virtual bool pagePans () const;
        virtual QString title () const;
        void accept ();
        void cancel ();

    private:
        AlertToneBrowser    *m_browser;
};

#endif
