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

#ifndef __FILESYSTEMSTUB_H__
#define __FILESYSTEMSTUB_H__
#define QIODEVICE_H

#include <QObject>
#include <QtGlobal>
#include <QByteArray>
#include <QString>

class QString;

class QDirStub : public QObject 
{
Q_OBJECT

public:
    QDirStub (const QString &path);
    QDirStub (const QDirStub &orig);

    enum Filter { Dirs        = 0x001,
                  Files       = 0x002,
                  Drives      = 0x004,
                  NoSymLinks  = 0x008,
                  AllEntries  = Dirs | Files | Drives,
                  TypeMask    = 0x00f,
                  Readable    = 0x010,
                  Writable    = 0x020,
                  Executable  = 0x040,
                  PermissionMask    = 0x070,
                  Modified    = 0x080,
                  Hidden      = 0x100,
                  System      = 0x200,
                  AccessMask  = 0x3F0,
                  AllDirs       = 0x400,
                  CaseSensitive = 0x800,
                  NoDotAndDotDot = 0x1000,
                  NoFilter = -1
    };

    Q_DECLARE_FLAGS(Filters, Filter)

    QDirStub& operator=(const QDirStub &rhs);
    bool operator== (const QDirStub &orig) const;
    bool operator== (const QString &path) const;

    bool exists() const;
    static bool exists (const QString &path) {
        Q_UNUSED (path);
        return true;
    };
    bool mkpath(const QString &dirPath) const;
    void setNameFilters(const QStringList &nameFilters);
    static QChar separator () { return '/'; };
    QStringList entryList (Filters filters = NoFilter);
    QStringList entryList (
		    const QStringList &nameFilters, 
		    Filters            filters = NoFilter);
    static QString homePath () { return qgetenv ("HOME"); };
    QString absolutePath () const { return m_Path; };

private:
    QString       m_Path;
};

class QIODevice : public QObject 
{
Q_OBJECT

public:
   enum OpenModeFlag {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 0x0004,
        Truncate = 0x0008,
        Text = 0x0010,
        Unbuffered = 0x0020
    };

#if 0
   qint64 writeData(const char *data, qint64 len);
   qint64 write (const char *data, qint64 len);
#endif
    Q_DECLARE_FLAGS(OpenMode, OpenModeFlag)
};

class QFileStub : public QIODevice 
{
Q_OBJECT
    friend class QDirStub;

public:
    QFileStub (const QString &path);
    QFileStub (const QFileStub &orig);

    QFileStub &operator= (const QFileStub &rhs);
    bool operator== (const QFileStub &other) const;
    bool operator== (const QString &path) const;
    
    bool remove();
    bool open(int flags);
    qint64 	writeData ( const char * data, qint64 len );
    bool exists() const;
    bool rename(const QString &newName);

private:
    QString   m_Path;
};

#endif

