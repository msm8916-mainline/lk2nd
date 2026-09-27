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
#include <limits.h>
#include <bits.h>
#include <clock.h>
#include <clock-local.h>
#include <platform/timer.h>

/*
 * When enabling/disabling a clock, check the halt bit up to this number
 * number of times (with a 1 us delay in between) before continuing.
 */
#define HALT_CHECK_MAX_LOOPS	100
/* For clock without halt checking, wait this long after enables/disables. */
#define HALT_CHECK_DELAY_US	10

struct clk_freq_tbl local_dummy_freq = F_END;

/*
 * Clock enable/disable functions
 */
static int branch_clk_is_halted(const struct branch *clk)
{
	int invert = (clk->halt_check == ENABLE);
	int status_bit = readl_relaxed(clk->halt_reg) & BIT(clk->halt_bit);
	return invert ? !status_bit : status_bit;
}

static void __branch_clk_enable_reg(const struct branch *clk, const char *name)
{
	uint32_t reg_val;

	if (clk->en_mask) {
		reg_val = readl_relaxed(clk->ctl_reg);
		reg_val |= clk->en_mask;
		writel_relaxed(reg_val, clk->ctl_reg);
	}

	/* Wait for clock to enable before returning. */
	if (clk->halt_check == DELAY)
		udelay(HALT_CHECK_DELAY_US);
	else if (clk->halt_check == ENABLE || clk->halt_check == HALT
			|| clk->halt_check == ENABLE_VOTED
			|| clk->halt_check == HALT_VOTED) {
		int count;

		/* Wait up to HALT_CHECK_MAX_LOOPS for clock to enable. */
		for (count = HALT_CHECK_MAX_LOOPS; branch_clk_is_halted(clk)
					&& count > 0; count--)
			udelay(1);
	}
}

/* Perform any register operations required to enable the clock. */
static void __local_clk_enable_reg(struct rcg_clk *clk)
{
	uint32_t reg_val;

	void *const reg = clk->b.ctl_reg;

	if(clk->current_freq == &local_dummy_freq)
		dprintf(CRITICAL, "Attempting to enable %s before setting its rate.", clk->c.dbg_name);
	/*
	 * Program the NS register, if applicable. NS registers are not
	 * set in the set_rate path because power can be saved by deferring
	 * the selection of a clocked source until the clock is enabled.
	 */
	if (clk->ns_mask) {
		reg_val = readl_relaxed(clk->ns_reg);
		reg_val &= ~(clk->ns_mask);
		reg_val |= (clk->current_freq->ns_val & clk->ns_mask);
		writel_relaxed(reg_val, clk->ns_reg);
	}

	/* Enable MN counter, if applicable. */
	reg_val = readl_relaxed(reg);
	if (clk->current_freq->mnd_en_mask) {
		reg_val |= clk->current_freq->mnd_en_mask;
		writel_relaxed(reg_val, reg);
	}
	/* Enable root. */
	if (clk->root_en_mask) {
		reg_val |= clk->root_en_mask;
		writel_relaxed(reg_val, reg);
	}
	__branch_clk_enable_reg(&clk->b, clk->c.dbg_name);
}

/* Enable a clock and any related power rail. */
int local_clk_enable(struct clk *c)
{
	int rc;
	struct rcg_clk *clk = to_rcg_clk(c);

	rc = clk_enable(clk->depends);
	if (rc)
		goto err_dep;
	__local_clk_enable_reg(clk);
	clk->enabled = true;
err_dep:
	return rc;
}

/* Disable a clock and any related power rail. */
void local_clk_disable(struct clk *c)
{
	/*TODO: Stub function for now.*/
}

/*
 * Frequency-related functions
 */

/* Set a clock's frequency. */
static int _local_clk_set_rate(struct rcg_clk *clk, struct clk_freq_tbl *nf)
{
	struct clk_freq_tbl *cf;
	int rc = 0;

	/* Check if frequency is actually changed. */
	cf = clk->current_freq;
	if (nf == cf)
		goto unlock;

	if (clk->enabled) {
		rc = clk_enable(nf->src_clk);
		if (rc) {
			goto unlock;
		}
	}

	/* Perform clock-specific frequency switch operations. */
	ASSERT(clk->set_rate);
	clk->set_rate(clk, nf);

	/*
	 * Current freq must be updated before __local_clk_enable_reg()
	 * is called to make sure the MNCNTR_EN bit is set correctly.
	 */
	clk->current_freq = nf;

	/* Enable any clocks that were disabled. */
	if (clk->bank_masks == NULL) {
		if (clk->enabled)
			__local_clk_enable_reg(clk);
	}

unlock:
	return rc;
}

/* Set a clock to an exact rate. */
int local_clk_set_rate(struct clk *c, unsigned rate)
{
	struct rcg_clk *clk = to_rcg_clk(c);
	struct clk_freq_tbl *nf;

	for (nf = clk->freq_tbl; nf->freq_hz != FREQ_END
			&& nf->freq_hz != rate; nf++)
		;

	if (nf->freq_hz == FREQ_END)
		return ERR_INVALID_ARGS;

	return _local_clk_set_rate(clk, nf);
}

/* Get the currently-set rate of a clock in Hz. */
unsigned local_clk_get_rate(struct clk *c)
{
	/* TODO: Stub function for now. */
	return 0;
}

/* Check if a clock is currently enabled. */
int local_clk_is_enabled(struct clk *clk)
{
	return to_rcg_clk(clk)->enabled;
}

/* Return a supported rate that's at least the specified rate. */
long local_clk_round_rate(struct clk *c, unsigned rate)
{
	struct rcg_clk *clk = to_rcg_clk(c);
	struct clk_freq_tbl *f;

	for (f = clk->freq_tbl; f->freq_hz != FREQ_END; f++)
		if (f->freq_hz >= rate)
			return f->freq_hz;

	return ERR_INVALID_ARGS;
}

struct clk *local_clk_get_parent(struct clk *clk)
{
	return to_rcg_clk(clk)->current_freq->src_clk;
}

/*
 * Branch clocks functions
 */
int branch_clk_enable(struct clk *clk)
{
	struct branch_clk *branch = to_branch_clk(clk);

	__branch_clk_enable_reg(&branch->b, branch->c.dbg_name);
	branch->enabled = true;

	return 0;
}

void branch_clk_disable(struct clk *clk)
{
	/* TODO: Stub function for now */
}

struct clk *branch_clk_get_parent(struct clk *clk)
{
	struct branch_clk *branch = to_branch_clk(clk);
	return branch->parent;
}

int branch_clk_set_parent(struct clk *clk, struct clk *parent)
{
	/* This is a stub function. */
	return 0;
}

int branch_clk_is_enabled(struct clk *clk)
{
	struct branch_clk *branch = to_branch_clk(clk);
	return branch->enabled;
}

/**/
/* For clocks with MND dividers. */
void set_rate_mnd(struct rcg_clk *clk, struct clk_freq_tbl *nf)
{
	uint32_t ns_reg_val, ctl_reg_val;

	/* Assert MND reset. */
	ns_reg_val = readl_relaxed(clk->ns_reg);
	ns_reg_val |= BIT(7);
	writel_relaxed(ns_reg_val, clk->ns_reg);

	/* Program M and D values. */
	writel_relaxed(nf->md_val, clk->md_reg);

	/* If the clock has a separate CC register, program it. */
	if (clk->ns_reg != clk->b.ctl_reg) {
		ctl_reg_val = readl_relaxed(clk->b.ctl_reg);
		ctl_reg_val &= ~(clk->ctl_mask);
		ctl_reg_val |= nf->ctl_val;
		writel_relaxed(ctl_reg_val, clk->b.ctl_reg);
	}

	/* Deassert MND reset. */
	ns_reg_val &= ~BIT(7);
	writel_relaxed(ns_reg_val, clk->ns_reg);
}

void set_rate_mnd_banked(struct rcg_clk *clk, struct clk_freq_tbl *nf)
{
	struct bank_masks *banks = clk->bank_masks;
	const struct bank_mask_info *new_bank_masks;
	const struct bank_mask_info *old_bank_masks;
	uint32_t ns_reg_val, ctl_reg_val;
	uint32_t bank_sel;

	/*
	 * Determine active bank and program the other one. If the clock is
	 * off, program the active bank since bank switching won't work if
	 * both banks aren't running.
	 */
	ctl_reg_val = readl_relaxed(clk->b.ctl_reg);
	bank_sel = !!(ctl_reg_val & banks->bank_sel_mask);

	/* If clock isn't running, don't switch banks. */
	bank_sel ^= (!clk->enabled || clk->current_freq->freq_hz == 0);
	if (bank_sel == 0) {
		new_bank_masks = &banks->bank1_mask;
		old_bank_masks = &banks->bank0_mask;
	} else {
		new_bank_masks = &banks->bank0_mask;
		old_bank_masks = &banks->bank1_mask;
	}

	ns_reg_val = readl_relaxed(clk->ns_reg);

	/* Assert bank MND reset. */
	ns_reg_val |= new_bank_masks->rst_mask;
	writel_relaxed(ns_reg_val, clk->ns_reg);

	/*
	 * Program NS only if the clock is enabled, since the NS will be set
	 * as part of the enable procedure and should remain with a low-power
	 * MUX input selected until then.
	 */
	if (clk->enabled) {
		ns_reg_val &= ~(new_bank_masks->ns_mask);
		ns_reg_val |= (nf->ns_val & new_bank_masks->ns_mask);
		writel_relaxed(ns_reg_val, clk->ns_reg);
	}

	writel_relaxed(nf->md_val, new_bank_masks->md_reg);

	/* Enable counter only if clock is enabled. */
	if (clk->enabled)
		ctl_reg_val |= new_bank_masks->mnd_en_mask;
	else
		ctl_reg_val &= ~(new_bank_masks->mnd_en_mask);

	ctl_reg_val &= ~(new_bank_masks->mode_mask);
	ctl_reg_val |= (nf->ctl_val & new_bank_masks->mode_mask);
	writel_relaxed(ctl_reg_val, clk->b.ctl_reg);

	/* Deassert bank MND reset. */
	ns_reg_val &= ~(new_bank_masks->rst_mask);
	writel_relaxed(ns_reg_val, clk->ns_reg);

	/*
	 * Switch to the new bank if clock is running.  If it isn't, then
	 * no switch is necessary since we programmed the active bank.
	 */
	if (clk->enabled && clk->current_freq->freq_hz) {
		ctl_reg_val ^= banks->bank_sel_mask;
		writel_relaxed(ctl_reg_val, clk->b.ctl_reg);
		/*
		 * Wait at least 6 cycles of slowest bank's clock
		 * for the glitch-free MUX to fully switch sources.
		 */
		udelay(1);

		/* Disable old bank's MN counter. */
		ctl_reg_val &= ~(old_bank_masks->mnd_en_mask);
		writel_relaxed(ctl_reg_val, clk->b.ctl_reg);

		/* Program old bank to a low-power source and divider. */
		ns_reg_val &= ~(old_bank_masks->ns_mask);
		ns_reg_val |= (clk->freq_tbl->ns_val & old_bank_masks->ns_mask);
		writel_relaxed(ns_reg_val, clk->ns_reg);
	}

	/*
	 * If this freq requires the MN counter to be enabled,
	 * update the enable mask to match the current bank.
	 */
	if (nf->mnd_en_mask)
		nf->mnd_en_mask = new_bank_masks->mnd_en_mask;
	/* Update the NS mask to match the current bank. */
	clk->ns_mask = new_bank_masks->ns_mask;
}

void set_rate_nop(struct rcg_clk *clk, struct clk_freq_tbl *nf)
{
	/*
	 * Nothing to do for fixed-rate or integer-divider clocks. Any settings
	 * in NS registers are applied in the enable path, since power can be
	 * saved by leaving an un-clocked or slowly-clocked source selected
	 * until the clock is enabled.
	 */
}

