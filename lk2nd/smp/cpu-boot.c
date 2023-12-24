// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2021-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <bits.h>
#include <debug.h>
#include <platform/timer.h>
#include <scm.h>

#include <libfdt.h>
#include <lk2nd/util/lkfdt.h>

#include "cpu-boot.h"

#define QCOM_SCM_BOOT_SET_ADDR		0x01
#define QCOM_SCM_BOOT_FLAG_COLD_ALL	(0 | BIT(0) | BIT(3) | BIT(5))
#define QCOM_SCM_BOOT_SET_ADDR_MC	0x11
#define QCOM_SCM_BOOT_MC_FLAG_AARCH64	BIT(0)
#define QCOM_SCM_BOOT_MC_FLAG_COLDBOOT	BIT(1)
#define QCOM_SCM_BOOT_MC_FLAG_WARMBOOT	BIT(2)

int cpu_boot_set_addr(uintptr_t addr, bool arm64)
{
	uint32_t aarch64 = arm64 ? QCOM_SCM_BOOT_MC_FLAG_AARCH64 : 0;
	scmcall_arg arg = {
		.x0 = MAKE_SIP_SCM_CMD(SCM_SVC_BOOT, QCOM_SCM_BOOT_SET_ADDR_MC),
		.x1 = MAKE_SCM_ARGS(6),
		.x2 = addr,
		.x3 = ~0UL, .x4 = ~0UL, .x5 = {~0UL, ~0UL, /* All CPUs */
		aarch64 | QCOM_SCM_BOOT_MC_FLAG_COLDBOOT},
	};

	if (is_scm_armv8_support())
		return scm_call2(&arg, NULL);

	dprintf(INFO, "Falling back to legacy QCOM_SCM_BOOT_SET_ADDR call\n");
	return scm_call_atomic2(SCM_SVC_BOOT, QCOM_SCM_BOOT_SET_ADDR,
				QCOM_SCM_BOOT_FLAG_COLD_ALL, addr);
}

static inline uint32_t read_mpidr(void)
{
	uint32_t res;
	__asm__ ("mrc p15, 0, %0, c0, c0, 5" : "=r" (res));
	return BITS(res, 23, 0);
}

static uint32_t read_phandle_reg(const void *dtb, int node, const char *prop)
{
	const fdt32_t *val;
	int target, len;

	target = lkfdt_lookup_phandle(dtb, node, prop);
	if (target < 0) {
		dprintf(CRITICAL, "Cannot find %s node in %s: %d\n",
			prop, fdt_get_name(dtb, node, NULL), target);
		return 0;
	}

	val = fdt_getprop(dtb, target, "reg", &len);
	if (len < (int)sizeof(*val)) {
		dprintf(CRITICAL, "Cannot read reg property of %s node: %d\n",
			prop, len);
		return 0;
	}
	return fdt32_to_cpu(*val);
}

bool cpu_boot(const void *dtb, int node, uint32_t mpidr)
{
	uint32_t extra_reg __UNUSED;

	if (mpidr == read_mpidr()) {
		dprintf(INFO, "Skipping boot of current CPU (%x)\n", mpidr);
		return true;
	}
#ifndef CPU_BOOT_CORTEX_A_MSM8994
	uint32_t acc;
	/* Boot the CPU core using registers in the ACC node */
	acc = read_phandle_reg(dtb, node, "qcom,acc");
	if (!acc)
		return false;

	dprintf(INFO, "Booting CPU%x @ %#08x\n", mpidr, acc);
#else
	dprintf(INFO, "Booting CPU%x\n", mpidr);
#endif

#if CPU_BOOT_CORTEX_A
	/*
	 * The CPU clock happens to point to the "APCS" node that also controls
	 * the power signals for the L2 cache. The address does not have to be
	 * present since on SoCs with a single CPU cluster the L2 cache should
	 * already be powered on and active.
	 */
	extra_reg = read_phandle_reg(dtb, node, "clocks");
	cpu_boot_cortex_a(acc, extra_reg);
#elif CPU_BOOT_CORTEX_A_MSM8994
	cpu_boot_cortex_a_msm8994(mpidr);
#elif CPU_BOOT_KPSSV1
	extra_reg = read_phandle_reg(dtb, node, "qcom,saw");
	if (!extra_reg)
		return false;
	cpu_boot_kpssv1(acc, extra_reg);
#elif CPU_BOOT_KPSSV2
	node = lkfdt_lookup_phandle(dtb, node, "next-level-cache");
	if (node < 0) {
		dprintf(CRITICAL, "Cannot find CPU next-level-cache: %d\n", node);
		return false;
	}
	extra_reg = read_phandle_reg(dtb, node, "qcom,saw");
	if (!extra_reg)
		return false;
	cpu_boot_kpssv2(acc, extra_reg);
#else
#error Unsupported CPU boot method!
#endif

	/* Give CPU some time to boot */
	udelay(100);
	return true;
}
