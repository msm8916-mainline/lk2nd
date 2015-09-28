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

#include <sys/types.h>
#include <debug.h>
#include <arch/arm/mmu.h>
#include <arch/ops.h>
#include <mmu.h>
#include <string.h>
#include <smem.h>

/* COMMON memory - cacheable, write through */
#define COMMON_MEMORY       (MMU_MEMORY_TYPE_NORMAL_WRITE_THROUGH | \
                           MMU_MEMORY_AP_READ_WRITE | MMU_MEMORY_XN)

#define MB (1024 * 1024)
static mmu_section_t ramdump_mmu_section_table_4gb[] =
{
  /*        Physical addr,    Virtual addr,     Mapping type ,              Size (in MB),            Flags */
	{    0xC0000000,        0xC0000000,       MMU_L2_NS_SECTION_MAPPING,  512,            COMMON_MEMORY},
	{    0x100000000,       0xC0000000,       MMU_L2_NS_SECTION_MAPPING,  1024,            COMMON_MEMORY},
	/* This entry is 484 MB because the hyp uses last 28MB for page tables */
	{    0x140000000,       0xC0000000,       MMU_L2_NS_SECTION_MAPPING,  484,            COMMON_MEMORY},
};

static mmu_section_t ramdump_mmu_section_table_3gb[] =
{
  /*        Physical addr,    Virtual addr,     Mapping type ,              Size (in MB),            Flags */
	{    0x20000000,        0x20000000,       MMU_L2_NS_SECTION_MAPPING,  512,            COMMON_MEMORY},
	{    0xC0000000,        0xC0000000,       MMU_L2_NS_SECTION_MAPPING,  484,            COMMON_MEMORY},
};
uint32_t vaddr_4gb[] = {0xc2300000, 0xcd000000, 0xde000000};
uint64_t paddr_4gb[] = {0xc2300000, 0x10d000000, 0x15e000000};

uint32_t vaddr_3gb[] = {0x23000000, 0xcd000000, 0xde000000};
uint64_t paddr_3gb[] = {0x23000000, 0xcd000000, 0xde000000};

void ramdump_table_map()
{
	uint32_t i, j;
	uint32_t table_sz = 0;
	char *ptr = NULL;
	bool pass_access = true;
	bool pass_conversion = true;
	uint64_t paddr_v;
	uint32_t vaddr_v;
	uint32_t *vaddr = NULL;
	uint64_t *paddr = NULL;
	mmu_section_t *ramdump_mmu_section_table = NULL;


	if (smem_get_ddr_size() == MEM_4GB)
	{
		vaddr = vaddr_4gb;
		paddr = paddr_4gb;
		ramdump_mmu_section_table = ramdump_mmu_section_table_4gb;
		table_sz = ARRAY_SIZE(ramdump_mmu_section_table_4gb);
	}
	else if (smem_get_ddr_size() == MEM_3GB)
	{
		vaddr = vaddr_3gb;
		paddr = paddr_3gb;
		ramdump_mmu_section_table = ramdump_mmu_section_table_3gb;
		table_sz = ARRAY_SIZE(ramdump_mmu_section_table_3gb);
	}
	for (i = 0 ; i < table_sz; i++)
	{
		arm_mmu_map_entry(&ramdump_mmu_section_table[i]);
		vaddr_v = physical_to_virtual_mapping(paddr[i]);
		if (vaddr_v != vaddr[i])
			pass_conversion = false;
		paddr_v = virtual_to_physical_mapping(vaddr[i]);
		if (paddr_v != paddr[i])
			pass_conversion = false;
		ptr = (char *)(uintptr_t)ramdump_mmu_section_table[i].vaddress;

		for (j = 0 ; j < (ramdump_mmu_section_table[i].size * MB)/6; j++)
		{
			strlcpy(ptr, "hello", 6);
			ptr+=6;
		}

		ptr = (char *)(uintptr_t)ramdump_mmu_section_table[i].vaddress;

		for (j = 0 ; j < (ramdump_mmu_section_table[i].size * MB)/6; j++)
		{
			if (memcmp((void *)ptr, "hello", 5))
			{
				pass_access = false;
				break;
			}
			ptr+=6;
		}
		if (pass_access)
			dprintf(CRITICAL, "LAPE TEST PASS for addr: 0x%llx\n", ramdump_mmu_section_table[i].paddress);
	}
		if (pass_conversion)
			dprintf(CRITICAL, "Physical to virtual conversion TEST PASS\n");
}
