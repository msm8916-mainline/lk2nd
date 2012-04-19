/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>
#include <kernel/thread.h>
#include <platform/iomap.h>
#include <reg.h>
#include <smem.h>
#include <debug.h>
#include <mmc.h>

#define ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))
#define BIT(x)	(1 << (x))

#define A11S_CLK_CNTL_ADDR	(MSM_CSR_BASE + 0x100)
#define A11S_CLK_SEL_ADDR	(MSM_CSR_BASE + 0x104)
#define VDD_SVS_PLEVEL_ADDR	(MSM_CSR_BASE + 0x124)

#define PLL2_MODE_ADDR		(MSM_CLK_CTL_BASE + 0x338)
#define PLL4_MODE_ADDR		(MSM_CLK_CTL_BASE + 0x374)

#define PLL_RESET_N			BIT(2)
#define PLL_BYPASSNL		BIT(1)
#define PLL_OUTCTRL			BIT(0)

#define SRC_SEL_TCX0	0	/* TCXO */
#define SRC_SEL_PLL1	1	/* PLL1: modem_pll */
#define SRC_SEL_PLL2	2	/* PLL2: backup_pll_0 */
#define SRC_SEL_PLL3	3	/* PLL3: backup_pll_1 */
#define SRC_SEL_PLL4	6	/* PLL4: sparrow_pll */

#define DIV_1		0
#define DIV_2		1
#define DIV_3		2
#define DIV_4		3
#define DIV_5		4
#define DIV_6		5
#define DIV_7		6
#define DIV_8		7
#define DIV_9		8
#define DIV_10		9
#define DIV_11		10
#define DIV_12		11
#define DIV_13		12
#define DIV_14		13
#define DIV_15		14
#define DIV_16		15

#define WAIT_CNT	100
#define VDD_LEVEL	7
#define MIN_AXI_HZ	120000000
#define ACPU_800MHZ	41

#define A11S_CLK_SEL_MASK 0x1	/* bits 2:0 */

/* The stepping frequencies have been choosen to make sure the step
 * is <= 256 MHz for both 7x27a and 7x25a targets.  The
 * table also assumes the ACPU is running at TCXO freq and AHB div is
 * set to DIV_1.
 *
 * To use the tables:
 * - Start at location 0/1 depending on clock source sel bit.
 * - Set values till end of table skipping every other entry.
 * - When you reach the end of the table, you are done scaling.
 */

uint32_t const clk_cntl_reg_val_7627A[] = {
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 4) | DIV_16,
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 12) | (DIV_8 << 8),
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 4) | DIV_4,
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 12) | (DIV_2 << 8),

	/* TODO: Fix it for 800MHz */
#if 0
	(WAIT_CNT << 16) | (SRC_SEL_PLL4 << 4) | DIV_1,
#endif
};

/*
 * Use PLL4 to run acpu @ 1.2 GHZ
 */
uint32_t const clk_cntl_reg_val_8X25[] = {
	(WAIT_CNT << 16) | (SRC_SEL_PLL4 << 4)  | DIV_2,
	(WAIT_CNT << 16) | (SRC_SEL_PLL4 << 12) | (DIV_1 << 8),
};

uint32_t const clk_cntl_reg_val_7625A[] = {
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 4) | DIV_16,
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 12) | (DIV_8 << 8),
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 4) | DIV_4,
	(WAIT_CNT << 16) | (SRC_SEL_PLL2 << 12) | (DIV_2 << 8),
};

/* Using DIV_1 for all cases to avoid worrying about turbo vs. normal
 * mode. Able to use DIV_1 for all steps because it's the largest AND
 * the final value. */
uint32_t const clk_sel_reg_val[] = {
	DIV_1 << 1 | 1,		/* Switch to src1 */
	DIV_1 << 1 | 0,		/* Switch to src0 */
};

/*
 * Mask to make sure current selected src frequency doesn't change.
 */
uint32_t const clk_cntl_mask[] = {
	0x0000FF00,		/* Mask to read src0 */
	0x000000FF		/* Mask to read src1 */
};

/* enum for SDC CLK IDs */
enum {
	SDC1_CLK = 19,
	SDC1_PCLK = 20,
	SDC2_CLK = 21,
	SDC2_PCLK = 22,
	SDC3_CLK = 23,
	SDC3_PCLK = 24,
	SDC4_CLK = 25,
	SDC4_PCLK = 26
};

/* Zero'th entry is dummy */
static uint8_t sdc_clk[] = { 0, SDC1_CLK, SDC2_CLK, SDC3_CLK, SDC4_CLK };
static uint8_t sdc_pclk[] = { 0, SDC1_PCLK, SDC2_PCLK, SDC3_PCLK, SDC4_PCLK };

void mdelay(unsigned msecs);
unsigned board_msm_id(void);

void pll_enable(void *pll_mode_addr)
{
	/* TODO: Need to add spin-lock to avoid race conditions */

	uint32_t nVal;
	/* Check status */
	nVal = readl(pll_mode_addr);
	if (nVal & PLL_OUTCTRL)
		return;

	/* Put the PLL in reset mode */
	nVal = 0;
	nVal &= ~PLL_RESET_N;
	nVal &= ~PLL_BYPASSNL;
	nVal &= ~PLL_OUTCTRL;
	writel(nVal, pll_mode_addr);

	/* Put the PLL in warm-up mode */
	nVal |= PLL_RESET_N;
	nVal |= PLL_BYPASSNL;
	writel(nVal, pll_mode_addr);

	/* Wait for the PLL warm-up time */
	udelay(50);

	/* Put the PLL in active mode */
	nVal |= PLL_RESET_N;
	nVal |= PLL_BYPASSNL;
	nVal |= PLL_OUTCTRL;
	writel(nVal, pll_mode_addr);
}

void pll_request(unsigned pll, unsigned enable)
{
	int val = 0;
	if (!enable) {
		/* Disable not supported */
		return;
	}
	switch (pll) {
	case 2:
		pll_enable(PLL2_MODE_ADDR);
		return;
	case 4:
		pll_enable(PLL4_MODE_ADDR);
		return;
	default:
		return;
	};
}

void acpu_clock_init(void)
{
	uint32_t i, clk;
	uint32_t val;
	uint32_t *clk_cntl_reg_val, size;
	unsigned msm_id;

	/* Increase VDD level to the final value. */
	writel((1 << 7) | (VDD_LEVEL << 3), VDD_SVS_PLEVEL_ADDR);

#if (!ENABLE_NANDWRITE)
	thread_sleep(1);
#else
	mdelay(1);
#endif

	msm_id = board_msm_id();
	switch (msm_id) {
	case MSM7227A:
	case MSM7627A:
	case ESM7227A:
		clk_cntl_reg_val = clk_cntl_reg_val_7627A;
		size = ARRAY_SIZE(clk_cntl_reg_val_7627A);
		pll_request(2, 1);

		/* TODO: Enable this PLL while switching to 800MHz */
#if 0
		pll_request(4, 1);
#endif
		break;
	case MSM8625:
		/* Fix me: Will move to PLL4 later */
		clk_cntl_reg_val = clk_cntl_reg_val_7627A;
		size = ARRAY_SIZE(clk_cntl_reg_val_7627A);
		pll_request(2, 1);
		break;

	case MSM7225A:
	case MSM7625A:
	default:
		clk_cntl_reg_val = clk_cntl_reg_val_7625A;
		size = ARRAY_SIZE(clk_cntl_reg_val_7625A);
		pll_request(2, 1);
		break;
	};

	/* Read clock source select bit. */
	val = readl(A11S_CLK_SEL_ADDR);
	i = val & 1;

	/* Jump into table and set every entry. */
	for (; i < size; i++) {

		val = readl(A11S_CLK_SEL_ADDR);
		val |= BIT(1) | BIT(2);
		writel(val, A11S_CLK_SEL_ADDR);

		val = readl(A11S_CLK_CNTL_ADDR);

		/* Make sure not to disturb already used src */
		val &= clk_cntl_mask[i % 2];
		val += clk_cntl_reg_val[i];
		writel(val, A11S_CLK_CNTL_ADDR);

		/* Would need a dmb() here but the whole address space is
		 * strongly ordered, so it should be fine.
		 */
		val = readl(A11S_CLK_SEL_ADDR);
		val &= ~(A11S_CLK_SEL_MASK);
		val |= (A11S_CLK_SEL_MASK & clk_sel_reg_val[i % 2]);
		writel(val, A11S_CLK_SEL_ADDR);

#if (!ENABLE_NANDWRITE)
		thread_sleep(1);
#else
		mdelay(1);
#endif
	}
}

void hsusb_clock_init(void)
{
	/* USB local clock control not enabled; use proc comm */
	usb_clock_init();
}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	uint32_t reg = 0;

	if (mmc_clock_set_rate(sdc_clk[interface], freq) < 0) {
		dprintf(CRITICAL, "Failure setting clock rate for MCLK - "
			"clk_rate: %d\n!", freq);
		ASSERT(0);
	}

	/* enable clock */
	if (mmc_clock_enable_disable(sdc_clk[interface], MMC_CLK_ENABLE) < 0) {
		dprintf(CRITICAL, "Failure enabling MMC Clock!\n");
		ASSERT(0);
	}

	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel(reg, MMC_BOOT_MCI_CLK);
}

/* Intialize MMC clock */
void clock_init_mmc(uint32_t interface)
{
	if (mmc_clock_enable_disable(sdc_pclk[interface], MMC_CLK_ENABLE) < 0) {
		dprintf(CRITICAL, "Failure enabling PCLK!\n");
		ASSERT(0);
	}
}
