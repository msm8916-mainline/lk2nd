#ifndef _LIBFDT_H
#define _LIBFDT_H
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

#include <fdt.h>
#include <libfdt_env.h>

#define FDT_FIRST_SUPPORTED_VERSION	0x10
#define FDT_LAST_SUPPORTED_VERSION	0x11

/* Errors */
#define FDT_ERR_OK		0
#define FDT_ERR_BADMAGIC	1
#define FDT_ERR_BADVERSION	2
#define FDT_ERR_BADPOINTER	3
#define FDT_ERR_BADHEADER	4
#define FDT_ERR_BADSTRUCTURE	5
#define FDT_ERR_BADOFFSET	6
#define FDT_ERR_NOTFOUND	7
#define FDT_ERR_BADPATH		8
#define FDT_ERR_TRUNCATED	9
#define FDT_ERR_NOSPACE		10
#define FDT_ERR_BADSTATE	11
#define FDT_ERR_SIZE_MISMATCH	12
#define FDT_ERR_INTERNAL	13
#define FDT_ERR_BADLAYOUT	14
#define FDT_ERR_EXISTS		15

#define FDT_ERR_MAX		14

#define fdt_magic(fdt)			(fdt32_to_cpu(fdt)->magic)
#define fdt_totalsize(fdt)		(fdt32_to_cpu(fdt)->totalsize)
#define fdt_off_dt_struct(fdt)		(fdt32_to_cpu(fdt)->off_dt_struct)
#define fdt_off_dt_strings(fdt)		(fdt32_to_cpu(fdt)->off_dt_strings)
#define fdt_off_mem_rsvmap(fdt)		(fdt32_to_cpu(fdt)->off_mem_rsvmap)
#define fdt_version(fdt)		(fdt32_to_cpu(fdt)->version)
#define fdt_last_comp_version(fdt)	(fdt32_to_cpu(fdt)->last_comp_version)
#define fdt_boot_cpuid_phys(fdt)	(fdt32_to_cpu(fdt)->boot_cpuid_phys)
#define fdt_size_dt_strings(fdt)	(fdt32_to_cpu(fdt)->size_dt_strings)
#define fdt_size_dt_struct(fdt)		(fdt32_to_cpu(fdt)->size_dt_struct)

void *fdt_offset_ptr(const struct fdt_header *fdt, int offset, int checklen);

#define fdt_offset_ptr_typed(fdt, offset, var) \
	((typeof(var))(fdt_offset_ptr((fdt), (offset), sizeof(*(var)))))

#define fdt_offset_error(offset) \
	( (offset) < 0 ? -(offset) : 0 )

#define fdt_ptr_error(ptr) \
	( (((long)(ptr) < 0) && ((long)(ptr) >= -FDT_ERR_MAX)) ? -(long)(ptr) : 0 )

struct fdt_header *fdt_move(const struct fdt_header *fdt, void *buf, int bufsize);

/* Read-only functions */
char *fdt_string(const struct fdt_header *fdt, int stroffset);

int fdt_property_offset(const struct fdt_header *fdt, int nodeoffset,
			const char *name);
int fdt_subnode_offset_namelen(const struct fdt_header *fdt, int parentoffset,
			       const char *name, int namelen);
int fdt_subnode_offset(const struct fdt_header *fdt, int parentoffset,
		       const char *name);

int fdt_path_offset(const struct fdt_header *fdt, const char *path);

void *fdt_getprop(const struct fdt_header *fdt, int nodeoffset,
		  const char *name, int *lenp);

/* Write-in-place functions */
int fdt_setprop_inplace(struct fdt_header *fdt, int nodeoffset, const char *name,
			const void *val, int len);

#define fdt_setprop_inplace_typed(fdt, nodeoffset, name, val) \
	({ \
		typeof(val) x = val; \
		fdt_setprop_inplace(fdt, nodeoffset, name, &x, sizeof(x)); \
	})

int fdt_nop_property(struct fdt_header *fdt, int nodeoffset, const char *name);
int fdt_nop_node(struct fdt_header *fdt, int nodeoffset);

/* Sequential-write functions */
struct fdt_header *fdt_create(void *buf, int bufsize);
int fdt_add_reservemap_entry(struct fdt_header *fdt, uint64_t addr, uint64_t size);
int fdt_finish_reservemap(struct fdt_header *fdt);
int fdt_begin_node(struct fdt_header *fdt, const char *name);
int fdt_property(struct fdt_header *fdt, const char *name, const void *val, int len);
#define fdt_property_typed(fdt, name, val) \
	({ \
		typeof(val) x = (val); \
		fdt_property((fdt), (name), &x, sizeof(x)); \
	})
#define fdt_property_string(fdt, name, str) \
	fdt_property(fdt, name, str, strlen(str)+1)
int fdt_end_node(struct fdt_header *fdt);
int fdt_finish(struct fdt_header *fdt);

/* Read-write functions */
struct fdt_header *fdt_open_into(struct fdt_header *fdt, void *buf, int bufsize);
struct fdt_header *fdt_pack(struct fdt_header *fdt);

int fdt_setprop(struct fdt_header *fdt, int nodeoffset, const char *name,
		const void *val, int len);
#define fdt_setprop_typed(fdt, nodeoffset, name, val) \
	({ \
		typeof(val) x = (val); \
		fdt_setprop((fdt), (nodeoffset), (name), &x, sizeof(x)); \
	})
#define fdt_setprop_string(fdt, nodeoffset, name, str) \
	fdt_setprop((fdt), (nodeoffset), (name), (str), strlen(str)+1)
int fdt_delprop(struct fdt_header *fdt, int nodeoffset, const char *name);
int fdt_add_subnode_namelen(struct fdt_header *fdt, int parentoffset,
			    const char *name, int namelen);
int fdt_add_subnode(struct fdt_header *fdt, int parentoffset, const char *name);
int fdt_del_node(struct fdt_header *fdt, int nodeoffset);

#endif /* _LIBFDT_H */
