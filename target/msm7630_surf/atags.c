/* Copyright (c) 2009, Code Aurora Forum. All rights reserved.
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

#define EBI1_ADDR_128M        0x08000000
#define SIZE_1M               0x00100000

static int scratch_addr = -1;

unsigned *target_atag_mem(unsigned *ptr)
{
	struct smem_ram_ptable ram_ptable;
	unsigned i = 0;

	if (smem_ram_ptable_init(&ram_ptable)) {
		for (i = 0; i < ram_ptable.len; i++) {
			if ((ram_ptable.parts[i].attr == READWRITE)
			    && (ram_ptable.parts[i].domain == APPS_DOMAIN)
			    && (ram_ptable.parts[i].type == APPS_MEMORY)
			    && (ram_ptable.parts[i].category != IMEM)) {
				/* ATAG_MEM */
				*ptr++ = 4;
				*ptr++ = 0x54410002;
				*ptr++ = ram_ptable.parts[i].size;
				*ptr++ = ram_ptable.parts[i].start;
			}

			/* Check for modem bootloader memory that can be reclaimed */
			if ((ram_ptable.parts[i].attr == READWRITE)
			    && (ram_ptable.parts[i].domain == APPS_DOMAIN)
			    && (ram_ptable.parts[i].type ==
				BOOT_REGION_MEMORY1)) {
				/* ATAG_MEM_OSBL */
				*ptr++ = 4;
				*ptr++ = 0x5441000C;
				*ptr++ = ram_ptable.parts[i].size;
				*ptr++ = ram_ptable.parts[i].start;
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
	struct smem_ram_ptable ram_ptable;
	unsigned i = 0;

	if (smem_ram_ptable_init(&ram_ptable)) {
		for (i = 0; i < ram_ptable.len; i++) {
			if ((ram_ptable.parts[i].attr == READWRITE)
			    && (ram_ptable.parts[i].domain == APPS_DOMAIN)
			    && (ram_ptable.parts[i].start != 0x0)) {
				if (ram_ptable.parts[i].size >=
				    FASTBOOT_BUF_SIZE) {
					scratch_addr =
					    ram_ptable.parts[i].start;
					break;
				}
			}
		}
	} else {
		dprintf(CRITICAL, "ERROR: Unable to read RAM partition\n");
		ASSERT(0);
	}

	return (void *)((scratch_addr == -1) ? EBI1_ADDR_128M : scratch_addr);
}

unsigned target_get_max_flash_size(void)
{
	return (256 * 1024 * 1024);
}
