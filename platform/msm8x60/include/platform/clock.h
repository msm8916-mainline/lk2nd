/*
 * * Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
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

#define MSM_MMSS_CLK_CTL_SIZE 4096
#define REG_MM(off)     (MSM_MMSS_CLK_CTL_BASE + (off))

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
#define MMSS_AHB_NS_REG     REG_MM(0x04)
#define MMSS_AHB_EN_REG     REG_MM(0x08)
#define MMSS_AXI_NS_REG     REG_MM(0x14)
#define MMSS_MAXI_EN_REG    REG_MM(0x18)
#define MMSS_MAXI_EN2_REG   REG_MM(0x20)
#define MMSS_SAXI_EN_REG    REG_MM(0x30)
#define DSI_NS_REG          REG_MM(0x54)
#define DSI_MD_REG          REG_MM(0x50)
#define DSI_CC_REG          REG_MM(0x4C)
#define MISC_CC2_REG        REG_MM(0x5C)

#define MDP_CC_REG          REG_MM(0xC0)
#define MDP_MD_REG          REG_MM(0xC4)
#define MDP_NS_REG          REG_MM(0xD0)
#define MMSS_PIXEL_MD_REG   REG_MM(0xD8)
#define MMSS_PIXEL_NS_REG   REG_MM(0xDC)
#define MMSS_PIXEL_CC_REG   REG_MM(0xD4)

/* MMSS DSI Pixel Registers not MMSS Pixel */
#define DSI_PIXEL_MD_REG    REG_MM(0x134)
#define DSI_PIXEL_NS_REG    REG_MM(0x138)
#define DSI_PIXEL_CC_REG    REG_MM(0x130)

/* Configured at 200 MHz */
#define MDP_NS_VAL              0x3F000008
#define MDP_MD_VAL              0x000001FB
#define MDP_CC_VAL              0x00000400

/* Configured at 53.99 MHz */
#define PIXEL_NS_VAL            0xFE4F4002
#define PIXEL_MD_VAL            0x00A9FDA6
#define PIXEL_CC_VAL            0x00000080

/* Configured at 25 MHz (AUO Panel) */
#define PIXEL_NS_VAL_25M        0xFFF10002
#define PIXEL_MD_VAL_25M        0x0001FFF0
#define PIXEL_CC_VAL_25M        0x00000080

/* PIXEL clock index */
#define PIXEL_CLK_INDEX_54M	0	/* 53.99MHz */
#define PIXEL_CLK_INDEX_25M	1	/* 25MHz */

#define MSM_CLK_CTL_BASE        0x00900000
#define BB_PLL8_L_VAL_REG       (MSM_CLK_CTL_BASE + 0x3144)
#define BB_PLL8_M_VAL_REG       (MSM_CLK_CTL_BASE + 0x3148)
#define BB_PLL8_MODE_REG        (MSM_CLK_CTL_BASE + 0x3140)
#define BB_PLL8_N_VAL_REG       (MSM_CLK_CTL_BASE + 0x314C)
#define CE2_HCLK_CTL            (MSM_CLK_CTL_BASE + 0x2740)

/* NS/MD value for UART */
#define UART_DM_CLK_NS_115200   0xFD940043
#define UART_DM_CLK_MD_115200   0x0006FD8E

#define UART_DM_CLK_RX_TX_BIT_RATE 0xEE

/* GSBI/I2C QUP APPS CLK definitions */
#define I2C_CLK_MD_24MHz        0x000100FB
#define I2C_CLK_NS_24MHz        0x00FC005B

/* NS/MD value for MMC */
#define SDC_CLK_NS_400KHZ    0x0010005B
#define SDC_CLK_MD_400KHZ    0x0001000F

#define SDC_CLK_NS_48MHZ     0x00FE005B
#define SDC_CLK_MD_48MHZ     0x000100FD

/* HDMI Clocks */
#define SW_RESET_AHB_REG  REG_MM(0x020C)
#define SW_RESET_CORE_REG REG_MM(0x0210)
#define TV_CC_REG         REG_MM(0x00EC)
#define TV_NS_REG         REG_MM(0x00F4)
#define TV_MD_REG         REG_MM(0x00F0)

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

void hsusb_clock_init(void);
void clock_config_uart_dm(uint8_t id);
void clock_config_i2c(uint8_t id, uint32_t freq);
void mdp_clock_init(void);
void mmss_pixel_clock_configure(uint32_t pclk_index);

#endif
