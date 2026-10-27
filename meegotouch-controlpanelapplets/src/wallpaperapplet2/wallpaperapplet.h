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
#ifndef WALLPAPERAPPLET_H
#define WALLPAPERAPPLET_H

#include "dcpappletif.h"
#include "wallpaperbusinesslogic.h"

#include <QObject>
#include <QPointer>
#include <DcpAppletIf>

class MSheet;
class WallpaperWidget;
class WallpaperEditorWidget;
class WallpaperViewWidget;

class Q_DECL_EXPORT WallpaperApplet : public QObject, public DcpAppletIf
{
    Q_OBJECT
    Q_INTERFACES(DcpAppletIf)

public:
    WallpaperApplet ();
    ~WallpaperApplet ();

    typedef enum {
        MainWidget    = 0,
        EditorWidget  = 1,
    } WindowID;

    virtual void init();
    virtual DcpStylableWidget *constructStylableWidget (int widgetId);
    virtual MSheet *constructSheet (int widgetId);
    virtual DcpStylableWidget *pageMain (int widgetId);
    virtual QString title() const;
    virtual QVector<MAction *> viewMenuItems();
    virtual DcpBrief* constructBrief(int partId);

    /*
     * Now everyone tells we should not show the help menu item... I can
     * certainly remove it...
     */
//private slots:
//    void userGuide ();

private:
    QPointer<WallpaperWidget>        m_MainWidget;
    QPointer<WallpaperViewWidget>    m_EditorWidget;
    QPointer<WallpaperBusinessLogic> m_WallpaperBusinessLogic;
    #ifdef UNIT_TEST
    friend class Ut_WallpaperApplet;
    #endif
};
#endif
