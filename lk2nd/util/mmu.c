// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <arch/arm/mmu.h>

#include <lk2nd/util/mmu.h>

/* Defined in aboot.c */
extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);
extern int check_ddr_addr_range_bound(uintptr_t start, uint32_t size);

static const char *try_map(uintptr_t start, uint32_t size, uint flags1, uint flags2)
{
	if (check_aboot_addr_range_overlap(start, size))
		return "overlaps with aboot";
	if (check_ddr_addr_range_bound(start, size))
		return "outside of RAM?";
	if (!arm_mmu_try_map_sections(start, start, size, flags1) &&
	    (!flags2 || !arm_mmu_try_map_sections(start, start, size, flags2)))
		return "mismatch with existing mappings";
	return NULL;
}

bool lk2nd_mmu_map_ram(const char *name, uintptr_t start, uint32_t size,
		       uint flags1, uint flags2)
{
	const char *fail = try_map(start, size, flags1, flags2);
	if (fail) {
		dprintf(CRITICAL, "Failed to map %s memory @ %#08lx (size: %#x): %s\n",
			name, start, size, fail);
		return false;
	}
	return true;
}

bool lk2nd_mmu_map_ram_wt(const char *name, uintptr_t start, uint32_t size)
{
	return lk2nd_mmu_map_ram(name, start, size,
				 MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH |
				 MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN, 0);
}

bool lk2nd_mmu_map_ram_dynamic(const char *name, uintptr_t start, uint32_t size)
{
	/*
	 * FIXME: At the moment the platforms use quite inconsistent memory
	 * types. Some map e.g. the kernel or framebuffer memory as
	 * "write-through", while others map it as "write-back-allocate".
	 * For now just try with both (calling code should handle both by
	 * flushing the cache if necessary). It might be better to make the
	 * platforms consistent instead but that needs careful testing.
	 */
	return lk2nd_mmu_map_ram(name, start, size,
				 MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH |
				 MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN,
				 MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE |
				 MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN);
}
