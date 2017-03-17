/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include <qgic.h>
#include <qtimer.h>
#include <mmu.h>
#include <arch/arm/mmu.h>
#include <smem.h>

#define MB (1024 *1024)

#define MSM_IOMAP_SIZE                      ((MSM_IOMAP_END - MSM_IOMAP_BASE)/MB)

#define A7_SS_SIZE                          ((A7_SS_END - A7_SS_BASE)/MB)

/* LK memory */
#define LK_MEMORY                             (MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
                                                                 MMU_MEMORY_AP_READ_WRITE)
/* Scratch memory - Strongly ordered, non-executable */
#define SCRATCH_MEMORY                        (MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
                                                                 MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)
/* Peripherals - shared device */
#define IOMAP_MEMORY                          (MMU_MEMORY_TYPE_DEVICE_SHARED | \
                                                                 MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)
#define SCRATCH_REGION1_VIRT_START_128            0x88000000
#define SCRATCH_REGION2_VIRT_START_128            (SCRATCH_REGION1_VIRT_START_128 + SCRATCH_REGION1_SIZE_128)

static void ddr_based_mmu_mappings(mmu_section_t *table, uint32_t table_size);
static uint64_t ddr_size;
static void board_ddr_detect();

/* Map all the accesssible memory according to the following rules:
 * 1. Map 1MB from MSM_SHARED_BASE with 1 -1 mapping.
 * 2. Map MEMBASE - MEMSIZE with 1 -1 mapping.
 * 3. Map all the scratch regions immediately after Appsbl memory.
 *     Virtual addresses start right after Appsbl Virtual address.
 * 4. Map all the IOMAP space with 1 - 1 mapping.
 * 5. Map all the rest of the SDRAM/ IMEM regions as 1 -1.
 */
mmu_section_t mmu_section_table[] = {
/*   Physical addr,               Virtual addr,                   Size (in MB),                   Flags   */
	{MSM_SHARED_BASE,         MSM_SHARED_BASE,                1,                              SCRATCH_MEMORY},
	{MEMBASE,                 MEMBASE,                        MEMSIZE / MB,                   LK_MEMORY},
	{MSM_IOMAP_BASE,          MSM_IOMAP_BASE,                 MSM_IOMAP_SIZE,                 IOMAP_MEMORY},
	{A7_SS_BASE,              A7_SS_BASE,                     A7_SS_SIZE,                     IOMAP_MEMORY},
	{MSM_SHARED_IMEM_BASE,    MSM_SHARED_IMEM_BASE,           1,                              IOMAP_MEMORY},
};

mmu_section_t mmu_section_table_128[] = {
	{SCRATCH_REGION1_128,         SCRATCH_REGION1_VIRT_START_128,     SCRATCH_REGION1_SIZE_128/ MB,      SCRATCH_MEMORY},
	{SCRATCH_REGION2_128,         SCRATCH_REGION2_VIRT_START_128,     SCRATCH_REGION2_SIZE_128/ MB,      SCRATCH_MEMORY},
};

mmu_section_t mmu_section_table_256[] = {
	{SCRATCH_REGION_256,         SCRATCH_REGION_256,          SCRATCH_REGION_SIZE_256/ MB,      SCRATCH_MEMORY},
	{KERNEL_REGION,              KERNEL_REGION,               KERNEL_REGION_SIZE/ MB,           SCRATCH_MEMORY},
};

static void board_ddr_detect()
{
	ddr_size = smem_get_ddr_size();
	/*128MB DDR*/
	if(ddr_size == 0x8000000)
		ddr_based_mmu_mappings(mmu_section_table_128, ARRAY_SIZE(mmu_section_table_128));
	else
		ddr_based_mmu_mappings(mmu_section_table_256, ARRAY_SIZE(mmu_section_table_256));
}

void platform_early_init(void)
{
	board_init();
	platform_clock_init();
	qgic_init();
	qtimer_init();
	scm_init();
	board_ddr_detect();
}

void platform_init(void)
{
	dprintf(INFO, "platform_init()\n");
}

void platform_uninit(void)
{
	qtimer_uninit();
	qpic_nand_uninit();
}

uint32_t platform_get_sclk_count(void)
{
	return readl(MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL);
}

addr_t get_bs_info_addr()
{
	return ((addr_t)BS_INFO_ADDR);
}

addr_t platform_get_virt_to_phys_mapping(addr_t virt_addr)
{
	uint32_t paddr;
	uint32_t table_size = ARRAY_SIZE(mmu_section_table);
	uint32_t limit;

	for (uint32_t i = 0; i < table_size; i++)
	{
		limit = (mmu_section_table[i].num_of_sections * MB) - 0x1;

		if (virt_addr >= mmu_section_table[i].vaddress &&
			virt_addr <= (mmu_section_table[i].vaddress + limit))
		{
				paddr = mmu_section_table[i].paddress + (virt_addr - mmu_section_table[i].vaddress);
				return paddr;
		}
	}
	if(ddr_size == 0x8000000)
	{
		table_size = ARRAY_SIZE(mmu_section_table_128);
		for (uint32_t i = 0; i < table_size; i++)
		{
			limit = (mmu_section_table_128[i].num_of_sections * MB) - 0x1;

			if (virt_addr >= mmu_section_table_128[i].vaddress &&
				virt_addr <= (mmu_section_table_128[i].vaddress + limit))
			{
				paddr = mmu_section_table_128[i].paddress + (virt_addr - mmu_section_table_128[i].vaddress);
				return paddr;
			}
		}
	}
	else
	{
		/* Any DDR > 256MB would be mapped here & LK would use only first 256 MB */
		table_size = ARRAY_SIZE(mmu_section_table_256);
		for (uint32_t i = 0; i < table_size; i++)
		{
			limit = (mmu_section_table_256[i].num_of_sections * MB) - 0x1;

			if (virt_addr >= mmu_section_table_256[i].vaddress &&
				virt_addr <= (mmu_section_table_256[i].vaddress + limit))
			{
				paddr = mmu_section_table_256[i].paddress + (virt_addr - mmu_section_table_256[i].vaddress);
				return paddr;
			}
		}
	}
	/* No special mapping found.
	 * Assume 1-1 mapping.
	 */
	 paddr = virt_addr;
	return paddr;

}

addr_t platform_get_phys_to_virt_mapping(addr_t phys_addr)
{
	uint32_t vaddr;
	uint32_t table_size = ARRAY_SIZE(mmu_section_table);
	uint32_t limit;

	for (uint32_t i = 0; i < table_size; i++)
	{
		limit = (mmu_section_table[i].num_of_sections * MB) - 0x1;

		if (phys_addr >= mmu_section_table[i].paddress &&
			phys_addr <= (mmu_section_table[i].paddress + limit))
		{
				vaddr = mmu_section_table[i].vaddress + (phys_addr - mmu_section_table[i].paddress);
				return vaddr;
		}
	}
	if(ddr_size == 0x8000000)
	{
		table_size = ARRAY_SIZE(mmu_section_table_128);
		for (uint32_t i = 0; i < table_size; i++)
		{
			limit = (mmu_section_table_128[i].num_of_sections * MB) - 0x1;

			if (phys_addr >= mmu_section_table_128[i].paddress &&
				phys_addr <= (mmu_section_table_128[i].paddress + limit))
			{
				vaddr = mmu_section_table_128[i].vaddress + (phys_addr - mmu_section_table_128[i].paddress);
				return vaddr;
			}
		}
	}
	else
	{
		/* Any DDR > 256MB would be mapped here & LK would use only first 256 MB */
		table_size = ARRAY_SIZE(mmu_section_table_256);
		for (uint32_t i = 0; i < table_size; i++)
		{
			limit = (mmu_section_table_256[i].num_of_sections * MB) - 0x1;

			if (phys_addr >= mmu_section_table_256[i].paddress &&
				phys_addr <= (mmu_section_table_256[i].paddress + limit))
			{
				vaddr = mmu_section_table_256[i].vaddress + (phys_addr - mmu_section_table_256[i].paddress);
				return vaddr;
			}
		}
	}

	/* No special mapping found.
	 * Assume 1-1 mapping.
	 */
	 vaddr = phys_addr;

	return vaddr;
}

/* Setup memory for this platform */
void platform_init_mmu_mappings(void)
{
	uint32_t i;
	uint32_t sections;
	uint32_t table_size = ARRAY_SIZE(mmu_section_table);

	/* Configure the MMU page entries for memory read from the
           mmu_section_table */
	for (i = 0; i < table_size; i++)
	{
		sections = mmu_section_table[i].num_of_sections;

		while (sections--)
		{
			arm_mmu_map_section(mmu_section_table[i].paddress +
								sections * MB,
								mmu_section_table[i].vaddress +
								sections * MB,
								mmu_section_table[i].flags);
		}
	}
}

/* Setup memory for this platform */
static void ddr_based_mmu_mappings(mmu_section_t *table,uint32_t table_size)
{
	uint32_t i;
	uint32_t sections;

	/* Configure the MMU page entries for memory read from the
		 mmu_section_table */
	for (i = 0; i < table_size; i++)
	{
		sections = table->num_of_sections;

		while (sections--)
		{
			arm_mmu_map_section(table->paddress +
								sections * MB,
								table->vaddress +
								sections * MB,
								table->flags);
		}
	table++;
	}
}

int platform_use_identity_mmu_mappings(void)
{
	/* Use only the mappings specified in this file. */
	return 0;
}

bool platform_is_mdm9206()
{
	uint32_t platform_id = board_platform_id();
	bool ret;

	switch(platform_id)
	{
		case MDM9206:
			ret = true;
			break;
		default:
			ret = false;
	}

	return ret;
}
