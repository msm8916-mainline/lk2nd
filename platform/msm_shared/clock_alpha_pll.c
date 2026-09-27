/*
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
#include <string.h>
#include <debug.h>
#include <reg.h>
#include <bits.h>
#include <limits.h>
#include <clock.h>
#include <clock_pll.h>
#include <clock_alpha_pll.h>
#include <arch/defines.h>
#include <platform/timer.h>

#define ALPHA_REG_BITWIDTH 40
#define ALPHA_BITWIDTH 32

static void set_fsm_mode(uint32_t mode_reg)
{
	uint32_t regval = readl_relaxed(mode_reg);

	/* De-assert reset to FSM */
	regval &= ~PLL_VOTE_FSM_RESET;
	writel_relaxed(regval, mode_reg);

	/* Program bias count */
	regval &= ~BM(PLL_BIAS_COUNT_END, PLL_BIAS_COUNT_START);
	regval |= BVAL(PLL_BIAS_COUNT_END, PLL_BIAS_COUNT_START, PLL_BIAS_COUNT);
	writel_relaxed(regval, mode_reg);

	/* Program lock count */
	regval &= ~BM(PLL_LOCK_COUNT_END, PLL_LOCK_COUNT_START);
	regval |= BVAL(PLL_LOCK_COUNT_END, PLL_LOCK_COUNT_START, 0x0);
	writel_relaxed(regval, mode_reg);

	/* Enable PLL FSM voting */
	regval |= PLL_VOTE_FSM_ENA;
	writel_relaxed(regval, mode_reg);
}

static inline struct alpha_pll_clk *to_alpha_pll_clk(struct clk *c)
{
	return container_of(c, struct alpha_pll_clk, c);
}

static void init_alpha_pll(struct clk *c)
{
	struct alpha_pll_clk *pll = to_alpha_pll_clk(c);
	struct alpha_pll_masks *masks = pll->masks;
	uint32_t output_en, userval;

	if (masks->output_mask && pll->enable_config) {
		output_en = readl_relaxed(OUTPUT_REG(pll));
		output_en &= ~masks->output_mask;
		output_en |= pll->enable_config;
		writel_relaxed(output_en, OUTPUT_REG(pll));
	}

	if (masks->post_div_mask) {
		userval = readl_relaxed(USER_CTL_LO_REG(pll));
		userval &= ~masks->post_div_mask;
		userval |= pll->post_div_config;
		writel_relaxed(userval, USER_CTL_LO_REG(pll));
	}

	if (pll->fsm_en_mask)
		set_fsm_mode(MODE_REG(pll));

	pll->inited = true;
}

static bool is_active(struct alpha_pll_clk *pll)
{
	uint32_t reg = readl(ACTIVE_REG(pll));
	uint32_t mask = pll->masks->active_mask;
	return (reg & mask) == mask;
}

static bool is_locked(struct alpha_pll_clk *pll)
{
	uint32_t reg = readl(LOCK_REG(pll));
	uint32_t mask = pll->masks->lock_mask;
	return (reg & mask) == mask;
}

static bool is_fsm_mode(uint32_t mode_reg)
{
	return !!(readl(mode_reg) & PLL_VOTE_FSM_ENA);
}

static bool update_finish(struct alpha_pll_clk *pll)
{
	return is_active(pll);
}

static int wait_for_update(struct alpha_pll_clk *pll)
{
	uint32_t retry = 100;

	while (retry)
	{
		if (update_finish(pll))
			break;
		udelay(1);
		retry--;
	}

	if (!retry)
	{
		dprintf(CRITICAL, "%s didn't lock after enabling it!\n", pll->c.dbg_name);
		return -1;
	}

	return 0;
}

static unsigned long compute_rate(struct alpha_pll_clk *pll,
				uint32_t l_val, uint32_t a_val)
{
	uint64_t rate, parent_rate;

	parent_rate = pll->parent->rate;
	rate = parent_rate * l_val;
	rate += (parent_rate * a_val) >> ALPHA_BITWIDTH;
	return rate;
}

static uint32_t do_div(uint64_t *n, uint32_t divisor)
{
	uint32_t remainder = *n % divisor;
	*n = *n / divisor;
	return remainder;
}

static unsigned long calc_values(struct alpha_pll_clk *pll,
		uint64_t rate, int *l_val, uint64_t *a_val, bool round_up)
{
	uint32_t parent_rate;
	uint64_t remainder;
	uint64_t quotient;
	unsigned long freq_hz;

	parent_rate = pll->parent->rate;
	quotient = rate;
	remainder = do_div(&quotient, parent_rate);

	*l_val = quotient;

	if (!remainder) {
		*a_val = 0;
		return rate;
	}

	/* Upper ALPHA_BITWIDTH bits of Alpha */
	quotient = remainder << ALPHA_BITWIDTH;
	remainder = do_div(&quotient, parent_rate);

	if (remainder && round_up)
		quotient++;

	*a_val = quotient;
	freq_hz = compute_rate(pll, *l_val, *a_val);
	return freq_hz;
}

static unsigned long round_rate_down(struct alpha_pll_clk *pll,
		unsigned long rate, int *l_val, uint64_t *a_val)
{
	return calc_values(pll, rate, l_val, a_val, false);
}

static unsigned long round_rate_up(struct alpha_pll_clk *pll,
		unsigned long rate, int *l_val, uint64_t *a_val)
{
	return calc_values(pll, rate, l_val, a_val, true);
}

static uint32_t find_vco(struct alpha_pll_clk *pll, unsigned long rate)
{
	unsigned long i;
	struct alpha_pll_vco_tbl *v = pll->vco_tbl;

	for (i = 0; i < pll->vco_num; i++) {
		if (rate >= v[i].min_freq && rate <= v[i].max_freq)
			return v[i].vco_val;
	}
	return -1;
}

static int alpha_pll_set_rate(struct clk *c, unsigned long rate)
{
	struct alpha_pll_clk *pll = to_alpha_pll_clk(c);
	struct alpha_pll_masks *masks = pll->masks;
	unsigned long freq_hz;
	int regval, l_val;
	uint64_t a_val;
	uint32_t vco_val;

	freq_hz = round_rate_up(pll, rate, &l_val, &a_val);
	if (freq_hz != rate) {
		dprintf(CRITICAL, "alpha_pll: Call clk_set_rate with rounded rates!\n");
		return -1;
	}

	vco_val = find_vco(pll, freq_hz);
	if (!vco_val)
	{
		dprintf(CRITICAL, "alpha pll: VCO not valid\n");
		return -1;
	}

	/*
	 * Ensure PLL is off before changing rate. For optimization reasons,
	 * assume no downstream clock is actively using it. No support
	 * for dynamic update at the moment.
	 */
	if (c->count)
		alpha_pll_disable(c);

	a_val = a_val << (ALPHA_REG_BITWIDTH - ALPHA_BITWIDTH);

	writel_relaxed(l_val, L_REG(pll));
	writel_relaxed(a_val, A_REG(pll));
	a_val = (a_val >> ALPHA_BITWIDTH);
	writel_relaxed(a_val, A_REG_U(pll));

	if (masks->vco_mask) {
		regval = readl_relaxed(VCO_REG(pll));
		regval &= ~(masks->vco_mask << masks->vco_shift);
		regval |= vco_val << masks->vco_shift;
		writel_relaxed(regval, VCO_REG(pll));
	}

	regval = readl_relaxed(ALPHA_EN_REG(pll));
	regval |= masks->alpha_en_mask;
	writel_relaxed(regval, ALPHA_EN_REG(pll));

	if (c->count)
		alpha_pll_enable(c);

	return 0;
}

static void update_vco_tbl(struct alpha_pll_clk *pll)
{
	uint32_t i;
	int l_val;
	uint64_t a_val;
	uint64_t rate;

	for (i = 0 ; i < pll->vco_num; i++)
	{
		rate = round_rate_up(pll, pll->vco_tbl[i].min_freq, &l_val, &a_val);
		pll->vco_tbl[i].min_freq = rate;

		rate = round_rate_down(pll, pll->vco_tbl[i].max_freq, &l_val, &a_val);
		pll->vco_tbl[i].max_freq = rate;
	}
}

int alpha_pll_enable(struct clk *c)
{
	struct alpha_pll_clk *pll = to_alpha_pll_clk(c);
	uint32_t ena;

	update_vco_tbl(pll);

	/* if PLL is not initialized already and is not in FSM state */
	if (!pll->inited && !is_locked(pll))
	{
		if (c->rate && alpha_pll_set_rate(c, c->rate))
			dprintf(INFO, "Warning: Failed to set rate for alpha pll\n");
		init_alpha_pll(c);
	}
	else if (!is_fsm_mode(MODE_REG(pll)))
	{
		dprintf(INFO, "FSM mode is not set for: %s\n", c->dbg_name);
	}

	ena = readl_relaxed(VOTE_REG(pll));
	ena |= pll->fsm_en_mask;
	writel_relaxed(ena, VOTE_REG(pll));
	dmb();

	return wait_for_update(pll);
}

void alpha_pll_disable(struct clk *c)
{
	struct alpha_pll_clk *pll = to_alpha_pll_clk(c);
	uint32_t ena;

	ena = readl_relaxed(VOTE_REG(pll));
	ena &= ~pll->fsm_en_mask;
	writel_relaxed(ena, VOTE_REG(pll));
	dmb();
}
