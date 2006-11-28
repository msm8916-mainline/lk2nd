/*
 * libfdt - Flat Device Tree manipulation
 *	Testcase for behaviour on searching for a non-existent node
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fdt.h>
#include <libfdt.h>

#include "tests.h"
#include "testdata.h"

void check_error(const char *s, int err)
{
	if (err != FDT_ERR_NOTFOUND)
		FAIL("%s return error %s instead of FDT_ERR_NOTFOUND", s,
		     fdt_strerror(err));
}

int main(int argc, char *argv[])
{
	struct fdt_header *fdt;
	int offset;
	int subnode1_offset;
	void *val;
	int err;

	test_init(argc, argv);
	fdt = load_blob_arg(argc, argv);

	offset = fdt_property_offset(fdt, 0, "nonexistant-property");
	check_error("fdt_property_offset(\"nonexistant-property\")",
		    fdt_offset_error(offset));

	val = fdt_getprop(fdt, 0, "nonexistant-property", NULL);
	check_error("fdt_getprop(\"nonexistant-property\"",
		    fdt_ptr_error(val));

	subnode1_offset = fdt_subnode_offset(fdt, 0, "subnode1");
	if ((err = fdt_offset_error(subnode1_offset)))
		FAIL("Couldn't find subnode1: %s", fdt_strerror(err));

	val = fdt_getprop(fdt, subnode1_offset, "prop-str", NULL);
	check_error("fdt_getprop(\"prop-str\")", fdt_ptr_error(val));

	offset = fdt_subnode_offset(fdt, 0, "nonexistant-subnode");
	check_error("fdt_subnode_offset(\"nonexistant-subnode\")",
		    fdt_offset_error(offset));

	offset = fdt_subnode_offset(fdt, 0, "subsubnode");
	check_error("fdt_subnode_offset(\"subsubnode\")",
		    fdt_offset_error(offset));

	offset = fdt_path_offset(fdt, "/nonexistant-subnode");
	check_error("fdt_path_offset(\"/nonexistant-subnode\")",
		    fdt_offset_error(offset));
	
	PASS();
}
