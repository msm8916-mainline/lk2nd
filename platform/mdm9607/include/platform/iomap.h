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

#ifndef _PLATFORM_MDM9607_IOMAP_H_
#define _PLATFORM_MDM9607_IOMAP_H_

#define MSM_IOMAP_BASE                     0x00000000
#define MSM_IOMAP_END                      0x08000000

#define A7_SS_BASE                 0x0B000000
#define A7_SS_END                  0x0B200000

#define SDRAM_START_ADDR                   0x80000000

#define SYSTEM_IMEM_BASE                   0x08600000
#define MSM_SHARED_BASE                    0x87D00000
#define MSM_SHARED_IMEM_BASE               0x08600000

#define BS_INFO_OFFSET                     (0x6B0)
#define BS_INFO_ADDR                       (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)

#define RESTART_REASON_ADDR                (MSM_SHARED_IMEM_BASE + 0x65C)

#define MSM_NAND_BASE                      0x79B0000
/* NAND BAM */
#define MSM_NAND_BAM_BASE           0x7984000

#define APPS_SS_BASE                       0x0B000000

#define MSM_GIC_DIST_BASE                  APPS_SS_BASE
#define MSM_GIC_CPU_BASE                   (APPS_SS_BASE + 0x2000)
#define APPS_APCS_QTMR_AC_BASE             (APPS_SS_BASE + 0x00020000)
#define APPS_APCS_F0_QTMR_V1_BASE          (APPS_SS_BASE + 0x00021000)
#define APCS_ALIAS0_IPC_INTERRUPT          (APPS_SS_BASE + 0x00011008)
#define QTMR_BASE                          APPS_APCS_F0_QTMR_V1_BASE

#define PERIPH_SS_BASE                     0x07800000


#define BLSP1_UART0_BASE                   (PERIPH_SS_BASE + 0x000AF000)
#define BLSP1_UART1_BASE                   (PERIPH_SS_BASE + 0x000B0000)
#define BLSP1_UART5_BASE                   (PERIPH_SS_BASE + 0x000B3000)
#define MSM_USB_BASE                       (PERIPH_SS_BASE + 0x000D9000)

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
#define  GCC_CRYPTO_BCR             (CLK_CTL_BASE + 0x16000)
#define  GCC_CRYPTO_CMD_RCGR        (CLK_CTL_BASE + 0x16004)
#define  GCC_CRYPTO_CFG_RCGR        (CLK_CTL_BASE + 0x16008)
#define  GCC_CRYPTO_CBCR            (CLK_CTL_BASE + 0x1601C)
#define  GCC_CRYPTO_AXI_CBCR        (CLK_CTL_BASE + 0x16020)
#define  GCC_CRYPTO_AHB_CBCR        (CLK_CTL_BASE + 0x16024)


/* GPLL */
#define GPLL0_MODE                         (CLK_CTL_BASE + 0x21000)
#define APCS_GPLL_ENA_VOTE                 (CLK_CTL_BASE + 0x45000)
#define APCS_CLOCK_BRANCH_ENA_VOTE         (CLK_CTL_BASE + 0x45004)

/* UART */
#define BLSP1_AHB_CBCR                     (CLK_CTL_BASE + 0x1008)
#define BLSP1_UART5_APPS_CBCR              (CLK_CTL_BASE + 0x603c)
#define BLSP1_UART5_APPS_CMD_RCGR          (CLK_CTL_BASE + 0x6044)
#define BLSP1_UART5_APPS_CFG_RCGR          (CLK_CTL_BASE + 0x6048)
#define BLSP1_UART5_APPS_M                 (CLK_CTL_BASE + 0x604C)
#define BLSP1_UART5_APPS_N                 (CLK_CTL_BASE + 0x6050)
#define BLSP1_UART5_APPS_D                 (CLK_CTL_BASE + 0x6054)

/* USB */
#define USB_HS_BCR                         (CLK_CTL_BASE + 0x41000)
#define USB_HS_SYSTEM_CBCR                 (CLK_CTL_BASE + 0x41004)
#define USB_HS_AHB_CBCR                    (CLK_CTL_BASE + 0x41008)
#define USB_HS_SYSTEM_CMD_RCGR             (CLK_CTL_BASE + 0x41010)
#define USB_HS_SYSTEM_CFG_RCGR             (CLK_CTL_BASE + 0x41014)

#define TCSR_TZ_WONCE               0x193D000
#define TCSR_BOOT_MISC_DETECT       0x193D100
#endif
