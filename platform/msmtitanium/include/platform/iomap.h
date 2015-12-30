/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MSMTITANIUM_IOMAP_H_
#define _PLATFORM_MSMTITANIUM_IOMAP_H_

#define MSM_IOMAP_BASE                     0x00000000
#define MSM_IOMAP_END                      0x08000000

#define SDRAM_START_ADDR                   0x80000000

#define MSM_SHARED_BASE                    0x86300000
#define MSM_SHARED_IMEM_BASE               0x08600000

#define BS_INFO_OFFSET                     (0x6B0)
#define BS_INFO_ADDR                       (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)

#define RESTART_REASON_ADDR                (MSM_SHARED_IMEM_BASE + 0x65C)

#define APPS_SS_BASE                       0x0B000000
#define APPS_SS_END                        0x0B200000

#define MSM_GIC_DIST_BASE                  APPS_SS_BASE
#define MSM_GIC_CPU_BASE                   (APPS_SS_BASE + 0x2000)
#define APPS_APCS_QTMR_AC_BASE             (APPS_SS_BASE + 0x00020000)
#define APPS_APCS_F0_QTMR_V1_BASE          (APPS_SS_BASE + 0x00021000)
#define QTMR_BASE                          APPS_APCS_F0_QTMR_V1_BASE
#define APCS_ALIAS0_IPC_INTERRUPT          (APPS_SS_BASE + 0x00111008)

#define PERIPH_SS_BASE                     0x07800000

#define MSM_SDC1_BASE                      (PERIPH_SS_BASE + 0x00024000)
#define MSM_SDC2_BASE                      (PERIPH_SS_BASE + 0x00064000)

#define BLSP1_UART0_BASE                   (PERIPH_SS_BASE + 0x000AF000)
#define BLSP1_UART1_BASE                   (PERIPH_SS_BASE + 0x000B0000)
#define MSM_USB_BASE                       (PERIPH_SS_BASE + 0x000DB000)

#define CLK_CTL_BASE                       0x1800000

#define SPMI_BASE                          0x02000000
#define SPMI_GENI_BASE                     (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE                      (SPMI_BASE +  0x01800000)
#define PMIC_ARB_CORE                      0x200F000

#define TLMM_BASE_ADDR                     0x1000000
#define GPIO_CONFIG_ADDR(x)                (TLMM_BASE_ADDR + (x)*0x1000)
#define GPIO_IN_OUT_ADDR(x)                (TLMM_BASE_ADDR + 0x00000004 + (x)*0x1000)

#define MPM2_MPM_CTRL_BASE                 0x004A0000
#define MPM2_MPM_PS_HOLD                   0x004AB000
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL  0x004A3000

/* CRYPTO ENGINE */
#define  MSM_CE1_BASE                      0x073A000
#define  MSM_CE1_BAM_BASE                  0x0704000
#define  GCC_CRYPTO_BCR                    (CLK_CTL_BASE + 0x16000)
#define  GCC_CRYPTO_CMD_RCGR               (CLK_CTL_BASE + 0x16004)
#define  GCC_CRYPTO_CFG_RCGR               (CLK_CTL_BASE + 0x16008)
#define  GCC_CRYPTO_CBCR                   (CLK_CTL_BASE + 0x1601C)
#define  GCC_CRYPTO_AXI_CBCR               (CLK_CTL_BASE + 0x16020)
#define  GCC_CRYPTO_AHB_CBCR               (CLK_CTL_BASE + 0x16024)


/* GPLL */
#define GPLL0_MODE                         (CLK_CTL_BASE + 0x21000)
#define APCS_GPLL_ENA_VOTE                 (CLK_CTL_BASE + 0x45000)
#define APCS_CLOCK_BRANCH_ENA_VOTE         (CLK_CTL_BASE + 0x45004)
#define GPLL4_MODE                         (CLK_CTL_BASE + 0x24000)
#define GPLL4_STATUS                       (CLK_CTL_BASE + 0x24024)
#define GPLL6_STATUS                       (CLK_CTL_BASE + 0x37024)

/* SDCC */
#define SDC1_HDRV_PULL_CTL                 (TLMM_BASE_ADDR + 0x10A000)
#define SDCC1_BCR                          (CLK_CTL_BASE + 0x42000) /* block reset*/
#define SDCC1_APPS_CBCR                    (CLK_CTL_BASE + 0x42018) /* branch ontrol */
#define SDCC1_AHB_CBCR                     (CLK_CTL_BASE + 0x4201C)
#define SDCC1_CMD_RCGR                     (CLK_CTL_BASE + 0x42004) /* cmd */
#define SDCC1_CFG_RCGR                     (CLK_CTL_BASE + 0x42008) /* cfg */
#define SDCC1_M                            (CLK_CTL_BASE + 0x4200C) /* m */
#define SDCC1_N                            (CLK_CTL_BASE + 0x42010) /* n */
#define SDCC1_D                            (CLK_CTL_BASE + 0x42014) /* d */

/* SDHCI */
#define MSM_SDC1_SDHCI_BASE                (PERIPH_SS_BASE + 0x00024900)
#define MSM_SDC2_SDHCI_BASE                (PERIPH_SS_BASE + 0x00064900)

#define SDCC_MCI_HC_MODE                   (0x00000078)
#define SDCC_HC_PWRCTL_STATUS_REG          (0x000000DC)
#define SDCC_HC_PWRCTL_MASK_REG            (0x000000E0)
#define SDCC_HC_PWRCTL_CLEAR_REG           (0x000000E4)
#define SDCC_HC_PWRCTL_CTL_REG             (0x000000E8)

#define SDCC2_BCR                          (CLK_CTL_BASE + 0x43000) /* block reset */
#define SDCC2_APPS_CBCR                    (CLK_CTL_BASE + 0x43018) /* branch control */
#define SDCC2_AHB_CBCR                     (CLK_CTL_BASE + 0x4301C)
#define SDCC2_CMD_RCGR                     (CLK_CTL_BASE + 0x43004) /* cmd */
#define SDCC2_CFG_RCGR                     (CLK_CTL_BASE + 0x43008) /* cfg */
#define SDCC2_M                            (CLK_CTL_BASE + 0x4300C) /* m */
#define SDCC2_N                            (CLK_CTL_BASE + 0x43010) /* n */
#define SDCC2_D                            (CLK_CTL_BASE + 0x43014) /* d */

/* UART */
#define BLSP1_AHB_CBCR                     (CLK_CTL_BASE + 0x1008)
#define BLSP1_UART1_APPS_CBCR              (CLK_CTL_BASE + 0x203C)
#define BLSP1_UART1_APPS_CMD_RCGR          (CLK_CTL_BASE + 0x2044)
#define BLSP1_UART1_APPS_CFG_RCGR          (CLK_CTL_BASE + 0x2048)
#define BLSP1_UART1_APPS_M                 (CLK_CTL_BASE + 0x204C)
#define BLSP1_UART1_APPS_N                 (CLK_CTL_BASE + 0x2050)
#define BLSP1_UART1_APPS_D                 (CLK_CTL_BASE + 0x2054)
#define BLSP1_UART2_APPS_CBCR              (CLK_CTL_BASE + 0x302C)
#define BLSP1_UART2_APPS_CMD_RCGR          (CLK_CTL_BASE + 0x3034)
#define BLSP1_UART2_APPS_CFG_RCGR          (CLK_CTL_BASE + 0x3038)
#define BLSP1_UART2_APPS_M                 (CLK_CTL_BASE + 0x303C)
#define BLSP1_UART2_APPS_N                 (CLK_CTL_BASE + 0x3040)
#define BLSP1_UART2_APPS_D                 (CLK_CTL_BASE + 0x3044)

/* USB */
#define USB_HS_BCR                         (CLK_CTL_BASE + 0x41000)
#define USB_HS_SYSTEM_CBCR                 (CLK_CTL_BASE + 0x41004)
#define USB_HS_AHB_CBCR                    (CLK_CTL_BASE + 0x41008)
#define USB_HS_SYSTEM_CMD_RCGR             (CLK_CTL_BASE + 0x41010)
#define USB_HS_SYSTEM_CFG_RCGR             (CLK_CTL_BASE + 0x41014)
#define GCC_QUSB2_PHY_BCR                  (CLK_CTL_BASE + 0x4103C)
#define MSM_USB30_QSCRATCH_BASE            0x070F8800
#define MSM_USB30_BASE                     0x7000000
#define USB2_PHY_SEL                       0x01937000
#define QUSB2_PHY_BASE                     0X79000

/* SS QMP (Qulacomm Multi Protocol) */
#define QMP_PHY_BASE                0x78000

#define AHB2_PHY_BASE               0x0007e000
#define PERIPH_SS_AHB2PHY_TOP_CFG   (AHB2_PHY_BASE + 0x10)

 /* USB3 clocks */
#define USB_30_BCR                  (CLK_CTL_BASE + 0x3F070)
#define GCC_USB30_GDSCR             (CLK_CTL_BASE + 0x3F078)
#define USB30_MASTER_CBCR           (CLK_CTL_BASE + 0x3F000)
#define USB30_SLEEP_CBCR            (CLK_CTL_BASE + 0x3F004)
#define USB30_MOCK_UTMI_CBCR        (CLK_CTL_BASE + 0x3F008)
#define USB30_MASTER_CMD_RCGR       (CLK_CTL_BASE + 0x3F00C)
#define USB30_MASTER_CFG_RCGR       (CLK_CTL_BASE + 0x3F010)
#define USB30_MASTER_M              (CLK_CTL_BASE + 0x3F014)
#define USB30_MASTER_N              (CLK_CTL_BASE + 0x3F018)
#define USB30_MASTER_D              (CLK_CTL_BASE + 0x3F01C)
#define USB30_MOCK_UTMI_CMD_RCGR    (CLK_CTL_BASE + 0x3F020)
#define USB30_MOCK_UTMI_CFG_RCGR    (CLK_CTL_BASE + 0x3F024)
#define PC_NOC_USB3_AXI_CBCR        (CLK_CTL_BASE + 0x3F038)

#define USB3_AUX_CMD_RCGR           (CLK_CTL_BASE + 0x3F05C)
#define USB3_AUX_CFG_RCGR     	    (CLK_CTL_BASE + 0x3F060)
#define USB3_AUX_CBCR       	    (CLK_CTL_BASE + 0x3F044)
#define USB3_AUX_M                  (CLK_CTL_BASE + 0x3F064)
#define USB3_AUX_N                  (CLK_CTL_BASE + 0x3F068)
#define USB3_AUX_D                  (CLK_CTL_BASE + 0x3F06C)
#define USB3_PIPE_CBCR              (CLK_CTL_BASE + 0x3F040)
#define USB3_PHY_BCR                (CLK_CTL_BASE + 0x3F034)
#define USB3PHY_PHY_BCR        	    (CLK_CTL_BASE + 0x3F03C)
#define USB_PHY_CFG_AHB_CBCR        (CLK_CTL_BASE + 0x3F080)

/* QMP rev registers */
#define USB3_PHY_REVISION_ID0       (QMP_PHY_BASE + 0x988)
#define USB3_PHY_REVISION_ID1       (QMP_PHY_BASE + 0x98C)
#define USB3_PHY_REVISION_ID2       (QMP_PHY_BASE + 0x990)
#define USB3_PHY_REVISION_ID3       (QMP_PHY_BASE + 0x994)

/* Dummy macro needed for compilation only */
#define PLATFORM_QMP_OFFSET         0x0

#define USB3_PHY_STATUS             0x78974
/* Register for finding out if single ended or differential clock enablement */
#define TCSR_PHY_CLK_SCHEME_SEL     0x0193F044

/* RPMB send receive buffer needs to be mapped
 * as device memory, define the start address
 * and size in MB
 */
#define RPMB_SND_RCV_BUF            0x90000000
#define RPMB_SND_RCV_BUF_SZ         0x1

/* QSEECOM: Secure app region notification */
#define APP_REGION_ADDR 0x85E00000
#define APP_REGION_SIZE 0x500000

#define TCSR_TZ_WONCE               0x193D000
#define TCSR_BOOT_MISC_DETECT       0x193D100

#define DDR_START                          0x80000000
#define ABOOT_FORCE_KERNEL_ADDR            DDR_START + 0x8000
#define ABOOT_FORCE_KERNEL64_ADDR          DDR_START + 0x80000
#define ABOOT_FORCE_RAMDISK_ADDR           DDR_START + 0x2000000
#define ABOOT_FORCE_TAGS_ADDR              DDR_START + 0x1E00000
#endif
