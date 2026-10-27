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

#include "ut_homefiledatastore.h"
#include "homefiledatastore.h"
#include "homefiledatastore_p.h"

// Map for storing the original settings values
QMap<QString, QVariant> * originalSettingsMap;
// Map for storing the changed settings values
QMap<QString, QVariant> * changedSettingsMap;
// Map to use for the current settings instance values
QMap<QString, QVariant> * settingsMapForNextQSettingsInstance;

// Indicator whether QSettings object has been synchronized.
bool gIsSynchronized;
// File in which the data will be stored.
QList<QString> gStoreFile;
// Status of the QSettings
QSettings::Status gQSettingsStatus;
// Whether QSettings is writable or not
bool gQSettingsIsWritable;
// Whether QSettings should synchronize itself when sync()ed
bool gSynchronize;
// Whether a sync fails
bool gQSettingsSyncFails;
//QSettings original file index
static const int ORIGINAL_SETTINGS_FILE_INDEX = 0;

// QMap for storing the settings object and the map for using settings values
QMap<const QSettings *, QMap<QString, QVariant> *> mapForQSettingsInstance;

//File path to data file
static const QString DATA_FILE_PATH("/tmp/store.data");

// Stubs of QSettings methods
QSettings::QSettings(const QString &fileName, Format format, QObject *parent)
{
    Q_UNUSED(format);
    Q_UNUSED(parent);
    gStoreFile.append(fileName);
    mapForQSettingsInstance.insert(this, settingsMapForNextQSettingsInstance);
}
QSettings::~QSettings()
{
}
QStringList QSettings::allKeys() const
{
    return mapForQSettingsInstance.value(this)->keys();
}
void QSettings::setValue(const QString &key, const QVariant &value)
{
    mapForQSettingsInstance.value(this)->insert(key, value);
}
QVariant QSettings::value(const QString &key, const QVariant &defaultValue) const
{
    if (mapForQSettingsInstance.value(this)->contains(key)) {
        return mapForQSettingsInstance.value(this)->value(key);
    } else {
        return defaultValue;
    }
}

void QSettings::sync()
{
    if (gQSettingsSyncFails) {
        return;
    }
    gIsSynchronized = true;
    if (gSynchronize) {
        mapForQSettingsInstance.value(this)->clear();
        foreach(const QString & key, settingsMapForNextQSettingsInstance->keys()) {
            mapForQSettingsInstance.value(this)->insert(key, settingsMapForNextQSettingsInstance->value(key));
        }
    }
}

void QSettings::remove(const QString &key)
{
    mapForQSettingsInstance.value(this)->remove(key);
}
void QSettings::clear()
{
    if (gQSettingsSyncFails) {
        return;
    }
    mapForQSettingsInstance.value(this)->clear();
}
bool QSettings::contains(const QString &key) const
{
    return mapForQSettingsInstance.value(this)->contains(key);
}
QSettings::Status QSettings::status() const
{
    if (gQSettingsSyncFails) {
        return QSettings::AccessError;
    }
    return gQSettingsStatus;
}
bool QSettings::isWritable() const
{
    return gQSettingsIsWritable;
}
QString QSettings::fileName() const
{
    return gStoreFile.at(ORIGINAL_SETTINGS_FILE_INDEX);
}

static bool fileExists = true;
bool QFileInfo::exists() const
{
    return fileExists;
}

QStringList gFileRenames;
bool QFile::rename(const QString &/*oldName*/, const QString &newName)
{
    gFileRenames << newName;
    return true;
}

//! Signal Receptor class
void SignalReceptor::valueChanged(const QString &key, QVariant value)
{
    keys.append(key);
    values.append(value);
}

void Ut_HomeFileDataStore::init()
{
    fileExists = true;
    gQSettingsSyncFails = false;
    gIsSynchronized = false;
    gSynchronize = false;
    gStoreFile.clear();
    gQSettingsStatus = QSettings::NoError;
    gQSettingsIsWritable = true;
    gFileRenames.clear();

    originalSettingsMap = new QMap<QString, QVariant>;
    settingsMapForNextQSettingsInstance = originalSettingsMap;

    m_subject = new HomeFileDataStore(DATA_FILE_PATH);
}

void Ut_HomeFileDataStore::cleanup()
{
    delete m_subject;
    m_subject = NULL;

    delete originalSettingsMap;
    originalSettingsMap = NULL;
}

void Ut_HomeFileDataStore::testInitialState()
{
    QVERIFY(disconnect(&m_subject->d_ptr->syncTimer, SIGNAL(timeout()), m_subject, SLOT(sync())));
}

void Ut_HomeFileDataStore::testFileOpening()
{
    QCOMPARE(gStoreFile.at(ORIGINAL_SETTINGS_FILE_INDEX), DATA_FILE_PATH);
}

void Ut_HomeFileDataStore::testValueSetting()
{
    SignalReceptor *receptor = new SignalReceptor;
    connect(m_subject, SIGNAL(valueChanged(QString, QVariant)), receptor, SLOT(valueChanged(QString, QVariant)));
    m_subject->setValue(QString("key"), QVariant(QString("value")));
    m_subject->sync();
    QVERIFY(!settingsMapForNextQSettingsInstance->contains("key"));
    QCOMPARE(receptor->keys.count(), 0);
    QCOMPARE(receptor->values.count(), 0);

    m_subject->createValue("key", "value");
    m_subject->sync();
    QCOMPARE(settingsMapForNextQSettingsInstance->value("key").toString(), QString("value"));
    QVERIFY(gIsSynchronized);
    // Verify that only one signal is sent with the given key/value
    QCOMPARE(receptor->keys.count(), 1);
    QCOMPARE(receptor->keys.at(0), QString("key"));
    QCOMPARE(receptor->values.count(), 1);
    QCOMPARE(receptor->values.at(0).toString(), QString("value"));

    m_subject->setValue(QString("key"), QVariant(QString("changed_value")));
    m_subject->sync();
    QCOMPARE(settingsMapForNextQSettingsInstance->value("key").toString(), QString("changed_value"));
    QVERIFY(gIsSynchronized);
    // Verify that another signal is sent with the new key/value
    QCOMPARE(receptor->keys.count(), 2);
    QCOMPARE(receptor->keys.at(1), QString("key"));
    QCOMPARE(receptor->values.count(), 2);
    QCOMPARE(receptor->values.at(1).toString(), QString("changed_value"));
    delete receptor;
}

void Ut_HomeFileDataStore::testValueReading()
{
    m_subject->createValue(QString("key"), QVariant(QString("value")));
    QCOMPARE(m_subject->value(QString("key")).toString(), QString("value"));
}

void Ut_HomeFileDataStore::testGettingAllKeys()
{
    QCOMPARE(m_subject->allKeys().size(), 0);

    m_subject->createValue(QString("key1"), QVariant(QString("foo")));
    m_subject->createValue(QString("key2"), QVariant(QString("bar")));
    QCOMPARE(m_subject->allKeys().size(), 2);
    QCOMPARE(m_subject->allKeys().at(0), QString("key1"));
    QCOMPARE(m_subject->allKeys().at(1), QString("key2"));
}

void Ut_HomeFileDataStore::testDataRemoval()
{
    m_subject->createValue(QString("key"), QVariant(QString("value")));

    m_subject->remove("key");
    QCOMPARE(m_subject->allKeys().size(), 0);
}

void Ut_HomeFileDataStore::testClear()
{
    m_subject->createValue(QString("key"), QVariant(QString("value")));
    m_subject->createValue(QString("key1"), QVariant(QString("value")));

    // Clear file data store
    m_subject->clear();

    // Verify that all keys were removed from file data store
    QCOMPARE(m_subject->allKeys().size(), 0);
}

void Ut_HomeFileDataStore::testContains()
{
    m_subject->createValue(QString("key"), QVariant(QString("value")));
    QCOMPARE(m_subject->contains("key"), true);

    m_subject->setValue(QString("he-man"), QVariant(QString("value")));
    QCOMPARE(m_subject->contains("he-man"), false);
}

void Ut_HomeFileDataStore::testCreatingValues()
{
    SignalReceptor receptor;
    connect(m_subject, SIGNAL(valueChanged(QString, QVariant)),
            &receptor, SLOT(valueChanged(QString, QVariant)));

    QHash<QString, QVariant> values;
    int amount = 3;
    for (int i = 0; i < amount; i++) {
        values.insert(QString("key%1").arg(i), QString("value%1").arg(i));
    }

    m_subject->createValues(values);
    m_subject->sync();

    QCOMPARE(receptor.keys.count(), 3);
    QCOMPARE(receptor.values.count(), 3);
    QCOMPARE(settingsMapForNextQSettingsInstance->count(), 3);
    for (int i = 0; i < amount; i++) {
        QString key = QString("key%1").arg(i);
        QString value = QString("value%1").arg(i);

        QCOMPARE(receptor.keys.at(i), key);
        QCOMPARE(receptor.values.at(i).toString(), value);

        QVERIFY(settingsMapForNextQSettingsInstance->contains(key));
        QCOMPARE(settingsMapForNextQSettingsInstance->value(key).toString(), value);
    }
}

void Ut_HomeFileDataStore::testUpdatingValuesWithCreateValues()
{
    SignalReceptor receptor;
    connect(m_subject, SIGNAL(valueChanged(QString, QVariant)),
            &receptor, SLOT(valueChanged(QString, QVariant)));

    QHash<QString, QVariant> values;
    int amount = 3;
    for (int i = 0; i < amount; i++) {
        values.insert(QString("key%1").arg(i), QString("value%1").arg(i));
    }
    m_subject->createValues(values);

    // Update value with one old and one new value
    QHash<QString, QVariant> newValues;
    int newAmount = 2;
    for (int i = amount - 1; i < amount + newAmount - 1; i++) {
        newValues.insert(QString("key%1").arg(i), QString("newValue%1").arg(i));
    }
    m_subject->createValues(newValues);
    m_subject->sync();

    QCOMPARE(receptor.keys.count(), 5);
    QCOMPARE(receptor.values.count(), 5);
    QCOMPARE(settingsMapForNextQSettingsInstance->count(), 4);
    // Verify old values (index 0&1)
    int amountOfOldValues = amount - 1;
    for (int i = 0; i < amountOfOldValues; i++) {
        QString key = QString("key%1").arg(i);
        QString value = QString("value%1").arg(i);

        QVERIFY(settingsMapForNextQSettingsInstance->contains(key));
        QCOMPARE(settingsMapForNextQSettingsInstance->value(key).toString(), value);
    }
    // Verify updated values (index 2&3)
    int amountOfUpdatedValues = amount + newAmount - 1;
    for (int i = amountOfOldValues; i < amountOfUpdatedValues; i++) {
        QString key = QString("key%1").arg(i);
        QString value = QString("newValue%1").arg(i);

        QVERIFY(settingsMapForNextQSettingsInstance->contains(key));
        QCOMPARE(settingsMapForNextQSettingsInstance->value(key).toString(), value);
    }
}

void Ut_HomeFileDataStore::testSync()
{
    QHash<QString, QVariant> values;
    int amount = 3;
    for (int i = 0; i < amount; i++) {
        values.insert(QString("key%1").arg(i), QString("value%1").arg(i));
    }
    m_subject->createValues(values);

    m_subject->d_ptr->syncTimer.stop();
    m_subject->setSyncDisabled(true);
    m_subject->sync();
    QVERIFY(!m_subject->d_ptr->syncTimer.isActive());
    QCOMPARE(settingsMapForNextQSettingsInstance->count(), 0);

    m_subject->setSyncDisabled(false);
    QVERIFY(m_subject->d_ptr->syncTimer.isActive());

    gQSettingsSyncFails = true;
    m_subject->sync();
    QCOMPARE(gFileRenames.count(), 0);

    gQSettingsSyncFails = false;
    m_subject->sync();
    QCOMPARE(gFileRenames.count(), 1);
    QCOMPARE(gFileRenames[0], DATA_FILE_PATH);
    QCOMPARE(settingsMapForNextQSettingsInstance->count(), amount);

    for (int i = 0; i < amount; i++) {
        QString key = QString("key%1").arg(i);
        QString value = QString("value%1").arg(i);

        QVERIFY(settingsMapForNextQSettingsInstance->contains(key));
        QCOMPARE(settingsMapForNextQSettingsInstance->value(key).toString(), value);
    }
}


QTEST_APPLESS_MAIN(Ut_HomeFileDataStore)
