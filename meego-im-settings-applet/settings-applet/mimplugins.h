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

#ifndef MIMPLUGINS_H
#define MIMPLUGINS_H

#include <QObject>
#include <QMap>
#include <QString>

class MAbstractInputMethodSettings;
class MInputMethodPlugin;
class MImPluginsLoader;

//! \internal
//! \brief Class that handles IM plugin loading and settings object creation.
//!
//! To start loading plugins, send signal to slot \c startLoading and
//! handle signals \c loaded(QString) and \c loadDone.
//! When a plugin is loaded, its settings object can be created with
//! \settingFor(plugin).

class MImPlugins : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MImPlugins)

public:
    explicit MImPlugins(QObject *parent = 0);
    virtual ~MImPlugins();

    //! \brief Create settings for the given plugin
    //! \param pluginName The (already loaded) plugin's name
    //! \return Settings object for the plugin or 0.
    MAbstractInputMethodSettings *settingsFor(const QString& pluginName);

signals:
    //! \brief Emitted when a plugin has been loaded
    //! \param pluginName the loaded plugin's name
    void loaded(const QString& pluginName);
    //! \brief Emitted when all plugins are loaded.
    void loadDone();

public slots:
    //! Start loading plugins.
    void startLoading();

private:
    friend class MImPluginsLoader;
    void addPlugin(const QString& pluginName, MInputMethodPlugin *plugin);

    QMap<QString, MInputMethodPlugin*> pluginMap;
    MImPluginsLoader *loader;
};

#endif // MIMPLUGINS_H
