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
#ifndef WALLPAPERWIDGET_H
#define WALLPAPERWIDGET_H

#include <QPointer>

#include "dcpwidget.h"
#include "wallpaperbusinesslogic.h"

class MLinearLayoutPolicy;
class MLabel;
class MList;
class WallpaperList;
class MContentItem;
class MLinearLayoutPolicy;

class WallpaperWidget : public DcpWidget
{
    Q_OBJECT

public:
    WallpaperWidget (
            WallpaperBusinessLogic *wallpaperBusinessLogic, 
            QGraphicsWidget        *parent = 0);
    ~WallpaperWidget ();

protected:
    virtual void polishEvent ();

private slots:
    void slotImageActivated (WallpaperDescriptor *desc);
    void slotImageActivated ();
    virtual void createContent ();
    void oviActivated ();
    
private:
    void addHeaderContainer (MLinearLayoutPolicy *mainLayout);
    void retranslateUi ();

private:
    QPointer<WallpaperBusinessLogic>       m_WallpaperBusinessLogic;
    MLabel                                *m_TitleLabel;
    WallpaperList                         *m_ImageList;

    MContentItem                          *m_OviItem;
    qreal                                  m_NavigationBarTransparency;
    #ifdef UNIT_TEST
    friend class Ut_WallpaperWidget;
    #endif
};

#endif
