/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
** Author: David Kedves <dkedves@blumsoft.eu>
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
#ifndef PROFILEWIDGETCONTAINER_H
#define PROFILEWIDGETCONTAINER_H

#include <MWidgetController>

class QPluginLoader;
class QGraphicsWidget;

class ProfileWidgetContainer : public MWidgetController
{
Q_OBJECT
public:
    ProfileWidgetContainer (QGraphicsWidget *parent = 0);
    ~ProfileWidgetContainer ();

public slots:
    void init ();

private:
    void pluginLoadSuccess (QGraphicsWidget *pluginWidget);
    void pluginLoadFailed ();

private:
    QPluginLoader   *m_loader;
    bool             m_initialized;
};

#endif
