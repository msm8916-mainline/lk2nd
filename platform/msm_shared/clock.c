/*
 * Copyright (c) 2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Code Aurora nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
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
 */

#include <stdint.h>
#include <debug.h>
#include <reg.h>
#include <err.h>
#include <limits.h>
#include <bits.h>
#include <clock.h>
#include <string.h>

static struct clk_list msm_clk_list;

int clk_set_parent(struct clk *clk, struct clk *parent)
{
	if (!clk->ops->set_parent)
		return 0;

	return clk->ops->set_parent(clk, parent);
}

struct clk *clk_get_parent(struct clk *clk)
{
	if (!clk->ops->get_parent)
		return NULL;

	return clk->ops->get_parent(clk);
}

/*
 * Standard clock functions defined in include/clk.h
 */
int clk_enable(struct clk *clk)
{
	int ret = 0;
	struct clk *parent;

	if (!clk)
		return 0;

	if (clk->count == 0) {
		parent = clk_get_parent(clk);
		ret = clk_enable(parent);
		if (ret)
			goto out;

		if (clk->ops->enable)
			ret = clk->ops->enable(clk);
		if (ret) {
			clk_disable(parent);
			goto out;
		}
	}
	clk->count++;
out:
	return ret;
}

void clk_disable(struct clk *clk)
{
	struct clk *parent;

	if (!clk)
		return;

	if (clk->count == 0)
		goto out;
	if (clk->count == 1) {
		if (clk->ops->disable)
			clk->ops->disable(clk);
		parent = clk_get_parent(clk);
		clk_disable(parent);
	}
	clk->count--;
out:
	return;
}

unsigned long clk_get_rate(struct clk *clk)
{
	if (!clk->ops->get_rate)
		return 0;

	return clk->ops->get_rate(clk);
}

int clk_set_rate(struct clk *clk, unsigned long rate)
{
	if (!clk->ops->set_rate)
		return ERR_NOT_VALID;

	return clk->ops->set_rate(clk, rate);
}

void clk_init(struct clk_lookup *clist, unsigned num)
{
	if(clist && num)
	{
		msm_clk_list.clist = (struct clk_lookup *)clist;
		msm_clk_list.num = num;
	}
}

struct clk *clk_get (const char * cid)
{
	unsigned i;
	struct clk_lookup *cl= msm_clk_list.clist;
	unsigned num = msm_clk_list.num;

	if(!cl || !num)
	{
		dprintf (CRITICAL, "Alert!! clock list not defined!\n");
		return NULL;
	}
	for(i=0; i < num; i++, cl++)
	{
		if(!strcmp(cl->con_id, cid))
		{
			return cl->clk;
		}
	}

	dprintf(CRITICAL, "Alert!! Requested clock \"%s\" is not supported!", cid);
	return NULL;
}

int clk_get_set_enable(char *id, unsigned long rate, bool enable)
{
	int ret = NO_ERROR;
	struct clk *cp;

	/* Get clk */
	cp = clk_get(id);
	if(!cp)
	{
		dprintf(CRITICAL, "Can't find clock with id: %s\n", id);
		ret = ERR_NOT_VALID;
		goto get_set_enable_error;
	}

	/* Set rate */
	if(rate)
	{
		ret = clk_set_rate(cp, rate);
		if(ret)
		{
			dprintf(CRITICAL, "Clock set rate failed.\n");
			goto get_set_enable_error;
		}
	}

	/* Enable clock */
	if(enable)
	{
		ret = clk_enable(cp);
		if(ret)
		{
			dprintf(CRITICAL, "Clock enable failed.\n");
		}
	}

get_set_enable_error:
	return ret;
}

#ifdef DEBUG_CLOCK
struct clk_list *clk_get_list()
{
	return &msm_clk_list;
}
#endif
