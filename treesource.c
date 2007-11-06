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

#include "dtc.h"
#include "srcpos.h"

extern FILE *yyin;
extern int yyparse(void);
extern void yyerror(char const *);

struct boot_info *the_boot_info;

struct boot_info *dt_from_source(const char *fname)
{
	the_boot_info = NULL;

	push_input_file(fname);

	if (yyparse() != 0)
		return NULL;

	fill_fullpaths(the_boot_info->dt, "");

	return the_boot_info;
}

static void write_prefix(FILE *f, int level)
{
	int i;

	for (i = 0; i < level; i++)
		fputc('\t', f);
}

int isstring(char c)
{
	return (isprint(c)
		|| (c == '\0')
		|| strchr("\a\b\t\n\v\f\r", c));
}

static void write_propval_string(FILE *f, struct data val)
{
	char *str = val.val;
	int i;

	assert(str[val.len-1] == '\0');

	fprintf(f, "\"");
	for (i = 0; i < (val.len-1); i++) {
		char c = str[i];

		switch (c) {
		case '\a':
			fprintf(f, "\\a");
			break;
		case '\b':
			fprintf(f, "\\b");
			break;
		case '\t':
			fprintf(f, "\\t");
			break;
		case '\n':
			fprintf(f, "\\n");
			break;
		case '\v':
			fprintf(f, "\\v");
			break;
		case '\f':
			fprintf(f, "\\f");
			break;
		case '\r':
			fprintf(f, "\\r");
			break;
		case '\\':
			fprintf(f, "\\\\");
			break;
		case '\"':
			fprintf(f, "\\\"");
			break;
		case '\0':
			fprintf(f, "\", \"");
			break;
		default:
			if (isprint(c))
				fprintf(f, "%c", c);
			else
				fprintf(f, "\\x%02hhx", c);
		}
	}
	fprintf(f, "\"");
}

static void write_propval_cells(FILE *f, struct data val)
{
	void *propend = val.val + val.len;
	cell_t *cp = (cell_t *)val.val;

	fprintf(f, "<");
	for (;;) {
		fprintf(f, "%x", be32_to_cpu(*cp++));
		if ((void *)cp >= propend)
			break;
		fprintf(f, " ");
	}
	fprintf(f, ">");
}

static void write_propval_bytes(FILE *f, struct data val)
{
	void *propend = val.val + val.len;
	char *bp = val.val;

	fprintf(f, "[");
	for (;;) {
		fprintf(f, "%02hhx", *bp++);
		if ((void *)bp >= propend)
			break;
		fprintf(f, " ");
	}
	fprintf(f, "]");
}

static void write_propval(FILE *f, struct property *prop)
{
	int len = prop->val.len;
	char *p = prop->val.val;
	int nnotstring = 0, nnul = 0;
	int i;

	if (len == 0) {
		fprintf(f, ";\n");
		return;
	}

	for (i = 0; i < len; i++) {
		if (! isstring(p[i]))
			nnotstring++;
		if (p[i] == '\0')
			nnul++;
	}

	fprintf(f, " = ");

	if ((p[len-1] == '\0') && (nnotstring == 0) && (nnul < (len-nnul))) {
		write_propval_string(f, prop->val);
	} else if (((len % sizeof(cell_t)) == 0)) {
		write_propval_cells(f, prop->val);
	} else {
		write_propval_bytes(f, prop->val);
	}
	fprintf(f, ";\n");
}

static void write_tree_source_node(FILE *f, struct node *tree, int level)
{
	struct property *prop;
	struct node *child;

	write_prefix(f, level);
	if (tree->label)
		fprintf(f, "%s: ", tree->label);
	if (tree->name && (*tree->name))
		fprintf(f, "%s {\n", tree->name);
	else
		fprintf(f, "/ {\n");

	for_each_property(tree, prop) {
		write_prefix(f, level+1);
		if (prop->label)
			fprintf(f, "%s: ", prop->label);
		fprintf(f, "%s", prop->name);
		write_propval(f, prop);
	}
	for_each_child(tree, child) {
		fprintf(f, "\n");
		write_tree_source_node(f, child, level+1);
	}
	write_prefix(f, level);
	fprintf(f, "};\n");
}


void dt_to_source(FILE *f, struct boot_info *bi)
{
	struct reserve_info *re;

	for (re = bi->reservelist; re; re = re->next) {
		if (re->label)
			fprintf(f, "%s: ", re->label);
		fprintf(f, "/memreserve/\t%016llx-%016llx;\n",
			(unsigned long long)re->re.address,
			(unsigned long long)(re->re.address + re->re.size - 1));
	}

	write_tree_source_node(f, bi->dt, 0);
}

