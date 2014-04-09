/*
 * Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
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
#include <arch/defines.h>
#include <assert.h>
#include <reg.h>
#include <err.h>
#include <clock.h>
#include <clock_pll.h>
#include <clock_lib2.h>


/*=============== CXO clock ops =============*/
int cxo_clk_enable(struct clk *clk)
{
	/* Nothing to do. */
	return 0;
}

void cxo_clk_disable(struct clk *clk)
{
	/* Nothing to do. */
	return;
}


/*=============== Branch clock ops =============*/

/* Branch clock enable */
int clock_lib2_branch_clk_enable(struct clk *clk)
{
	int rc = 0;
	uint32_t cbcr_val;
	struct branch_clk *bclk = to_branch_clk(clk);

	cbcr_val  = readl(bclk->cbcr_reg);
	cbcr_val |= CBCR_BRANCH_ENABLE_BIT;
	writel(cbcr_val, bclk->cbcr_reg);

	/* wait until status shows it is enabled */
	while(readl(bclk->cbcr_reg) & CBCR_BRANCH_OFF_BIT);

	return rc;
}

/* Branch clock disable */
void clock_lib2_branch_clk_disable(struct clk *clk)
{
	uint32_t cbcr_val;
	struct branch_clk *bclk = to_branch_clk(clk);

	cbcr_val  = readl(bclk->cbcr_reg);
	cbcr_val &= ~CBCR_BRANCH_ENABLE_BIT;
	writel(cbcr_val, bclk->cbcr_reg);

	/* wait until status shows it is disabled */
	while(!(readl(bclk->cbcr_reg) & CBCR_BRANCH_OFF_BIT));
}

/* Branch clock set rate */
int clock_lib2_branch_set_rate(struct clk *c, unsigned rate)
{
	struct branch_clk *branch = to_branch_clk(c);

	if (!branch->has_sibling)
		return clk_set_rate(branch->parent, rate);

	return -1;
}


/*=============== Root clock ops =============*/

/* Root enable */
int clock_lib2_rcg_enable(struct clk *c)
{
	/* Hardware feedback from branch enable results in root being enabled.
	 * Nothing to do here.
	 */

	return 0;
}

/* Root set rate:
 * Find the entry in the frequecy table corresponding to the requested rate.
 * Enable the source clock required for the new frequency.
 * Call the set_rate function defined for this particular root clock.
 */
int clock_lib2_rcg_set_rate(struct clk *c, unsigned rate)
{
	struct rcg_clk *rclk = to_rcg_clk(c);
	struct clk_freq_tbl *nf; /* new freq */
	int rc = 0;

	/* ck if new freq is in table */
	for (nf = rclk->freq_tbl; nf->freq_hz != FREQ_END
			&& nf->freq_hz != rate; nf++)
		;

	/* Frequency not found in the table */
	if (nf->freq_hz == FREQ_END)
		return ERR_INVALID_ARGS;

	/* Check if frequency is actually changed. */
	if (nf == rclk->current_freq)
		return rc;

	/* First enable the source clock for this freq. */
	clk_enable(nf->src_clk);

	/* Perform clock-specific frequency switch operations. */
	ASSERT(rclk->set_rate);
	rclk->set_rate(rclk, nf);

	/* update current freq */
	rclk->current_freq = nf;

	return rc;
}

/* root update config: informs h/w to start using the new config values */
static void clock_lib2_rcg_update_config(struct rcg_clk *rclk)
{
	uint32_t cmd;

	cmd  = readl(rclk->cmd_reg);
	cmd |= CMD_UPDATE_BIT;
	writel(cmd, rclk->cmd_reg);

	/* Wait for frequency to be updated. */
	while(readl(rclk->cmd_reg) & CMD_UPDATE_MASK);
}

/* root set rate for clocks with half integer and MND divider */
void clock_lib2_rcg_set_rate_mnd(struct rcg_clk *rclk, struct clk_freq_tbl *freq)
{
	uint32_t cfg;

	/* Program MND values */
	writel(freq->m_val, rclk->m_reg);
	writel(freq->n_val, rclk->n_reg);
	writel(freq->d_val, rclk->d_reg);

	/* setup src select and divider */
	cfg  = readl(rclk->cfg_reg);
	cfg &= ~(CFG_SRC_SEL_MASK | CFG_SRC_DIV_MASK | CFG_MODE_MASK);
	cfg |= freq->div_src_val;
	if(freq->n_val !=0)
	{
		cfg |= (CFG_MODE_DUAL_EDGE << CFG_MODE_OFFSET);
	}
	writel(cfg, rclk->cfg_reg);

	/* Inform h/w to start using the new config. */
	clock_lib2_rcg_update_config(rclk);
}

/* root set rate for clocks with half integer divider */
void clock_lib2_rcg_set_rate_hid(struct rcg_clk *rclk, struct clk_freq_tbl *freq)
{
	uint32_t cfg;

	/* setup src select and divider */
	cfg  = readl(rclk->cfg_reg);
	cfg &= ~(CFG_SRC_SEL_MASK | CFG_SRC_DIV_MASK);
	cfg |= freq->div_src_val;
	writel(cfg, rclk->cfg_reg);

	clock_lib2_rcg_update_config(rclk);
}

/*=============== Vote clock ops =============*/

/* Vote clock enable */
int clock_lib2_vote_clk_enable(struct clk *c)
{
	uint32_t vote_regval;
	uint32_t val;
	struct vote_clk *vclk = to_local_vote_clk(c);

	vote_regval = readl(vclk->vote_reg);
	vote_regval |= vclk->en_mask;
	writel_relaxed(vote_regval, vclk->vote_reg);
	do {
		val = readl(vclk->cbcr_reg);
		val &= BRANCH_CHECK_MASK;
	}
	/*  wait until status shows it is enabled */
	while((val != BRANCH_ON_VAL) && (val != BRANCH_NOC_FSM_ON_VAL));

	return 0;
}

/* Vote clock disable */
void clock_lib2_vote_clk_disable(struct clk *c)
{
	uint32_t vote_regval;
	struct vote_clk *vclk = to_local_vote_clk(c);

	vote_regval = readl(vclk->vote_reg);
	vote_regval &= ~vclk->en_mask;
    writel_relaxed(vote_regval, vclk->vote_reg);
}

/* Reset clock */
static int __clock_lib2_branch_clk_reset(uint32_t bcr_reg, enum clk_reset_action action)
{
	uint32_t reg;
	int ret = 0;

	reg = readl(bcr_reg);

	switch (action) {
	case CLK_RESET_ASSERT:
		reg |= BIT(0);
		break;
	case CLK_RESET_DEASSERT:
		reg &= ~BIT(0);
		break;
	default:
		ret = 1;
	}

	writel(reg, bcr_reg);

	/* Wait for writes to go through */
	dmb();

	return ret;
}

int clock_lib2_reset_clk_reset(struct clk *c, enum clk_reset_action action)
{
	struct reset_clk *rst = to_reset_clk(c);

	if (!rst)
		return 0;

	return __clock_lib2_branch_clk_reset(rst->bcr_reg, action);
}

int clock_lib2_branch_clk_reset(struct clk *c, enum clk_reset_action action)
{
	struct branch_clk *bclk = to_branch_clk(c);

	if (!bclk)
		return 0;

	return __clock_lib2_branch_clk_reset(bclk->bcr_reg, action);
}
