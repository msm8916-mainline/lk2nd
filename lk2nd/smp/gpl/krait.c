// SPDX-License-Identifier: GPL-2.0-only
/*
 * Based on code from the Linux kernel:
 * https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git/tree/arch/arm/mach-qcom/platsmp.c
 *
 *  Copyright (C) 2002 ARM Ltd.
 *  All Rights Reserved
 *  Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *  Copyright (c) 2014 The Linux Foundation. All rights reserved.
 */

#include <arch/defines.h>
#include <bits.h>
#include <kernel/thread.h>
#include <platform/timer.h>
#include <reg.h>

#include "../cpu-boot.h"

#define CPU_PWR_CTL			0x4
#define APC_PWR_GATE_CTL		0x14	/* kpssv2 only */

#define CPU_PWR_CTL_CLAMP		BIT(0)
#define CPU_PWR_CTL_L2DT_SLP		BIT(3)
#define CPU_PWR_CTL_CORE_RST		BIT(4)
#define CPU_PWR_CTL_COREPOR_RST		BIT(5)
#define CPU_PWR_CTL_GATE_CLK		BIT(6)
#define CPU_PWR_CTL_CORE_PWRD_UP	BIT(7)
#define CPU_PWR_CTL_PLL_CLAMP		BIT(8)	/* kpssv1 only */

#define APC_PWR_GATE_CTL_BHS_EN		BIT(0)
#define APC_PWR_GATE_CTL_BHS_SEG	(0x3f << 1)
#define APC_PWR_GATE_CTL_LDO_BYP	(0x3f << 8)
#define APC_PWR_GATE_CTL_LDO_PWR_DWN	(0x3f << 16)
#define APC_PWR_GATE_CTL_BHS_CNT(cnt)	((cnt) << 24)

#define APCS_SAW2_VCTL			0x14	/* kpssv1 */
#define APCS_SAW2_2_VCTL		0x1c	/* kpssv2 */

void cpu_boot_kpssv1(uint32_t reg, uint32_t saw_reg)
{
	uint32_t val;

	enter_critical_section();

	/* Turn on CPU rail */
	writel(0xA4, saw_reg + APCS_SAW2_VCTL);
	dsb();
	udelay(512);

	/* Krait bring-up sequence */
	val = CPU_PWR_CTL_PLL_CLAMP | CPU_PWR_CTL_L2DT_SLP | CPU_PWR_CTL_CLAMP;
	writel(val, reg + CPU_PWR_CTL);
	val &= ~CPU_PWR_CTL_L2DT_SLP;
	writel(val, reg + CPU_PWR_CTL);
	dsb();
	udelay(1); /* ndelay(300); */

	val |= CPU_PWR_CTL_COREPOR_RST;
	writel(val, reg + CPU_PWR_CTL);
	dsb();
	udelay(2);

	val &= ~CPU_PWR_CTL_CLAMP;
	writel(val, reg + CPU_PWR_CTL);
	dsb();
	udelay(2);

	val &= ~CPU_PWR_CTL_COREPOR_RST;
	writel(val, reg + CPU_PWR_CTL);
	dsb();
	udelay(100);

	val |= CPU_PWR_CTL_CORE_PWRD_UP;
	writel(val, reg + CPU_PWR_CTL);
	dsb();

	exit_critical_section();
}

void cpu_boot_kpssv2(uint32_t reg, uint32_t l2_saw_base)
{
	uint32_t reg_val;

	enter_critical_section();

	/* Turn on the BHS, turn off LDO Bypass and power down LDO */
	reg_val = APC_PWR_GATE_CTL_BHS_EN | APC_PWR_GATE_CTL_LDO_PWR_DWN |
		  APC_PWR_GATE_CTL_BHS_CNT(64);
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

	exit_critical_section();
}
