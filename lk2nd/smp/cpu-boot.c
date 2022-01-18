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

void qcom_power_up_arm_cortex(uint32_t mpidr, uint32_t base)
{
	uint32_t pwr_ctl;

	if (mpidr == read_mpidr()) {
		dprintf(INFO, "Skipping boot of current CPU (%d)\n", mpidr);
		return;
	}

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
