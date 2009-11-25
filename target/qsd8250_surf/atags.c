/* Copyright (c) 2009, Code Aurora Forum.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
	size = EBT1_SIZE2_512M;
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
