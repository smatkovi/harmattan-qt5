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
#include "homefiledatastore.h"
#include "homefiledatastore_p.h"
#include <QApplication>
#include <QTemporaryFile>
#include <QFileInfo>

/*!
 * Creates a temporary file in the directory of an original file.
 * \param originalPath Absolute path that is used as a base for generating
 * the temporary file.
 * \return Path of the created file, or an empty string if creating the
 * file fails. The returned value is a copy, so for uses where it's assigned
 * to a variable, it need not be const. For cases where it's passed as a
 * parameter, it need not be const because references to non-const will
 * not bind to function return values, because they are rvalues. When
 * C++0x brings rvalue references, the value should not be const in order
 * to allow rvalue references to bind to it and thus enable moving from it.
 */
static QString createTempFile(const QString &originalPath)
{
    QString returnValue;
    QTemporaryFile tempFile(originalPath);
    if (tempFile.open()) {
        tempFile.setAutoRemove(false);
        returnValue = tempFile.fileName();
    }
    return returnValue;
}

/*!
 * Renames a file. Ensures that a file with the new name
 * doesn't exist.
 * \param oldname Name of the file to rename.
 * \param newName New name.
 */
static void renameSettingFile(const QString &oldName,
                              const QString &newName)
{
    QFile::remove(newName);
    QFile::rename(oldName, newName);
}

HomeFileDataStorePrivate::HomeFileDataStorePrivate(const QString &filePath) :
    settingsFileName(filePath), outOfSync(false), syncEnabled(true)
{
    syncTimer.setSingleShot(true);
    syncTimer.setInterval(1000);
}

HomeFileDataStore::HomeFileDataStore(const QString &filePath) :
    d_ptr(new HomeFileDataStorePrivate(filePath))
{
    takeSnapshot();

    connect(&d_ptr->syncTimer, SIGNAL(timeout()), this, SLOT(sync()));
    connect(qApp, SIGNAL(userActivityStateChanged(bool)), this, SLOT(setSyncDisabled(bool)));
}

HomeFileDataStore::~HomeFileDataStore()
{
    d_ptr->syncEnabled = true;
    sync();

    delete d_ptr;
}

void HomeFileDataStore::takeSnapshot()
{
    Q_D(HomeFileDataStore);

    QSettings settings(d->settingsFileName, QSettings::IniFormat);

    d->settingsSnapshot.clear();
    foreach(const QString & key, settings.allKeys()) {
        d->settingsSnapshot.insert(key, settings.value(key));
    }
}

void HomeFileDataStore::sync()
{
    Q_D(HomeFileDataStore);

    if(d->outOfSync && d->syncEnabled) {
        // The settings are first saved to a temporary file, and then that file is copied
        // over the original settings. This avoids clearing settings when there's no disk space.
        QString tempFileName = createTempFile(d->settingsFileName);

        if (!tempFileName.isEmpty()) {
            QSettings copiedSettings(tempFileName, QSettings::IniFormat);

            foreach(const QString &key, d->settingsSnapshot.keys()) {
                copiedSettings.setValue(key, d->settingsSnapshot.value(key));
            }

            copiedSettings.sync();

            if (copiedSettings.status() == QSettings::NoError) {
                renameSettingFile(tempFileName, d->settingsFileName);
                d->outOfSync = false;
            }
        }
    }
}

void HomeFileDataStore::setSyncDisabled(bool disabled)
{
    Q_D(HomeFileDataStore);

    d->syncEnabled = !disabled;

    if(d->syncEnabled) {
        d->syncTimer.start();
    }
}

bool HomeFileDataStore::createValue(const QString &key, const QVariant &value)
{
    Q_D(HomeFileDataStore);

    if(!d->settingsSnapshot.contains(key) || d->settingsSnapshot[key] != value) {
        d->outOfSync = true;
        d->settingsSnapshot[key] = value;
        emit valueChanged(key, value);
        d->syncTimer.start();
    }

    return true;
}

bool HomeFileDataStore::setValue(const QString &key, const QVariant &value)
{
    Q_D(HomeFileDataStore);

    if(d->settingsSnapshot.contains(key) && d->settingsSnapshot[key] != value) {
        d->outOfSync = true;
        d->settingsSnapshot[key] = value;
        emit valueChanged(key, value);
        d->syncTimer.start();
    }

    return true;
}

QVariant HomeFileDataStore::value(const QString &key) const
{
    Q_D(const HomeFileDataStore);

    return d->settingsSnapshot.value(key);
}

QStringList HomeFileDataStore::allKeys() const
{
    Q_D(const HomeFileDataStore);

    return d->settingsSnapshot.keys();
}

void HomeFileDataStore::remove(const QString &key)
{
    Q_D(HomeFileDataStore);

    if(d->settingsSnapshot.contains(key)) {
        d->outOfSync = true;
        d->settingsSnapshot.remove(key);
        emit valueChanged(key, QVariant());
        d->syncTimer.start();
    }
}

void HomeFileDataStore::clear()
{
    Q_D(HomeFileDataStore);

    d->outOfSync = true;
    d->settingsSnapshot.clear();
    d->syncTimer.start();
}

bool HomeFileDataStore::contains(const QString &key) const
{
    Q_D(const HomeFileDataStore);

    return d->settingsSnapshot.contains(key);
}

bool HomeFileDataStore::createValues(const QHash<QString, QVariant> &values)
{
    foreach (const QString &key, values.keys()) {
        createValue(key, values[key]);
    }

    return true;
}
