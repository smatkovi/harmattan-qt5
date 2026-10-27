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
#ifndef WALLPAPERINFOHEADER_H
#define WALLPAPERINFOHEADER_H

#include <QObject>
#include <MStylableWidget>
#include <meegocontrolexport.h>

class MLabel;

class MC_EXPORT WallpaperInfoHeader : public MStylableWidget
{
    Q_OBJECT

public:
    WallpaperInfoHeader (QGraphicsWidget *parent = 0);
    ~WallpaperInfoHeader ();

    virtual void retranslateUi ();

private:
    MLabel           *m_Label;
};

#endif
