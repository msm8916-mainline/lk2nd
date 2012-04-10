/* Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
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

#include <reg.h>
#include <debug.h>
#include <smem.h>
#include <stdint.h>
#include "board.h"
#include "baseband.h"

#define SIZE_1M     (1024 * 1024)
#define SIZE_2M     (2 * SIZE_1M)
#define SIZE_256M   (256 * SIZE_1M)
#define SIZE_512M   (512 * SIZE_1M)

#define ATAG_MEM            0x54410002

#define PHYS_MEM_START_ADDR 0x80000000

typedef struct {
	uint32_t size;
	uint32_t start_addr;
}atag_mem_info;

atag_mem_info apq8064_standalone_first_256M[] = {
	{	.size = (140 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + SIZE_2M
	},
	{	.size = (60 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + (0x9E * SIZE_1M)
	},
	{	.size = (7 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + (0xF7 * SIZE_1M)
	}
};

atag_mem_info apq8064_fusion_first_256M[] = {
	{	.size = (140 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + SIZE_2M
	},
	{	.size = (74 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + (0x90 * SIZE_1M)
	},
	{	.size = (7 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + (0xF7 * SIZE_1M)
	},
	{	.size = SIZE_1M,
		.start_addr = PHYS_MEM_START_ADDR + (0xFF * SIZE_1M)
	}
};

atag_mem_info mpq8064_first_256M[] = {
	{	.size = (140 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + SIZE_2M
	},
	{	.size = (74 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + (0x90 * SIZE_1M)
	},
	{	.size = (14 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + (0xF2 * SIZE_1M)
	}
};

atag_mem_info msm8960_default_first_256M[] = {
	{	.size = (140 * SIZE_1M),
		.start_addr = PHYS_MEM_START_ADDR + SIZE_2M
	}
};


unsigned *target_mem_atag_create(unsigned *ptr, uint32_t size, uint32_t addr)
{
	*ptr++ = 4;
	*ptr++ = ATAG_MEM;
	*ptr++ = size;
	*ptr++ = addr;

	return ptr;
}

unsigned *target_atag(unsigned *ptr, atag_mem_info usable_mem_map[], unsigned num_regions)
{
	unsigned i;

	dprintf(SPEW, "Number of regions for HLOS in 1st 256MB = %u\n", num_regions);
	for (i=0; i < num_regions; i++)
	{
		ptr = target_mem_atag_create(ptr,
						usable_mem_map[i].size,
						usable_mem_map[i].start_addr);
	}
	return ptr;
}

unsigned *target_first_256M_atag(unsigned *ptr)
{
	unsigned int platform_id = board_platform_id();
	unsigned int baseband = board_baseband();

	switch (platform_id) {
		case APQ8064:
			if(baseband == BASEBAND_MDM)
			{
				/* Use 8064 Fusion 3 memory map */
				ptr = target_atag(ptr,
							apq8064_fusion_first_256M,
							ARRAY_SIZE(apq8064_fusion_first_256M));
			} else {
				/* Use 8064 standalone memory map */
				ptr = target_atag(ptr,
							apq8064_standalone_first_256M,
							ARRAY_SIZE(apq8064_standalone_first_256M));
			}
			break;

		case MPQ8064:
			ptr = target_atag(ptr, mpq8064_first_256M, ARRAY_SIZE(mpq8064_first_256M));
			break;
		case MSM8960: /* fall through */
		default:
			ptr = target_atag(ptr,
						msm8960_default_first_256M,
						ARRAY_SIZE(msm8960_default_first_256M));
			break;
	}
	return ptr;
}

unsigned *target_atag_mem(unsigned *ptr)
{
	struct smem_ram_ptable ram_ptable;
	uint8_t i = 0;

	if (smem_ram_ptable_init(&ram_ptable))
	{
		for (i = 0; i < ram_ptable.len; i++)
		{
			if (ram_ptable.parts[i].category == SDRAM &&
				(ram_ptable.parts[i].type == SYS_MEMORY) &&
				(ram_ptable.parts[i].start == PHYS_MEM_START_ADDR))
			{
				ASSERT(ram_ptable.parts[i].size >= SIZE_256M);

				ptr = target_first_256M_atag(ptr);

				if (ram_ptable.parts[i].size > SIZE_256M)
				{
					ptr = target_mem_atag_create(ptr,
							(ram_ptable.parts[i].size - SIZE_256M),
							(ram_ptable.parts[i].start + SIZE_256M));
				}
			}

			/* Pass along all other usable memory regions to Linux */
			if (ram_ptable.parts[i].category == SDRAM &&
				(ram_ptable.parts[i].type == SYS_MEMORY) &&
				(ram_ptable.parts[i].start != PHYS_MEM_START_ADDR))
			{
				ptr = target_mem_atag_create(ptr,
							ram_ptable.parts[i].size,
							ram_ptable.parts[i].start);
			}
		}
	} else {
		dprintf(CRITICAL, "ERROR: Unable to read RAM partition\n");
		ASSERT(0);
	}

	return ptr;
}

void *target_get_scratch_address(void)
{
	return ((void *) SCRATCH_ADDR);
}

unsigned target_get_max_flash_size(void)
{
	return (SIZE_512M);
}
