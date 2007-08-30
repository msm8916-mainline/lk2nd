/*
 * libfdt - Flat Device Tree manipulation
 * Copyright (C) 2006 David Gibson, IBM Corporation.
 *
 * libfdt is dual licensed: you can use it either under the terms of
 * the GPL, or the BSD license, at your option.
 *
 *  a) This library is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation; either version 2 of the
 *     License, or (at your option) any later version.
 *
 *     This library is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public
 *     License along with this library; if not, write to the Free
 *     Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 *     MA 02110-1301 USA
 *
 * Alternatively,
 *
 *  b) Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *     1. Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *     2. Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 *     CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 *     INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *     MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *     DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 *     CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *     SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 *     NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 *     HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *     CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *     OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *     EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "libfdt_env.h"

#include <fdt.h>
#include <libfdt.h>

#include "libfdt_internal.h"

#define CHECK_HEADER(fdt) \
	{ \
		int err; \
		if ((err = _fdt_check_header(fdt)) != 0) \
			return err; \
	}

static int offset_streq(const void *fdt, int offset,
			const char *s, int len)
{
	const char *p = fdt_offset_ptr(fdt, offset, len+1);

	if (! p)
		/* short match */
		return 0;

	if (memcmp(p, s, len) != 0)
		return 0;

	if (p[len] != '\0')
		return 0;

	return 1;
}

char *fdt_string(const void *fdt, int stroffset)
{
	return (char *)fdt + fdt_off_dt_strings(fdt) + stroffset;
}

int fdt_subnode_offset_namelen(const void *fdt, int parentoffset,
			       const char *name, int namelen)
{
	int level = 0;
	uint32_t tag;
	int offset, nextoffset;

	CHECK_HEADER(fdt);

	tag = _fdt_next_tag(fdt, parentoffset, &nextoffset);
	if (tag != FDT_BEGIN_NODE)
		return -FDT_ERR_BADOFFSET;

	do {
		offset = nextoffset;
		tag = _fdt_next_tag(fdt, offset, &nextoffset);

		switch (tag) {
		case FDT_END:
			return -FDT_ERR_TRUNCATED;

		case FDT_BEGIN_NODE:
			level++;
			if (level != 1)
				continue;
			if (offset_streq(fdt, offset+FDT_TAGSIZE, name, namelen))
				/* Found it! */
				return offset;
			break;

		case FDT_END_NODE:
			level--;
			break;

		case FDT_PROP:
		case FDT_NOP:
			break;

		default:
			return -FDT_ERR_BADSTRUCTURE;
		}
	} while (level >= 0);

	return -FDT_ERR_NOTFOUND;
}

int fdt_subnode_offset(const void *fdt, int parentoffset,
		       const char *name)
{
	return fdt_subnode_offset_namelen(fdt, parentoffset, name, strlen(name));
}

int fdt_path_offset(const void *fdt, const char *path)
{
	const char *end = path + strlen(path);
	const char *p = path;
	int offset = 0;

	CHECK_HEADER(fdt);

	if (*path != '/')
		return -FDT_ERR_BADPATH;

	while (*p) {
		const char *q;

		while (*p == '/')
			p++;
		if (! *p)
			return offset;
		q = strchr(p, '/');
		if (! q)
			q = end;

		offset = fdt_subnode_offset_namelen(fdt, offset, p, q-p);
		if (offset < 0)
			return offset;

		p = q;
	}

	return offset;	
}

const char *fdt_get_name(const void *fdt, int nodeoffset, int *len)
{
	const struct fdt_node_header *nh;
	int err;

	if ((err = _fdt_check_header(fdt)) != 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	nh = fdt_offset_ptr(fdt, nodeoffset, sizeof(*nh));
	if (!nh || (fdt32_to_cpu(nh->tag) != FDT_BEGIN_NODE))
		goto fail;

	if (len)
		*len = strlen(nh->name);

	return nh->name;

 fail:
	if (len)
		*len = err;
	return NULL;
}

const struct fdt_property *fdt_get_property(const void *fdt,
					    int nodeoffset,
					    const char *name, int *lenp)
{
	int level = 0;
	uint32_t tag;
	const struct fdt_property *prop;
	int namestroff;
	int offset, nextoffset;
	int err;

	if ((err = _fdt_check_header(fdt)) != 0)
		goto fail;

	err = -FDT_ERR_BADOFFSET;
	if (nodeoffset % FDT_TAGSIZE)
		goto fail;

	tag = _fdt_next_tag(fdt, nodeoffset, &nextoffset);
	if (tag != FDT_BEGIN_NODE)
		goto fail;

	do {
		offset = nextoffset;

		tag = _fdt_next_tag(fdt, offset, &nextoffset);
		switch (tag) {
		case FDT_END:
			err = -FDT_ERR_TRUNCATED;
			goto fail;

		case FDT_BEGIN_NODE:
			level++;
			break;

		case FDT_END_NODE:
			level--;
			break;

		case FDT_PROP:
			if (level != 0)
				continue;

			err = -FDT_ERR_BADSTRUCTURE;
			prop = fdt_offset_ptr_typed(fdt, offset, prop);
			if (! prop)
				goto fail;
			namestroff = fdt32_to_cpu(prop->nameoff);
			if (streq(fdt_string(fdt, namestroff), name)) {
				/* Found it! */
				int len = fdt32_to_cpu(prop->len);
				prop = fdt_offset_ptr(fdt, offset,
						      sizeof(*prop)+len);
				if (! prop)
					goto fail;

				if (lenp)
					*lenp = len;
				
				return prop;
			}
			break;

		case FDT_NOP:
			break;

		default:
			err = -FDT_ERR_BADSTRUCTURE;
			goto fail;
		}
	} while (level >= 0);

	err = -FDT_ERR_NOTFOUND;
 fail:
	if (lenp)
		*lenp = err;
	return NULL;
}

const void *fdt_getprop(const void *fdt, int nodeoffset,
		  const char *name, int *lenp)
{
	const struct fdt_property *prop;

	prop = fdt_get_property(fdt, nodeoffset, name, lenp);
	if (! prop)
		return NULL;

	return prop->data;
}
