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

static int check_header_sw(struct fdt_header *fdt)
{
	if (fdt32_to_cpu(fdt->magic) != SW_MAGIC)
		return FDT_ERR_BADMAGIC;
	return 0;
}

static void *grab_space(struct fdt_header *fdt, int len)
{
	int off_dt_struct = fdt32_to_cpu(fdt->off_dt_struct);
	int offset = fdt32_to_cpu(SW_SIZE_DT_STRUCT(fdt));
	int spaceleft;

	spaceleft = fdt32_to_cpu(fdt->totalsize) - off_dt_struct
		- fdt32_to_cpu(fdt->size_dt_strings);

	if ((offset + len < offset) || (offset + len > spaceleft))
		return NULL;

	SW_SIZE_DT_STRUCT(fdt) = cpu_to_fdt32(offset + len);
	return fdt_offset_ptr(fdt, offset, len);
}

struct fdt_header *fdt_create(void *buf, int bufsize)
{
	struct fdt_header *fdt = buf;

	if (bufsize < sizeof(struct fdt_header))
		return NULL;

	memset(buf, 0, bufsize);

	fdt->magic = cpu_to_fdt32(SW_MAGIC);
	fdt->totalsize = cpu_to_fdt32(bufsize);

	fdt->off_mem_rsvmap = cpu_to_fdt32(ALIGN(sizeof(*fdt),
						 sizeof(struct fdt_reserve_entry)));
	fdt->off_dt_struct = fdt->off_mem_rsvmap;
	fdt->off_dt_strings = fdt32_to_cpu(bufsize);

	return fdt;
}

int fdt_add_reservemap_entry(struct fdt_header *fdt, uint64_t addr, uint64_t size)
{
	struct fdt_reserve_entry *re;
	int err = check_header_sw(fdt);
	int offset;

	if (err)
		return err;
	if (SW_SIZE_DT_STRUCT(fdt))
		return FDT_ERR_BADSTATE;

	offset = fdt32_to_cpu(fdt->off_dt_struct);
	if ((offset + sizeof(*re)) > fdt32_to_cpu(fdt->totalsize))
		return FDT_ERR_NOSPACE;

	re = (struct fdt_reserve_entry *)((void *)fdt + offset);
	re->address = cpu_to_fdt64(addr);
	re->size = cpu_to_fdt64(size);

	fdt->off_dt_struct = cpu_to_fdt32(offset + sizeof(*re));

	return 0;
}

int fdt_finish_reservemap(struct fdt_header *fdt)
{
	return fdt_add_reservemap_entry(fdt, 0, 0);
}

int fdt_begin_node(struct fdt_header *fdt, const char *name)
{
	struct fdt_node_header *nh;
	int err = check_header_sw(fdt);
	int namelen = strlen(name) + 1;

	if (err)
		return err;

	nh = grab_space(fdt, sizeof(*nh) + ALIGN(namelen, FDT_TAGSIZE));
	if (! nh)
		return FDT_ERR_NOSPACE;

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memcpy(nh->name, name, namelen);
	return 0;
}

int fdt_end_node(struct fdt_header *fdt)
{
	uint32_t *en;
	int err = check_header_sw(fdt);

	if (err)
		return err;

	en = grab_space(fdt, FDT_TAGSIZE);
	if (! en)
		return FDT_ERR_NOSPACE;

	*en = cpu_to_fdt32(FDT_END_NODE);
	return 0;
}

static int find_add_string(struct fdt_header *fdt, const char *s)
{
	int totalsize = fdt32_to_cpu(fdt->totalsize);
	char *strtab = (char *)fdt + totalsize;
	int strtabsize = fdt32_to_cpu(fdt->size_dt_strings);
	int len = strlen(s) + 1;
	int struct_top, offset;

	/* We treat string offsets as negative from the end of our buffer */
	/* then fix them up in fdt_finish() */
	offset = -strtabsize;
	while ((offset < 0) && (memcmp(strtab + offset, s, len) != 0))
		offset++;

	if (offset < 0)
		/* Found it */
		return offset;

	/* Add it */
	offset = -strtabsize - len;
	struct_top = fdt32_to_cpu(fdt->off_dt_struct)
		+ fdt32_to_cpu(SW_SIZE_DT_STRUCT(fdt));
	if (totalsize + offset < struct_top)
		return 0; /* no more room :( */

	memcpy(strtab + offset, s, len);
	fdt->size_dt_strings = cpu_to_fdt32(strtabsize + len);
	return offset;
}

int fdt_property(struct fdt_header *fdt, const char *name, const void *val, int len)
{
	struct fdt_property *prop;
	int err = check_header_sw(fdt);
	int nameoff;

	if (err)
		return err;

	nameoff = find_add_string(fdt, name);
	if (nameoff == 0)
		return FDT_ERR_NOSPACE;

	prop = grab_space(fdt, sizeof(*prop) + ALIGN(len, FDT_TAGSIZE));
	if (! prop)
		return FDT_ERR_NOSPACE;

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(nameoff);
	prop->len = cpu_to_fdt32(len);
	memcpy(prop->data, val, len);
	return 0;
}

int fdt_finish(struct fdt_header *fdt)
{
	int err = check_header_sw(fdt);
	char *p = (char *)fdt;
	uint32_t *end;
	int oldstroffset, newstroffset, strsize;
	uint32_t tag;
	int offset, nextoffset;

	if (err)
		return err;

	/* Add terminator */
	end = grab_space(fdt, sizeof(*end));
	if (! end)
		return FDT_ERR_NOSPACE;
	*end = cpu_to_fdt32(FDT_END);

	/* Relocate the string table */
	strsize = fdt32_to_cpu(fdt->size_dt_strings);
	oldstroffset = fdt32_to_cpu(fdt->totalsize) - strsize;
	newstroffset = fdt32_to_cpu(fdt->off_dt_struct)
		+ fdt32_to_cpu(SW_SIZE_DT_STRUCT(fdt));
	memmove(p + newstroffset, p + oldstroffset, strsize);
	fdt->off_dt_strings = fdt32_to_cpu(newstroffset);

	/* Walk the structure, correcting string offsets */
	offset = 0;
	while ((tag = _fdt_next_tag(fdt, offset, &nextoffset)) != FDT_END) {
		if (tag == FDT_PROP) {
			struct fdt_property *prop = fdt_offset_ptr(fdt, offset,
								   sizeof(*prop));
			int nameoff;

			if (! prop)
				return FDT_ERR_BADSTRUCTURE;

			nameoff = fdt32_to_cpu(prop->nameoff);
			nameoff += strsize;
			prop->nameoff = cpu_to_fdt32(nameoff);
		}
		offset = nextoffset;
	}

	/* Finally, adjust the header */
	fdt->totalsize = cpu_to_fdt32(newstroffset + strsize);
	fdt->version = cpu_to_fdt32(FDT_LAST_SUPPORTED_VERSION);
	fdt->last_comp_version= cpu_to_fdt32(FDT_FIRST_SUPPORTED_VERSION);
	fdt->magic = cpu_to_fdt32(FDT_MAGIC);
	return 0;
}
