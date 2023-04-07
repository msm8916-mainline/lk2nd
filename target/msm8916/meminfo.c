/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
#include <platform/iomap.h>
#include <dev_tree.h>

uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset)
{
	ram_partition ptn_entry;
	unsigned int index;
	int ret = 0;
	uint32_t len = 0;

	/* Make sure RAM partition table is initialized */
	ASSERT(smem_ram_ptable_init_v1());

	len = smem_get_ram_ptable_len();

	/* Calculating the size of the mem_info_ptr */
	for (index = 0 ; index < len; index++)
	{
		smem_get_ram_ptable_entry(&ptn_entry, index);

		if((ptn_entry.category == SDRAM) &&
			(ptn_entry.type == SYS_MEMORY))
		{

			/* Pass along all other usable memory regions to Linux */
			ret = dev_tree_add_mem_info(fdt,
							memory_node_offset,
							ptn_entry.start,
							ptn_entry.size);

			if (ret)
			{
				dprintf(CRITICAL, "Failed to add secondary banks memory addresses\n");
				goto target_dev_tree_mem_err;
			}
		}
	}
target_dev_tree_mem_err:

	return ret;
}

void *target_get_scratch_address(void)
{
	return ((void *)SCRATCH_ADDR);
}

unsigned target_get_max_flash_size(void)
{
	// 256 MiB breaks some low-memory devices, 192 MiB should be sufficient.
	return (192 * 1024 * 1024);
}
