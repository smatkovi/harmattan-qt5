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
#include "profilewidgetcontainer.h"
#include <MApplicationExtensionInterface>
#include <MWidgetController>
#include <QGraphicsLinearLayout>
#include <QPluginLoader>
#include <QString>
#include <QFile>

#define DEBUG
#define WARNING
#include "../debug.h"

/*
 * The path to the closed-source profile+volume plugin
 */
static const QString pluginPath (M_APPLICATION_EXTENSION_DIR \
                                 "/libstatusindicatormenu-volume-nokia.so");

ProfileWidgetContainer::ProfileWidgetContainer (
        QGraphicsWidget *parent) :
        MWidgetController (parent),
        m_loader (0),
        m_initialized (false)
{
    setContentsMargins (0., 0., 0., 0.);
    setObjectName ("ProfileWidgetContainer");

    /*
     * Check whether the plugin is exists
     */
    if (QFile::exists (pluginPath))
    {
        setStyleName ("ProfileWidgetContainer");
    }
    else
    {
        setStyleName ("ProfileWidgetContainerNoPlugin");
    }
}

ProfileWidgetContainer::~ProfileWidgetContainer ()
{
    if (m_loader)
    {
        delete m_loader;
        m_loader = 0;
    }
}

void
ProfileWidgetContainer::init ()
{
    /*
     * init must be run only once
     */
    if (m_initialized)
        return;

    m_initialized = true;

    if (! QFile::exists (pluginPath))
        return;

    m_loader = new QPluginLoader (pluginPath);

    if (! m_loader->load ())
    {
        SYS_WARNING ("Can not load the plugin: %s", SYS_STR (pluginPath));
        SYS_WARNING ("ERROR: %s", SYS_STR (m_loader->errorString ()));
        delete m_loader;
        m_loader = 0;

        pluginLoadFailed ();
        return;
    }

    MApplicationExtensionInterface *pluginIf =
        qobject_cast<MApplicationExtensionInterface *>(m_loader->instance ());

    if (! pluginIf)
    {
        SYS_WARNING ("Error, the plugin can not be instantiated!");
        pluginLoadFailed ();
        return;
    }

    // Lets initialize the plugin
    pluginIf->initialize ("com.meego.core.MStatusIndicatorMenuExtensionInterface/1.0");

    QGraphicsWidget *pluginWidget = pluginIf->widget ();
    if (! pluginWidget)
    {
        SYS_WARNING ("Error, could not get the plugin's widget!");
        pluginLoadFailed ();
        return;
    }

    pluginLoadSuccess (pluginWidget);
}

void
ProfileWidgetContainer::pluginLoadFailed ()
{
    SYS_WARNING ("");

    /*
     * Lets 'hide' then ourselves...
     */
    setStyleName ("ProfileWidgetContainerNoPlugin");
}

void
ProfileWidgetContainer::pluginLoadSuccess (
    QGraphicsWidget     *pluginWidget)
{
    SYS_DEBUG ("");
    /*
     * Create a layout and put the widget into it
     */
    QGraphicsLinearLayout *mainLayout =
        new QGraphicsLinearLayout (Qt::Vertical);

    mainLayout->setSpacing (0.);
    mainLayout->setContentsMargins (0., 0., 0., 0.);

    mainLayout->addItem (pluginWidget);

    setLayout (mainLayout);
}

