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

#include "filesystemstub.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QFileInfo>

#define DEBUG
#include "../../src/debug.h"

static QList<QDirStub> fileSystemDirs;
static QList<QFileStub> fileSystemFiles;
#if 0
qint64 
QIODevice::writeData(
        const char *data, qint64 len)
{
    SYS_WARNING (">>>>>>>>>>>>>>>");
    return len;
}

qint64 
QIODevice::write(
        const char *data, qint64 len)
{
    SYS_WARNING (">>>>>>>>>>>>>>>");
    return len;
}
#endif
/******************************************************************************
 * QDirStub implementation.
 */
QDirStub::QDirStub (
	const QString &path)
{
    SYS_DEBUG ("path = %s", SYS_STR(path));
    m_Path = path;
}

QDirStub::QDirStub (
	const QDirStub &orig) : 
    QObject ()
{
    m_Path = orig.m_Path;
    SYS_DEBUG ("path = %s", SYS_STR(m_Path));
}

QDirStub &
QDirStub::operator= (
        const QDirStub &rhs)
{
    if (this == &rhs)
        return *this;

    m_Path = rhs.m_Path;

    return *this;
}

bool 
QDirStub::operator== (
        const QDirStub &other) const
{
    return m_Path == other.m_Path;
}

bool 
QDirStub::operator== (
        const QString &path) const
{
    return m_Path == path;
}

bool
QDirStub::exists() const
{
    bool exists = fileSystemDirs.indexOf (*this) >= 0;

    SYS_DEBUG ("*** path   = %s", SYS_STR(m_Path));
    SYS_DEBUG ("*** exists = %s", SYS_BOOL(exists));
    return exists;
}

bool 
QDirStub::mkpath (
        const QString &dirPath) const
{
    QDirStub  dir (dirPath);

    if (!dir.exists())
        fileSystemDirs.append (dir);

    return true;
}

void 
QDirStub::setNameFilters (
        const QStringList &nameFilters)
{
    Q_UNUSED (nameFilters);
    //m_NameFilters = nameFilters;
}


QStringList
QDirStub::entryList (
		const QStringList &nameFilters, 
		Filters            filters)
{
    QStringList retval;

    Q_UNUSED (nameFilters);
    Q_UNUSED (filters);
    /*
     * FIXME: Not implemented.
     */
    return retval;
}

QStringList
QDirStub::entryList (
        Filters filters)
{
    QStringList retval;

    SYS_DEBUG ("-> %d", fileSystemFiles.size());
    /*
     * FIXME: Ignoring m_NameFilters here!
     */
    if (filters & Files) {
        for (int n = 0; n < fileSystemFiles.size(); ++n) {
            QFileInfo fileInfo (fileSystemFiles[n].m_Path);
            QString   dirname = fileInfo.absolutePath();
            QString   basename = fileInfo.fileName();

            SYS_DEBUG ("*** m_Path   = %s", SYS_STR(m_Path));
            SYS_DEBUG ("*** dirname  = %s", SYS_STR(dirname));
            SYS_DEBUG ("*** basename = %s", SYS_STR(basename));
            if ((dirname + QString("/")) == m_Path) {
                SYS_DEBUG ("Adding: %s", SYS_STR(basename));
                retval << basename;
            }
        }
    }
    return retval;
}

/******************************************************************************
 *
 */
QFileStub::QFileStub (
        const QString &path)
{
    m_Path = path;
}

QFileStub::QFileStub (
	const QFileStub &orig)
{
    m_Path = orig.m_Path;
}

QFileStub &
QFileStub::operator= (
        const QFileStub &rhs)
{
    if (this == &rhs)
        return *this;

    m_Path = rhs.m_Path;

    return *this;
}

bool 
QFileStub::operator== (
        const QFileStub &other) const
{
    return m_Path == other.m_Path;
}

bool 
QFileStub::operator== (
        const QString &path) const
{
    return m_Path == path;
}

bool
QFileStub::remove()
{
    int index;
    
    SYS_DEBUG ("Removing %s", SYS_STR(m_Path));
    index = fileSystemFiles.indexOf (*this);
    if (index < 0)
        return false;
   
    fileSystemFiles.removeAt (index);
    return true;
}
   
bool
QFileStub::open (
        int flags)
{
    SYS_DEBUG ("*** opening %s", SYS_STR(m_Path));
    if (flags & WriteOnly &&
            !exists()) {
        fileSystemFiles.append (*this);
    }

    return true;
}


bool
QFileStub::exists() const
{
    bool exists = fileSystemFiles.indexOf (*this) >= 0;;

    SYS_DEBUG ("*** path   = %s", SYS_STR(m_Path));
    SYS_DEBUG ("*** exists = %s", SYS_BOOL(exists));
    return exists;
}
    
bool
QFileStub::rename(
        const QString &newName)
{
    int index;
    
    index = fileSystemFiles.indexOf (*this);
    if (index < 0)
        return false;
    
    fileSystemFiles[index].m_Path = newName;
    m_Path = newName;

    return true;
}

