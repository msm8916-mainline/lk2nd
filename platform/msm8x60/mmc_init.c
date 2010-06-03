/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.

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

#include <string.h>
#include <stdlib.h>
#include <debug.h>
#include <reg.h>
#include "mmc.h"

#define CLK_CTL_BASE    0x00900000

#define SDC_NS(n)       (CLK_CTL_BASE + 0x282C + 32*((n) - 1))
#define SDC1_NS         SDC_NS(1)
#define SDC2_NS         SDC_NS(2)
#define SDC3_NS         SDC_NS(3)
#define SDC4_NS         SDC_NS(4)
#define SDC5_NS         SDC_NS(5)

#define SDC_MD(n)       (CLK_CTL_BASE + 0x2828 + 32*((n) - 1))
#define SDC1_MD         SDC_MD(1)
#define SDC2_MD         SDC_MD(2)
#define SDC3_MD         SDC_MD(3)
#define SDC4_MD         SDC_MD(4)
#define SDC5_MD         SDC_MD(5)

static void mmc_set_clk(unsigned ns, unsigned md)
{
    unsigned int val;
    /*Clock Init*/
    // 1. Set bit 7 in the NS registers
    val = 1 << 7;
    writel(val, SDC1_NS);

    //2. Program MD registers
    writel(md, SDC1_MD);

    //3. Program NS resgister OR'd with Bit 7
    val = 1 << 7;
    val |= ns;
    writel(val, SDC1_NS);

    //4. Clear bit 7 of NS register
    val = 1 << 7;
    val = ~val;
    val = val & readl(SDC1_NS);
    writel(val, SDC1_NS);

    //5. For MD != NA set bit 8 of NS register
    val = 1 << 8;
    val = val | readl(SDC1_NS);
    writel(val, SDC1_NS);

    //6. Set bit 11 in NS register
    val = 1 << 11;
    val = val | readl(SDC1_NS);
    writel(val, SDC1_NS);

    //7. Set bit 9 in NS register
    val = 1 << 9;
    val = val | readl(SDC1_NS);
    writel(val, SDC1_NS);
}


void clock_set_enable (unsigned int mclk)
{
    if (mclk == MMC_CLK_400KHZ)
    {
        mmc_set_clk(0x0010005B, 0x0001000F);
    }
    else if (mclk == MMC_CLK_20MHZ)
    {
        mmc_set_clk(0x00ED0043, 0x000100EC);
    }
    else if (mclk == MMC_CLK_48MHZ)
    {
        mmc_set_clk(0x00FE005B, 0x000100FD);
    }
}
