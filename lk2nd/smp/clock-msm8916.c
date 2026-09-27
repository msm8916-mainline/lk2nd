// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <board.h>
#include <boot.h>
#include <clock.h>
#include <platform/iomap.h>
#include <reg.h>

#define PLL_MODE			0x00
#define PLL_L_VAL			0x04
#define PLL_M_VAL			0x08
#define PLL_N_VAL			0x0c
#define PLL_USER_CTL			0x10
#define PLL_CONFIG_CTL			0x14

#define PLL_USER_CTL_PLLOUT_LV_MAIN	BIT(0)
#define PLL_USER_CTL_PLLOUT_LV_AUX	BIT(1)
#define PLL_USER_CTL_PLLOUT_LV_AUX2	BIT(2)
#define PLL_USER_CTL_PLLOUT_LV_EARLY	BIT(3)
#define PLL_USER_CTL_MN_EN		BIT(24)

#define MHZ				1000000

static bool platform_has_second_cluster(void)
{
	switch (board_platform_id()) {
		case APQ8029:
		case APQ8039:
		case MSM8229:
		case MSM8239:
		case MSM8629:
		case MSM8929:
		case MSM8939:
			return true;
		default:
			/* MSM8916/MSM8936 have only one CPU cluster */
			return false;
	};
}

static void setup_little_cluster_clocks(void)
{
	if (!platform_has_second_cluster())
		return;

	/* Initialize little cores with 400 MHz */
	clk_get_set_enable("a53ssmux_lc", 400 * MHZ, 0);

	/* Avoid touching PLL if it is already enabled for some reason */
	if (readl(APCS_LC_PLL_BASE + PLL_MODE))
		return;

	/* Initialize PLL configuration to avoid extra code in Linux */
	writel(PLL_USER_CTL_PLLOUT_LV_MAIN | PLL_USER_CTL_PLLOUT_LV_EARLY,
	       APCS_LC_PLL_BASE + PLL_USER_CTL);
	writel(0x4c015765, APCS_LC_PLL_BASE + PLL_CONFIG_CTL);
}

static int lk2nd_smp_clock_setup(void *dtb, const char *cmdline,
				  enum boot_type boot_type)
{
	/* Downstream sets this itself */
	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	setup_little_cluster_clocks();
	return 0;
}
DEV_TREE_UPDATE(lk2nd_smp_clock_setup);
