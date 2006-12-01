#ifndef _LIBFDT_INTERNAL_H
#define _LIBFDT_INTERNAL_H
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

#define ALIGN(x, a)	(((x) + (a) - 1) & ~((a) - 1))
#define PALIGN(p, a)	((void *)ALIGN((unsigned long)(p), (a)))

#define memeq(p, q, n)	(memcmp((p), (q), (n)) == 0)
#define streq(p, q)	(strcmp((p), (q)) == 0)

int _fdt_check_header(const struct fdt_header *fdt);
uint32_t _fdt_next_tag(const struct fdt_header *fdt, int startoffset, int *nextoffset);
struct fdt_property *_fdt_getprop(const struct fdt_header *fdt, int nodeoffset,
				  const char *name, int *lenp);


#define OFFSET_ERROR(code)	-(code)
#define PTR_ERROR(code)		(void *)(-(code))

#define SW_MAGIC		(~FDT_MAGIC)
#define sw_size_dt_struct(fdt)	(fdt32_to_cpu(((fdt)->version)))

#endif /* _LIBFDT_INTERNAL_H */
