/* Copyright (c) 2009-2010, Code Aurora Forum. All rights reserved.
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

#define SIZE_44M              0x02C00000	// 44M
#define EBI1_ADDR_1026M       0x40200000

#define SIZE_128M             0x08000000	// 128M
#define EBI1_ADDR_1152M       0x48000000

#define SIZE_256M             0x10000000	// 256M
#define EBI1_ADDR_1280M       0x50000000

#define SIZE_768M             0x30000000	// 256M + 512M
#define SIZE_1792M            0x70000000	// 256M + 512M + 1G

#define EBI1_CS1_ADDR_BASE    0x00A40024

unsigned *target_atag_mem(unsigned *ptr)
{
	unsigned value = 0;

	/* ATAG_MEM */
	*ptr++ = 4;
	*ptr++ = 0x54410002;
	*ptr++ = SIZE_44M;
	*ptr++ = EBI1_ADDR_1026M;

	*ptr++ = 4;
	*ptr++ = 0x54410002;
	*ptr++ = SIZE_128M;
	*ptr++ = EBI1_ADDR_1152M;

	value = readl(EBI1_CS1_ADDR_BASE);
	value = (value >> 8) & 0xFF;

	if (value == 0x50) {
		/* For 512MB RAM */
		*ptr++ = 4;
		*ptr++ = 0x54410002;
		*ptr++ = SIZE_256M;
		*ptr++ = EBI1_ADDR_1280M;
	} else if (value == 0x60) {
		/* For 1GB RAM */
		*ptr++ = 4;
		*ptr++ = 0x54410002;
		*ptr++ = SIZE_768M;
		*ptr++ = EBI1_ADDR_1280M;
	} else if (value == 0x80) {
		/* For 2GB RAM */
		*ptr++ = 4;
		*ptr++ = 0x54410002;
		//*ptr++ = SIZE_1792M;
		*ptr++ = SIZE_768M;
		*ptr++ = EBI1_ADDR_1280M;
	}

	return ptr;
}

void *target_get_scratch_address(void)
{
	return ((void *)SCRATCH_ADDR);
}

unsigned target_get_max_flash_size(void)
{
	return (384 * 1024 * 1024);
}
