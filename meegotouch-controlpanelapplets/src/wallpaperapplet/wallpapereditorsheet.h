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
** Contact: Karoliina T. Salminen <karoliina.t.salminen@nokia.com>
** Authors: David Kedves <dkedves@blumsoft.eu>
**          Laszlo Pere <lpere@blumsoft.eu>
**
****************************************************************************/
#ifndef WALLPAPEREDITORSHEET_H
#define WALLPAPEREDITORSHEET_H

#include <MSheet>
class WallpaperBusinessLogic;
class WallpaperEditorWidget;

class WallpaperEditorSheet : public MSheet
{
    Q_OBJECT

    public:
        WallpaperEditorSheet (
                WallpaperBusinessLogic *wallpaperBusinessLogic);

        void createHeaderWidget();

    private:
        void createCentralWidget(
                WallpaperBusinessLogic *wallpaperBusinessLogic);

    private Q_SLOTS:
        void doneActivated ();
        void cancelActivated ();

    private:
        WallpaperEditorWidget  *m_EditorWidget;
};
#endif
