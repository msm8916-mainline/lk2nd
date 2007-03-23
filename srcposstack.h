/*
 * Copyright 2007 Jon Loeliger, Freescale Semiconductor, Inc.
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

#include "srcpos.h"


/*
 * This code should only be included into the lexical analysis.
 * It references global context symbols that are only present
 * in the generated lex.yy,c file.
 */

#ifdef FLEX_SCANNER


/*
 * Stack of nested include file contexts.
 */

struct incl_file {
	int filenum;
	FILE *file;
	YY_BUFFER_STATE yy_prev_buf;
	int yy_prev_lineno;
	struct incl_file *prev;
};

struct incl_file *incl_file_stack;


/*
 * Detect infinite include recursion.
 */
#define MAX_INCLUDE_DEPTH	(100)

static int incl_depth = 0;



int push_input_file(const char *filename)
{
	FILE *f;
	struct incl_file *incl_file;

	if (!filename) {
		yyerror("No include file name given.");
		return 0;
	}

	if (incl_depth++ >= MAX_INCLUDE_DEPTH) {
		yyerror("Includes nested too deeply");
		return 0;
	}

	f = dtc_open_file(filename);

	incl_file = malloc(sizeof(struct incl_file));
	if (!incl_file) {
		yyerror("Can not allocate include file space.");
		return 0;
	}

	/*
	 * Save current context.
	 */
	incl_file->yy_prev_buf = YY_CURRENT_BUFFER;
	incl_file->yy_prev_lineno = yylineno;
	incl_file->filenum = srcpos_filenum;
	incl_file->file = yyin;
	incl_file->prev = incl_file_stack;

	incl_file_stack = incl_file;

	/*
	 * Establish new context.
	 */
	srcpos_filenum = lookup_file_name(filename, 0);
	yylineno = 1;
	yyin = f;
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));

	return 1;
}


int pop_input_file(void)
{
	struct incl_file *incl_file;

	if (incl_file_stack == 0)
		return 0;

	fclose(yyin);

	/*
	 * Pop.
	 */
	--incl_depth;
	incl_file = incl_file_stack;
	incl_file_stack = incl_file->prev;

	/*
	 * Recover old context.
	 */
	yy_delete_buffer(YY_CURRENT_BUFFER);
	yy_switch_to_buffer(incl_file->yy_prev_buf);
	yylineno = incl_file->yy_prev_lineno;
	srcpos_filenum = incl_file->filenum;
	yyin = incl_file->file;

	/*
	 * Free old state.
	 */
	free(incl_file);

	if (YY_CURRENT_BUFFER == 0)
		return 0;

	return 1;
}

#endif	/* FLEX_SCANNER */
