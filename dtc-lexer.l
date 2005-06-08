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

%option noyywrap

%x CELLDATA
%x BYTESTRING

PROPCHAR	[a-zA-Z0-9,._+*#?-]
UNITCHAR	[0-9a-f,]
WS		[ \t\n]

%%

%{
#include "dtc.h"

#include "y.tab.h"

#undef LEXDEBUG	1

%}

\"[^"]*\"	{
#ifdef LEXDEBUG
			fprintf(stderr, "String: %s\n", yytext);
#endif
			yylval.data = data_copy_escape_string(yytext+1,
					yyleng-2);
			return DT_STRING;
		}

<CELLDATA>[0-9a-fA-F]+	{
			if (yyleng > 2*sizeof(yylval.cval)) {
				fprintf(stderr,
					"Cell value %s too long\n", yytext);
			}
			yylval.cval = strtol(yytext, NULL, 16);
#ifdef LEXDEBUG
			fprintf(stderr, "Cell: %x\n", yylval.cval);
#endif
			return DT_CELL;
		}

<CELLDATA>">"	{
#ifdef LEXDEBUG
			fprintf(stderr, "/CELLDATA\n");
#endif
			BEGIN(INITIAL);
			return '>';
		}

<BYTESTRING>[0-9a-fA-F]{2} {
			yylval.byte = strtol(yytext, NULL, 16);
#ifdef LEXDEBUG
			fprintf(stderr, "Byte: %02x\n", (int)yylval.byte);
#endif
			return DT_BYTE;
		}

<BYTESTRING>"]"	{
#ifdef LEXDEBUG
			fprintf(stderr, "/BYTESTRING\n");
#endif
			BEGIN(INITIAL);
			return ']';
		}

{PROPCHAR}+(@{UNITCHAR}+)?/{WS}*\{ {
#ifdef LEXDEBUG
			fprintf(stderr, "NodeName: %s\n", yytext);
#endif
			yylval.str = strdup(yytext);
			return DT_NODENAME;
		}

{PROPCHAR}+	{
#ifdef LEXDEBUG
			fprintf(stderr, "PropName: %s\n", yytext);
#endif
			yylval.str = strdup(yytext);
			return DT_PROPNAME;
		}


<*>{WS}+	/* eat whitespace */

<*>"/*"([^*]|\*+[^*/])*\*+"/"	{
#ifdef LEXDEBUG
			fprintf(stderr, "Comment: %s\n", yytext);
			/* eat comments */
#endif
		}

<*>"//".*\n	/* eat line comments */

.		{
			switch (yytext[0]) {
				case '<':
#ifdef LEXDEBUG
					fprintf(stderr, "CELLDATA\n");
#endif
					BEGIN(CELLDATA);
					break;
				case '[':
#ifdef LEXDEBUG
					fprintf(stderr, "BYTESTRING\n");
#endif
					BEGIN(BYTESTRING);
					break;
				default:

#ifdef LEXDEBUG
			fprintf(stderr, "Char: %c (\\x%02x)\n", yytext[0],
				(unsigned)yytext[0]);
#endif
					break;
			}

			return yytext[0];
		}

%%
