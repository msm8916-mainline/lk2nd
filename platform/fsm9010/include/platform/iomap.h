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

#ifndef _PLATFORM_FSM9010_IOMAP_H_
#define _PLATFORM_FSM9010_IOMAP_H_

#define MSM_IOMAP_BASE              0xF9000000
#define MSM_IOMAP_END               0xFEFFFFFF

#define SDRAM_START_ADDR            0x00000000
#define SDRAM_SEC_BANK_START_ADDR   0x10000000

#define MSM_SHARED_BASE             0x0FA00000

#define RPM_MSG_RAM_BASE            0xFC42B000
#define SYSTEM_IMEM_BASE            0xFE800000
#define MSM_SHARED_IMEM_BASE        0xFE805000

#define RESTART_REASON_ADDR             (RPM_MSG_RAM_BASE     + 0x65C)
#define RESTART_REASON_ADDR_V2          (MSM_SHARED_IMEM_BASE + 0x65C)
#define DLOAD_MODE_ADDR_V2              (MSM_SHARED_IMEM_BASE + 0x0)
#define EMERGENCY_DLOAD_MODE_ADDR_V2    (MSM_SHARED_IMEM_BASE + 0xFE0)

#define APPS_SS_BASE                0xF9000000

#define MSM_GIC_DIST_BASE           APPS_SS_BASE
#define MSM_GIC_CPU_BASE            (APPS_SS_BASE + 0x2000)
#define APPS_APCS_QTMR_AC_BASE      (APPS_SS_BASE + 0x00020000)
#define APPS_APCS_F0_QTMR_V1_BASE   (APPS_SS_BASE + 0x00021000)
#define QTMR_BASE                   APPS_APCS_F0_QTMR_V1_BASE
#define PERIPH_SS_BASE              0xF9800000

#define MSM_SDC1_BAM_BASE           (PERIPH_SS_BASE + 0x00004000)
#define MSM_SDC1_BASE               (PERIPH_SS_BASE + 0x00024000)
#define MSM_SDC1_DML_BASE           (PERIPH_SS_BASE + 0x00024800)
#define MSM_SDC1_SDHCI_BASE         (PERIPH_SS_BASE + 0x00024900)

/* BLSP1_UART[0:5] */
#define BLSP1_UART0_BASE            (PERIPH_SS_BASE + 0x0011D000)
#define BLSP1_UART1_BASE            (PERIPH_SS_BASE + 0x0011E000)
#define BLSP1_UART2_BASE            (PERIPH_SS_BASE + 0x0011F000)
#define BLSP1_UART3_BASE            (PERIPH_SS_BASE + 0x00120000)

#define MSM_USB_BASE                (PERIPH_SS_BASE + 0x00200000)

#define CLK_CTL_BASE                0xFC400000

#define GCC_WDOG_DEBUG              (CLK_CTL_BASE +  0x00001780)

#define USB_HS_BCR                  (CLK_CTL_BASE + 0x480)

#define SPMI_BASE                   0xFC4C0000
#define SPMI_GENI_BASE              (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE               (SPMI_BASE + 0xB000)

#define MSM_CE2_BAM_BASE            0xFD444000
#define MSM_CE2_BASE                0xFD45A000

#define TLMM_BASE_ADDR              0xFD510000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x)         (TLMM_BASE_ADDR + 0x1004 + (x)*0x10)

#define MPM2_MPM_CTRL_BASE                   0xFC4A1000
#define MPM2_MPM_PS_HOLD                     0xFC4AB000
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL    0xFC4A3000

/* CE 1 */
#define  GCC_CE1_BCR                (CLK_CTL_BASE + 0x1040)
#define  GCC_CE1_CMD_RCGR           (CLK_CTL_BASE + 0x1050)
#define  GCC_CE1_CFG_RCGR           (CLK_CTL_BASE + 0x1054)
#define  GCC_CE1_CBCR               (CLK_CTL_BASE + 0x1044)
#define  GCC_CE1_AXI_CBCR           (CLK_CTL_BASE + 0x1048)
#define  GCC_CE1_AHB_CBCR           (CLK_CTL_BASE + 0x104C)

/* CE 2 */
#define  GCC_CE2_BCR                (CLK_CTL_BASE + 0x1080)
#define  GCC_CE2_CMD_RCGR           (CLK_CTL_BASE + 0x1090)
#define  GCC_CE2_CFG_RCGR           (CLK_CTL_BASE + 0x1094)
#define  GCC_CE2_CBCR               (CLK_CTL_BASE + 0x1084)
#define  GCC_CE2_AXI_CBCR           (CLK_CTL_BASE + 0x1088)
#define  GCC_CE2_AHB_CBCR           (CLK_CTL_BASE + 0x108C)

/* GPLL */
#define GPLL0_STATUS                (CLK_CTL_BASE + 0x001C)
#define APCS_GPLL_ENA_VOTE          (CLK_CTL_BASE + 0x1480)
#define APCS_CLOCK_BRANCH_ENA_VOTE  (CLK_CTL_BASE + 0x1484)

/* SDCC 1 */
#define SDCC1_BCR                   (CLK_CTL_BASE + 0x4C0) /* block reset */
#define SDCC1_APPS_CBCR             (CLK_CTL_BASE + 0x4C4) /* branch control */
#define SDCC1_AHB_CBCR              (CLK_CTL_BASE + 0x4C8)
#define SDCC1_INACTIVITY_TIMER_CBCR (CLK_CTL_BASE + 0x4CC)
#define SDCC1_CMD_RCGR              (CLK_CTL_BASE + 0x4D0) /* cmd */
#define SDCC1_CFG_RCGR              (CLK_CTL_BASE + 0x4D4) /* cfg */
#define SDCC1_M                     (CLK_CTL_BASE + 0x4D8) /* m */
#define SDCC1_N                     (CLK_CTL_BASE + 0x4DC) /* n */
#define SDCC1_D                     (CLK_CTL_BASE + 0x4E0) /* d */

/* UART
   BLSP1_UART[0:3]
*/
#define BLSP1_AHB_CBCR              (CLK_CTL_BASE + 0x5C4)

#define BLSP1_UART0_APPS_CBCR       (CLK_CTL_BASE + 0x684)
#define BLSP1_UART0_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x68C)
#define BLSP1_UART0_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x690)
#define BLSP1_UART0_APPS_M          (CLK_CTL_BASE + 0x694)
#define BLSP1_UART0_APPS_N          (CLK_CTL_BASE + 0x698)
#define BLSP1_UART0_APPS_D          (CLK_CTL_BASE + 0x69C)

#define BLSP1_UART1_APPS_CBCR       (CLK_CTL_BASE + 0x704)
#define BLSP1_UART1_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x70C)
#define BLSP1_UART1_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x710)
#define BLSP1_UART1_APPS_M          (CLK_CTL_BASE + 0x714)
#define BLSP1_UART1_APPS_N          (CLK_CTL_BASE + 0x718)
#define BLSP1_UART1_APPS_D          (CLK_CTL_BASE + 0x71C)

#define BLSP1_UART2_APPS_CBCR       (CLK_CTL_BASE + 0x784)
#define BLSP1_UART2_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x78C)
#define BLSP1_UART2_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x790)
#define BLSP1_UART2_APPS_M          (CLK_CTL_BASE + 0x794)
#define BLSP1_UART2_APPS_N          (CLK_CTL_BASE + 0x798)
#define BLSP1_UART2_APPS_D          (CLK_CTL_BASE + 0x79C)

#define BLSP1_UART3_APPS_CBCR       (CLK_CTL_BASE + 0x804)
#define BLSP1_UART3_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x80C)
#define BLSP1_UART3_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x810)
#define BLSP1_UART3_APPS_M          (CLK_CTL_BASE + 0x814)
#define BLSP1_UART3_APPS_N          (CLK_CTL_BASE + 0x818)
#define BLSP1_UART3_APPS_D          (CLK_CTL_BASE + 0x81C)

/* USB */
#define USB_HS_SYSTEM_CBCR          (CLK_CTL_BASE + 0x484)
#define USB_HS_AHB_CBCR             (CLK_CTL_BASE + 0x488)
#define USB_HS_SYSTEM_CMD_RCGR      (CLK_CTL_BASE + 0x490)
#define USB_HS_SYSTEM_CFG_RCGR      (CLK_CTL_BASE + 0x494)

/* I2C */
#define BLSP_QUP_BASE(blsp_id, qup_id)   ((blsp_id == 1) ? \
                                         (PERIPH_SS_BASE + 0x00123000 \
                                         + (qup_id * 0x1000)) :\
                                         (PERIPH_SS_BASE + 0x00163000 + \
                                         (qup_id * 0x1000)))

/* DRV strength for sdcc */
#define SDC1_HDRV_PULL_CTL           (TLMM_BASE_ADDR + 0x00002044)

/* SDHCI */
#define SDCC_MCI_HC_MODE            (0x00000078)
#define SDCC_HC_PWRCTL_STATUS_REG   (0x000000DC)
#define SDCC_HC_PWRCTL_MASK_REG     (0x000000E0)
#define SDCC_HC_PWRCTL_CLEAR_REG    (0x000000E4)
#define SDCC_HC_PWRCTL_CTL_REG      (0x000000E8)
#endif
