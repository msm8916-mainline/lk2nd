/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2011, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MDM9615_IOMAP_H_
#define _PLATFORM_MDM9615_IOMAP_H_

#define GSBI_BASE(id)         ((id) <= 7 ? (0x16000000 + (((id)-1) << 20)) : \
                                           (0x1A000000 + (((id)-8) << 20)))
#define GSBI_UART_DM_BASE(id) (GSBI_BASE(id) + 0x40000)

#define TLMM_BASE_ADDR      0x00800000
#define GPIO_CONFIG_ADDR(x) (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x) (TLMM_BASE_ADDR + 0x1004 + (x)*0x10)
#define MSM_PSHOLD_CTL_SU   (TLMM_BASE_ADDR + 0x820)

#define MSM_SHARED_IMEM_BASE 0x2B000000
#define RESTART_REASON_ADDR  (MSM_SHARED_IMEM_BASE + 0x65C)

/* QGIC2 */
#define MSM_GIC_DIST_BASE   0x02000000
#define MSM_GIC_CPU_BASE    0x02002000

#define APCS_GLBSECURE_BASE 0x02010000
#define APCS_GLB_REG(x)     (APCS_GLBSECURE_BASE + (x))
#define APCS_GLB_QGIC_CFG   APCS_GLB_REG(0x38)

#define MSM_TMR_BASE   0x0200A000
#define TMR_REG(off)   (MSM_TMR_BASE + (off))

#define MSM_GPT_BASE   TMR_REG(0x4)
#define GPT_REG(off)   (MSM_GPT_BASE + (off))

#define GPT_MATCH_VAL     GPT_REG(0x0000)
#define GPT_COUNT_VAL     GPT_REG(0x0004)
#define GPT_ENABLE        GPT_REG(0x0008)
#define GPT_CLEAR         GPT_REG(0x000C)
#define DGT_MATCH_VAL     TMR_REG(0x0024)
#define DGT_COUNT_VAL     TMR_REG(0x0028)
#define DGT_ENABLE        TMR_REG(0x002C)
#define DGT_CLEAR         TMR_REG(0x0030)
/* TMR_STS - status of SCSS timers */
#define SPSS_TIMER_STATUS TMR_REG(0x0088)

#define MSM_USB_BASE      0x12500000

#define CLK_CTL_BASE      0x00900000
#define USB_HS1_HCLK_CTL        (CLK_CTL_BASE + 0x2900)
#define USB_HS1_XCVR_FS_CLK_MD  (CLK_CTL_BASE + 0x2908)
#define USB_HS1_XCVR_FS_CLK_NS  (CLK_CTL_BASE + 0x290C)
#define USB_HS1_SYSTEM_CLK_NS   (CLK_CTL_BASE + 0x36A4)
#define USB_HS1_SYSTEM_CLK_MD   (CLK_CTL_BASE + 0x36A0)
#define GSBIn_HCLK_CTL(n)       (CLK_CTL_BASE + 0x29C0 + (32 * ((n) - 1)))
#define GSBIn_UART_APPS_MD(n)   (CLK_CTL_BASE + 0x29D0 + (32 * ((n) - 1)))
#define GSBIn_UART_APPS_NS(n)   (CLK_CTL_BASE + 0x29D4 + (32 * ((n) - 1)))
#define MSM_BOOT_PLL8_STATUS    (CLK_CTL_BASE + 0x3158)
#define MSM_BOOT_PLL_ENABLE_SC0 (CLK_CTL_BASE + 0x34C0)

#define MSM_SHARED_BASE   0x40000000

#endif
