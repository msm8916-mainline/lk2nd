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

/* IMEM memory - cacheable, write through */
#define IMEM_MEMORY       (MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
                           MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

static mmu_section_t mmu_section_table[] = {
/*   Physical addr,    Virtual addr,     Size (in MB),   Flags */
	{MEMBASE,          MEMBASE,          (MEMSIZE / MB), LK_MEMORY},
	{MSM_IOMAP_BASE,   MSM_IOMAP_BASE,   MSM_IOMAP_SIZE, IOMAP_MEMORY},
	/* IMEM  needs a seperate entry in the table as it's length is only 0x8000. */
	{SYSTEM_IMEM_BASE, SYSTEM_IMEM_BASE, 1,              IMEM_MEMORY},
};

static struct smem_ram_ptable ram_ptable;

/* Boot timestamps */
#define BS_INFO_OFFSET     (0x6B0)
#define BS_INFO_ADDR_V1    (RPM_MSG_RAM_BASE     + BS_INFO_OFFSET)
#define BS_INFO_ADDR_V2    (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)

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

uint32_t platform_get_sclk_count(void)
{
	return readl(MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL);
}

/* Check for 8974 chip */
int platform_is_8974()
{
	uint32_t platform = board_platform_id();
	int ret = 0;

	switch(platform)
	{
		case APQ8074:
		case MSM8274:
		case MSM8674:
		case MSM8974:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

/* Check for 8974 PRO chip */
int platform_is_8974Pro()
{
	uint32_t platform = board_platform_id();
	int ret = 0;

	switch(platform)
	{
		case MSM8974AC:
		case MSM8674AC:
		case MSM8274AC:
		case APQ8074AC:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

/* Check for 8974PRO AC chip */
int platform_is_8974ac()
{
	uint32_t platform = board_platform_id();
	int ret = 0;

	switch(platform)
	{
		case APQ8074AA:
		case APQ8074AB:
		case APQ8074AC:

		case MSM8274AA:
		case MSM8274AB:
		case MSM8274AC:

		case MSM8674AA:
		case MSM8674AB:
		case MSM8674AC:

		case MSM8974AA:
		case MSM8974AB:
		case MSM8974AC:

			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

addr_t get_bs_info_addr()
{
	uint32_t soc_ver = board_soc_version();

	if (platform_is_8974() && (soc_ver < BOARD_SOC_VERSION2))
		return ((addr_t)BS_INFO_ADDR_V1);
	else
		return ((addr_t)BS_INFO_ADDR_V2);

}

void platform_uninit(void)
{
#if DISPLAY_SPLASH_SCREEN
	display_shutdown();
#endif

	qtimer_uninit();
}

int platform_use_identity_mmu_mappings(void)
{
	/* Use only the mappings specified in this file. */
	return 0;
}

addr_t platform_get_virt_to_phys_mapping(addr_t virt_addr)
{
	/* Return same address as we are using 1-1 mapping. */
	return virt_addr;
}

addr_t platform_get_phys_to_virt_mapping(addr_t phys_addr)
{
	/* Return same address as we are using 1-1 mapping. */
	return phys_addr;
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
				while(sections--) {
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
	for (i = 0; i < table_size; i++) {
		sections = mmu_section_table[i].num_of_sections;

		while (sections--) {
			arm_mmu_map_section(mmu_section_table[i].paddress +
					    sections * MB,
					    mmu_section_table[i].vaddress +
					    sections * MB,
					    mmu_section_table[i].flags);
		}
	}
}
