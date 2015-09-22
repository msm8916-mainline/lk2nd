/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MSM8994_IOMAP_H_
#define _PLATFORM_MSM8994_IOMAP_H_

#define MSM_SHARED_BASE             0x06A00000

#define MSM_IOMAP_BASE              0xF9000000
#define MSM_IOMAP_END               0xFEFFFFFF

#define SYSTEM_IMEM_BASE            0xFE800000
#define MSM_SHARED_IMEM_BASE        0xFE87F000
#define MSM_SHARED_IMEM_BASE2       0xFE80F000
#define RESTART_REASON_ADDR         (MSM_SHARED_IMEM_BASE + 0x65C)
#define RESTART_REASON_ADDR2        (MSM_SHARED_IMEM_BASE2 + 0x65C)

#define DLOAD_MODE_ADDR              (MSM_SHARED_IMEM_BASE + 0x0)
#define EMERGENCY_DLOAD_MODE_ADDR    (MSM_SHARED_IMEM_BASE + 0xFE0)
#define DLOAD_MODE_ADDR_V2           (MSM_SHARED_IMEM_BASE2 + 0x0)
#define EMERGENCY_DLOAD_MODE_ADDR_V2 (MSM_SHARED_IMEM_BASE2 + 0xFE0)


#define BS_INFO_OFFSET              (0x6B0)
#define BS_INFO_ADDR                (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)
#define BS_INFO_ADDR2               (MSM_SHARED_IMEM_BASE2 + BS_INFO_OFFSET)

#define KPSS_BASE                   0xF9000000

#define MSM_GIC_DIST_BASE           KPSS_BASE
#define MSM_GIC_CPU_BASE            (KPSS_BASE + 0x00002000)
#define APCS_KPSS_ACS_BASE          (KPSS_BASE + 0x00008000)
#define APCS_APC_KPSS_PLL_BASE      (KPSS_BASE + 0x0000A000)
#define APCS_KPSS_CFG_BASE          (KPSS_BASE + 0x00010000)
#define APCS_KPSS_WDT_BASE          (KPSS_BASE + 0x00017000)
#define KPSS_APCS_QTMR_AC_BASE      (KPSS_BASE + 0x00020000)
#define KPSS_APCS_F0_QTMR_V1_BASE   (KPSS_BASE + 0x00021000)
#define APCS_ALIAS0_IPC_INTERRUPT   (KPSS_BASE + 0x0000D008)
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

#define MSM_USB_BASE                (PERIPH_SS_BASE + 0x00255000)
#define USB2_PHY_SEL                0xFD4AB000

/* QUSB2 PHY */
#define QUSB2_PHY_BASE              (PERIPH_SS_BASE + 0x00339000)

#define MSM_USB30_BASE              0xF9200000
#define MSM_USB30_QSCRATCH_BASE     0xF92F8800

/* SS QMP (Qulacomm Multi Protocol) */
#define QMP_PHY_BASE                0xF9B38000
#define PERIPH_SS_AHB2PHY_TOP_CFG   0xF9B3E010

/* Clocks */
#define CLK_CTL_BASE                0xFC400000

/* GPLL */
#define GPLL0_MODE                  (CLK_CTL_BASE + 0x0000)
#define GPLL4_MODE                  (CLK_CTL_BASE + 0x1DC0)
#define APCS_GPLL_ENA_VOTE          (CLK_CTL_BASE + 0x1480)
#define APCS_CLOCK_BRANCH_ENA_VOTE  (CLK_CTL_BASE + 0x1484)

/* UART */
#define BLSP1_AHB_CBCR              (CLK_CTL_BASE + 0x5C4)
#define BLSP1_UART2_APPS_CBCR       (CLK_CTL_BASE + 0x704)
#define BLSP1_UART2_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x70C)
#define BLSP1_UART2_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x710)
#define BLSP1_UART2_APPS_M          (CLK_CTL_BASE + 0x714)
#define BLSP1_UART2_APPS_N          (CLK_CTL_BASE + 0x718)
#define BLSP1_UART2_APPS_D          (CLK_CTL_BASE + 0x71C)
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

/* USB3 clocks */
#define SYS_NOC_USB3_AXI_CBCR       (CLK_CTL_BASE + 0x03FC)
#define USB2B_PHY_SLEEP_CBCR        (CLK_CTL_BASE + 0x04AC)
#define USB2B_PHY_BCR               (CLK_CTL_BASE + 0x04A8)
#define USB30_MASTER_CMD_RCGR       (CLK_CTL_BASE + 0x03D4)
#define USB30_MASTER_CFG_RCGR       (CLK_CTL_BASE + 0x03D8)
#define USB30_MASTER_M              (CLK_CTL_BASE + 0x03DC)
#define USB30_MASTER_N              (CLK_CTL_BASE + 0x03E0)
#define USB30_MASTER_D              (CLK_CTL_BASE + 0x03E4)
#define USB30_MASTER_CBCR           (CLK_CTL_BASE + 0x03C8)
#define USB_30_BCR                  (CLK_CTL_BASE + 0x03C0)
#define USB30_MOCK_UTMI_CMD_RCGR    (CLK_CTL_BASE + 0x03E8)
#define USB30_MOCK_UTMI_CFG_RCGR    (CLK_CTL_BASE + 0x03EC)
#define USB30_MOCK_UTMI_CBCR        (CLK_CTL_BASE + 0x03D0)
#define USB30_SLEEP_CBCR            (CLK_CTL_BASE + 0x03CC)
#define USB30_PHY_AUX_CMD_RCGR      (CLK_CTL_BASE + 0x1414)
#define USB30_PHY_AUX_CFG_RCGR      (CLK_CTL_BASE + 0x1418)
#define USB30_PHY_AUX_CBCR          (CLK_CTL_BASE + 0x1408)
#define USB30_PHY_PIPE_CBCR         (CLK_CTL_BASE + 0x140C)
#define USB30_PHY_BCR               (CLK_CTL_BASE + 0x1400)
#define USB30PHY_PHY_BCR            (CLK_CTL_BASE + 0x1404)
#define GCC_USB30_GDSCR             (CLK_CTL_BASE + 0x03C4)
#define GCC_QUSB2_PHY_BCR           (CLK_CTL_BASE + 0x04B8)
#define USB_PHY_CFG_AHB2PHY_CBCR    (CLK_CTL_BASE + 0x1A84)

/* SDCC */
#define SDCC1_BCR                   (CLK_CTL_BASE + 0x4C0) /* block reset */
#define SDCC1_APPS_CBCR             (CLK_CTL_BASE + 0x4C4) /* branch control */
#define SDCC1_AHB_CBCR              (CLK_CTL_BASE + 0x4C8)
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

/* SDCC3 */
#define SDCC3_BCR                   (CLK_CTL_BASE + 0x540) /* block reset */
#define SDCC3_APPS_CBCR             (CLK_CTL_BASE + 0x544) /* branch control */
#define SDCC3_AHB_CBCR              (CLK_CTL_BASE + 0x548)
#define SDCC3_INACTIVITY_TIMER_CBCR (CLK_CTL_BASE + 0x54C)
#define SDCC3_CMD_RCGR              (CLK_CTL_BASE + 0x550) /* cmd */
#define SDCC3_CFG_RCGR              (CLK_CTL_BASE + 0x554) /* cfg */
#define SDCC3_M                     (CLK_CTL_BASE + 0x558) /* m */
#define SDCC3_N                     (CLK_CTL_BASE + 0x55C) /* n */
#define SDCC3_D                     (CLK_CTL_BASE + 0x560) /* d */


#define GCC_WDOG_DEBUG              (CLK_CTL_BASE +  0x00001780)

#define UFS_BASE                    (0xFC590000 + 0x00004000)

#define SPMI_BASE                   0xFC4C0000
#define SPMI_GENI_BASE              (SPMI_BASE + 0xA000)
#define SPMI_PIC_BASE               (SPMI_BASE + 0xB000)

#define MSM_CE2_BAM_BASE            0xFD444000
#define MSM_CE2_BASE                0xFD45A000
#define  GCC_CE2_BCR                (CLK_CTL_BASE + 0x1080)

#define TLMM_BASE_ADDR              0xFD510000
#define GPIO_CONFIG_ADDR(x)         (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x)         (TLMM_BASE_ADDR + 0x1004 + (x)*0x10)

#define MPM2_MPM_CTRL_BASE                   0xFC4A1000
#define MPM2_MPM_PS_HOLD                     0xFC4AB000
#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL    0xFC4A3000

/* DRV strength for sdcc */
#define SDC1_HDRV_PULL_CTL           (TLMM_BASE_ADDR + 0x00002044)
#define SDC2_HDRV_PULL_CTL           (TLMM_BASE_ADDR + 0x00002048)

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

#define MPM2_MPM_SLEEP_TIMETICK_COUNT_VAL    0xFC4A3000

#define TCSR_PHSS_USB2_PHY_SEL               0xFD4AB000
#define PLATFORM_QMP_OFFSET                  0x8

#define SMEM_TARG_INFO_ADDR                  0xFE805FF0


/* RPMB send receive buffer needs to be mapped
 * as device memory, define the start address
 * and size in MB
 */
#define RPMB_SND_RCV_BUF            0x10000000
#define RPMB_SND_RCV_BUF_SZ         0x1

/* QSEECOM: Secure app region notification */
#define APP_REGION_ADDR 0x6500000
#define APP_REGION_SIZE 0x500000

/* MDSS */
#define MSM_MMSS_CLK_CTL_BASE       0xFD8C0000
#define MMSS_MISC_AHB_CBCR          (MSM_MMSS_CLK_CTL_BASE + 0x502C)
#define MIPI_DSI_BASE               (0xFD998000)
#define MIPI_DSI0_BASE              (MIPI_DSI_BASE)
#define MIPI_DSI1_BASE              (0xFD9A0000)
#define DSI0_PHY_BASE               (0xFD998500)
#define DSI1_PHY_BASE               (0xFD9A0500)
#define DSI0_PLL_BASE               (0xFD998300)
#define DSI1_PLL_BASE               (0xFD9A0300)
#define DSI0_REGULATOR_BASE         (0xFD998780)
#define DSI1_REGULATOR_BASE         (0xFD9A0780)

#define MMSS_DSI_PHY_PLL_CORE_VCO_TUNE  0x0160
#define MMSS_DSI_PHY_PLL_CORE_KVCO_CODE 0x0168

#define MDP_BASE                    (0xfd900000)


#ifdef MDP_PP_0_BASE
#undef MDP_PP_0_BASE
#endif
#define MDP_PP_0_BASE               REG_MDP(0x71000)

#ifdef MDP_PP_1_BASE
#undef MDP_PP_1_BASE
#endif
#define MDP_PP_1_BASE               REG_MDP(0x71800)

#define REG_MDP(off)                (MDP_BASE + (off))

#ifdef MDP_HW_REV
#undef MDP_HW_REV
#endif
#define MDP_HW_REV                              REG_MDP(0x1000)

#ifdef MDP_INTR_EN
#undef MDP_INTR_EN
#endif
#define MDP_INTR_EN                             REG_MDP(0x1010)

#ifdef MDP_INTR_CLEAR
#undef MDP_INTR_CLEAR
#endif
#define MDP_INTR_CLEAR                          REG_MDP(0x1018)

#ifdef MDP_HIST_INTR_EN
#undef MDP_HIST_INTR_EN
#endif
#define MDP_HIST_INTR_EN                        REG_MDP(0x101C)

#ifdef MDP_DISP_INTF_SEL
#undef MDP_DISP_INTF_SEL
#endif
#define MDP_DISP_INTF_SEL                       REG_MDP(0x1004)

#ifdef MDP_VIDEO_INTF_UNDERFLOW_CTL
#undef MDP_VIDEO_INTF_UNDERFLOW_CTL
#endif
#define MDP_VIDEO_INTF_UNDERFLOW_CTL            REG_MDP(0x12E0)

#ifdef MDP_UPPER_NEW_ROI_PRIOR_RO_START
#undef MDP_UPPER_NEW_ROI_PRIOR_RO_START
#endif
#define MDP_UPPER_NEW_ROI_PRIOR_RO_START        REG_MDP(0x11EC)

#ifdef MDP_LOWER_NEW_ROI_PRIOR_TO_START
#undef MDP_LOWER_NEW_ROI_PRIOR_TO_START
#endif
#define MDP_LOWER_NEW_ROI_PRIOR_TO_START        REG_MDP(0x13F8)

#ifdef MDP_INTF_0_TIMING_ENGINE_EN
#undef MDP_INTF_0_TIMING_ENGINE_EN
#endif
#define MDP_INTF_0_TIMING_ENGINE_EN             REG_MDP(0x6b000)

#ifdef MDP_INTF_1_TIMING_ENGINE_EN
#undef MDP_INTF_1_TIMING_ENGINE_EN
#endif
#define MDP_INTF_1_TIMING_ENGINE_EN             REG_MDP(0x6b800)

#ifdef MDP_INTF_2_TIMING_ENGINE_EN
#undef MDP_INTF_2_TIMING_ENGINE_EN
#endif
#define MDP_INTF_2_TIMING_ENGINE_EN             REG_MDP(0x6C000)

#ifdef MDP_INTF_3_TIMING_ENGINE_EN
#undef MDP_INTF_3_TIMING_ENGINE_EN
#endif
#define MDP_INTF_3_TIMING_ENGINE_EN             REG_MDP(0x6C800)

#ifdef MDP_CTL_0_BASE
#undef MDP_CTL_0_BASE
#endif
#define MDP_CTL_0_BASE				REG_MDP(0x2000)

#ifdef MDP_CTL_1_BASE
#undef MDP_CTL_1_BASE
#endif
#define MDP_CTL_1_BASE				REG_MDP(0x2200)

#ifdef MDP_REG_SPLIT_DISPLAY_EN
#undef MDP_REG_SPLIT_DISPLAY_EN
#endif
#define MDP_REG_SPLIT_DISPLAY_EN                REG_MDP(0x12F4)

#ifdef MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL
#undef MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL
#endif
#define MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL    REG_MDP(0x12F8)

#ifdef MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL
#undef MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL
#endif
#define MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL    REG_MDP(0x13F0)

#ifdef MDP_INTF_0_BASE
#undef MDP_INTF_0_BASE
#endif
#define MDP_INTF_0_BASE                         REG_MDP(0x6b000)

#ifdef MDP_INTF_1_BASE
#undef MDP_INTF_1_BASE
#endif
#define MDP_INTF_1_BASE                         REG_MDP(0x6b800)

#ifdef MDP_INTF_2_BASE
#undef MDP_INTF_2_BASE
#endif
#define MDP_INTF_2_BASE                         REG_MDP(0x6c000)

#ifdef MDP_INTF_3_BASE
#undef MDP_INTF_3_BASE
#endif
#define MDP_INTF_3_BASE                         REG_MDP(0x6c800)

#ifdef MDP_CLK_CTRL0
#undef MDP_CLK_CTRL0
#endif
#define MDP_CLK_CTRL0                           REG_MDP(0x12AC)

#ifdef MDP_CLK_CTRL1
#undef MDP_CLK_CTRL1
#endif
#define MDP_CLK_CTRL1                           REG_MDP(0x12B4)

#ifdef MDP_CLK_CTRL2
#undef MDP_CLK_CTRL2
#endif
#define MDP_CLK_CTRL2                           REG_MDP(0x12BC)

#ifdef MDP_CLK_CTRL3
#undef MDP_CLK_CTRL3
#endif
#define MDP_CLK_CTRL3                           REG_MDP(0x13A8)

#ifdef MDP_CLK_CTRL4
#undef MDP_CLK_CTRL4
#endif
#define MDP_CLK_CTRL4                           REG_MDP(0x13B0)

#ifdef MDP_CLK_CTRL5
#undef MDP_CLK_CTRL5
#endif
#define MDP_CLK_CTRL5                           REG_MDP(0x13B8)

#ifdef MDP_CLK_CTRL6
#undef MDP_CLK_CTRL6
#endif
#define MDP_CLK_CTRL6                           REG_MDP(0x12C4)

#ifdef MDP_CLK_CTRL7
#undef MDP_CLK_CTRL7
#endif
#define MDP_CLK_CTRL7                           REG_MDP(0x13D0)

#ifdef MMSS_MDP_SMP_ALLOC_W_BASE
#undef MMSS_MDP_SMP_ALLOC_W_BASE
#endif
#define MMSS_MDP_SMP_ALLOC_W_BASE               REG_MDP(0x1080)

#ifdef MMSS_MDP_SMP_ALLOC_R_BASE
#undef MMSS_MDP_SMP_ALLOC_R_BASE
#endif
#define MMSS_MDP_SMP_ALLOC_R_BASE               REG_MDP(0x1130)

#ifdef MDP_QOS_REMAPPER_CLASS_0
#undef MDP_QOS_REMAPPER_CLASS_0
#endif
#define MDP_QOS_REMAPPER_CLASS_0                REG_MDP(0x11E0)

#ifdef MDP_QOS_REMAPPER_CLASS_1
#undef MDP_QOS_REMAPPER_CLASS_1
#endif
#define MDP_QOS_REMAPPER_CLASS_1                REG_MDP(0x11E4)

#ifdef VBIF_VBIF_DDR_FORCE_CLK_ON
#undef VBIF_VBIF_DDR_FORCE_CLK_ON
#endif
#define VBIF_VBIF_DDR_FORCE_CLK_ON              REG_MDP(0xc8004)

#ifdef VBIF_VBIF_DDR_OUT_MAX_BURST
#undef VBIF_VBIF_DDR_OUT_MAX_BURST
#endif
#define VBIF_VBIF_DDR_OUT_MAX_BURST             REG_MDP(0xc80D8)

#ifdef VBIF_VBIF_DDR_ARB_CTRL
#undef VBIF_VBIF_DDR_ARB_CTRL
#endif
#define VBIF_VBIF_DDR_ARB_CTRL                  REG_MDP(0xc80F0)

#ifdef VBIF_VBIF_DDR_RND_RBN_QOS_ARB
#undef VBIF_VBIF_DDR_RND_RBN_QOS_ARB
#endif
#define VBIF_VBIF_DDR_RND_RBN_QOS_ARB           REG_MDP(0xc8124)

#ifdef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0
#undef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0
#endif
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0        REG_MDP(0xc8160)

#ifdef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1
#undef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1
#endif
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1        REG_MDP(0xc8164)

#ifdef VBIF_VBIF_DDR_OUT_AOOO_AXI_EN
#undef VBIF_VBIF_DDR_OUT_AOOO_AXI_EN
#endif
#define VBIF_VBIF_DDR_OUT_AOOO_AXI_EN           REG_MDP(0xc8178)

#ifdef VBIF_VBIF_DDR_OUT_AX_AOOO
#undef VBIF_VBIF_DDR_OUT_AX_AOOO
#endif
#define VBIF_VBIF_DDR_OUT_AX_AOOO               REG_MDP(0xc817C)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF0
#undef VBIF_VBIF_IN_RD_LIM_CONF0
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF0               REG_MDP(0xc80B0)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF1
#undef VBIF_VBIF_IN_RD_LIM_CONF1
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF1               REG_MDP(0xc80B4)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF2
#undef VBIF_VBIF_IN_RD_LIM_CONF2
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF2               REG_MDP(0xc80B8)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF3
#undef VBIF_VBIF_IN_RD_LIM_CONF3
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF3               REG_MDP(0xc80BC)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF0
#undef VBIF_VBIF_IN_WR_LIM_CONF0
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF0               REG_MDP(0xc80C0)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF1
#undef VBIF_VBIF_IN_WR_LIM_CONF1
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF1               REG_MDP(0xc80C4)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF2
#undef VBIF_VBIF_IN_WR_LIM_CONF2
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF2               REG_MDP(0xc80C8)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF3
#undef VBIF_VBIF_IN_WR_LIM_CONF3
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF3               REG_MDP(0xc80CC)

#ifdef VBIF_VBIF_ABIT_SHORT
#undef VBIF_VBIF_ABIT_SHORT
#endif
#define VBIF_VBIF_ABIT_SHORT                    REG_MDP(0xc8070)

#ifdef VBIF_VBIF_ABIT_SHORT_CONF
#undef VBIF_VBIF_ABIT_SHORT_CONF
#endif
#define VBIF_VBIF_ABIT_SHORT_CONF               REG_MDP(0xc8074)

#ifdef VBIF_VBIF_GATE_OFF_WRREQ_EN
#undef VBIF_VBIF_GATE_OFF_WRREQ_EN
#endif
#define VBIF_VBIF_GATE_OFF_WRREQ_EN             REG_MDP(0xc80A8)

#define MDP_VP_0_VIG_0_BASE                     REG_MDP(0x5000)
#define MDP_VP_0_VIG_1_BASE                     REG_MDP(0x7000)
#define MDP_VP_0_RGB_0_BASE                     REG_MDP(0x15000)
#define MDP_VP_0_RGB_1_BASE                     REG_MDP(0x17000)
#define MDP_VP_0_DMA_0_BASE                     REG_MDP(0x25000)
#define MDP_VP_0_DMA_1_BASE                     REG_MDP(0x27000)
#define MDP_VP_0_MIXER_0_BASE                   REG_MDP(0x45000)
#define MDP_VP_0_MIXER_1_BASE                   REG_MDP(0x46000)

#define DMA_CMD_OFFSET              0x048
#define DMA_CMD_LENGTH              0x04C

#define INT_CTRL                    0x110
#define CMD_MODE_DMA_SW_TRIGGER     0x090

#define EOT_PACKET_CTRL             0x0CC
#define MISR_CMD_CTRL               0x0A0
#define MISR_VIDEO_CTRL             0x0A4
#define VIDEO_MODE_CTRL             0x010
#define HS_TIMER_CTRL               0x0BC

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

#define QPNP_LED_CTRL_BASE          0xD000
#define QPNP_BLUE_LPG_CTRL_BASE     0xB100
#define QPNP_GREEN_LPG_CTRL_BASE    0xB200
#define QPNP_RED_LPG_CTRL_BASE      0xB300

/* HDMI reg addresses */
#define HDMI_BASE               0xFD9A8000
#define REG_HDMI(off)           (HDMI_BASE + (off))

#define HDMI_ACR_32_0           REG_HDMI(0xC4)
#define HDMI_ACR_32_1           REG_HDMI(0xC8)
#define HDMI_ACR_44_0           REG_HDMI(0xCC)
#define HDMI_ACR_44_1           REG_HDMI(0xD0)
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

#define HDMI_DDC_SPEED          REG_HDMI(0x220)
#define HDMI_DDC_SETUP          REG_HDMI(0x224)
#define HDMI_DDC_REF            REG_HDMI(0x27C)
#define HDMI_DDC_DATA           REG_HDMI(0x238)
#define HDMI_DDC_TRANS0         REG_HDMI(0x228)
#define HDMI_DDC_TRANS1         REG_HDMI(0x22C)
#define HDMI_DDC_CTRL           REG_HDMI(0x20C)
#define HDMI_DDC_INT_CTRL       REG_HDMI(0x214)
#define HDMI_DDC_SW_STATUS      REG_HDMI(0x218)
#define HDMI_DDC_ARBITRATION    REG_HDMI(0x210)

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

#define LPASS_LPAIF_RDDMA_CTL0       0xFE0D2000
#define LPASS_LPAIF_RDDMA_BASE0      0xFE0D2004
#define LPASS_LPAIF_RDDMA_BUFF_LEN0  0xFE0D2008
#define LPASS_LPAIF_RDDMA_PER_LEN0   0xFE0D2010
#define LPASS_LPAIF_DEBUG_CTL        0xFE0DE004

#endif
