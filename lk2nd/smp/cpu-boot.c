// SPDX-License-Identifier: GPL-2.0-only
#include <arch/defines.h>
#include <bits.h>
#include <debug.h>
#include <platform/timer.h>
#include <reg.h>
#include <scm.h>

#include "cpu-boot.h"

#define CPU_PWR_CTL			0x4
#define APC_PWR_GATE_CTL		0x14

#define CPU_PWR_CTL_CLAMP		BIT(0)
#define CPU_PWR_CTL_CORE_MEM_CLAMP	BIT(1)
#define CPU_PWR_CTL_L1_RST_DIS		BIT(2)
#define CPU_PWR_CTL_CORE_MEM_HS		BIT(3)
#define CPU_PWR_CTL_CORE_RST		BIT(4)
#define CPU_PWR_CTL_COREPOR_RST		BIT(5)
#define CPU_PWR_CTL_GATE_CLK		BIT(6)
#define CPU_PWR_CTL_CORE_PWRD_UP	BIT(7)

#define APC_PWR_GATE_CTL_GHDS_EN	BIT(0)
#define APC_PWR_GATE_CTL_GHDS_CNT(cnt)	((cnt) << 24)

/*
 * Each CPU within a cluster has a clock controller.
 * There is just once L2 cache controller per cluster. We can take the base
 * address of a given CPU clock controller and using a combination of mask
 * and offset find the base address of the L2 cache controller.
 *
 * APCS_GLB_BASE takes the base of given CPU clock controller and offsets
 * back the single APCS_GLB register which is the base for the L2
 * cache controller in the cluster.
 *
 */
#define APCS_GLB_MASK			0xfff00000
#define APCS_GLB_OFFSET			0x00011000
#define APCS_GLB_BASE(base)		(((base) & APCS_GLB_MASK) + \
					APCS_GLB_OFFSET)

#define APCS_PWR_CTL_OVERRIDE			0xc
#define APCS_PWR_CTL_OVR_PRESETDBG		BIT(22)

#define APCS_L2_PWR_CTL				0x14
#define APCS_L2_PWR_CTL_PMIC_APC_ON		BIT(28)
#define APCS_L2_PWR_CTL_L2_HS_CNT(cnt)		((cnt) << 16)
#define APCS_L2_PWR_CTL_L2_ARRAY_HS_CLAMP	BIT(15)
#define APCS_L2_PWR_CTL_SCU_ARRAY_HS_CLAMP	BIT(14)
#define APCS_L2_PWR_CTL_L2_RET_SLP		BIT(13)
#define APCS_L2_PWR_CTL_SYS_RESET		BIT(12)
#define APCS_L2_PWR_CTL_L2_SLEEP_STATE		BIT(11)
#define APCS_L2_PWR_CTL_L2_HS_RST		BIT(10)
#define APCS_L2_PWR_CTL_L2_HS_EN		BIT(9)
#define APCS_L2_PWR_CTL_L2_HS_CLAMP		BIT(8)
#define APCS_L2_PWR_CTL_L2_RST_DIS		BIT(2)
#define APCS_L2_PWR_CTL_SCU_ARRAY_HS		BIT(1)
#define APCS_L2_PWR_CTL_L2_ARRAY_HS		BIT(0)

#define APCS_L2_PWR_STATUS		0x18
#define APCS_L2_PWR_STATUS_L2_HS_STS	BIT(9)

#define APCS_L2_CORE_CBCR		0x58
#define APCS_L2_CORE_CBCR_CLK_OFF	BIT(31)
#define APCS_L2_CORE_CBCR_HW_CTL	BIT(1)
#define APCS_L2_CORE_CBCR_CLK_ENABLE	BIT(0)

#define QCOM_SCM_BOOT_SET_ADDR		0x01
#define QCOM_SCM_BOOT_FLAG_COLD_ALL	(0 | BIT(0) | BIT(3) | BIT(5))
#define QCOM_SCM_BOOT_SET_ADDR_MC	0x11
#define QCOM_SCM_BOOT_MC_FLAG_AARCH64	BIT(0)
#define QCOM_SCM_BOOT_MC_FLAG_COLDBOOT	BIT(1)
#define QCOM_SCM_BOOT_MC_FLAG_WARMBOOT	BIT(2)

static inline uint32_t read_mpidr(void)
{
	uint32_t res;
	__asm__ ("mrc p15, 0, %0, c0, c0, 5" : "=r" (res));
	return res & 0x00ffffff;
}

int qcom_set_boot_addr(uint32_t addr, bool arm64)
{
	uint32_t aarch64 = arm64 ? QCOM_SCM_BOOT_MC_FLAG_AARCH64 : 0;
	scmcall_arg arg = {
		MAKE_SIP_SCM_CMD(SCM_SVC_BOOT, QCOM_SCM_BOOT_SET_ADDR_MC),
		MAKE_SCM_ARGS(6),
		addr,
		~0UL, ~0UL, ~0UL, ~0UL, /* All CPUs */
		aarch64 | QCOM_SCM_BOOT_MC_FLAG_COLDBOOT,
	};

	if (is_scm_armv8_support())
		return scm_call2(&arg, NULL);

	dprintf(INFO, "Falling back to legacy QCOM_SCM_BOOT_SET_ADDR call\n");
	return scm_call_atomic2(SCM_SVC_BOOT, QCOM_SCM_BOOT_SET_ADDR,
				QCOM_SCM_BOOT_FLAG_COLD_ALL, addr);
}

void qcom_power_up_l2_cache(uint32_t mpidr, uint32_t base)
{
	uint32_t val;

	/* Skip if cluster L2 is already powered on */
	val = readl(base + APCS_L2_PWR_STATUS);
	if (val & APCS_L2_PWR_STATUS_L2_HS_STS) {
		dprintf(INFO, "CPU (%d) L2 cache already powered-up\n", mpidr);
		return;
	}

	/* Close L2/SCU Logic GDHS and power up the cache */
	val = APCS_L2_PWR_CTL_L2_HS_CNT(16) |
	      APCS_L2_PWR_CTL_L2_ARRAY_HS_CLAMP |
	      APCS_L2_PWR_CTL_SCU_ARRAY_HS_CLAMP | APCS_L2_PWR_CTL_SYS_RESET |
	      APCS_L2_PWR_CTL_L2_HS_RST | APCS_L2_PWR_CTL_L2_HS_EN |
	      APCS_L2_PWR_CTL_L2_HS_CLAMP;

	writel(val, base + APCS_L2_PWR_CTL);

	/* Assert PRESETDBGn */
	writel(APCS_PWR_CTL_OVR_PRESETDBG, base + APCS_PWR_CTL_OVERRIDE);
	dsb();
	udelay(2);

	/* De-assert L2/SCU memory Clamp */
	val &= ~(APCS_L2_PWR_CTL_L2_ARRAY_HS_CLAMP |
		 APCS_L2_PWR_CTL_SCU_ARRAY_HS_CLAMP);
	writel(val, base + APCS_L2_PWR_CTL);

	/* Wakeup L2/SCU RAMs by deasserting sleep signals */
	val |= (APCS_L2_PWR_CTL_SCU_ARRAY_HS | APCS_L2_PWR_CTL_L2_ARRAY_HS);
	writel(val, base + APCS_L2_PWR_CTL);
	dsb();
	udelay(2);

	/* Enable clocks via SW_CLK_EN */
	writel(APCS_L2_CORE_CBCR_CLK_ENABLE, base + APCS_L2_CORE_CBCR);

	/* De-assert L2/SCU logic clamp */
	val &= ~APCS_L2_PWR_CTL_L2_HS_CLAMP;
	writel(val, base + APCS_L2_PWR_CTL);
	dsb();
	udelay(2);

	/* De-assert PRESSETDBg */
	writel(0x0, base + APCS_PWR_CTL_OVERRIDE);

	/* De-assert L2/SCU Logic reset */
	val &= ~(APCS_L2_PWR_CTL_SYS_RESET | APCS_L2_PWR_CTL_L2_HS_RST);
	writel(val, base + APCS_L2_PWR_CTL);
	dsb();
	udelay(54);

	/* Turn on the PMIC_APC */
	val |= APCS_L2_PWR_CTL_PMIC_APC_ON;
	writel(val, base + APCS_L2_PWR_CTL);

	/* Set H/W clock control for the cpu CBC block */
	writel(APCS_L2_CORE_CBCR_HW_CTL | APCS_L2_CORE_CBCR_CLK_ENABLE,
	       base + APCS_L2_CORE_CBCR);
	dsb();
}

void qcom_power_up_arm_cortex(uint32_t mpidr, uint32_t base)
{
	uint32_t pwr_ctl;

	if (mpidr == read_mpidr()) {
		dprintf(INFO, "Skipping boot of current CPU (%d)\n", mpidr);
		return;
	}

	qcom_power_up_l2_cache(mpidr, APCS_GLB_BASE(base));

	pwr_ctl = CPU_PWR_CTL_CLAMP | CPU_PWR_CTL_CORE_MEM_CLAMP |
		  CPU_PWR_CTL_CORE_RST | CPU_PWR_CTL_COREPOR_RST;
	writel(pwr_ctl, base + CPU_PWR_CTL);
	dsb();

	writel(APC_PWR_GATE_CTL_GHDS_EN | APC_PWR_GATE_CTL_GHDS_CNT(16),
	       base + APC_PWR_GATE_CTL);
	dsb();
	udelay(2);

	pwr_ctl &= ~CPU_PWR_CTL_CORE_MEM_CLAMP;
	writel(pwr_ctl, base + CPU_PWR_CTL);
	dsb();

	pwr_ctl |= CPU_PWR_CTL_CORE_MEM_HS;
	writel(pwr_ctl, base + CPU_PWR_CTL);
	dsb();
	udelay(2);

	pwr_ctl &= ~CPU_PWR_CTL_CLAMP;
	writel(pwr_ctl, base + CPU_PWR_CTL);
	dsb();
	udelay(2);

	pwr_ctl &= ~(CPU_PWR_CTL_CORE_RST | CPU_PWR_CTL_COREPOR_RST);
	writel(pwr_ctl, base + CPU_PWR_CTL);
	dsb();

	pwr_ctl |= CPU_PWR_CTL_CORE_PWRD_UP;
	writel(pwr_ctl, base + CPU_PWR_CTL);
	dsb();

	/* Give CPU some time to boot */
	udelay(100);
}

/*
 * The MSM8974 CPU boot sequence is adapted from the Linux kernel:
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm/mach-qcom/platsmp.c
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *  Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *  Copyright (c) 2014 The Linux Foundation. All rights reserved.
 */
#define APC_PWR_GATE_CTL_LDO_PWR_DWN	(0x3f << 16)
#define APC_PWR_GATE_CTL_LDO_BYP	(0x3f << 8)
#define APC_PWR_GATE_CTL_BHS_SEG	(0x3f << 1)

#define APCS_SAW2_2_VCTL		0x1c

void qcom_power_up_kpssv2(uint32_t mpidr, uint32_t reg, uint32_t l2_saw_base)
{
	uint32_t reg_val;

	if (mpidr == read_mpidr()) {
		dprintf(INFO, "Skipping boot of current CPU (%d)\n", mpidr);
		return;
	}

	/* Turn on the BHS, turn off LDO Bypass and power down LDO */
	reg_val = APC_PWR_GATE_CTL_GHDS_EN | APC_PWR_GATE_CTL_GHDS_CNT(64) |
		  APC_PWR_GATE_CTL_LDO_PWR_DWN;
	writel(reg_val, reg + APC_PWR_GATE_CTL);
	dsb();
	/* wait for the BHS to settle */
	udelay(1);

	/* Turn on BHS segments */
	reg_val |= APC_PWR_GATE_CTL_BHS_SEG;
	writel(reg_val, reg + APC_PWR_GATE_CTL);
	dsb();
	/* wait for the BHS to settle */
	udelay(1);

	/* Finally turn on the bypass so that BHS supplies power */
	reg_val |= APC_PWR_GATE_CTL_LDO_BYP;
	writel(reg_val, reg + APC_PWR_GATE_CTL);

	/* enable max phases */
	writel(0x10003, l2_saw_base + APCS_SAW2_2_VCTL);
	dsb();
	udelay(50);

	reg_val = CPU_PWR_CTL_COREPOR_RST | CPU_PWR_CTL_CLAMP;
	writel(reg_val, reg + CPU_PWR_CTL);
	dsb();
	udelay(2);

	reg_val &= ~CPU_PWR_CTL_CLAMP;
	writel(reg_val, reg + CPU_PWR_CTL);
	dsb();
	udelay(2);

	reg_val &= ~CPU_PWR_CTL_COREPOR_RST;
	writel(reg_val, reg + CPU_PWR_CTL);
	dsb();

	reg_val |= CPU_PWR_CTL_CORE_PWRD_UP;
	writel(reg_val, reg + CPU_PWR_CTL);
	dsb();

	/* Give CPU some time to boot */
	udelay(100);
}
