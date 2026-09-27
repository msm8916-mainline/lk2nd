/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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

#if DEVICE_TREE /* If using device tree */

#include <reg.h>
#include <debug.h>
#include <malloc.h>
#include <smem.h>
#include <stdint.h>
#include <libfdt.h>
#include <platform/iomap.h>
#include <dev_tree.h>

/* SMEM region that must be reserved */
#define SMEM_START MSM_SHARED_BASE
#define SMEM_SIZE  0x00200000
#define SMEM_END   (SMEM_START + SMEM_SIZE)

/* Funtion to add the ram partition entries into device tree.
 * The function assumes that all the entire fixed memory regions should
 * be listed in the first bank of the passed in ddr regions.
 * SMEM region (0x80000000-0x801FFFFF) is excluded from usable memory.
 */
uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset)
{
	ram_partition ptn_entry;
	unsigned int index;
	int ret = 0;
	uint32_t len = 0;
	uint64_t mem_start, mem_end;

	/* Make sure RAM partition table is initialized */
	ASSERT(smem_ram_ptable_init_v1());

	len = smem_get_ram_ptable_len();

	/* Calculating the size of the mem_info_ptr */
	for (index = 0 ; index < len; index++)
	{
		smem_get_ram_ptable_entry(&ptn_entry, index);

		if (!smem_ram_ptn_is_ddr(&ptn_entry))
			continue;

		mem_start = ptn_entry.start;
		mem_end = ptn_entry.start + ptn_entry.size;

		if (mem_end <= SMEM_START || mem_start >= SMEM_END)
		{
			dprintf(SPEW, "Adding memory outside SMEM: start=0x%llx size=0x%llx\n",
				ptn_entry.start, ptn_entry.size);

			ret = dev_tree_add_mem_info(fdt, memory_node_offset,
						    ptn_entry.start, ptn_entry.size);

			if (ret)
			{
				dprintf(CRITICAL,
					"Failed to add memory outside SMEM (ret=%d)\n", ret);
				goto target_dev_tree_mem_err;
			}
		}
		else if (mem_start >= SMEM_START && mem_end <= SMEM_END)
		{
			dprintf(SPEW, "Skipping SMEM only region: start=0x%llx size=0x%llx\n",
				ptn_entry.start, ptn_entry.size);
		}
		else
		{
			if (mem_start < SMEM_START)
			{
				uint64_t before_size = SMEM_START - mem_start;
				dprintf(SPEW,
					"Adding memory before SMEM: start=0x%llx size=0x%llx\n",
					mem_start, before_size);

				ret = dev_tree_add_mem_info(fdt, memory_node_offset,
							    mem_start, before_size);

				if (ret)
				{
					dprintf(CRITICAL,
						"Failed to add memory before SMEM (ret=%d)\n",
						ret);
					goto target_dev_tree_mem_err;
				}
			}
			if (mem_end > SMEM_END)
			{
				uint64_t after_size = mem_end - SMEM_END;
				dprintf(SPEW,
					"Adding memory after SMEM: start=0x%llx size=0x%llx\n",
					(uint64_t)SMEM_END, after_size);

				ret = dev_tree_add_mem_info(fdt, memory_node_offset,
							    SMEM_END, after_size);

				if (ret)
				{
					dprintf(CRITICAL,
						"Failed to add memory after SMEM (ret=%d)\n",
						ret);
					goto target_dev_tree_mem_err;
				}
			}
		}
	}

target_dev_tree_mem_err:

	return ret;
}
#endif /* DEVICE_TREE */
