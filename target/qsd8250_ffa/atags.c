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

#define EBI1_SIZE1   	0x0E800000 //232MB for 256/512/1024MB RAM
#define EBI1_ADDR1    	0x20000000

#define EBI1_SIZE2_512M 0x10000000 //256MB for 512MB RAM
#define EBI1_SIZE2_1G   0x30000000 //768MB for 1GB RAM
#define EBI1_ADDR2    	0x30000000

static unsigned check_1gb_mem()
{
	// check for 1GB
	unsigned adr1 = 0x57000000;
	unsigned adr2 = 0x5F000000;
	unsigned value1 = 0x55555555;
	unsigned value2 = 0xAAAAAAAA;

	writel(value1, adr1);
	writel(value2, adr2);

	return ((value1 == readl(adr1)) && (value2 == readl(adr2)));
}


unsigned* target_atag_mem(unsigned* ptr)
{
	unsigned size;

	/* ATAG_MEM */
	/* 1st segment */
	*ptr++ = 4;
	*ptr++ = 0x54410002;
	*ptr++ = EBI1_SIZE1;
	*ptr++ = EBI1_ADDR1;

	/* 2nd segment */
#ifdef USE_512M_RAM
	size = EBI1_SIZE2_512M;
#else
	size = 0;
#endif
	if (check_1gb_mem()) {
		size = EBI1_SIZE2_1G;
	}

	if (size > 0) {
		*ptr++ = 4;
		*ptr++ = 0x54410002;
		*ptr++ = size;
		*ptr++ = EBI1_ADDR2;
	}

	return ptr;
}
