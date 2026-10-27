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

#include "wallpaperimagecontentprovider.h"

#ifdef HAVE_GALLERYCORE

#include <QtSparql>
#include <TrackerLiveQuery>

#include "wallpaperimagecontentprovider_p.h"
#include <gallerymodel.h>

static const QString liveQuery = "SELECT "
      "nie:url(?urn) AS ?url "
      "nfo:fileName(?urn) AS ?filename "
      "nie:contentCreated(?urn) AS ?created "
      "nie:mimeType(?urn) AS ?mimetype "
      "( EXISTS { "
      "    ?urn nao:hasTag nao:predefined-tag-favorite . "
      "} ) AS ?favorite "
      "nfo:duration(?urn) AS ?duration "
      "?urn tracker:id(?urn) AS ?trackerid "
      "WHERE { "
      "?urn rdf:type nmm:Photo . "
      "?urn tracker:available \"true\"^^xsd:boolean . "
      "} "
      "ORDER BY "
      "DESC (?created) "
      "DESC (?filename)";

#define DEBUG
#define WARNING
#include "../debug.h"

ImageContentProvider::ImageContentProvider (
        GalleryModel   &galleryModel, 
        QObject        *parent) :
    GalleryContentProvider (parent),
    d_ptr (new ImageContentProviderPrivate(galleryModel))
{
    SYS_WARNING ("");
}

ImageContentProvider::~ImageContentProvider()
{
    SYS_WARNING ("");
    delete d_ptr;
    d_ptr = 0;
}

QAbstractItemModel *
ImageContentProvider::model() const
{
    Q_D (const ImageContentProvider);
    return d->mLiveQuery ? d->mLiveQuery->model() : 0;
}

void 
ImageContentProvider::queryContent (
        int limit)
{
    Q_UNUSED(limit);
    Q_D(ImageContentProvider);

    SYS_DEBUG ("Starting query...");

    d->mQueryRunning = true;

    TrackerPartialUpdater updater(liveQuery);
    updater.watchClass ("nmm:Photo", QStringList(), 
            "tracker:id(?urn) in %LIST", TrackerPartialUpdater::Subject, 7);
    d->mLiveQuery->addUpdater(updater);
    d->mLiveQuery->setIdentityColumns(QList<int>() << 7);

    // Collate by creation time first, file name then.
    d->mLiveQuery->setCollationColumns(
            QList<TrackerLiveQuery::CollationColumn>() << 
                TrackerLiveQuery::CollationColumn (
                    GalleryContentProvider::CreatedColumn,
                    QVariant::DateTime,
                    Qt::DescendingOrder) << 
                TrackerLiveQuery::CollationColumn (
                    GalleryContentProvider::FileNameColumn,
                    QVariant::String,
                    Qt::DescendingOrder));

    connect(d->mLiveQuery,
            SIGNAL(initialQueryFinished()),
            d, SLOT(onInitialQueryFinished()),
            Qt::UniqueConnection);

    d->mLiveQuery->start();
}

bool 
ImageContentProvider::isLoadingContent() const
{
    Q_D(const ImageContentProvider);
    return d->mQueryRunning;
}

/******************************************************************************
 *
 */
ImageContentProviderPrivate::ImageContentProviderPrivate (
        GalleryModel  &galleryModel):
    QObject(),
    mLiveQuery(new TrackerLiveQuery(liveQuery, 8, *galleryModel.sparqlConnection())),
    mQueryRunning(false)
{
    mLiveQuery->setUpdatesEnabled (true);
}

ImageContentProviderPrivate::~ImageContentProviderPrivate()
{    
    mLiveQuery->setUpdatesEnabled(false);
    delete mLiveQuery;
}

void ImageContentProviderPrivate::onInitialQueryFinished()
{
    SYS_DEBUG ("Query finished.");
    mQueryRunning = false;
}

#endif // HAVE_GALLERYCORE
