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

#include "gconf_mock.h"

#include <QtGlobal>
#include <QtDebug>

#include "globals.h"

G_DEFINE_TYPE (GConfClient, gconf_client, G_TYPE_OBJECT);

/* Init functions */
static void
gconf_client_class_init (GConfClientClass *klass)
{
    Q_UNUSED(klass);
}

static void
gconf_client_init (GConfClient *self)
{
    Q_UNUSED(self);

    /* Initialize all public and private members to reasonable default values. */

    /* Initialize public fields */

    /* Initialize private fields */

    /* If you need specific construction properties to complete initialization,
     * delay initialization completion until the property is set.
     */

}

gchar* gconf_client_get_string(GConfClient* client,
                               const gchar* key,
                               GError** err)
{
    Q_UNUSED(client);
    Q_UNUSED(key);
    Q_UNUSED(err);
    return 0;
}

GConfClient* gconf_client_get_default(void)
{
    GConfClient *client;

    client = (GConfClient*)g_object_new(gconf_client_get_type(), NULL);

    return client;
}

void gconf_client_add_dir(GConfClient* client,
                          const gchar* dir,
                          GConfClientPreloadType preload,
                          GError** err)
{
    Q_UNUSED(client);
    Q_UNUSED(dir);
    Q_UNUSED(preload);
    Q_UNUSED(err);
}

const char* gconf_value_get_string(const GConfValue *value)
{
    Q_UNUSED(value);

    return value->payload;
}

GSList* gconf_client_all_dirs(GConfClient* client,
                              const gchar* dir, GError** err)
{
    Q_UNUSED(client);
    Q_UNUSED(dir);
    Q_UNUSED(err);

    return NULL;
}

const char* gconf_entry_get_key         (const GConfEntry *entry)
{
    Q_UNUSED(entry);

    return entry->key;
}

guint gconf_client_notify_add(GConfClient* client,
                              const gchar* namespace_section,
                              GConfClientNotifyFunc func,
                              gpointer user_data,
                              GFreeFunc destroy_notify,
                              GError** err)
{
    Q_UNUSED(client);
    Q_UNUSED(destroy_notify);
    Q_UNUSED(err);

    if (g_strrstr(namespace_section, "theme") != 0) {
        g_gconfClientNotifyThemeUserData = user_data;
        g_gconfClientNotifyThemeFuncPtr = func;
    } else if (g_strrstr(namespace_section, "volume") != 0) {
        g_gconfClientNotifyVolumeUserData = user_data;
        g_gconfClientNotifyVolumeFuncPtr = func;
    }

    return 0;
}

void gconf_entry_unref(GConfEntry *entry)
{
    Q_UNUSED(entry);
}

GSList* gconf_client_all_entries(GConfClient* client,
                                 const gchar* dir, GError** err)
{
    Q_UNUSED(client);
    Q_UNUSED(dir);
    Q_UNUSED(err);
    GSList *retVal = NULL, *itemList = NULL;

    if (g_strcmp0(dir, "/meegotouch/input_feedback/volume") == 0 &&
        g_gconfClientAllEntriesList0 != NULL) {
        itemList = g_gconfClientAllEntriesList0;
    } else if (g_strcmp0(dir, "/meegotouch/input_feedback/volume/priority1") == 0 &&
               g_gconfClientAllEntriesList1 != NULL) {
        itemList = g_gconfClientAllEntriesList1;
    } else if (g_strcmp0(dir, "/meegotouch/input_feedback/volume/priority2") == 0 &&
               g_gconfClientAllEntriesList2 != NULL) {
        itemList = g_gconfClientAllEntriesList2;
    } else {
        return NULL;
    }

    // Only allocate new list, content will be "freed" with
    // mocked gconf_entry_unref that doesn't actually do anything.
    for (GSList *item = itemList; item != NULL; item = item->next) {
        retVal = g_slist_append(retVal, item->data);
    }
    return retVal;
}

