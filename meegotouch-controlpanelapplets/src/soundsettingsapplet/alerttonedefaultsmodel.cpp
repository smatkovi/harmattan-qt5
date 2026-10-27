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

#include <QTimer>
#include <QFile>
#include <QFileInfo>

#include "soundsettingsutils.h"
#include "trackerconnection.h"
#include "alerttonedefaultsmodel.h"
#include "alerttone.h"
#include "static.h"

/*
 * Define this macro to add items asynchronously.
 */
#undef USE_ASYNC_LOADING

//#define DEBUG
#define WARNING
#include "../debug.h"

static const int fileSystemReCheckDelay = 2500;

/******************************************************************************
 * AlertToneDefaultsModel implementation.
 */
AlertToneDefaultsModel::AlertToneDefaultsModel() : QStandardItemModel(),
    m_isFinished(false)
{
    QString     oviDirPath = oviRingTonesPath ();
    QStringList customDirs = SoundSettings::customAlertToneDirs ();

    insertColumn (NiceNameColumn);
    insertColumn (FullPathColumn);
    insertColumn (ForcedColumn);

    // Creating the directory so that we can connect and watch them.
    ensureHasDirectory (oviDirPath);

    m_dirStack.push(QDir(DEFAULT_RINGTONE_PATH1));
    m_dirIdx.push(0);
    m_dirStack.push(QDir(DEFAULT_RINGTONE_PATH2));
    m_dirIdx.push(0);
    m_dirStack.push(QDir(oviDirPath));
    m_dirIdx.push(0);
    
    foreach (QString customDir, customDirs) {
        m_dirStack.push(QDir(customDir));
        m_dirIdx.push(0);
    }

//for meego.com, we have to add sounds from
//  "/usr/share/sounds/meego/stereo/"

#ifdef MEEGO
   QString meegoPath = "/usr/share/sounds/meego/stereo/";

   SYS_DEBUG ("*** MEEGO sounds path = %s", SYS_STR(meegoPath));

   m_dirStack.push(QDir(meegoPath));
   m_dirIdx.push(0);
#endif

    /*
     * Creatig a file system watcher.
     */
    m_FileWatcher = new QFileSystemWatcher (this);
    m_FileWatcher->addPath (oviDirPath);
    m_FileWatcher->addPath (SoundSettings::OptDir);

    connect (m_FileWatcher, SIGNAL(fileChanged(const QString &)),
            this, SLOT(fileChanged(const QString &)));
    connect (m_FileWatcher, SIGNAL(directoryChanged(const QString &)),
            this, SLOT(directoryChanged(const QString &)));

    /*
     * A timer that handles installation of new alert-tone package
     * installations.
     */
    m_FileSystemTimer.setInterval (fileSystemReCheckDelay);
    m_FileSystemTimer.setSingleShot (true);
    connect (&m_FileSystemTimer, SIGNAL(timeout()),
            this, SLOT(loadFromDirectory()));

    /*
     * TrackerConnection might send a signal about the tracker answer.
     */
    connect (TrackerConnection::instance(),
            SIGNAL(dataReady(QString,QString,QString)),
            this,
            SLOT(dataReceived(QString,QString,QString)));

    #ifdef USE_ASYNC_LOADING
    /*
     * We need to give the UI a chance to process the tap events, but we also
     * want to be as fast as possible... this is not a perfect solution though.
     */
    QTimer::singleShot(0, this, SLOT(addSingleItem()));
    #else
    while (!m_isFinished)
        addSingleItem();
    #endif
}

AlertToneDefaultsModel::~AlertToneDefaultsModel ()
{
    delete m_FileWatcher.data ();
}

void
AlertToneDefaultsModel::addSingleItem()
{
    if (m_isFinished)
        return;

    while (m_dirStack.count() > 0) {
        if (m_dirIdx.top() >= m_dirStack.top().count()) {
            m_dirIdx.pop();
            m_dirStack.pop();
        } else {
            break;
        }
    }

    if (m_dirStack.count() <= 0) {
        m_isFinished = true;
        emit finished();
        return;
    }

    #if 0
    SYS_WARNING ("*** path        = %s",
            SYS_STR(m_dirStack.top().absolutePath()));
    SYS_WARNING ("***             = %s",
            SYS_STR(m_dirStack.top()[m_dirIdx.top()]));
    #endif
    int      currentLevel = m_dirIdx.count() - 1;
    QString  baseName     = m_dirStack.top()[m_dirIdx.top()];

    if (baseName != "." && baseName != "..")
    {
        QString fullPath = m_dirStack.top().absolutePath() + 
                QDir::separator() + baseName;
        QFileInfo fileInfo (fullPath);

        SYS_WARNING ("directory: %s path: %s", 
                SYS_BOOL(fileInfo.isDir()),
                SYS_STR(fullPath));

        if (fileInfo.isDir()) {
            SYS_WARNING ("directory: %s", SYS_STR(fullPath));
            QDir subdir(fullPath);
            m_dirStack.push(subdir);
            m_dirIdx.push(0);
        } else {
            QString extension = baseName.right(4).toLower();

            if (extension == ".aac" ||
                extension == ".mp3" ||
                extension == ".wma" ||
                extension == ".wav" ||
                extension == ".mp2" ||
                extension == ".amr" ||
                extension == ".mid") {
                QString niceName = TrackerConnection::instance()->
                    niceNameFromFileName (fullPath);
                addSingleItem (niceName, fullPath);
            }
        }
    }

    m_dirIdx[currentLevel]++;

    #ifdef USE_ASYNC_LOADING
    /*
     * We have to keep this delay at 0, otherwise the loading of the list is
     * going to be sooooo slow!
     */
    QTimer::singleShot(0, this, SLOT(addSingleItem()));
    #endif
}

/*!
 * \param niceName The human readable name for the new item
 * \param fileName The full path file name for the new entry
 * \param forced Should be true when the item is not found in the filesystem but
 *   inserted for some other reasons (maybe coming from the content picker).
 *
 * This is the method that should be used to add a new item into the model once
 * the file name and the human readable name is known. No other function should
 * be created to add new row to the model.
 */
int
AlertToneDefaultsModel::addSingleItem (
            const QString &niceName, 
            const QString &fileName,
            bool           forced)
{
    int      row = rowCount();
    int      idx = findItemByFileName (fileName);
    QString  myNiceName;

    if (idx >= 0)
        return row;

    /*
     *
     */
    emit loading();

    /*
     * Adding the file to the watcher: we should not get duplications added to
     * the model, so we don't need to check if the file is already added to the
     * watcher.
     */
    m_FileWatcher->addPath (fileName);    


    /**************************************************************************
     * Adding the new row.
     */
    insertRow(row);

    /*
     * Last minute checking, just to be sure.
     */
    myNiceName = niceName;
    if (myNiceName.isEmpty()) {
        myNiceName = 
            TrackerConnection::instance()->niceNameFromFileName (fileName);
    }

    SYS_DEBUG ("   %-20s %-32s %s",
            myNiceName.toUtf8().constData(),
            fileName.toUtf8().constData(),
            forced ? "forced" : "");

    m_FileNameCache[fileName] = row;
    setData (index(row, NiceNameColumn), QVariant(myNiceName));
    setData (index(row, FullPathColumn), QVariant(fileName));
    setData (index(row, ForcedColumn), QVariant(forced));

    return row;
}

/*!
 * This slot is called when the tracker connection sends a signal about a
 * tracker answer to send the nince name (title9 of a particular item.
 */
void 
AlertToneDefaultsModel::dataReceived (
            const QString   &filename, 
            const QString   &title,
            const QString   &trackerId)
{
    Q_UNUSED (trackerId);
    int Idx = findItemByFileName (filename);

    SYS_DEBUG ("*** filename = %s", SYS_STR(filename));
    SYS_DEBUG ("*** Idx      = %d", Idx);
    if (Idx >= 0)
        setData (index(Idx, NiceNameColumn), QVariant(title));
}

/*!
 * \returns The item index for the given file name or -1 if not found.
 */
int 
AlertToneDefaultsModel::findItemByFileName (
        const QString &FileName) const
{
    int retval = -1;

    if (m_FileNameCache.contains(FileName)) {
        retval = m_FileNameCache[FileName];
        goto finalize;
    } else {
        QString originalFile = 
            TrackerConnection::instance()->getFileCopyOriginal (FileName);

        if (!originalFile.isEmpty() && 
                m_FileNameCache.contains(originalFile))
            retval = m_FileNameCache[originalFile];
    }

finalize:
    SYS_WARNING ("Returning %d for %s", retval, SYS_STR(FileName));
    return retval;
}

/*!
 * \returns The item index for the given nice name or -1 if not found.
 */
int 
AlertToneDefaultsModel::findItemByNiceName (
        const QString &NiceName) const
{
    for (int n = 0; n < rowCount(); ++n) {
        if (niceName(n) == NiceName)
            return n;
    }

    return -1;
}

/*
 * We call this function when the whole model needs to be refreshed on the
 * screen, for example when the highlight is changed because the user typed a
 * search string.
 */
void
AlertToneDefaultsModel::refresh ()
{
    int rows = rowCount();

    if (rows != 0) {
        QModelIndex first = index (0, 0);
        QModelIndex last  = index (rows - 1, 0);

        emit dataChanged (first, last);
    }
}

/*!
 * Helper function to return the nice name (human readable name) for a given
 * item.
 */
QString
AlertToneDefaultsModel::niceName (
        const QModelIndex &item) const
{
    QModelIndex myItem = index (item.row(), NiceNameColumn);
    return data(myItem).toString ();
}



/*!
 * Helper function to return the filename (full path name) for a given item.
 */
QString
AlertToneDefaultsModel::fileName (
        const QModelIndex &item) const
{
    QModelIndex myItem = index (item.row(), FullPathColumn);
    return data(myItem).toString ();
}

/*!
 * Helper function to return the filename (full path name) for a given item.
 */
QString
AlertToneDefaultsModel::fileName (
        int indexRow) const
{
    QModelIndex myItem = index (indexRow, FullPathColumn);
    return data(myItem).toString ();
}

QString
AlertToneDefaultsModel::originalFileName (
        int indexRow) const
{
    QModelIndex myItem   = index (indexRow, FullPathColumn);
    QString     filePath = data(myItem).toString ();

    return TrackerConnection::instance()->getFileCopyOriginal (filePath);   
}

/*!
 * Helper function to return the nice name (human readable name) for a given
 * item.
 */
QString
AlertToneDefaultsModel::niceName (
        int indexRow) const
{
    QModelIndex myItem = index (indexRow, NiceNameColumn);
    return data(myItem).toString ();
}

bool 
AlertToneDefaultsModel::isFinished () const
{
    return m_isFinished;
}

/*!
 * This slot is called when the filesystem watcher reports a file to be changed.
 * In this case we are going to remove the referencing row from the model, so
 * the file will disappear from the list widget.
 * Fortunately this file system change is also monitored by the QProfileValue
 * object, and that will handle the selection that will be changing to the
 * default item when the currently set sound file is removed.
 */
void
AlertToneDefaultsModel::fileChanged (
        const QString &filename)
{
    QFile thisFile (filename);
    bool  exists = thisFile.exists (filename);
    int   idx;

    SYS_DEBUG ("*** filename = %s", SYS_STR(filename));

    /*
     * If the file still exists we don't need to do nothing.
     */
    if (!exists) {
        idx = findItemByFileName (filename);
        /*
         * Removing the referencing row from the model.
         */
        if (idx >= 0) {
            SYS_DEBUG ("removing %d for %s", idx, SYS_STR(filename));
            removeRows (idx, 1);
            m_FileNameCache.remove (filename);
        } else {
            SYS_WARNING ("NOT FOUND");
        }
        /*
         * We are not watching the file any more.
         */
        m_FileWatcher->removePath (filename);
    }
}
 
void 
AlertToneDefaultsModel::loadFromDirectory ()
{
    QStringList customDirs = SoundSettings::customAlertToneDirs ();

    SYS_DEBUG ("*** customDirs = %s", SYS_STR(customDirs.join(";")));
    foreach (QString customDir, customDirs) {
        m_dirStack.push(QDir(customDir));
        m_dirIdx.push(0);
    }
    
    m_isFinished = false;
    while (!m_isFinished)
        addSingleItem();
}

void
AlertToneDefaultsModel::directoryChanged (
        const QString &directory)
{
    SYS_DEBUG ("*** directory = %s", SYS_STR(directory));
    if (directory == SoundSettings::OptDir) {
        m_FileSystemTimer.start ();
        return;
    }
    
    m_dirStack.push(QDir(directory));
    m_dirIdx.push(0);
    m_isFinished = false;

    while (!m_isFinished)
        addSingleItem();
}

QString 
AlertToneDefaultsModel::oviRingTonesPath ()
{
    QString retval (OVI_RINGTONE_PATH);
    QString homeDir (getenv("HOME"));

    retval.replace ('~', homeDir);
    return retval;
}

bool
AlertToneDefaultsModel::ensureHasDirectory (
        const QString &directoryPath)
{
    QDir dir (directoryPath);
    bool retval;

    if (dir.exists()) {
        SYS_DEBUG ("Directory %s already exists.", SYS_STR(directoryPath));
        retval = true;
    } else if (!dir.mkpath(directoryPath)) {
        SYS_WARNING ("Unable to create %s directory.", SYS_STR(directoryPath));
    } else {
        retval = true;
    }

    return retval;
}

void
AlertToneDefaultsModel::moveItem(int from, int destination)
{
    SYS_DEBUG("move item from %d to %d", from, destination);
    QString  destFileName = 
        data (index(destination, FullPathColumn)).toString();
    QVariant niceName = data (index(from, NiceNameColumn));
    QVariant fileName = data (index(from, FullPathColumn));
    QVariant forced   = data (index(from, ForcedColumn));

    removeRow(from);
    insertRow(destination);

    setData (index(destination, NiceNameColumn), QVariant(niceName));
    setData (index(destination, FullPathColumn), QVariant(fileName));
    setData (index(destination, ForcedColumn),   QVariant(forced));

    m_FileNameCache.clear();
    for (int n = 0; n < rowCount(); ++n) {
        QString fn = data (index(n, FullPathColumn)).toString(); 
        m_FileNameCache[fn] = n;
    }
}
