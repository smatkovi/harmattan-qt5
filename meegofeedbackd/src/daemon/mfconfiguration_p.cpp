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

#include "mfconfiguration_p.h"

#include <QtDebug>
#include <mfcommon.h>

static void themeChangedCallback(GConfClient *client,
                                 guint cnxn_id,
                                 GConfEntry *entry,
                                 gpointer user_data);
static void volumeChangedCallback(GConfClient *client,
                                 guint cnxn_id,
                                 GConfEntry *entry,
                                 gpointer user_data);

MfConfigurationPrivate::~MfConfigurationPrivate()
{
    if (gconfClient) {
        g_object_unref(gconfClient);
        gconfClient = NULL;
    }
}

void MfConfigurationPrivate::init()
{
    GError *error = NULL;

    g_type_init();

    gconfClient = gconf_client_get_default();
    if (gconfClient == NULL) goto FAILED;

    gconf_client_add_dir(gconfClient,
                         MFCONFIGURATION_DIR,
                         GCONF_CLIENT_PRELOAD_NONE,
                         &error);
    if (handleGError(&error)) goto FAILED;

    gconf_client_notify_add(gconfClient,
                            MFCONFIGURATION_DIR"/theme/name",
                            themeChangedCallback,
                            this,
                            NULL, &error);
    if (handleGError(&error)) goto FAILED;

    gconf_client_notify_add(gconfClient,
                            MFCONFIGURATION_DIR"/input_feedback/volume",
                            volumeChangedCallback,
                            this,
                            NULL, &error);
    if (handleGError(&error)) goto FAILED;

    return;
 FAILED:
    if (gconfClient) {
        g_object_unref(gconfClient);
        gconfClient = NULL;
    }
}

bool MfConfigurationPrivate::handleGError(GError **error)
{
    if (error && *error) {
        qDebug() << (*error)->message;
        g_error_free(*error);
        *error = NULL;
        return true;
    } else {
        return false;
    }
}

void MfConfigurationPrivate::emitThemeChanged(QString newTheme)
{
    emit themeChanged(newTheme);
}

MfBackendBase::playbackVolume MfConfigurationPrivate::toVolume(const QString &volume, bool *ok)
{
    MfBackendBase::playbackVolume result = MfBackendBase::VolumeMedium;

    if (ok != NULL) {
        *ok = true;
    }

    if (volume.compare(MfBackendBase::volumeOffID, Qt::CaseInsensitive) == 0) {
        result = MfBackendBase::VolumeOff;
    } else if (volume.compare(MfBackendBase::volumeLowID, Qt::CaseInsensitive) == 0) {
        result = MfBackendBase::VolumeLow;
    } else if (volume.compare(MfBackendBase::volumeMediumID, Qt::CaseInsensitive) == 0) {
        result = MfBackendBase::VolumeMedium;
    } else if (volume.compare(MfBackendBase::volumeHighID, Qt::CaseInsensitive) == 0) {
        result = MfBackendBase::VolumeHigh;
    } else {
        if (ok != NULL) {
            *ok = false;
        }
    }

    return result;
}

void MfConfigurationPrivate::emitVolumeChanged(QString keyPath, QString newVolume)
{
    QString keyPathStart = MFCONFIGURATION_DIR"/input_feedback/volume/";
    MfBackendBase::playbackVolume volume;
    bool ok;
    int volumeId = -1;
    QString backend;

    if (!keyPath.startsWith(keyPathStart)) {
        // Got some garbage
        return;
    }

    keyPath.remove(0, keyPathStart.size());

    // Find volume id number and backend name
    QStringList keyPathParts = keyPath.split("/");
    if (keyPathParts.size() == 1) {
        backend = keyPathParts[0];
        volumeId = 0;
    } else if (keyPathParts.size() == 2) {
        backend = keyPathParts[1];
        if (keyPathParts[0].startsWith(volumePriorityPrefix)) {
            keyPathParts[0].remove(0, strlen(volumePriorityPrefix));
            volumeId = keyPathParts[0].toInt(&ok);
            if (ok == false || volumeId > maxVolumePriority) {
                volumeId = -1;
            }
        }
    }

    if (volumeId >= 0) {
        volume = toVolume(newVolume, &ok);
        if (ok == false) {
            // Unknown volume value
            qWarning() << "MfConfiguration: Received unknown volume value from GConf:"
                       << newVolume << "for backend:" << backend << "with volume ID:"
                       << volumeId << ". Reverting to default";
            volume = MfBackendBase::VolumeMedium;
        }
        emit volumeChanged(volumeId, backend, volume);
    }
}

static void themeChangedCallback(GConfClient *client,
                                 guint cnxn_id,
                                 GConfEntry *entry,
                                 gpointer user_data)
{
    MfConfigurationPrivate *d = (MfConfigurationPrivate*) user_data;
    Q_UNUSED(cnxn_id);
    Q_UNUSED(client);

    if (entry->value != NULL && entry->value->type == GCONF_VALUE_STRING) {
        d->emitThemeChanged(gconf_value_get_string(entry->value));
    }
}

static void volumeChangedCallback(GConfClient *client,
                                 guint cnxn_id,
                                 GConfEntry *entry,
                                 gpointer user_data)
{
    MfConfigurationPrivate *d = (MfConfigurationPrivate*) user_data;
    Q_UNUSED(cnxn_id);
    Q_UNUSED(client);

    if (entry->value != NULL && entry->value->type == GCONF_VALUE_STRING) {
        d->emitVolumeChanged(gconf_entry_get_key(entry),
                             gconf_value_get_string(entry->value));
    }
}

QString MfConfigurationPrivate::getTheme()
{
    gchar *str = NULL;
    GError *error = NULL;
    QString result;

    if (gconfClient == NULL) {
        // GConf is not available. Let's keep with the default theme then.
        return MeegoDefaultTheme;
    }

    str = gconf_client_get_string(gconfClient,
                                  MFCONFIGURATION_DIR"/theme/name",
                                  &error);
    if (handleGError(&error)) {
        result = MeegoDefaultTheme;
    } else if (str) {
        result = str;
        g_free(str);
        str = NULL;
    } else {
        qWarning("MfConfiguration: GConf \""MFCONFIGURATION_DIR"/theme/name\" is NULL");
        result = MeegoDefaultTheme;
    }

    return result;
}

QMap<QString, MfBackendBase::playbackVolume> MfConfigurationPrivate::getVolumesOfDir(const char* dirStr)
{
    GError *error = NULL;
    GSList *valueList;
    QMap<QString, MfBackendBase::playbackVolume> retVal;

    valueList = gconf_client_all_entries(gconfClient,
                                         dirStr,
                                         &error);

    if (handleGError(&error) || !valueList) {
        qWarning("MfConfiguration: Error getting GConf entries from directory: %s\n",
                 dirStr);
        return retVal;
    }

    // Get all volumes in the directory
    for (GSList *item = valueList; item != NULL; item = item->next) {
        GConfEntry *entry = (GConfEntry*)item->data;

        if (entry == NULL || entry->value == NULL ||
            entry->value->type != GCONF_VALUE_STRING) {
             continue;
        }

        QString volume = gconf_value_get_string(entry->value);
        QString keyPath = gconf_entry_get_key(entry);

        QString backend = keyPath.section("/", -1);
        retVal[backend] = toVolume(volume);
    }

    g_slist_foreach(valueList, (GFunc)gconf_entry_unref, NULL);
    g_slist_free(valueList);
    valueList = NULL;

    return retVal;
}

QList<QMap<QString, MfBackendBase::playbackVolume> > MfConfigurationPrivate::getVolumes()
{
    QList<QMap<QString, MfBackendBase::playbackVolume> > volumes;
    const char *const feedbackVolumeDir = MFCONFIGURATION_DIR"/input_feedback/volume";

    // Preallocate volume map list
    for (int i = 0; i <= maxVolumePriority; ++i) {
        volumes << QMap<QString, MfBackendBase::playbackVolume>();
    }

    if (gconfClient == NULL) {
        // GConf is not available. Return empty volumes.
        return volumes;
    }

    // Get volumes for volume ID 0
    volumes[0] = getVolumesOfDir(feedbackVolumeDir);

    // Get volumes for following volume ID's
    for (int i = 1; i <= maxVolumePriority; ++i) {
        QString volumeIdDir = feedbackVolumeDir;
        volumeIdDir += "/";
        volumeIdDir += volumePriorityPrefix;
        volumeIdDir += QString::number(i);
        volumes[i] = getVolumesOfDir(volumeIdDir.toLatin1().constData());
    }

    return volumes;
}

void MfConfigurationPrivate::stringListFromGSList(QStringList &qlist,
                                                  GSList *glist)
{
    GSList *item;
    qlist.clear();

    item = glist;
    while (item != NULL) {
        GConfValue *value = (GConfValue*)item->data;
        if (value != NULL && value->type == GCONF_VALUE_STRING) {
            qlist << gconf_value_get_string((GConfValue*)item->data);
        }
        item = item->next;
    }
}
