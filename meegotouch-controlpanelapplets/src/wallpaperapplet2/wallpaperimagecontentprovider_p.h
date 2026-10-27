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
#ifndef IMAGECONTENTPROVIDER_P_H
#define IMAGECONTENTPROVIDER_P_H

#ifdef HAVE_GALLERYCORE

#include "wallpaperimagecontentprovider.h"

class TrackerLiveQuery;
class GalleryModel;

class ImageContentProviderPrivate: public QObject
{
    Q_OBJECT

public:
    ImageContentProviderPrivate(GalleryModel& galleryModel);
    virtual ~ImageContentProviderPrivate();

public Q_SLOTS:
    void onInitialQueryFinished();

public:
    TrackerLiveQuery *mLiveQuery;

    bool mQueryRunning;
};

#endif // HAVE_GALLERYCORE

#endif

