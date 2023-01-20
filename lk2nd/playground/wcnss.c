/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/defines.h>
#include <bits.h>
#include <debug.h>
#include <fastboot.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <reg.h>
#include <string.h>

#include <clock.h>
#include <clock_pll.h>
#include <clock_lib2.h>

#include <lk2nd/playground.h>

#define PRONTO_PMU_COMMON_GDSCR				0x24
#define PRONTO_PMU_COMMON_GDSCR_SW_COLLAPSE		BIT(0)
#define CLK_DIS_WAIT					12
#define EN_FEW_WAIT					16
#define EN_REST_WAIT					20

#define PRONTO_PMU_COMMON_CPU_CBCR			0x30
#define PRONTO_PMU_COMMON_CPU_CBCR_CLK_EN		BIT(0)
#define PRONTO_PMU_COMMON_CPU_CLK_OFF			BIT(31)

#define PRONTO_PMU_COMMON_AHB_CBCR			0x34
#define PRONTO_PMU_COMMON_AHB_CBCR_CLK_EN		BIT(0)
#define PRONTO_PMU_COMMON_AHB_CLK_OFF			BIT(31)

#define PRONTO_PMU_CPU_AHB_CMD_RCGR			0x120
#define PRONTO_PMU_CPU_AHB_CFG_RCGR			0x124
#define PRONTO_PMU_PLL_MODE				0x1C0
#define PRONTO_PMU_PLL_USER_CTL				0x1D0
#define PRONTO_PMU_PLL_CONFIG_CTL			0x1D4

#define PRONTO_PMU_COMMON_CSR				0x1040
#define PRONTO_PMU_COMMON_CSR_A2XB_CFG_EN		BIT(0)

#define PRONTO_PMU_SOFT_RESET				0x104C
#define PRONTO_PMU_SOFT_RESET_CRCM_CCPU_SOFT_RESET	BIT(10)

#define PRONTO_PMU_CCPU_CTL				0x2000
#define PRONTO_PMU_CCPU_CTL_REMAP_EN			BIT(2)
#define PRONTO_PMU_CCPU_CTL_HIGH_IVT			BIT(0)

#define PRONTO_PMU_CCPU_BOOT_REMAP_ADDR			0x2004

#define PRONTO_PMU_SPARE				0x1088
#define PRONTO_PMU_SPARE_SSR_BIT			BIT(23)

#define CLK_CTL_WCNSS_RESTART_BIT			BIT(0)

#define AXI_HALTREQ					0x0
#define AXI_HALTACK					0x4
#define AXI_IDLE					0x8

#define HALT_ACK_TIMEOUT_US				500000
#define CLK_UPDATE_TIMEOUT_US				500000

#define GCC_WCSS_RESTART				0x01811000

#define cxo_source_val		0
#define wcnpll_source_val	1
static struct clk_freq_tbl rcg_dummy_freq = F_END;

static struct clk_ops clk_ops_cxo =
{
	.enable     = cxo_clk_enable,
	.disable    = cxo_clk_disable,
};

static struct clk_ops clk_ops_pll =
{
	.enable = pll_clk_enable,
	.disable = pll_clk_disable,
	.get_rate = pll_clk_get_rate,
	.get_parent = pll_clk_get_parent,
};

static struct clk_ops clk_ops_rcg =
{
	.enable     = clock_lib2_rcg_enable,
	.set_rate   = clock_lib2_rcg_set_rate,
};

static struct clk_ops clk_ops_branch =
{
	.enable     = clock_lib2_branch_clk_enable,
	.disable    = clock_lib2_branch_clk_disable,
	.set_rate   = clock_lib2_branch_set_rate,
};

static struct fixed_clk cxo_clk_src =
{
	.c = {
		.rate     = 19200000,
		.dbg_name = "cxo_clk_src",
		.ops      = &clk_ops_cxo,
	},
};

static struct pll_clk wcnpll_clk_src = {
	.rate = 480000000,
	.mode_reg = (void *)(0xa21b000 + PRONTO_PMU_PLL_MODE),
	.parent = &cxo_clk_src.c,
	.c = {
		.dbg_name = "wcnpll_clk",
		.ops = &clk_ops_pll,
	},
};

static struct clk_freq_tbl ftbl_wcss_cpu_ahb[] =
{
	F( 19200000,    cxo,  1, 0, 0),
	F( 32000000, wcnpll, 15, 0, 0),
	F(120000000, wcnpll,  4, 0, 0),
	F(240000000, wcnpll,  2, 0, 0),
	F(480000000, wcnpll,  1, 0, 0),
	F_END
};

static struct rcg_clk wcss_cpu_ahb_clk_src =
{
	.cmd_reg      = (uint32_t *)(0xa21b000 + PRONTO_PMU_CPU_AHB_CMD_RCGR),
	.cfg_reg      = (uint32_t *)(0xa21b000 + PRONTO_PMU_CPU_AHB_CFG_RCGR),
	.set_rate     = clock_lib2_rcg_set_rate_hid,
	.freq_tbl     = ftbl_wcss_cpu_ahb,
	.current_freq = &rcg_dummy_freq,
	.c = {
		.dbg_name = "wcss_cpu_ahb_clk_src",
		.ops      = &clk_ops_rcg,
	},
};

static struct branch_clk wcss_ahb_clk =
{
	.cbcr_reg     = (uint32_t *)(0xa21b000 + PRONTO_PMU_COMMON_AHB_CBCR),
	.parent       = &wcss_cpu_ahb_clk_src.c,
	.halt_check   = 1,
	.c = {
		.dbg_name = "wcss_ahb_clk",
		.ops      = &clk_ops_branch,
	},
};

static struct branch_clk wcss_cpu_clk =
{
	.cbcr_reg     = (uint32_t *)(0xa21b000 + PRONTO_PMU_COMMON_CPU_CBCR),
	.parent       = &wcss_cpu_ahb_clk_src.c,
	.halt_check   = 1,
	.c = {
		.dbg_name = "wcss_cpu_clk",
		.ops      = &clk_ops_branch,
	},
};

static int pil_pronto_reset(uint32_t base, uint32_t start_addr)
{
	uint32_t reg;
	int rc;

	/* Deassert reset to subsystem and wait for propagation */
	reg = readl_relaxed(GCC_WCSS_RESTART);
	reg &= ~CLK_CTL_WCNSS_RESTART_BIT;
	writel_relaxed(reg, GCC_WCSS_RESTART);
	dsb();
	udelay(2);

	/* Set up boot remapper */
	writel_relaxed(start_addr >> 16, base + PRONTO_PMU_CCPU_BOOT_REMAP_ADDR);

	reg = readl_relaxed(base + PRONTO_PMU_CCPU_CTL);
	reg |= PRONTO_PMU_CCPU_CTL_REMAP_EN;
	writel_relaxed(reg, base + PRONTO_PMU_CCPU_CTL);

	/* Setup WCNSS PLL */
	writel_relaxed(0x30000109, base + PRONTO_PMU_PLL_USER_CTL);
	writel_relaxed(0x0300403d, base + PRONTO_PMU_PLL_CONFIG_CTL);

	/* Enable A2XB bridge */
	reg = readl_relaxed(base + PRONTO_PMU_COMMON_CSR);
	reg |= PRONTO_PMU_COMMON_CSR_A2XB_CFG_EN;
	writel_relaxed(reg, base + PRONTO_PMU_COMMON_CSR);

	/* Enable common_ss power */
	reg = readl_relaxed(base + PRONTO_PMU_COMMON_GDSCR);
	reg &= ~PRONTO_PMU_COMMON_GDSCR_SW_COLLAPSE;
	writel_relaxed(reg, base + PRONTO_PMU_COMMON_GDSCR);

	rc = clk_set_rate(&wcss_cpu_clk.c, 240000000);
	if (rc) {
		dprintf(CRITICAL, "Failed to set WCNSS CPU clock rate: %d\n", rc);
		return rc;
	}

	rc = clk_enable(&wcss_ahb_clk.c);
	if (rc) {
		dprintf(CRITICAL, "Failed to enable WCNSS AHB clock: %d\n", rc);
		return rc;
	}
	rc = clk_enable(&wcss_cpu_clk.c);
	if (rc) {
		dprintf(CRITICAL, "Failed to enable WCNSS CPU clock: %d\n", rc);
		return rc;
	}

	/* Initialize frame counter frequency */
	writel(19200000, 0x0A211000);

	/* Route UART IRQs to PHSS UART interrupts */
	writel(BIT(0), 0x0194B080); /* UART1 */
	writel(BIT(1), 0x0194B080 + 0x10); /* UART2 */

	writel(BIT(3), 0x0194B100); /* I2C4 */

	/* Deassert ARM9 software reset */
	reg = readl_relaxed(base + PRONTO_PMU_SOFT_RESET);
	reg &= ~PRONTO_PMU_SOFT_RESET_CRCM_CCPU_SOFT_RESET;
	writel_relaxed(reg, base + PRONTO_PMU_SOFT_RESET);

	return 0;
}

static void cmd_oem_wcnss(const char *arg, void *data, unsigned sz)
{
	extern void wcnss_entry(void);

	dprintf(CRITICAL, "WCNSS start address: %p\n", wcnss_entry);
	pil_pronto_reset(0xa21b000, (uintptr_t)wcnss_entry);
	fastboot_okay("");
}
FASTBOOT_REGISTER("oem wcnss", cmd_oem_wcnss);

void wcnss_handover(uint32_t r0, uint32_t r1, uint32_t r2, void *entry)
{
	extern uint32_t wcnss_spin_addr;
	extern uint32_t wcnss_args[3];

	wcnss_args[0] = r0;
	wcnss_args[1] = r1;
	wcnss_args[2] = r2;
	arch_clean_cache_range((uintptr_t)wcnss_args, sizeof(wcnss_args));

	dprintf(CRITICAL, "Handing over control to WCNSS @ %p\n", entry);

	wcnss_spin_addr = (uint32_t)entry;
	arch_clean_cache_range(wcnss_spin_addr, sizeof(wcnss_spin_addr));

	/* Loop forever on this CPU */
	while (true)
		__asm__ volatile("wfi");
}
