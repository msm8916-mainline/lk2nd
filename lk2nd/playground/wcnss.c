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

#define readl_relaxed readl
#define writel_relaxed writel

#define readl_tight_poll_timeout(addr, val, cond, timeout_us) \
({ \
	unsigned i; \
	for (i = 0; i < (timeout_us); ++i) { \
		(val) = readl(addr); \
		if (cond) \
			break; \
		udelay(1); \
	} \
	(cond) ? 0 : 1; \
})

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

	/* Configure boot address */
	writel_relaxed(start_addr >> 16, base +
			PRONTO_PMU_CCPU_BOOT_REMAP_ADDR);

	/* Use the high vector table */
	reg = readl_relaxed(base + PRONTO_PMU_CCPU_CTL);
	reg |= PRONTO_PMU_CCPU_CTL_REMAP_EN | PRONTO_PMU_CCPU_CTL_HIGH_IVT;
	writel_relaxed(reg, base + PRONTO_PMU_CCPU_CTL);

	/* Turn on AHB clock of common_ss */
	reg = readl_relaxed(base + PRONTO_PMU_COMMON_AHB_CBCR);
	reg |= PRONTO_PMU_COMMON_AHB_CBCR_CLK_EN;
	writel_relaxed(reg, base + PRONTO_PMU_COMMON_AHB_CBCR);

	/* Turn on CPU clock of common_ss */
	reg = readl_relaxed(base + PRONTO_PMU_COMMON_CPU_CBCR);
	reg |= PRONTO_PMU_COMMON_CPU_CBCR_CLK_EN;
	writel_relaxed(reg, base + PRONTO_PMU_COMMON_CPU_CBCR);

	/* Enable A2XB bridge */
	reg = readl_relaxed(base + PRONTO_PMU_COMMON_CSR);
	reg |= PRONTO_PMU_COMMON_CSR_A2XB_CFG_EN;
	writel_relaxed(reg, base + PRONTO_PMU_COMMON_CSR);

	/* Enable common_ss power */
	reg = readl_relaxed(base + PRONTO_PMU_COMMON_GDSCR);
	reg &= ~PRONTO_PMU_COMMON_GDSCR_SW_COLLAPSE;
	writel_relaxed(reg, base + PRONTO_PMU_COMMON_GDSCR);

	/* Wait for AHB clock to be on */
	rc = readl_tight_poll_timeout(base + PRONTO_PMU_COMMON_AHB_CBCR,
				      reg,
				      !(reg & PRONTO_PMU_COMMON_AHB_CLK_OFF),
				      CLK_UPDATE_TIMEOUT_US);
	if (rc) {
		dprintf(CRITICAL, "pronto common ahb clk enable timeout\n");
		return rc;
	}

	/* Wait for CPU clock to be on */
	rc = readl_tight_poll_timeout(base + PRONTO_PMU_COMMON_CPU_CBCR,
				      reg,
				      !(reg & PRONTO_PMU_COMMON_CPU_CLK_OFF),
				      CLK_UPDATE_TIMEOUT_US);
	if (rc) {
		dprintf(CRITICAL, "pronto common cpu clk enable timeout\n");
		return rc;
	}

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
