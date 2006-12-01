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

#define PTR_CHECK(ptr, code) \
	{ \
		err = fdt_ptr_error((ptr) = (code)); \
		if (err) \
			FAIL(#code ": %s", fdt_strerror(err)); \
	}

#define OFF_CHECK(off, code) \
	{ \
		err = fdt_offset_error((off) = (code)); \
		if (err) \
			FAIL(#code ": %s", fdt_strerror(err)); \
	}

int main(int argc, char *argv[])
{
	void *buf;
	struct fdt_header *fdt;
	int err;
	int offset;

	test_init(argc, argv);

	buf = xmalloc(SPACE);

	/* First create empty tree with SW */
	fdt = fdt_create(buf, SPACE);

	CHECK(fdt_finish_reservemap(fdt));
	CHECK(fdt_begin_node(fdt, ""));
	CHECK(fdt_end_node(fdt));
	CHECK(fdt_finish(fdt));

	verbose_printf("Built empty tree, totalsize = %d\n",
		       fdt32_to_cpu(fdt->totalsize));

	PTR_CHECK(fdt, fdt_open_into(fdt, buf, SPACE));

	CHECK(fdt_setprop_typed(fdt, 0, "prop-int", TEST_VALUE_1));
	CHECK(fdt_setprop_string(fdt, 0, "prop-str", TEST_STRING_1));

	OFF_CHECK(offset, fdt_add_subnode(fdt, 0, "subnode1"));
	CHECK(fdt_setprop_typed(fdt, offset, "prop-int", TEST_VALUE_1));
	OFF_CHECK(offset, fdt_add_subnode(fdt, offset, "subsubnode"));
	CHECK(fdt_setprop_typed(fdt, offset, "prop-int", TEST_VALUE_1));

	OFF_CHECK(offset, fdt_add_subnode(fdt, 0, "subnode2"));
	CHECK(fdt_setprop_typed(fdt, offset, "prop-int", TEST_VALUE_2));
	OFF_CHECK(offset, fdt_add_subnode(fdt, offset, "subsubnode"));

	CHECK(fdt_setprop_typed(fdt, offset, "prop-int", TEST_VALUE_2));

	PTR_CHECK(fdt, fdt_pack(fdt));

	save_blob("rw_tree1.test.dtb", fdt);

	PASS();
}
