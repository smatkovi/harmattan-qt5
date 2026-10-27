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
#include "trackerconnection.h"
#include "soundsettingsutils.h"

#include <QUrl>

#ifdef HAVE_QTSPARQL
#include <QSparqlConnection>
#include <QSparqlQuery>
#include <QSparqlResult>
#include <QSparqlResultRow>
#include <QSparqlError>
#endif
#include <QTimer>

#define DEBUG
#define WARNING
#include "../debug.h"

TrackerConnection *TrackerConnection::s_Instance = 0;

static const int delayBetweenRequests = 100;
static const int delayFirstRequests = 100;

#ifdef DEBUG
static int nRequests = 0;
static int nCacheHits = 0;
#endif

/*
 * This macro decides which are those files that we try to discover with
 * tracker. Currently tracker is processing only the user's home, whatever the
 * username is.
 */
#define TRACKER_HAS_A_CHANCE(string) (string).startsWith("/home")

TrackerConnection *
TrackerConnection::instance ()
{
    if (!TrackerConnection::s_Instance)
        TrackerConnection::s_Instance = 
            new TrackerConnection;

    return TrackerConnection::s_Instance;
}

TrackerConnection::TrackerConnection ()
{
#ifdef HAVE_QTSPARQL
    m_sparqlconn = new QSparqlConnection ("QTRACKER");
#endif
}

TrackerConnection::~TrackerConnection ()
{
#ifdef HAVE_QTSPARQL
    delete m_sparqlconn;
    m_sparqlconn = 0;
#endif

    s_Instance = 0;
}

QString
TrackerConnection::niceNameFromFileName (
        const QString &fileName)
{
    QString     niceName;
    QString     myFileName = fileName;

    if (SoundSettings::isFileCopy(fileName)) {
        QString  xmlFileName;
        QString  origFileName;
        QString  copyFileName;
        QString  title;

        SYS_WARNING ("This is a file copy...");
        SoundSettings::suggestedXmlFilePath (fileName, xmlFileName);
        SoundSettings::loadXML (
                xmlFileName, origFileName, copyFileName, title);

        if (!origFileName.isEmpty() && 
                !copyFileName.isEmpty() && 
                !title.isEmpty()) {
            m_NiceNameCache[origFileName] = title;
            m_NiceNameCache[copyFileName] = title;
            m_FileCopies[copyFileName]    = origFileName;
        }
    }

    /*
     * If the requested file is a copy we might try to use the original file...
     * nope, this is not gonna work.
     */
    if (m_FileCopies.contains (myFileName) && 
            m_NiceNameCache.contains(m_FileCopies[myFileName]) &&
            !m_NiceNameCache.contains(myFileName)) {
        SYS_WARNING ("*** myFileName = %s", SYS_STR(myFileName));
        myFileName = m_FileCopies[myFileName];
    }

    /**************************************************************************
     * First we try to find the filename in the cache. If we find the nice name
     * in the cache we are returning it and will not initiate a tracker request.
     */
    if (TRACKER_HAS_A_CHANCE(myFileName)) 
        niceName = m_NiceNameCache[myFileName];

    #ifdef DEBUG
    ++nRequests;
    #endif

#ifdef LOTDEBUG
    SYS_DEBUG ("*********************************");
    SYS_DEBUG ("Cache hits/requests %d/%d (%.2g%%)", 
               nCacheHits, nRequests, 
               100.0 * (((qreal)nCacheHits / nRequests)));
    SYS_DEBUG ("*********************************");
#endif

    if (!niceName.isEmpty()) {
        #ifdef DEBUG
        nCacheHits++;
        #endif
        return niceName;
    }

    /**************************************************************************
     * Tracker is processing the user's home, it will not give information about
     * the default sound files. We are not bothering tracker about these files,
     * this will speed up the process considerably. 
     * NOTE: Please comment this code block out to test what happens when
     * tracker does not provide information about a particular file. In that
     * case we send a signal in the processRequest() method. It is not
     * necessary, but this way we can show a placeholder string while waiting
     * for the tracker answer.
     */
    if (!TRACKER_HAS_A_CHANCE(fileName)) {
        niceName = poorNiceName (fileName);
        return niceName;
    }

    /**************************************************************************
     * So we have no information in the cache, we have a chance to get the
     * information from tracker. We add a pandingrequest info about this and we
     * return a temporary title. If necessary we fire up the request processing.
     *
     * NOTE: This value is a temporary value, we will send a signal even if the
     * tracker is not answering. We are of course not adding this value to the 
     * cache.
     */
    if (m_PendingRequests.indexOf(myFileName) < 0) {
        m_PendingRequests << myFileName;
        if (m_PendingRequests.size() == 1)
            QTimer::singleShot (
                    delayFirstRequests, this, SLOT(processPendingRequests()));
    }

    niceName = poorNiceName (myFileName);

    SYS_DEBUG ("*** returning %s", SYS_STR(niceName));
    return niceName;
}

QString
TrackerConnection::trackerIdFromFileName (
        const QString &fileName)
{
    QString trackerId;

    trackerId = m_TrackerIdCache[fileName];
    if (!trackerId.isEmpty())
        return trackerId;

    /*
     * This call will initiate a tracker request and s signal about the
     * trackerId later.
     */
    niceNameFromFileName (fileName);
    return QString ("");
}

void
TrackerConnection::processPendingRequests()
{
    QString request;
    SYS_DEBUG ("We have %d requests.", m_PendingRequests.size());

    processRequest (m_PendingRequests.takeFirst());
    if (m_PendingRequests.size() > 0)
        QTimer::singleShot (
                delayBetweenRequests, this, SLOT(processPendingRequests()));
}

void
TrackerConnection::customRingToneFiles (
        QSet<QString>   &files)
{
#ifdef HAVE_QTSPARQL
    static QSparqlQuery theQuery (
        "select ?r {?c maemo:contactAudioRingtone [ nie:url ?r ]}");
    QSparqlResult       *result = 0;
    
    //theQuery.bindValue ("fileUrl", QUrl::fromLocalFile(fileName));
    result = m_sparqlconn->syncExec (theQuery);
    result->waitForFinished ();
    
    if (result->hasError()) {
        SYS_WARNING ("ERROR: %s", SYS_STR (result->lastError().message()));
        return;
    } 
    
    if (! result->first()) {
        SYS_DEBUG ("No items.");
        return;
    }

    do {
        QSparqlResultRow row = result->current();
        QVariant         col0 = row.value(0);

        SYS_DEBUG ("*** col0 = '%s'", SYS_STR(col0.toString()));
        files.insert (col0.toString());
    } while (result->next());
#endif
}

/*!
 * Low level function to process one request, sends a tracker query, checks the
 * return value and sends a signal about the result. Please note that this
 * method will emit a signal even if the tracker fails to return the requested
 * data. The returned data also added to the cache, so we call tracker at most
 * once for every file.
 */
void 
TrackerConnection::processRequest (
        const QString &fileName)
{
    QString              title;
    QString              trackerId;

#ifdef HAVE_QTSPARQL
    static QSparqlQuery theQuery (
        "select nie:title(?u) ?u where { ?u a "
        "nmm:MusicPiece . ?u nie:url ?:fileUrl }");
    theQuery.bindValue ("fileUrl", QUrl::fromLocalFile(fileName));

    QSparqlResult       *result = 0;

    SYS_DEBUG ("*** fileName = %s", SYS_STR (fileName));
    if (!TRACKER_HAS_A_CHANCE (fileName))
    {
        // Speedup
        goto fallback;
    }
    SYS_DEBUG ("*** query    = %s", SYS_STR (theQuery.preparedQueryText ()));

    result = m_sparqlconn->syncExec (theQuery);
    result->waitForFinished ();

    if (result->hasError()) {
        SYS_WARNING ("Could get title from url (%s): %s",
                     SYS_STR (fileName),
                     SYS_STR (result->lastError ().message ()));

        goto fallback;
    } else if (! result->first()) {
        SYS_DEBUG ("File not found in tracker: %s",
                     SYS_STR (fileName));

        goto fallback;
    }

    if (! result->stringValue (0).isEmpty ())
    {
        title = result->stringValue (0);
        m_NiceNameCache[fileName] = title;
    }

    SYS_DEBUG ("title/trackerid: '%s'/'%s'",
               SYS_STR (result->stringValue (0)),
               SYS_STR (result->stringValue (1)));

    if (! result->stringValue (1).isEmpty ())
    {
        trackerId = result->stringValue (0);
        m_TrackerIdCache[fileName] = trackerId;
    }
#endif

fallback:
    if (result)
    {
        delete result;
        result = 0;
    }

    /*
     * This part is handling the case when the tracker information is not
     * available. In this case we are creating a nice name from the filename. We
     * also store this name in the cache.
     */
    if (title.isEmpty()) {
        title = poorNiceName (fileName);
        m_NiceNameCache[fileName] = title;
    }
    
    SYS_DEBUG ("*** title     = %s", SYS_STR(title));
    SYS_DEBUG ("*** trackerId = %s", SYS_STR(trackerId));
    SYS_DEBUG ("Emitting dataReady()");
    emit dataReady (fileName, title, trackerId);
}

/*!
 * Our own nice name generator. We return the basename of the file without the
 * extension. 
 */
QString
TrackerConnection::poorNiceName (
        const QString &fileName)
{
    int     startIndex, endIndex;
    QString niceName;

    startIndex = fileName.lastIndexOf ('/') + 1;
    endIndex   = fileName.lastIndexOf ('.') - startIndex;

    niceName = fileName.mid (startIndex, endIndex);
    niceName.replace ("_", " ");
   
    if (niceName == "No sound")
        //% "No sound"
        niceName = qtTrId ("qtn_sond_ringtone_nosound");

    return niceName;
}

QString
TrackerConnection::trackerIdToFilename (
        const QString &trackerId)
{
    if (trackerId.isEmpty ())
        return "";

#ifdef HAVE_QTSPARQL 
    static QSparqlQuery theQuery ("select ?u where { ?:trackerId nie:url ?u }");
    theQuery.bindValue ("trackerId", QUrl(trackerId));

    QSparqlResult *result = m_sparqlconn->syncExec (theQuery);
    result->waitForFinished ();

    if (result->hasError()) {
        SYS_WARNING ("Could get filename from tracker id (%s): %s",
                     SYS_STR (trackerId),
                     SYS_STR (result->lastError ().message ()));

        delete result;
        return QString ("");
    } else if (! result->first()) {
        SYS_WARNING ("Tracker id not found: %s",
                     SYS_STR (trackerId));

        delete result;
        return QString ("");
    } else {
        QUrl url (result->value (0).toUrl ());
        delete result;

        if (! url.isValid () || !(url.scheme () == "file"))
        {
            SYS_WARNING ("Tracker returned URL is not valid: %s",
                         SYS_STR (url.toString ()));
        }
        else
            return QUrl::fromPercentEncoding(url.path().toUtf8());
    }
#endif

    return QString("");
}

void
TrackerConnection::registerFileCopy (
        const QString     &originalFilePath,
        const QString     &copyFilePath)
{
    SYS_DEBUG ("*** originalFilePath  = %s", SYS_STR(originalFilePath));
    SYS_DEBUG ("*** copyFilePath      = %s", SYS_STR(copyFilePath));
    
    m_FileCopies[copyFilePath] = originalFilePath;
}

QString
TrackerConnection::getFileCopyOriginal (
        const QString     &copyFilePath)
{
    SYS_DEBUG ("*** copyFilePath      = %s", SYS_STR(copyFilePath));
    SYS_DEBUG ("*** original          = %s", SYS_STR(m_FileCopies[copyFilePath]));
    return m_FileCopies[copyFilePath];
}
