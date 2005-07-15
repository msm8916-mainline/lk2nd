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

%option noyywrap nounput

%x CELLDATA
%x BYTESTRING
%x MEMRESERVE

PROPCHAR	[a-zA-Z0-9,._+*#?-]
UNITCHAR	[0-9a-f,]
WS		[ \t\n]

REFCHAR		({PROPCHAR}|{UNITCHAR}|[/@])

%{
#include "dtc.h"

#include "dtc-parser.tab.h"

/*#define LEXDEBUG	1*/

#ifdef LEXDEBUG
#define DPRINT(fmt, ...)	fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DPRINT(fmt, ...)	do { } while (0)
#endif

%}

%%

\"[^"]*\"	{
			DPRINT("String: %s\n", yytext);
			yylval.data = data_copy_escape_string(yytext+1,
					yyleng-2);
			return DT_STRING;
		}

"/memreserve/"	{
			DPRINT("Keyword: /memreserve/\n");
			BEGIN(MEMRESERVE);
			return DT_MEMRESERVE;
		}

<MEMRESERVE>[0-9a-fA-F]+ {
			if (yyleng > 2*sizeof(yylval.addr)) {
				fprintf(stderr, "Address value %s too large\n",
					yytext);
			}
			yylval.addr = (u64) strtoull(yytext, NULL, 16);
			DPRINT("Addr: %llx\n",
			       (unsigned long long)yylval.addr);
			return DT_ADDR;
		}

<MEMRESERVE>";"	{
			DPRINT("/MEMRESERVE\n");
			BEGIN(INITIAL);
			return ';';
		}

<CELLDATA>[0-9a-fA-F]+	{
			if (yyleng > 2*sizeof(yylval.cval)) {
				fprintf(stderr,
					"Cell value %s too long\n", yytext);
			}
			yylval.cval = strtol(yytext, NULL, 16);
			DPRINT("Cell: %x\n", yylval.cval);
			return DT_CELL;
		}

<CELLDATA>">"	{
			DPRINT("/CELLDATA\n");
			BEGIN(INITIAL);
			return '>';
		}

<CELLDATA>\&{REFCHAR}*	{
			DPRINT("Ref: %s\n", yytext+1);
			yylval.str = strdup(yytext+1);
			return DT_REF;
		}

<BYTESTRING>[0-9a-fA-F]{2} {
			yylval.byte = strtol(yytext, NULL, 16);
			DPRINT("Byte: %02x\n", (int)yylval.byte);
			return DT_BYTE;
		}

<BYTESTRING>"]"	{
			DPRINT("/BYTESTRING\n");
			BEGIN(INITIAL);
			return ']';
		}

{PROPCHAR}+	{
			DPRINT("PropName: %s\n", yytext);
			yylval.str = strdup(yytext);
			return DT_PROPNAME;
		}

{PROPCHAR}+(@{UNITCHAR}+)? {
			DPRINT("NodeName: %s\n", yytext);
			yylval.str = strdup(yytext);
			return DT_NODENAME;
		}


[a-zA-Z_][a-zA-Z0-9_]*:	{
			DPRINT("Label: %s\n", yytext);
			yylval.str = strdup(yytext);
			yylval.str[yyleng-1] = '\0';
			return DT_LABEL;
		}

<*>{WS}+	/* eat whitespace */

<*>"/*"([^*]|\*+[^*/])*\*+"/"	{
			DPRINT("Comment: %s\n", yytext);
			/* eat comments */
		}

<*>"//".*\n	/* eat line comments */

<*>.		{
			switch (yytext[0]) {
				case '<':
					DPRINT("CELLDATA\n");
					BEGIN(CELLDATA);
					break;
				case '[':
					DPRINT("BYTESTRING\n");
					BEGIN(BYTESTRING);
					break;
				default:

			DPRINT("Char: %c (\\x%02x)\n", yytext[0],
				(unsigned)yytext[0]);
					break;
			}

			return yytext[0];
		}

%%
