/* This file is part of meegofeedbackd
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

#ifndef QPLUGINLOADER_H
#define QPLUGINLOADER_H

#include <QLibrary>
#include <QString>
#include <QObjectList>

class QPluginLoader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QLibrary::LoadHints loadHints READ loadHints WRITE setLoadHints)
public:
    explicit QPluginLoader(QObject *parent = 0);
    explicit QPluginLoader(const QString &fileName, QObject *parent = 0);
    ~QPluginLoader();

    QObject *instance();

    static QObjectList staticInstances();

    bool load();
    bool unload();
    bool isLoaded() const;

    void setFileName(const QString &fileName);
    QString fileName() const;

    QString errorString() const;

    void setLoadHints(QLibrary::LoadHints loadHints);
    QLibrary::LoadHints loadHints() const;

    /////
    // Now comes the mock stuff

    QString m_fileName;
    bool m_isLoaded;
    QObject *m_instance;
};

#endif //QPLUGINLOADER_H
