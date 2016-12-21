/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#include <platform/irqs.h>
#include <platform/clock.h>
#include <qgic.h>
#include <qtimer.h>
#include <mmu.h>
#include <arch/arm/mmu.h>
#include <smem.h>
#include <board.h>
#include <boot_stats.h>
#include <platform.h>
#include <target/display.h>

#define MSM8976_SOC_V11 0x10001
#define MSM_IOMAP_SIZE ((MSM_IOMAP_END - MSM_IOMAP_BASE)/MB)
#define APPS_SS_SIZE   ((APPS_SS_END - APPS_SS_BASE)/MB)

/* LK memory - cacheable, write through */
#define LK_MEMORY         (MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE | \
				MMU_MEMORY_AP_READ_WRITE)

/* Peripherals - non-shared device */
#define IOMAP_MEMORY      (MMU_MEMORY_TYPE_DEVICE_SHARED | \
				MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

/* IMEM memory - cacheable, write through */
#define COMMON_MEMORY     (MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
				MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

#define SCRATCH_MEMORY    (MMU_MEMORY_TYPE_NORMAL_WRITE_BACK_ALLOCATE | \
				MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

static mmu_section_t mmu_section_table[] = {
/*           Physical addr,         Virtual addr,            Size (in MB),     Flags */
	{    MEMBASE,               MEMBASE,                 (MEMSIZE / MB),         LK_MEMORY},
	{    MSM_IOMAP_BASE,        MSM_IOMAP_BASE,          MSM_IOMAP_SIZE,         IOMAP_MEMORY},
	{    APPS_SS_BASE,          APPS_SS_BASE,            APPS_SS_SIZE,           IOMAP_MEMORY},
	{    MSM_SHARED_IMEM_BASE,  MSM_SHARED_IMEM_BASE,    1,                      COMMON_MEMORY},
	{    SCRATCH_ADDR,          SCRATCH_ADDR,            SCRATCH_SIZE,           SCRATCH_MEMORY},
	{    MIPI_FB_ADDR,          MIPI_FB_ADDR,            20,                     COMMON_MEMORY},
	{    RPMB_SND_RCV_BUF,      RPMB_SND_RCV_BUF,        RPMB_SND_RCV_BUF_SZ,    IOMAP_MEMORY},
};

static mmu_section_t mmu_section_table_512[] = {
/*           Physical addr,         Virtual addr,            Size (in MB),     Flags */
	{    MEMBASE,               MEMBASE,                 (MEMSIZE / MB),         LK_MEMORY},
	{    MSM_IOMAP_BASE,        MSM_IOMAP_BASE,          MSM_IOMAP_SIZE,         IOMAP_MEMORY},
	{    APPS_SS_BASE,          APPS_SS_BASE,            APPS_SS_SIZE,           IOMAP_MEMORY},
	{    MSM_SHARED_IMEM_BASE,  MSM_SHARED_IMEM_BASE,    1,                      COMMON_MEMORY},
	{    SCRATCH_ADDR_512,      SCRATCH_ADDR_512,        SCRATCH_SIZE_512,       SCRATCH_MEMORY},
	{    MIPI_FB_ADDR,          MIPI_FB_ADDR,            20,                     COMMON_MEMORY},
	{    RPMB_SND_RCV_BUF_512,  RPMB_SND_RCV_BUF_512,    RPMB_SND_RCV_BUF_SZ,    IOMAP_MEMORY},
};

void platform_early_init(void)
{
	board_init();
	platform_clock_init();
	qgic_init();
	qtimer_init();
	scm_init();
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

/* Setup MMU mapping for this platform */
void platform_init_mmu_mappings(void)
{
	uint32_t i;
	uint32_t sections;
	uint32_t table_size;
	uint32_t ddr_start = get_ddr_start();
	uint32_t smem_addr = platform_get_smem_base_addr();
	mmu_section_t *table_addr;

	/*Mapping the ddr start address for loading the kernel about 90 MB*/
	sections = 90;
	while(sections--)
	{
		arm_mmu_map_section(ddr_start + sections * MB, ddr_start + sections* MB, SCRATCH_MEMORY);
	}


	/* Mapping the SMEM addr */
	arm_mmu_map_section(smem_addr, smem_addr, COMMON_MEMORY);

	/* Configure the MMU page entries for memory read from the
	   mmu_section_table */
	if(smem_get_ddr_size() > 0x20000000)
	{
		table_addr = mmu_section_table;
		table_size = ARRAY_SIZE(mmu_section_table);
	}
	else
	{
		table_addr = mmu_section_table_512;
		table_size = ARRAY_SIZE(mmu_section_table_512);
	}

	for (i = 0; i < table_size; i++)
	{
		sections = table_addr->num_of_sections;

		while (sections--)
		{
			arm_mmu_map_section(table_addr->paddress +
								sections * MB,
								table_addr->vaddress +
								sections * MB,
								table_addr->flags);
		}
		table_addr++;
	}

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

uint32_t platform_get_max_periph()
{
	return 256;
}

int platform_is_msm8917()
{
	uint32_t platform = board_platform_id();
	uint32_t ret = 0;

	switch(platform)
	{
		case MSM8917:
		case MSM8920:
		case MSM8217:
		case MSM8617:
		case APQ8017:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

int platform_is_msm8937()
{
	uint32_t platform = board_platform_id();
	uint32_t ret = 0;

	switch(platform)
	{
		case MSM8937:
		case MSM8940:
		case APQ8037:
			ret = 1;
			break;
		default:
			ret = 0;
		};

	return ret;
}

int platform_is_msm8952()
{
	uint32_t platform = board_platform_id();
	uint32_t ret = 0;

	switch(platform)
	{
	case MSM8952:
	case APQ8052:
		ret = 1;
		break;
	default:
		ret = 0;
	};

	return ret;
}

int platform_is_msm8956()
{
	uint32_t platform = board_platform_id();
	uint32_t ret = 0;

	switch(platform)
	{
		case MSM8956:
		case APQ8056:
		case APQ8076:
		case MSM8976:
			ret = 1;
			break;
		default:
			ret = 0;
	};

	return ret;
}

uint32_t platform_get_tz_app_add()
{
	if(platform_is_msm8937() || platform_is_msm8917())
		return APP_REGION_ADDR_8937;
	else
		return APP_REGION_ADDR_8952;
}

uint32_t platform_get_tz_app_size()
{
	if(platform_is_msm8937() || platform_is_msm8917())
		return APP_REGION_SIZE_8937;
	else
		return APP_REGION_SIZE_8952;
}

uint32_t platform_get_apcs_ipc_base()
{
	if(platform_is_msm8917())
		return APCS_ALIAS1_IPC_INTERRUPT_1;
	else
		return APCS_ALIAS0_IPC_INTERRUPT_2;
}

uint32_t platform_is_msm8976_v_1_1()
{
	uint32_t soc_ver = board_soc_version();
	uint32_t ret = 0;

	if(soc_ver == MSM8976_SOC_V11)
		ret = 1;

	return ret;
}
