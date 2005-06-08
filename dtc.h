#ifndef _DTC_H
#define _DTC_H

/*
 * (C) Copyright David Gibson <dwg@au1.ibm.com>, IBM Corporation.  2005.
 *
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *                                                                       
 *  You should have received a copy of the GNU General Public License    
 *  along with this program; if not, write to the Free Software          
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 
 *                                                                   USA 
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>

static inline void die(char * str, ...)
{
	va_list ap;

	va_start(ap, str);
	fprintf(stderr, "FATAL ERROR: ");
	vfprintf(stderr, str, ap);
	exit(1);
}

static inline void *xmalloc(size_t len)
{
	void *new = malloc(len);

	if (! new)
		die("malloc() failed\n");

	return new;
}

static inline void *xrealloc(void *p, size_t len)
{
	void *new = realloc(p, len);

	if (! new)
		die("realloc() failed (len=%d)\n", len);

	return new;
}

typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef u32 cell_t;

#define cpu_to_be16(x)	htons(x)
#define be16_to_cpu(x)	ntohs(x)

#define cpu_to_be32(x)	htonl(x)
#define be32_to_cpu(x)	ntohl(x)



#define streq(a, b)	(strcmp((a), (b)) == 0)
#define ALIGN(x, a)	(((x) + (a) - 1) & ~((a) - 1))
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

/* Data blobs */
struct data {
	int len;
	char *val;
	int asize;
};

#define empty_data ((struct data){.len = 0, .val = NULL, .asize = 0})

void data_free(struct data d);

struct data data_grow_for(struct data d, int xlen);

struct data data_copy_mem(char *mem, int len);
struct data data_copy_escape_string(char *s, int len);
struct data data_copy_file(FILE *f, size_t len);

struct data data_append_data(struct data d, void *p, int len);
struct data data_append_cell(struct data d, cell_t word);
struct data data_append_byte(struct data d, uint8_t byte);
struct data data_append_zeroes(struct data d, int len);
struct data data_append_align(struct data d, int align);

int data_is_one_string(struct data d);

/* DT constraints */

#define MAX_PROPNAME_LEN	31
#define MAX_NODENAME_LEN	31

/* Live trees */
struct property {
	char *name;
	struct data val;

	struct property *next;
};

struct node {
	char *name;
	struct property *proplist;
	struct node *children;

	struct node *parent;
	struct node *next_sibling;

	char *fullpath;
	int basenamelen;

	cell_t phandle;
	int addr_cells, size_cells;
};

#define for_each_property(n, p) \
	for ((p) = (n)->proplist; (p); (p) = (p)->next)

#define for_each_child(n, c)	\
	for ((c) = (n)->children; (c); (c) = (c)->next_sibling)

struct property *build_property(char *name, struct data val);
struct property *build_empty_property(char *name);
struct property *chain_property(struct property *first, struct property *list);

struct node *build_node(struct property *proplist, struct node *children);
struct node *name_node(struct node *node, char *name);
struct node *chain_node(struct node *first, struct node *list);

void add_property(struct node *node, struct property *prop);
void add_child(struct node *parent, struct node *child);

int check_device_tree(struct node *dt);

/* Flattened trees */

enum flat_dt_format {
	FFMT_BIN,
	FFMT_ASM,
};

void write_dt_blob(FILE *f, struct node *tree, int version, int reservenum);
void write_dt_asm(FILE *f, struct node *tree, int version, int reservenum);

struct node *dt_from_blob(FILE *f);

/* Tree source */

void write_tree_source(FILE *f, struct node *tree, int level);

struct node *dt_from_source(FILE *f);

/* FS trees */

struct node *dt_from_fs(char *dirname);

/* misc */

char *join_path(char *path, char *name);
void fill_fullpaths(struct node *tree, char *prefix);

#endif /* _DTC_H */
