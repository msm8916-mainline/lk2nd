/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of The Linux Foundation nor
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
#include <bits.h>
#include <limits.h>
#include <platform/timer.h>
#include <clock.h>
#include <clock_pll.h>

/*
 * pll_vote_clk functions
 */
int pll_vote_clk_enable(struct clk *clk)
{
	uint32_t ena;
	struct pll_vote_clk *pll = to_pll_vote_clk(clk);

	ena = readl_relaxed(pll->en_reg);
	ena |= pll->en_mask;
	writel_relaxed(ena, pll->en_reg);

	/* Wait until PLL is enabled */
	while ((readl_relaxed(pll->status_reg) & pll->status_mask) == 0);

	return 0;
}

void pll_vote_clk_disable(struct clk *clk)
{
	uint32_t ena;
	struct pll_vote_clk *pll = to_pll_vote_clk(clk);

	ena = readl_relaxed(pll->en_reg);
	ena &= ~(pll->en_mask);
	writel_relaxed(ena, pll->en_reg);
}

unsigned pll_vote_clk_get_rate(struct clk *clk)
{
	struct pll_vote_clk *pll = to_pll_vote_clk(clk);
	return pll->rate;
}

struct clk *pll_vote_clk_get_parent(struct clk *clk)
{
	struct pll_vote_clk *pll = to_pll_vote_clk(clk);
	return pll->parent;
}

int pll_vote_clk_is_enabled(struct clk *clk)
{
	struct pll_vote_clk *pll = to_pll_vote_clk(clk);
	return !!(readl_relaxed(pll->status_reg) & pll->status_mask);
}

/*
 * PLLs functions
 */
int pll_clk_enable(struct clk *clk)
{
	uint32_t mode;
	struct pll_clk *pll = to_pll_clk(clk);

	mode = readl_relaxed(pll->mode_reg);
	/* Disable PLL bypass mode. */
	mode |= BIT(1);
	writel_relaxed(mode, pll->mode_reg);

	/*
	 * H/W requires a 5us delay between disabling the bypass and
	 * de-asserting the reset. Delay 10us just to be safe.
	 */
	udelay(10);

	/* De-assert active-low PLL reset. */
	mode |= BIT(2);
	writel_relaxed(mode, pll->mode_reg);

	/* Wait until PLL is locked. */
	udelay(50);

	/* Enable PLL output. */
	mode |= BIT(0);
	writel_relaxed(mode, pll->mode_reg);

	return 0;
}

void pll_clk_disable(struct clk *clk)
{
	uint32_t mode;
	struct pll_clk *pll = to_pll_clk(clk);

	/*
	 * Disable the PLL output, disable test mode, enable
	 * the bypass mode, and assert the reset.
	 */
	mode = readl_relaxed(pll->mode_reg);
	mode &= ~BM(3, 0);
	writel_relaxed(mode, pll->mode_reg);
}

unsigned pll_clk_get_rate(struct clk *clk)
{
	struct pll_clk *pll = to_pll_clk(clk);
	return pll->rate;
}

struct clk *pll_clk_get_parent(struct clk *clk)
{
	struct pll_clk *pll = to_pll_clk(clk);
	return pll->parent;
}

