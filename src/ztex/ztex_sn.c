/*
 * This software is Copyright (c) 2019 Denis Burykin
 * [denis_burykin yahoo com], [denis-burykin2014 yandex ru]
 * and it is hereby released to the general public under the following terms:
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ztex_sn.h"

#include "../config.h"
#include "../list.h"
#include "../misc.h"


int ztex_sn_is_valid(char *sn)
{
	int i;
	for (i = 0; i < ZTEX_SNSTRING_LEN; i++) {
		if (!sn[i])
			return i < ZTEX_SNSTRING_MIN_LEN ? 0 : 1;
		if ( !( (sn[i] >= '0' && sn[i] <= '9') || (sn[i] >= 'A' && sn[i] <= 'F')
				|| (sn[i] >= 'a' && sn[i] <= 'f')) )
			return 0;
	}
	if (sn[i])
		return 0;

	return 1;
}


struct cfg_list *ztex_sn_conf_devices = NULL;

void ztex_sn_init_conf_devices(void)
{
	if (ztex_sn_conf_devices)
		return;

	ztex_sn_conf_devices = cfg_get_list("List.ZTEX:", "Devices");
	if (!ztex_sn_conf_devices)
		return;

	struct cfg_line *line;
	int found_error = 0;
	for (line = ztex_sn_conf_devices->head; line; line = line->next) {
		if (!ztex_sn_is_valid(line->data)) {
			fprintf(stderr, "Error: [List.ZTEX:Devices] device %d "
				"(line %d) bad Serial Number '%s'\n",
				line->id + 1, line->number, line->data);
			found_error = 1;
		}
	}

	if (found_error)
		error();
}


int ztex_sn_check_alias(char *alias)
{
	static int ztex_sn_get_by_alias_error = 0;

	if (!ztex_sn_conf_devices) {
		if (!ztex_sn_get_by_alias_error) {
			fprintf(stderr, "Error: [List.ZTEX:Devices] not found\n");
			ztex_sn_get_by_alias_error = 1;
		}
		return 0;
	}

	int id = atoi(alias); // alias specified in -dev option
	if (!id) {
		fprintf(stderr, "Error: invalid -dev=0. Devices are numbered "
			"starting from 1.\n");
		return 0;
	}

	struct cfg_line *line;
	int last_id = 0;
	for (line = ztex_sn_conf_devices->head; line; line = line->next) {
		if (id == line->id + 1)
			return 1;//line->data;
		if (!line->next)
			last_id = line->id + 1;
	}

	fprintf(stderr, "Error: invalid -dev=%d. Total %d devices are "
		"defined in [List.ZTEX:Devices]\n", id, last_id);
	ztex_sn_get_by_alias_error = 1;
	return 0;
}


char *ztex_sn_get_by_sn_orig(char *sn_orig)
{
	if (!ztex_sn_conf_devices)
		return sn_orig;

	struct cfg_line *line;
	int found = 0;
	for (line = ztex_sn_conf_devices->head; line; line = line->next) {
		if (!strncmp(line->data, sn_orig, ZTEX_SNSTRING_LEN)) {
			found = 1;
			break;
		}
	}
	if (!found)
		return sn_orig;

	static char result[ZTEX_SNSTRING_LEN];
	sprintf(result, "%d", line->id + 1);
	return result;
}
