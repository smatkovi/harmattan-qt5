/*
  Tests for connectivity settings module using check tool

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

#include <stdlib.h>
#include <check.h>

#include <limits.h>
#include <float.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <glib-object.h>
#include <glib.h>

#include <gconf/gconf-client.h>
#include <osso-ic.h>
#include <osso-ic-gconf.h>

#include "../src/conn_settings.h"

void setup(void);
void teardown(void);
void setup_empty(void);
void teardown_empty(void);
void setup_db(void);
void teardown_db(void);
Suite *conn_settings_suite(void);

gboolean is_ok;
int num_of_expected_general_entries = 0;
int num_of_expected_network_entries = 0;
int num_of_expected_network_gprs_entries = 0;
int num_of_expected_network_wlan_entries = 0;
int num_of_expected_conn_entries = 0;
int num_of_expected_service_entries = 0;
int num_of_expected_conn_iap_entries = 0;

int num_of_expected_general_dirs = 0;
int num_of_expected_network_dirs = 0;
int num_of_expected_conn_dirs = 0;
int num_of_expected_service_dirs = 0;

double double_value = (double)1.01;
unsigned char wlan_ssid[4] = {'s','s','i','d'};
int wlan_ssid_len = 4;

unsigned int int_list[3] = {1000,2,987654};
int int_list_len = 3;

char *string_list[5] = {"foo", "bar", "dev", "null", "jarru"};
int string_list_len = 5;

double double_list[4] = {0.1, 10.2, 3.14176, 0.0223};
int double_list_len = 4;


#if 0
#define DEBUG(msg) do { printf("debug:%d:%s():%s\n", __LINE__, __FUNCTION__, msg); } while(0)
#else
#define DEBUG(msg)
#endif

/* ------------------------------------------------------------------------- */
void setup(void)
{
	DEBUG("tc setup");
	g_type_init();
	is_ok = TRUE;

	num_of_expected_general_entries = 0;
	num_of_expected_network_entries = 0;
	num_of_expected_network_gprs_entries = 0;
	num_of_expected_network_wlan_entries = 0;
	num_of_expected_conn_entries = 0;
	num_of_expected_service_entries = 0;
	num_of_expected_conn_iap_entries = 0;

	num_of_expected_general_dirs = 0;
	num_of_expected_network_dirs = 0;
	num_of_expected_conn_dirs = 0;
	num_of_expected_service_dirs = 0;
}


void teardown(void)
{
	DEBUG("tc teardown");
	is_ok = FALSE;
}


/* ------------------------------------------------------------------------- */
void setup_empty(void)
{
	GConfClient *gconf_client;
	gchar *dir_name;
	GError *error = NULL;

	DEBUG("cleaning up db");
	setup();
	gconf_client = gconf_client_get_default ();

	dir_name = ICD_GCONF_SETTINGS;
	gconf_client_recursive_unset (gconf_client,
				      dir_name,
				      GCONF_UNSET_INCLUDING_SCHEMA_NAMES,
				      &error);
	if (error != NULL) {
		fail("failed to remove '%s' from gconf", dir_name);
		return;
	}
	g_object_unref (gconf_client);
}


void teardown_empty(void)
{
	setup_empty();
	teardown();
}


/* ------------------------------------------------------------------------- */
void setup_db(void)
{
	/* Create entries to backend database */

	GConfClient *gconf_client;
	GError *error;
	gboolean ret;

	DEBUG("setting up db");

	setup();
	gconf_client = gconf_client_get_default ();

#define SET_STR(key,val)					\
	do {							\
		ret = gconf_client_set_string(gconf_client,	\
					      key,		\
					      val,		\
					      &error);		\
		if (!ret) {					\
			printf("Cannot set %s to %s (%s)\n",	\
			       key, val, error->message);	\
		}						\
	} while(0)

#define SET_INT(key,val)					\
	do {							\
		ret = gconf_client_set_int(gconf_client,	\
					   key,			\
					   val,			\
					   &error);		\
		if (!ret) {					\
			printf("Cannot set %s to %d (%s)\n",	\
			       key, val, error->message);	\
		}						\
	} while(0)

#define SET_BOOL(key,val)					\
	do {							\
		ret = gconf_client_set_bool(gconf_client,	\
					   key,			\
					   val,			\
					   &error);		\
		if (!ret) {					\
			printf("Cannot set %s to %d (%s)\n",	\
			       key, val, error->message);	\
		}						\
	} while(0)

#define SET_DOUBLE(key,val)					\
	do {							\
		ret = gconf_client_set_float(gconf_client,	\
					     key,		\
					     val,		\
					     &error);		\
		if (!ret) {					\
			printf("Cannot set %s to %f (%s)\n",	\
			       key, val, error->message);	\
		}						\
	} while(0)

#define SET_BYTE_ARRAY(key, val, len)					\
	do {								\
		GSList *lst = NULL;					\
		int i;							\
		for (i=0; i<len; i++) {					\
			long v = val[i];				\
			lst = g_slist_append(lst, GINT_TO_POINTER(v));	\
		}							\
		ret = gconf_client_set_list(gconf_client,		\
					    key,			\
					    GCONF_VALUE_INT,		\
					    lst,			\
					    &error);			\
		if (!ret) {						\
			printf("Cannot set %s to list (%s)\n",		\
			       key, error->message);			\
		}							\
	} while(0)

#define SET_STRING_LIST(key, val, len)					\
	do {								\
		GSList *lst = NULL;					\
		int i;							\
		for (i=0; i<len; i++) {					\
			lst = g_slist_append(lst, val[i]);		\
		}							\
		ret = gconf_client_set_list(gconf_client,		\
					    key,			\
					    GCONF_VALUE_STRING,		\
					    lst,			\
					    &error);			\
		if (!ret) {						\
			printf("Cannot set %s to string list (%s)\n",	\
			       key, error->message);			\
		}							\
	} while(0)

#define SET_INT_LIST(key, val, len)					\
	do {								\
		GSList *lst = NULL;					\
		int i;							\
		for (i=0; i<len; i++) {					\
			lst = g_slist_append(lst, GINT_TO_POINTER(val[i])); \
		}							\
		ret = gconf_client_set_list(gconf_client,		\
					    key,			\
					    GCONF_VALUE_INT,		\
					    lst,			\
					    &error);			\
		if (!ret) {						\
			printf("Cannot set %s to int list (%s)\n",	\
			       key, error->message);			\
		}							\
	} while(0)

#define SET_DOUBLE_LIST(key, val, len)					\
	do {								\
		GSList *lst = NULL;					\
		int i;							\
		for (i=0; i<len; i++) {					\
			lst = g_slist_append(lst, &val[i]);	\
		}							\
		ret = gconf_client_set_list(gconf_client,		\
					    key,			\
					    GCONF_VALUE_FLOAT,		\
					    lst,			\
					    &error);			\
		if (!ret) {						\
			printf("Cannot set %s to double list (%s)\n",	\
			       key, error->message);			\
		}							\
	} while(0)

#define SET_PAIR(key,car,cdr)						\
	do {								\
		ret = gconf_client_set_pair(gconf_client,		\
					    key,			\
					    GCONF_VALUE_INT,		\
					    GCONF_VALUE_INT,		\
					    &car,			\
					    &cdr,			\
					    &error);			\
		if (!ret) {						\
			printf("Cannot set %s to %d/%d (%s)\n",		\
			       key, car, cdr, error->message);		\
		}							\
	} while(0)


	// General settings
	SET_STR(ICD_GCONF_SETTINGS "/foo", "bar");
	num_of_expected_general_entries++;
	SET_STR(ICD_GCONF_SETTINGS "/foo2", "bar2");
	num_of_expected_general_entries++;
	SET_INT(ICD_GCONF_SETTINGS "/num1", 255);
	num_of_expected_general_entries++;

	int a=1, b=2;
	SET_PAIR(ICD_GCONF_SETTINGS "/pair", a, b);
	num_of_expected_general_entries++;

	// Network type settings
	SET_STR(ICD_GCONF_NETWORK_MAPPING "/auto_connect_test", "invalid");
	num_of_expected_network_entries++;
	SET_STR(ICD_GCONF_NETWORK_MAPPING "/GPRS/foo", "bar");
	num_of_expected_network_gprs_entries++;
	num_of_expected_network_dirs++;
	SET_STR(ICD_GCONF_NETWORK_MAPPING "/WLAN/foo2", "bar2");
	num_of_expected_network_wlan_entries++;
	num_of_expected_network_dirs++;
	SET_BOOL(ICD_GCONF_NETWORK_MAPPING "/WLAN/boolval", TRUE);
	num_of_expected_network_wlan_entries++;
	SET_INT(ICD_GCONF_NETWORK_MAPPING "/WLAN/int@32@val", 112);
	num_of_expected_network_wlan_entries++;
	SET_INT(ICD_GCONF_NETWORK_MAPPING "/WLAN/num1", 255);
	num_of_expected_network_wlan_entries++;
	SET_DOUBLE(ICD_GCONF_NETWORK_MAPPING "/WLAN/double1", double_value);
	num_of_expected_network_wlan_entries++;
	num_of_expected_general_dirs++;

	// Connection settings
	SET_STR(ICD_GCONF_PATH "/conn_param", "testing");
	num_of_expected_conn_entries++;
	SET_STR(ICD_GCONF_PATH "/netti/wlan_testi", "nlaw");
	num_of_expected_conn_iap_entries++;
	SET_INT(ICD_GCONF_PATH "/netti/wlan_testi_int", -1);
	num_of_expected_conn_iap_entries++;
	SET_DOUBLE(ICD_GCONF_PATH "/netti/wlan_testi_double", 99.0);
	num_of_expected_conn_iap_entries++;
	SET_BOOL(ICD_GCONF_PATH "/netti/wlan_testi_bool", TRUE);
	num_of_expected_conn_iap_entries++;
	SET_BYTE_ARRAY(ICD_GCONF_PATH "/netti/wlan_ssid", wlan_ssid, wlan_ssid_len);
	num_of_expected_conn_iap_entries++;
	SET_INT_LIST(ICD_GCONF_PATH "/netti/int_list", int_list, int_list_len);
	num_of_expected_conn_iap_entries++;
	SET_STRING_LIST(ICD_GCONF_PATH "/netti/string_list", string_list, string_list_len);
	num_of_expected_conn_iap_entries++;
	SET_DOUBLE_LIST(ICD_GCONF_PATH "/netti/double_list", double_list, double_list_len);
	num_of_expected_conn_iap_entries++;
	num_of_expected_conn_dirs++;
	SET_STR(ICD_GCONF_PATH "/yksi@32@kaksi/wlan_testi", "nlaw");
	SET_STR(ICD_GCONF_PATH "/yksi@32@kaksi/wlan_testi2", "nl aw");
	SET_INT(ICD_GCONF_PATH "/yksi@32@kaksi/wlan_testi3", 3);
	num_of_expected_conn_dirs++;
	num_of_expected_general_dirs++;

	// Service type settings
	SET_STR(ICD_GCONF_SRV_PROVIDERS "/srvtest", "operator1");
	num_of_expected_service_entries++;
	SET_STR(ICD_GCONF_SRV_PROVIDERS "/operator1/foo", "bar");
	num_of_expected_service_dirs++;
	SET_STR(ICD_GCONF_SRV_PROVIDERS "/my_service/custom_ui/smiley/icon_name", "foobar.jpg");
	SET_STR(ICD_GCONF_SRV_PROVIDERS "/my_service/custom@32@ui/smiley@32@face/icon@32@name", "foo bar.jpg");
	num_of_expected_service_dirs++;
	num_of_expected_general_dirs++;
}


static int check_expected(char **expecting, char *id)
{
	int i;
	for (i=0; expecting[i]!=NULL; i++) {
		if (strcmp(expecting[i], id) == 0)
			return 1;
	}
	return 0;
}


void teardown_db(void)
{
	teardown_empty();
}


/* ========================================================================= */
START_TEST(initially_no_iaps_in_gconf)
{
	char **ids;

	DEBUG("");
	fail_unless(is_ok);
	ids = conn_settings_list_ids(CONN_SETTINGS_CONNECTION);
	fail_unless(ids == NULL);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_ids_general)
{
	char **ids;
	int num_of_general_entries = 0;
	int i;

	char *expecting[] = { "IAP", "network_type", "srv_provider", NULL };

	DEBUG("");
	ids = conn_settings_list_ids(CONN_SETTINGS_GENERAL);
	fail_unless(ids != NULL, "Backend database is empty"); 
	for (i=0; ids[i]!=NULL; i++) {
		if (check_expected(expecting, ids[i]))
			num_of_general_entries++;
	}
	fail_unless(num_of_expected_general_dirs == num_of_general_entries, "Got %d, expected %d", num_of_general_entries, num_of_expected_general_dirs);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_ids_network)
{
	char **ids;
	int num_of_network_entries = 0;
	int i;

	char *expecting[] = { "WLAN", "GPRS", NULL };

	DEBUG("");
	ids = conn_settings_list_ids(CONN_SETTINGS_NETWORK_TYPE);
	fail_unless(ids != NULL, "Backend database is empty"); 
	for (i=0; ids[i]!=NULL; i++) {
		if (check_expected(expecting, ids[i]))
			num_of_network_entries++;
	}
	fail_unless(num_of_expected_network_dirs == num_of_network_entries, "Got %d, expected %d", num_of_network_entries, num_of_expected_network_dirs);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_ids_connection)
{
	char **ids;
	int num_of_conn_entries = 0;
	int i;

	DEBUG("");
	ids = conn_settings_list_ids(CONN_SETTINGS_CONNECTION);
	fail_unless(ids != NULL, "Backend database is empty"); 
	for (i=0; ids[i]!=NULL; i++)
		num_of_conn_entries++;
	fail_unless(num_of_expected_conn_dirs == num_of_conn_entries, "Got %d, expected %d", num_of_conn_entries, num_of_expected_conn_dirs);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_ids_service)
{
	char **ids;
	int num_of_service_entries = 0;
	int i;

	DEBUG("");
	ids = conn_settings_list_ids(CONN_SETTINGS_SERVICE_TYPE);
	fail_unless(ids != NULL, "Backend database is empty"); 
	for (i=0; ids[i]!=NULL; i++) 
		num_of_service_entries++;
	fail_unless(num_of_expected_service_dirs == num_of_service_entries, "Got %d, expected %d", num_of_service_entries, num_of_expected_service_dirs);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_open_fail)
{
	ConnSettings *o;

	DEBUG("");
	o = conn_settings_open(100, "TEST-IAP1");
	fail_unless(o == NULL, "Setting found but it should not"); 
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_open_fail_connection)
{
	ConnSettings *o;
	ConnSettingsValue *value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "TEST-IAP1");
	fail_unless(o != NULL, "Setting not found but it should");

	value = conn_settings_get(o, "not found");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_INVALID, "Value found");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_open_ok_connection)
{
	ConnSettings *o;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found"); 
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_open_ok_connection_with_null_id)
{
	ConnSettings *o;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, NULL);
	fail_unless(o != NULL, "Setting not found"); 
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_connection)
{
	ConnSettings *o;
	char **keys;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");
	keys = conn_settings_list_keys(o);
	fail_unless(keys != NULL, "Backend database is empty"); 
	for (i=0; keys[i]!=NULL; i++);
	fail_unless(num_of_expected_conn_iap_entries == i, "Got %d, expected %d", i, num_of_expected_conn_iap_entries);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_network)
{
	ConnSettings *o;
	char **keys;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");
	keys = conn_settings_list_keys(o);
	fail_unless(keys != NULL, "Backend database is empty"); 
	for (i=0; keys[i]!=NULL; i++);
	fail_unless(num_of_expected_network_wlan_entries == i, "Got %d, expected %d", i, num_of_expected_network_wlan_entries);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_general)
{
	ConnSettings *o;
	char **keys;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_GENERAL, NULL);
	fail_unless(o != NULL, "Setting not found");
	keys = conn_settings_list_keys(o);
	fail_unless(keys != NULL, "Backend database is empty"); 
	for (i=0; keys[i]!=NULL; i++);
	fail_unless(num_of_expected_general_entries == i, "Got %d, expected %d", i, num_of_expected_general_entries);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_service)
{
	ConnSettings *o;
	char **keys;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_SERVICE_TYPE, "operator1");
	fail_unless(o != NULL, "Setting not found");
	keys = conn_settings_list_keys(o);
	fail_unless(keys != NULL, "Backend database is empty"); 
	for (i=0; keys[i]!=NULL; i++);
	fail_unless(num_of_expected_service_entries == i, "Got %d, expected %d", i, num_of_expected_service_entries);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_connection_ok)
{
	ConnSettings *o;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti2");
	fail_unless(o != NULL, "Setting not found");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_network_ok)
{
	ConnSettings *o;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WiMax");
	fail_unless(o != NULL, "Setting not found");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_general_ok)
{
	ConnSettings *o;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_GENERAL, "puppu");
	fail_unless(o != NULL, "Setting not found");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_service_ok)
{
	ConnSettings *o;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_SERVICE_TYPE, "operatorxxx");
	fail_unless(o != NULL, "Setting not found");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_connection_null)
{
	ConnSettings *o;
	char **keys;
	int i, count = 0;

	char *expecting[] = { "conn_param", NULL };

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, NULL);
	fail_unless(o != NULL, "Setting not found");
	keys = conn_settings_list_keys(o);
	fail_unless(keys != NULL, "Backend database is empty"); 
	for (i=0; keys[i]!=NULL; i++) {
		if (check_expected(expecting, keys[i]))
			count++;
	}
	fail_unless(num_of_expected_conn_entries == count, "Got %d, expected %d", count, num_of_expected_conn_entries);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_network_null)
{
	ConnSettings *o;
	char **keys;
	int i, count = 0;

	char *expecting[] = { "auto_connect_test", NULL };

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, NULL);
	fail_unless(o != NULL, "Setting not found");
	keys = conn_settings_list_keys(o);
	fail_unless(keys != NULL, "Backend database is empty"); 
	for (i=0; keys[i]!=NULL; i++) {
		if (check_expected(expecting, keys[i]))
			count++;
	}
	fail_unless(num_of_expected_network_entries == count, "Got %d, expected %d", count, num_of_expected_network_entries);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_service_null)
{
	ConnSettings *o;
	char **keys;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_SERVICE_TYPE, NULL);
	fail_unless(o != NULL, "Setting not found");
	keys = conn_settings_list_keys(o);
	fail_unless(keys != NULL, "Backend database is empty"); 
	for (i=0; keys[i]!=NULL; i++);
	fail_unless(num_of_expected_service_entries == i, "Got %d, expected %d", i, num_of_expected_service_entries);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_list_keys_null)
{
	DEBUG("");
	fail_unless(conn_settings_list_keys(NULL) == NULL, "Context is not null");
}
END_TEST



/* ========================================================================= */
START_TEST(conn_settings_get_invalid)
{
	ConnSettings *o;
	ConnSettingsValue *value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "not found");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_INVALID, "Value found");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_int)
{
	ConnSettings *o;
	ConnSettingsValue *value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "num1");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_INT, "Value found");
	fail_unless(value->value.int_val == 255, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_string)
{
	ConnSettings *o;
	ConnSettingsValue *value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "foo2");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_STRING, "Value found");
	fail_unless(strcmp(value->value.string_val, "bar2") == 0, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_bool)
{
	ConnSettings *o;
	ConnSettingsValue *value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "boolval");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_BOOL, "Value found");
	fail_unless(value->value.bool_val == TRUE, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_double)
{
	ConnSettings *o;
	ConnSettingsValue *value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "double1");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_DOUBLE, "Value found");
	/* convert to int in order to avoid rounding errors */
	int val1 = (int)(value->value.double_val * 1000.0);
	int val2 = (int)(double_value * 1000.0);
	fail_unless(val1 == val2, "Value is not correct, should be %d was %d", val1, val2);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_byte_array)
{
	ConnSettings *o;
	int ret;
	unsigned char *ssid;
	unsigned int len = 0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	ret = conn_settings_get_byte_array(o, "wlan_ssid", &ssid, &len);
	fail_unless(ret == CONN_SETTINGS_E_NO_ERROR);
	fail_unless(len == wlan_ssid_len);
	fail_unless(memcmp(ssid, wlan_ssid, wlan_ssid_len) == 0, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_int_list)
{
	ConnSettings *o;
	ConnSettingsValue *value;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "int_list");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_LIST, "Invalid type %d (should be %d)", value->type, CONN_SETTINGS_VALUE_LIST);
	for (i=0; value->value.list_val[i]!=NULL; i++) {
		fail_unless(value->value.list_val[i]->type == CONN_SETTINGS_VALUE_INT);
		fail_unless(value->value.list_val[i]->value.int_val == int_list[i], "Int list [%d] = %d, should be %d", i, value->value.list_val[i]->value.int_val, int_list[i]);
	}
	fail_unless(int_list_len == i, "Got %d, expected %d", i, int_list_len);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_string_list)
{
	ConnSettings *o;
	ConnSettingsValue *value;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "string_list");
	fail_unless(value != NULL, "string_list was not found in backend db");
	fail_unless(value->type == CONN_SETTINGS_VALUE_LIST, "Invalid type %d (should be %d)", value->type, CONN_SETTINGS_VALUE_LIST);
	for (i=0; value->value.list_val[i]!=NULL; i++) {
		fail_unless(value->value.list_val[i]->type == CONN_SETTINGS_VALUE_STRING, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_STRING, value->value.list_val[i]->type);
		fail_unless(strcmp(value->value.list_val[i]->value.string_val, string_list[i]) == 0, "String list [%d] = \"%s\", should be \"%s\"", i, value->value.list_val[i]->value.string_val, string_list[i]);
	}
	fail_unless(string_list_len == i, "Got %d, expected %d", i, string_list_len);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_ok_double_list)
{
	ConnSettings *o;
	ConnSettingsValue *value;
	int i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	value = conn_settings_get(o, "double_list");
	fail_unless(value != NULL);
	fail_unless(value->type == CONN_SETTINGS_VALUE_LIST, "Invalid type %d (should be %d)", value->type, CONN_SETTINGS_VALUE_LIST);
	for (i=0; value->value.list_val[i]!=NULL; i++) {
		fail_unless(value->value.list_val[i]->type == CONN_SETTINGS_VALUE_DOUBLE, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_DOUBLE, value->value.list_val[i]->type);
#if 0
		/* does not seem to work correctly because of conversion errors */
		int val1 = (int)(value->value.list_val[i]->value.double_val * 1000000.0);
		int val2 = (int)(double_list[i] * 1000000.0);
		fail_unless(val1 == val2, "Double list [%d] = %f (%d), should be %f (%d)", i, value->value.list_val[i]->value.double_val, double_list[i]);
#endif
	}
	fail_unless(double_list_len == i, "Got %d, expected %d", i, double_list_len);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_string_test)
{
	ConnSettings *o;
	char *value = NULL;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_get_string(o, "wlan_testi_not_found", NULL)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_get_string(o, "wlan_testi_not_found", &value)==CONN_SETTINGS_E_NO_SUCH_KEY);
	fail_unless(conn_settings_get_string(o, "wlan_testi", &value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(strcmp(value, "nlaw") == 0);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_int_test)
{
	ConnSettings *o;
	int value = 0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_get_int(o, "wlan_testi_int", NULL)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_get_int(o, "wlan_testi_int", &value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(value == -1);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_double_test)
{
	ConnSettings *o;
	double value = 0.0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_get_double(o, "wlan_testi_double", NULL)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_get_double(o, "wlan_testi_double", &value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(value == 99.0, "Test failed, value == %f, should be %f", value, 99.0);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_bool_test)
{
	ConnSettings *o;
	int value = FALSE;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_get_bool(o, "wlan_testi_bool", NULL)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_get_bool(o, "wlan_testi_bool", &value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(value, "Test failed, value == %d, should be %d", value, TRUE);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_byte_array_test)
{
	ConnSettings *o;
	unsigned char *value = 0;
	unsigned int len = 0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_get_byte_array(o, "wlan_ssid", NULL, NULL)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_get_byte_array(o, "wlan_ssid", &value, NULL)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_get_byte_array(o, "wlan_ssid", &value, &len)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(len == wlan_ssid_len, "Byte array length is not correct, should be %d, was %d", wlan_ssid_len, len);
	fail_unless(memcmp(value, wlan_ssid, len) == 0, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_list_test)
{
	ConnSettings *o;
	ConnSettingsValue **value;
	int i, ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	ret = conn_settings_get_list(o, "int_list", &value);
	fail_unless(value != NULL);
	fail_unless(value[0]->type == CONN_SETTINGS_VALUE_INT, "Invalid type %d (should be %d)", value[0]->type, CONN_SETTINGS_VALUE_INT);
	for (i=0; value[i]!=NULL; i++) {
		fail_unless(value[i]->type == CONN_SETTINGS_VALUE_INT, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_INT, value[i]->type);
		fail_unless(value[i]->value.int_val == int_list[i], "Int list [%d] = %d, should be %d", i, value[i]->value.int_val, int_list[i]);
	}
	fail_unless(int_list_len == i, "Got %d, expected %d", i, int_list_len);
	conn_settings_values_destroy(value);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_invalid)
{
	ConnSettings *o;
	ConnSettingsValue *value = NULL;
	int ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	ret = conn_settings_set(o, "not found", value);
	fail_unless(ret != 0);
	ret = conn_settings_set(o, "boolval", value);
	fail_unless(ret != 0);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_create_iap)
{
	ConnSettings *o;
	
	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "new-iap");
	fail_unless(o != NULL, "Setting not found");
	conn_settings_close(o);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_different_type)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval;
	int ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value.type = CONN_SETTINGS_VALUE_INT;
	value.value.int_val = 10;
	ret = conn_settings_set(o, "num_int", &value);
	fail_unless(ret == 0);
	retval = conn_settings_get(o, "num_int");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_INT, "Value found");
	fail_unless(retval->value.int_val == 10, "Value is not correct");

	/* GConf allows one to put different type (atleast if there is no schema involved) */
	value.type = CONN_SETTINGS_VALUE_STRING;
	value.value.string_val = "foo bar string";
	ret = conn_settings_set(o, "num_int", &value);
	fail_unless(ret == CONN_SETTINGS_E_NO_ERROR, "Returned %d, was expecting %d", ret, CONN_SETTINGS_E_NO_ERROR);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_int)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval;
	int ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value.type = CONN_SETTINGS_VALUE_INT;
	value.value.int_val = 10;
	ret = conn_settings_set(o, "numxx", &value);
	fail_unless(ret == 0);
	retval = conn_settings_get(o, "numxx");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_INT, "Value found");
	fail_unless(retval->value.int_val == 10, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_string)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval;
	int ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value.type = CONN_SETTINGS_VALUE_STRING;
	value.value.string_val = "string test";
	ret = conn_settings_set(o, "string test", &value);
	fail_unless(ret == 0);
	retval = conn_settings_get(o, "string test");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_STRING, "Value found");
	fail_unless(strcmp(retval->value.string_val, "string test") == 0, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_double)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval;
	int ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value.type = CONN_SETTINGS_VALUE_DOUBLE;
	value.value.double_val = 10.0;
	ret = conn_settings_set(o, "double test", &value);
	fail_unless(ret == 0);
	retval = conn_settings_get(o, "double test");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_DOUBLE, "Value found");
	fail_unless(retval->value.double_val == 10.0, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_bool)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval;
	int ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value.type = CONN_SETTINGS_VALUE_BOOL;
	value.value.bool_val = TRUE;
	ret = conn_settings_set(o, "bool test", &value);
	fail_unless(ret == 0);
	retval = conn_settings_get(o, "bool test");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_BOOL, "Value found");
	fail_unless(retval->value.bool_val == TRUE, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_byte_array)
{
	ConnSettings *o;
	ConnSettingsValue value;
	int ret;
	unsigned char ba[] = { 'A', 'B', 'C', 'E', 'F', 'G', 'H' };
	unsigned char *buf;
	unsigned int len;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	value.type = CONN_SETTINGS_VALUE_BYTE_ARRAY;
	value.value.byte_array.val = ba;
	value.value.byte_array.len = sizeof(ba);
	ret = conn_settings_set(o, "byte array test", &value);
	fail_unless(ret == 0);
	ret = conn_settings_get_byte_array(o, "byte array test", &buf, &len);
	fail_unless(len == sizeof(ba));
	fail_unless(memcmp(buf, ba, len) == 0, "Value is not correct");
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_mixed_list_which_should_fail)
{
	ConnSettings *o;
	ConnSettingsValue value, test_list[4], *plist[5];
	int ret;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	test_list[0].type = CONN_SETTINGS_VALUE_STRING;
	test_list[0].value.string_val = "1000";
	test_list[1].type = CONN_SETTINGS_VALUE_INT;
	test_list[1].value.int_val = INT_MAX;
	test_list[2].type = CONN_SETTINGS_VALUE_INT;
	test_list[2].value.int_val = INT_MIN;
	test_list[3].type = CONN_SETTINGS_VALUE_STRING;
	test_list[3].value.string_val = "-1000";

	plist[0] = &test_list[0];
	plist[1] = &test_list[1];
	plist[2] = &test_list[2];
	plist[3] = &test_list[3];
	plist[4] = NULL;

	value.type = CONN_SETTINGS_VALUE_LIST;
	value.value.list_val = plist;

	ret = conn_settings_set(o, "string list test", &value);
	fail_unless(ret == CONN_SETTINGS_E_INVALID_TYPE);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_int_list)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval, test_int_list[4], *plist[5];
	int ret, i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	test_int_list[0].type = CONN_SETTINGS_VALUE_INT;
	test_int_list[0].value.int_val = 1000;
	test_int_list[1].type = CONN_SETTINGS_VALUE_INT;
	test_int_list[1].value.int_val = INT_MAX;
	test_int_list[2].type = CONN_SETTINGS_VALUE_INT;
	test_int_list[2].value.int_val = INT_MIN;
	test_int_list[3].type = CONN_SETTINGS_VALUE_INT;
	test_int_list[3].value.int_val = -1000;

	plist[0] = &test_int_list[0];
	plist[1] = &test_int_list[1];
	plist[2] = &test_int_list[2];
	plist[3] = &test_int_list[3];
	plist[4] = NULL;

	value.type = CONN_SETTINGS_VALUE_LIST;
	value.value.list_val = plist;

	ret = conn_settings_set(o, "int list test", &value);
	fail_unless(ret == 0);

	retval = conn_settings_get(o, "int list test");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_LIST, "Value found");

	fail_unless(retval->value.list_val[0]->type == CONN_SETTINGS_VALUE_INT, "Invalid type %d (should be %d)", retval->value.list_val[0]->type, CONN_SETTINGS_VALUE_INT);
	for (i=0; retval->value.list_val[i]!=NULL; i++) {
		fail_unless(retval->value.list_val[i]->type == CONN_SETTINGS_VALUE_INT, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_INT, retval->value.list_val[i]->type);
		fail_unless(retval->value.list_val[i]->value.int_val == test_int_list[i].value.int_val, "Int list [%d] = %d, should be %d", i, retval->value.list_val[i]->value.int_val, test_int_list[i].value.int_val);
	}
	fail_unless((sizeof(test_int_list)/sizeof(ConnSettingsValue)) == i, "Got %d, expected %d", i, sizeof(test_int_list)/sizeof(ConnSettingsValue));
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_string_list)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval, test_list[4], *plist[5];
	int ret, i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	test_list[0].type = CONN_SETTINGS_VALUE_STRING;
	test_list[0].value.string_val = "1000";
	test_list[1].type = CONN_SETTINGS_VALUE_STRING;
	test_list[1].value.string_val = "INT_MAX";
	test_list[2].type = CONN_SETTINGS_VALUE_STRING;
	test_list[2].value.string_val = "INT_MIN";
	test_list[3].type = CONN_SETTINGS_VALUE_STRING;
	test_list[3].value.string_val = "-1000";

	plist[0] = &test_list[0];
	plist[1] = &test_list[1];
	plist[2] = &test_list[2];
	plist[3] = &test_list[3];
	plist[4] = NULL;

	value.type = CONN_SETTINGS_VALUE_LIST;
	value.value.list_val = plist;

	ret = conn_settings_set(o, "string list test", &value);
	fail_unless(ret == 0);

	retval = conn_settings_get(o, "string list test");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_LIST, "Value found");

	fail_unless(retval->value.list_val[0]->type == CONN_SETTINGS_VALUE_STRING, "Invalid type %d (should be %d)", retval->value.list_val[0]->type, CONN_SETTINGS_VALUE_STRING);
	for (i=0; retval->value.list_val[i]!=NULL; i++) {
		fail_unless(retval->value.list_val[i]->type == CONN_SETTINGS_VALUE_STRING, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_STRING, retval->value.list_val[i]->type);
		fail_unless(strcmp(retval->value.list_val[i]->value.string_val, test_list[i].value.string_val) == 0, "String list [%d] = %s, should be %s", i, retval->value.list_val[i]->value.string_val, test_list[i].value.string_val);
	}
	fail_unless((sizeof(test_list)/sizeof(ConnSettingsValue)) == i, "Got %d, expected %d", i, sizeof(test_list)/sizeof(ConnSettingsValue));
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_ok_double_list)
{
	ConnSettings *o;
	ConnSettingsValue value, *retval, test_list[4], *plist[5];
	int ret, i;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_NETWORK_TYPE, "WLAN");
	fail_unless(o != NULL, "Setting not found");

	test_list[0].type = CONN_SETTINGS_VALUE_DOUBLE;
	test_list[0].value.double_val = 1000.0;
	test_list[1].type = CONN_SETTINGS_VALUE_DOUBLE;
	test_list[1].value.double_val = -3.1415926;
	test_list[2].type = CONN_SETTINGS_VALUE_DOUBLE;
	test_list[2].value.double_val = 0.00001;
	test_list[3].type = CONN_SETTINGS_VALUE_DOUBLE;
	test_list[3].value.double_val = FLT_MAX;

	plist[0] = &test_list[0];
	plist[1] = &test_list[1];
	plist[2] = &test_list[2];
	plist[3] = &test_list[3];
	plist[4] = NULL;

	value.type = CONN_SETTINGS_VALUE_LIST;
	value.value.list_val = plist;

	ret = conn_settings_set(o, "double list test", &value);
	fail_unless(ret == 0);

	retval = conn_settings_get(o, "double list test");
	fail_unless(retval->type == CONN_SETTINGS_VALUE_LIST, "Value found");

	fail_unless(retval->value.list_val[0]->type == CONN_SETTINGS_VALUE_DOUBLE, "Invalid type %d (should be %d)", retval->value.list_val[0]->type, CONN_SETTINGS_VALUE_DOUBLE);
	for (i=0; retval->value.list_val[i]!=NULL; i++) {
		fail_unless(retval->value.list_val[i]->type == CONN_SETTINGS_VALUE_DOUBLE, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_DOUBLE, retval->value.list_val[i]->type);
		fail_unless(retval->value.list_val[i]->value.double_val == test_list[i].value.double_val, "Double list [%d] = %f, should be %f", i, retval->value.list_val[i]->value.double_val, test_list[i].value.double_val);
	}
	fail_unless((sizeof(test_list)/sizeof(ConnSettingsValue)) == i, "Got %d, expected %d", i, sizeof(test_list)/sizeof(ConnSettingsValue));
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_string_test)
{
	ConnSettings *o;
	char *value = NULL;
	char *retval = NULL;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_set_string(o, NULL, "foo bar")==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_set_string(o, "wlan_test_not_found", value)==CONN_SETTINGS_E_INVALID_PARAMETER);
	value = "foobar";
	fail_unless(conn_settings_set_string(o, "string test", value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(conn_settings_get_string(o, "string test", &retval)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(strcmp(value, retval) == 0);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_int_test)
{
	ConnSettings *o;
	int value = 0;
	int retval = 0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_set_int(o, NULL, -1)==CONN_SETTINGS_E_INVALID_PARAMETER);
	value = 999;
	fail_unless(conn_settings_set_int(o, "int test", value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(conn_settings_get_int(o, "int test", &retval)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(value == retval);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_double_test)
{
	ConnSettings *o;
	double value = 0;
	double retval = 0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_set_double(o, NULL, -1.1)==CONN_SETTINGS_E_INVALID_PARAMETER);
	value = 999.999;
	fail_unless(conn_settings_set_double(o, "double test", value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(conn_settings_get_double(o, "double test", &retval)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(value == retval);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_bool_test)
{
	ConnSettings *o;
	int value = 0;
	int retval = 0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_set_bool(o, NULL, -1)==CONN_SETTINGS_E_INVALID_PARAMETER);
	value = TRUE;
	fail_unless(conn_settings_set_bool(o, "bool test", value)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(conn_settings_get_bool(o, "bool test", &retval)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(value == retval);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_byte_array_test)
{
	ConnSettings *o;
	unsigned char value[] = { 'A','P','U','V','A' };
	unsigned char *retval = NULL;
	unsigned int len = sizeof(value), retlen = 0;
	int st;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_set_byte_array(o, NULL, (unsigned char *)"foo bar", 2)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_set_byte_array(o, "wlan_test_not_found", value, 0)==CONN_SETTINGS_E_INVALID_PARAMETER);
	st = conn_settings_set_byte_array(o, "byte array test", value, len);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
	fail_unless(conn_settings_get_byte_array(o, "byte array test", &retval, &retlen)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(len == retlen);
	fail_unless(memcmp(value, retval, len) == 0);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_list_int_test)
{
	ConnSettings *o;
	int st, i;
	int int_list[] = { 'A','P','U','V','A' };
	int int_list2[] = { 1000, 99, -1, 32, 1020304 };
	int len = sizeof(int_list) / sizeof(int);
	int len2 = sizeof(int_list2) / sizeof(int);
	ConnSettingsValue **value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	st = conn_settings_set_list(o, "foobar", int_list, len, CONN_SETTINGS_VALUE_INT);
	fail_if(st, "set_list() failed, returned %d instead of %d", st, 0);

	st = conn_settings_set_list(o, "foobar2", int_list2, len2, CONN_SETTINGS_VALUE_INT);
	fail_if(st, "set_list() failed, returned %d instead of %d", st, 0);

	st = conn_settings_set_list(o, "foobar_empty", NULL, 0, CONN_SETTINGS_VALUE_INT);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Invalid return value %d, should be %d", st, CONN_SETTINGS_E_NO_ERROR);

	st = conn_settings_get_list(o, "foobar_empty", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_if(*value, "Invalid value, the returned entry was not null");

	st = conn_settings_get_list(o, "foobar", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);

	st = conn_settings_get_list(o, "foobar2", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);

	fail_unless(value[0]->type == CONN_SETTINGS_VALUE_INT, "Invalid type %d (should be %d)", value[0]->type, CONN_SETTINGS_VALUE_INT);
	for (i=0; value[i]!=NULL; i++) {
		fail_unless(value[i]->type == CONN_SETTINGS_VALUE_INT, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_INT, value[i]->type);
		fail_unless(value[i]->value.int_val == int_list2[i], "Int list [%d] = %d, should be %d", i, value[i]->value.int_val, int_list2[i]);
	}
	fail_unless(len == i, "Got %d, expected %d", i, len);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_set_list_string_test)
{
	ConnSettings *o;
	int st, i;
	char *list[] = { "koe", NULL };
	char *list2[] = { "koe", "apuva" };
	char *empty[] = { "", NULL };
	int len0 = sizeof(list2) / sizeof(char *);
	ConnSettingsValue **value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_set_list(o, NULL, list, 2, CONN_SETTINGS_VALUE_LIST)==CONN_SETTINGS_E_INVALID_PARAMETER);
	st = conn_settings_set_list(o, "foobar", NULL, 2, CONN_SETTINGS_VALUE_LIST);
	fail_unless(st==CONN_SETTINGS_E_INVALID_TYPE, "Invalid return value %d, should be %d", st, CONN_SETTINGS_E_INVALID_TYPE);

	st = conn_settings_set_list(o, "foobar_empty", NULL, 2, CONN_SETTINGS_VALUE_STRING);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Invalid return value %d, should be %d", st, CONN_SETTINGS_E_NO_ERROR);

	st = conn_settings_get_list(o, "foobar_empty", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_if(*value, "Invalid value, the returned entry was not null");

	st = conn_settings_get_list(o, "foobar_not_found", &value);
	fail_unless(value != NULL);
	fail_unless(st == CONN_SETTINGS_E_NO_SUCH_KEY);

	st=conn_settings_set_list(o, "foobar", list, 1, CONN_SETTINGS_VALUE_LIST);
	fail_unless(st==CONN_SETTINGS_E_INVALID_TYPE, "Invalid return value %d, should be %d", st, CONN_SETTINGS_E_INVALID_TYPE);

	st=conn_settings_set_list(o, "foobar", list2, 2, CONN_SETTINGS_VALUE_STRING);
	fail_if(st, "Invalid return code %d", st);
	st = conn_settings_get_list(o, "foobar", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);
	fail_unless(value[0]->type == CONN_SETTINGS_VALUE_STRING, "Invalid type %d (should be %d)", value[0]->type, CONN_SETTINGS_VALUE_STRING);
	for (i=0; value[i]!=NULL; i++) {
		fail_unless(value[i]->type == CONN_SETTINGS_VALUE_STRING, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_STRING, value[i]->type);
		fail_unless(strcmp(value[i]->value.string_val, list2[i]) == 0, "String list [%d] = \"%s\", should be \"%s\"", i, value[i]->value.string_val, list2[i]);
	}
	fail_unless(len0 == i, "Got %d, expected %d", i, len0);

	st=conn_settings_set_list(o, "foobar_empty", empty, 1, CONN_SETTINGS_VALUE_STRING);
	fail_if(st, "Invalid return code %d", st);
	st = conn_settings_get_list(o, "foobar_empty", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);
	fail_unless(value[0]->type == CONN_SETTINGS_VALUE_STRING, "Invalid type %d (should be %d)", value[0]->type, CONN_SETTINGS_VALUE_STRING);
	fail_unless(strcmp(value[0]->value.string_val, empty[0]) == 0, "String list [%d] = \"%s\", should be \"%s\"", 0, value[0]->value.string_val, empty[0]);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_unset_string_test)
{
	ConnSettings *o;
	char *value = "foobar", *retval;
	int st;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	st = conn_settings_set_string(o, "string test", value);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
	fail_unless(conn_settings_get_string(o, "string test", &retval)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(strlen(value) == strlen(retval));

	st = conn_settings_unset(o, "string test");
	fail_if(st, "Unset failed (%d)", st);
	fail_unless(conn_settings_get_string(o, "string test", &retval)==CONN_SETTINGS_E_NO_SUCH_KEY);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_unset_int_test)
{
	ConnSettings *o;
	int v1 = 99, v2;
	int st;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	st = conn_settings_set_int(o, "int test", v1);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
	fail_unless(conn_settings_get_int(o, "int test", &v2)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(v1 == v2);

	st = conn_settings_unset(o, "int test");
	fail_if(st, "Unset failed (%d)", st);
	fail_unless(conn_settings_get_int(o, "int test", &v2)==CONN_SETTINGS_E_NO_SUCH_KEY);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_unset_double_test)
{
	ConnSettings *o;
	double v1 = 99.23, v2;
	int st;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	st = conn_settings_set_double(o, "double test", v1);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
	fail_unless(conn_settings_get_double(o, "double test", &v2)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(v1 == v2);

	st = conn_settings_unset(o, "double test");
	fail_if(st, "Unset failed (%d)", st);
	fail_unless(conn_settings_get_double(o, "double test", &v2)==CONN_SETTINGS_E_NO_SUCH_KEY);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_unset_bool_test)
{
	ConnSettings *o;
	int v1 = TRUE, v2;
	int st;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	st = conn_settings_set_bool(o, "bool test", v1);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
	fail_unless(conn_settings_get_bool(o, "bool test", &v2)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(v1 == v2);

	st = conn_settings_unset(o, "bool test");
	fail_if(st, "Unset failed (%d)", st);
	fail_unless(conn_settings_get_bool(o, "bool test", &v2)==CONN_SETTINGS_E_NO_SUCH_KEY);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_unset_byte_array_test)
{
	ConnSettings *o;
	unsigned char value[] = { 'A','P','U','V','A' };
	unsigned char *retval = NULL;
	unsigned int len = sizeof(value), retlen = 0;
	int st;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	st = conn_settings_set_byte_array(o, "byte array test", value, len);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
	fail_unless(conn_settings_get_byte_array(o, "byte array test", &retval, &retlen)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(len == retlen);
	fail_unless(memcmp(value, retval, len) == 0);

	st = conn_settings_unset(o, "byte array test");
	fail_if(st, "Unset failed (%d)", st);
	fail_unless(conn_settings_get_byte_array(o, "byte array test", &retval, &retlen)==CONN_SETTINGS_E_NO_SUCH_KEY);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_unset_list_test)
{
	ConnSettings *o;
	int st, i;
	char *list = "koe";
	int int_list[] = { 'A','P','U','V','A' };
	int int_list2[] = { 1000, 99, -1, 32, 1020304 };
	int len = sizeof(int_list) / sizeof(int);
	int len2 = sizeof(int_list2) / sizeof(int);
	ConnSettingsValue **value;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	fail_unless(conn_settings_set_list(o, NULL, list, 2, CONN_SETTINGS_VALUE_LIST)==CONN_SETTINGS_E_INVALID_PARAMETER);
	fail_unless(conn_settings_set_list(o, "foobar", NULL, 2, CONN_SETTINGS_VALUE_LIST)==CONN_SETTINGS_E_INVALID_TYPE);
	fail_unless(conn_settings_set_list(o, "foobar", list, 2, CONN_SETTINGS_VALUE_LIST)==CONN_SETTINGS_E_INVALID_TYPE);

	st = conn_settings_set_list(o, "foobar", int_list, len, CONN_SETTINGS_VALUE_INT);
	fail_if(st, "set_list() failed, returned %d instead of %d", st, 0);

	st = conn_settings_set_list(o, "foobar2", int_list2, len2, CONN_SETTINGS_VALUE_INT);
	fail_if(st, "set_list() failed, returned %d instead of %d", st, 0);

	st = conn_settings_get_list(o, "foobar", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);

	st = conn_settings_get_list(o, "foobar2", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);

	fail_unless(value[0]->type == CONN_SETTINGS_VALUE_INT, "Invalid type %d (should be %d)", value[0]->type, CONN_SETTINGS_VALUE_INT);
	for (i=0; value[i]!=NULL; i++) {
		fail_unless(value[i]->type == CONN_SETTINGS_VALUE_INT, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_INT, value[i]->type);
		fail_unless(value[i]->value.int_val == int_list2[i], "Int list [%d] = %d, should be %d", i, value[i]->value.int_val, int_list2[i]);
	}
	fail_unless(len == i, "Got %d, expected %d", i, len);

	st = conn_settings_unset(o, "foobar");
	fail_if(st, "Unset failed (%d)", st);
	st = conn_settings_get_list(o, "foobar", &value);
	fail_unless(st==CONN_SETTINGS_E_NO_SUCH_KEY, "Return code %d, should be %d", st, CONN_SETTINGS_E_NO_SUCH_KEY);

	st = conn_settings_unset(o, "foobar2");
	fail_if(st, "Unset failed (%d)", st);
	fail_unless(conn_settings_get_list(o, "foobar2", &value)==CONN_SETTINGS_E_NO_SUCH_KEY);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_remove_test)
{
	ConnSettings *o;
	int v1 = TRUE, v2;
	int st;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	st = conn_settings_set_bool(o, "bool test", v1);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
	fail_unless(conn_settings_get_bool(o, "bool test", &v2)==CONN_SETTINGS_E_NO_ERROR);
	fail_unless(v1 == v2);

	st = conn_settings_remove(o);
	fail_if(st, "Remove failed (%d)", st);
	fail_unless(conn_settings_get_bool(o, "bool test", &v2)==CONN_SETTINGS_E_NO_SUCH_KEY);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_close_test)
{
	ConnSettings *o;
	int st;
	int v1 = 0;

	DEBUG("");
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	conn_settings_close(o);
	st = conn_settings_set_bool(o, "bool test", v1);
	fail_unless(st);
}
END_TEST



/* ========================================================================= */
GMainLoop* mainloop = NULL;
gboolean notify_received = FALSE;

void notify_func(ConnSettingsType type,
		 const char *id,
		 const char *key,
		 ConnSettingsValue *value,
		 void *user_data);
void notify_func(ConnSettingsType type,
		 const char *id,
		 const char *key,
		 ConnSettingsValue *value,
		 void *user_data)
{
	g_debug("%s(): type=%d, id=%s, key=%s, value=%p, user_data=%p\n", __FUNCTION__, (int)type, id, key, value, user_data);
	notify_received = TRUE;
}


gboolean func_idle(gpointer data);
gboolean func_idle(gpointer data)
{
	static ConnSettings *o = NULL;
	int v1 = 99, v2;
	int st = 0;
	static int state = 0;

	if (state == 0) {
		o = (ConnSettings *)data;
		st = conn_settings_set_int(o, "int_test", v1);
		fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
		state = 1;
		return TRUE;
	} else if (state == 1) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		fail_unless(conn_settings_get_int(o, "int_test", &v2)==CONN_SETTINGS_E_NO_ERROR);
		fail_unless(v1 == v2);
		st = conn_settings_unset(o, "int_test");
		fail_if(st, "Unset failed (%d)", st);
		state = 2;
		return TRUE;
	} else if (state == 2) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		fail_unless(conn_settings_get_int(o, "int_test", &v2)==CONN_SETTINGS_E_NO_SUCH_KEY);
		conn_settings_del_notify(o);

		/* Make sure we do not get any more notifications */
		st = conn_settings_set_int(o, "int_test", v1);
		fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
		state = 3;
		g_timeout_add(1, func_idle, NULL);
		return FALSE;
	} else if (state == 3) {
		fail_unless(notify_received == FALSE, "Notification received");
		conn_settings_close(o);
		DEBUG("test succeed");
		g_main_quit(mainloop);
	}

	return FALSE;
}


gboolean func(gpointer data);
gboolean func(gpointer data)
{
	ConnSettings *o;

	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	conn_settings_add_notify(o, (ConnSettingsNotifyFunc *)notify_func, NULL);

	g_idle_add(func_idle, (gpointer)o);
	return FALSE;
}


START_TEST(conn_settings_notify_test)
{
	mainloop = g_main_loop_new(NULL, FALSE);
	if (mainloop == NULL) {
		g_error("%s(): Failed to create mainloop!\n", __FUNCTION__);
	}

	DEBUG("");
	g_idle_add(func, NULL);
	g_main_loop_run(mainloop);
	fail_if(notify_received, "Notification not received.");
}
END_TEST


/* ========================================================================= */
int notify_int_list[] = { 'A','P','U','V','A' };
int notify_int_list_len = sizeof(notify_int_list) / sizeof(int);

void notify_func2(ConnSettingsType type,
		 const char *id,
		 const char *key,
		 ConnSettingsValue *value,
		 void *user_data);
void notify_func2(ConnSettingsType type,
		 const char *id,
		 const char *key,
		 ConnSettingsValue *value,
		 void *user_data)
{
	static int count = 0;
	g_debug("%s(): type=%d, id=%s, key=%s, value=%p, user_data=%p\n", __FUNCTION__, (int)type, id, key, value, user_data);
	if (count == 0) {
		fail_unless(value != NULL, "Value is missing when setting");
		fail_unless(value->type == CONN_SETTINGS_VALUE_LIST, "Value is not a list");
		fail_unless(value->value.list_val[0]->type == CONN_SETTINGS_VALUE_INT, "Value is not a list of ints");
		int i;
		ConnSettingsValue **list = value->value.list_val;
		for (i=0; list[i]!=NULL; i++) {
			fail_unless(list[i]->type == CONN_SETTINGS_VALUE_INT, "Invalid list type, should be %d, was %d", CONN_SETTINGS_VALUE_INT, list[i]->type);
			fail_unless(list[i]->value.int_val == notify_int_list[i], "Int list [%d] = %d, should be %d", i, list[i]->value.int_val, notify_int_list[i]);
		}
		fail_unless(notify_int_list_len == i, "Got %d, expected %d", i, notify_int_list_len);

		count++;
	} else if (count == 1) {
		fail_unless(value == NULL, "Value is set when unsetting");
		count++;
	}
	notify_received = TRUE;
}


gboolean func_idle2(gpointer data);
gboolean func_idle2(gpointer data)
{
	static ConnSettings *o = NULL;
	int st = 0;
	static int state = 0;
	ConnSettingsValue **value;

	if (state == 0) {
		o = (ConnSettings *)data;
		st = conn_settings_set_list(o, "int_list_test", notify_int_list, notify_int_list_len, CONN_SETTINGS_VALUE_INT);
		fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
		state = 1;
		return TRUE;
	} else if (state == 1) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		st = conn_settings_get_list(o, "int_list_test", &value);
		fail_unless(value != NULL);
		fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
		fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);

		st = conn_settings_unset(o, "int_list_test");
		fail_if(st, "Unset failed (%d)", st);
		state = 2;
		return TRUE;
	} else if (state == 2) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		st = conn_settings_get_list(o, "int_list_test", &value);
		fail_unless(st == CONN_SETTINGS_E_NO_SUCH_KEY);
		conn_settings_del_notify(o);

		/* Make sure we do not get any more notifications */
		st = conn_settings_set_list(o, "int_list_test", notify_int_list, notify_int_list_len, CONN_SETTINGS_VALUE_INT);
		fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
		state = 3;
		g_timeout_add(1, func_idle2, NULL);
		return FALSE;
	} else if (state == 3) {
		fail_unless(notify_received == FALSE, "Notification received");
		conn_settings_close(o);
		DEBUG("test succeed");
		g_main_quit(mainloop);
	}

	return FALSE;
}

gboolean func2(gpointer data);
gboolean func2(gpointer data)
{
	ConnSettings *o;

	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	conn_settings_add_notify(o, (ConnSettingsNotifyFunc *)notify_func2, NULL);

	g_idle_add(func_idle2, (gpointer)o);
	return FALSE;
}

START_TEST(conn_settings_notify_test_list)
{
	notify_received = FALSE;
	mainloop = g_main_loop_new(NULL, FALSE);
	if (mainloop == NULL) {
		g_error("%s(): Failed to create mainloop!\n", __FUNCTION__);
	}

	DEBUG("");
	g_idle_add(func2, NULL);
	g_main_loop_run(mainloop);
	fail_if(notify_received, "Notification not received.");
}
END_TEST



/* ========================================================================= */
int notify_int_list3[] = { 'A','P','U','V','A' };
int notify_int_list_len3 = sizeof(notify_int_list) / sizeof(int);
gboolean iap_removed = FALSE;
ConnSettings *notif_ctx = NULL;


void notify_func3(ConnSettingsType type,
		 const char *id,
		 const char *key,
		 ConnSettingsValue *value,
		 void *user_data);
void notify_func3(ConnSettingsType type,
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


gboolean func_idle3(gpointer data);
gboolean func_idle3(gpointer data)
{
	static ConnSettings *o = NULL;
	int st = 0;
	static int state = 0;
	ConnSettingsValue **value;

	if (state == 0) {
		o = (ConnSettings *)data;
		st = conn_settings_set_list(o, "int_list_test", notify_int_list3, notify_int_list_len3, CONN_SETTINGS_VALUE_INT);
		fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Setting failed, return code %d should be %d", st, 0);
		state = 1;
		return TRUE;
	} else if (state == 1) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		st = conn_settings_get_list(o, "int_list_test", &value);
		fail_unless(value != NULL);
		fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
		fail_unless((*value)->type != CONN_SETTINGS_VALUE_LIST, "Invalid type %d, should be %d", (*value)->type, CONN_SETTINGS_VALUE_LIST);

		st = conn_settings_remove(o);
		fail_if(st, "Unset failed (%d)", st);
		state = 2;
		return TRUE;
	} else if (state == 2) {
		if (!notify_received)
			return TRUE;
		notify_received = FALSE;

		st = conn_settings_get_list(o, "int_list_test", &value);
		fail_unless(st == CONN_SETTINGS_E_NO_SUCH_KEY);
		conn_settings_del_notify(o);
		conn_settings_close(o);
		fail_unless(iap_removed, "IAP was not removed");
		DEBUG("test succeed");
		conn_settings_close(notif_ctx);
		g_main_quit(mainloop);
	}

	return FALSE;
}

gboolean func3(gpointer data);
gboolean func3(gpointer data)
{
	ConnSettings *o;

	notif_ctx = conn_settings_open(CONN_SETTINGS_CONNECTION, NULL);
	fail_unless(notif_ctx != NULL, "Setting not found");
	conn_settings_add_notify(notif_ctx, (ConnSettingsNotifyFunc *)notify_func3, NULL);

	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(o != NULL, "Setting not found");

	g_idle_add(func_idle3, (gpointer)o);
	return FALSE;
}

START_TEST(conn_settings_notify_test_remove_iap)
{
	notify_received = FALSE;
	mainloop = g_main_loop_new(NULL, FALSE);
	if (mainloop == NULL) {
		g_error("%s(): Failed to create mainloop!\n", __FUNCTION__);
	}

	DEBUG("");
	g_idle_add(func3, NULL);
	g_main_loop_run(mainloop);
	fail_if(notify_received, "Notification not received.");
}
END_TEST



/* ========================================================================= */
START_TEST(conn_settings_get_invalid_data_type)
{
	ConnSettings *o;
	ConnSettingsValue *value;

	o = conn_settings_open(CONN_SETTINGS_GENERAL, NULL);
	fail_unless(o != NULL, "Setting not found");
	value = conn_settings_get(o, "pair");
	fail_unless(value->type == CONN_SETTINGS_VALUE_INVALID);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_long_path_key_no_escape)
{
	ConnSettings *o;
	char *value;
	int ret;

	o = conn_settings_open(CONN_SETTINGS_SERVICE_TYPE, "my_service");
	fail_unless(o != NULL, "Setting not found");

	ret = conn_settings_get_string(o, "custom_ui/smiley/icon_name", &value);
	fail_unless(ret == CONN_SETTINGS_E_NO_ERROR, "get_string() failed, ret=%d, should be %d", ret, CONN_SETTINGS_E_NO_ERROR);
	fail_unless(strcmp("foobar.jpg", value) == 0);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_get_long_path_key_escape)
{
	ConnSettings *o;
	char *value;
	int ret;

	o = conn_settings_open(CONN_SETTINGS_SERVICE_TYPE, "my_service");
	fail_unless(o != NULL, "Setting not found");

	ret = conn_settings_get_string(o, "custom ui/smiley face/icon name", &value);
	fail_unless(ret == CONN_SETTINGS_E_NO_ERROR, "get_string() failed, ret=%d, should be %d", ret, CONN_SETTINGS_E_NO_ERROR);
	fail_unless(strcmp("foo bar.jpg", value) == 0);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_check_if_id_exists)
{
	int ret;

	ret = conn_settings_id_exists(CONN_SETTINGS_CONNECTION, NULL);
	fail_unless(!ret);

	ret = conn_settings_id_exists(CONN_SETTINGS_CONNECTION, "netti");
	fail_unless(ret);

	ret = conn_settings_id_exists(CONN_SETTINGS_CONNECTION, "missing");
	fail_unless(!ret);
}
END_TEST


/* ========================================================================= */
START_TEST(conn_settings_check_values_destroy)
{
	int st;
	ConnSettings *o;

	ConnSettingsValue **value = NULL;
	ConnSettingsValue *value2 = NULL;
	conn_settings_values_destroy(value);
	value2 = conn_settings_value_new();
	conn_settings_values_destroy(value2);

	value = NULL;
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	st = conn_settings_get_list(o, "int_list", &value);
	fail_unless(st == 0);
	conn_settings_close(o);
	conn_settings_values_destroy(value);

	value = NULL;
	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	st = conn_settings_get_list(o, "int_list_not_found", &value);
	fail_unless(st != 0);
	conn_settings_close(o);
	conn_settings_values_destroy(value);

	o = conn_settings_open(CONN_SETTINGS_CONNECTION, "netti");
	st = conn_settings_set_list(o, "foobar_empty", NULL, 2, CONN_SETTINGS_VALUE_STRING);
	fail_unless(st==CONN_SETTINGS_E_NO_ERROR, "Invalid return value %d, should be %d", st, CONN_SETTINGS_E_NO_ERROR);

	st = conn_settings_get_list(o, "foobar_empty", &value);
	fail_unless(value != NULL);
	fail_if(st, "Invalid return value, should be %d but was %d", 0, st);
	fail_if(*value, "Invalid value, the returned entry was not null");
	conn_settings_close(o);
	conn_settings_values_destroy(value);
}
END_TEST


/* ========================================================================= */
Suite *conn_settings_suite(void)
{
	Suite *s = suite_create("Connectivity Settings");

	/* Core test case */
	TCase *tc_empty_db = tcase_create("conn settings empty db");
	tcase_add_checked_fixture(tc_empty_db, setup_empty, teardown_empty);
	tcase_add_test(tc_empty_db, initially_no_iaps_in_gconf);
	suite_add_tcase(s, tc_empty_db);

	TCase *tc_list_ids = tcase_create("conn_settings_list_ids");
	tcase_add_checked_fixture(tc_list_ids, setup_db, teardown_db);
	tcase_add_test(tc_list_ids, conn_settings_list_ids_general);
	tcase_add_test(tc_list_ids, conn_settings_list_ids_network);
	tcase_add_test(tc_list_ids, conn_settings_list_ids_connection);
	tcase_add_test(tc_list_ids, conn_settings_list_ids_service);
	suite_add_tcase(s, tc_list_ids);

	TCase *tc_open_connection = tcase_create("conn_settings_open connection");
	tcase_add_checked_fixture(tc_open_connection, setup_db, teardown_db);
	tcase_add_test(tc_open_connection, conn_settings_open_fail);
	tcase_add_test(tc_open_connection, conn_settings_open_fail_connection);
	tcase_add_test(tc_open_connection, conn_settings_open_ok_connection);
	tcase_add_test(tc_open_connection, conn_settings_open_ok_connection_with_null_id);
	suite_add_tcase(s, tc_open_connection);

	TCase *tc_list_keys = tcase_create("conn_settings_list_keys");
	tcase_add_checked_fixture(tc_list_keys, setup_db, teardown_db);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_general);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_network);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_connection);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_service);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_general_ok);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_network_ok);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_connection_ok);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_service_ok);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_connection_null);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_network_null);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_service_null);
	tcase_add_test(tc_list_keys, conn_settings_list_keys_null);
	suite_add_tcase(s, tc_list_keys);

	TCase *tc_get = tcase_create("conn_settings_get");
	tcase_add_checked_fixture(tc_get, setup_db, teardown_db);
	tcase_add_test(tc_get, conn_settings_get_invalid);
	tcase_add_test(tc_get, conn_settings_get_ok_int);
	tcase_add_test(tc_get, conn_settings_get_ok_string);
	tcase_add_test(tc_get, conn_settings_get_ok_bool);
	tcase_add_test(tc_get, conn_settings_get_ok_double);
	tcase_add_test(tc_get, conn_settings_get_ok_byte_array);
	tcase_add_test(tc_get, conn_settings_get_ok_int_list);
	tcase_add_test(tc_get, conn_settings_get_ok_string_list);
	tcase_add_test(tc_get, conn_settings_get_ok_double_list);
	suite_add_tcase(s, tc_get);

	TCase *tc_getters = tcase_create("conn_settings_getters");
	tcase_add_checked_fixture(tc_getters, setup_db, teardown_db);
	tcase_add_test(tc_getters, conn_settings_get_string_test);
	tcase_add_test(tc_getters, conn_settings_get_int_test);
	tcase_add_test(tc_getters, conn_settings_get_double_test);
	tcase_add_test(tc_getters, conn_settings_get_bool_test);
	tcase_add_test(tc_getters, conn_settings_get_byte_array_test);
	tcase_add_test(tc_getters, conn_settings_get_list_test);
	suite_add_tcase(s, tc_getters);

	TCase *tc_set = tcase_create("conn_settings_set");
	tcase_add_checked_fixture(tc_set, setup_db, teardown_db);
	tcase_add_test(tc_set, conn_settings_set_invalid);
	tcase_add_test(tc_set, conn_settings_create_iap);
	tcase_add_test(tc_set, conn_settings_set_different_type);
	tcase_add_test(tc_set, conn_settings_set_ok_int);
	tcase_add_test(tc_set, conn_settings_set_ok_string);
	tcase_add_test(tc_set, conn_settings_set_ok_double);
	tcase_add_test(tc_set, conn_settings_set_ok_bool);
	tcase_add_test(tc_set, conn_settings_set_ok_byte_array);
	tcase_add_test(tc_set, conn_settings_set_ok_mixed_list_which_should_fail);
	tcase_add_test(tc_set, conn_settings_set_ok_int_list);
	tcase_add_test(tc_set, conn_settings_set_ok_string_list);
	tcase_add_test(tc_set, conn_settings_set_ok_double_list);
	suite_add_tcase(s, tc_set);

	TCase *tc_setters = tcase_create("conn_settings_setters");
	tcase_add_checked_fixture(tc_setters, setup_db, teardown_db);
	tcase_add_test(tc_setters, conn_settings_set_string_test);
	tcase_add_test(tc_setters, conn_settings_set_int_test);
	tcase_add_test(tc_setters, conn_settings_set_double_test);
	tcase_add_test(tc_setters, conn_settings_set_bool_test);
	tcase_add_test(tc_setters, conn_settings_set_byte_array_test);
	tcase_add_test(tc_setters, conn_settings_set_list_int_test);
	tcase_add_test(tc_setters, conn_settings_set_list_string_test);
	suite_add_tcase(s, tc_setters);

	TCase *tc_id_exists = tcase_create("conn_settings_id_exists");
	tcase_add_checked_fixture(tc_id_exists, setup_db, teardown_db);
	tcase_add_test(tc_id_exists, conn_settings_check_if_id_exists);
	suite_add_tcase(s, tc_id_exists);

	TCase *tc_unset = tcase_create("conn_settings_unset");
	tcase_add_checked_fixture(tc_unset, setup_db, teardown_db);
	tcase_add_test(tc_unset, conn_settings_unset_string_test);
	tcase_add_test(tc_unset, conn_settings_unset_int_test);
	tcase_add_test(tc_unset, conn_settings_unset_double_test);
	tcase_add_test(tc_unset, conn_settings_unset_bool_test);
	tcase_add_test(tc_unset, conn_settings_unset_byte_array_test);
	tcase_add_test(tc_unset, conn_settings_unset_list_test);
	suite_add_tcase(s, tc_unset);

	TCase *tc_remove = tcase_create("conn_settings_remove");
	tcase_add_checked_fixture(tc_remove, setup_db, teardown_db);
	tcase_add_test(tc_remove, conn_settings_remove_test);
	suite_add_tcase(s, tc_remove);

	TCase *tc_close = tcase_create("conn_settings_close");
	tcase_add_checked_fixture(tc_close, setup_db, teardown_db);
	tcase_add_test(tc_close, conn_settings_close_test);
	suite_add_tcase(s, tc_close);

	TCase *tc_notify = tcase_create("conn_settings_notify");
	tcase_set_timeout(tc_notify, 10);
	tcase_add_checked_fixture(tc_notify, setup_db, teardown_db);
	tcase_add_test(tc_notify, conn_settings_notify_test);
	tcase_add_test(tc_notify, conn_settings_notify_test_list);
	tcase_add_test(tc_notify, conn_settings_notify_test_remove_iap);
	suite_add_tcase(s, tc_notify);

	TCase *tc_invalid_data_type = tcase_create("conn_settings_invalid_data_type");
	tcase_add_checked_fixture(tc_invalid_data_type, setup_db, teardown_db);
	tcase_add_test(tc_invalid_data_type, conn_settings_get_invalid_data_type);
	suite_add_tcase(s, tc_invalid_data_type);

	TCase *tc_long_path = tcase_create("conn_settings_long_path");
	tcase_add_checked_fixture(tc_long_path, setup_db, teardown_db);
	tcase_add_test(tc_long_path, conn_settings_get_long_path_key_no_escape);
	tcase_add_test(tc_long_path, conn_settings_get_long_path_key_escape);
	suite_add_tcase(s, tc_long_path);

	TCase *tc_values_destroy = tcase_create("conn_settings_check_values_destroy");
	tcase_add_checked_fixture(tc_values_destroy, setup_db, teardown_db);
	tcase_add_test(tc_values_destroy, conn_settings_check_values_destroy);
	suite_add_tcase(s, tc_values_destroy);

	return s;
}


int main(void)
{
	int number_failed;
	Suite *s = conn_settings_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_NORMAL);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
