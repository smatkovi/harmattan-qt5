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

#ifndef GCONF_MOCK_H
#define GCONF_MOCK_H

#include <glib-object.h>

G_BEGIN_DECLS

/* From gconf-value.h */
typedef enum {
  GCONF_VALUE_INVALID,
  GCONF_VALUE_STRING,
  GCONF_VALUE_INT,
  GCONF_VALUE_FLOAT,
  GCONF_VALUE_BOOL,
  GCONF_VALUE_SCHEMA,

  /* unfortunately these aren't really types; we want list_of_string,
     list_of_int, etc.  but it's just too complicated to implement.
     instead we'll complain in various places if you do something
     moronic like mix types in a list or treat pair<string,int> and
     pair<float,bool> as the same type. */
  GCONF_VALUE_LIST,
  GCONF_VALUE_PAIR

} GConfValueType;

typedef struct _GConfValue GConfValue;

struct _GConfValue {
  GConfValueType type;

  // Invented for the test. Contains the mocked string.
  char *payload;
};

const char*    gconf_value_get_string    (const GConfValue *value);

typedef struct _GConfEntry GConfEntry;

struct _GConfEntry {
    char *key;
    GConfValue *value;
};

const char* gconf_entry_get_key         (const GConfEntry *entry);

/* From gconf-client.h */
typedef enum {
  GCONF_CLIENT_PRELOAD_NONE,
  GCONF_CLIENT_PRELOAD_ONELEVEL,
  GCONF_CLIENT_PRELOAD_RECURSIVE
} GConfClientPreloadType;

typedef struct _GConfClient       GConfClient;
typedef struct _GConfClientClass  GConfClientClass;

typedef void (*GConfClientNotifyFunc)(GConfClient* client,
                                      guint cnxn_id,
                                      GConfEntry *entry,
                                      gpointer user_data);

#define GCONF_TYPE_CLIENT                  (gconf_client_get_type ())
#define GCONF_CLIENT(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GCONF_TYPE_CLIENT, GConfClient))
#define GCONF_CLIENT_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), GCONF_TYPE_CLIENT, GConfClientClass))
#define GCONF_IS_CLIENT(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GCONF_TYPE_CLIENT))
#define GCONF_IS_CLIENT_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), GCONF_TYPE_CLIENT))
#define GCONF_CLIENT_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), GCONF_TYPE_CLIENT, GConfClientClass))

struct _GConfClient
{
    GObject object;

    /* public */

    /*< private >*/
};

struct _GConfClientClass
{
    GObjectClass parent_class;

    /* class members */

    /* Virtual public methods */
};

/* use the default engine */
GConfClient*      gconf_client_get_default             (void);

/* Add a directory to monitor and emit the value_changed signal and
   key notifications for.  Optionally pre-load the contents of this
   directory, much faster if you plan to access most of the directory
   contents.
*/

void              gconf_client_add_dir     (GConfClient* client,
                                            const gchar* dir,
                                            GConfClientPreloadType preload,
                                            GError** err);

/* Returns ID of the notification */
/* returns 0 on error, 0 is an invalid ID */
guint        gconf_client_notify_add(GConfClient* client,
                                     const gchar* namespace_section, /* dir or key to listen to */
                                     GConfClientNotifyFunc func,
                                     gpointer user_data,
                                     GFreeFunc destroy_notify,
                                     GError** err);

/* free the retval, if non-NULL */
gchar*       gconf_client_get_string(GConfClient* client, const gchar* key,
                                     GError** err);

GSList*      gconf_client_all_dirs       (GConfClient* client,
                                          const gchar* dir, GError** err);

void        gconf_entry_unref (GConfEntry *entry);

GSList*      gconf_client_all_entries    (GConfClient* client,
                                          const gchar* dir, GError** err);

G_END_DECLS

#endif
