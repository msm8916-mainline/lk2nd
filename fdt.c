/*
 * libfdt - Flat Device Tree manipulation
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
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

void *fdt_offset_ptr(const struct fdt_header *fdt, int offset, int len)
{
	void *p;

	p = (void *)fdt + fdt32_to_cpu(fdt->off_dt_struct) + offset;

	if (p + len < p)
		return NULL;
	return p;
}

char *fdt_string(const struct fdt_header *fdt, int stroffset)
{
	return (char *)fdt + fdt32_to_cpu(fdt->off_dt_strings) + stroffset;
}

int fdt_string_cmp(const struct fdt_header *fdt, int stroffset, const char *s2)
{
	const char *s1 = fdt_string(fdt, stroffset);
	int len = strlen(s2) + 1;

	if (! s1)
		return 0;

	if ((stroffset + len < stroffset)
	    || (stroffset + len > fdt32_to_cpu(fdt->size_dt_strings)))
		return -2;

	return strcmp(s1, s2);
}

uint32_t _fdt_next_tag(const struct fdt_header *fdt, int offset, int *nextoffset)
{
	const uint32_t *tagp, *lenp;
	uint32_t tag;
	const char *p;

	if (offset % FDT_TAGSIZE)
		return -1;

	tagp = fdt_offset_ptr(fdt, offset, FDT_TAGSIZE);
	if (! tagp)
		return FDT_END; /* premature end */
	tag = fdt32_to_cpu(*tagp);
	offset += FDT_TAGSIZE;

	switch (tag) {
	case FDT_BEGIN_NODE:
		/* skip name */
		do {
			p = fdt_offset_ptr(fdt, offset++, 1);
		} while (p && (*p != '\0'));
		if (! p)
			return FDT_END;
		break;
	case FDT_PROP:
		lenp = fdt_offset_ptr(fdt, offset + FDT_TAGSIZE, sizeof(*lenp));
		if (! lenp)
			return FDT_END;
		/* skip name offset, length and value */
		offset += 2*FDT_TAGSIZE + fdt32_to_cpu(*lenp);
		break;
	}

	if (nextoffset)
		*nextoffset = ALIGN(offset, FDT_TAGSIZE);

	return tag;
}
