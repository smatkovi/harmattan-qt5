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

#ifndef MIMSETTINGSCONTAINERSTORE_H
#define MIMSETTINGSCONTAINERSTORE_H

#include <QMap>
#include <QString>
#include <QList>
#include <QPointer>

class MContainer;
class MLabel;
class MAbstractInputMethodSettings;

class MImSettingsContainerStore
{
public:
    explicit MImSettingsContainerStore();
    ~MImSettingsContainerStore();

    //! Retranslate the labels:
    void retranslate();
    //! Add settings for one plugin:
    MContainer *addPluginSettings(const QString &plugin,
                                  MAbstractInputMethodSettings *settings);
    //! Interface for iteration:
    typedef QMap<QString, MContainer*>::const_iterator const_iterator;
    const_iterator constBegin() const
    { return settingsContainerMap.constBegin(); }
    const_iterator constEnd() const
    { return settingsContainerMap.constEnd(); }
private:

    // Container for all the settings containers;
    QMap<QString, MContainer*> settingsContainerMap;

    // All labels and corresponding settings plugins for retranslate:
    QMap<MAbstractInputMethodSettings*, MLabel*> settingsLabelMap;

    // List of created MContainers:
    // QPointer: Qt 5's QWeakPointer is no longer made from a plain QObject pointer
    QList< QPointer<MContainer> > containers;

};

#endif // MIMSETTINGSCONTAINERSTORE_H
