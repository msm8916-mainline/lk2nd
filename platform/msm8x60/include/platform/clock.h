/*
 * * Copyright (c) 2010, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#ifndef __PLATFORM_MSM8X60_CLOCK_H
#define __PLATFORM_MSM8X60_CLOCK_H

/* MMSS CLK CTR base address */
#define MSM_MMSS_CLK_CTL    0x04000000
#define REG_MM(off)     (MSM_MMSS_CLK_CTL + (off))

#define AHB_NS_REG                              REG_MM(0x0004)
#define AXI_NS_REG                              REG_MM(0x0014)
#define MM_PLL0_CONFIG_REG                      REG_MM(0x0310)
#define MM_PLL0_L_VAL_REG                       REG_MM(0x0304)
#define MM_PLL0_M_VAL_REG                       REG_MM(0x0308)
#define MM_PLL0_MODE_REG                        REG_MM(0x0300)
#define MM_PLL0_N_VAL_REG                       REG_MM(0x030C)
#define MM_PLL0_STATUS_REG                      REG_MM(0x0318)
#define MM_PLL1_CONFIG_REG                      REG_MM(0x032C)
#define MM_PLL1_L_VAL_REG                       REG_MM(0x0320)
#define MM_PLL1_M_VAL_REG                       REG_MM(0x0324)
#define MM_PLL1_MODE_REG                        REG_MM(0x031C)
#define MM_PLL1_N_VAL_REG                       REG_MM(0x0328)
#define MM_PLL1_STATUS_REG                      REG_MM(0x0334)
#define MM_PLL2_CONFIG_REG                      REG_MM(0x0348)
#define MM_PLL2_L_VAL_REG                       REG_MM(0x033C)
#define MM_PLL2_M_VAL_REG                       REG_MM(0x0340)
#define MM_PLL2_MODE_REG                        REG_MM(0x0338)
#define MM_PLL2_N_VAL_REG                       REG_MM(0x0344)
#define MM_PLL2_STATUS_REG                      REG_MM(0x0350)

/* LCD related clock defines */
#define MMSS_AHB_NS_REG     (MSM_MMSS_CLK_CTL + 0x04)
#define MMSS_AHB_EN_REG     (MSM_MMSS_CLK_CTL + 0x08)
#define MMSS_AXI_NS_REG     (MSM_MMSS_CLK_CTL + 0x14)
#define MMSS_MAXI_EN_REG    (MSM_MMSS_CLK_CTL + 0x18)
#define MMSS_MAXI_EN2_REG   (MSM_MMSS_CLK_CTL + 0x20)
#define MMSS_SAXI_EN_REG    (MSM_MMSS_CLK_CTL + 0x30)

#define MDP_CC_REG      (MSM_MMSS_CLK_CTL + 0xC0)
#define MDP_MD_REG      (MSM_MMSS_CLK_CTL + 0xC4)
#define MDP_NS_REG      (MSM_MMSS_CLK_CTL + 0xD0)
#define LCD_PIXEL_CC_REG    (MSM_MMSS_CLK_CTL + 0xD4)
#define LCD_PIXEL_NS_REG    (MSM_MMSS_CLK_CTL + 0xDC)
#define LCD_PIXEL_MD_REG    (MSM_MMSS_CLK_CTL + 0xD8)

/* Configured at 200 MHz */
#define MDP_NS_VAL              0x3F000008
#define MDP_MD_VAL              0x000001FB
#define MDP_CC_VAL              0x00000400

/* Configured at 53.99 MHz */
#define PIXEL_NS_VAL            0xFE4F4002
#define PIXEL_MD_VAL            0x00A9FDA6
#define PIXEL_CC_VAL            0x00000080

enum clk_sources {
    PLL_0 = 0,
    PLL_1,
    PLL_2,
    PLL_3,
    PLL_4,
    PLL_5,
    PLL_6,
    PLL_7,
    PLL_8,
    MXO,
    PXO,
    CXO,
    NUM_SRC
};

#endif
