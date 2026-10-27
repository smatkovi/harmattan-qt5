/**
  @file conn_settings.c
    
  libconnsettings - Connectivity settings library.

  Copyright (C) 2010 Nokia Corporation. All rights reserved.

  Contact: Jukka Rissanen <jukka.rissanen@nokia.com>
    
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  version 2.1 as published by the Free Software Foundation.

  This library is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
  02110-1301 USA
*/

#include <string.h>
#include <gconf/gconf-client.h>

#include <osso-ic-gconf.h>

#include "conn_settings.h"

typedef struct ConnSettingsNotifier_ {
	ConnSettingsType type;
	guint notify_id;
	ConnSettingsNotifyFunc func;
	void *user_data;
	gchar *dir;
	gchar *id;
} ConnSettingsNotifier;


struct ConnSettings_ {
	GConfClient *gconf_client;
	ConnSettingsType type;
	gchar *id;
	gchar *escaped_id;
	gchar *dir;

	ConnSettingsNotifier *notifier;
};


ConnSettingsValue *conn_settings_value_new()
{
	return (ConnSettingsValue *)g_try_malloc0(sizeof(ConnSettingsValue));
}

static ConnSettingsValue *conn_settings_create_value(ConnSettingsValueType type)
{
	ConnSettingsValue *value = conn_settings_value_new();
	value->type = type;
	return value;
}

void conn_settings_value_destroy(ConnSettingsValue *value)
{
	int i;

	if (!value)
		return;

	switch (value->type) {
	case CONN_SETTINGS_VALUE_STRING:
		g_free(value->value.string_val);
		break;
	case CONN_SETTINGS_VALUE_BYTE_ARRAY:
		g_free(value->value.byte_array.val);
		break;
	case CONN_SETTINGS_VALUE_LIST:
		for (i=0; value->value.list_val[i]; i++) {
			conn_settings_value_destroy(value->value.list_val[i]);
		}
		g_free(value->value.list_val);
		break;
	case CONN_SETTINGS_VALUE_INT:
	case CONN_SETTINGS_VALUE_DOUBLE:
	case CONN_SETTINGS_VALUE_BOOL:
	default:
		break;
	}

	g_free(value->data);
	g_free(value);
}


void conn_settings_values_destroy(ConnSettingsValue **value)
{
	ConnSettingsValue *tmp;

	if (!value)
		return;

	if (*value == NULL) {
		/* Empty value */
		g_free(value);
		return;
	}

	tmp = conn_settings_value_new();
	tmp->type = CONN_SETTINGS_VALUE_LIST;
	tmp->value.list_val = value;
	conn_settings_value_destroy(tmp);
	return;
}


char *conn_settings_escape_string(const char *str)
{
	return gconf_escape_key(str, -1);
}


static char *escape_key(const char *key)
{
	/* If the key contains '/' chars, then we must escape
	 * them individually.
	 */
	char **result;
	char *escaped_key, *tmp;
	int i = 0;

	if (g_strrstr(key, "/") == NULL)
		return conn_settings_escape_string(key);

	result = g_strsplit(key, "/", -1);
	while (result[i]) {
		tmp = conn_settings_escape_string(result[i]);
		g_free(result[i]);
		result[i] = tmp;
		i++;
	}
	escaped_key = g_strjoinv("/", result);
	g_strfreev(result);
	return escaped_key;
}


/** Print out a gconf error
 * @param error reference to the GError
 */
static int conn_settings_gconf_check_error(GError **error)
{
	if (*error) {
		g_debug("connectivity settings gconf error: %s", (*error)->message);
		g_clear_error(error);
		*error = NULL;
		return CONN_SETTINGS_E_GCONF_ERROR;
	}
	return 0;
}


char **conn_settings_list_ids(ConnSettingsType type)
{
	GSList *ids, *id;
	gchar *dir;
	GError *error = NULL;
	gchar *unescaped_name;
	char **entries = NULL;
	int num_entries = 0;
	GConfClient *gconf_client;
	gchar *entry_name;

	switch (type) {
	case CONN_SETTINGS_GENERAL:
		dir = ICD_GCONF_SETTINGS;
		break;
	case CONN_SETTINGS_NETWORK_TYPE:
		dir = ICD_GCONF_NETWORK_MAPPING;
		break;
	case CONN_SETTINGS_CONNECTION:
		dir = ICD_GCONF_PATH;
		break;
	case CONN_SETTINGS_SERVICE_TYPE:
		dir = ICD_GCONF_SRV_PROVIDERS;
		break;
	default:
		return NULL;
	}

	g_type_init();

	gconf_client = gconf_client_get_default();
	ids = gconf_client_all_dirs(gconf_client,
				    dir,
				    &error);
	if (error) {
		g_object_unref(gconf_client);
		conn_settings_gconf_check_error(&error);
		return NULL;
	}

	/* convert to string pointer array */
	for (id = ids; id; id = g_slist_next(id)) {
		entry_name = g_strrstr(id->data, "/");
		if (entry_name != NULL) {
			entry_name += 1;
			unescaped_name = gconf_unescape_key(entry_name, -1);
			//g_debug("%s():dir=%s\n", __FUNCTION__, unescaped_name);
			entries = (char **)g_realloc(entries, (num_entries+1)*sizeof(char *));
			entries[num_entries++] = unescaped_name;
		}
		g_free(id->data);
	}
	g_slist_free(ids);

	if (entries) {
		entries = (char **)g_realloc(entries, (num_entries+1)*sizeof(char *));
		entries[num_entries] = NULL;
	}

	g_object_unref (gconf_client);
	return entries;
}


ConnSettings *conn_settings_open(ConnSettingsType type, const char *id)
{
	ConnSettings *ctx;

	g_type_init();

	ctx = (ConnSettings *)g_try_malloc0(sizeof(ConnSettings));
	if (!ctx)
		return NULL;

	if (id)
		ctx->escaped_id = gconf_escape_key(id, -1);
	else
		ctx->escaped_id = NULL;

	switch (type) {
	case CONN_SETTINGS_GENERAL:
		if (id)
			ctx->dir = g_strdup_printf(ICD_GCONF_SETTINGS "/%s", ctx->escaped_id);
		else
			ctx->dir = g_strdup_printf(ICD_GCONF_SETTINGS);
		break;
	case CONN_SETTINGS_NETWORK_TYPE:
		if (id)
			ctx->dir = g_strdup_printf(ICD_GCONF_NETWORK_MAPPING "/%s", ctx->escaped_id);
		else
			ctx->dir = g_strdup_printf(ICD_GCONF_NETWORK_MAPPING);
		break;
	case CONN_SETTINGS_CONNECTION:
		if (id)
			ctx->dir = g_strdup_printf(ICD_GCONF_PATH "/%s", ctx->escaped_id);
		else
			ctx->dir = g_strdup_printf(ICD_GCONF_PATH);
		break;
	case CONN_SETTINGS_SERVICE_TYPE:
		if (id)
			ctx->dir = g_strdup_printf(ICD_GCONF_SRV_PROVIDERS "/%s", ctx->escaped_id);
		else
			ctx->dir = g_strdup_printf(ICD_GCONF_SRV_PROVIDERS);
		break;
	default:
		g_debug("%s():Invalid type %d", __FUNCTION__, type);
		g_free(ctx->escaped_id);
		return NULL;
	}

	ctx->gconf_client = gconf_client_get_default();
	ctx->type = type;

	if (id)
		ctx->id = g_strdup(id);

	return ctx;
}


char **conn_settings_list_keys(ConnSettings *ctx)
{
	GSList *keys, *key;
	GError *error = NULL;
	int num_entries = 0;
	gchar *entry_name;
	GConfEntry *entry;
	char **entries = NULL;
	gchar *unescaped_name;

	if (!ctx)
		return NULL;

	keys = gconf_client_all_entries(ctx->gconf_client,
					ctx->dir,
					&error);
	if (error) {
		conn_settings_gconf_check_error(&error);
		return NULL;
	}

	/* convert to pointer array to strings */
	for (key = keys; key; key = g_slist_next(key)) {
		entry = (GConfEntry *)key->data;
		entry_name = g_strrstr(gconf_entry_get_key(entry), "/");
		if (entry_name != NULL) {
			entry_name += 1;
			unescaped_name = gconf_unescape_key(entry_name, -1);
			//g_debug("%s():%p:[%d]=%s (%s)\n", __FUNCTION__, entries, num_entries, entry_name, unescaped_name);
			entries = (char **)g_realloc(entries, (num_entries+1)*sizeof(char *));
			entries[num_entries++] = unescaped_name;
		}
		gconf_entry_unref(entry);
	}
	g_slist_free(keys);

	if (entries) {
		entries = (char **)g_realloc(entries, (num_entries+1)*sizeof(char *));
		entries[num_entries] = NULL;
	}

	return entries;
}


static ConnSettingsValue *create_value_from_backend(GConfValue *backend_value)
{
	ConnSettingsValue *value = NULL;
	GSList *entry, *list;
	int list_len, i;

	if (backend_value == NULL) {
		/* key is not set */
		value = conn_settings_create_value(CONN_SETTINGS_VALUE_INVALID);
		return value;
	}

	switch (backend_value->type) {
	case GCONF_VALUE_STRING:
		value = conn_settings_create_value(CONN_SETTINGS_VALUE_STRING);
		value->value.string_val = (char *)g_strdup(gconf_value_get_string(backend_value));
		break;
	case GCONF_VALUE_INT:
		value = conn_settings_create_value(CONN_SETTINGS_VALUE_INT);
		value->value.int_val = gconf_value_get_int(backend_value);
		break;
	case GCONF_VALUE_FLOAT:
		value = conn_settings_create_value(CONN_SETTINGS_VALUE_DOUBLE);
		value->value.double_val = gconf_value_get_float(backend_value);
		break;
	case GCONF_VALUE_BOOL:
		value = conn_settings_create_value(CONN_SETTINGS_VALUE_BOOL);
		value->value.bool_val = (int)gconf_value_get_bool(backend_value);
		break;
	case GCONF_VALUE_LIST:
		list = gconf_value_get_list(backend_value);
		list_len = g_slist_length(list);

		value = conn_settings_create_value(CONN_SETTINGS_VALUE_LIST);
		value->value.list_val = g_new0(struct ConnSettingsValue_ *, list_len + 1);
		for (i=0, entry = list; entry; entry = entry->next, i++) {
			GConfValue *list_val = (GConfValue *)entry->data;
			ConnSettingsValue *val = create_value_from_backend(list_val);
			if (val && (val->type != CONN_SETTINGS_VALUE_INVALID))
				value->value.list_val[i] = val;
			else {
				int j;
				for (j=0; j<i; j++)
					conn_settings_value_destroy(value->value.list_val[j]);
				g_free(value->value.list_val);
				value->type = CONN_SETTINGS_VALUE_INVALID;
				break;
			}
		}
		break;
	case GCONF_VALUE_SCHEMA:
	case GCONF_VALUE_PAIR:
	default:
		value = conn_settings_create_value(CONN_SETTINGS_VALUE_INVALID);
		break;
	}

	return value;
}


ConnSettingsValue *conn_settings_get(ConnSettings *ctx, const char *key)
{
	GConfValue *backend_value;
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	ConnSettingsValue *value = NULL;

	if (!ctx)
		return NULL;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return NULL;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return NULL;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	backend_value = gconf_client_get(ctx->gconf_client, path, &error);
	g_free(escaped_key);
	g_free(path);

	if (!(conn_settings_gconf_check_error(&error) < 0)) {
		value = create_value_from_backend(backend_value);
		if (value == NULL)
			value = conn_settings_create_value(CONN_SETTINGS_VALUE_INVALID);
	}
	if (backend_value)
		gconf_value_free(backend_value);

	return value;
}


int conn_settings_get_string(ConnSettings *ctx, const char *key, char **value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	*value = gconf_client_get_string(ctx->gconf_client,
					 path,
					 &error);
	if (*value == NULL) {
		ret = CONN_SETTINGS_E_NO_SUCH_KEY;
		goto cleanup;
	}
	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_get_int(ConnSettings *ctx, const char *key, int *value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	int backend_value;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	backend_value = gconf_client_get_int(ctx->gconf_client,
					     path,
					     &error);
	if (backend_value == 0) {
		/* Check if the key has a value or not */
		GConfEntry *entry;
		entry = gconf_client_get_entry(ctx->gconf_client,
					       path,
					       NULL,
					       TRUE,
					       NULL);
		if (!entry) {
			ret = CONN_SETTINGS_E_GCONF_ERROR;
			goto cleanup;
		} else if (!entry->value) {
			ret = CONN_SETTINGS_E_NO_SUCH_KEY;
			gconf_entry_unref(entry);
			goto cleanup;
		}
		gconf_entry_unref(entry);
	}
	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

	*value = backend_value;

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_get_double(ConnSettings *ctx, const char *key, double *value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	double backend_value;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	backend_value = gconf_client_get_float(ctx->gconf_client,
					       path,
					       &error);
	if (!backend_value) {
		/* Check if the key has a value or not */
		GConfEntry *entry;
		entry = gconf_client_get_entry(ctx->gconf_client,
					       path,
					       NULL,
					       TRUE,
					       NULL);
		if (!entry) {
			ret = CONN_SETTINGS_E_GCONF_ERROR;
			goto cleanup;
		} else if (!entry->value) {
			ret = CONN_SETTINGS_E_NO_SUCH_KEY;
			gconf_entry_unref(entry);
			goto cleanup;
		}
		gconf_entry_unref(entry);
	}

	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

	*value = backend_value;

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_get_bool(ConnSettings *ctx, const char *key, int *value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	int backend_value;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	backend_value = gconf_client_get_bool(ctx->gconf_client,
					      path,
					      &error);
	if (!backend_value) {
		/* Check if the key has a value or not */
		GConfEntry *entry;
		entry = gconf_client_get_entry(ctx->gconf_client,
					       path,
					       NULL,
					       TRUE,
					       NULL);
		if (!entry) {
			ret = CONN_SETTINGS_E_GCONF_ERROR;
			goto cleanup;
		} else if (!entry->value) {
			ret = CONN_SETTINGS_E_NO_SUCH_KEY;
			gconf_entry_unref(entry);
			goto cleanup;
		}
		gconf_entry_unref(entry);
	}

	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

	*value = backend_value;

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_get_byte_array(ConnSettings *ctx,
				 const char *key,
				 unsigned char **value,
				 unsigned int *value_len)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	GConfValue *backend_value;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value_len) {
		g_debug("%s():value length not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);

	backend_value = gconf_client_get(ctx->gconf_client, path, &error);
	if (backend_value == NULL) {
		if (conn_settings_gconf_check_error(&error) < 0) {
			ret = CONN_SETTINGS_E_GCONF_ERROR;
			goto cleanup;
		}
		ret = CONN_SETTINGS_E_NO_SUCH_KEY;
		goto cleanup;
	}

	switch (backend_value->type) {
	case GCONF_VALUE_STRING:
		/* support also string to byte array conversion */
		*value = (unsigned char *)g_strdup(gconf_value_get_string(backend_value));
		*value_len = (unsigned int)strlen((char*)*value);
		break;

	case GCONF_VALUE_LIST:
		if (gconf_value_get_list_type(backend_value) == GCONF_VALUE_INT) {
			int i;
			GSList *list, *entry;
			list = gconf_value_get_list(backend_value);
			*value_len = g_slist_length(list);
			*value = g_new(unsigned char, *value_len + 1);
			for (i = 0, entry = list; entry; entry = entry->next, i++)
				(*value)[i] = gconf_value_get_int(entry->data);
			(*value)[i] = '\0';
			break;
		}
		/* fallthrough for error */
	default:
		g_debug("%s():expected 'string' or 'list of int' for key '%s' in gconf", __FUNCTION__, key);
		ret = CONN_SETTINGS_E_DIFFERENT_TYPE;
		break;
	}

	gconf_value_free(backend_value);

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_get_list(ConnSettings *ctx, const char *key,
			   ConnSettingsValue ***value)
{
	int ret = 0;
	ConnSettingsValue *svalue;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	svalue = conn_settings_get(ctx, key);
	if (!svalue) {
		g_debug("%s():value not found\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_VALUE;
	}

	if (svalue->type == CONN_SETTINGS_VALUE_INVALID) {
		ret = CONN_SETTINGS_E_NO_SUCH_KEY;
		conn_settings_value_destroy(svalue);
		goto cleanup;
	}

	if (svalue->type != CONN_SETTINGS_VALUE_LIST) {
		ret = CONN_SETTINGS_E_DIFFERENT_TYPE;
		conn_settings_value_destroy(svalue);
		goto cleanup;
	}

	*value = svalue->value.list_val;
	g_free(svalue); /* We must not free the actual list */

cleanup:
	return ret;
}


static GSList *create_value_list(ConnSettingsValue *value, GSList *list)
{
	int i;
	GConfValueType list_type = GCONF_VALUE_INVALID;
	for (i=0; value->value.list_val[i]; i++) {
		switch (value->value.list_val[i]->type) {
		case CONN_SETTINGS_VALUE_STRING:
			if (list_type == GCONF_VALUE_INVALID)
				list_type = GCONF_VALUE_STRING;
			else if (list_type != GCONF_VALUE_STRING)
				goto fail; /* mixed type lists are not supported */
			list = g_slist_append(list, value->value.list_val[i]->value.string_val);
			break;
		case CONN_SETTINGS_VALUE_INT:
			if (list_type == GCONF_VALUE_INVALID)
				list_type = GCONF_VALUE_INT;
			else if (list_type != GCONF_VALUE_INT)
				goto fail;
			list = g_slist_append(list, GINT_TO_POINTER((long)(value->value.list_val[i]->value.int_val)));
			break;
		case CONN_SETTINGS_VALUE_DOUBLE:
			if (list_type == GCONF_VALUE_INVALID)
				list_type = GCONF_VALUE_FLOAT;
			else if (list_type != GCONF_VALUE_FLOAT)
				goto fail;
			list = g_slist_append(list, &value->value.list_val[i]->value.double_val);
			break;
		case CONN_SETTINGS_VALUE_BOOL:
			if (list_type == GCONF_VALUE_INVALID)
				list_type = GCONF_VALUE_BOOL;
			else if (list_type != GCONF_VALUE_BOOL)
				goto fail;

			list = g_slist_append(list, GINT_TO_POINTER((long)(value->value.list_val[i]->value.bool_val)));
			break;
		default:
			goto fail;
		}
	}
	return list;

fail:
	g_debug("%s():list is invalid\n", __FUNCTION__);
	g_slist_free(list);
	return NULL;
}


static GConfValueType map_to_backend_type(ConnSettingsValueType type, char **type_str)
{
	switch (type) {
	case CONN_SETTINGS_VALUE_INVALID:
		break;
	case CONN_SETTINGS_VALUE_STRING:
		*type_str = "string list";
		return GCONF_VALUE_STRING;
	case CONN_SETTINGS_VALUE_INT:
		*type_str = "int list";
		return GCONF_VALUE_INT;
	case CONN_SETTINGS_VALUE_DOUBLE:
		*type_str = "double list";
		return GCONF_VALUE_FLOAT;
	case CONN_SETTINGS_VALUE_BOOL:
		*type_str = "bool list";
		return GCONF_VALUE_BOOL;
	case CONN_SETTINGS_VALUE_LIST:
		break;
	case CONN_SETTINGS_VALUE_BYTE_ARRAY:
		break;
	default:
		break;
	}

	*type_str = "invalid";
	return GCONF_VALUE_INVALID;
}


int conn_settings_set(ConnSettings *ctx, const char *key, ConnSettingsValue *value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	GConfValueType backend_type;
	int i, ret = 0;
	gboolean gc = TRUE;
	char *type_str = "";
	GSList *list = NULL;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);

	switch (value->type) {
	case CONN_SETTINGS_VALUE_STRING:
		gc = gconf_client_set_string(ctx->gconf_client, path,
					     value->value.string_val, &error);
		type_str = "string";
		break;
	case CONN_SETTINGS_VALUE_INT:
		gc = gconf_client_set_int(ctx->gconf_client, path,
					  value->value.int_val, &error);
		type_str = "int";
		break;
	case CONN_SETTINGS_VALUE_DOUBLE:
		gc = gconf_client_set_float(ctx->gconf_client, path,
					    value->value.double_val, &error);
		type_str = "double";
		break;
	case CONN_SETTINGS_VALUE_BOOL:
		gc = gconf_client_set_bool(ctx->gconf_client, path,
					   value->value.bool_val, &error);
		type_str = "bool";
		break;
	case CONN_SETTINGS_VALUE_BYTE_ARRAY:
		if (value->value.byte_array.val == NULL ||
		    value->value.byte_array.len == 0) {
			ret = CONN_SETTINGS_E_INVALID_VALUE;
			goto cleanup;
		}
		for (i=0; i<value->value.byte_array.len; i++)
			list = g_slist_append(list, GINT_TO_POINTER((long)value->value.byte_array.val[i]));
		gc = gconf_client_set_list(ctx->gconf_client,
					   path,
					   GCONF_VALUE_INT,
					   list,
					   &error);
		type_str = "int list (byte array)";
		g_slist_free(list);
		break;
	case CONN_SETTINGS_VALUE_LIST:
		if (value->value.list_val==NULL || value->value.list_val[0]==NULL) {
			/* We are trying to set an empty list so unset the value
			 * or should we just return error?
			 */
			conn_settings_unset(ctx, key);
			goto cleanup;
		}

		backend_type = map_to_backend_type(value->value.list_val[0]->type, &type_str);
		if (backend_type == GCONF_VALUE_INVALID) {
			ret = CONN_SETTINGS_E_INVALID_TYPE;
			goto cleanup;
		}

		list = create_value_list(value, list);
		if (list == NULL) {
			/* The ConnSettingsValue is incorrectly formulated */
			ret = CONN_SETTINGS_E_INVALID_TYPE;
			goto cleanup;
		}

		gc = gconf_client_set_list(ctx->gconf_client,
					   path,
					   backend_type,
					   list,
					   &error);
		g_slist_free(list);
		break;
	default:
		ret = CONN_SETTINGS_E_INVALID_TYPE;
		goto cleanup;
		break;
	}


	if (!gc) {
		g_error("%s():Cannot set %s (%s) to %s (%s)\n", __FUNCTION__, key, path, type_str, error->message);
	}

	gconf_client_suggest_sync(ctx->gconf_client, NULL);

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_set_string(ConnSettings *ctx, const char *key, char *value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	gboolean st;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		conn_settings_unset(ctx, key);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	st = gconf_client_set_string(ctx->gconf_client,
				     path,
				     value,
				     &error);
	if (st == FALSE) {
		ret = CONN_SETTINGS_E_DIFFERENT_TYPE;
		goto cleanup;
	}
	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

	gconf_client_suggest_sync(ctx->gconf_client, NULL);

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_set_int(ConnSettings *ctx, const char *key, int value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	gboolean st;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	st = gconf_client_set_int(ctx->gconf_client,
				  path,
				  value,
				  &error);
	if (st == FALSE) {
		ret = CONN_SETTINGS_E_DIFFERENT_TYPE;
		goto cleanup;
	}
	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

	gconf_client_suggest_sync(ctx->gconf_client, NULL);

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_set_double(ConnSettings *ctx, const char *key, double value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	gboolean st;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	st = gconf_client_set_float(ctx->gconf_client,
				    path,
				    value,
				    &error);
	if (st == FALSE) {
		ret = CONN_SETTINGS_E_DIFFERENT_TYPE;
		goto cleanup;
	}
	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

	gconf_client_suggest_sync(ctx->gconf_client, NULL);

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_set_bool(ConnSettings *ctx, const char *key, int value)
{
	GError *error = NULL;
	gchar *path;
	gchar *escaped_key;
	int ret = 0;
	gboolean st;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);
	st = gconf_client_set_bool(ctx->gconf_client,
				   path,
				   value,
				   &error);
	if (st == FALSE) {
		ret = CONN_SETTINGS_E_DIFFERENT_TYPE;
		goto cleanup;
	}
	if (conn_settings_gconf_check_error(&error) < 0) {
		ret = CONN_SETTINGS_E_GCONF_ERROR;
		goto cleanup;
	}

	gconf_client_suggest_sync(ctx->gconf_client, NULL);

cleanup:
	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_set_byte_array(ConnSettings *ctx, const char *key,
				 unsigned char *value, unsigned int value_len)
{
	ConnSettingsValue svalue;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value) {
		g_debug("%s():value not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if (!value_len) {
		g_debug("%s():value length not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	svalue.type = CONN_SETTINGS_VALUE_BYTE_ARRAY;
	svalue.value.byte_array.len = value_len;
	svalue.value.byte_array.val = value;
	return conn_settings_set(ctx, key, &svalue);
}


int conn_settings_set_list(ConnSettings *ctx, const char *key, void *list,
			   unsigned int list_len, ConnSettingsValueType elem_type)
{
	int ret = 0, i;
	char *type_str;
	ConnSettingsValue *value, **elements;
	GConfValueType backend_type;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	if ((backend_type = map_to_backend_type(elem_type, &type_str)) == GCONF_VALUE_INVALID) {
		return CONN_SETTINGS_E_INVALID_TYPE;
	}

	/* If list is empty, we should set empty list in backend instead of
	 * giving error. For empty string list, user can supply { "", NULL }
	 * but that does not work for number list so we need to check that
	 * separately.
	 */
	if (!list || list_len == 0) {
		gboolean gc;
		gchar *path;
		gchar *escaped_key;
		GError *error = NULL;
		int st = 0;

		escaped_key = escape_key(key);
		path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);

		gc = gconf_client_set_list(ctx->gconf_client,
					   path,
					   backend_type,
					   NULL,
					   &error);
		if (!gc) {
			g_error("%s():Cannot set empty list %s (%s) (%s)\n", __FUNCTION__, key, path, error->message);
			st = CONN_SETTINGS_E_INVALID_VALUE;
		}
		g_free(escaped_key);
		g_free(path);
		g_clear_error(&error);

		gconf_client_suggest_sync(ctx->gconf_client, NULL);
		return st;
	}

	value = conn_settings_create_value(CONN_SETTINGS_VALUE_LIST);
	elements = (ConnSettingsValue **)g_malloc0(sizeof(void *) * (list_len + 1));

	for (i=0; i<list_len; i++) {
		elements[i] = conn_settings_create_value(elem_type);

		switch (elem_type) {
		case CONN_SETTINGS_VALUE_STRING:
			elements[i]->value.string_val = g_strdup(((char **)list)[i]);
			break;
		case CONN_SETTINGS_VALUE_INT:
		case CONN_SETTINGS_VALUE_BOOL:
			elements[i]->value.int_val = ((int *)list)[i];
			break;
		case CONN_SETTINGS_VALUE_DOUBLE:
			elements[i]->value.double_val = ((double *)list)[i];
			break;
		default:
			/* should not happen */
			ret = CONN_SETTINGS_E_INVALID_TYPE;
			goto cleanup;
			break;
		}
	}

	value->value.list_val = elements;
	ret = conn_settings_set(ctx, key, value);
	conn_settings_value_destroy(value);
	return ret;

cleanup:
	for (i=0; i<list_len; i++) {
		conn_settings_value_destroy(elements[i]);
	}
	g_free(elements);
	conn_settings_value_destroy(value);
	return ret;
}


int conn_settings_unset(ConnSettings *ctx, const char *key)
{
	gchar *path;
	gchar *escaped_key;
	GError *error = NULL;
	int ret = 0;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!key) {
		g_debug("%s():key not set\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_PARAMETER;
	}

	escaped_key = escape_key(key);
	path = g_strdup_printf("%s/%s", ctx->dir, escaped_key);

	if (!gconf_client_unset(ctx->gconf_client, path, &error)) {
		conn_settings_gconf_check_error(&error);
		ret = CONN_SETTINGS_E_GCONF_ERROR;
	}

	g_free(escaped_key);
	g_free(path);
	g_clear_error(&error);

	return ret;
}


int conn_settings_remove(ConnSettings *ctx)
{
	GError *error = NULL;
	int ret = 0;

	if (!ctx)
		return CONN_SETTINGS_E_INVALID_CONTEXT;

	if (!ctx->dir) {
		g_debug("%s():context is not valid\n", __FUNCTION__);
		return CONN_SETTINGS_E_INVALID_CONTEXT;
	}

	if (!gconf_client_recursive_unset(ctx->gconf_client, ctx->dir,
					  0, &error)) {
		conn_settings_gconf_check_error(&error);
		ret = CONN_SETTINGS_E_GCONF_ERROR;
	}

	return ret;
}


void conn_settings_close(ConnSettings *ctx)
{
	if (!ctx)
		return;

	if (ctx->notifier)
		conn_settings_del_notify(ctx);

	g_free(ctx->id);
	g_free(ctx->escaped_id);
	g_free(ctx->dir);

	ctx->id = NULL;
	ctx->escaped_id = NULL;
	ctx->dir = NULL;

	g_object_unref(ctx->gconf_client);

	g_free(ctx);
}


const char *conn_settings_error_text(ConnSettingsError code)
{
	switch (code) {
	case CONN_SETTINGS_E_INVALID_CONTEXT:
		return "Context is not set";
	case CONN_SETTINGS_E_DIFFERENT_TYPE:
		return "Backend has different type for a given key";
	case CONN_SETTINGS_E_GCONF_ERROR:
		return "GConf error";
	case CONN_SETTINGS_E_INVALID_PARAMETER:
		return "Parameter not set or invalid";
	case CONN_SETTINGS_E_NO_SUCH_KEY:
		return "Key not found in storage";
	case CONN_SETTINGS_E_INVALID_TYPE:
		return "Invalid type, backend does not support such a type";
	case CONN_SETTINGS_E_INVALID_VALUE:
		return "Value is invalid (either null or otherwise invalid)";
	case CONN_SETTINGS_E_NO_ERROR:
		return "";
	default:
		break;
	}
	return "Unknown error";
}




static void _gconf_notify(GConfClient *gconf_client,
			  guint connection_id,
			  GConfEntry *entry,
			  gpointer user_data)
{
	ConnSettingsNotifier *notifier = (ConnSettingsNotifier *)user_data;
	int head_len;
	const gchar *key;
	const gchar *tail = NULL;
	const gchar *slash = NULL;
	ConnSettingsValue *value = NULL;
	GConfValue *backend_value;

	if (!notifier)
		return;

	key = gconf_entry_get_key(entry);
	head_len = strlen(notifier->dir);

	//g_debug("%s(): key=%s, head_len=%d\n", __FUNCTION__, key, head_len);

	if(strncmp(key, notifier->dir, head_len)) {
		/* Not our branch, just ignore */
		return;
	}

	tail = &(key[head_len]);
	slash = strchr(tail, '/');
	if (slash && (tail == slash))
		tail++; /* skip the last '/' */

	backend_value = gconf_entry_get_value(entry);
	if (backend_value)
		value = create_value_from_backend(backend_value);

#if 0
	g_debug("%s(): func = %p\n", __FUNCTION__, notifier->func);
	g_debug("%s(): type = %d\n", __FUNCTION__, notifier->type);
	g_debug("%s(): tail = %s\n", __FUNCTION__, tail);
	g_debug("%s(): value = %p\n", __FUNCTION__, value);
	g_debug("%s(): user_data = %p\n", __FUNCTION__, notifier->user_data);
#endif

	notifier->func(notifier->type, notifier->id, tail, value, notifier->user_data);
	conn_settings_value_destroy(value);
	return;
}



int conn_settings_add_notify(ConnSettings *ctx,
			     ConnSettingsNotifyFunc *func,
			     void *user_data)
{
	GError *error = NULL;

	if (!ctx)
		return FALSE;

	if (ctx->notifier) {
		g_error("%s(): notify function already defined, please remove earlier notifier first.\n", __FUNCTION__);
		return FALSE;
	}

	ctx->notifier = g_malloc0(sizeof(ConnSettingsNotifier));
	ctx->notifier->func = (ConnSettingsNotifyFunc)func;
	ctx->notifier->user_data = user_data;
	ctx->notifier->type = ctx->type;
	ctx->notifier->dir = g_strdup(ctx->dir);
	ctx->notifier->id = g_strdup(ctx->id);

	gconf_client_add_dir(ctx->gconf_client,
			     ctx->dir,
			     GCONF_CLIENT_PRELOAD_ONELEVEL,
			     &error);
	conn_settings_gconf_check_error(&error);

	ctx->notifier->notify_id = gconf_client_notify_add(ctx->gconf_client,
							   ctx->dir,
							   _gconf_notify,
							   ctx->notifier,
							   NULL,
							   &error);
	if (!ctx->notifier->notify_id) {
		conn_settings_gconf_check_error(&error);
		g_free(ctx->notifier->id);
		g_free(ctx->notifier->dir);
		g_free(ctx->notifier);
		ctx->notifier = NULL;
	}
	g_clear_error(&error);

	return ctx->notifier != NULL;
}


void conn_settings_del_notify(ConnSettings *ctx)
{
	GError *error = NULL;

	if (!ctx)
		return;
	if (!ctx->notifier)
		return;
	if (!ctx->notifier->notify_id)
		return;

	gconf_client_notify_remove(ctx->gconf_client,
				   ctx->notifier->notify_id);

	gconf_client_remove_dir(ctx->gconf_client,
				ctx->dir,
				&error);
	conn_settings_gconf_check_error(&error);

	ctx->notifier->notify_id = 0;
	g_free(ctx->notifier->id);
	g_free(ctx->notifier->dir);
	g_free(ctx->notifier);
	ctx->notifier = NULL;
}


int conn_settings_id_exists(ConnSettingsType type, char *id)
{
	GConfClient *gconf;
	char *escaped, *dir, *path;
	int ret;

	if (!id)
		return FALSE;
  
	switch (type) {
	case CONN_SETTINGS_GENERAL:
		dir = ICD_GCONF_SETTINGS;
		break;
	case CONN_SETTINGS_NETWORK_TYPE:
		dir = ICD_GCONF_NETWORK_MAPPING;
		break;
	case CONN_SETTINGS_CONNECTION:
		dir = ICD_GCONF_PATH;
		break;
	case CONN_SETTINGS_SERVICE_TYPE:
		dir = ICD_GCONF_SRV_PROVIDERS;
		break;
	default:
		return FALSE;
	}

	g_type_init();

	gconf = gconf_client_get_default();
	escaped = conn_settings_escape_string(id);
	path = g_strconcat (dir, "/", escaped, NULL);
	g_free (escaped);

	ret = gconf_client_dir_exists(gconf, path, NULL);

	g_free(path);
	g_object_unref(gconf);

	return ret;
}


