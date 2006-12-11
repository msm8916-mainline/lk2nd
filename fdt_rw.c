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

static int rw_check_header(struct fdt_header *fdt)
{
	int err;

	if ((err = _fdt_check_header(fdt)))
		return err;
	if (fdt_version(fdt) < 0x11)
		return FDT_ERR_BADVERSION;
	if (fdt_off_mem_rsvmap(fdt) < ALIGN(sizeof(*fdt), 8))
		return FDT_ERR_BADLAYOUT;
	if (fdt_off_dt_struct(fdt) <
	    (fdt_off_mem_rsvmap(fdt) + sizeof(struct fdt_reserve_entry)))
		return FDT_ERR_BADLAYOUT;
	if (fdt_off_dt_strings(fdt) <
	    (fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt)))
		return FDT_ERR_BADLAYOUT;
	if (fdt_totalsize(fdt) <
	    (fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)))
		return FDT_ERR_BADLAYOUT;
	return 0;
}

#define RW_OFFSET_CHECK_HEADER(fdt) \
	{ \
		int err; \
		if ((err = rw_check_header(fdt)) != 0) \
			return OFFSET_ERROR(err); \
	}

static inline int _blob_data_size(struct fdt_header *fdt)
{
	return fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
}

static int _blob_splice(struct fdt_header *fdt, void *p, int oldlen, int newlen)
{
	void *blob = fdt;
	void *end = blob + _blob_data_size(fdt);

	if (((p + oldlen) < p) || ((p + oldlen) > end))
		return FDT_ERR_BADOFFSET;
	if ((end - oldlen + newlen) > (blob + fdt_totalsize(fdt)))
		return FDT_ERR_NOSPACE;
	memmove(p + newlen, p + oldlen, end - p - oldlen);
	return 0;
}

static int _blob_splice_struct(struct fdt_header *fdt, void *p,
			       int oldlen, int newlen)
{
	int delta = newlen - oldlen;
	int err;

	if ((err = _blob_splice(fdt, p, oldlen, newlen)))
		return err;

	fdt->size_dt_struct = cpu_to_fdt32(fdt_size_dt_struct(fdt) + delta);
	fdt->off_dt_strings = cpu_to_fdt32(fdt_off_dt_strings(fdt) + delta);
	return 0;
}

static int _blob_splice_string(struct fdt_header *fdt, int newlen)
{
	void *blob = fdt;
	void *p = blob + fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt);
	int err;

	if ((err = _blob_splice(fdt, p, 0, newlen)))
		return err;

	fdt->size_dt_strings = cpu_to_fdt32(fdt_size_dt_strings(fdt) + newlen);
	return 0;
}

static int _find_add_string(struct fdt_header *fdt, const char *s)
{
	char *strtab = (char *)fdt + fdt_off_dt_strings(fdt);
	const char *p;
	char *new;
	int len = strlen(s) + 1;
	int err;

	p = _fdt_find_string(strtab, fdt_size_dt_strings(fdt), s);
	if (p)
		/* found it */
		return (p - strtab);

	new = strtab + fdt_size_dt_strings(fdt);
	err = _blob_splice_string(fdt, len);
	if (err)
		return -err;

	memcpy(new, s, len);
	return (new - strtab);
}

static struct fdt_property *_resize_property(struct fdt_header *fdt, int nodeoffset,
					     const char *name, int len)
{
	struct fdt_property *prop;
	int oldlen;
	int err;

	prop = fdt_get_property(fdt, nodeoffset, name, &oldlen);
	if ((err = fdt_ptr_error(prop)))
		return PTR_ERROR(err);

	if ((err = _blob_splice_struct(fdt, prop->data,
				       ALIGN(oldlen, FDT_TAGSIZE),
				       ALIGN(len, FDT_TAGSIZE))))
		return PTR_ERROR(err);

	prop->len = cpu_to_fdt32(len);
	return prop;
}

static struct fdt_property *_add_property(struct fdt_header *fdt, int nodeoffset,
					  const char *name, int len)
{
	uint32_t tag;
	struct fdt_property *prop;
	int proplen;
	int nextoffset;
	int namestroff;
	int err;

	tag = _fdt_next_tag(fdt, nodeoffset, &nextoffset);
	if (tag != FDT_BEGIN_NODE)
		return PTR_ERROR(FDT_ERR_BADOFFSET);

	namestroff = _find_add_string(fdt, name);
	if (namestroff < 0)
		return PTR_ERROR(-namestroff);

	prop = fdt_offset_ptr(fdt, nextoffset, 0);
	if ((err = fdt_ptr_error(prop)))
		return PTR_ERROR(err);

	proplen = sizeof(*prop) + ALIGN(len, FDT_TAGSIZE);

	err = _blob_splice_struct(fdt, prop, 0, proplen);
	if (err)
		return PTR_ERROR(err);

	prop->tag = cpu_to_fdt32(FDT_PROP);
	prop->nameoff = cpu_to_fdt32(namestroff);
	prop->len = cpu_to_fdt32(len);
	return prop;
}

int fdt_setprop(struct fdt_header *fdt, int nodeoffset, const char *name,
		const void *val, int len)
{
	struct fdt_property *prop;
	int err;

	if ((err = rw_check_header(fdt)))
		return err;

	prop = _resize_property(fdt, nodeoffset, name, len);
	err = fdt_ptr_error(prop);
	if (err == FDT_ERR_NOTFOUND) {
		prop = _add_property(fdt, nodeoffset, name, len);
		err = fdt_ptr_error(prop);
	}
	if (err)
		return err;

	memcpy(prop->data, val, len);
	return 0;
}

int fdt_delprop(struct fdt_header *fdt, int nodeoffset, const char *name)
{
	struct fdt_property *prop;
	int len, proplen;
	int err;

	RW_OFFSET_CHECK_HEADER(fdt);

	prop = fdt_get_property(fdt, nodeoffset, name, &len);
	if ((err = fdt_ptr_error(prop)))
		return err;

	proplen = sizeof(*prop) + ALIGN(len, FDT_TAGSIZE);
	return _blob_splice_struct(fdt, prop, proplen, 0);
}

int fdt_add_subnode_namelen(struct fdt_header *fdt, int parentoffset,
			    const char *name, int namelen)
{
	struct fdt_node_header *nh;
	int offset, nextoffset;
	int nodelen;
	int err;
	uint32_t tag;
	uint32_t *endtag;

	RW_OFFSET_CHECK_HEADER(fdt);

	offset = fdt_subnode_offset_namelen(fdt, parentoffset, name, namelen);
	if ((err = fdt_offset_error(offset)) == 0)
		return OFFSET_ERROR(FDT_ERR_EXISTS);
	else if (err != FDT_ERR_NOTFOUND)
		return OFFSET_ERROR(err);

	/* Try to place the new node after the parent's properties */
	_fdt_next_tag(fdt, parentoffset, &nextoffset); /* skip the BEGIN_NODE */
	do {
		offset = nextoffset;
		tag = _fdt_next_tag(fdt, offset, &nextoffset);
	} while (tag == FDT_PROP);

	nh = fdt_offset_ptr(fdt, offset, 0);
	if ((err = fdt_ptr_error(nh)))
		return OFFSET_ERROR(err);

	nodelen = sizeof(*nh) + ALIGN(namelen+1, FDT_TAGSIZE) + FDT_TAGSIZE;
	err = _blob_splice_struct(fdt, nh, 0, nodelen);
	if (err)
		return OFFSET_ERROR(err);

	nh->tag = cpu_to_fdt32(FDT_BEGIN_NODE);
	memset(nh->name, 0, ALIGN(namelen+1, FDT_TAGSIZE));
	memcpy(nh->name, name, namelen);
	endtag = (uint32_t *)((void *)nh + nodelen - FDT_TAGSIZE);
	*endtag = cpu_to_fdt32(FDT_END_NODE);

	return offset;
}

int fdt_add_subnode(struct fdt_header *fdt, int parentoffset, const char *name)
{
	return fdt_add_subnode_namelen(fdt, parentoffset, name, strlen(name));
}

int fdt_del_node(struct fdt_header *fdt, int nodeoffset)
{
	struct fdt_node_header *nh;
	int endoffset;
	int err;

	endoffset = _fdt_node_end_offset(fdt, nodeoffset);
	if ((err = fdt_offset_error(endoffset)))
		return err;

	nh = fdt_offset_ptr(fdt, nodeoffset, 0);
	if ((err = fdt_ptr_error(nh)))
		return err;

	return _blob_splice_struct(fdt, nh, endoffset - nodeoffset, 0);
}

struct fdt_header *fdt_open_into(struct fdt_header *fdt, void *buf, int bufsize)
{
	int err;

	fdt = fdt_move(fdt, buf, bufsize);
	if ((err = fdt_ptr_error(fdt)))
		return PTR_ERROR(err);

	fdt->totalsize = cpu_to_fdt32(bufsize);

	/* FIXME: re-order if necessary */

	err = rw_check_header(fdt);
	if (err)
		return PTR_ERROR(err);

	return fdt;
}

struct fdt_header *fdt_pack(struct fdt_header *fdt)
{
	int err;

	err = rw_check_header(fdt);
	if (err)
		return PTR_ERROR(err);

	/* FIXME: pack components */

	fdt->totalsize = cpu_to_fdt32(_blob_data_size(fdt));
	return fdt;
}
