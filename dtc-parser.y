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

%glr-parser
%locations

%{
#include "dtc.h"
#include "srcpos.h"

int yylex(void);
void yyerror(char const *);
cell_t cell_from_string(char *s, unsigned int base);

extern struct boot_info *the_boot_info;

%}

%union {
	cell_t cval;
	unsigned int cbase;
	u8 byte;
	char *str;
	struct data data;
	struct property *prop;
	struct property *proplist;
	struct node *node;
	struct node *nodelist;
	int datalen;
	int hexlen;
	u64 addr;
	struct reserve_info *re;
}

%token DT_MEMRESERVE
%token <addr> DT_ADDR
%token <str> DT_PROPNAME
%token <str> DT_NODENAME
%token <cbase> DT_BASE
%token <str> DT_CELL
%token <byte> DT_BYTE
%token <data> DT_STRING
%token <str> DT_UNIT
%token <str> DT_LABEL
%token <str> DT_REF

%type <data> propdata
%type <data> propdataprefix
%type <re> memreserve
%type <re> memreserves
%type <cbase> opt_cell_base
%type <data> celllist
%type <data> bytestring
%type <prop> propdef
%type <proplist> proplist

%type <node> devicetree
%type <node> nodedef
%type <node> subnode
%type <nodelist> subnodes
%type <str> label
%type <str> nodename

%%

sourcefile:	memreserves devicetree {
			the_boot_info = build_boot_info($1, $2);
		}
	;

memreserves:	memreserve memreserves {
			$$ = chain_reserve_entry($1, $2);
		}
	|	/* empty */	{
			$$ = NULL;
		}
	;

memreserve:	DT_MEMRESERVE DT_ADDR DT_ADDR ';' {
			$$ = build_reserve_entry($2, $3, NULL);
		}
	|	DT_MEMRESERVE DT_ADDR '-' DT_ADDR ';' {
			$$ = build_reserve_entry($2, $4 - $2 + 1, NULL);
		}
	;

devicetree:	'/' nodedef {
			$$ = name_node($2, "", NULL);
		}
	;

nodedef:	'{' proplist subnodes '}' ';' {
			$$ = build_node($2, $3);
		}
	;

proplist:	propdef proplist {
			$$ = chain_property($1, $2);
		}
	|	/* empty */	{
			$$ = NULL;
		}
	;

propdef:	label DT_PROPNAME '=' propdata ';' {
			$$ = build_property($2, $4, $1);
		}
	|	label DT_PROPNAME ';' {
			$$ = build_property($2, empty_data, $1);
		}
	;

propdata:	propdataprefix DT_STRING { $$ = data_merge($1, $2); }
	|	propdataprefix '<' celllist '>' {
			$$ = data_merge(data_append_align($1, sizeof(cell_t)), $3);
		}
	|	propdataprefix '[' bytestring ']' { $$ = data_merge($1, $3); }
	;

propdataprefix:	propdata ',' { $$ = $1; }
	|	/* empty */ { $$ = empty_data; }
	;

opt_cell_base:
	  /* empty */
		{ $$ = 16; }
	| DT_BASE
	;

celllist:	celllist opt_cell_base DT_CELL {
			$$ = data_append_cell($1,
					      cell_from_string($3, $2));
		}
	|	celllist DT_REF	{
			$$ = data_append_cell(data_add_fixup($1, $2), -1);
		}
	|	/* empty */ { $$ = empty_data; }
	;

bytestring:	bytestring DT_BYTE { $$ = data_append_byte($1, $2); }
	|	/* empty */ { $$ = empty_data; }
	;

subnodes:	subnode subnodes {
			$$ = chain_node($1, $2);
		}
	|	/* empty */ { $$ = NULL; }
	;

subnode:	label nodename nodedef { $$ = name_node($3, $2, $1); }
	;

nodename:	DT_NODENAME	{ $$ = $1; }
	|	DT_PROPNAME	{ $$ = $1; }
	;

label:		DT_LABEL	{ $$ = $1; }
	|	/* empty */	{ $$ = NULL; }
	;

%%

void yyerror (char const *s)
{
	const char *fname = srcpos_filename_for_num(yylloc.filenum);

	if (strcmp(fname, "-") == 0)
		fname = "stdin";

	fprintf(stderr, "%s:%d %s\n",
		fname, yylloc.first_line, s);
}


/*
 * Convert a string representation of a numeric cell
 * in the given base into a cell.
 *
 * FIXME: The string "abc123", base 10, should be flagged
 *        as an error due to the leading "a", but isn't yet.
 */

cell_t cell_from_string(char *s, unsigned int base)
{
	cell_t c;

	c = strtoul(s, NULL, base);
	if (errno == EINVAL || errno == ERANGE) {
		fprintf(stderr,
			"Line %d: Invalid cell value '%s'; %d assumed\n",
			yylloc.first_line, s, c);
	}

	return c;
}
