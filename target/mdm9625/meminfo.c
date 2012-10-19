/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

#include <reg.h>
#include <debug.h>
#include <malloc.h>
#include <smem.h>
#include <stdint.h>
#include <libfdt.h>
#include <platform.h>
#include <platform/iomap.h>
#include <dev_tree.h>

#define SIZE_1M             (1024 * 1024)

typedef struct {
	uint32_t size;
	uint32_t start_addr;
}mem_info;

static struct smem_ram_ptable ram_ptable;

mem_info mdm9625_default_fixed_memory[] = {
	{	.size = (29 * SIZE_1M),
		.start_addr = SDRAM_START_ADDR +
				(2 * SIZE_1M)
	},
	{	.size = (10 * SIZE_1M),
		.start_addr = SDRAM_START_ADDR +
				(118 * SIZE_1M)
	},
};

struct smem_ram_ptable* target_smem_ram_ptable_init()
{
   /* Make sure RAM partition table is initialized */
   ASSERT(smem_ram_ptable_init(&ram_ptable));

   return &ram_ptable;
}

int target_add_first_mem_bank(void *fdt,
							  uint32_t offset,
							  mem_info usable_mem_map[],
							  uint32_t num_regions)
{
	uint32_t i;
	int ret;

	ASSERT(num_regions);

	dprintf(SPEW, "Number of HLOS regions in 1st bank = %u\n", num_regions);

	for (i = 0; i < num_regions; i++)
	{
           ret = dev_tree_add_mem_info(fdt,
									   offset,
									   usable_mem_map[i].start_addr,
									   usable_mem_map[i].size);
	}
	return ret;
}

/* Funtion to add the ram partition entries into device tree.
 * The function assumes that all the entire fixed memory regions should
 * be listed in the first bank of the passed in ddr regions.
 */
uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset)
{
    uint32_t last_fixed_addr;
    int n;
    unsigned int i;
	int ret;

    n = ARRAY_SIZE(mdm9625_default_fixed_memory);

    last_fixed_addr = mdm9625_default_fixed_memory[n-1].start_addr +
					mdm9625_default_fixed_memory[n-1].size;

    for (i = 0; i < ram_ptable.len; i++)
    {
        if((ram_ptable.parts[i].category == SDRAM) &&
		   (ram_ptable.parts[i].type == SYS_MEMORY))
        {
            if((ram_ptable.parts[i].start <= last_fixed_addr) &&
               ((ram_ptable.parts[i].start + ram_ptable.parts[i].size) >= last_fixed_addr))
            {

				/* Pass along all fixed memory regions to Linux */
				 ret = target_add_first_mem_bank(fdt,
												 memory_node_offset,
												 mdm9625_default_fixed_memory,
												 ARRAY_SIZE(mdm9625_default_fixed_memory));

				if (ret)
				{
					dprintf(CRITICAL, "Failed to add first bank fixed memory addresses\n");
					goto target_dev_tree_mem_err;
				}

				if((ram_ptable.parts[i].start + ram_ptable.parts[i].size) != last_fixed_addr)
                {
					/* Pass the memory beyond the fixed memory present in the partition */
					ret = dev_tree_add_mem_info(fdt,
												memory_node_offset,
												ram_ptable.parts[i].start + last_fixed_addr,
												ram_ptable.parts[i].size - last_fixed_addr);

					if (ret)
					{
						dprintf(CRITICAL, "Failed to add first bank memory addresses\n");
						goto target_dev_tree_mem_err;
					}
                }
            }
			else
			{
				/* Pass along all other usable memory regions to Linux */
				ret = dev_tree_add_mem_info(fdt,
											memory_node_offset,
											ram_ptable.parts[i].start,
											ram_ptable.parts[i].size);

				if (ret)
				{
					dprintf(CRITICAL, "Failed to add secondary banks memory addresses\n");
					goto target_dev_tree_mem_err;
				}
			}
        }
    }

target_dev_tree_mem_err:

    return ret;
}

void *target_get_scratch_address(void)
{
	return ((void *) VA((addr_t)SCRATCH_REGION1));
}

unsigned target_get_max_flash_size(void)
{
	return (SCRATCH_REGION1_SIZE + SCRATCH_REGION2_SIZE);
}
