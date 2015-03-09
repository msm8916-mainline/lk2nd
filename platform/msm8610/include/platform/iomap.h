/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
 *
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
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

#ifndef _PLATFORM_MSM8610_IOMAP_H_
#define _PLATFORM_MSM8610_IOMAP_H_

#define MSM_IOMAP_BASE              0xF9000000
#define MSM_IOMAP_END               0xFEFFFFFF

#define SDRAM_START_ADDR            0x00000000

#define MSM_SHARED_BASE             0x0D900000

#define APPS_SS_BASE                0xF9000000

#define SYSTEM_IMEM_BASE            0xFE800000
#define MSM_SHARED_IMEM_BASE        0xFE805000
#define RESTART_REASON_ADDR         (MSM_SHARED_IMEM_BASE + 0x65C)
#define DLOAD_MODE_ADDR             (MSM_SHARED_IMEM_BASE + 0x0)
#define EMERGENCY_DLOAD_MODE_ADDR   (MSM_SHARED_IMEM_BASE + 0xFE0)

#define BS_INFO_OFFSET              (0x6B0)
#define BS_INFO_ADDR                (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL    0xFC4A3000

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
#define MSM_SDC2_BAM_BASE           (PERIPH_SS_BASE + 0x00084000)
#define MSM_SDC2_BASE               (PERIPH_SS_BASE + 0x000A4000)
#define MSM_SDC2_DML_BASE           (PERIPH_SS_BASE + 0x000A4800)
#define MSM_SDC2_SDHCI_BASE         (PERIPH_SS_BASE + 0x000A4900)

#define BLSP1_UART0_BASE            (PERIPH_SS_BASE + 0x0011D000)
#define BLSP1_UART1_BASE            (PERIPH_SS_BASE + 0x0011E000)
#define BLSP1_UART2_BASE            (PERIPH_SS_BASE + 0x0011F000)
#define BLSP1_UART3_BASE            (PERIPH_SS_BASE + 0x00120000)
#define BLSP1_UART4_BASE            (PERIPH_SS_BASE + 0x00121000)
#define BLSP1_UART5_BASE            (PERIPH_SS_BASE + 0x00122000)
#define MSM_USB_BASE                (PERIPH_SS_BASE + 0x00255000)

#define CLK_CTL_BASE                0xFC400000

#define GCC_WDOG_DEBUG              (CLK_CTL_BASE +  0x00001780)

#define USB_HS_BCR                  (CLK_CTL_BASE + 0x480)
#define USB_BOOT_CLOCK_CTL          (CLK_CTL_BASE + 0x1A00)

#define SPMI_BASE                   0xFC4C0000
#define SPMI_GENI_BASE              (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE               (SPMI_BASE + 0xB000)

#define MSM_CE1_BAM_BASE            0xFD404000
#define MSM_CE1_BASE                0xFD41A000

#define TLMM_BASE_ADDR              0xFD510000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x)         (TLMM_BASE_ADDR + 0x1004 + (x)*0x10)

#define MSM_MMSS_CLK_CTL_BASE       0xFD8C0000

/* DSI */
#define MIPI_DSI_BASE               0xFDD00000
#define MIPI_DSI0_BASE              MIPI_DSI_BASE
#define MIPI_DSI1_BASE              MIPI_DSI_BASE
#define DSI0_PHY_BASE               MIPI_DSI_BASE
#define DSI1_PHY_BASE               MIPI_DSI_BASE
#define DSI0_PLL_BASE               MIPI_DSI_BASE
#define DSI1_PLL_BASE               MIPI_DSI_BASE
#define REG_DSI(off)                (MIPI_DSI_BASE + (off))

#define DSIPHY_REGULATOR_BASE       0x500
#define DSIPHY_TIMING_BASE          0x440
#define DSIPHY_CTRL_BASE            0x470
#define DSIPHY_PLL_BASE             0x200
#define DSIPHY_STRENGTH_BASE        0x480
#define DSIPHY_CAL_SW_BASE          0x52C
#define DSIPHY_CAL_HW_BASE          0x538

/* Range 0 - 4 */
#define DSIPHY_REGULATOR_CTRL(x)    REG_DSI(DSIPHY_REGULATOR_BASE + (x) * 4)
/* Range 0 - 11 */
#define DSIPHY_TIMING_CTRL(x)       REG_DSI(DSIPHY_TIMING_BASE + (x) * 4)
/* Range 0 - 3 */
#define DSIPHY_CTRL(x)              REG_DSI(DSIPHY_CTRL_BASE + (x) * 4)
/* Range 0 - 2 */
#define DSIPHY_STRENGTH_CTRL(x)     REG_DSI(DSIPHY_STRENGTH_BASE + (x) * 4)
/* Range 0 - 19 */
#define DSIPHY_PLL_CTRL(x)          REG_DSI(DSIPHY_PLL_BASE + (x) * 4)
/* Range 0 - 2 */
#define DSIPHY_CAL_SW_CFG(x)        REG_DSI(DSIPHY_CAL_SW_BASE + (x) * 4)
/* Range 0 - 4 */
#define DSIPHY_CAL_HW_CFG(x)        REG_DSI(DSIPHY_CAL_HW_BASE + (x) * 4)

#define DSIPHY_REGULATOR_CAL_PWR_CFG REG_DSI(0x518)
#define DSIPHY_CAL_HW_TRIGGER       REG_DSI(0x528)
#define DSIPHY_SW_RESET             REG_DSI(0x128)
#define DSIPHY_LANE_SWAP            REG_DSI(0x0ac)
#define DSIPHY_PLL_READY            REG_DSI(0x280)

/* MDP */
#define MDP_BASE                    0xFD900000
#define REG_MDP(off)                (MDP_BASE + (off))

#define MDP_DMA_P_CONFIG            REG_MDP(0x90000)
#define MDP_DMA_P_OUT_XY            REG_MDP(0x90010)
#define MDP_DMA_P_SIZE              REG_MDP(0x90004)
#define MDP_DMA_P_BUF_ADDR          REG_MDP(0x90008)
#define MDP_DMA_P_BUF_Y_STRIDE      REG_MDP(0x9000C)

#define MDP_DSI_VIDEO_EN                 REG_MDP(0xF0000)
#define MDP_DSI_VIDEO_HSYNC_CTL          REG_MDP(0xF0004)
#define MDP_DSI_VIDEO_VSYNC_PERIOD       REG_MDP(0xF0008)
#define MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH  REG_MDP(0xF000C)
#define MDP_DSI_VIDEO_DISPLAY_HCTL       REG_MDP(0xF0010)
#define MDP_DSI_VIDEO_DISPLAY_V_START    REG_MDP(0xF0014)
#define MDP_DSI_VIDEO_DISPLAY_V_END      REG_MDP(0xF0018)
#define MDP_DSI_VIDEO_BORDER_CLR         REG_MDP(0xF0028)
#define MDP_DSI_VIDEO_HSYNC_SKEW         REG_MDP(0xF0030)
#define MDP_DSI_VIDEO_CTL_POLARITY       REG_MDP(0xF0038)
#define MDP_DSI_VIDEO_TEST_CTL           REG_MDP(0xF0034)

#define MDP_DMA_P_START                REG_MDP(0x00044)
#define MDP_DMA_S_START                REG_MDP(0x00048)
#define MDP_DISP_INTF_SEL              REG_MDP(0x00038)
#define MDP_MAX_RD_PENDING_CMD_CONFIG  REG_MDP(0x0004C)
#define MDP_INTR_ENABLE                REG_MDP(0x00020)
#define MDP_INTR_CLEAR                 REG_MDP(0x00028)
#define MDP_DSI_CMD_MODE_ID_MAP        REG_MDP(0xF1000)
#define MDP_DSI_CMD_MODE_TRIGGER_EN    REG_MDP(0XF1004)

#define MDP_TEST_MODE_CLK           REG_MDP(0xF0000)
#define MDP_INTR_STATUS             REG_MDP(0x00054)

#define SOFT_RESET                  0x114
#define CLK_CTRL                    0x118
#define TRIG_CTRL                   0x080
#define CTRL                        0x000
#define COMMAND_MODE_DMA_CTRL       0x038
#define COMMAND_MODE_MDP_CTRL       0x03C
#define COMMAND_MODE_MDP_DCS_CMD_CTRL   0x040
#define COMMAND_MODE_MDP_STREAM0_CTRL   0x054
#define COMMAND_MODE_MDP_STREAM0_TOTAL  0x058
#define COMMAND_MODE_MDP_STREAM1_CTRL   0x05C
#define COMMAND_MODE_MDP_STREAM1_TOTAL  0x060
#define ERR_INT_MASK0               0x108
#define RDBK_DATA0                  0x068

#define LANE_CTL                    0x0A8
#define LANE_SWAP_CTL               0x0AC
#define TIMING_CTL                  0x0C0

#define VIDEO_MODE_ACTIVE_H         0x020
#define VIDEO_MODE_ACTIVE_V         0x024
#define VIDEO_MODE_TOTAL            0x028
#define VIDEO_MODE_HSYNC            0x02C
#define VIDEO_MODE_VSYNC            0x030
#define VIDEO_MODE_VSYNC_VPOS       0x034

#define DMA_CMD_OFFSET              0x044
#define DMA_CMD_LENGTH              0x048

#define INT_CTRL                    0x10C
#define CMD_MODE_DMA_SW_TRIGGER     0x08C

#define EOT_PACKET_CTRL             0x0C8
#define MISR_CMD_CTRL               0x09C
#define MISR_VIDEO_CTRL             0x0A0
#define VIDEO_MODE_CTRL             0x00C
#define HS_TIMER_CTRL               0x0B8

#define MPM2_MPM_CTRL_BASE          0xFC4A1000
#define MPM2_MPM_PS_HOLD            0xFC4AB000

/* GPLL */
#define GPLL0_MODE                  CLK_CTL_BASE
#define GPLL0_STATUS                (CLK_CTL_BASE + 0x001C)
#define APCS_GPLL_ENA_VOTE          (CLK_CTL_BASE + 0x1480)
#define APCS_CLOCK_BRANCH_ENA_VOTE  (CLK_CTL_BASE + 0x1484)

/* SDCC */
#define SDCC1_BCR                   (CLK_CTL_BASE + 0x4C0) /* block reset */
#define SDCC1_APPS_CBCR             (CLK_CTL_BASE + 0x4C4) /* branch control */
#define SDCC1_AHB_CBCR              (CLK_CTL_BASE + 0x4C8)
#define SDCC1_INACTIVITY_TIMER_CBCR (CLK_CTL_BASE + 0x4CC)
#define SDCC1_CMD_RCGR              (CLK_CTL_BASE + 0x4D0) /* cmd */
#define SDCC1_CFG_RCGR              (CLK_CTL_BASE + 0x4D4) /* cfg */
#define SDCC1_M                     (CLK_CTL_BASE + 0x4D8) /* m */
#define SDCC1_N                     (CLK_CTL_BASE + 0x4DC) /* n */
#define SDCC1_D                     (CLK_CTL_BASE + 0x4E0) /* d */

/* SDCC2 */
#define SDCC2_BCR                   (CLK_CTL_BASE + 0x500) /* block reset */
#define SDCC2_APPS_CBCR             (CLK_CTL_BASE + 0x504) /* branch control */
#define SDCC2_AHB_CBCR              (CLK_CTL_BASE + 0x508)
#define SDCC2_INACTIVITY_TIMER_CBCR (CLK_CTL_BASE + 0x50C)
#define SDCC2_CMD_RCGR              (CLK_CTL_BASE + 0x510) /* cmd */
#define SDCC2_CFG_RCGR              (CLK_CTL_BASE + 0x514) /* cfg */
#define SDCC2_M                     (CLK_CTL_BASE + 0x518) /* m */
#define SDCC2_N                     (CLK_CTL_BASE + 0x51C) /* n */
#define SDCC2_D                     (CLK_CTL_BASE + 0x520) /* d */

/* UART */
#define BLSP1_AHB_CBCR              (CLK_CTL_BASE + 0x5C4)
#define BLSP1_UART2_APPS_CBCR       (CLK_CTL_BASE + 0x704)
#define BLSP1_UART2_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x70C)
#define BLSP1_UART2_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x710)
#define BLSP1_UART2_APPS_M          (CLK_CTL_BASE + 0x714)
#define BLSP1_UART2_APPS_N          (CLK_CTL_BASE + 0x718)
#define BLSP1_UART2_APPS_D          (CLK_CTL_BASE + 0x71C)

/* USB */
#define USB_HS_SYSTEM_CBCR          (CLK_CTL_BASE + 0x484)
#define USB_HS_AHB_CBCR             (CLK_CTL_BASE + 0x488)
#define USB_HS_SYSTEM_CMD_RCGR      (CLK_CTL_BASE + 0x490)
#define USB_HS_SYSTEM_CFG_RCGR      (CLK_CTL_BASE + 0x494)

/* DRV strength for sdcc */
#define SDC1_HDRV_PULL_CTL           (TLMM_BASE_ADDR + 0x00002044)

/* SDHCI */
#define SDCC_MCI_HC_MODE            (0x00000078)
#define SDCC_HC_PWRCTL_STATUS_REG   (0x000000DC)
#define SDCC_HC_PWRCTL_MASK_REG     (0x000000E0)
#define SDCC_HC_PWRCTL_CLEAR_REG    (0x000000E4)
#define SDCC_HC_PWRCTL_CTL_REG      (0x000000E8)

/* CE 1 */
#define  GCC_CE1_BCR                (CLK_CTL_BASE + 0x1040)
#define  GCC_CE1_CMD_RCGR           (CLK_CTL_BASE + 0x1050)
#define  GCC_CE1_CFG_RCGR           (CLK_CTL_BASE + 0x1054)
#define  GCC_CE1_CBCR               (CLK_CTL_BASE + 0x1044)
#define  GCC_CE1_AXI_CBCR           (CLK_CTL_BASE + 0x1048)
#define  GCC_CE1_AHB_CBCR           (CLK_CTL_BASE + 0x104C)

#endif
