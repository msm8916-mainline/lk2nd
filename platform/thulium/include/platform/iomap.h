/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_THULIUM_IOMAP_H_
#define _PLATFORM_THULIUM_IOMAP_H_

#define MSM_SHARED_BASE             0x86000000

#define MSM_IOMAP_HMSS_START        0x09800000

#define MSM_IOMAP_BASE              0x00000000
#define MSM_IOMAP_END               0x10000000

#define MSM_SHARED_IMEM_BASE        0x066BF000
#define RESTART_REASON_ADDR         (MSM_SHARED_IMEM_BASE + 0x65C)
#define BS_INFO_ADDR                (MSM_SHARED_IMEM_BASE + 0x6B0)

#define MSM_GIC_DIST_BASE           (MSM_IOMAP_HMSS_START + 0x003C0000)
#define MSM_GIC_REDIST_BASE         (MSM_IOMAP_HMSS_START + 0x00400000)

#define HMSS_APCS_F0_QTMR_V1_BASE   (MSM_IOMAP_HMSS_START + 0x00050000)
#define QTMR_BASE                   HMSS_APCS_F0_QTMR_V1_BASE

#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS 0x00068000
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE      RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS
#define RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE 0x00006000

#define APCS_HLOS_IPC_INTERRUPT_0   0x9820010

#define PERIPH_SS_BASE              0x07400000

#define MSM_SDC1_BASE               (PERIPH_SS_BASE + 0x00064000)
#define MSM_SDC1_SDHCI_BASE         (PERIPH_SS_BASE + 0x00064900)
#define MSM_SDC2_BASE               (PERIPH_SS_BASE + 0x000A4000)
#define MSM_SDC2_SDHCI_BASE         (PERIPH_SS_BASE + 0x000A4900)

#define BLSP1_UART0_BASE            (PERIPH_SS_BASE + 0x0016F000)
#define BLSP1_UART1_BASE            (PERIPH_SS_BASE + 0x00170000)
#define BLSP1_UART2_BASE            (PERIPH_SS_BASE + 0x00171000)
#define BLSP1_UART3_BASE            (PERIPH_SS_BASE + 0x00172000)
#define BLSP1_UART4_BASE            (PERIPH_SS_BASE + 0x00173000)
#define BLSP1_UART5_BASE            (PERIPH_SS_BASE + 0x00174000)

#define BLSP2_UART1_BASE            (PERIPH_SS_BASE + 0x001B0000)

/* USB3.0 */
#define MSM_USB30_BASE              0x6A00000
#define MSM_USB30_QSCRATCH_BASE     0x6AF8800
/* SS QMP (Qulacomm Multi Protocol) */
#define QMP_PHY_BASE                0x7410000

/* QUSB2 PHY */
#define QUSB2_PHY_BASE              0x7411000
#define QUSB2PHY_PORT_POWERDOWN     (QUSB2_PHY_BASE + 0x000000B4)
#define GCC_QUSB2_PHY_BCR           (CLK_CTL_BASE + 0x00012038)
#define QUSB2PHY_PORT_UTMI_CTRL2    (QUSB2_PHY_BASE + 0x000000C4)
#define QUSB2PHY_PORT_TUNE1         (QUSB2_PHY_BASE + 0x00000080)
#define QUSB2PHY_PORT_TUNE2         (QUSB2_PHY_BASE + 0x00000084)
#define QUSB2PHY_PORT_TUNE3         (QUSB2_PHY_BASE + 0x00000088)
#define QUSB2PHY_PORT_TUNE4         (QUSB2_PHY_BASE + 0x0000008C)

/* Clocks */
#define CLK_CTL_BASE                0x300000

/* GPLL */
#define GPLL0_MODE                  (CLK_CTL_BASE + 0x0000)
#define GPLL4_MODE                  (CLK_CTL_BASE + 0x77000)
#define APCS_GPLL_ENA_VOTE          (CLK_CTL_BASE + 0x52000)
#define APCS_CLOCK_BRANCH_ENA_VOTE  (CLK_CTL_BASE + 0x52004)

/* UART Clocks */
#define BLSP2_AHB_CBCR              (CLK_CTL_BASE + 0x25004)
#define BLSP2_UART2_APPS_CBCR       (CLK_CTL_BASE + 0x29004)
#define BLSP2_UART2_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x2900C)
#define BLSP2_UART2_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x29010)
#define BLSP2_UART2_APPS_M          (CLK_CTL_BASE + 0x29014)
#define BLSP2_UART2_APPS_N          (CLK_CTL_BASE + 0x29018)
#define BLSP2_UART2_APPS_D          (CLK_CTL_BASE + 0x2901C)

/* USB3 clocks */
#define USB_30_BCR                  (CLK_CTL_BASE + 0xF000)
#define GCC_USB30_GDSCR             (CLK_CTL_BASE + 0xF004)
#define USB30_MASTER_CBCR           (CLK_CTL_BASE + 0xF008)
#define USB30_SLEEP_CBCR            (CLK_CTL_BASE + 0xF00C)
#define USB30_MOCK_UTMI_CBCR        (CLK_CTL_BASE + 0xF010)
#define USB30_MASTER_CMD_RCGR       (CLK_CTL_BASE + 0xF014)
#define USB30_MASTER_CFG_RCGR       (CLK_CTL_BASE + 0xF018)
#define USB30_MASTER_M              (CLK_CTL_BASE + 0xF01C)
#define USB30_MASTER_N              (CLK_CTL_BASE + 0xF020)
#define USB30_MASTER_D              (CLK_CTL_BASE + 0xF024)
#define USB30_MOCK_UTMI_CMD_RCGR    (CLK_CTL_BASE + 0xF028)
#define USB30_MOCK_UTMI_CFG_RCGR    (CLK_CTL_BASE + 0xF02C)
#define SYS_NOC_USB3_AXI_CBCR       (CLK_CTL_BASE + 0xF03C)

#define USB30_PHY_AUX_CMD_RCGR      (CLK_CTL_BASE + 0x5000C)
#define USB30_PHY_AUX_CFG_RCGR      (CLK_CTL_BASE + 0x50010)
#define USB30_PHY_AUX_CBCR          (CLK_CTL_BASE + 0x50000)
#define USB30_PHY_PIPE_CBCR         (CLK_CTL_BASE + 0x50004)
#define USB30_PHY_BCR               (CLK_CTL_BASE + 0x50020)
#define USB30PHY_PHY_BCR            (CLK_CTL_BASE + 0x50024)
#define USB_PHY_CFG_AHB2PHY_CBCR    (CLK_CTL_BASE + 0x6A004)
#define GCC_AGGRE2_USB3_AXI_CBCR    (CLK_CTL_BASE + 0x83018)

/* SDCC */
#define SDCC1_BCR                   (CLK_CTL_BASE + 0x13000) /* block reset */
#define SDCC1_APPS_CBCR             (CLK_CTL_BASE + 0x13004) /* branch control */
#define SDCC1_AHB_CBCR              (CLK_CTL_BASE + 0x13008)
#define SDCC1_CMD_RCGR              (CLK_CTL_BASE + 0x13010) /* cmd */
#define SDCC1_CFG_RCGR              (CLK_CTL_BASE + 0x13014) /* cfg */
#define SDCC1_M                     (CLK_CTL_BASE + 0x13018) /* m */
#define SDCC1_N                     (CLK_CTL_BASE + 0x1301C) /* n */
#define SDCC1_D                     (CLK_CTL_BASE + 0x13020) /* d */

/* SDCC2 */
#define SDCC2_BCR                   (CLK_CTL_BASE + 0x14000) /* block reset */
#define SDCC2_APPS_CBCR             (CLK_CTL_BASE + 0x14004) /* branch control */
#define SDCC2_AHB_CBCR              (CLK_CTL_BASE + 0x14008)
#define SDCC2_CMD_RCGR              (CLK_CTL_BASE + 0x14010) /* cmd */
#define SDCC2_CFG_RCGR              (CLK_CTL_BASE + 0x14014) /* cfg */
#define SDCC2_M                     (CLK_CTL_BASE + 0x14018) /* m */
#define SDCC2_N                     (CLK_CTL_BASE + 0x1401C) /* n */
#define SDCC2_D                     (CLK_CTL_BASE + 0x14020) /* d */

#define UFS_BASE                    0x624000

#define SPMI_BASE                   0x4000000
#define SPMI_GENI_BASE              (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE               (SPMI_BASE + 0x1800000)
#define PMIC_ARB_CORE               0x400F000

#define MSM_CE_BAM_BASE             0x644000
#define MSM_CE_BASE                 0x67A000
#define GCC_CE1_BCR                 (CLK_CTL_BASE + 0x00041000)

#define TLMM_BASE_ADDR              0x1010000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + (x)*0x1000)
#define GPIO_IN_OUT_ADDR(x)         (TLMM_BASE_ADDR + 0x4 + (x)*0x1000)

#define MPM2_MPM_CTRL_BASE                   0x4A1000
#define MPM2_MPM_PS_HOLD                     0x4AB000
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL    0x4A3000

/* DRV strength for sdcc */
#define SDC1_HDRV_PULL_CTL           (TLMM_BASE_ADDR + 0x0012C000)

/* SDHCI - power control registers */
#define SDCC_MCI_HC_MODE            (0x00000078)
#define SDCC_HC_PWRCTL_STATUS_REG   (0x000000DC)
#define SDCC_HC_PWRCTL_MASK_REG     (0x000000E0)
#define SDCC_HC_PWRCTL_CLEAR_REG    (0x000000E4)
#define SDCC_HC_PWRCTL_CTL_REG      (0x000000E8)

/* Boot config */
#define SEC_CTRL_CORE_BASE          0x70000
#define BOOT_CONFIG_OFFSET          0x00006044
#define BOOT_CONFIG_REG             (SEC_CTRL_CORE_BASE + BOOT_CONFIG_OFFSET)

/* QMP rev registers */
#define USB3_PHY_REVISION_ID0       (QMP_PHY_BASE + 0x788)
#define USB3_PHY_REVISION_ID1       (QMP_PHY_BASE + 0x78C)
#define USB3_PHY_REVISION_ID2       (QMP_PHY_BASE + 0x790)
#define USB3_PHY_REVISION_ID3       (QMP_PHY_BASE + 0x794)

/* Dummy macro needed for compilation only */
#define PLATFORM_QMP_OFFSET         0x0

#endif
