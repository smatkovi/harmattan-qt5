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
#include "alerttonepreview.h"
#include "static.h"
#include <QApplication>

#ifdef HAVE_LIBRESOURCEQT
#include <policy/resource-set.h>
ResourcePolicy::ResourceSet   *resources = 0;
ResourcePolicy::AudioResource *audioResource = 0;
#endif

//#define DEBUG
#define WARNING
#include "../debug.h"

static const char *GstStartCommand =
    "filesrc name=alerttonepreviewfilesrc ! "
    "decodebin ! audioconvert ! volume name=alerttonepreviewvolume ! "
    "pulsesink name=alerttonepreviewpulsesink";

AlertTonePreview::AlertTonePreview (const QString &fname) :
       QObject (0),
       m_gstPipeline (NULL),
       m_gstFilesrc (NULL),
       m_gstVolume (NULL),
       m_profileVolume (QString (ALERT_TONE_VOLUME_VOLUME_KEY)),
       m_Filename (fname)
{
#ifdef HAVE_LIBRESOURCEQT
    getResources ();
#endif

    gstInit ();

#ifndef HAVE_LIBRESOURCEQT
    audioResourceAcquired ();
#endif
}

AlertTonePreview::~AlertTonePreview ()
{
    SYS_DEBUG ("Stopping the playback.");
    gst_element_set_state (m_gstPipeline, GST_STATE_NULL);
    gst_bus_remove_signal_watch (gst_element_get_bus (m_gstPipeline));
    gst_object_unref (m_gstPipeline);
    m_gstPipeline = NULL;

#ifdef HAVE_LIBRESOURCEQT
   disconnect (resources,
               SIGNAL (resourcesGranted (QList<ResourcePolicy::ResourceType>)),
               this,
               SLOT (audioResourceAcquired ()));
   disconnect (resources,
               SIGNAL (lostResources ()),
               this,
               SLOT (audioResourceLost()));

    resources->release ();
#endif
}

void
AlertTonePreview::gstInit ()
{
    GError *err = NULL;
    SYS_DEBUG ("*** fname = %s", SYS_STR (m_Filename));
    SYS_DEBUG ("Starting the playback.");

    m_gstPipeline = gst_parse_launch (GstStartCommand, &err);
    if (err)
    {
        SYS_WARNING ("ERROR from gst_parse_launch: %s",
        err->message ? err->message : "Unknown error");
        g_error_free(err);
        goto finalize;
    }

    m_gstVolume = gst_bin_get_by_name(
        GST_BIN(m_gstPipeline), "alerttonepreviewvolume");
    m_gstFilesrc = gst_bin_get_by_name(
        GST_BIN(m_gstPipeline), "alerttonepreviewfilesrc");

    if (m_gstVolume && m_gstFilesrc) {
        g_object_set (G_OBJECT(m_gstVolume),
                "volume", profileToGstVolume(),
                NULL);
        g_object_set (G_OBJECT(m_gstFilesrc),
                "location", m_Filename.toUtf8().constData(),
                NULL);
    }

    gst_bus_add_signal_watch (gst_element_get_bus (m_gstPipeline));
    g_signal_connect (G_OBJECT (gst_element_get_bus (m_gstPipeline)),
                      "message", (GCallback) gstSignalHandler, this);

finalize:
    connect (&m_profileVolume, SIGNAL (changed ()),
             SLOT (profileVolumeChanged ()));
}

#ifdef HAVE_LIBRESOURCEQT
void
AlertTonePreview::getResources()
{
    /*
     * Create these objects only once during the run...
     */
    if (! resources)
    {
        resources = new ResourcePolicy::ResourceSet ("player");
        resources->setAutoRelease ();
        resources->setAlwaysReply ();
    }

    if (! audioResource)
    {
        /*
         * This is for the libresource to be able to identify our pulseaudio stream,
         * so this is very important that the same key-value pair should be set
         * for the pulsesink. The value should be uniqe.
         */
        audioResource = new ResourcePolicy::AudioResource ("player");
        audioResource->setProcessID (QApplication::applicationPid ());
        audioResource->setStreamTag("media.name", "*");

        resources->addResourceObject (audioResource);
        resources->initAndConnect ();
    }

    connect (resources,
             SIGNAL (resourcesGranted (QList<ResourcePolicy::ResourceType>)),
             SLOT (audioResourceAcquired ()));

    connect (resources,
             SIGNAL (lostResources ()),
             SLOT (audioResourceLost ()));

    connect (resources,
             SIGNAL (resourcesReleasedByManager ()),
             SLOT (audioResourceLost ()));

    resources->acquire ();
}
#endif

void
AlertTonePreview::audioResourceAcquired ()
{
    SYS_DEBUG("");
    if(m_gstPipeline)
        gst_element_set_state(m_gstPipeline, GST_STATE_PLAYING);
}

void
AlertTonePreview::audioResourceLost ()
{
    SYS_DEBUG("");
#ifdef HAVE_LIBRESOURCEQT
    resources->release ();
#endif

    if (m_gstPipeline)
    {
        gst_element_set_state (m_gstPipeline, GST_STATE_PAUSED);
        rewind ();
    }
}

void
AlertTonePreview::profileVolumeChanged ()
{
    g_object_set (G_OBJECT (m_gstVolume),
                  "volume", profileToGstVolume(),
                  NULL);
}

double
AlertTonePreview::profileToGstVolume ()
{
    QList<QVariant> range = m_profileVolume.possibleValues (NULL);

    if (range.count () < 2)
        return 0.0;

    return (((double) (m_profileVolume.value().toInt() - range[0].toInt())) /
            ((double)(range[1].toInt() - range[0].toInt())));
}

QString
AlertTonePreview::fname () const
{
    return m_Filename;
}

void
AlertTonePreview::rewind ()
{
    SYS_DEBUG ("");
    if (!gst_element_seek_simple (
           m_gstPipeline, GST_FORMAT_TIME,
           (GstSeekFlags) (GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), 0l))
        SYS_WARNING ("seek failed");
}

void
AlertTonePreview::gstSignalHandler (
    GstBus              *bus,
    GstMessage          *msg,
    AlertTonePreview    *atp)
{
    Q_UNUSED (bus);

    if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_ERROR)
    {
        GError *err = NULL;
        char *debug = NULL;

        gst_message_parse_error (msg, &err, &debug);
        SYS_WARNING ("\nfrom '%s'\ntype '%s'\n'error '%s'\ndebug '%s'",
                     GST_MESSAGE_SRC_NAME (msg), GST_MESSAGE_TYPE_NAME (msg),
                     (err && err->message) ? err->message : "Unknown error",
                     debug ? debug : "empty");

        g_error_free(err);
        g_free(debug);
    }
    else if (GST_MESSAGE_TYPE (msg) == GST_MESSAGE_EOS)
        atp->rewind ();
}

void
AlertTonePreview::freeResources ()
{
    SYS_DEBUG ("");
#ifdef HAVE_LIBRESOURCEQT
    delete resources;
    resources = 0;
    audioResource = 0;
/*
 * This causes crash, and done already by the ResourceSet :
    delete audioResource;
 */
#endif
}

