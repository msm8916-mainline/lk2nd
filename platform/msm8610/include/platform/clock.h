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

#ifndef __MSM8610_CLOCK_H
#define __MSM8610_CLOCK_H

#include <clock.h>
#include <clock_lib2.h>

#define REG_MM(off)                     (MSM_MMSS_CLK_CTL_BASE + (off))

#define VSYNC_CMD_RCGR                  REG_MM(0x2080)
#define VSYNC_CFG_RCGR                  REG_MM(0x2084)
#define AXI_CMD_RCGR                    REG_MM(0x5040)
#define AXI_CFG_RCGR                    REG_MM(0x5044)

#define MDP_AXI_CBCR                    REG_MM(0x2314)
#define MDP_AHB_CBCR                    REG_MM(0x2318)
#define MDP_VSYNC_CBCR                  REG_MM(0x231C)
#define MDP_DSI_CBCR                    REG_MM(0x2320)
#define MDP_LCDC_CBCR                   REG_MM(0x2340)

#define MMSS_S0_AXI_CBCR                REG_MM(0x5064)
#define MMSS_MMSSNOC_AXI_CBCR           REG_MM(0x506C)

#define DSI_CBCR                        REG_MM(0x2324)
#define DSI_BYTE_CBCR                   REG_MM(0x2328)
#define DSI_ESC_CBCR                    REG_MM(0x232C)
#define DSI_AHB_CBCR                    REG_MM(0x2330)
#define DSI_PCLK_CBCR                   REG_MM(0x233C)

#define DSI_CMD_RCGR                    REG_MM(0x2020)
#define DSI_CFG_RCGR                    REG_MM(0x2024)
#define DSI_PCLK_CMD_RCGR               REG_MM(0x2000)
#define DSI_PCLK_CFG_RCGR               REG_MM(0x2004)
#define DSI_BYTE_CMD_RCGR               REG_MM(0x2120)
#define DSI_BYTE_CFG_RCGR               REG_MM(0x2124)

#define UART_DM_CLK_RX_TX_BIT_RATE 0xCC

#define VCO_MAX_DIVIDER		 256
#define VCO_MIN_RATE       600000000
#define VCO_MAX_RATE       1200000000
#define VCO_PREF_DIV_RATIO 26
#define VCO_PARENT_RATE    19200000

void platform_clock_init(void);

void clock_init_mmc(uint32_t interface);
void clock_config_mmc(uint32_t interface, uint32_t freq);
void clock_config_uart_dm(uint8_t id);
void hsusb_clock_init(void);
void mdp_clock_enable(void);
void mdp_clock_disable(void);
void dsi_clock_enable(uint32_t dsiclk_rate, uint32_t byteclk_rate);
void dsi_clock_disable(void);
void clock_ce_enable(uint8_t instance);
void clock_ce_disable(uint8_t instance);

#endif
