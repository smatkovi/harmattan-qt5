/* * This file is part of meego-im-framework *
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef MIMPLUGINSLOADER_H
#define MIMPLUGINSLOADER_H

#include <QObject>
#include <QStringList>

class QString;
class MImPlugins;

//! \brief The worker class for plugin loading.
class MImPluginsLoader : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MImPluginsLoader)
public:
    explicit MImPluginsLoader(MImPlugins *plugins, QObject *parent = 0);
    virtual ~MImPluginsLoader();

    //! \brief Start loading all plugins
    void start();
signals:
    //! \brief All plugins are loaded.
    void done();
private slots:
    void loadNext();
private:
    void loadPlugin(const QString& absoluteFileName);
    MImPlugins *plugins;
    QStringList workQueue;
};

#endif // MIMPLUGINSLOADER_H
