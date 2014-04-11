/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include <qgic.h>
#include <qtimer.h>
#include <platform/clock.h>
#include <mmu.h>
#include <arch/arm/mmu.h>
#include <smem.h>
#include <board.h>
#include <boot_stats.h>

#define MB (1024*1024)

#define MSM_IOMAP_SIZE ((MSM_IOMAP_END - MSM_IOMAP_BASE)/MB)

/* LK memory - cacheable, write through */
#define LK_MEMORY         (MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
                           MMU_MEMORY_AP_READ_WRITE)

/* Peripherals - non-shared device */
#define IOMAP_MEMORY      (MMU_MEMORY_TYPE_DEVICE_SHARED | \
                           MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

static mmu_section_t mmu_section_table[] = {
/*       Physical addr,    Virtual addr,    Size (in MB),    Flags */
	{    MEMBASE,          MEMBASE,        (MEMSIZE / MB),   LK_MEMORY},
	{    MSM_IOMAP_BASE,   MSM_IOMAP_BASE,  MSM_IOMAP_SIZE,  IOMAP_MEMORY},
};

static struct smem_ram_ptable ram_ptable;

void platform_early_init(void)
{
	board_init();
	platform_clock_init();
	qgic_init();
	qtimer_init();
}

void platform_init(void)
{
	dprintf(INFO, "platform_init()\n");
}

void platform_uninit(void)
{
#if DISPLAY_SPLASH_SCREEN
	display_shutdown();
#endif

	qtimer_uninit();
}

uint32_t platform_get_sclk_count(void)
{
	return readl(MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL);
}

addr_t get_bs_info_addr()
{
	return ((addr_t)BS_INFO_ADDR);
}

/* Setup memory for this platform */
void platform_init_mmu_mappings(void)
{
	uint32_t i;
	uint32_t sections;
	uint32_t table_size = ARRAY_SIZE(mmu_section_table);

	ASSERT(smem_ram_ptable_init(&ram_ptable));

	/* Configure the MMU page entries for SDRAM and IMEM memory read
	   from the smem ram table*/
	for(i = 0; i < ram_ptable.len; i++)
	{
		if(ram_ptable.parts[i].type == SYS_MEMORY)
		{
			if((ram_ptable.parts[i].category == SDRAM) ||
			   (ram_ptable.parts[i].category == IMEM))
			{
				/* Check to ensure that start address is 1MB aligned */
				ASSERT((ram_ptable.parts[i].start & 0xFFFFF) == 0);

				sections = (ram_ptable.parts[i].size) / MB;
				while(sections--)
				{
					arm_mmu_map_section(ram_ptable.parts[i].start +
										sections * MB,
										ram_ptable.parts[i].start +
										sections * MB,
										(MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
										 MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN));
				}
			}
		}
	}

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

/* DYNAMIC SMEM REGION feature enables LK to dynamically read the SMEM address from MSM_DYNAMIC_SHARED_BASE.
 * If the feature is enabled(by reading the MAGIC as mentioned below) then the SMEM address is read from
 * MSM_DYNAMIC_SHARED_BASE otherwise revert to the older way of getting SMEM address - MSM_SHARED_BASE */

uint32_t platform_get_smem_base_addr()
{
	uint32_t smem_addr = 0;

	if(readl(SMEM_TARGET_INFO_ADDR) == SMEM_TARGET_INFO_IDENTIFIER)
		smem_addr = readl(MSM_DYNAMIC_SHARED_BASE);
	else
		smem_addr = MSM_SHARED_BASE;

	return smem_addr;
}
