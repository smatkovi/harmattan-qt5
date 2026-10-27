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
#ifndef WALLPAPERUTILS_H
#define WALLPAPERUTILS_H

#include <QString>
#include <QHash>
#include <QSize>
#include <QImage>
#include <QDateTime>

#define SYS_SIZE(size) \
    SYS_STR((QString::number(size.width())+"x"+QString::number(size.height())))

#define SYS_POINTF(point) \
    SYS_STR((QString::number(point.x())+", "+QString::number(point.y())))

class QStringList;
class WallpaperITrans;

namespace Wallpaper 
{
    QString constructPath (
            const QString &dirName,
            const QString &filename = QString());

    QString baseName (
            const QString &filePath);

    QDateTime fileTimeStamp (
            const QString &filePath);

    QString setFileExtension (
            const QString &fileName,
            const QString &extension);

    QString setFileVariant (
        const QString &filePath,
        const QString &variant  = QString("-copy"));

    QString logicalIdToFilePath (
            const QString    &id);

    bool isInDir (
            const QString  &directory,
            const QString  &filePath);

    QStringList imageNameFilter ();

    /*
     * File system manipulation methods.
     */
    QHash<QString, qint64> readDir (
            const QString     &directoryPath,
            const QStringList &nameFilters);

    /*
     * @hashTable: the data will be appended here
     */
    void readDir (
            const QString           &directoryPath,
            const QStringList       &nameFilters,
            QHash<QString, qint64>  &hashTable);

    QStringList customWallpaperDirs ();

    bool imageFile (
            const QString     &filePath);

    bool ensureHasDirectory (
            const QString &directoryPath);

    bool hasDirectory (
            const QString &directoryPath);

    /*
     * Graphics
     */
    bool smallerSize (
            const QSize &a,
            const QSize &b);
#if 0
    QImage rotate (
            QImage                  &image,
            const WallpaperITrans   &trans);
#endif
};
#endif
