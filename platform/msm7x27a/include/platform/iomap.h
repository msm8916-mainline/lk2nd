/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *	may be used to endorse or promote products derived from this
 *	software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PLATFORM_MSM7627A_IOMAP_H_
#define _PLATFORM_MSM7627A_IOMAP_H_

#define MSM_GPIO1_BASE	0xA9200000
#define MSM_GPIO2_BASE	0xA9300000

#define MSM_UART1_BASE	0xA9A00000
#define MSM_UART2_BASE	0xA9B00000
#define MSM_UART3_BASE	0xA9C00000

#define MSM_VIC_BASE	0xC0000000
#define MSM_GPT_BASE	0xC0100000

#define GPT_REG(off)    (MSM_GPT_BASE + (off))

#define GPT_MATCH_VAL     GPT_REG(0x0000)
#define GPT_COUNT_VAL     GPT_REG(0x0004)
#define GPT_ENABLE        GPT_REG(0x0008)
#define GPT_CLEAR         GPT_REG(0x000C)
#define DGT_MATCH_VAL     GPT_REG(0x0010)
#define DGT_COUNT_VAL     GPT_REG(0x0014)
#define DGT_ENABLE        GPT_REG(0x0018)
#define DGT_CLEAR         GPT_REG(0x001C)
#define SPSS_TIMER_STATUS GPT_REG(0x0034)

#define MSM_CSR_BASE	0xC0100000
#define MSM_CLK_CTL_BASE	0xA8600000

#define MSM_SHARED_BASE 0x00100000

#define MSM_SDC1_BASE   0xA0400000
#define MSM_SDC3_BASE   0xA0600000

#define MIPI_DSI_BASE                         (0xA1100000)
#define DSI_PHY_SW_RESET                      (0xA1100128)
#define REG_DSI(off)                          (MIPI_DSI_BASE + (off))
#define MDP_BASE                              (0xAA200000)
#define REG_MDP(off)                          (MDP_BASE + (off))
#define DSIPHY_REGULATOR_BASE                 (0x2CC)
#define DSIPHY_TIMING_BASE                    (0x260)
#define DSIPHY_CTRL_BASE                      (0x290)
#define DSIPHY_PLL_BASE                       (0x200)
#define DSIPHY_STRENGTH_BASE                  (0x2A0)

/* Range 0 - 4 */
#define DSIPHY_REGULATOR_CTRL(x) REG_DSI(DSIPHY_REGULATOR_BASE + (x) * 4)
/* Range 0 - 11 */
#define DSIPHY_TIMING_CTRL(x)    REG_DSI(DSIPHY_TIMING_BASE + (x) * 4)
/* Range 0 - 3 */
#define DSIPHY_CTRL(x)           REG_DSI(DSIPHY_CTRL_BASE + (x) * 4)
/* Range 0 - 2 */
#define DSIPHY_STRENGTH_CTRL(x)  REG_DSI(DSIPHY_STRENGTH_BASE + (x) * 4)
/* Range 0 - 19 */
#define DSIPHY_PLL_CTRL(x)       REG_DSI(DSIPHY_PLL_BASE + (x) * 4)

#define MDP_DMA_P_CONFIG 		      (0xAA290000)
#define MDP_DMA_P_OUT_XY                      (0xAA290010)
#define MDP_DMA_P_SIZE                        (0xAA290004)
#define MDP_DMA_P_BUF_ADDR                    (0xAA290008)
#define MDP_DMA_P_BUF_Y_STRIDE                (0xAA29000C)

#define MDP_DSI_VIDEO_EN                      (0xAA2F0000)
#define MDP_DSI_VIDEO_HSYNC_CTL               (0xAA2F0004)
#define MDP_DSI_VIDEO_VSYNC_PERIOD            (0xAA2F0008)
#define MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH       (0xAA2F000C)
#define MDP_DSI_VIDEO_DISPLAY_HCTL            (0xAA2F0010)
#define MDP_DSI_VIDEO_DISPLAY_V_START         (0xAA2F0014)
#define MDP_DSI_VIDEO_DISPLAY_V_END           (0xAA2F0018)
#define MDP_DSI_VIDEO_BORDER_CLR              (0xAA2F0028)
#define MDP_DSI_VIDEO_HSYNC_SKEW              (0xAA2F0030)
#define MDP_DSI_VIDEO_CTL_POLARITY            (0xAA2F0038)
#define MDP_DSI_VIDEO_TEST_CTL                (0xAA2F0034)

#define MDP_DMA_P_START                       REG_MDP(0x00044)
#define MDP_DMA_S_START                       REG_MDP(0x00048)
#define MDP_DISP_INTF_SEL                     REG_MDP(0x00038)
#define MDP_MAX_RD_PENDING_CMD_CONFIG         REG_MDP(0x0004C)
#define MDP_INTR_ENABLE                       REG_MDP(0x00020)
#define MDP_INTR_CLEAR			      REG_MDP(0x00028)
#define MDP_DSI_CMD_MODE_ID_MAP               REG_MDP(0xF1000)
#define MDP_DSI_CMD_MODE_TRIGGER_EN           REG_MDP(0XF1004)

#define MDP_TEST_MODE_CLK                     REG_MDP(0xF0000)
#define MDP_INTR_STATUS                       REG_MDP(0x00054)
#define MSM_CRYPTO_BASE                       (0xA0C00000)

#define MSM_GIC_DIST_BASE                     (0xC0000000)
#define MSM_GIC_CPU_BASE                      (0xC0002000)
#endif
