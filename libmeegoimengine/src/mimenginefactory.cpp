/* This file is part of meegoimengine *
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved.
 * Contact: Nokia Corporation (directui@nokia.com)
 *
 * If you have questions regarding the use of this file, please contact
 * Nokia at directui@nokia.com.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mimenginefactory.h"

#include <QDir>
#include <QPluginLoader>
#include <QTimer>
#include <QtDebug>

namespace
{
    const QString defaultDriverDirWords = "/usr/lib/meego-imengines/drivers/words";
    const QString defaultDriverDirHwr = "/usr/lib/meego-imengines/drivers/hwr";

    //for searching convinience
    enum DriverType{
        Words,
        Hwr
    };
}

class MImEngineFactoryPrivate
{
    Q_DISABLE_COPY(MImEngineFactoryPrivate)
    Q_DECLARE_PUBLIC(MImEngineFactory)
    friend class Ut_MImEngineFactory;

protected:
    MImEngineFactoryPrivate();
    ~MImEngineFactoryPrivate();

    bool  verifyDriver(QObject *p, DriverType curType);
    void updateDrivers(DriverType curType);
    QStringList drivers();

    QObject *createEngine(const QString &driver, DriverType curType);
    void deleteEngine(QObject *engine);

    //1:driver name; 2:file absolute name
    QMap<QString, QString> driversMap;    

    //1: driver interface pointer 2 plugin pointer
    QMap<QObject*, QPluginLoader*>  pluginMap;

    QString driversPathWords;
    QString driversPathHwr;
    MImEngineFactory *q_ptr;
};

MImEngineFactoryPrivate::MImEngineFactoryPrivate()
    :driversPathWords(defaultDriverDirWords),
     driversPathHwr(defaultDriverDirHwr),
     q_ptr(0)
{
}

MImEngineFactoryPrivate::~MImEngineFactoryPrivate()
{
    QMap<QObject*, QPluginLoader*>::const_iterator i = pluginMap.constBegin();
    while (i != pluginMap.constEnd()) {
          QPluginLoader *plugin = i.value();
           if (plugin) {
              if (plugin->isLoaded()) {
                  plugin->unload();
              }
              delete plugin;
          }
          ++i;
      }
}

QStringList MImEngineFactoryPrivate::drivers( )
{    
    driversMap.clear();
    updateDrivers(Words);
    updateDrivers(Hwr);
    return driversMap.keys();
}

void MImEngineFactoryPrivate::updateDrivers(DriverType curType)
{
    QDir pluginsDir;
    QPluginLoader *plugin = 0;
    QStringList filterList("*.so");
    QObject *p = 0;
    QString absoluteFileName;

    //read the default directory for engine
    if ( curType == Words)
        pluginsDir.setPath(driversPathWords);
    else if (curType == Hwr)
        pluginsDir.setPath(driversPathHwr);

    foreach (const QString &fileName, pluginsDir.entryList(filterList, QDir::Files)) {
        MImEngineWordsInterface *driverWords = 0;
        MImEngineHwrInterface* driverHwr = 0;

        plugin = 0;
        p = 0;
        absoluteFileName = pluginsDir.absoluteFilePath(fileName);

        qDebug() << __PRETTY_FUNCTION__ << "try to load plugin: " << fileName;
        if (!absoluteFileName.isEmpty())
            plugin = new QPluginLoader(absoluteFileName);
        if (plugin) {
            p = plugin->instance();
            if (p) {
                if (curType == Words ) {
                    driverWords = qobject_cast<MImEngineWordsInterface *>(p);
                    if (driverWords)
                        driversMap.insert(driverWords->driver(), absoluteFileName);
                } else if( curType == Hwr ) {
                    driverHwr = qobject_cast<MImEngineHwrInterface *>(p);
                    if (driverHwr)
                        driversMap.insert(driverHwr->driver(), absoluteFileName);
                }
            } else {
                qDebug() << plugin->errorString();
            }

            if ( plugin && plugin->isLoaded()) {
                plugin->unload();
            }
            delete plugin;
        }
        else
            qDebug()<< __PRETTY_FUNCTION__ << "plugin load fails!";
    }
    qDebug() << __PRETTY_FUNCTION__ << "valid drivers:" << driversMap;
}

bool  MImEngineFactoryPrivate::verifyDriver(QObject *p, DriverType curType)
{
    bool validDriver = false;
    MImEngineWordsInterface *driverWords = 0;
    MImEngineHwrInterface *driverHwr = 0;

    switch(curType)
    {
    case Words:
        driverWords = qobject_cast<MImEngineWordsInterface *>(p);
            if (driverWords ) {
              if (!driverWords->init()) {
                    qDebug() << __PRETTY_FUNCTION__ << "load driver" << driverWords << "success, but init failed";
                }
                validDriver = true;
             }
        break;
    case Hwr:
        driverHwr = qobject_cast<MImEngineHwrInterface *>(p);
        if (driverHwr ) {
                if (!driverHwr->init()) {
                    qDebug() << __PRETTY_FUNCTION__ << "load driver" << driverHwr << "success, but init failed";
                }
                validDriver = true;
            }
        break;
    }

    return validDriver;
}

QObject *MImEngineFactoryPrivate::createEngine(const QString &driver,  DriverType curType)
{
    driversMap.clear();
    updateDrivers(Words);
    updateDrivers(Hwr);

    QPluginLoader *plugin = 0;
    QObject *p = 0;

    qDebug() << __PRETTY_FUNCTION__ << "try to create driver:" << driver;

    if(driversMap.contains(driver)) {
        QString absoluteFileName = driversMap.value(driver);
        qDebug() << __PRETTY_FUNCTION__ << "find driver file:" << absoluteFileName;
        plugin = new QPluginLoader(absoluteFileName);
        if (plugin)
            p = plugin->instance();
        if (p) {
            if (verifyDriver(p, curType) ) {
                qDebug() << __PRETTY_FUNCTION__ << "successfully create driver:" << driver;
                pluginMap.insert(p, plugin);
            } else {
                if (plugin) {
                    if (plugin->isLoaded())
                        plugin->unload();
                    delete plugin;
                }
            }
        } else
            qDebug() << __PRETTY_FUNCTION__ << "failed create driver:" << plugin->errorString();
    } else
        qDebug() << __PRETTY_FUNCTION__ << "can't find driver file for:" << driver;

    return p;
}

void MImEngineFactoryPrivate::deleteEngine(QObject *engine)
{
    if(engine && pluginMap.contains(engine)) {
        QPluginLoader *plugin = pluginMap.value(engine);
        if (plugin) {
            if (plugin->isLoaded()) {
                plugin->unload();
            }
            delete plugin;
        }
        pluginMap.remove(engine);
    }
}

MImEngineFactory::MImEngineFactory()
    :d_ptr(new MImEngineFactoryPrivate)
{
    Q_D(MImEngineFactory);
    d->q_ptr = this;
}

MImEngineFactory::~MImEngineFactory()
{
    delete d_ptr;
}

MImEngineFactory *MImEngineFactory::instance()
{
    static MImEngineFactory ap_instance;
    return &ap_instance;
}

QStringList MImEngineFactory::drivers()
{
    return d_ptr->drivers();
}

MImEngineWordsInterface *MImEngineFactory::createEngineWords(const QString &driver)
{
    return qobject_cast<MImEngineWordsInterface *>(d_ptr->createEngine(driver, Words));
}

MImEngineHwrInterface *MImEngineFactory::createEngineHwr(const QString &driver)
{
    return qobject_cast<MImEngineHwrInterface *>(d_ptr->createEngine(driver, Hwr));
}

void MImEngineFactory::deleteEngine(MImEngineWordsInterface *engine)
{
    d_ptr->deleteEngine(dynamic_cast<QObject*>(engine) );
}

void MImEngineFactory::deleteEngine(MImEngineHwrInterface *engine)
{
    d_ptr->deleteEngine(dynamic_cast<QObject*>(engine));
}

