/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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

static struct smem_ram_ptable ram_ptable;

struct smem_ram_ptable* target_smem_ram_ptable_init()
{
   /* Make sure RAM partition table is initialized */
   ASSERT(smem_ram_ptable_init(&ram_ptable));

   return &ram_ptable;
}

/* Funtion to add the ram partition entries into device tree.
 * The function assumes that all the entire fixed memory regions should
 * be listed in the first bank of the passed in ddr regions.
 */
uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset)
{
    unsigned int i;
	int ret;

     for (i = 0; i < ram_ptable.len; i++)
    {
        if ((ram_ptable.parts[i].category == SDRAM) &&
		   (ram_ptable.parts[i].type == SYS_MEMORY))
        {
		if (ram_ptable.parts[i].start == 0)
			{
				/* Pass along all other usable memory regions to Linux */
				/* Any memory not accessible by the kernel which is below the
				 * kernel start address must *not* be given to the kernel.
				 */
				ret = dev_tree_add_mem_info(fdt,
											memory_node_offset,
											ram_ptable.parts[i].start + 3*SIZE_1M,
											ram_ptable.parts[i].size  - 3*SIZE_1M);

			}
			else
			{
					/* Pass along all other usable memory regions to Linux */
					ret = dev_tree_add_mem_info(fdt,
												memory_node_offset,
												ram_ptable.parts[i].start,
												ram_ptable.parts[i].size);

			}


			if (ret)
			{
				dprintf(CRITICAL, "Failed to add memory bank addresses to device tree\n");
				goto target_dev_tree_mem_err;
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
