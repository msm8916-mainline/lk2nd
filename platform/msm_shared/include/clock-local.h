/*
 * Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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

#ifndef CLOCK_LOCAL_H
#define CLOCK_LOCAL_H

struct clk;
struct clk_ops;

#define FREQ_END	(UINT_MAX-1)
#define F_END \
	{ \
		.freq_hz = FREQ_END, \
	}

#define container_of(ptr, type, member) \
	((type *)((addr_t)(ptr) - offsetof(type, member)))

/*
 * Bit manipulation macros
 */
#define BM(msb, lsb)	(((((uint32_t)-1) << (31-msb)) >> (31-msb+lsb)) << lsb)
#define BVAL(msb, lsb, val)	(((val) << lsb) & BM(msb, lsb))

/*
 * Halt/Status Checking Mode Macros
 */
#define HALT            0   /* Bit pol: 1 = halted */
#define NOCHECK         1   /* No bit to check, do nothing */
#define HALT_VOTED      2   /* Bit pol: 1 = halted; delay on disable */
#define ENABLE          3   /* Bit pol: 1 = running */
#define ENABLE_VOTED    4   /* Bit pol: 1 = running; delay on disable */
#define DELAY           5   /* No bit to check, just delay */

/*
 * Variables from clock-local driver
 */
extern struct fixed_clk gnd_clk;

/*
 * Generic frequency-definition structs and macros
 */
struct clk_freq_tbl {
	const uint32_t	freq_hz;
	struct clk	*src_clk;
	const uint32_t	md_val;
	const uint32_t	ns_val;
	const uint32_t	ctl_val;
	uint32_t	mnd_en_mask;
	void		*const extra_freq_data;
};

extern struct clk_freq_tbl local_dummy_freq;

/* Some clocks have two banks to avoid glitches when switching frequencies.
 * The unused bank is programmed while running on the other bank, and
 * switched to afterwards. The following two structs describe the banks. */
struct bank_mask_info {
	void *const md_reg;
	const uint32_t	ns_mask;
	const uint32_t	rst_mask;
	const uint32_t	mnd_en_mask;
	const uint32_t	mode_mask;
};

struct bank_masks {
	const uint32_t			bank_sel_mask;
	const struct bank_mask_info	bank0_mask;
	const struct bank_mask_info	bank1_mask;
};

/**
 * struct branch - branch on/off
 * @ctl_reg: clock control register
 * @en_mask: ORed with @ctl_reg to enable the clock
 * @halt_reg: halt register
 * @halt_check: type of halt check to perform
 * @halt_bit: ANDed with @halt_reg to test for clock halted
 * @reset_reg: reset register
 * @reset_mask: ORed with @reset_reg to reset the clock domain
 */
struct branch {
	void *const ctl_reg;
	const uint32_t en_mask;

	void *const halt_reg;
	const uint16_t halt_check;
	const uint16_t halt_bit;

	void *const reset_reg;
	const uint32_t reset_mask;
};

/*
 * Generic clock-definition struct and macros
 */
struct rcg_clk {
	bool		enabled;
	void		*const ns_reg;
	void		*const md_reg;

	const uint32_t	root_en_mask;
	uint32_t	ns_mask;
	const uint32_t	ctl_mask;
	struct bank_masks *const bank_masks;

	void   (*set_rate)(struct rcg_clk *, struct clk_freq_tbl *);
	struct clk_freq_tbl *freq_tbl;
	struct clk_freq_tbl *current_freq;

	struct clk *depends;
	struct branch	b;
	struct clk	c;
};

static inline struct rcg_clk *to_rcg_clk(struct clk *clk)
{
	return container_of(clk, struct rcg_clk, c);
}

/**
 * struct fixed_clk - fixed rate clock (used for crystal oscillators)
 * @rate: output rate
 * @c: clk
 */
struct fixed_clk {
	unsigned long rate;
	struct clk c;
};

static inline struct fixed_clk *to_fixed_clk(struct clk *clk)
{
	return container_of(clk, struct fixed_clk, c);
}

static inline unsigned fixed_clk_get_rate(struct clk *clk)
{
	struct fixed_clk *f = to_fixed_clk(clk);
	return f->rate;
}

/**
 * struct branch_clk - branch
 * @enabled: true if clock is on, false otherwise
 * @b: branch
 * @parent: clock source
 * @c: clk
 *
 * An on/off switch with a rate derived from the parent.
 */
struct branch_clk {
	bool enabled;
	struct branch b;
	struct clk *parent;
	struct clk c;
};

static inline struct branch_clk *to_branch_clk(struct clk *clk)
{
	return container_of(clk, struct branch_clk, c);
}

int branch_clk_enable(struct clk *clk);
void branch_clk_disable(struct clk *clk);
struct clk *branch_clk_get_parent(struct clk *clk);
int branch_clk_set_parent(struct clk *clk, struct clk *parent);
int branch_clk_is_enabled(struct clk *clk);
void branch_clk_auto_off(struct clk *clk);
int branch_clk_reset(struct clk *c, enum clk_reset_action action);

/**
 * struct measure_clk - for rate measurement debug use
 * @sample_ticks: sample period in reference clock ticks
 * @multiplier: measurement scale-up factor
 * @divider: measurement scale-down factor
 * @c: clk
*/
struct measure_clk {
	uint64_t sample_ticks;
	uint32_t multiplier;
	uint32_t divider;
	struct clk c;
};

extern struct clk_ops clk_ops_measure;

static inline struct measure_clk *to_measure_clk(struct clk *clk)
{
	return container_of(clk, struct measure_clk, c);
}

/*
 * clk_ops APIs
 */
int local_clk_enable(struct clk *c);
void local_clk_disable(struct clk *c);
int local_clk_set_rate(struct clk *c, unsigned rate);
unsigned local_clk_get_rate(struct clk *c);
int local_clk_is_enabled(struct clk *clk);
long local_clk_round_rate(struct clk *c, unsigned rate);
struct clk *local_clk_get_parent(struct clk *clk);

/*
 * Generic set-rate implementations
 */
void set_rate_mnd(struct rcg_clk *clk, struct clk_freq_tbl *nf);
void set_rate_mnd_banked(struct rcg_clk *clk, struct clk_freq_tbl *nf);
void set_rate_nop(struct rcg_clk *clk, struct clk_freq_tbl *nf);
#endif
