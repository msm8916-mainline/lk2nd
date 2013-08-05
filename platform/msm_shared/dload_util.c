/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#include <stdlib.h>
#include <reg.h>
#include <dload_util.h>

#define NORMAL_DLOAD_COOKIE_0       0xE47B337D
#define NORMAL_DLOAD_COOKIE_1       0xCE14091A

#define EMERGENCY_DLOAD_COOKIE_0    0x322A4F99
#define EMERGENCY_DLOAD_COOKIE_1    0xC67E4350
#define EMERGENCY_DLOAD_COOKIE_2    0x77777777

void dload_util_write_cookie(uint32_t target_dload_mode_addr,
		enum dload_mode mode)
{
	if (mode == NORMAL_DLOAD)
	{
		writel(NORMAL_DLOAD_COOKIE_0, target_dload_mode_addr);
		writel(NORMAL_DLOAD_COOKIE_1,
				target_dload_mode_addr + sizeof(uint32_t));
	}
	else
	{
		writel(EMERGENCY_DLOAD_COOKIE_0, target_dload_mode_addr);
		writel(EMERGENCY_DLOAD_COOKIE_1,
				target_dload_mode_addr + sizeof(uint32_t));
		writel(EMERGENCY_DLOAD_COOKIE_2,
				target_dload_mode_addr + 2 * sizeof(uint32_t));
	}

	dsb();
}
