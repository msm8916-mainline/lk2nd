/*
 * * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
#ifndef __PLATFORM_MSM8960_CLOCK_H
#define __PLATFORM_MSM8960_CLOCK_H

/* GSBI/I2C QUP APPS CLK definitions */
#define I2C_CLK_MD_24MHz       0x000100FB
#define I2C_CLK_NS_24MHz       0x00FC005B

/* NS/MD value for USB XCVR */
#define USB_XCVR_CLK_NS        0x00E400C3
#define USB_XCVR_CLK_MD        0x000500DF

/* NS/MD value for UART */
#define UART_DM_CLK_NS_115200  0xFFE40040
#define UART_DM_CLK_MD_115200  0x0002FFE2


#define UART_DM_CLK_RX_TX_BIT_RATE 0xFF


/* NS/MD value for MMC */
#define SDC_CLK_NS_400KHZ    0x00440040
#define SDC_CLK_MD_400KHZ    0x00010043

#define SDC_CLK_NS_48MHZ     0x00FE005B
#define SDC_CLK_MD_48MHZ     0x000100FD

void hsusb_clock_init(void);
void clock_config_uart_dm(uint8_t id);
void clock_config_i2c(uint8_t id, uint32_t freq);

#define REG_MM(off)     (MSM_MMSS_CLK_CTL_BASE + (off))

#define MDP_CC_REG              REG_MM(0xC0)
#define MDP_MD_REG              REG_MM(0xC4)
#define MDP_NS_REG              REG_MM(0xD0)
#define ESC_CC_REG              REG_MM(0xCC)
#define ESC_NS_REG              REG_MM(0x11C)
#define BYTE_CC_REG             REG_MM(0x90)
#define BYTE_NS_REG             REG_MM(0xB0)
#define PIXEL_CC_REG            REG_MM(0x130)
#define PIXEL_MD_REG            REG_MM(0x134)
#define PIXEL_NS_REG            REG_MM(0x138)
#define DSI_NS_REG              REG_MM(0x54)
#define DSI_MD_REG              REG_MM(0x50)
#define DSI_CC_REG              REG_MM(0x4C)
#define MISC_CC2_REG            REG_MM(0x5C)
#define MDP_LUT_CC_REG          REG_MM(0x016C)

#define MM_PLL1_MODE_REG        REG_MM(0x031C)
#define MM_PLL1_STATUS_REG      REG_MM(0x0334)
#define MM_PLL1_CONFIG_REG      REG_MM(0x032C)

/* Configured for 200MHz */
#define MDP_NS_VAL            0x3F3FC008
#define MDP_MD_VAL            0x000001FB
#define MDP_CC_VAL            0x00000500

/* Configured at 13.5 MHz */
#define ESC_NS_VAL            0x00001000
#define ESC_CC_VAL            0x00000004

#define BYTE_NS_VAL           0x00000001
#define BYTE_CC_VAL           0x00000004

#define PIXEL_NS_VAL          0x00F80003
#define PIXEL_MD_VAL          0x000001FB
#define PIXEL_CC_VAL          0x00000080

#define DSI_NS_VAL            0xFA000003
#define DSI_MD_VAL            0x000003FB
#define DSI_CC_VAL            0x00000080

#define MDP_LUT_VAL           0x00000001

void config_mmss_clk(  uint32_t ns,
        uint32_t md,
        uint32_t cc,
        uint32_t ns_addr,
        uint32_t md_addr,
        uint32_t cc_addr);
void pll1_enable(void);
void config_mdp_lut_clk(void);
void mdp_clock_init(void);

#endif
