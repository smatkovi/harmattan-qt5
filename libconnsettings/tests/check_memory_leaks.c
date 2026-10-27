/* Do various operations and try to find out memory leaks.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>

#include "../src/conn_settings.h"

int notify_int_list[] = { 'A','P','U','V','A' };
int notify_int_list_len = sizeof(notify_int_list) / sizeof(int);
gboolean iap_removed = FALSE;
ConnSettings *notif_ctx = NULL;
gboolean notify_received = FALSE;
GMainLoop* mainloop = NULL;


void notify_func(ConnSettingsType type,
		 const char *id,
		 const char *key,
		 ConnSettingsValue *value,
		 void *user_data)
{
	g_debug("%s(): type=%d, id=%s, key=%s, value=%p, user_data=%p\n", __FUNCTION__, (int)type, id, key, value, user_data);
	if (!value && key && !strstr(key, "/"))
		iap_removed = TRUE;
	notify_received = TRUE;
}


gboolean func_idle(gpointer data)
{
	static ConnSettings *ctx = NULL;
	int st = 0;
	static int state = 0;
	ConnSettingsValue **value;

	if (state == 0) {
		ctx = (ConnSettings *)data;
		st = conn_settings_set_list(ctx, "int_list_test", notify_int_list, notify_int_list_len, CONN_SETTINGS_VALUE_INT);
		state = 1;
		return TRUE;
	} else if (state == 1) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		st = conn_settings_get_list(ctx, "int_list_test", &value);
		if (!st) {
			conn_settings_values_destroy(value);
		}
		st = conn_settings_remove(ctx);
		state = 2;
		return TRUE;
	} else if (state == 2) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		st = conn_settings_get_list(ctx, "int_list_test", &value);
		conn_settings_del_notify(ctx);
		conn_settings_close(ctx);
		conn_settings_close(notif_ctx);
		g_main_quit(mainloop);
	}

	return FALSE;
}


gboolean func_once(gpointer data)
{
	ConnSettings *ctx;

	notif_ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, NULL);
	conn_settings_add_notify(notif_ctx, (ConnSettingsNotifyFunc *)notify_func, NULL);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	g_idle_add(func_idle, (gpointer)ctx);
	return FALSE;
}


int main()
{
	ConnSettings *ctx;
	int ret, i;
	int int_list[] = { 'T','E','S','T' };
	int len = sizeof(int_list) / sizeof(int);
	ConnSettingsValue **value = NULL, *new_value;
	char **keys;
	unsigned char *ssid = 0;
	unsigned int len2 = 0;
	unsigned char ba[] = { 'T', 'e', 'S', 't' };
	unsigned int ba_len = 4;
	int int_value;
	char *string_value;
	double double_value;
	int bool_value;

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
			conn_settings_value_destroy(new_value);

			if (ret == CONN_SETTINGS_E_NO_ERROR) {
			        ret = conn_settings_get_list(ctx, "int_list", &value);
				for (i=0; value[i]!=NULL; i++) {
				        printf("[%d] = %c (%d)\n", i, value[i]->value.int_val, value[i]->value.int_val);
				}
			}
		}
        }

	conn_settings_values_destroy(value);

	ret = conn_settings_set_list(ctx, "int_list_empty", NULL, 0, CONN_SETTINGS_VALUE_INT);
	if (ret != CONN_SETTINGS_E_NO_ERROR)
		printf("Cannot set int list\n");
	else {
		value = NULL;
	        ret = conn_settings_get_list(ctx, "int_list_empty", &value);
	        if (ret != CONN_SETTINGS_E_NO_ERROR)
		        printf("Cannot get empty list\n");
		else if (*value == NULL)
		        printf("List is empty\n");
		else if ((*value)->type != CONN_SETTINGS_VALUE_INT)
		        printf("Invalid list type\n");
        }
	conn_settings_values_destroy(value);

	ret = conn_settings_set_list(ctx, "string_list_empty", NULL, 0, CONN_SETTINGS_VALUE_STRING);
	if (ret != CONN_SETTINGS_E_NO_ERROR)
		printf("Cannot set string list\n");
	else {
		value = NULL;
	        ret = conn_settings_get_list(ctx, "string_list_empty", &value);
	        if (ret != CONN_SETTINGS_E_NO_ERROR)
		        printf("Cannot get empty string list\n");
		else if (*value == NULL)
		        printf("String list is empty\n");
		else if ((*value)->type != CONN_SETTINGS_VALUE_STRING)
		        printf("Invalid list type (%d)\n", (*value)->type);
        }
	conn_settings_values_destroy(value);

	value = NULL;
	ret = conn_settings_get_list(ctx, "int_list_not_found", &value);
	if (ret != CONN_SETTINGS_E_NO_ERROR)
		printf("Cannot get empty list\n");
	conn_settings_values_destroy(value);

	conn_settings_close(ctx);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	conn_settings_set_int(ctx, "int_value2", 1000);
	conn_settings_set_int(ctx, "int_value3", 1000);
	conn_settings_close(ctx);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	keys = conn_settings_list_keys(ctx);
	for (i=0; keys[i]!=NULL; i++) {
	    printf("%s\n", keys[i]);
	    free(keys[i]);
	}
	free(keys);
	conn_settings_close(ctx);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	conn_settings_set_int(ctx, "int_value", 1000);
	conn_settings_set_string(ctx, "string_value", "foobar");
	conn_settings_set_double(ctx, "double_value", 10.0);
	conn_settings_set_bool(ctx, "bool_value", 1);
	conn_settings_set_byte_array(ctx, "byte_array_value", ba, ba_len);
	conn_settings_close(ctx);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	conn_settings_get_int(ctx, "int_value", &int_value);
	conn_settings_get_string(ctx, "string_value", &string_value);
	g_free(string_value);
	conn_settings_get_double(ctx, "double_value", &double_value);
	conn_settings_get_bool(ctx, "bool_value", &bool_value);
	conn_settings_get_byte_array(ctx, "byte_array_value", &ssid, &len2);
	g_free(ssid);
	new_value = conn_settings_get(ctx, "int_value");
	conn_settings_value_destroy(new_value);
	new_value = conn_settings_get(ctx, "string_value");
	conn_settings_value_destroy(new_value);
	new_value = conn_settings_get(ctx, "double_value");
	conn_settings_value_destroy(new_value);
	new_value = conn_settings_get(ctx, "bool_value");
	conn_settings_value_destroy(new_value);
	new_value = conn_settings_get(ctx, "byte_array_value");
	conn_settings_value_destroy(new_value);
	conn_settings_close(ctx);

	keys = conn_settings_list_ids(CONN_SETTINGS_CONNECTION);
	for (i=0; keys[i]!=NULL; i++) {
	    printf("Connection: %s\n", keys[i]);
	    free(keys[i]);
	}
	free(keys);

	if (conn_settings_id_exists(CONN_SETTINGS_CONNECTION, "not-found"))
		printf("ERROR: id found\n");
	if (!conn_settings_id_exists(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e"))
		printf("ERROR: id not found\n");
	if (conn_settings_id_exists(CONN_SETTINGS_NETWORK_TYPE, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e"))
		printf("ERROR: id found\n");

	keys = conn_settings_list_ids(CONN_SETTINGS_GENERAL);
	for (i=0; keys[i]!=NULL; i++) {
	    printf("General: %s\n", keys[i]);
	    free(keys[i]);
	}
	free(keys);

	ctx = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "GPRS");
	conn_settings_set_int(ctx, "gprs_rx_bytes", 1000);
	new_value = conn_settings_get(ctx, "gprs_rx_bytes");
	if (new_value && new_value->type == CONN_SETTINGS_VALUE_INT)
		printf("GPRS RX byte count = %d\n", new_value->value.int_val);
	conn_settings_value_destroy(new_value);
	ret = conn_settings_get_int(ctx, "gprs_rx_bytes", &i);
	if (!ret)
		printf("GPRS RX byte count = %d\n", i);
	conn_settings_unset(ctx, "gprs_rx_bytes");
	conn_settings_close(ctx);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	ret = conn_settings_get_byte_array(ctx, "wlan_ssid", &ssid, &len2);
	if (ret == CONN_SETTINGS_E_NO_ERROR) {
		if (len>0) {
			printf("wlan_ssid: ");
			for (i=0; i<len; i++)
				printf("%c", ssid[i]);
			printf("\n");
			free(ssid);
		}
	}
	conn_settings_close(ctx);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	conn_settings_unset(ctx, "int_value");
	conn_settings_unset(ctx, "string_value");
	conn_settings_unset(ctx, "double_value");
	conn_settings_unset(ctx, "bool_value");
	conn_settings_unset(ctx, "byte_array_value");
	conn_settings_close(ctx);

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
	conn_settings_values_destroy(value);
	conn_settings_close(ctx);

	ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, "89ebd4e9-2e86-4fda-bd15-1cdcd98e581e");
	conn_settings_remove(ctx);
	conn_settings_close(ctx);

	/* We need mainloop for checking notification support */
	mainloop = g_main_loop_new(NULL, FALSE);
	if (mainloop == NULL) {
		g_error("%s(): Failed to create mainloop!\n", __FUNCTION__);
	} else {
		g_idle_add(func_once, NULL);
		g_main_loop_run(mainloop);
	}

	exit(notify_received);
}
