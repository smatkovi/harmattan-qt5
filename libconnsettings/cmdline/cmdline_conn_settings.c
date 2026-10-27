/**
  Command line tool for manipulating connectivity settings using
  libconnsettings library.

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

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <gconf/gconf-client.h>

#include "conn_settings.h"

void usage(char *argv0, char *msg)
{
	if (msg)
		fprintf(stderr, "%s\n\n", msg);
	fprintf(stderr, "Usage: %s [-g | -n | -c | -s] -d <directory> -k <key> [-p] [-t] [-l] [-u] [-i] [-e] -- <one or more values>\n", argv0);
	fprintf(stderr, "\t-g  general connectivity setting\n");
	fprintf(stderr, "\t-n  network type setting\n");
	fprintf(stderr, "\t-c  connection (IAP) setting\n");
	fprintf(stderr, "\t-s  service type setting\n");
	fprintf(stderr, "\t-k  key name\n");
	fprintf(stderr, "\t-p  put value (default is to fetch value from db)\n");
	fprintf(stderr, "\t-l  when printing a list, print elements in one line, by default list are printed one line/item. If -p option (put) is given, then parameter is a list type\n");
	fprintf(stderr, "\t-u  unset a key\n");
	fprintf(stderr, "\t-r  remove settings\n");
	fprintf(stderr, "\t-i  list ids for a given type\n");
	fprintf(stderr, "\t-e  list keys for a given type\n");
	fprintf(stderr, "\t-z  check whether given id exists, requires also -d and one of -g, -n, -c or -s options, return 0 if ok, >0 if not found\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Options -g, -n, -c and -s are mutually exclusive.\n");
	fprintf(stderr, "When -g option is given, then -d option must not be set.\n");
	fprintf(stderr, "Options -p, -u and -r are mutually exclusive.\n");
	fprintf(stderr, "The meaning of -d <directory> depends on given option like this:\n");
	fprintf(stderr, "\t-n  The -d option should be network type like WLAN or GPRS\n");
	fprintf(stderr, "\t-c  The -d option should be IAP id\n");
	fprintf(stderr, "\t-s  The -d option should be operator id\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "When putting entries (-p option), the type of the value must be given (-t option)\n");
	fprintf(stderr, "Following types can be given:\n");
	fprintf(stderr, "\tint     for integer\n");
	fprintf(stderr, "\tbool    for boolean\n");
	fprintf(stderr, "\tdouble  for double/float\n");
	fprintf(stderr, "\tstring  for string\n");
	fprintf(stderr, "List is created by giving list entries as extra parameters.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Example:\n");
	fprintf(stderr, "- get IAP foobar wlan ssid\n");
	fprintf(stderr, "\t%s -c -d foobar -k wlan_ssid\n", argv0);
	fprintf(stderr, "- put IAP foobar wlan ssid ABC\n");
	fprintf(stderr, "\t%s -c -d foobar -k wlan_ssid -p -t int -- 65 66 67\n", argv0);
	fprintf(stderr, "- put IAP foobar a string list (note that -l option is not needed because there is multiple elements so the type must be a list)\n");
	fprintf(stderr, "\t%s -c -d foobar -k \"string list\" -p -t string -- abc \"def ghi\" jkl mno\n", argv0);
	fprintf(stderr, "- set network type auto connect value (here -l option is needed so that the program knows the parameter is a list with only one element)\n");
	fprintf(stderr, "\t%s -n -d WLAN_INFRA -k network_type -p -t string -l -- '*'\n", argv0);
	fprintf(stderr, "- clear network type auto connect value\n");
	fprintf(stderr, "\t%s -n -k network_type -p -t string -l -- ''\n", argv0);
	fprintf(stderr, "  or\n");
	fprintf(stderr, "\t%s -n -k network_type -p -t string -l\n", argv0);
	fprintf(stderr, "\n");

	exit(1);
}


static ConnSettingsError print_value(ConnSettingsValue *value, int is_list, int one_liner)
{
	int i, st = 0;

	switch (value->type) {
	case CONN_SETTINGS_VALUE_INVALID:
		printf("%s", "");
		break;
	case CONN_SETTINGS_VALUE_STRING:
		printf("%s", value->value.string_val);
		break;
	case CONN_SETTINGS_VALUE_INT:
		printf("%d", value->value.int_val);
		break;
	case CONN_SETTINGS_VALUE_BOOL:
		printf("%d", value->value.bool_val);
		break;
	case CONN_SETTINGS_VALUE_DOUBLE:
		printf("%f", value->value.double_val);
		break;
	case CONN_SETTINGS_VALUE_LIST:
		if (one_liner)
			printf("[");
		else
			printf("[\n");
		for (i=0; value->value.list_val[i]; i++) {
			if (!one_liner && value->value.list_val[i])
				printf("\t");
			if (print_value(value->value.list_val[i], 1, one_liner))
				break;
			if (value->value.list_val[i+1]) {
				if (one_liner)
					printf(",");
				else
					printf("\n");
			}
		}
		if (one_liner)
			printf("]");
		else
			printf("\n]");
		break;
	default:
		fprintf(stderr, "Invalid value type (%d)", value->type);
		st = CONN_SETTINGS_E_INVALID_TYPE;
		break;
	}

	return st;
}


static ConnSettingsValueType convert_value_type(char *str)
{
	if (!strcasecmp(str, "int"))
		return CONN_SETTINGS_VALUE_INT;

	if (!strcasecmp(str, "string"))
		return CONN_SETTINGS_VALUE_STRING;

	if (!strcasecmp(str, "double"))
		return CONN_SETTINGS_VALUE_DOUBLE;

	if (!strcasecmp(str, "bool"))
		return CONN_SETTINGS_VALUE_BOOL;

	return CONN_SETTINGS_VALUE_INVALID;
}


int main(int argc, char **argv)
{
	int c, st = 0, i;
	char *dir = NULL, *key = NULL;
	ConnSettingsType type = CONN_SETTINGS_INVALID;
	ConnSettings *setting;
	ConnSettingsValue *value = NULL;
	int put_value = 0;
	char **values = NULL;
	int values_count = 0;
	ConnSettingsValueType value_type = CONN_SETTINGS_VALUE_INVALID;
	int one_liner = 0;
	char *endptr;
	int remove = 0, unset_key = 0, list_ids = 0, list_keys = 0;
	int check_id = 0, list_type = 0;

	while ((c=getopt(argc, argv, "cd:eghik:lnprst:uz"))>-1) {

		switch (c) {

		case 'c':
			type = CONN_SETTINGS_CONNECTION;
			break;

		case 'd':
			dir = optarg;
			break;

		case 'e':
			list_keys = 1;
			break;

		case 'g':
			type = CONN_SETTINGS_GENERAL;
			break;

		case 'i':
			list_ids = 1;
			break;

		case 'k':
			key = optarg;
			break;

		case 'l':
			one_liner = 1;
			list_type = 1;
			break;

		case 'n':
			type = CONN_SETTINGS_NETWORK_TYPE;
			break;

		case 'p':
			put_value = 1;
			break;

		case 'r':
			remove = 1;
			break;

		case 's':
			type = CONN_SETTINGS_SERVICE_TYPE;
			break;

		case 't':
			value_type = convert_value_type(optarg);
			break;

		case 'u':
			unset_key = 1;
			break;

		case 'z':
			check_id = 1;
			break;

		case 'h':
			usage(argv[0], 0);
			break;

		default:
			break;
		}
	}

	if (!type)
		usage(argv[0], "Type must be set, please use either -g, -n, -c or -s option");

	if (!key && !(list_ids || list_keys || remove || check_id))
		usage(argv[0], "Key not set, please use -k option");

	if (remove) {
		if (unset_key || put_value)
			usage(argv[0], "Only one of -p, -r or -u options can be given at a time.");
	}
	if (unset_key) {
		if (remove || put_value)
			usage(argv[0], "Only one of -p, -r or -u options can be given at a time.");
	}
	if (put_value) {
		if (remove || unset_key)
			usage(argv[0], "Only one of -p, -r or -u options can be given at a time.");
	}

	if (check_id) {
		if (!dir)
			usage(argv[0], "If -z option is used, then -d must be set.");

		return !conn_settings_id_exists(type, dir);
	}

	if (type == CONN_SETTINGS_GENERAL && dir != NULL)
		usage(argv[0], "If -g option is used, then -d must not be set.");

	if (optind < argc) {
		/* Rest of the arguments are the values */
		values_count = argc - optind;
		values = &argv[optind];
	}

	if (list_ids) {
		char **ids;
		int i;

		g_type_init();
		ids = conn_settings_list_ids(type);
		if (ids)
			for (i=0; ids[i]!=NULL; i++) {
				printf("%s\n", ids[i]);
				free(ids[i]);
			}
		free(ids);
		exit(0);
	} 

	setting = conn_settings_open(type, dir);
	if (!setting) {
		fprintf(stderr, "Cannot create context for type %d (%s)\n", type, dir);
		exit(CONN_SETTINGS_E_INVALID_CONTEXT);
	}

	if (list_keys) {

		char **keys;
		int i;

		keys = conn_settings_list_keys(setting);
		for (i=0; keys && keys[i]!=NULL; i++) {
			printf("%s\n", keys[i]);
			free(keys[i]);
		}
		free(keys);

	} else if (put_value) {
		if (value_type == CONN_SETTINGS_VALUE_INVALID) {
			fprintf(stderr, "Type of the value (-t option) is invalid, please use either int, string, double or bool\n");
			exit(CONN_SETTINGS_E_INVALID_TYPE);
		}

		if (values_count <= 1 && list_type) {
			if (values_count == 0 || strlen(values[0])==0) {
				/* User wishes to set the list to empty */
				st = conn_settings_set_list(setting, key, NULL, 0, value_type);
				goto EXIT;
			}
		}


		value = conn_settings_value_new();
		if (values_count>1 || list_type)
			value->type = CONN_SETTINGS_VALUE_LIST;

		ConnSettingsValue **elements = (ConnSettingsValue **)malloc(sizeof(void *) * (values_count + 1));
		elements[values_count] = NULL;
		memset(elements, 0, sizeof(void *) * values_count);

		if (values_count>1 || list_type)
			value->value.list_val = elements;

		for (i=0; i<values_count; i++) {
			elements[i] = conn_settings_value_new();
			elements[i]->type = value_type;

			switch (value_type) {
			case CONN_SETTINGS_VALUE_STRING:
				elements[i]->value.string_val = strdup(values[i]);
				break;

			case CONN_SETTINGS_VALUE_INT:
				endptr = NULL;
				elements[i]->value.int_val = strtol(values[i], &endptr, 10);
				if (!elements[i]->value.int_val && endptr && endptr[0]) {
					fprintf(stderr, "Non numeric value (%s), cannot convert.\n", values[i]);
					exit(CONN_SETTINGS_E_INVALID_PARAMETER);
				}
				break;

			case CONN_SETTINGS_VALUE_BOOL:
				if (!strcasecmp(values[i], "true"))
					elements[i]->value.bool_val = 1;
				else if (!strcasecmp(values[i], "false"))
					elements[i]->value.bool_val = 0;
				else
					elements[i]->value.bool_val = atoi(values[i]);
				break;
			case CONN_SETTINGS_VALUE_DOUBLE:
				elements[i]->value.double_val = atof(values[i]);
				break;
			default:
				/* should not happen */
				fprintf(stderr, "Invalid type\n");
				exit(CONN_SETTINGS_E_INVALID_TYPE);
				break;
			}
		}

		if (value->type != CONN_SETTINGS_VALUE_LIST) {
			conn_settings_value_destroy(value);
			value = elements[0];
		}

		st = conn_settings_set(setting, key, value);
		if (st) {
			fprintf(stderr, "Cannot set %s in %s (%s/%d)\n", key, dir, conn_settings_error_text(st), st);
			exit(st);
		}
		conn_settings_value_destroy(value);

	} else if (remove) {
		st = conn_settings_remove(setting);
		if (st) {
			fprintf(stderr, "Cannot remove %s (%s/%d)\n", dir, conn_settings_error_text(st), st);
			exit(st);
		}

	} else if (unset_key) {
		st = conn_settings_unset(setting, key);
		if (st) {
			fprintf(stderr, "Cannot unset %s in %s (%s/%d)\n", key, dir, conn_settings_error_text(st), st);
			exit(st);
		}

	} else {
		value = conn_settings_get(setting, key);
		if (!value) {
			fprintf(stderr, "Cannot get %s for %s\n", key, dir);
			exit(CONN_SETTINGS_E_GCONF_ERROR);
		}

		st = print_value(value, 0, one_liner);
		if (value->type)
			printf("\n");
		conn_settings_value_destroy(value);
	}

EXIT:
	conn_settings_close(setting);
	exit(st);
}
