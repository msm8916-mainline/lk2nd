/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#ifndef __MSM8916_CLOCK_H
#define __MSM8916_CLOCK_H

#include <clock.h>
#include <clock_lib2.h>

#define UART_DM_CLK_RX_TX_BIT_RATE 0xCC

#define REG_MM(off)                     (CLK_CTL_BASE + (off))

#define MDP_GDSCR                       REG_MM(0x4D078)
#define GDSC_POWER_ON_BIT               BIT(31)
#define GDSC_POWER_ON_STATUS_BIT        BIT(29)
#define GDSC_EN_FEW_WAIT_MASK           (0x0F << 16)
#define GDSC_EN_FEW_WAIT_256_MASK       BIT(19)

#define VSYNC_CMD_RCGR                  REG_MM(0x4D02C)
#define VSYNC_CFG_RCGR                  REG_MM(0x4D030)
#define MDSS_VSYNC_CBCR                 REG_MM(0x4D090)

#define MDP_CMD_RCGR                    REG_MM(0x4D014)
#define MDP_CFG_RCGR                    REG_MM(0x4D018)
#define MDP_CBCR                        REG_MM(0x4D088)
#define MDP_AHB_CBCR                    REG_MM(0x4D07C)
#define MDP_AXI_CBCR                    REG_MM(0x4D080)

#define DSI_BYTE0_CMD_RCGR              REG_MM(0x4D044)
#define DSI_BYTE0_CFG_RCGR              REG_MM(0x4D048)
#define DSI_BYTE0_CBCR                  REG_MM(0x4D094)
#define DSI_ESC0_CMD_RCGR               REG_MM(0x4D05C)
#define DSI_ESC0_CFG_RCGR               REG_MM(0x4D060)
#define DSI_ESC0_CBCR                   REG_MM(0x4D098)
#define DSI_PIXEL0_CMD_RCGR             REG_MM(0x4D000)
#define DSI_PIXEL0_CFG_RCGR             REG_MM(0x4D004)
#define DSI_PIXEL0_CBCR                 REG_MM(0x4D084)
#define DSI_PIXEL0_M                    REG_MM(0x4D008)
#define DSI_PIXEL0_N                    REG_MM(0x4D00C)
#define DSI_PIXEL0_D                    REG_MM(0x4D010)

#define DSI_BYTE1_CMD_RCGR              REG_MM(0x4D0B0)
#define DSI_BYTE1_CFG_RCGR              REG_MM(0x4D0B4)
#define DSI_BYTE1_CBCR                  REG_MM(0x4D0A0)
#define DSI_ESC1_CMD_RCGR               REG_MM(0x4D0A8)
#define DSI_ESC1_CFG_RCGR               REG_MM(0x4D0AC)
#define DSI_ESC1_CBCR                   REG_MM(0x4D09C)
#define DSI_PIXEL1_CMD_RCGR             REG_MM(0x4D0B8)
#define DSI_PIXEL1_CFG_RCGR             REG_MM(0x4D0BC)
#define DSI_PIXEL1_CBCR                 REG_MM(0x4D0A4)
#define DSI_PIXEL1_M                    REG_MM(0x4D0C0)
#define DSI_PIXEL1_N                    REG_MM(0x4D0C4)
#define DSI_PIXEL1_D                    REG_MM(0x4D0C8)

void platform_clock_init(void);

void clock_init_mmc(uint32_t interface);
void clock_config_mmc(uint32_t interface, uint32_t freq);
void clock_config_uart_dm(uint8_t id);
void hsusb_clock_init(void);
void clock_config_ce(uint8_t instance);
void mdp_clock_init(void);
void mdp_gdsc_ctrl(uint8_t enable);
void clock_ce_enable(uint8_t instance);
void clock_ce_disable(uint8_t instance);
void clock_config_blsp_i2c(uint8_t blsp_id, uint8_t qup_id);
#endif
