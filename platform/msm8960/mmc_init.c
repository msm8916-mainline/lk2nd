/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include "mmc.h"

#define MSM_BOOT_SDC_NS_VAL_400KHZ    0x00440040
#define MSM_BOOT_SDC_MD_VAL_400KHZ    0x00010043

#define MSM_BOOT_SDC_NS_VAL_48MHZ     0x00FE005B
#define MSM_BOOT_SDC_MD_VAL_48MHZ     0x000100FD

void clock_set_enable (unsigned char slot, unsigned int mclk)
{

	if (mclk == MMC_CLK_400KHZ)
	{
		clock_config(MSM_BOOT_SDC_NS_VAL_400KHZ,
                             MSM_BOOT_SDC_MD_VAL_400KHZ,
                             SDC_NS(slot),
                             SDC_MD(slot));
		mdelay(10);
	}
	else if (mclk == MMC_CLK_48MHZ)
	{
		clock_config(MSM_BOOT_SDC_NS_VAL_48MHZ,
                             MSM_BOOT_SDC_MD_VAL_48MHZ,
                             SDC_NS(slot),
                             SDC_MD(slot));
		mdelay(10);
	}
	else
	{
		ASSERT(0);
	}
}

