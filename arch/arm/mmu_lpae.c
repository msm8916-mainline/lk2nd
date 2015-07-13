/* Copyright (c) 2015, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <debug.h>
#include <sys/types.h>
#include <compiler.h>
#include <arch.h>
#include <arch/arm.h>
#include <arch/defines.h>
#include <arch/arm/mmu.h>
#include <mmu.h>
#include <platform.h>

#if ARM_WITH_MMU

#define LPAE_SIZE               (1ULL << 40)
#define LPAE_MASK               (LPAE_SIZE - 1)
#define L1_PT_INDEX             0x7FC0000000
#define PT_TABLE_DESC_BIT       0x2
#define SIZE_1GB                (0x400000000UL)
#define SIZE_2MB                (0x200000)
#define MMU_L2_PT_SIZE          512
#define MMU_PT_BLOCK_DESCRIPTOR 0x1
#define MMU_PT_TABLE_DESCRIPTOR 0x3
#define MMU_AP_FLAG             (0x1 << 10)
#define L2_PT_MASK              0xFFFFE00000
#define L2_INDEX_MASK           0x3FE00000

uint64_t mmu_l1_pagetable[L1_PT_SZ] __attribute__ ((aligned(4096))); /* Max is 8 */
uint64_t mmu_l2_pagetable[L2_PT_SZ*MMU_L2_PT_SIZE] __attribute__ ((aligned(4096))); /* Macro from target code * 512 */
uint64_t avail_l2_pt = L2_PT_SZ;
uint64_t *empty_l2_pt = mmu_l2_pagetable;

/************************************************************/
/* MAP 2MB granules in 1GB section in L2 page table */
/***********************************************************/

static void mmu_map_l2_entry(mmu_section_t *block)
{
	uint64_t *l2_pt = NULL;
	uint64_t  address_start;
	uint64_t  address_end;
	uint64_t  p_addr;

	/* First initialize the first level descriptor for each 1 GB
	 * Bits[47:12] provide the physical base address of the level 2 page table
	 *
	 *    ________________________________________________________________________________
	 *   |  |     |  |   |       |        |                            |       |          |
	 *   |63|62-61|60| 59|58---52|51----40|39------------------------12|11----2|1------- 0|
	 *   |NS| AP  |XN|PXN|Ignored|UNK|SBZP|Next-level table addr[39:12]|Ignored|Descriptor|
	 *   |__|_____|__|___|_______|________|____________________________|_______|__________|
	 * NS: Used only in secure state
	 * AP: Access protection
	 */

	/* Convert the virtual address[38:30] into an index of the L1 page table */
	address_start = (block->vaddress & LPAE_MASK) >> 30;

	 /* Check if this 1GB entry has L2 page table mapped already
	 * if L1 entry hasn't mapped any L2 page table, allocate a L2 page table for it
	 */

	if((mmu_l1_pagetable[address_start] & PT_TABLE_DESC_BIT) == 0)
	{
		ASSERT(avail_l2_pt);

		/* Get the first l2 empty page table and fill in the L1 PTE with a table descriptor,
		 * The l2_pt address bits 12:39 are used for L1 PTE entry
		 */
		l2_pt = empty_l2_pt;

		/* Bits 39.12 of the page table address are mapped into the L1 PTE entry */
		mmu_l1_pagetable[address_start] = ((uint64_t)(uintptr_t)l2_pt & 0x0FFFFFFF000) | MMU_PT_TABLE_DESCRIPTOR;

		/* Advance pointer to next empty l2 page table */
		empty_l2_pt += MMU_L2_PT_SIZE;
		avail_l2_pt--;
	}
	else
	{
		/* Entry has L2 page table mapped already, so just get the existing L2 page table address */
		l2_pt = (uint64_t *) (uintptr_t)(mmu_l1_pagetable[address_start] & 0xFFFFFFF000);
    }

	/* Get the physical address of 2MB sections, bits 21:39 are used to populate the L2 entry */
	p_addr = block->paddress & L2_PT_MASK;

	/* Start index into the L2 page table for this section using the virtual address[29:21]*/
	address_start  = (block->vaddress & L2_INDEX_MASK) >> 21;

	/* The end index for the given section. size given is in MB convert it to number of 2MB segments */
	address_end = address_start + ((block->size) >> 1);

	/*
	 *      ___________________________________________________________________________________________________________________
	 *     |       |        |  |   |    |        |                  |        |  |  |       |       |  |             |          |
	 *     |63---59|58----55|54|53 |52  |51----40|39--------------21|20----12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
	 *     |Ignored|Reserved|XN|PXN|Cont|UNK|SBZP|Output addr[39:21]|UNK|SBZP|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
	 *     |_______|________|__|___|____|________|__________________|________|__|__|_______|_______|__|_____________|__________|
	 */

	/* Map all the 2MB segments in the 1GB section */
	while (address_start < address_end)
	{
		l2_pt[address_start] =  (p_addr) | MMU_PT_BLOCK_DESCRIPTOR | MMU_AP_FLAG | block->flags;
		address_start++;
		/* Increment to the next 2MB segment in current L2 page table*/
		p_addr += SIZE_2MB;
		arm_invalidate_tlb();
	}
}

/************************************************************/
/* MAP 1GB granules in L1 page table */
/***********************************************************/
static void mmu_map_l1_entry(mmu_section_t *block)
{
	uint64_t address_start;
	uint64_t address_end;
	uint64_t p_addr;

	/* Convert our base address into an index into the page table */
	address_start = (block->vaddress & LPAE_MASK) >> 30;

	/* Get the end address into the page table, size is in MB, convert to GB for L1 mapping */
	address_end = address_start + ((block->size) >> 10);

	/* bits 38:30 provide the physical base address of the section */
	p_addr = block->paddress & L1_PT_INDEX;

	while(address_start < address_end)
	{
	 /*
	 *    A Block descriptor for first stage, level one is as follows (Descriptor = 0b01):
	 *         ___________________________________________________________________________________________________________________
	 *        |       |        |  |	  |    |        |                  |        |  |  |       |       |  |             |          |
	 *        |63---59|58----55|54|53 |52  |51----40|39--------------30|n-1 --12|11|10|9     8|7     6|5 |4-----------2|  1   0   |
	 *        |Ignored|Reserved|XN|PXN|Cont|UNK/SBZP|Output addr[47:30]|UNK/SBZP|nG|AF|SH[1:0]|AP[2:1]|NS|AttrIndx[2:0]|Descriptor|
	 *        |_______|________|__|___|____|________|__________________|________|__|__|_______|_______|__|_____________|__________|
	 */

		mmu_l1_pagetable[address_start] =  (p_addr) | block->flags | MMU_AP_FLAG | MMU_PT_BLOCK_DESCRIPTOR;

		p_addr += SIZE_1GB; /* Point to next level */
		address_start++;
		arm_invalidate_tlb();
	}
}

void arm_mmu_map_entry(mmu_section_t *entry)
{
	ASSERT(entry);

	if (entry->type == MMU_L1_NS_SECTION_MAPPING)
		mmu_map_l1_entry(entry);
	else if(entry->type == MMU_L2_NS_SECTION_MAPPING)
		mmu_map_l2_entry(entry);
	else
		dprintf(CRITICAL, "Invalid mapping type in the mmu table: %d\n", entry->type);
}

void arm_mmu_init(void)
{
	/* set some mmu specific control bits:
	 * access flag disabled, TEX remap disabled, mmu disabled
	 */
	arm_write_cr1(arm_read_cr1() & ~((1<<29)|(1<<28)|(1<<0)));

	platform_init_mmu_mappings();

	/* set up the translation table base */
	arm_write_ttbr((uint32_t)mmu_l1_pagetable);

	/* set up the Memory Attribute Indirection Registers 0 and 1 */
	arm_write_mair0(MAIR0);
	arm_write_mair1(MAIR1);

	/* TTBCR.EAE = 1 */
	arm_write_ttbcr(0x80000000);

	/* Enable TRE */
	arm_write_cr1(arm_read_cr1() | (1<<28));

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

uint64_t virtual_to_physical_mapping(uint32_t vaddr)
{
	uint32_t l1_index;
	uint64_t *l2_pt = NULL;
	uint32_t l2_index;
	uint32_t offset = 0;
	uint64_t paddr = 0;

	/* Find the L1 index from virtual address */
	l1_index = (vaddr & LPAE_MASK) >> 30;

	if ((mmu_l1_pagetable[l1_index] & MMU_PT_TABLE_DESCRIPTOR) == MMU_PT_TABLE_DESCRIPTOR)
	{
		/* Get the l2 page table address */
		l2_pt = (uint64_t *) (uintptr_t) (mmu_l1_pagetable[l1_index] & 0x0FFFFFFF000);
		/* Get the l2 index from virtual address */
		l2_index = (vaddr & L2_INDEX_MASK) >> 21;
		/* Calculate the offset from vaddr. */
		offset = vaddr & 0x1FFFFF;
		/* Get the physical address bits from 21 to 39 */
		paddr = (l2_pt[l2_index] & L2_PT_MASK) + offset;
	} else if ((mmu_l1_pagetable[l1_index] & MMU_PT_TABLE_DESCRIPTOR) == MMU_PT_BLOCK_DESCRIPTOR)
	{
		/* Calculate the offset from bits 0 to 30 */
		offset = vaddr & 0x3FFFFFFF;
		/* Return the entry from l1 page table */
		paddr = (mmu_l1_pagetable[l1_index] & L1_PT_INDEX) + offset;
	} else
	{
		ASSERT(0);
	}

	return paddr;
}

uint32_t physical_to_virtual_mapping(uint64_t paddr)
{
	uint32_t i, j;
	uint32_t vaddr_index = 0;
	uint32_t vaddr = 0;
	uint64_t paddr_base_l1;
	uint64_t paddr_base_l2;
	uint64_t *l2_pt = NULL;
	bool l1_index_found = false;
	uint32_t l1_index = 0;
	uint32_t offset = 0;

	/* Traverse through the L1 page table to look for block descriptor */
	for (i = 0; i < L1_PT_SZ; i++)
	{
		/* Find the L1 page table index */
		paddr_base_l1 = paddr & L1_PT_INDEX;

		/* If the L1 index is unused continue */
		if ((mmu_l1_pagetable[i] & MMU_PT_TABLE_DESCRIPTOR) == 0)
			continue;

		/* If Its a block entry, find the matching entry and return the index */
		if ((mmu_l1_pagetable[i] & MMU_PT_TABLE_DESCRIPTOR) == MMU_PT_BLOCK_DESCRIPTOR)
		{
			if ((mmu_l1_pagetable[i] & L1_PT_INDEX) == paddr_base_l1)
			{
				offset = paddr - paddr_base_l1;
				vaddr_index = i;
				l1_index_found = true;
				goto end;
			} /* If the entry is table, extract table address */
		} else if ((mmu_l1_pagetable[i] & MMU_PT_TABLE_DESCRIPTOR) == MMU_PT_TABLE_DESCRIPTOR)
		{
			l1_index = i;
			l2_pt = (uint64_t *) (uintptr_t) (mmu_l1_pagetable[l1_index] & 0x0FFFFFFF000);
			paddr_base_l2 = paddr & L2_PT_MASK;
			/* Search the table to find index in L2 page table */
			for (j = 0; j < MMU_L2_PT_SIZE; j++)
			{
				if (paddr_base_l2 == (l2_pt[j] & L2_PT_MASK))
				{
					vaddr_index = j;
					offset = paddr - paddr_base_l2;
					goto end;
				}
			}
		}
	}
	/* If we reach here, that means the addrss is either no mapped or invalid request */
	dprintf(CRITICAL, "The address %llx is not mapped\n", paddr);
	ASSERT(0);

end:
	/* Convert the index into the virtual address */
	if (l1_index_found)
		vaddr = (vaddr_index << 30);
	else
		vaddr = ((vaddr_index << 21) & L2_INDEX_MASK) + (l1_index << 30);

	vaddr += offset;

	return vaddr;
}
#endif // ARM_WITH_MMU
