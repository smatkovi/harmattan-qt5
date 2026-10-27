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
#ifndef LOGOWIDGET_H
#define LOGOWIDGET_H

#include <MImageWidget>
#include <MBanner>

class LogoWidget : public MImageWidget 
{
    Q_OBJECT
    public:
        LogoWidget (QGraphicsWidget *parent = 0);

    signals:
        void eggs (bool on);
        
    protected:
        bool event (QEvent* event);
        void turnedOn ();
        void showInfoBanner (const QString &text);
        
    private:
        time_t     m_LastTime;
        int        m_StateCounter;
        MBanner   *m_InfoBanner;
};

#endif
