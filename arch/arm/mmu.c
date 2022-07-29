/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <debug.h>
#include <sys/types.h>
#include <compiler.h>
#include <arch.h>
#include <arch/arm.h>
#include <arch/defines.h>
#include <arch/ops.h>
#include <arch/arm/mmu.h>
#include <platform.h>

#if ARM_WITH_MMU

#define MB (1024*1024)

/* the location of the table may be brought in from outside */
#if WITH_EXTERNAL_TRANSLATION_TABLE
#if !defined(MMU_TRANSLATION_TABLE_ADDR)
#error must set MMU_TRANSLATION_TABLE_ADDR in the make configuration
#endif
static uint32_t *tt = (void *)MMU_TRANSLATION_TABLE_ADDR;
#else
/* the main translation table */
static uint32_t tt[4096] __ALIGNED(16384);
#endif

static inline uint32_t arm_mmu_section_desc(addr_t paddr, uint flags)
{
	/*
	 * (2<<0): Section entry
	 * (0<<5): Domain = 0
	 *  flags: TEX, CB and AP bit settings provided by the caller.
	 */
	return (paddr & ~(MB-1)) | (0<<5) | (2<<0) | flags;
}

void arm_mmu_map_section(addr_t paddr, addr_t vaddr, uint flags)
{
	int index;

	/* Get the index into the translation table */
	index = vaddr / MB;

	/* Set the entry value */
	tt[index] = arm_mmu_section_desc(paddr, flags);

	arm_invalidate_tlb();
}

void arm_mmu_init(void)
{
	int i;

	/* set some mmu specific control bits:
	 * access flag disabled, TEX remap disabled, mmu disabled
	 */
	arm_write_cr1(arm_read_cr1() & ~((1<<29)|(1<<28)|(1<<0)));

	if (platform_use_identity_mmu_mappings())
	{
		/* set up an identity-mapped translation table with
		 * strongly ordered memory type and read/write access.
		 */
		for (i=0; i < 4096; i++) {
			arm_mmu_map_section(i * MB,
								i * MB,
								MMU_MEMORY_TYPE_STRONGLY_ORDERED | MMU_MEMORY_AP_READ_WRITE);
		}
	}

	platform_init_mmu_mappings();

	/* ensure short-descriptor format is selected (clear TTBCR.EAE) */
	arm_write_ttbcr(0);

	/* set up the translation table base */
	arm_write_ttbr((uint32_t)tt);

	/* set up the domain access register */
	arm_write_dacr(0x00000001);

	/* turn on the mmu */
	arm_write_cr1(arm_read_cr1() | 0x1);
}

void arch_disable_mmu(void)
{
	/* Ensure all memory access are complete
	 * before disabling MMU
	 */
	dsb();
	arm_write_cr1(arm_read_cr1() & ~(1<<0));
	arm_invalidate_tlb();
}

bool arm_mmu_try_map_sections(addr_t paddr, addr_t vaddr, uint size, uint flags)
{
	/* Round up to next MB and handle offsets within sections */
	uint mb = (size + (paddr % MB) + MB - 1) / MB;
	uint i, index = vaddr / MB;
	bool fully_mapped = true;

	/* Offset within mapped section must be equal */
	if (size == 0 || (paddr % MB) != (vaddr % MB))
		return false;

	/* Check if any existing mappings conflict */
	for (i = 0; i < mb; ++i) {
		uint32_t desc = arm_mmu_section_desc(paddr + i * MB, flags);
		if (!tt[index + i]) {
			fully_mapped = false;
			continue;
		}
		if (tt[index + i] != desc) {
			dprintf(CRITICAL, "MMU mapping mismatch @ %#08x: %#08x != %#08x\n",
				(index + i) * MB, tt[index + i], desc);
			return false;
		}
	}
	if (fully_mapped)
		return true;

	/* Add the new mappings */
	for (i = 0; i < mb; ++i)
		tt[index + i] = arm_mmu_section_desc(paddr + i * MB, flags);
	arm_mmu_flush();
	return true;
}

void arm_mmu_flush(void)
{
	arch_clean_cache_range((vaddr_t)&tt, sizeof(tt));
	dsb();
	arm_invalidate_tlb();
	dsb();
	isb();
}

#endif // ARM_WITH_MMU
