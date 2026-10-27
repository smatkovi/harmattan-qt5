/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (directui@nokia.com)
**
** This file is part of meegotouch-controlpanelsoundsettingsapplet.
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
#include "soundsettingsapplet.h"

#include <gst/gst.h>
#include <MAction>

#include "alerttone.h"
#include "alerttonebrowserstylable.h"
#include "alerttoneappletwidget.h"

// for freeing up the singletons
#include "alerttonepreview.h"
#include "trackerconnection.h"
#include "profilebackend.h"
#include "alerttonebrowsersheet.h"

//#define DEBUG
#define WARNING
#include "../debug.h"

#ifndef UNIT_TEST
#include <MLibrary>
M_LIBRARY
#endif

Q_EXPORT_PLUGIN2 (soundsettingsapplet, SoundSettingsApplet)

int    gst_argc = 1;
char** gst_argv = NULL;

SoundSettingsApplet::SoundSettingsApplet ()
{
}

SoundSettingsApplet::~SoundSettingsApplet ()
{
    gst_deinit ();

    if ((gst_argv != NULL) && (gst_argv[0] != NULL))
    {
        delete[] gst_argv[0];
        delete[] gst_argv;
    }

    // free up the m_alertTones member
    if (! m_alertTones.isEmpty ())
    {
        foreach (AlertTone *tone, m_alertTones)
            delete tone;
        m_alertTones.clear ();
    }

    /*
     * And then free up the singletons
     */
    delete ProfileBackend::getInstance ();
    delete TrackerConnection::instance ();

    AlertTonePreview::freeResources ();

    SysDebug::closeDebugFile ();
}

void
SoundSettingsApplet::init ()
{
    SYS_DEBUG ("");
    gst_argv = new char*[2];
    gst_argv[0] = qstrdup ("app");
    gst_argv[1] = NULL;

    gst_init (&gst_argc, &gst_argv);

    m_alertTones = AlertTone::alertTones ();
}

/* widgetId: 0xaaaabbbb where
   0xaaaa is the widget ID and
   0xbbbb is the alert tone index */
DcpStylableWidget *
SoundSettingsApplet::constructStylableWidget (int widgetId)
{
    DEBUG_CLOCK_START;
    SYS_DEBUG ("%s: widgetId = %d", SYS_TIME_STR, widgetId);

    DcpStylableWidget *newWidget = 0;
    int realWidgetId = widgetId / 65536;

    if (realWidgetId == AlertToneAppletWidget_id)
    {
        /*
         * Do not re-create the main view if it is on top already
         */
        if (m_stack.isEmpty () ||
            qobject_cast<AlertToneAppletWidget*>(m_stack.top ()) == 0) {
            newWidget = new AlertToneAppletWidget (m_alertTones);
            connect (newWidget, SIGNAL(showWidget(int)),
                    this, SLOT(showWidget(int)));
        } else {
            return m_stack.top ();
        }
    }
    else if (realWidgetId == AlertToneBrowser_id)
    {
        /*
         * Lets handle the AlertToneBrowser widgets here
         */
        int alertToneIdx = widgetId - realWidgetId * 65536;

        if (alertToneIdx >= 0 && alertToneIdx < m_alertTones.size ())
            newWidget = new AlertToneBrowserStylable (m_alertTones[alertToneIdx]);
    }

    if (newWidget)
    {
        m_stack.push (newWidget);
        connect (newWidget, SIGNAL (destroyed (QObject *)),
                 SLOT (toplevelDestroyed (QObject *)));
    }

    SYS_DEBUG ("%s [DONE]: widgetId = %d", SYS_TIME_STR, widgetId);
    DEBUG_CLOCK_END("widget creation");
    return newWidget;
}

void
SoundSettingsApplet::toplevelDestroyed (QObject *goner)
{
    if (m_stack.size () == 0)
        return;

    if (goner == qobject_cast<QObject *> (m_stack.top ()))
        m_stack.pop ();
}

QString
SoundSettingsApplet::title () const
{
    //% "Alert tones"
    QString title = qtTrId ("qtn_sond_sounds");

    if ((m_stack.size () > 0) && (m_stack.top ()))
        title = qobject_cast<DcpStylableWidget *> (m_stack.top ())->title ();

    return title;
}

QVector<MAction *>
SoundSettingsApplet::viewMenuItems ()
{
    QVector<MAction*> vector;

    return vector;
}

DcpBrief *
SoundSettingsApplet::constructBrief (int partId)
{
    Q_UNUSED (partId);
    return 0;
}

void
SoundSettingsApplet::showWidget (
        int widgetId)
{
    int realWidgetId = widgetId / 65536;
    int alertToneIdx = widgetId - realWidgetId * 65536;
    AlertToneBrowserSheet *sheet;

    sheet = new AlertToneBrowserSheet(m_alertTones[alertToneIdx]);
    sheet->appear(m_stack.top()->scene(), MSceneWindow::DestroyWhenDone);
}
