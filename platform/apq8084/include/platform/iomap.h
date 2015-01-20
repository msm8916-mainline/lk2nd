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

#ifndef _PLATFORM_APQ8084_IOMAP_H_
#define _PLATFORM_APQ8084_IOMAP_H_

#define MSM_SHARED_BASE             0x0FA00000

#define SYSTEM_IMEM_BASE            0xFE800000

#define MSM_IOMAP_BASE              0xF9000000
#define MSM_IOMAP_END               0xFEFFFFFF

#define MSM_SHARED_IMEM_BASE        0xFE805000

#define RESTART_REASON_ADDR         (MSM_SHARED_IMEM_BASE + 0x65C)

#define KPSS_BASE                   0xF9000000

#define MSM_GIC_DIST_BASE           KPSS_BASE
#define MSM_GIC_CPU_BASE            (KPSS_BASE + 0x2000)
#define APCS_KPSS_ACS_BASE          (KPSS_BASE + 0x00008000)
#define APCS_APC_KPSS_PLL_BASE      (KPSS_BASE + 0x0000A000)
#define APCS_KPSS_CFG_BASE          (KPSS_BASE + 0x00010000)
#define APCS_KPSS_WDT_BASE          (KPSS_BASE + 0x00017000)
#define KPSS_APCS_QTMR_AC_BASE      (KPSS_BASE + 0x00020000)
#define KPSS_APCS_F0_QTMR_V1_BASE   (KPSS_BASE + 0x00021000)
#define QTMR_BASE                   KPSS_APCS_F0_QTMR_V1_BASE

#define PERIPH_SS_BASE              0xF9800000

#define MSM_SDC1_BASE               (PERIPH_SS_BASE + 0x00024000)
#define MSM_SDC1_SDHCI_BASE         (PERIPH_SS_BASE + 0x00024900)
#define MSM_SDC3_BASE               (PERIPH_SS_BASE + 0x00064000)
#define MSM_SDC3_SDHCI_BASE         (PERIPH_SS_BASE + 0x00064900)
#define MSM_SDC2_BASE               (PERIPH_SS_BASE + 0x000A4000)
#define MSM_SDC2_SDHCI_BASE         (PERIPH_SS_BASE + 0x000A4900)
#define MSM_SDC4_BASE               (PERIPH_SS_BASE + 0x000E4000)
#define MSM_SDC4_SDHCI_BASE         (PERIPH_SS_BASE + 0x000E4900)

#define BLSP1_UART0_BASE            (PERIPH_SS_BASE + 0x0011D000)
#define BLSP1_UART1_BASE            (PERIPH_SS_BASE + 0x0011E000)
#define BLSP1_UART2_BASE            (PERIPH_SS_BASE + 0x0011F000)
#define BLSP1_UART3_BASE            (PERIPH_SS_BASE + 0x00120000)
#define BLSP1_UART4_BASE            (PERIPH_SS_BASE + 0x00121000)
#define BLSP1_UART5_BASE            (PERIPH_SS_BASE + 0x00122000)

#define BLSP2_UART1_BASE            (PERIPH_SS_BASE + 0x0015E000)

#define CLK_CTL_BASE                0xFC400000

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

/*GPLL4 */
#define GPLL4_STATUS                (CLK_CTL_BASE + 0x1DDC)

/* UART */
#define BLSP1_AHB_CBCR              (CLK_CTL_BASE + 0x5C4)
#define BLSP2_AHB_CBCR              (CLK_CTL_BASE + 0x944)
#define BLSP2_UART2_APPS_CBCR       (CLK_CTL_BASE + 0xA44)
#define BLSP2_UART2_APPS_CMD_RCGR   (CLK_CTL_BASE + 0xA4C)
#define BLSP2_UART2_APPS_CFG_RCGR   (CLK_CTL_BASE + 0xA50)
#define BLSP2_UART2_APPS_M          (CLK_CTL_BASE + 0xA54)
#define BLSP2_UART2_APPS_N          (CLK_CTL_BASE + 0xA58)
#define BLSP2_UART2_APPS_D          (CLK_CTL_BASE + 0xA5C)

/* USB */
#define USB_HS_BCR                  (CLK_CTL_BASE + 0x480)

#define USB_HS_SYSTEM_CBCR          (CLK_CTL_BASE + 0x484)
#define USB_HS_AHB_CBCR             (CLK_CTL_BASE + 0x488)
#define USB_HS_SYSTEM_CMD_RCGR      (CLK_CTL_BASE + 0x490)
#define USB_HS_SYSTEM_CFG_RCGR      (CLK_CTL_BASE + 0x494)

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

/* SDCC1 */
#define SDCC1_BCR                   (CLK_CTL_BASE + 0x4C0) /* block reset */
#define SDCC1_APPS_CBCR             (CLK_CTL_BASE + 0x4C4) /* branch control */
#define SDCC1_AHB_CBCR              (CLK_CTL_BASE + 0x4C8)
#define SDCC1_CMD_RCGR              (CLK_CTL_BASE + 0x4D0) /* cmd */
#define SDCC1_CFG_RCGR              (CLK_CTL_BASE + 0x4D4) /* cfg */
#define SDCC1_M                     (CLK_CTL_BASE + 0x4D8) /* m */
#define SDCC1_N                     (CLK_CTL_BASE + 0x4DC) /* n */
#define SDCC1_D                     (CLK_CTL_BASE + 0x4E0) /* d */
#define SDCC1_CDCCAL_SLEEP_CBCR     (CLK_CTL_BASE + 0x4E4)

/* USB 3.0 clocks */
#define SYS_NOC_USB3_AXI_CBCR       (CLK_CTL_BASE + 0x0108)

#define GCC_USB_30_BCR              (CLK_CTL_BASE + 0x03C0)
#define GCC_USB_30_MISC             (CLK_CTL_BASE + 0x03C4)

#define GCC_USB30_MASTER_CBCR       (CLK_CTL_BASE + 0x03C8)
#define GCC_USB30_SLEEP_CBCR        (CLK_CTL_BASE + 0x03CC)
#define GCC_USB30_MOCK_UTMI_CBCR    (CLK_CTL_BASE + 0x03D0)

#define GCC_USB30_MASTER_CMD_RCGR   (CLK_CTL_BASE + 0x03D4)
#define GCC_USB30_MASTER_CFG_RCGR   (CLK_CTL_BASE + 0x03D8)
#define GCC_USB30_MASTER_M          (CLK_CTL_BASE + 0x03DC)
#define GCC_USB30_MASTER_N          (CLK_CTL_BASE + 0x03E0)
#define GCC_USB30_MASTER_D          (CLK_CTL_BASE + 0x03E4)

#define GCC_USB3_PHY_BCR            (CLK_CTL_BASE + 0x03FC)
#define GCC_USB30_GDSCR             (CLK_CTL_BASE + 0x1E84)
#define GCC_USB30_PHY_COM_BCR       (CLK_CTL_BASE + 0x1E80)

/* USB30 base */
#define MSM_USB30_BASE               0xF9200000
#define MSM_USB30_QSCRATCH_BASE      0xF92F8800

/* SDCC clocks for CDC calibration*/
#define SDCC1_CDCCAL_SLEEP_CBCR     (CLK_CTL_BASE + 0x04E4)
#define SDCC1_CDCCAL_FF_CBCR        (CLK_CTL_BASE + 0x04E8)

/* Addresses below this point needs to be verified.
 * Included only for compilation purposes.
 */
#define MSM_USB_BASE                (PERIPH_SS_BASE + 0x00255000)

#define CLK_CTL_BASE                0xFC400000
#define GCC_WDOG_DEBUG              (CLK_CTL_BASE +  0x00001780)

#define USB_HS_BCR                  (CLK_CTL_BASE + 0x480)

#define UFS_BASE                    (0xFC590000 + 0x00004000)

#define SPMI_BASE                   0xFC4C0000
#define SPMI_GENI_BASE              (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE               (SPMI_BASE + 0xB000)

#define MSM_CE2_BAM_BASE            0xFD444000
#define MSM_CE2_BASE                0xFD45A000
#define USB2_PHY_SEL                0xFD4AB000
#define COPSS_USB_CONTROL_WITH_JDR  0xFD4AB204

#define TLMM_BASE_ADDR              0xFD510000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x)         (TLMM_BASE_ADDR + 0x1004 + (x)*0x10)

#define MPM2_MPM_CTRL_BASE                   0xFC4A1000
#define MPM2_MPM_PS_HOLD                     0xFC4AB000
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL    0xFC4A3000

#define BS_INFO_OFFSET                       (0x6B0)
#define BS_INFO_ADDR                         (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)

/* DRV strength for sdcc */
#define SDC1_HDRV_PULL_CTL           (TLMM_BASE_ADDR + 0x00002044)

/* SDHCI */
#define SDCC_MCI_HC_MODE            (0x00000078)
#define SDCC_HC_PWRCTL_STATUS_REG   (0x000000DC)
#define SDCC_HC_PWRCTL_MASK_REG     (0x000000E0)
#define SDCC_HC_PWRCTL_CLEAR_REG    (0x000000E4)
#define SDCC_HC_PWRCTL_CTL_REG      (0x000000E8)

/* Boot config */
#define SEC_CTRL_CORE_BASE          0xFC4B8000
#define BOOT_CONFIG_OFFSET          0x00006034
#define BOOT_CONFIG_REG             (SEC_CTRL_CORE_BASE+BOOT_CONFIG_OFFSET)

#define EDP_BASE                    (0xFD923400)

#define SOFT_RESET                  0x118
#define CLK_CTRL                    0x11C
#define TRIG_CTRL                   0x084
#define CTRL                        0x004
#define COMMAND_MODE_DMA_CTRL       0x03C
#define COMMAND_MODE_MDP_CTRL       0x040
#define COMMAND_MODE_MDP_DCS_CMD_CTRL   0x044
#define COMMAND_MODE_MDP_STREAM0_CTRL   0x058
#define COMMAND_MODE_MDP_STREAM0_TOTAL  0x05C
#define COMMAND_MODE_MDP_STREAM1_CTRL   0x060
#define COMMAND_MODE_MDP_STREAM1_TOTAL  0x064
#define ERR_INT_MASK0               0x10C

#define LANE_CTL                    0x0AC
#define LANE_SWAP_CTL               0x0B0
#define TIMING_CTL                  0x0C4

#define VIDEO_MODE_ACTIVE_H         0x024
#define VIDEO_MODE_ACTIVE_V         0x028
#define VIDEO_MODE_TOTAL            0x02C
#define VIDEO_MODE_HSYNC            0x030
#define VIDEO_MODE_VSYNC            0x034
#define VIDEO_MODE_VSYNC_VPOS       0x038

/* MDSS */
#define MSM_MMSS_CLK_CTL_BASE       0xFD8C0000
#define MIPI_DSI_BASE               (0xFD922800)
#define MIPI_DSI0_BASE              (MIPI_DSI_BASE)
#define MIPI_DSI1_BASE              (0xFD922E00)
#define DSI0_PHY_BASE               (0xFD922B00)
#define DSI1_PHY_BASE               (0xFD923100)
#define DSI0_PLL_BASE               (0xFD922A00)
#define DSI1_PLL_BASE               (0xFD923000)
#define REG_DSI(off)                (MIPI_DSI_BASE + 0x04 + (off))
#define MDP_BASE                    (0xfd900000)
#define REG_MDP(off)                (MDP_BASE + (off))
#define MDP_VP_0_VIG_0_BASE         REG_MDP(0x1200)
#define MDP_VP_0_VIG_1_BASE         REG_MDP(0x1600)
#define MDP_VP_0_RGB_0_BASE         REG_MDP(0x2200)
#define MDP_VP_0_RGB_1_BASE         REG_MDP(0x2600)
#define MDP_VP_0_DMA_0_BASE         REG_MDP(0x3200)
#define MDP_VP_0_DMA_1_BASE         REG_MDP(0x3600)
#define MDP_VP_0_MIXER_0_BASE       REG_MDP(0x3A00)
#define MDP_VP_0_MIXER_1_BASE       REG_MDP(0x3E00)

#ifdef MDP_PP_0_BASE
#undef MDP_PP_0_BASE
#endif
#define MDP_PP_0_BASE               REG_MDP(0x12F00)

#ifdef MDP_PP_1_BASE
#undef MDP_PP_1_BASE
#endif
#define MDP_PP_1_BASE               REG_MDP(0x13000)

#define DMA_CMD_OFFSET              0x048
#define DMA_CMD_LENGTH              0x04C

#define INT_CTRL                    0x110
#define CMD_MODE_DMA_SW_TRIGGER     0x090

#define EOT_PACKET_CTRL             0x0CC
#define MISR_CMD_CTRL               0x0A0
#define MISR_VIDEO_CTRL             0x0A4
#define VIDEO_MODE_CTRL             0x010
#define HS_TIMER_CTRL               0x0BC

/* HDMI reg addresses */
#define HDMI_BASE               0xFD922100
#define REG_HDMI(off)           (HDMI_BASE + (off))

#define HDMI_ACR_48_0           REG_HDMI(0xD4)
#define HDMI_ACR_48_1           REG_HDMI(0xD8)
#define HDMI_AUDIO_PKT_CTRL2    REG_HDMI(0x44)
#define HDMI_ACR_PKT_CTRL       REG_HDMI(0x24)
#define HDMI_INFOFRAME_CTRL0    REG_HDMI(0x2C)
#define HDMI_AUDIO_INFO0        REG_HDMI(0xE4)
#define HDMI_AUDIO_INFO1        REG_HDMI(0xE8)
#define HDMI_AUDIO_PKT_CTRL     REG_HDMI(0x20)
#define HDMI_VBI_PKT_CTRL       REG_HDMI(0x28)
#define HDMI_GEN_PKT_CTRL       REG_HDMI(0x34)
#define HDMI_GC                 REG_HDMI(0x40)
#define HDMI_AUDIO_CFG          REG_HDMI(0x1D0)

#define LPASS_LPAIF_RDDMA_CTL0       0xFE152000
#define LPASS_LPAIF_RDDMA_BASE0      0xFE152004
#define LPASS_LPAIF_RDDMA_BUFF_LEN0  0xFE152008
#define LPASS_LPAIF_RDDMA_PER_LEN0   0xFE152010
#define LPASS_LPAIF_DEBUG_CTL        0xFE15E004

#define HDMI_USEC_REFTIMER      REG_HDMI(0x208)
#define HDMI_CTRL               REG_HDMI(0x000)
#define HDMI_HPD_INT_STATUS     REG_HDMI(0x250)
#define HDMI_HPD_INT_CTRL       REG_HDMI(0x254)
#define HDMI_HPD_CTRL           REG_HDMI(0x258)
#define HDMI_PHY_CTRL           REG_HDMI(0x2D4)
#define HDMI_TOTAL              REG_HDMI(0x2C0)
#define HDMI_ACTIVE_H           REG_HDMI(0x2B4)
#define HDMI_ACTIVE_V           REG_HDMI(0x2B8)
#define HDMI_V_TOTAL_F2         REG_HDMI(0x2C4)
#define HDMI_ACTIVE_V_F2        REG_HDMI(0x2BC)
#define HDMI_FRAME_CTRL         REG_HDMI(0x2C8)

#define HDMI_AVI_INFO0          REG_HDMI(0x06C)
#define HDMI_AVI_INFO1          REG_HDMI(0x070)
#define HDMI_AVI_INFO2          REG_HDMI(0x074)
#define HDMI_AVI_INFO3          REG_HDMI(0x078)
#define HDMI_INFOFRAME_CTRL0    REG_HDMI(0x02C)
#endif
