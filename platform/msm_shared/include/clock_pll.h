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
#ifndef CLOCK_PLL_H
#define CLOCK_PLL_H


/*
 * Bit manipulation macros
 */
#define BM(msb, lsb)	(((((uint32_t)-1) << (31-msb)) >> (31-msb+lsb)) << lsb)
#define BVAL(msb, lsb, val)	(((val) << lsb) & BM(msb, lsb))

struct clk;
struct clk_ops;

#define container_of(ptr, type, member) \
	((type *)((addr_t)(ptr) - offsetof(type, member)))


/**
 * struct pll_vote_clk - phase locked loop (HW voteable)
 * @rate: output rate
 * @en_reg: enable register
 * @en_mask: ORed with @en_reg to enable the clock
 * @status_reg: status register
 * @parent: clock source
 * @c: clk
 */
struct pll_vote_clk {
	unsigned long rate;

	void *const en_reg;
	const uint32_t en_mask;

	void *const status_reg;
	const uint32_t status_mask;

	struct clk *parent;
	struct clk c;
};

static inline struct pll_vote_clk *to_pll_vote_clk(struct clk *clk)
{
	return container_of(clk, struct pll_vote_clk, c);
}

/**
 * struct pll_clk - phase locked loop
 * @rate: output rate
 * @mode_reg: enable register
 * @parent: clock source
 * @c: clk
 */
struct pll_clk {
	unsigned long rate;

	void *const mode_reg;

	struct clk *parent;
	struct clk c;
};

static inline struct pll_clk *to_pll_clk(struct clk *clk)
{
	return container_of(clk, struct pll_clk, c);
}

int pll_vote_clk_enable(struct clk *clk);
void pll_vote_clk_disable(struct clk *clk);
unsigned pll_vote_clk_get_rate(struct clk *clk);
struct clk *pll_vote_clk_get_parent(struct clk *clk);
int pll_vote_clk_is_enabled(struct clk *clk);


int pll_clk_enable(struct clk *clk);
void pll_clk_disable(struct clk *clk);
unsigned pll_clk_get_rate(struct clk *clk);
struct clk *pll_clk_get_parent(struct clk *clk);
#endif
