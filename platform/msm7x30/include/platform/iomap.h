/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#ifndef _PLATFORM_MSM7X30_IOMAP_H_
#define _PLATFORM_MSM7X30_IOMAP_H_

#define MSM_UART1_BASE 0xACA00000
#define MSM_UART2_BASE 0xACB00000
#define MSM_UART3_BASE 0xACC00000

#define MSM_VIC_BASE	0xC0080000
#define MSM_TMR_BASE 	0xC0100000

#define MSM_GPT_BASE      (MSM_TMR_BASE + 0x04)
#define MSM_DGT_BASE      (MSM_TMR_BASE + 0x24)
#define SPSS_TIMER_STATUS (MSM_TMR_BASE + 0x88)

#define GPT_REG(off)      (MSM_GPT_BASE + (off))
#define DGT_REG(off)      (MSM_DGT_BASE + (off))

#define GPT_MATCH_VAL      GPT_REG(0x0000)
#define GPT_COUNT_VAL      GPT_REG(0x0004)
#define GPT_ENABLE         GPT_REG(0x0008)
#define GPT_CLEAR          GPT_REG(0x000C)

#define DGT_MATCH_VAL      DGT_REG(0x0000)
#define DGT_COUNT_VAL      DGT_REG(0x0004)
#define DGT_ENABLE         DGT_REG(0x0008)
#define DGT_CLEAR          DGT_REG(0x000C)
#define DGT_CLK_CTL        DGT_REG(0x0010)

#define HW_REVISION_NUMBER   0xABC00270

#define MSM_CSR_BASE    0xC0100000
#define MSM_GCC_BASE 	0xC0182000

#define MSM_SDC1_BASE   0xA0400000
#define MSM_SDC2_BASE   0xA0500000
#define MSM_SDC3_BASE   0xA3000000
#define MSM_SDC4_BASE   0xA3100000

#define MSM_SHARED_BASE      0x00100000

#define MSM_CLK_CTL_BASE        0xAB800000
#define MSM_CLK_CTL_SH2_BASE    0xABA01000

#define REG_BASE(off)           (MSM_CLK_CTL_BASE + (off))
#define REG_SH2_BASE(off)       (MSM_CLK_CTL_SH2_BASE + (off))

#define SCSS_CLK_CTL            0xC0101004
#define SCSS_CLK_SEL            0xC0101008

#define MSM_USB_BASE			0xA3600000
#define MSM_CRYPTO_BASE			0xA8400000

#define SH2_USBH_MD_REG         REG_SH2_BASE(0x2BC)
#define SH2_USBH_NS_REG         REG_SH2_BASE(0x2C0)

#define SH2_MDP_NS_REG          REG_SH2_BASE(0x14C)
#define SH2_MDP_LCDC_MD_REG     REG_SH2_BASE(0x38C)
#define SH2_MDP_LCDC_NS_REG     REG_SH2_BASE(0x390)
#define SH2_MDP_VSYNC_REG       REG_SH2_BASE(0x460)
#define SH2_PMDH_NS_REG         REG_SH2_BASE(0x8C)

#define SH2_GLBL_CLK_ENA_SC     REG_SH2_BASE(0x3BC)
#define SH2_GLBL_CLK_ENA_2_SC   REG_SH2_BASE(0x3C0)

#define SH2_OWN_ROW1_BASE_REG   REG_BASE(0x041C)
#define SH2_OWN_ROW2_BASE_REG   REG_BASE(0x0424)
#define SH2_OWN_APPS2_BASE_REG  REG_BASE(0x0414)

#define MSM_ADM_BASE            0xAC200000
#define MSM_ADM_SD_OFFSET       0x00100400

#define MSM_SAW_BASE            0xC0102000

#define PLL_ENA_REG             REG_SH2_BASE(0x0264)
#define PLL2_STATUS_BASE_REG    REG_BASE(0x0350)
#define PLL2_L_VAL_ADDR         REG_BASE(0x033C)
#endif
