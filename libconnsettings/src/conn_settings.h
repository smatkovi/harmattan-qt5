/**
  @file conn_settings.h
  @author Aapo Makela <aapo.makela@nokia.com>
  @author Jukka Rissanen <jukka.rissanen@nokia.com>
    
  libconnsettings - Connectivity settings library.

  Copyright (C) 2010 Nokia Corporation. All rights reserved.
    
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

#ifndef CONN_SETTINGS_H
#define CONN_SETTINGS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
@file conn_settings.h

@addtogroup conn_settings_api Connectivity settings API

Connectivity settings API provides settings from various backends (currently
 gconf is supported).
Depending on ConnSettingsType following items will be returned or set in gconf by various API functions
<table>
<tr><td>CONN_SETTINGS_GENERAL</td><td>Generic connectivity settings from /system/osso/connectivity</td></tr>
<tr><td>CONN_SETTINGS_NETWORK_TYPE</td><td>Network type settings from /system/osso/connectivity/network_type, the id is the network type like WLAN or GPRS</td></tr>
<tr><td>CONN_SETTINGS_CONNECTION</td><td>Connection settings from /system/osso/connectivity/IAP, the id is the IAP id</td></tr>
<tr><td>CONN_SETTINGS_SERVICE_TYPE</td><td>Service type settings from /system/osso/connectivity/srv_provider, the id is the provider identifier</td></tr>
</table>
<p>
Example of usage:
<p>
Return IAP ids
<pre>
	char **ids;
	int i;

	ids = conn_settings_list_ids(CONN_SETTINGS_CONNECTION);
	if (ids) {
	        for (i=0; ids[i]!=NULL; i++) {
		        printf("IAP: %s\n", ids[i]);
			free(ids[i]);
		}
		free(ids);
        }
</pre>
<br>
<p>
Check if given IAP exists
<pre>
        if (conn_settings_id_exists(CONN_SETTINGS_CONNECTION, "known-IAP"))
	        printf("IAP exists\n");
	else
	        printf("IAP is not found\n");
</pre>
<br>
<p>
Return keys for a IAP id 89ebd4e9-2e86-4fda-bd15-1cdcd98e581e
<pre>
	ConnSettings *ctx;
	char **keys;
	int i;

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	keys = conn_settings_list_keys(ctx);
	if (keys) {
	        for (i=0; keys[i]!=NULL; i++) {
		        printf("%s\n", keys[i]);
			free(keys[i]);
		}
		free(keys);
	}
	conn_settings_close(ctx);
</pre>
<br>
<p>
Return GPRS RX byte count
<pre>
	ConnSettings *ctx;
	ConnSettingsValue *value;
	int count = 0, ret;

	ctx = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "GPRS");
	value = conn_settings_get(ctx, "gprs_rx_bytes");
	if (value && value->type == CONN_SETTINGS_VALUE_INT)
		printf("GPRS RX byte count = %d\n", value->value.int_val);
	conn_settings_value_destroy(value);

	// or the count can be fetched using other function
	ret = conn_settings_get_int(ctx, "gprs_rx_bytes", &count);
	if (!ret)
		printf("GPRS RX byte count = %d\n", count);

	conn_settings_close(ctx);
</pre>
<br>
<p>
Return WLAN ssid
<pre>
	ConnSettings *ctx;
	int ret;
	unsigned char *ssid = 0;
	unsigned int len = 0;

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	ret = conn_settings_get_byte_array(ctx, "wlan_ssid", &ssid, &len);
	if (ret == CONN_SETTINGS_E_NO_ERROR) {
		int i;
		if (len>0) {
			printf("wlan_ssid: ");
			for (i=0; i<len; i++)
				printf("%c", ssid[i]);
			printf("\n");
			free(ssid);
		}
	}
	conn_settings_close(ctx);
</pre>
<br>
<p>
Remove IAP
<pre>
	ConnSettings *ctx;
	int ret;

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	ret = conn_settings_remove(ctx);
	if (ret != CONN_SETTINGS_E_NO_ERROR)
		printf("Cannot remove IAP\n");

	conn_settings_close(ctx);
</pre>
<br>
<p>
Set/get empty list
<pre>
	ConnSettings *ctx;
	int ret;
	ConnSettingsValue **value = NULL;

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	ret = conn_settings_set_list(ctx, "empty_list", NULL, 0, CONN_SETTINGS_VALUE_INT);
	if (ret != CONN_SETTINGS_E_NO_ERROR)
		printf("Cannot set empty list\n");
	else {
	        ret = conn_settings_get_list(ctx, "empty_list", &value);
	        if (ret != CONN_SETTINGS_E_NO_ERROR)
		        printf("Cannot get empty list\n");
		else if (*value == NULL)
		        printf("List is empty\n");
        }

	conn_settings_close(ctx);
</pre>
<br>
<p>
Set/get int list using various API functions
<pre>
	ConnSettings *ctx;
	int ret, i;
	int int_list[] = { 'T','E','S','T' };
	int len = sizeof(int_list) / sizeof(int);
	ConnSettingsValue **value = NULL, *new_value;

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	ret = conn_settings_set_list(ctx, "int_list", int_list, len, CONN_SETTINGS_VALUE_INT);
	if (ret != CONN_SETTINGS_E_NO_ERROR)
		printf("Cannot set list\n");
	else {
	        ret = conn_settings_get_list(ctx, "int_list", &value);
	        if (ret != CONN_SETTINGS_E_NO_ERROR)
		        printf("Cannot get empty list\n");
		else if (*value == NULL)
		        printf("List is empty\n");
		else if ((*value)->type != CONN_SETTINGS_VALUE_INT)
		        printf("Invalid list type\n");
		else {
		        for (i=0; value[i]!=NULL; i++) {
				printf("[%d] = %c (%d)\n", i, value[i]->value.int_val, value[i]->value.int_val);
			}

			// change the list and put it back
			value[0]->value.int_val = 82;
			printf("\n");
			new_value = conn_settings_value_new();
			new_value->type = CONN_SETTINGS_VALUE_LIST;
			new_value->value.list_val = value;

			ret = conn_settings_set(ctx, "int_list", new_value);

			// this also clears the list values
			conn_settings_value_destroy(new_value);

			if (ret == CONN_SETTINGS_E_NO_ERROR) {
			        ret = conn_settings_get_list(ctx, "int_list", &value);
				for (i=0; value[i]!=NULL; i++) {
				        printf("[%d] = %c (%d)\n", i, value[i]->value.int_val, value[i]->value.int_val);
				}

				conn_settings_values_destroy(value);
			}
		}
        }

	conn_settings_close(ctx);
</pre>
*/

/** Error values for the library
 */
typedef enum ConnSettingsError_ {
	/** Context is not set */
	CONN_SETTINGS_E_INVALID_CONTEXT=-1,
	/** Backend has different type for a given key */
	CONN_SETTINGS_E_DIFFERENT_TYPE=-2,
	/** GConf error */
	CONN_SETTINGS_E_GCONF_ERROR=-3,
	/** Parameter not set */
	CONN_SETTINGS_E_INVALID_PARAMETER=-4,
	/** Key not found in store */
	CONN_SETTINGS_E_NO_SUCH_KEY=-5,
	/** Invalid type, backend does not support such a type */
	CONN_SETTINGS_E_INVALID_TYPE=-6,
	/** Value is invalid (either null or otherwise invalid) */
	CONN_SETTINGS_E_INVALID_VALUE=-7,

	/** Last entry, no error */
	CONN_SETTINGS_E_NO_ERROR=0
} ConnSettingsError;


/** ConnSettings context. This contains library internal data.
 */
typedef struct ConnSettings_ ConnSettings;

/** Type of ConnSettings context
 */
typedef enum ConnSettingsType_ {
	/** Invalid settings (this is an error condition) */
	CONN_SETTINGS_INVALID,
	/** General connectivity settings */
	CONN_SETTINGS_GENERAL,
	/** Network type settings */
	CONN_SETTINGS_NETWORK_TYPE,
	/** Connection settings */
	CONN_SETTINGS_CONNECTION,
	/** Service type settings */
	CONN_SETTINGS_SERVICE_TYPE
} ConnSettingsType;

/** ConnSettings value type
 */
typedef enum ConnSettingsValueType_  {
	/** Invalid value */
	CONN_SETTINGS_VALUE_INVALID,
	/** String value */
	CONN_SETTINGS_VALUE_STRING,
	/** Integer value */
	CONN_SETTINGS_VALUE_INT,
	/** Double value */
	CONN_SETTINGS_VALUE_DOUBLE,
	/** Boolean value */
	CONN_SETTINGS_VALUE_BOOL,
	/** List of ConnSettingsValue value */
	CONN_SETTINGS_VALUE_LIST,
	/** Byte array */
	CONN_SETTINGS_VALUE_BYTE_ARRAY
} ConnSettingsValueType;

typedef struct ConnSettingsValueData_ ConnSettingsValueData;

/** ConnSettings value. When filling the value union, you must
 * allocate new space for string_val, byte_array.val and list_val
 * as the conn_settings_value_destroy() will deallocate that
 * memory.
 */
typedef struct ConnSettingsValue_ {
	ConnSettingsValueType type;

	union {
		char *string_val;
		int int_val;
		double double_val;
		int bool_val;
		struct {
			unsigned char *val;
			unsigned int len;
		} byte_array;
		struct ConnSettingsValue_ **list_val;
	} value;
    
	ConnSettingsValueData *data;
} ConnSettingsValue;


/** Allocate new settings value struct
 */
ConnSettingsValue *conn_settings_value_new(void);

/** Deallocate settings value struct. Note that string_val, byte_array.val and
 * list_val pointers are also freed (depeding on value type).
 * This means that user should allocate space for those when filling up the
 * struct value union.
 * @param value Pointer to settings value struct to be deallocated.
 */
void conn_settings_value_destroy(ConnSettingsValue *value);

/** Deallocate array of settings value structs. Note that string_val, byte_array.val and
 * list_val pointers are also freed (depeding on value type).
 * This means that user should allocate space for those when filling up the
 * struct value union. This function can be used to deallocate
 * values returned by conn_settings_get_list()
 * @param value Pointer to array of settings value structs to be deallocated.
 */
void conn_settings_values_destroy(ConnSettingsValue **values);

/** List available Ids for a connectivity setting type.
 * For gconf backend, this function returns corresponding
 * directory names from gconf.
 * @param type Setting type.
 * @return A new list of pointers with last element set to NULL or NULL if none found.
 * User should  deallocate it using g_strfreev().
*/
char **conn_settings_list_ids(ConnSettingsType type);

/** Open connectivity settings for given type and ID
 * @param type What settings to manipulate
 * @param id Id of the setting (typically a gconf directory i.e., for connectivity type id is the IAP id
 * @return ConnSettings or NULL if invalid parameters were given
*/
ConnSettings *conn_settings_open(ConnSettingsType type, const char *id);

/** List available keys for a given connectivity settings.
 * For gconf backend this returns keys in a defined directory
 * @param ctx Settings context
 * @return A new list of pointers with last element set to NULL or NULL if none found.
 * User should  deallocate it using g_strfreev().
*/
char **conn_settings_list_keys(ConnSettings *ctx);

/** Get value for a given key.
 * @param ctx Settings context
 * @param key Settings name
 * @return ConnSettingsValue or NULL if value is not found
 */
ConnSettingsValue *conn_settings_get(ConnSettings *ctx, const char *key);

/** Get string value of a given key
 * @param ctx Settings context
 * @param key Settings key
 * @param value Pointer to value is returned. User should deallocate the value after use using g_free().
 * @return ConnSettingsError return code
 */
int conn_settings_get_string(ConnSettings *ctx, const char *key, char **value);

/** Get int value of a given key
 * @param ctx Settings context
 * @param key Settings key
 * @param value Value is returned.
 * @return ConnSettingsError return code
 */
int conn_settings_get_int(ConnSettings *ctx, const char *key, int *value);

/** Get double value of a given key
 * @param ctx Settings context
 * @param key Settings key
 * @param value Value is returned.
 * @return ConnSettingsError return code
 */
int conn_settings_get_double(ConnSettings *ctx, const char *key, double *value);

/** Get bool value of a given key
 * @param ctx Settings context
 * @param key Settings key
 * @param value Value is returned.
 * @return ConnSettingsError return code
 */
int conn_settings_get_bool(ConnSettings *ctx, const char *key, int *value);

/** Get list value of a given key
 * @param ctx Settings context
 * @param key Settings key
 * @param value Value is returned. User is responsible for deallocating the returned value. Note that the value is array of pointers to list values, it is not a list itself
 * @return ConnSettingsError return code
 */
int conn_settings_get_list(ConnSettings *ctx, const char *key, ConnSettingsValue ***value);

/** Get byte array value of a given key
 * @param ctx Settings context
 * @param key Settings key
 * @param value Value is returned. User is responsible for deallocating the returned value.
 * @param value_len Length of the returned value
 * @return ConnSettingsError return code
 */
int conn_settings_get_byte_array(ConnSettings *ctx, const char *key, unsigned char **value, unsigned int *value_len);

/** Set value for a given key.
 * @param ctx Settings context
 * @param key Settings key
 * @param value Settings value to set
 * @return ConnSettingsError return code
 */
int conn_settings_set(ConnSettings *ctx, const char *key, ConnSettingsValue *value);

/** Set string value for a given key.
 * @param ctx Settings context
 * @param key Settings key
 * @param value String value to set
 * @return ConnSettingsError return code
 */
int conn_settings_set_string(ConnSettings *ctx, const char *key, char *value);

/** Set int value for a given key.
 * @param ctx Settings context
 * @param key Settings key
 * @param value Integer value to set
 * @return ConnSettingsError return code
 */
int conn_settings_set_int(ConnSettings *ctx, const char *key, int value);

/** Set double value for a given key.
 * @param ctx Settings context
 * @param key Settings key
 * @param value Double value to set
 * @return ConnSettingsError return code
 */
int conn_settings_set_double(ConnSettings *ctx, const char *key, double value);

/** Set boolean value for a given key.
 * @param ctx Settings context
 * @param key Settings key
 * @param value Boolean value to set
 * @return ConnSettingsError return code
 */
int conn_settings_set_bool(ConnSettings *ctx, const char *key, int value);

/** Set list value for a given key.
 * @param ctx Settings context
 * @param key Settings key
 * @param list List value to set. The list is an array of elements (the elements are not pointers)
 * @param list_len Length of the list (how many elements there is in the list array)
 * @param elem_type Type of the list element
 * @return ConnSettingsError return code
 */
int conn_settings_set_list(ConnSettings *ctx, const char *key, void *list, unsigned int list_len, ConnSettingsValueType elem_type);

/** Set byte array value for a given key.
 * @param ctx Settings context
 * @param key Settings key
 * @param value Byte array value to set
 * @param value_len Length of the byte array
 * @return ConnSettingsError return code
 */
int conn_settings_set_byte_array(ConnSettings *ctx, const char *key, unsigned char *value, unsigned int value_len);

/** Unset / remove key. For gconf backend this removes one individual key.
 * @param ctx Settings context
 * @param key Settings key
 * @return ConnSettingsError return code
 */
int conn_settings_unset(ConnSettings *ctx, const char *key);

/** Remove current settings. For gconf backend this removes a directory tree.
 * @param ctx Settings context
 * @return ConnSettingsError return code
 */
int conn_settings_remove(ConnSettings *ctx);

/** Close connectivity settings context. Deallocates the context struct.
 * @param ctx Settings context
 */
void conn_settings_close(ConnSettings *ctx);

/** Return error text to an error code.
 * @param code Error code
 * @return Error string
 */
const char *conn_settings_error_text(ConnSettingsError code);


/** This function will be called if configuration setting is modified or removed in backend. Not all backends support this. Note that the value is freed by the library.
 */
typedef void (*ConnSettingsNotifyFunc) (ConnSettingsType type,
					const char *id,
					const char *key,
					ConnSettingsValue *value,
					void *user_data);

/** Set notification func for settings changes
 * @param ctx Settings context
 * @param func Notification function to be called
 * @param user_data Opaque user data
 * @return TRUE on success, FALSE on failure
 */
int conn_settings_add_notify(ConnSettings *ctx,
			     ConnSettingsNotifyFunc *func,
			     void *user_data);

/** Remove notification func for settings changes
 * @param ctx Settings context
 */
void conn_settings_del_notify(ConnSettings *ctx);

/** Escape a string
 * @param str String to be escaped
 * @return Escaped string, caller should de-allocate the string
 */
char *conn_settings_escape_string(const char *str);


/** Check if the id exists.
 * @param type Setting type.
 * @param id Id name (directory in gconf)
 * @return TRUE if id exists, false otherwise
*/
int conn_settings_id_exists(ConnSettingsType type, char *id);


#ifdef __cplusplus
}
#endif
  
#endif /* CONN_SETTINGS_H */
