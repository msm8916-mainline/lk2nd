/* Copyright (c) 2009-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Code Aurora nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
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
 *
 */

#if DEVICE_TREE /* If using device tree */

#include <reg.h>
#include <debug.h>
#include <malloc.h>
#include <smem.h>
#include <stdint.h>
#include <libfdt.h>
#include <platform/iomap.h>

#define SIZE_1M             (1024 * 1024)

typedef struct {
	uint32_t size;
	uint32_t start_addr;
}mem_info;


mem_info copper_default_fixed_memory[] = {
	{	.size = (132 * SIZE_1M),
		.start_addr = SDRAM_START_ADDR
	},
	{	.size = SIZE_1M,
		.start_addr = SDRAM_START_ADDR +
				(250 * SIZE_1M) +
				(5 * SIZE_1M)
	},
	{	.size = (240 * SIZE_1M),
		.start_addr = SDRAM_START_ADDR +
				(16 * SIZE_1M) +
				(256 * SIZE_1M)
	},
	{	.size = (512 * SIZE_1M),
		.start_addr = (512 * SIZE_1M),
	}
};

uint32_t *target_mem_dev_tree_create(uint32_t *ptr, uint32_t size, uint32_t addr)
{
	*ptr++ = cpu_to_fdt32(addr);
	*ptr++ = cpu_to_fdt32(size);

	return ptr;
}

uint32_t *target_dev_tree_create(uint32_t *ptr,
								 mem_info usable_mem_map[],
								 uint32_t num_regions)
{
	uint32_t i;

	ASSERT(num_regions);

	dprintf(SPEW, "Number of HLOS regions in 1st bank = %u\n", num_regions);

	for (i = 0; i < num_regions; i++)
	{
            ptr = target_mem_dev_tree_create(ptr,
                            usable_mem_map[i].size,
                            usable_mem_map[i].start_addr);
	}
	return ptr;
}

uint32_t* target_dev_tree_mem(uint32_t *num_of_entries)
{
    struct smem_ram_ptable ram_ptable;
	uint32_t *meminfo_ptr;
	uint32_t num_of_sections;
	uint32_t *ptr;

	/* Make sure RAM partition table is initialized */
	ASSERT(smem_ram_ptable_init(&ram_ptable));

	num_of_sections = ARRAY_SIZE(copper_default_fixed_memory);
	*num_of_entries = num_of_sections;

	meminfo_ptr = (uint32_t*) malloc(sizeof(uint32_t) * num_of_sections * 2);
	ptr = meminfo_ptr;

	target_dev_tree_create(ptr,
						   copper_default_fixed_memory,
						   ARRAY_SIZE(copper_default_fixed_memory));

    return meminfo_ptr;
}

void *target_get_scratch_address(void)
{
	return ((void *)SCRATCH_ADDR);
}

unsigned target_get_max_flash_size(void)
{
	return (512 * 1024 * 1024);
}
#endif /* DEVICE_TREE */

