/*
 * Copyright (c) 2012-2014, Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Linux Foundation nor
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

#ifndef __CLOCK_LIB2_H
#define __CLOCK_LIB2_H

#include <bits.h>
/*
 * Bit manipulation macros
 */
#define BM(msb, lsb)        (((((uint32_t)-1) << (31-msb)) >> (31-msb+lsb)) << lsb)
#define BVAL(msb, lsb, val) (((val) << lsb) & BM(msb, lsb))

#define container_of(ptr, type, member) \
        ((type *)((addr_t)(ptr) - offsetof(type, member)))

/* Frequency Macros */
#define FREQ_END        (UINT_MAX-1)
#define F_END \
        { \
                .freq_hz = FREQ_END, \
        }

/* F(frequency, source, div, m, n) */
#define F(f, s, div, m, n) \
        { \
                .freq_hz = (f), \
                .src_clk = &s##_clk_src.c, \
                .m_val   = (m), \
                .n_val   = ~((n)-(m)) * !!(n), \
                .d_val   = ~(n),\
                .div_src_val = BVAL(4, 0, (int)(2*(div) - 1)) \
                              | BVAL(10, 8, s##_source_val), \
        }

/* F(frequency, source, div, m, n) */
#define F_EXT_SRC(f, s, div, m, n) \
        { \
                .freq_hz = (f), \
                .m_val   = (m), \
                .n_val   = ~((n)-(m)) * !!(n), \
                .d_val   = ~(n),\
                .div_src_val = BVAL(4, 0, (int)(2*(div) - 1)) \
                              | BVAL(10, 8, s##_source_val), \
        }

/* F_MM(frequency, source, div, m, n) */
#define F_MM(f, s, div, m, n) \
	{ \
		.freq_hz = (f), \
		.src_clk = &s##_clk_src.c, \
		.m_val = (m), \
		.n_val = ~((n)-(m)) * !!(n), \
		.d_val = ~(n),\
		.div_src_val = BVAL(4, 0, (int)(2*(div) - 1)) \
			| BVAL(10, 8, s##_mm_source_val), \
	}

#define F_MDSS(f, s, div, m, n) \
	{ \
		.freq_hz = (f), \
		.m_val = (m), \
		.n_val = ~((n)-(m)) * !!(n), \
		.d_val = ~(n),\
		.div_src_val = BVAL(4, 0, (int)(2*(div) - 1)) \
			| BVAL(10, 8, s##_mm_source_val), \
	}

/* Branch Clock Bits */
#define CBCR_BRANCH_ENABLE_BIT  BIT(0)
#define CBCR_BRANCH_OFF_BIT     BIT(31)
#define BRANCH_CHECK_MASK       BM(31, 28)
#define BRANCH_ON_VAL           BVAL(31, 28, 0x0)
#define BRANCH_NOC_FSM_ON_VAL   BVAL(31, 28, 0x2)

/* Root Clock Bits */
#define CMD_UPDATE_BIT          BIT(0)
#define CMD_UPDATE_MASK         1

#define CFG_SRC_DIV_OFFSET      0
#define CFG_SRC_DIV_MASK        (0x1F << CFG_SRC_DIV_OFFSET)

#define CFG_SRC_SEL_OFFSET      8
#define CFG_SRC_SEL_MASK        (0x3 << CFG_SRC_SEL_OFFSET)

#define CFG_MODE_DUAL_EDGE      0x2

#define CFG_MODE_OFFSET         12
#define CFG_MODE_MASK           (0x3 << CFG_MODE_OFFSET)


/*
 * Generic frequency-definition structs and macros
 */
struct clk_freq_tbl {

	const uint32_t  freq_hz;
	struct clk     *src_clk;
	const uint32_t  div_src_val;

	/* TODO: find out if sys_vdd is needed. */

	const uint32_t  m_val;
	const uint32_t  n_val; /* not_n_minus_m_val */
	const uint32_t  d_val; /* not_2d_val        */
};

/* Fixed clock */
struct fixed_clk {
	struct clk c;
};

/* Branch clock */
struct branch_clk {

	uint32_t *const bcr_reg;
	uint32_t *const cbcr_reg;

	void   (*set_rate)(struct branch_clk *, struct clk_freq_tbl *);

	struct clk *parent;
	struct clk  c;

	int      has_sibling;
	uint32_t cur_div;
	uint32_t max_div;
	uint32_t halt_check;
};

/* Root Clock */
struct rcg_clk {

	/* RCG registers for this clock */

	uint32_t *const cmd_reg; /* Command reg */
	uint32_t *const cfg_reg; /* Config  reg */
	uint32_t *const m_reg;   /*       m     */
	uint32_t *const n_reg;   /* not (n-m)   */
	uint32_t *const d_reg;   /* not (2d)    */

	/* set rate function for this clock */
	void   (*set_rate)(struct rcg_clk *, struct clk_freq_tbl *);

	/* freq table */
	struct clk_freq_tbl *freq_tbl;
	struct clk_freq_tbl *current_freq;

	struct clk c;
};

/* Vote Clock */
struct vote_clk {

	uint32_t *const cbcr_reg;
	uint32_t *const vote_reg;
	uint32_t en_mask;

    struct clk c;
};

struct reset_clk {
	uint32_t bcr_reg;
	struct clk c;
};

static inline struct reset_clk *to_reset_clk(struct clk *clk)
{
	return container_of(clk, struct reset_clk, c);
}

static inline struct rcg_clk *to_rcg_clk(struct clk *clk)
{
	return container_of(clk, struct rcg_clk, c);
}

static inline struct branch_clk *to_branch_clk(struct clk *clk)
{
	return container_of(clk, struct branch_clk, c);
}

static inline struct vote_clk *to_local_vote_clk(struct clk *clk)
{
	return container_of(clk, struct vote_clk, c);
}

/* RCG clock functions */
int  clock_lib2_rcg_enable(struct clk *c);
int  clock_lib2_rcg_set_rate(struct clk *c, unsigned rate);
void clock_lib2_rcg_set_rate_mnd(struct rcg_clk *rclk, struct clk_freq_tbl *freq);
void clock_lib2_rcg_set_rate_hid(struct rcg_clk *rclk, struct clk_freq_tbl *freq);

/* CXO clock functions */
int  cxo_clk_enable(struct clk *clk);
void cxo_clk_disable(struct clk *clk);

/* Branch clock functions */
int  clock_lib2_branch_clk_enable(struct clk *clk);
void clock_lib2_branch_clk_disable(struct clk *clk);
int  clock_lib2_branch_set_rate(struct clk *c, unsigned rate);

/* Vote clock functions*/
int clock_lib2_vote_clk_enable(struct clk *c);
void clock_lib2_vote_clk_disable(struct clk *c);
/* clock reset function */
int clock_lib2_reset_clk_reset(struct clk *c, enum clk_reset_action action);
int clock_lib2_branch_clk_reset(struct clk *c, enum clk_reset_action action);
#endif
