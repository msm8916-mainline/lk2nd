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

%{
#include <stdio.h>

#include "dtc.h"
#include "srcpos.h"

YYLTYPE yylloc;

extern int yylex(void);
extern void yyerror(char const *s);

extern struct boot_info *the_boot_info;
extern int treesource_error;

static unsigned long long eval_literal(const char *s, int base, int bits);
%}

%union {
	char *propnodename;
	char *literal;
	char *labelref;
	unsigned int cbase;
	uint8_t byte;
	struct data data;

	uint64_t addr;
	cell_t cell;
	struct property *prop;
	struct property *proplist;
	struct node *node;
	struct node *nodelist;
	struct reserve_info *re;
}

%token DT_V1
%token DT_MEMRESERVE
%token <propnodename> DT_PROPNODENAME
%token <literal> DT_LITERAL
%token <cbase> DT_BASE
%token <byte> DT_BYTE
%token <data> DT_STRING
%token <labelref> DT_LABEL
%token <labelref> DT_REF
%token DT_INCBIN

%type <data> propdata
%type <data> propdataprefix
%type <re> memreserve
%type <re> memreserves
%type <addr> addr
%type <data> celllist
%type <cell> cellval
%type <data> bytestring
%type <prop> propdef
%type <proplist> proplist

%type <node> devicetree
%type <node> nodedef
%type <node> subnode
%type <nodelist> subnodes

%%

sourcefile:
	  DT_V1 ';' memreserves devicetree
		{
			the_boot_info = build_boot_info($3, $4,
							guess_boot_cpuid($4));
		}
	;

memreserves:
	  /* empty */
		{
			$$ = NULL;
		}
	| memreserve memreserves
		{
			$$ = chain_reserve_entry($1, $2);
		}
	;

memreserve:
	  DT_MEMRESERVE addr addr ';'
		{
			$$ = build_reserve_entry($2, $3);
		}
	| DT_LABEL memreserve
		{
			add_label(&$2->labels, $1);
			$$ = $2;
		}
	;

addr:
	  DT_LITERAL
		{
			$$ = eval_literal($1, 0, 64);
		}
	  ;

devicetree:
	  '/' nodedef
		{
			$$ = name_node($2, "");
		}
	| devicetree '/' nodedef
		{
			$$ = merge_nodes($1, $3);
		}
	| devicetree DT_REF nodedef
		{
			struct node *target;

			target = get_node_by_label($1, $2);
			if (target)
				merge_nodes(target, $3);
			else
				yyerror("label does not exist in "
					" node redefinition");
			$$ = $1;
		}
	;

nodedef:
	  '{' proplist subnodes '}' ';'
		{
			$$ = build_node($2, $3);
		}
	;

proplist:
	  /* empty */
		{
			$$ = NULL;
		}
	| proplist propdef
		{
			$$ = chain_property($2, $1);
		}
	;

propdef:
	  DT_PROPNODENAME '=' propdata ';'
		{
			$$ = build_property($1, $3);
		}
	| DT_PROPNODENAME ';'
		{
			$$ = build_property($1, empty_data);
		}
	| DT_LABEL propdef
		{
			add_label(&$2->labels, $1);
			$$ = $2;
		}
	;

propdata:
	  propdataprefix DT_STRING
		{
			$$ = data_merge($1, $2);
		}
	| propdataprefix '<' celllist '>'
		{
			$$ = data_merge($1, $3);
		}
	| propdataprefix '[' bytestring ']'
		{
			$$ = data_merge($1, $3);
		}
	| propdataprefix DT_REF
		{
			$$ = data_add_marker($1, REF_PATH, $2);
		}
	| propdataprefix DT_INCBIN '(' DT_STRING ',' addr ',' addr ')'
		{
			FILE *f = srcfile_relative_open($4.val, NULL);
			struct data d;

			if ($6 != 0)
				if (fseek(f, $6, SEEK_SET) != 0)
					srcpos_error(&yylloc,
						     "Couldn't seek to offset %llu in \"%s\": %s",
						     (unsigned long long)$6,
						     $4.val,
						     strerror(errno));

			d = data_copy_file(f, $8);

			$$ = data_merge($1, d);
			fclose(f);
		}
	| propdataprefix DT_INCBIN '(' DT_STRING ')'
		{
			FILE *f = srcfile_relative_open($4.val, NULL);
			struct data d = empty_data;

			d = data_copy_file(f, -1);

			$$ = data_merge($1, d);
			fclose(f);
		}
	| propdata DT_LABEL
		{
			$$ = data_add_marker($1, LABEL, $2);
		}
	;

propdataprefix:
	  /* empty */
		{
			$$ = empty_data;
		}
	| propdata ','
		{
			$$ = $1;
		}
	| propdataprefix DT_LABEL
		{
			$$ = data_add_marker($1, LABEL, $2);
		}
	;

celllist:
	  /* empty */
		{
			$$ = empty_data;
		}
	| celllist cellval
		{
			$$ = data_append_cell($1, $2);
		}
	| celllist DT_REF
		{
			$$ = data_append_cell(data_add_marker($1, REF_PHANDLE,
							      $2), -1);
		}
	| celllist DT_LABEL
		{
			$$ = data_add_marker($1, LABEL, $2);
		}
	;

cellval:
	  DT_LITERAL
		{
			$$ = eval_literal($1, 0, 32);
		}
	;

bytestring:
	  /* empty */
		{
			$$ = empty_data;
		}
	| bytestring DT_BYTE
		{
			$$ = data_append_byte($1, $2);
		}
	| bytestring DT_LABEL
		{
			$$ = data_add_marker($1, LABEL, $2);
		}
	;

subnodes:
	  /* empty */
		{
			$$ = NULL;
		}
	| subnode subnodes
		{
			$$ = chain_node($1, $2);
		}
	| subnode propdef
		{
			yyerror("syntax error: properties must precede subnodes");
			YYERROR;
		}
	;

subnode:
	  DT_PROPNODENAME nodedef
		{
			$$ = name_node($2, $1);
		}
	| DT_LABEL subnode
		{
			add_label(&$2->labels, $1);
			$$ = $2;
		}
	;

%%

void yyerror(char const *s)
{
	srcpos_error(&yylloc, "%s", s);
	treesource_error = 1;
}

static unsigned long long eval_literal(const char *s, int base, int bits)
{
	unsigned long long val;
	char *e;

	errno = 0;
	val = strtoull(s, &e, base);
	if (*e)
		yyerror("bad characters in literal");
	else if ((errno == ERANGE)
		 || ((bits < 64) && (val >= (1ULL << bits))))
		yyerror("literal out of range");
	else if (errno != 0)
		yyerror("bad literal");
	return val;
}
