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
#include "dtc.h"

int yylex (void);
void yyerror (char const *);

extern struct node *device_tree;

%}

%union {
	cell_t cval;
	uint8_t byte;
	char *str;
	struct data data;
	struct property *prop;
	struct property *proplist;
	struct node *node;
	struct node *nodelist;
	int datalen;
	int hexlen;
}

%token <str> DT_PROPNAME
%token <str> DT_NODENAME
%token <cval> DT_CELL
%token <byte> DT_BYTE
%token <data> DT_STRING
%token <str> DT_UNIT

%type <data> propdata
%type <data> celllist
%type <data> bytestring
%type <prop> propdef
%type <proplist> proplist

%type <node> nodedef
%type <node> subnode
%type <nodelist> subnodes

%%

devicetree:	{
			assert(device_tree == NULL);
		} '/' nodedef { device_tree = name_node($3, ""); }
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

propdef:	DT_PROPNAME '=' propdata ';' {
			$$ = build_property($1, $3);
		}
	|	DT_PROPNAME ';' {
			$$ = build_empty_property($1);
		}
	;

propdata:	DT_STRING { $$ = $1; }
	|	'<' celllist '>' { $$ = $2; }
	|	'[' bytestring ']' { $$ = $2; }
	;

celllist:	celllist DT_CELL { $$ = data_append_cell($1, $2); }
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

subnode:	DT_NODENAME nodedef { $$ = name_node($2, $1); }
	;

%%

void yyerror (char const *s)
{
	fprintf (stderr, "%s\n", s);
}
