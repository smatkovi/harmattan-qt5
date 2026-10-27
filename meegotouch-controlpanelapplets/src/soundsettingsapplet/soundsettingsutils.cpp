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
#include "soundsettingsutils.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QCryptographicHash>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <timed/interface>
#include <QDBusReply>

#include "trackerconnection.h"
#include "static.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

QString
SoundSettings::mountPoint ()
{
    QString  retval = QDir::homePath () + "/MyDocs/";

    return retval;
}

QString
SoundSettings::userSaveDir ()
{
    QString retval = "~/.ringtones";

    retval.replace ("~", QDir::homePath ());

    return retval;
}

bool
SoundSettings::isTemporaryFile (
        const QString &filePath)
{
    bool     retval = filePath.startsWith(mountPoint());

    SYS_DEBUG ("*** filePath   = %s", SYS_STR(filePath));
    SYS_DEBUG ("*** retval     = %s", SYS_BOOL(retval));

    return retval;
}

bool
SoundSettings::isFileCopy (
        const QString &filePath)
{
    bool     retval = filePath.startsWith(userSaveDir());

    return retval;
}

void
SoundSettings::suggestedXmlFilePath (
            const QString &filePath,
            QString       &xmlFilePath)
{
    QFileInfo          fileInfo (filePath);

    xmlFilePath = fileInfo.path() + QDir::separator() +
        fileInfo.baseName() + ".xml";
}

void
SoundSettings::suggestedTargetFilePath (
        const QString &filePath,
        QString       &baseDir,
        QString       &fileName,
        QString       &xmlFileName)
{
    QString            subDir;
    QFileInfo          fileInfo (filePath);
    QCryptographicHash hash(QCryptographicHash::Sha1);

    hash.addData (fileInfo.path().toUtf8());
    subDir = QString(hash.result().toHex());
    subDir.truncate (32);

    baseDir = userSaveDir() +
        QDir::separator() +
        subDir;

    fileName = fileInfo.baseName() + "." + fileInfo.completeSuffix();
    xmlFileName = fileInfo.baseName() + ".xml";

    SYS_DEBUG ("*** baseName()       = %s", SYS_STR(fileInfo.baseName()));
    SYS_DEBUG ("*** path()           = %s", SYS_STR(fileInfo.path()));
    SYS_DEBUG ("*** completeSuffix() = %s", SYS_STR(fileInfo.completeSuffix()));
}

bool
SoundSettings::loadXML (
        const QString   &xmlFileName,
        QString         &origFileName,
        QString         &copyFileName,
        QString         &title)
{
    QFile            *file = 0;
    QXmlStreamReader *xml  = 0;
    bool              retval = false;
    QStringList       names;

    SYS_DEBUG ("*** xmlFileName = %s", SYS_STR(xmlFileName));
    file = new QFile(xmlFileName);

    if (!file || !file->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        SYS_WARNING ("Can not open '%s' for reading: %m",
                SYS_STR(xmlFileName));
        goto finalize;
    }

    xml = new QXmlStreamReader(file);
    while(!xml->atEnd() && !xml->hasError()) {
        QString name(xml->name().toString());

        #if 0
        SYS_DEBUG ("*************************************");
        SYS_DEBUG ("*** name: %s", SYS_STR(name));
        SYS_DEBUG ("*** lineNumber : %d", xml->lineNumber());
        #endif

        if (xml->isStartElement()) {
            names << xml->name().toString();
        } else if (xml->isEndElement()) {
            names.removeLast();
        } else if (xml->isCharacters()) {
            QString path = names.join("/");

            if (path == "soundsettings-applet/orig-file")
                origFileName = xml->text().toString();
            else if (path == "soundsettings-applet/copy-file")
                copyFileName = xml->text().toString();
            else if (path == "soundsettings-applet/title")
                title = xml->text().toString();
        }

        xml->readNext();
    }

finalize:
    if (xml)
        delete xml;
    if (file)
        delete file;

    return retval;
}

void
SoundSettings::saveXML (
        const QString   &xmlFileName,
        const QString   &origFileName,
        const QString   &copyFileName,
        const QString   &title)
{
    QXmlStreamWriter *writer;
    QFile             file (xmlFileName);

    SYS_DEBUG ("-----------------------------------------------------");
    SYS_DEBUG ("*** xmlFileName = %s", SYS_STR(xmlFileName));

    if (!file.open(QIODevice::WriteOnly)) {
        SYS_WARNING ("Unable to open file for writing: %s",
                SYS_STR(xmlFileName));
        return;
    }

    /*
     *
     */
    writer = new QXmlStreamWriter();
    writer->setDevice (&file);
    writer->setAutoFormatting(true);
    writer->setCodec ("UTF-8");
    writer->writeStartDocument ();
    writer->writeStartElement ("soundsettings-applet");

    writer->writeStartElement("orig-file");
        writer->writeCharacters (origFileName);
    writer->writeEndElement ();

    writer->writeStartElement("copy-file");
        writer->writeCharacters (copyFileName);
    writer->writeEndElement ();

    writer->writeStartElement("title");
        writer->writeCharacters (title);
    writer->writeEndElement ();

    /*
     *
     */
    writer->writeEndElement();
    writer->writeEndDocument();

    delete writer;
    file.close ();
}

QString
SoundSettings::saveFile (
        const QString &filePath)
{
    QFile         sourceFile (filePath);
    QString       baseDir;
    QString       fileName;
    QString       xmlFileName;
    QString       retval = filePath;
    QDir          baseDirectory;
    QString       targetFilePath;

    suggestedTargetFilePath (filePath, baseDir, fileName, xmlFileName);

    SYS_DEBUG ("*** baseDir  = %s", SYS_STR(baseDir));
    SYS_DEBUG ("*** fileName = %s", SYS_STR(fileName));
    baseDirectory = QDir (baseDir);
    if (!baseDirectory.exists()) {
        if (!QDir::root().mkpath(baseDir)) {
            SYS_WARNING ("ERROR: mkdir(%s) failed.", SYS_STR(baseDir));
            goto finalize;
        }
    }

    targetFilePath =
        baseDir + QDir::separator() + fileName;
    xmlFileName =
        baseDir + QDir::separator() + xmlFileName;

    if (QFile(targetFilePath).exists()) {
        SYS_DEBUG ("The file '%s' already exists.", SYS_STR(targetFilePath));
        retval = targetFilePath;
        goto finalize;
    }

    if (sourceFile.copy(targetFilePath)) {
        SYS_DEBUG ("File copy to %s success.", SYS_STR(targetFilePath));
        retval = targetFilePath;
    } else {
        SYS_WARNING ("ERROR: Unable to copy %s -> %s: %m",
                SYS_STR(filePath),
                SYS_STR(targetFilePath));
    }

finalize:
    if (retval != filePath) {
        TrackerConnection *tracker = TrackerConnection::instance();
        QString title;

        tracker->registerFileCopy (filePath, retval);
        title = tracker->niceNameFromFileName (filePath);
        saveXML (xmlFileName, filePath, retval, title);
    }

    return retval;
}

QStringList
SoundSettings::customAlertToneDirs ()
{
    QDir                   directory (SoundSettings::OptDir);
    QStringList            dirNameList;
    QStringList            retval;

    if (!directory.exists(SoundSettings::OptDir))
        return retval;

    dirNameList = directory.entryList (QDir::Dirs | QDir::NoDotAndDotDot);
    SYS_WARNING ("*** dirNameList = %s", SYS_STR(dirNameList.join(";")));

    foreach (QString base, dirNameList) {
        QString path = 
            SoundSettings::OptDir +
            QDir::separator () + 
            base +
            DEFAULT_RINGTONE_PATH1;
        QDir    dir (path);

        if (dir.exists()) {
            SYS_DEBUG ("exists     : %s", SYS_STR(path));
            retval << path;
        } else {
            SYS_DEBUG ("not exists : %s", SYS_STR(path));
        }
    }

    return retval;
}


/*!
 * \param usedFiles The sound files that are used so shouldn't be removed.
 *
 * Collects the sound files that are saved into the directory but not used any
 * more.
 */
static void
collectFilesRecursive (
        QSet<QString>   &usedFiles,
        QSet<QString>   &toRemove,
        const QString    dirName)
{
    QDir           dir(dirName);
    QFileInfoList  list = dir.entryInfoList();

    for (int iList = 0; iList < list.count(); iList++) {
        QFileInfo  info = list[iList];
        QString    sFilePath = info.filePath();

        if (info.fileName().startsWith(".") ||
                info.fileName().endsWith(".xml"))
            continue;

        if (info.isDir()) {
            if (info.symLinkTarget().isEmpty())
                collectFilesRecursive (usedFiles, toRemove, sFilePath);
        } else {
            if (!usedFiles.contains(sFilePath)) {
                toRemove += sFilePath;
            }
        }
    }
}

/*!
 * Removes the saved but not used sound siles, their XML descriptor files and
 * the empty directories if any.
 */
void
SoundSettings::removeUnusedFiles (
        QSet<QString>   &usedFiles)
{
    QSet<QString>    toRemove;
    QSet<QString>    directories;

    /*
     * Collecting unused files.
     */
    collectFilesRecursive (usedFiles, toRemove, userSaveDir());

    /*
     * Removing all the unused files.
     */
    foreach (QString fileName, toRemove) {
        QFileInfo fileInfo (fileName);
        QString   xmlFileName (fileInfo.path() + QDir::separator() +
                fileInfo.baseName() + ".xml");
        QFile     binFile (fileName);
        QFile     xmlFile (xmlFileName);

        if (binFile.remove()) {
            if (!xmlFile.remove()) {
                SYS_WARNING ("Unable to remove %s: %m", SYS_STR(xmlFileName));
            }
        } else {
            SYS_WARNING ("Unable to remove %s: %m", SYS_STR(fileName));
        }

        if (!directories.contains(fileInfo.path()))
            directories += fileInfo.path();
    }

    /*
     * Once we removed the files some directory might become empty, so we can
     * remove them too.
     */
    foreach (QString dirName, directories) {
        QDir::root().rmdir(dirName);
    }
}

/******************************************************************************
 * Timed related methods.
 */
typedef long qmAlarmCookie;  // Cookie ID type definition

static QList<qmAlarmCookie> 
queryAlarmCookies (
        bool &error)
{
    Maemo::Timed::Interface  interface_;    
    QList<qmAlarmCookie>     cookieList;
    QMap<QString, QVariant>  attribute;

    attribute.insert("APPLICATION", "clock");

    QDBusReply< QList<QVariant> > response = interface_.query_async(attribute);

    if(response.isValid()) {
    	QList<QVariant> responseValue = response.value();

        SYS_DEBUG ("We have %d alarms.", responseValue.size());
	    for (int n = 0; n < responseValue.size(); ++n)
    	    cookieList.append(responseValue[n].toUInt());

        error = false;
    } else {
    	SYS_WARNING ("Failed to query: %s",
                SYS_STR(response.error().message()));
    	error = true;
    }

    return cookieList;
}

void
SoundSettings::customAlarmSoundFiles (
        QSet<QString>   &files)
{
    bool                    error;
    QList<qmAlarmCookie>    cookies = queryAlarmCookies (error);
    Maemo::Timed::Interface timed;
    QString                 sound;

    SYS_DEBUG ("error = %s", SYS_BOOL(error));
    if (error)
        return;

    SYS_DEBUG ("We have %d alarms.", cookies.size());
    for (int n = 0; n < cookies.size(); ++n) {
        QDBusReply< QMap < QString,QVariant > > reply = 
            timed.query_attributes_async(cookies[n]);
        
        if (reply.isValid()) {
            QMap<QString, QVariant> attributes = reply.value();

            sound = attributes["sound"].toString ();
            SYS_DEBUG ("*** sound = %s", SYS_STR(sound));
            files.insert (sound);
        } else {
            SYS_WARNING ("Invalid reply.");
        }
    }
}

