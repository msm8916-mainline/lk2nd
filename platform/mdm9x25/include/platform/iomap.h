/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MDM9625_IOMAP_H_
#define _PLATFORM_MDM9625_IOMAP_H_

/*SDRAM start address */
#define SDRAM_START_ADDR           0x0

/* TBD: shared base is not defined yet */
#define MSM_SHARED_BASE           0x00000000

#define ELAN_A5SS_BASE            0xF9000000

/* Peripheral subsystem */
#define PERIPH_SS_BASE            0xF9800000
#define PERIPH_SS_QPIC_BASE       0xF9AC4000

#define CLK_CTL_BASE              0xFC400000 /* GCC base */
#define TLMM_BASE_ADDR            0xFD500000

/* QGIC2 */
#define MSM_GIC_DIST_BASE         (ELAN_A5SS_BASE + 0x0000)
#define MSM_GIC_CPU_BASE          (ELAN_A5SS_BASE + 0x2000)

/* QTMR */
#define APCS_F0_QTMR_V1_BASE      (ELAN_A5SS_BASE + 0x21000)
#define QTMR_BASE                  APCS_F0_QTMR_V1_BASE

/* GPIO */
#define GPIO_CONFIG_ADDR(x)       (TLMM_BASE_ADDR + 0x10000 + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x)       (TLMM_BASE_ADDR + 0x10000 + 0x1004 + (x)*0x10)

/* USB */
#define MSM_USB_BASE              (PERIPH_SS_BASE + 0x00255000)

/* NAND */
#define MSM_NAND_BASE              0xF9AF0000
/* NAND BAM */
#define MSM_NAND_BAM_BASE          0xF9AC4000

/************ CLOCKS ***********/

/* GPLL */
#define GPLL0_STATUS                         (CLK_CTL_BASE + 0x001C)
#define APCS_GPLL_ENA_VOTE                   (CLK_CTL_BASE + 0x1480)

/* UART */
#define BLSP1_UART1_APPS_CBCR                (CLK_CTL_BASE + 0x684)
#define BLSP1_UART1_APPS_CMD_RCGR            (CLK_CTL_BASE + 0x68C)
#define BLSP1_UART1_APPS_CFG_RCGR            (CLK_CTL_BASE + 0x690)
#define BLSP1_UART1_APPS_M                   (CLK_CTL_BASE + 0x694)
#define BLSP1_UART1_APPS_N                   (CLK_CTL_BASE + 0x698)
#define BLSP1_UART1_APPS_D                   (CLK_CTL_BASE + 0x69C)

#define BLSP1_UART3_APPS_CBCR                (CLK_CTL_BASE + 0x784)
#define BLSP1_UART3_APPS_CMD_RCGR            (CLK_CTL_BASE + 0x78C)
#define BLSP1_UART3_APPS_CFG_RCGR            (CLK_CTL_BASE + 0x790)
#define BLSP1_UART3_APPS_M                   (CLK_CTL_BASE + 0x794)
#define BLSP1_UART3_APPS_N                   (CLK_CTL_BASE + 0x798)
#define BLSP1_UART3_APPS_D                   (CLK_CTL_BASE + 0x79C)

/* USB */
#define USB_HS_BCR                           (CLK_CTL_BASE + 0x480)
#define USB_HS_SYSTEM_CBCR                   (CLK_CTL_BASE + 0x484)
#define USB_HS_AHB_CBCR                      (CLK_CTL_BASE + 0x488)
#define GCC_USB_HS_INACTIVITY_TIMERS_CBCR    (CLK_CTL_BASE + 0x48C)
#define USB_HS_SYSTEM_CMD_RCGR               (CLK_CTL_BASE + 0x490)
#define USB_HS_SYSTEM_CFG_RCGR               (CLK_CTL_BASE + 0x494)

#define USB_BOOT_CLOCK_CTL                   (CLK_CTL_BASE + 0x1A00)

#define GCC_QPIC_BCR                         (CLK_CTL_BASE + 0x1A40)
#define GCC_QPIC_CBCR                        (CLK_CTL_BASE + 0x1A44)
#define GCC_QPIC_AHB_CBCR                    (CLK_CTL_BASE + 0x1A48)
#define GCC_QPIC_SYSTEM_CBCR                 (CLK_CTL_BASE + 0x1A4C)
#define GCC_QPIC_CMD_RCGR                    (CLK_CTL_BASE + 0x1A50)
#define GCC_QPIC_CFG_RCGR                    (CLK_CTL_BASE + 0x1A54)
#define GCC_QPIC_M                           (CLK_CTL_BASE + 0x1A58)
#define GCC_QPIC_N                           (CLK_CTL_BASE + 0x1A5C)
#define GCC_QPIC_D                           (CLK_CTL_BASE + 0x1A60)

#endif
