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

int qcom_set_boot_addr(uint32_t addr)
{
	scmcall_arg arg = {
		MAKE_SIP_SCM_CMD(SCM_SVC_BOOT, QCOM_SCM_BOOT_SET_ADDR_MC),
		MAKE_SCM_ARGS(6),
		addr,
		~0UL, ~0UL, ~0UL, ~0UL, /* All CPUs */
		QCOM_SCM_BOOT_MC_FLAG_AARCH64 | QCOM_SCM_BOOT_MC_FLAG_COLDBOOT,
	};
	return scm_call2(&arg, NULL);
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
