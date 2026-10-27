/***************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of mhome.
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

#ifndef HOMEFILEDATASTORE_P_H
#define HOMEFILEDATASTORE_P_H

#include <QSettings>
#include <QScopedPointer>
#include <QFileSystemWatcher>
#include <QMap>
#include <QTimer>

class HomeFileDataStorePrivate
{
public:
    HomeFileDataStorePrivate(const QString &filePath);

    //! The name of the settings file
    QString settingsFileName;

    //! Snapshot of the settings
    QMap<QString, QVariant> settingsSnapshot;

    //! Timer for writing the settings to the file after a delay
    QTimer syncTimer;

    //! Whether the settings contain changes that have not been written to the file
    bool outOfSync;

    //! Whether writing the file is enabled or not
    bool syncEnabled;
};

#endif
