// SPDX-License-Identifier: GPL-2.0-only

#include <arch/defines.h>
#include <bits.h>
#include <debug.h>
#include <kernel/thread.h>
#include <platform/timer.h>
#include <reg.h>

#include "../cpu-boot.h"

#define CPU_PWR_CTL			0x4
#define APC_PWR_GATE_CTL	0x14

#define L1_RST_DIS			0x284

#define L2_VREG_CTL			0x1c
#define L2_PWR_CTL			0x14
#define L2_PWR_CTL_OVERRIDE	0xc
#define L2_PWR_STATUS_L2_HS_STS_MSM8994	(BIT(9) | BIT(28))

// delay for voltage to settle on the core
#define REGULATOR_SETUP_VOLTAGE_TIMEOUT 2000

/**
 * enum msm8994_cpu_node_mpidrs - Enum that used for mapping cores and its mpidrs
 */
enum msm8994_cpu_node_mpidrs {
	CPU0 = 0x0,
	CPU1 = 0x1,
	CPU2 = 0x2,
	CPU3 = 0x3,
	CPU4 = 0x100,
	CPU5 = 0x101,
	CPU6 = 0x102,
	CPU7 = 0x103,
};

/**
 * enum msm8994_cpu_clusters - Enum that used for clusters
 * @first: describes little cluster
 * @second: describes BIG cluster
 */
enum msm8994_cpu_clusters {
	first,
	second,
};

/**
 * struct msm8994_cpu_info - This struct used to describe the core values 
 * needed to enable it
 * @base: this value had got from downstream dts this way: cpu@X -> qcom,acc -> reg
 * @cache_info: description of next-level-cache node in cpu@X
 */
struct msm8994_cpu_info {
	uint32_t base;
	const struct msm8994_cpu_cache_info *cache_info;
};

/**
 * struct msm8994_cpu_cache_info - This struct used to describe cache node 
 * (and children's) values needed to enable the cpu cache
 * @l2ccc_base: this value had got from downstream dts this way: l2-cache -> power-domain -> reg
 * @vctl_base_0: l2-cache -> power-domain -> qcom,vctl-node -> reg (first value of first tuple)
 * @vctl_base_1: l2-cache -> power-domain -> qcom,vctl-node -> reg (first value of second tuple)
 * @vctl_val: l2-cache -> power-domain -> qcom,vctl-val (may be absent, then set as 0)
 */
struct msm8994_cpu_cache_info {
	uint32_t l2ccc_base; 
	uint32_t vctl_base_0;
	uint32_t vctl_base_1;
	uint32_t vctl_val;
};

static const struct msm8994_cpu_cache_info cache_info[] = {
	[first] = {
		.l2ccc_base = 0xf900d000,
		.vctl_base_0 = 0xf9012000,
		.vctl_base_1 = 0xf900d210,
		.vctl_val = 0,
	},
	[second] = {
		.l2ccc_base = 0xf900f000,
		.vctl_base_0 = 0xf9013000,
		.vctl_base_1 = 0xf900f210,
		.vctl_val = 0xb8,
	},
};

static const struct msm8994_cpu_info cpu_info[] = {
	[CPU0] = {
		.base = 0xf908b000,
		.cache_info = &cache_info[first],
	},
	[CPU1] = {
		.base = 0xf909b000,
		.cache_info = &cache_info[first],
	},
	[CPU2] = {
		.base = 0xf90ab000,
		.cache_info = &cache_info[first],
	},
	[CPU3] = {
		.base = 0xf90bb000,
		.cache_info = &cache_info[first],
	},
	[CPU4] = {
		.base = 0xf90cb000,
		.cache_info = &cache_info[second],
	},
	[CPU5] = {
		.base = 0xf90db000,
		.cache_info = &cache_info[second],
	},
	[CPU6] = {
		.base = 0xf90eb000,
		.cache_info = &cache_info[second],
	},
	[CPU7] = {
		.base = 0xf90fb000,
		.cache_info = &cache_info[second],
	},
};

/**
 * msm_spm_turn_on_cpu_rail() - Power on cpu rail before turning on core
 * @vctl_base_0: first qcom,vctl-node reg address
 * @vctl_base_1: second qcom,vctl-node reg address
 * @vctl_val: The value to be set on the rail
 */
static void msm_spm_turn_on_cpu_rail(uint32_t vctl_base_0, uint32_t vctl_base_1,
		unsigned int vctl_val)
{
	if (vctl_base_1) {
		/*
		 * Program Q2S to disable SPM legacy mode and ignore Q2S
		 * channel requests.
		 * bit[1] = qchannel_ignore = 1
		 * bit[2] = spm_legacy_mode = 0
		 */
		writel(0x2, vctl_base_1);
		dsb();
	}

	/* Set the CPU supply regulator voltage */
	vctl_val = (vctl_val & 0xFF);
	writel(vctl_val, vctl_base_0 + L2_VREG_CTL);
	dsb();
	udelay(REGULATOR_SETUP_VOLTAGE_TIMEOUT);

	/* Enable the CPU supply regulator*/
	vctl_val = 0x30080;
	writel(vctl_val, vctl_base_0 + L2_VREG_CTL);
	dsb();
	udelay(REGULATOR_SETUP_VOLTAGE_TIMEOUT);
}

/**
 * power_on_l2_cache_msm8994() - This function used to enable l2 cache
 * @l2ccc_base: value of l2 clock controller reg
 * @vctl_base_0: first qcom,vctl-node reg address
 * @vctl_base_1: second qcom,vctl-node reg address
 * @vctl_val: The value to be set on the rail
 *
 * As l2 cache for first(boot) cluster enabled by lk1st,
 * this function skips it and work only for second cluster
 * Function has a check if cache at @l2ccc_base already enabled
 */
static void power_on_l2_cache_msm8994(uint32_t l2ccc_base, uint32_t vctl_base_0,
		uint32_t vctl_base_1, uint32_t vctl_val) {
	/* Skip if cluster L2 is already powered on */
	if (readl(l2ccc_base + L2_PWR_CTL) & L2_PWR_STATUS_L2_HS_STS_MSM8994)
		return;

	dprintf(INFO, "Powering on L2 cache @ %#08x\n", l2ccc_base);

	if(vctl_val != 0) {
		dprintf(INFO, "Not found qcom,vctl-val for this l2 cache node, so skip msm_spm_turn_on_cpu_rail\n");
		msm_spm_turn_on_cpu_rail(vctl_base_0, vctl_base_1, vctl_val);
	}

	enter_critical_section();

	/* Enable L1 invalidation by h/w */
	writel(0x00000000, l2ccc_base + L1_RST_DIS);
	dsb();

	/* Assert PRESETDBGn */
	writel(0x00400000 , l2ccc_base + L2_PWR_CTL_OVERRIDE);
	dsb();

	/* Close L2/SCU Logic GDHS and power up the cache */
	writel(0x00029716 , l2ccc_base + L2_PWR_CTL);
	dsb();
	udelay(8);

	/* De-assert L2/SCU memory Clamp */
	writel(0x00023716 , l2ccc_base + L2_PWR_CTL);
	dsb();

	/* Wakeup L2/SCU RAMs by deasserting sleep signals */
	writel(0x0002371E , l2ccc_base + L2_PWR_CTL);
	dsb();
	udelay(8);

	/* Un-gate clock and wait for sequential waking up
	 * of L2 rams with a delay of 2*X0 cycles
	 */
	writel(0x0002371C , l2ccc_base + L2_PWR_CTL);
	dsb();
	udelay(4);

	/* De-assert L2/SCU logic clamp */
	writel(0x0002361C , l2ccc_base + L2_PWR_CTL);
	dsb();
	udelay(2);

	/* De-assert L2/SCU logic reset */
	writel(0x00022218 , l2ccc_base + L2_PWR_CTL);
	dsb();
	udelay(4);

	/* Turn on the PMIC_APC */
	writel(0x10022218 , l2ccc_base + L2_PWR_CTL);
	dsb();

	/* De-assert PRESETDBGn */
	writel(0x00000000 , l2ccc_base + L2_PWR_CTL_OVERRIDE);
	dsb();
	exit_critical_section();
}

void cpu_boot_cortex_a_msm8994(uint32_t mpidr)
{
	uint32_t base, l2ccc_base, vctl_base_0, vctl_base_1, vctl_val;

	const struct msm8994_cpu_info *info = &cpu_info[mpidr];
	base = info->base;
	l2ccc_base = info->cache_info->l2ccc_base;
	vctl_base_0 = info->cache_info->vctl_base_0;
	vctl_base_1 = info->cache_info->vctl_base_1;
	vctl_val = info->cache_info->vctl_val;

	if (l2ccc_base)
		power_on_l2_cache_msm8994(l2ccc_base, vctl_base_0, vctl_base_1, vctl_val);

	enter_critical_section();

	/* Assert head switch enable few */
	writel(0x00000001, base + APC_PWR_GATE_CTL);
	dsb();
	udelay(1);

	/* Assert head switch enable rest */
	writel(0x00000003, base + APC_PWR_GATE_CTL);
	dsb();
	udelay(1);

	/* De-assert coremem clamp. This is asserted by default */
	writel(0x00000079, base + CPU_PWR_CTL);
	dsb();
	udelay(2);

	/* Close coremem array gdhs */
	writel(0x0000007D, base + CPU_PWR_CTL);
	dsb();
	udelay(2);

	/* De-assert clamp */
	writel(0x0000003D, base + CPU_PWR_CTL);
	dsb();

	/* De-assert clamp */
	writel(0x0000003C, base + CPU_PWR_CTL);
	dsb();
	udelay(1);

	/* De-assert core0 reset */
	writel(0x0000000C, base + CPU_PWR_CTL);
	dsb();

	/* Assert PWRDUP */
	writel(0x0000008C, base + CPU_PWR_CTL);
	dsb();

	exit_critical_section();
}
