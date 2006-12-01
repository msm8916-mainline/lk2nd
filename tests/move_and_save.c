/*
 * libfdt - Flat Device Tree manipulation
 *	Basic testcase for read-only access
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
#include <limits.h>
#include <fdt.h>
#include <libfdt.h>

#include "tests.h"
#include "testdata.h"

int main(int argc, char *argv[])
{
	struct fdt_header *fdt, *fdt1, *fdt2, *fdt3;
	void *buf;
	int shuntsize;
	int bufsize;
	int err;
	const char *inname;
	char outname[PATH_MAX];

	test_init(argc, argv);
	fdt = load_blob_arg(argc, argv);
	inname = argv[1];

	shuntsize = ALIGN(fdt_totalsize(fdt) / 2, sizeof(uint64_t));
	bufsize = fdt_totalsize(fdt) + shuntsize;
	buf = xmalloc(bufsize);

	fdt1 = fdt_move(fdt, buf, bufsize);
	if ((err = fdt_ptr_error(fdt1)))
		FAIL("Failed to move tree into new buffer: %s",
		     fdt_strerror(err));
	sprintf(outname, "moved.%s", inname);
	save_blob(outname, fdt1);

	fdt2 = fdt_move(fdt1, buf + shuntsize, bufsize-shuntsize);
	if ((err = fdt_ptr_error(fdt2)))
		FAIL("Failed to shunt tree %d bytes: %s",
		     shuntsize, fdt_strerror(err));
	sprintf(outname, "shunted.%s", inname);
	save_blob(outname, fdt2);

	fdt3 = fdt_move(fdt2, buf, bufsize);
	if ((err = fdt_ptr_error(fdt3)))
		FAIL("Failed to deshunt tree %d bytes: %s",
		     shuntsize, fdt_strerror(err));
	sprintf(outname, "deshunted.%s", inname);
	save_blob(outname, fdt3);

	PASS();
}
