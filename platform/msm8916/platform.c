/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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
#include <platform/irqs.h>
#include <qgic.h>
#include <qtimer.h>
#include <mmu.h>
#include <arch/arm/mmu.h>
#include <smem.h>
#include <board.h>
#include <boot_stats.h>
#include <platform.h>
#include <target/display.h>

#define MSM_IOMAP_SIZE ((MSM_IOMAP_END - MSM_IOMAP_BASE)/MB)
#define A53_SS_SIZE    ((A53_SS_END - A53_SS_BASE)/MB)

/* LK memory - cacheable, write through */
#define LK_MEMORY         (MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE | \
					MMU_MEMORY_AP_READ_WRITE)

/* Peripherals - non-shared device */
#define IOMAP_MEMORY      (MMU_MEMORY_TYPE_DEVICE_SHARED | \
			MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

#define COMMON_MEMORY       (MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
                           MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

#define SCRATCH_MEMORY       (MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE | \
                           MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

static mmu_section_t mmu_section_table[] = {
/*           Physical addr,     Virtual addr,     Size (in MB),     Flags */
	{    MEMBASE,           MEMBASE,          (MEMSIZE / MB),   LK_MEMORY},
	{    MSM_IOMAP_BASE,    MSM_IOMAP_BASE,   MSM_IOMAP_SIZE,   IOMAP_MEMORY},
	{    A53_SS_BASE,       A53_SS_BASE,      A53_SS_SIZE,      IOMAP_MEMORY},
	{    SYSTEM_IMEM_BASE,  SYSTEM_IMEM_BASE, 1,                COMMON_MEMORY},
	{    MSM_SHARED_BASE,   MSM_SHARED_BASE,  1,                COMMON_MEMORY},
	{    MIPI_FB_ADDR,      MIPI_FB_ADDR,     10,              COMMON_MEMORY},
	{    SCRATCH_ADDR,      SCRATCH_ADDR,     256,              SCRATCH_MEMORY},
        {    RPMB_SND_RCV_BUF,      RPMB_SND_RCV_BUF,        RPMB_SND_RCV_BUF_SZ,    IOMAP_MEMORY},
};


int platform_is_msm8939();
int platform_is_msm8929();

void platform_early_init(void)
{
	/* Reset and disable watchdog if necessary */
	if (readl(MSM_WATCHDOG_EN)) {
		writel(1, MSM_WATCHDOG_RST);
		writel(0, MSM_WATCHDOG_EN);
	}

	board_init();
	platform_clock_init();
	qgic_init();
	qtimer_init();
	scm_init();
}

int qtmr_irq()
{
	if (platform_is_msm8939() || platform_is_msm8929())
		return INT_QTMR_FRM_0_PHYSICAL_TIMER_EXP_8x39;
	else
		return INT_QTMR_FRM_0_PHYSICAL_TIMER_EXP_8x16;
}

void platform_init(void)
{
	dprintf(INFO, "platform_init()\n");
}

void platform_uninit(void)
{
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

int platform_use_identity_mmu_mappings(void)
{
	/* Use only the mappings specified in this file. */
	return 0;
}
/* Setup memory for this platform */
void platform_init_mmu_mappings(void)
{
	uint32_t i;
	uint32_t sections;
	uint32_t table_size = ARRAY_SIZE(mmu_section_table);
	uint32_t ddr_start = get_ddr_start();

	/*Mapping the ddr start address for loading the kernel about 90 MB*/
	sections = 90;
	while(sections--)
	{
		arm_mmu_map_section(ddr_start + sections * MB, ddr_start + sections* MB, COMMON_MEMORY);
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

addr_t platform_map_fb(addr_t phys_addr, uint32_t size)
{
	if (phys_addr != MIPI_FB_ADDR) {
		addr_t end = phys_addr + size;
		addr_t addr;
		for (addr = phys_addr; addr < end; addr += MB) {
			arm_mmu_map_section(addr, addr, COMMON_MEMORY);
		}
		arm_mmu_flush();
	}

	return phys_addr;
}

addr_t platform_get_virt_to_phys_mapping(addr_t virt_addr)
{
	/* Using 1-1 mapping on this platform. */
	return virt_addr;
}

addr_t platform_get_phys_to_virt_mapping(addr_t phys_addr)
{
	/* Using 1-1 mapping on this platform. */
	return phys_addr;
}

int platform_is_msm8939()
{
	uint32_t platform = board_platform_id();
	uint32_t ret = 0;

	switch(platform)
	{
		case MSM8939:
		case APQ8036:
		case APQ8039:
		case MSM8236:
		case MSM8636:
		case MSM8936:
		case MSM8239:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

int platform_is_msm8929()
{
	uint32_t platform = board_platform_id();
	uint32_t ret = 0;

	switch(platform)
	{
		case MSM8929:
		case MSM8629:
		case MSM8229:
		case APQ8029:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

/* DYNAMIC SMEM REGION feature enables LK to dynamically
 * read the SMEM addr info from TCSR_TZ_WONCE register.
 * The first word read, if indicates a MAGIC number, then
 * Dynamic SMEM is assumed to be enabled. Read the remaining
 * SMEM info for SMEM Size and Phy_addr from the other bytes.
 */
uint32_t platform_get_smem_base_addr()
{
	struct smem_addr_info *smem_info = NULL;

	smem_info = (struct smem_addr_info *)readl(TCSR_TZ_WONCE);
	if(smem_info && (smem_info->identifier == SMEM_TARGET_INFO_IDENTIFIER))
		return smem_info->phy_addr;
	else
		return MSM_SHARED_BASE;
}
