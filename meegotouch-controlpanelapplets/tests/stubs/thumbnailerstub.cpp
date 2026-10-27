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

#include "thumbnailerstub.h"
using namespace Thumbnails;

#include <QString>
#include <QStringList>

#define DEBUG
#define WARNING
#include "../../src/debug.h"


Thumbnailer::Thumbnailer ()
{
    SYS_DEBUG ("Constructor: %p", this);
    reset ();
}

Thumbnailer::~Thumbnailer ()
{
    SYS_DEBUG ("Destructor : %p", this);
}

void
Thumbnailer::reset ()
{
    m_RequestCame = false;
}

/*
 * Emits the 'finished' signal with the proper data. Currently we always thumbnail
 * images one by one, so there should be no thumbnail generating left.
 */
void
Thumbnailer::sendFinished ()
{
    emit finished (0);
}

/*
 * Emits the thumbnail() signal with the proper data.
 */
void
Thumbnailer::sendThumbnail ()
{
    // Always using 100x100 images as thumbnails.
    QPixmap pixmap (100, 100);
    QUrl    tumbnailUrl ("file:///NoSuchAThumbnailFile.png");

    emit thumbnail (m_Url, tumbnailUrl, pixmap, m_Flavor);
}

void
Thumbnailer::sendError ()
{
    QString message ("Emulated error message Thumbnailer stub.");

    SYS_DEBUG ("Sending error: %s", SYS_STR(message));
    emit error (message, m_Url);
}

bool
Thumbnailer::request (
            QList<QUrl> &uris, 
            QStringList &mimeTypes, 
            bool         sendPixmap, 
            QString      flavor, 
            QString      scheduler, 
            bool         autoCancel, 
            bool         sendRemainingSignals)
{
    SYS_DEBUG ("Request came.");

    Q_UNUSED (sendPixmap);
    Q_UNUSED (scheduler);
    Q_UNUSED (autoCancel);
    Q_UNUSED (sendRemainingSignals);

    for (int n = 0; n < uris.size(); ++n) {
        SYS_DEBUG ("*** url [%d] = %s", n, SYS_STR(uris[n].toString()));
        SYS_DEBUG ("*** mime[%d] = %s", n, SYS_STR(mimeTypes[n]));
    }

    /*
     * We are a test program, so we do some checking here. But because we are
     * outside of a normal test method we just drop a segfault if something is
     * seriously wrong.
     */
    Q_ASSERT (uris.size() > 0);
    Q_ASSERT (mimeTypes.size() > 0);
    Q_ASSERT (uris.size() == mimeTypes.size());
    
    for (int n = 0; n < uris.size(); ++n) {
        Q_ASSERT (!uris[n].toString().isEmpty());
        if (mimeTypes[n].isEmpty()) {
            SYS_WARNING ("MimeType for %s is empty.",
                    SYS_STR(uris[n].toString()));
        }
        Q_ASSERT (!mimeTypes[n].isEmpty());
        Q_ASSERT (mimeTypes[n].startsWith("image/"));
    }

    /*
     * Storing the request and remembering the fact that we got a request. We
     * use these data when sending signals.
     */
    m_Url         = uris[0];
    m_Flavor      = flavor;
    m_RequestCame = true;
    return true;
}

void
Thumbnailer::cancel  (
        bool sendRemainingSignals, 
        bool unqueueFromTumbler)
{
    Q_UNUSED (sendRemainingSignals);
    Q_UNUSED (unqueueFromTumbler);
}
