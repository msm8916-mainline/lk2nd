/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011,2015 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *	may be used to endorse or promote products derived from this
 *	software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <reg.h>
#include <debug.h>
#include <kernel/thread.h>
#include <platform/debug.h>
#include <arch/arm/mmu.h>
#include <platform/iomap.h>
#include <smem.h>
#include <mmu.h>
#include <qgic.h>

#define MB                                  (1024*1024)

#define MSM_IOMAP_SIZE                      ((MSM_IOMAP_END - MSM_IOMAP_BASE)/MB)

/* LK memory - Strongly ordered, executable */
#define LK_MEMORY                             (MMU_MEMORY_TYPE_NORMAL | \
                                              MMU_MEMORY_AP_READ_WRITE)
/* Scratch memory - Strongly ordered, non-executable */
#define SCRATCH_MEMORY                        (MMU_MEMORY_TYPE_NORMAL | \
                                              MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)
/* Peripherals - shared device */
#define IOMAP_MEMORY                          (MMU_MEMORY_TYPE_DEVICE_SHARED | \
                                              MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

#define SCRATCH_REGION1_VIRT_START            SCRATCH_REGION1
#define SCRATCH_REGION2_VIRT_START            (SCRATCH_REGION1_VIRT_START + \
                                              (SCRATCH_REGION1_SIZE))


/* Map all the accesssible memory according to the following rules:
 * 1. Map 1MB from MSM_SHARED_BASE with 1 -1 mapping.
 * 2. Map MEMBASE - MEMSIZE with 1 -1 mapping.
 * 3. Map all the scratch regions immediately after Appsbl memory.
 *     Virtual addresses start right after Appsbl Virtual address.
 * 4. Map all the IOMAP space with 1 - 1 mapping.
 * 5. Map all the rest of the SDRAM/ IMEM regions as 1 -1.
 */
mmu_section_t mmu_section_table[] = {
/*   Physical addr,         Virtual addr,               Size (in MB),              Flags   */
        {MSM_SHARED_BASE,       MSM_SHARED_BASE,            1,                         SCRATCH_MEMORY},
        {MEMBASE,               MEMBASE,                    MEMSIZE / MB,              LK_MEMORY},
        {SCRATCH_REGION1,       SCRATCH_REGION1_VIRT_START, SCRATCH_REGION1_SIZE / MB, SCRATCH_MEMORY},
        {SCRATCH_REGION2,       SCRATCH_REGION2_VIRT_START, SCRATCH_REGION2_SIZE / MB, SCRATCH_MEMORY},
};

static uint32_t ticks_per_sec = 0;

extern void platform_uninit_timer(void);

void platform_init_timer();

void platform_early_init(void)
{
	uint8_t cfg_bid = 0x1;
	uint8_t cfg_pid = 0x1;
	uint8_t gsbi_id = target_uart_gsbi();

	uart_dm_init(gsbi_id, GSBI_BASE(gsbi_id), GSBI_UART_DM_BASE(gsbi_id));

	/* Timers - QGIC Config */
	writel((cfg_bid << 7 | cfg_pid << 10), APCS_GLB_QGIC_CFG);
	qgic_init();
	platform_init_timer();
}

void platform_init(void)
{
	dprintf(INFO, "platform_init()\n");
	acpu_clock_init();
}

void platform_uninit(void)
{
	platform_uninit_timer();
}

void platform_init_mmu_mappings(void)
{
	struct smem_ram_ptable *ram_ptable;
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
			arm_mmu_map_section(mmu_section_table[i].paddress + sections * MB,
								mmu_section_table[i].vaddress + sections * MB,
								mmu_section_table[i].flags);
		}
	}
}


/* Initialize DGT timer */
void platform_init_timer(void)
{
	/* disable timer */
	writel(0, DGT_ENABLE);

	/* DGT uses CXO source which is 19.2MHz.
	* Set clock divider to 4.
	*/
	writel(3, DGT_CLK_CTL);

	ticks_per_sec = 4800000; /* (19.2MHz/4) */
}

/* Returns timer ticks per sec */
uint32_t platform_tick_rate(void)
{
	return ticks_per_sec;
}

/* Do not use default identitiy mappings. */
int platform_use_identity_mmu_mappings(void)
{
	return 0;
}
