/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <libfdt.h>

#include "util.h"

/* Holds information which controls our output and options */
struct display_info {
	int type;		/* data type (s/i/u/x or 0 for default) */
	int size;		/* data size (1/2/4) */
};

static void report_error(const char *where, int err)
{
	fprintf(stderr, "Error at '%s': %s\n", where, fdt_strerror(err));
}

/**
 * Displays data of a given length according to selected options
 *
 * If a specific data type is provided in disp, then this is used. Otherwise
 * we try to guess the data type / size from the contents.
 *
 * @param disp		Display information / options
 * @param data		Data to display
 * @param len		Maximum length of buffer
 * @return 0 if ok, -1 if data does not match format
 */
static int show_data(struct display_info *disp, const char *data, int len)
{
	int i, size;
	const uint8_t *p = (const uint8_t *)data;
	const char *s;
	int value;
	int is_string;
	char fmt[3];

	/* no data, don't print */
	if (len == 0)
		return 0;

	is_string = (disp->type) == 's' ||
		(!disp->type && util_is_printable_string(data, len));
	if (is_string) {
		if (data[len - 1] != '\0') {
			fprintf(stderr, "Unterminated string\n");
			return -1;
		}
		for (s = data; s - data < len; s += strlen(s) + 1) {
			if (s != data)
				printf(" ");
			printf("%s", (const char *)s);
		}
		return 0;
	}
	size = disp->size;
	if (size == -1) {
		size = (len % 4) == 0 ? 4 : 1;
	} else if (len % size) {
		fprintf(stderr, "Property length must be a multiple of "
				"selected data size\n");
		return -1;
	}
	fmt[0] = '%';
	fmt[1] = disp->type ? disp->type : 'd';
	fmt[2] = '\0';
	for (i = 0; i < len; i += size, p += size) {
		if (i)
			printf(" ");
		value = size == 4 ? fdt32_to_cpu(*(const uint32_t *)p) :
			size == 2 ? (*p << 8) | p[1] : *p;
		printf(fmt, value);
	}
	return 0;
}

/**
 * Show the data for a given node (and perhaps property) according to the
 * display option provided.
 *
 * @param blob		FDT blob
 * @param disp		Display information / options
 * @param node		Node to display
 * @param property	Name of property to display, or NULL if none
 * @return 0 if ok, -ve on error
 */
static int show_data_for_item(const void *blob, struct display_info *disp,
		int node, const char *property)
{
	const void *value = NULL;
	int len, err = 0;

	value = fdt_getprop(blob, node, property, &len);
	if (value) {
		if (show_data(disp, value, len))
			err = -1;
		else
			printf("\n");
	} else {
		report_error(property, len);
		err = -1;
	}
	return err;
}

/**
 * Run the main fdtget operation, given a filename and valid arguments
 *
 * @param disp		Display information / options
 * @param filename	Filename of blob file
 * @param arg		List of arguments to process
 * @param arg_count	Number of arguments
 * @param return 0 if ok, -ve on error
 */
static int do_fdtget(struct display_info *disp, const char *filename,
		     char **arg, int arg_count)
{
	char *blob;
	int i, node;

	blob = utilfdt_read(filename);
	if (!blob)
		return -1;

	for (i = 0; i + 2 <= arg_count; i += 2) {
		node = fdt_path_offset(blob, arg[i]);
		if (node < 0) {
			report_error(arg[i], node);
			return -1;
		}

		if (show_data_for_item(blob, disp, node, arg[i + 1]))
			return -1;
	}
	return 0;
}

static const char *usage_msg =
	"fdtget - read values from device tree\n"
	"\n"
	"Each value is printed on a new line.\n\n"
	"Usage:\n"
	"	fdtget <options> <dt file> [<node> <property>]...\n"
	"Options:\n"
	"\t-t <type>\tType of data\n"
	"\t-h\t\tPrint this help\n\n"
	USAGE_TYPE_MSG;

static void usage(const char *msg)
{
	if (msg)
		fprintf(stderr, "Error: %s\n\n", msg);

	fprintf(stderr, "%s", usage_msg);
	exit(2);
}

int main(int argc, char *argv[])
{
	char *filename = NULL;
	struct display_info disp;

	/* set defaults */
	memset(&disp, '\0', sizeof(disp));
	disp.size = -1;
	for (;;) {
		int c = getopt(argc, argv, "ht:");
		if (c == -1)
			break;

		switch (c) {
		case 'h':
		case '?':
			usage(NULL);

		case 't':
			if (utilfdt_decode_type(optarg, &disp.type,
					&disp.size))
				usage("Invalid type string");
			break;
		}
	}

	if (optind < argc)
		filename = argv[optind++];
	if (!filename)
		usage("Missing filename");

	argv += optind;
	argc -= optind;

	/* Allow no arguments, and silently succeed */
	if (!argc)
		return 0;

	/* Check for node, property arguments */
	if (argc % 2)
		usage("Must have an even number of arguments");

	if (do_fdtget(&disp, filename, argv, argc))
		return 1;
	return 0;
}
