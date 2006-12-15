/*
 * libfdt - Flat Device Tree manipulation
 *	Testcase for fdt_nop_node()
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
#include <ctype.h>

#include <fdt.h>
#include <libfdt.h>

#include "tests.h"
#include "testdata.h"

#define SPACE	65536

#define CHECK(code) \
	{ \
		err = (code); \
		if (err) \
			FAIL(#code ": %s", fdt_strerror(err)); \
	}

int main(int argc, char *argv[])
{
	void *fdt;
	int err;

	test_init(argc, argv);

	fdt = xmalloc(SPACE);
	CHECK(fdt_create(fdt, SPACE));

	CHECK(fdt_finish_reservemap(fdt));
	CHECK(fdt_begin_node(fdt, ""));
	CHECK(fdt_property_typed(fdt, "prop-int", TEST_VALUE_1));
	CHECK(fdt_property_string(fdt, "prop-str", TEST_STRING_1));

	CHECK(fdt_begin_node(fdt, "subnode1"));
	CHECK(fdt_property_typed(fdt, "prop-int", TEST_VALUE_1));
	CHECK(fdt_begin_node(fdt, "subsubnode"));
	CHECK(fdt_property_typed(fdt, "prop-int", TEST_VALUE_1));
	CHECK(fdt_end_node(fdt));
	CHECK(fdt_end_node(fdt));

	CHECK(fdt_begin_node(fdt, "subnode2"));
	CHECK(fdt_property_typed(fdt, "prop-int", TEST_VALUE_2));
	CHECK(fdt_begin_node(fdt, "subsubnode"));
	CHECK(fdt_property_typed(fdt, "prop-int", TEST_VALUE_2));
	CHECK(fdt_end_node(fdt));
	CHECK(fdt_end_node(fdt));

	CHECK(fdt_end_node(fdt));

	save_blob("unfinished_tree1.test.dtb", fdt);

	CHECK(fdt_finish(fdt));

	verbose_printf("Completed tree, totalsize = %d\n",
		       fdt_totalsize(fdt));

	save_blob("sw_tree1.test.dtb", fdt);

	PASS();
}
