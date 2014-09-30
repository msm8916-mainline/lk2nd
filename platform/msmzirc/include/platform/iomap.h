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

#ifndef _PLATFORM_MSMZIRC_IOMAP_H_
#define _PLATFORM_MSMZIRC_IOMAP_H_

/* NAND */
#define MSM_NAND_BASE               0x079B0000
/* NAND BAM */
#define MSM_NAND_BAM_BASE           0x07984000

#define APPS_SS_BASE                0x0B000000

#define MSM_IOMAP_BASE              0x00000000
#define MSM_IOMAP_END               0x80000000

#define SYSTEM_IMEM_BASE            0x08600000
#define MSM_SHARED_IMEM_BASE        0x08600000

#define RESTART_REASON_ADDR         (MSM_SHARED_IMEM_BASE + 0x65C)
#define BS_INFO_OFFSET              (0x6B0)
#define BS_INFO_ADDR                (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)
#define SDRAM_START_ADDR            0x80000000

#define MSM_SHARED_BASE             0x87E80000

#define MSM_GIC_DIST_BASE           APPS_SS_BASE
#define MSM_GIC_CPU_BASE            (APPS_SS_BASE + 0x2000)
#define APPS_APCS_QTMR_AC_BASE      (APPS_SS_BASE + 0x00020000)
#define APPS_APCS_F0_QTMR_V1_BASE   (APPS_SS_BASE + 0x00021000)
#define QTMR_BASE                   APPS_APCS_F0_QTMR_V1_BASE

#define PERIPH_SS_BASE              0x07800000

#define MSM_SDC1_BASE               (PERIPH_SS_BASE + 0x00024000)
#define MSM_SDC1_SDHCI_BASE         (PERIPH_SS_BASE + 0x00024900)

/* SDHCI */
#define SDCC_MCI_HC_MODE            (0x00000078)
#define SDCC_HC_PWRCTL_STATUS_REG   (0x000000DC)
#define SDCC_HC_PWRCTL_MASK_REG     (0x000000E0)
#define SDCC_HC_PWRCTL_CLEAR_REG    (0x000000E4)
#define SDCC_HC_PWRCTL_CTL_REG      (0x000000E8)
#define BLSP1_UART0_BASE            (PERIPH_SS_BASE + 0x000AF000)
#define BLSP1_UART1_BASE            (PERIPH_SS_BASE + 0x000B0000)
#define BLSP1_UART2_BASE            (PERIPH_SS_BASE + 0x000B1000)
#define MSM_USB30_BASE              0x08A00000
#define MSM_USB30_QSCRATCH_BASE     0x08AF8800

#define CLK_CTL_BASE                0x1800000

#define SPMI_BASE                   0x02000000
#define SPMI_GENI_BASE              (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE               (SPMI_BASE +  0x01800000)
#define PMIC_ARB_CORE               0x200F000

#define TLMM_BASE_ADDR              0x1000000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + (x)*0x1000)
#define GPIO_IN_OUT_ADDR(x)         (TLMM_BASE_ADDR + 0x00000004 + (x)*0x1000)

#define MPM2_MPM_CTRL_BASE          0x004A0000
#define MPM2_MPM_PS_HOLD            0x004AB000
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL  0x004A3000

/* CRYPTO ENGINE */
#define  GCC_CRYPTO_BCR             (CLK_CTL_BASE + 0x16000)
#define  GCC_CRYPTO_CMD_RCGR        (CLK_CTL_BASE + 0x16004)
#define  GCC_0RYPTO_CFG_RCGR        (CLK_CTL_BASE + 0x16008)
#define  GCC_CRYPTO_CBCR            (CLK_CTL_BASE + 0x1601C)
#define  GCC_CRYPTO_AXI_CBCR        (CLK_CTL_BASE + 0x16020)
#define  GCC_CRYPTO_AHB_CBCR        (CLK_CTL_BASE + 0x16024)
/* GPLL */
#define GPLL0_STATUS                (CLK_CTL_BASE + 0x21000)
#define APCS_GPLL_ENA_VOTE          (CLK_CTL_BASE + 0x45000)
#define APCS_CLOCK_BRANCH_ENA_VOTE  (CLK_CTL_BASE + 0x45004)

/* SDCC */
#define SDC1_HDRV_PULL_CTL          (TLMM_BASE_ADDR + 0x10A000)
#define SDCC1_BCR                   (CLK_CTL_BASE + 0x42000)
#define SDCC1_APPS_CBCR             (CLK_CTL_BASE + 0x42018)
#define SDCC1_AHB_CBCR              (CLK_CTL_BASE + 0x4201C)
#define SDCC1_CMD_RCGR              (CLK_CTL_BASE + 0x42004)
#define SDCC1_CFG_RCGR              (CLK_CTL_BASE + 0x42008)
#define SDCC1_M                     (CLK_CTL_BASE + 0x4200C)
#define SDCC1_N                     (CLK_CTL_BASE + 0x42010)
#define SDCC1_D                     (CLK_CTL_BASE + 0x42014)

/* UART */
#define BLSP1_AHB_CBCR              (CLK_CTL_BASE + 0x1008)
#define BLSP1_UART1_APPS_CBCR       (CLK_CTL_BASE + 0x203C)
#define BLSP1_UART1_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x2044)
#define BLSP1_UART1_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x2048)
#define BLSP1_UART1_APPS_M          (CLK_CTL_BASE + 0x204C)
#define BLSP1_UART1_APPS_N          (CLK_CTL_BASE + 0x2050)
#define BLSP1_UART1_APPS_D          (CLK_CTL_BASE + 0x2054)

#define BLSP1_UART2_APPS_CBCR       (CLK_CTL_BASE + 0x302C)
#define BLSP1_UART2_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x3034)
#define BLSP1_UART2_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x3038)
#define BLSP1_UART2_APPS_M          (CLK_CTL_BASE + 0x303C)
#define BLSP1_UART2_APPS_N          (CLK_CTL_BASE + 0x3040)
#define BLSP1_UART2_APPS_D          (CLK_CTL_BASE + 0x3044)

#define BLSP1_UART3_APPS_CBCR       (CLK_CTL_BASE + 0x403C)
#define BLSP1_UART3_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x4044)
#define BLSP1_UART3_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x4048)
#define BLSP1_UART3_APPS_M          (CLK_CTL_BASE + 0x404C)
#define BLSP1_UART3_APPS_N          (CLK_CTL_BASE + 0x4050)
#define BLSP1_UART3_APPS_D          (CLK_CTL_BASE + 0x4054)


/* USB */
#define USB_HS_BCR                  (CLK_CTL_BASE + 0x41000)
#define USB_HS_SYSTEM_CBCR          (CLK_CTL_BASE + 0x41004)
#define USB_HS_AHB_CBCR             (CLK_CTL_BASE + 0x41008)
#define USB_HS_SYSTEM_CMD_RCGR      (CLK_CTL_BASE + 0x41010)
#define USB_HS_SYSTEM_CFG_RCGR      (CLK_CTL_BASE + 0x41014)
#define QUSB2A_PHY_BCR              (CLK_CTL_BASE + 0x41028)

/* USB 3.0 clock */
#define SYS_NOC_USB3_AXI_CBCR       (CLK_CTL_BASE + 0x5E084)
#define GCC_USB30_MASTER_CBCR       (CLK_CTL_BASE + 0x5E000)
#define GCC_USB30_GDSCR             (CLK_CTL_BASE + 0x5E078)
#define GCC_USB30_MASTER_CMD_RCGR   (CLK_CTL_BASE + 0x5E00C)
#define GCC_USB30_MASTER_CFG_RCGR   (CLK_CTL_BASE + 0x5E010)
#define GCC_USB30_MASTER_M          (CLK_CTL_BASE + 0x5E014)
#define GCC_USB30_MASTER_N          (CLK_CTL_BASE + 0x5E018)
#define GCC_USB30_MASTER_D          (CLK_CTL_BASE + 0x5E01C)

/* USB 3.0 base */
#define USB3_PIPE_CMD_RCGR          (CLK_CTL_BASE + 0x5E048)
#define USB3_PIPE_CFG_RCGR          (CLK_CTL_BASE + 0x5E04C)
#define USB_PHY_CFG_AHB_CBCR        (CLK_CTL_BASE + 0x5E080)
#define USB3_PIPE_CBCR              (CLK_CTL_BASE + 0x5E040)
#define USB3_PIPE_BCR               (CLK_CTL_BASE + 0x5E03C)

#define USB3_AUX_CMD_RCGR           (CLK_CTL_BASE + 0x5E05C)
#define USB3_AUX_CFG_RCGR           (CLK_CTL_BASE + 0x5E060)
#define USB3_AUX_M                  (CLK_CTL_BASE + 0x5E064)
#define USB3_AUX_N                  (CLK_CTL_BASE + 0x5E068)
#define USB3_AUX_D                  (CLK_CTL_BASE + 0x5E06C)
#define USB3_AUX_CBCR               (CLK_CTL_BASE + 0x5E044)

/* USB 3.0 phy */
#define USB3_PHY_BCR                (CLK_CTL_BASE + 0x0005E034)

/* QUSB2 PHY */
#define QUSB2_PHY_BASE              0x00079000
#define QUSB2PHY_PORT_POWERDOWN     (QUSB2_PHY_BASE + 0x000000B4)
#define GCC_QUSB2_PHY_BCR           (CLK_CTL_BASE + 0x00041028)
#define QUSB2PHY_PORT_UTMI_CTRL2    (QUSB2_PHY_BASE + 0x000000C4)
#define QUSB2PHY_PORT_TUNE1         (QUSB2_PHY_BASE + 0x00000080)
#define QUSB2PHY_PORT_TUNE2         (QUSB2_PHY_BASE + 0x00000084)
#define QUSB2PHY_PORT_TUNE3         (QUSB2_PHY_BASE + 0x00000088)
#define QUSB2PHY_PORT_TUNE4         (QUSB2_PHY_BASE + 0x0000008C)

/* SS QMP (Qulacomm Multi Protocol) */
#define QMP_PHY_BASE                0x78000

/* QMP register offset */
#define PLATFORM_QMP_OFFSET         0x8

/* Boot config */
#define SEC_CTRL_CORE_BASE          0x00058000
#define BOOT_CONFIG_OFFSET          0x0000602C
#define BOOT_CONFIG_REG             (SEC_CTRL_CORE_BASE + BOOT_CONFIG_OFFSET)

#endif
