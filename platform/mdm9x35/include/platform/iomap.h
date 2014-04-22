/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MDM9635_IOMAP_H_
#define _PLATFORM_MDM9635_IOMAP_H_

#define MSM_IOMAP_BASE            0xF9000000
#define MSM_IOMAP_END             0xFEFFFFFF

#define MSM_SHARED_BASE           0x01100000

/*SDRAM start address */
#define SDRAM_START_ADDR          0x00000000

#define MSM_SHARED_IMEM_BASE      0xFE805000
#define RESTART_REASON_ADDR       (MSM_SHARED_IMEM_BASE + 0x65C)

#define MSM_SHARED_IMEM_BASE_V2   0xFE807800
#define RESTART_REASON_ADDR_V2    (MSM_SHARED_IMEM_BASE_V2 + 0x65C)

#define A7SS_BASE                 0xF9000000

/* Peripheral subsystem */
#define PERIPH_SS_BASE            0xF9800000
#define PERIPH_SS_QPIC_BASE       0xF9AC4000

#define CLK_CTL_BASE              0xFC400000 /* GCC base */

/* MPM2_MPM */
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL    0xFC4A3000
#define MPM2_MPM_PS_HOLD                     0xFC4AB000

#define BS_INFO_OFFSET            (0x6B0)
#define BS_INFO_ADDR              (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)

#define SPMI_BASE                 0xFC4C0000
#define SPMI_GENI_BASE            (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE             (SPMI_BASE + 0xB000)
#define TLMM_BASE_ADDR            0xFD500000

/* QGIC2 */
#define MSM_GIC_DIST_BASE         (A7SS_BASE + 0x0000)
#define MSM_GIC_CPU_BASE          (A7SS_BASE + 0x2000)

/* QTMR */
#define APCS_F0_QTMR_V1_BASE      (A7SS_BASE + 0x21000)
#define QTMR_BASE                  APCS_F0_QTMR_V1_BASE

/* GPIO */
#define GPIO_CONFIG_ADDR(x)       (TLMM_BASE_ADDR + 0x10000 + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x)       (TLMM_BASE_ADDR + 0x10000 + 0x1004 + (x)*0x10)

/* USB */
#define MSM_USB_BASE              (PERIPH_SS_BASE + 0x00255000)
#define MSM_USB30_BASE            0xF9200000
#define MSM_USB30_QSCRATCH_BASE   0xF92F8800

/* SS QMP (Qulacomm Multi Protocol) */
#define QMP_PHY_BASE              0xF9B38000

/* UART */
#define MSM_UART2_BASE             0xF991F000

/* NAND */
#define MSM_NAND_BASE              0xF9AF0000
/* NAND BAM */
#define MSM_NAND_BAM_BASE          0xF9AC4000

/************ CLOCKS ***********/

/* GPLL */
#define GPLL0_STATUS                         (CLK_CTL_BASE + 0x0000)
#define APCS_GPLL_ENA_VOTE                   (CLK_CTL_BASE + 0x1480)

/* UART */

#define BLSP1_AHB_CBCR                       (CLK_CTL_BASE + 0x5C4)

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

#define BLSP1_UART2_APPS_CBCR                (CLK_CTL_BASE + 0x704)
#define BLSP1_UART2_APPS_CMD_RCGR            (CLK_CTL_BASE + 0x70C)
#define BLSP1_UART2_APPS_CFG_RCGR            (CLK_CTL_BASE + 0x710)
#define BLSP1_UART2_APPS_M                   (CLK_CTL_BASE + 0x714)
#define BLSP1_UART2_APPS_N                   (CLK_CTL_BASE + 0x718)
#define BLSP1_UART2_APPS_D                   (CLK_CTL_BASE + 0x71C)

#define APCS_CLOCK_BRANCH_ENA_VOTE           (CLK_CTL_BASE + 0x1484)

/* USB */
#define USB_HS_BCR                           (CLK_CTL_BASE + 0x480)
#define USB_HS_SYSTEM_CBCR                   (CLK_CTL_BASE + 0x484)
#define USB_HS_AHB_CBCR                      (CLK_CTL_BASE + 0x488)
#define GCC_USB_HS_INACTIVITY_TIMERS_CBCR    (CLK_CTL_BASE + 0x48C)
#define USB_HS_SYSTEM_CMD_RCGR               (CLK_CTL_BASE + 0x490)
#define USB_HS_SYSTEM_CFG_RCGR               (CLK_CTL_BASE + 0x494)

/* USB Phy reset */
#define USB3_PHY_BCR                         (CLK_CTL_BASE + 0x03FC)
#define USB3_PHY_COM_BCR                     (CLK_CTL_BASE + 0x1B88)
#define USB3PHY_PHY_BCR                      (CLK_CTL_BASE + 0x1B8C)
#define USB2B_PHY_BCR                        (CLK_CTL_BASE + 0x04B0)

/* USB 3.0 clocks */
#define USB2B_PHY_SLEEP_CBCR                 (CLK_CTL_BASE + 0x04B4)
#define SYS_NOC_USB3_AXI_CBCR                (CLK_CTL_BASE + 0x0108)
#define GCC_USB_30_BCR                       (CLK_CTL_BASE + 0x03C0)
#define GCC_USB_30_MISC                      (CLK_CTL_BASE + 0x03C4)
#define GCC_USB30_MASTER_CBCR                (CLK_CTL_BASE + 0x03C8)
#define GCC_USB30_SLEEP_CBCR                 (CLK_CTL_BASE + 0x03CC)
#define GCC_USB30_MOCK_UTMI_CBCR             (CLK_CTL_BASE + 0x03D0)
#define GCC_USB30_MASTER_CMD_RCGR            (CLK_CTL_BASE + 0x03D4)
#define GCC_USB30_MASTER_CFG_RCGR            (CLK_CTL_BASE + 0x03D8)
#define GCC_USB30_MASTER_M                   (CLK_CTL_BASE + 0x03DC)
#define GCC_USB30_MASTER_N                   (CLK_CTL_BASE + 0x03E0)
#define GCC_USB30_MASTER_D                   (CLK_CTL_BASE + 0x03E4)
#define GCC_USB30_GDSCR                      (CLK_CTL_BASE + 0x1B80)

/* USB30 base */
#define USB3_PIPE_CMD_RCGR                   (CLK_CTL_BASE + 0x1B98)
#define USB3_PIPE_CFG_RCGR                   (CLK_CTL_BASE + 0x1B9C)
#define USB3_PIPE_CBCR                       (CLK_CTL_BASE + 0x1B90)
#define USB3_AUX_CMD_RCGR                    (CLK_CTL_BASE + 0x1BC0)
#define USB3_AUX_CBCR                        (CLK_CTL_BASE + 0x1B94)
#define USB_PHY_CFG_AHB_CBCR                 (CLK_CTL_BASE + 0x1B84)
#define USB3_AUX_CFG_RCGR                    (CLK_CTL_BASE + 0x1BC4)
#define USB3_AUX_M                           (CLK_CTL_BASE + 0x1BC8)
#define USB3_AUX_N                           (CLK_CTL_BASE + 0x1BCC)
#define USB3_AUX_D                           (CLK_CTL_BASE + 0x1BD0)

/* USB3.0 Mux selector */
#define TCSR_PHSS_USB2_PHY_SEL               0xFD4AB000
#define PERIPH_SS_AHB2PHY_TOP_CFG            0xF9B3E010

#define PLATFORM_QMP_OFFSET                  0x0
#endif
