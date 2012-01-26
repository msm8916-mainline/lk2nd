/*
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <debug.h>
#include <string.h>
#include <app.h>
#include <platform.h>
#include <kernel/thread.h>
#include <err.h>

#if defined(WITH_LIB_CONSOLE) && defined(DEBUG_CLOCK)
#include <lib/console.h>
#include <clock.h>

static void print_clock_list()
{
	unsigned i;
	struct clk_lookup *cl;
	unsigned num;
	struct clk_list *clock_list = clk_get_list();

	if(!clock_list)
		return;

	cl = clock_list->clist;
	num = clock_list->num;

	if(!cl || !num)
		return;

	printf("Clock list:\n");
	for(i=0; i < num; i++, cl++)
	{
		printf("%s\n", cl->con_id);
	}
}

static int clock_measure(const char *id)
{
	int ret = NO_ERROR;
	struct clk *cp, *mcp;
	unsigned long rate;

	/* Get clk */
	cp = clk_get(id);
	if(!cp)
	{
		ret = ERR_NOT_VALID;
		goto measure_error;
	}

	/* Get measure clk */
	mcp = clk_get("measure");
	if(!mcp)
	{
		ret = ERR_NOT_VALID;
		goto measure_error;
	}

	/* Set parent clk */
	clk_set_parent(mcp, cp);

	rate = clk_get_rate(mcp);

	printf("Clock %s is running at: %lu Hz.\n", id, rate);

measure_error:
	return ret;
}

static int clock_tests(int argc, cmd_args *argv)
{
	if (argc < 2) {
		printf("not enough arguments:\n");
		printf("%s  <clock-name>\n", argv[0].str);
		printf("%s  ?\n", argv[0].str);
		goto out;
	}
	if(argv[1].str[0] == '?')
		print_clock_list();
	else
		clock_measure(argv[1].str);
out:
	return 0;
}

STATIC_COMMAND_START
{ "test_clock", "Test Clock", &clock_tests },
STATIC_COMMAND_END(clocktests);

#endif

APP_START(clocktests)
APP_END

