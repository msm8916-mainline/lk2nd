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

#ifndef CLK_APLHA_PLL
#define CLK_APLHA_PLL

#include <platform/iomap.h>
#include <platform/clock.h>

#define L_REG(pll) (pll->base + pll->offset + 0x4)
#define A_REG(pll) (pll->base + pll->offset + 0x8)
#define A_REG_U(pll) (pll->base + pll->offset + 0xC)
#define ACTIVE_REG(pll) (pll->base + pll->offset + 0x0)
#define VOTE_REG(pll) (pll->base + pll->fsm_reg_offset)
#define VCO_REG(pll) (pll->base + pll->offset + 0x10)
#define OUTPUT_REG(pll) (pll->base + pll->offset + 0x10)
#define USER_CTL_LO_REG(pll) (pll->base + pll->offset + 0x10)
#define MODE_REG(pll) (pll->base + pll->offset + 0x0)
#define LOCK_REG(pll) (pll->base + pll->offset + 0x0)
#define ALPHA_EN_REG(pll) (pll->base + pll->offset + 0x10)

#define PLL_VOTE_FSM_ENA     BIT(20)
#define PLL_VOTE_FSM_RESET   BIT(21)
#define PLL_BIAS_COUNT_START 14
#define PLL_BIAS_COUNT_END   19
#define PLL_BIAS_COUNT       0x6
#define PLL_LOCK_COUNT_START 8
#define PLL_LOCK_COUNT_END   13

#define VCO(a, b, c) { \
	.vco_val = a,\
	.min_freq = b,\
	.max_freq = c,\
}

struct alpha_pll_vco_tbl {
	uint32_t vco_val;
	unsigned long min_freq;
	unsigned long max_freq;
};

struct alpha_pll_masks {
	uint32_t lock_mask;      /* lock_det bit */
	uint32_t active_mask;    /* active_flag in FSM mode */
	uint32_t update_mask;    /* update bit for dynamic update */
	uint32_t vco_mask;       /* vco_sel bits */
	uint32_t vco_shift;
	uint32_t alpha_en_mask;  /* alpha_en bit */
	uint32_t output_mask;    /* pllout_* bits */
	uint32_t post_div_mask;
};

struct alpha_pll_clk {
	struct alpha_pll_masks *masks;
	uint32_t offset;
	uint32_t base;
	struct alpha_pll_vco_tbl *vco_tbl;
	uint32_t vco_num;
	/* if fsm_en_mask is set, config PLL to FSM mode */
	uint32_t fsm_reg_offset;
	uint32_t fsm_en_mask;
	uint32_t enable_config;  /* bitmask of outputs to be enabled */
	uint32_t post_div_config;    /* masked post divider setting */
	bool inited;
	struct clk *parent;
	struct clk c;
};

void alpha_pll_disable(struct clk *c);
int  alpha_pll_enable(struct clk *c);

#endif
