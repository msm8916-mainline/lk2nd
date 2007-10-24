#ifndef _LIBFDT_H
#define _LIBFDT_H
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

#include <libfdt_env.h>
#include <fdt.h>

#define FDT_FIRST_SUPPORTED_VERSION	0x10
#define FDT_LAST_SUPPORTED_VERSION	0x11

/* Error codes: informative error codes */
#define FDT_ERR_NOTFOUND	1
#define FDT_ERR_EXISTS		2
#define FDT_ERR_NOSPACE		3

/* Error codes: codes for bad parameters */
#define FDT_ERR_BADOFFSET	4
#define FDT_ERR_BADPATH		5
#define FDT_ERR_BADSTATE	6

/* Error codes: codes for bad device tree blobs */
#define FDT_ERR_TRUNCATED	7
#define FDT_ERR_BADMAGIC	8
#define FDT_ERR_BADVERSION	9
#define FDT_ERR_BADSTRUCTURE	10
#define FDT_ERR_BADLAYOUT	11

/* "Can't happen" error indicating a bug in libfdt */
#define FDT_ERR_INTERNAL	12

#define FDT_ERR_MAX		12

/* Low-level functions (you probably don't need these) */

const void *fdt_offset_ptr(const void *fdt, int offset, int checklen);
static inline void *fdt_offset_ptr_w(void *fdt, int offset, int checklen)
{
	return (void *)fdt_offset_ptr(fdt, offset, checklen);
}


#define fdt_offset_ptr_typed(fdt, offset, var) \
	((typeof(var))(fdt_offset_ptr((fdt), (offset), sizeof(*(var)))))
#define fdt_offset_ptr_typed_w(fdt, offset, var) \
	((typeof(var))(fdt_offset_ptr_w((fdt), (offset), sizeof(*(var)))))

uint32_t fdt_next_tag(const void *fdt, int offset, int *nextoffset);

/* General functions */
#define fdt_get_header(fdt, field) \
	(fdt32_to_cpu(((const struct fdt_header *)(fdt))->field))
#define fdt_magic(fdt) 			(fdt_get_header(fdt, magic))
#define fdt_totalsize(fdt)		(fdt_get_header(fdt, totalsize))
#define fdt_off_dt_struct(fdt)		(fdt_get_header(fdt, off_dt_struct))
#define fdt_off_dt_strings(fdt)		(fdt_get_header(fdt, off_dt_strings))
#define fdt_off_mem_rsvmap(fdt)		(fdt_get_header(fdt, off_mem_rsvmap))
#define fdt_version(fdt)		(fdt_get_header(fdt, version))
#define fdt_last_comp_version(fdt) 	(fdt_get_header(fdt, last_comp_version))
#define fdt_boot_cpuid_phys(fdt) 	(fdt_get_header(fdt, boot_cpuid_phys))
#define fdt_size_dt_strings(fdt) 	(fdt_get_header(fdt, size_dt_strings))
#define fdt_size_dt_struct(fdt)		(fdt_get_header(fdt, size_dt_struct))

#define fdt_set_header(fdt, field, val) \
	((struct fdt_header *)(fdt))->field = cpu_to_fdt32(val)

int fdt_check_header(const void *fdt);
int fdt_move(const void *fdt, void *buf, int bufsize);

/* Read-only functions */
const char *fdt_string(const void *fdt, int stroffset);

int fdt_get_mem_rsv(const void *fdt, int n, uint64_t *address, uint64_t *size);
int fdt_num_mem_rsv(const void *fdt);

int fdt_subnode_offset_namelen(const void *fdt, int parentoffset,
			       const char *name, int namelen);
int fdt_subnode_offset(const void *fdt, int parentoffset, const char *name);

int fdt_path_offset(const void *fdt, const char *path);

const char *fdt_get_name(const void *fdt, int nodeoffset, int *baselen);

const struct fdt_property *fdt_get_property(const void *fdt, int nodeoffset,
					    const char *name, int *lenp);

static inline struct fdt_property *fdt_get_property_w(void *fdt, int nodeoffset,
						      const char *name,
						      int *lenp)
{
	return (struct fdt_property *)fdt_get_property(fdt, nodeoffset,
						       name, lenp);
}

const void *fdt_getprop(const void *fdt, int nodeoffset,
			const char *name, int *lenp);
static inline void *fdt_getprop_w(void *fdt, int nodeoffset,
				  const char *name, int *lenp)
{
	return (void *)fdt_getprop(fdt, nodeoffset, name, lenp);
}

int fdt_get_path(const void *fdt, int nodeoffset, char *buf, int buflen);

int fdt_supernode_atdepth_offset(const void *fdt, int nodeoffset,
				 int supernodedepth, int *nodedepth);
int fdt_node_depth(const void *fdt, int nodeoffset);
int fdt_parent_offset(const void *fdt, int nodeoffset);

int fdt_node_offset_by_prop_value(const void *fdt, int startoffset,
				  const char *propname,
				  const void *propval, int proplen);

int fdt_node_check_compatible(const void *fdt, int nodeoffset,
			      const char *compatible);
int fdt_node_offset_by_compatible(const void *fdt, int startoffset,
				  const char *compatible);

/* Write-in-place functions */
int fdt_setprop_inplace(void *fdt, int nodeoffset, const char *name,
			const void *val, int len);

#define fdt_setprop_inplace_typed(fdt, nodeoffset, name, val) \
	({ \
		typeof(val) x = val; \
		fdt_setprop_inplace(fdt, nodeoffset, name, &x, sizeof(x)); \
	})

int fdt_nop_property(void *fdt, int nodeoffset, const char *name);
int fdt_nop_node(void *fdt, int nodeoffset);

/* Sequential-write functions */
int fdt_create(void *buf, int bufsize);
int fdt_add_reservemap_entry(void *fdt, uint64_t addr, uint64_t size);
int fdt_finish_reservemap(void *fdt);
int fdt_begin_node(void *fdt, const char *name);
int fdt_property(void *fdt, const char *name, const void *val, int len);
#define fdt_property_typed(fdt, name, val) \
	({ \
		typeof(val) x = (val); \
		fdt_property((fdt), (name), &x, sizeof(x)); \
	})
#define fdt_property_string(fdt, name, str) \
	fdt_property(fdt, name, str, strlen(str)+1)
int fdt_end_node(void *fdt);
int fdt_finish(void *fdt);

/* Read-write functions */
int fdt_open_into(void *fdt, void *buf, int bufsize);
int fdt_pack(void *fdt);

int fdt_add_mem_rsv(void *fdt, uint64_t address, uint64_t size);
int fdt_del_mem_rsv(void *fdt, int n);

int fdt_setprop(void *fdt, int nodeoffset, const char *name,
		const void *val, int len);
#define fdt_setprop_typed(fdt, nodeoffset, name, val) \
	({ \
		typeof(val) x = (val); \
		fdt_setprop((fdt), (nodeoffset), (name), &x, sizeof(x)); \
	})
#define fdt_setprop_string(fdt, nodeoffset, name, str) \
	fdt_setprop((fdt), (nodeoffset), (name), (str), strlen(str)+1)
int fdt_delprop(void *fdt, int nodeoffset, const char *name);
int fdt_add_subnode_namelen(void *fdt, int parentoffset,
			    const char *name, int namelen);
int fdt_add_subnode(void *fdt, int parentoffset, const char *name);
int fdt_del_node(void *fdt, int nodeoffset);

/* Extra functions */
const char *fdt_strerror(int errval);

#endif /* _LIBFDT_H */
