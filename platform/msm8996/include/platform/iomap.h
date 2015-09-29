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
#define GCC_QUSB2_PHY_BCR           (CLK_CTL_BASE + 0x00012038)

#define AHB2_PHY_BASE               0x7416000
#define PERIPH_SS_AHB2PHY_TOP_CFG   (AHB2_PHY_BASE + 0x10)
#define GCC_RX2_USB2_CLKREF_EN      0x00388014

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

/* QSEECOM: Secure app region notification */
#define APP_REGION_ADDR 0x86600000
#define APP_REGION_SIZE 0x2200000

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

/* RPMB send receive buffer needs to be mapped
 * as device memory, define the start address
 * and size in MB
 */
#define RPMB_SND_RCV_BUF            0x91A00000
#define RPMB_SND_RCV_BUF_SZ         0x2

#define TCSR_BOOT_MISC_DETECT       0x007B3000

#define MSM_MMSS_CLK_CTL_BASE       0x8C0000
#define MMSS_MISC_AHB_CBCR          (MSM_MMSS_CLK_CTL_BASE + 0x5018)

#define MIPI_DSI_BASE               (0x994000)
#define MIPI_DSI0_BASE              (MIPI_DSI_BASE)
#define MIPI_DSI1_BASE              (0x996000)
#define DSI0_PHY_BASE               (0x994400)
#define DSI1_PHY_BASE               (0x996400)
#define DSI0_PLL_BASE               (0x994800)
#define DSI1_PLL_BASE               (0x996800)
#define DSI0_REGULATOR_BASE         (0x994000)
#define DSI1_REGULATOR_BASE         (0x996000)

#define MMSS_DSI_PHY_PLL_CORE_VCO_TUNE  0x0160
#define MMSS_DSI_PHY_PLL_CORE_KVCO_CODE 0x0168

#define MDP_BASE                    (0x900000)
#define REG_MDP(off)                (MDP_BASE + (off))

#ifdef MDP_PP_0_BASE
#undef MDP_PP_0_BASE
#endif
#define MDP_PP_0_BASE               REG_MDP(0x71000)

#ifdef MDP_PP_1_BASE
#undef MDP_PP_1_BASE
#endif
#define MDP_PP_1_BASE               REG_MDP(0x71800)

#define MDP_DSC_0_BASE			REG_MDP(0x81000)
#define MDP_DSC_1_BASE			REG_MDP(0x81400)

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
#define VBIF_VBIF_DDR_FORCE_CLK_ON              REG_MDP(0xb0004)

#ifdef VBIF_VBIF_DDR_OUT_MAX_BURST
#undef VBIF_VBIF_DDR_OUT_MAX_BURST
#endif
#define VBIF_VBIF_DDR_OUT_MAX_BURST             REG_MDP(0xb00D8)

#ifdef VBIF_VBIF_DDR_ARB_CTRL
#undef VBIF_VBIF_DDR_ARB_CTRL
#endif
#define VBIF_VBIF_DDR_ARB_CTRL                  REG_MDP(0xb00F0)

#ifdef VBIF_VBIF_DDR_RND_RBN_QOS_ARB
#undef VBIF_VBIF_DDR_RND_RBN_QOS_ARB
#endif
#define VBIF_VBIF_DDR_RND_RBN_QOS_ARB           REG_MDP(0xb0124)

#ifdef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0
#undef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0
#endif
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0        REG_MDP(0xb0160)

#ifdef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1
#undef VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1
#endif
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1        REG_MDP(0xb0164)

#ifdef VBIF_VBIF_DDR_OUT_AOOO_AXI_EN
#undef VBIF_VBIF_DDR_OUT_AOOO_AXI_EN
#endif
#define VBIF_VBIF_DDR_OUT_AOOO_AXI_EN           REG_MDP(0xb0178)

#ifdef VBIF_VBIF_DDR_OUT_AX_AOOO
#undef VBIF_VBIF_DDR_OUT_AX_AOOO
#endif
#define VBIF_VBIF_DDR_OUT_AX_AOOO               REG_MDP(0xb017C)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF0
#undef VBIF_VBIF_IN_RD_LIM_CONF0
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF0               REG_MDP(0xb00B0)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF1
#undef VBIF_VBIF_IN_RD_LIM_CONF1
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF1               REG_MDP(0xb00B4)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF2
#undef VBIF_VBIF_IN_RD_LIM_CONF2
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF2               REG_MDP(0xb00B8)

#ifdef VBIF_VBIF_IN_RD_LIM_CONF3
#undef VBIF_VBIF_IN_RD_LIM_CONF3
#endif
#define VBIF_VBIF_IN_RD_LIM_CONF3               REG_MDP(0xb00BC)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF0
#undef VBIF_VBIF_IN_WR_LIM_CONF0
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF0               REG_MDP(0xb00C0)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF1
#undef VBIF_VBIF_IN_WR_LIM_CONF1
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF1               REG_MDP(0xb00C4)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF2
#undef VBIF_VBIF_IN_WR_LIM_CONF2
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF2               REG_MDP(0xb00C8)

#ifdef VBIF_VBIF_IN_WR_LIM_CONF3
#undef VBIF_VBIF_IN_WR_LIM_CONF3
#endif
#define VBIF_VBIF_IN_WR_LIM_CONF3               REG_MDP(0xb00CC)

#ifdef VBIF_VBIF_ABIT_SHORT
#undef VBIF_VBIF_ABIT_SHORT
#endif
#define VBIF_VBIF_ABIT_SHORT                    REG_MDP(0xb0070)

#ifdef VBIF_VBIF_ABIT_SHORT_CONF
#undef VBIF_VBIF_ABIT_SHORT_CONF
#endif
#define VBIF_VBIF_ABIT_SHORT_CONF               REG_MDP(0xb0074)

#ifdef VBIF_VBIF_GATE_OFF_WRREQ_EN
#undef VBIF_VBIF_GATE_OFF_WRREQ_EN
#endif
#define VBIF_VBIF_GATE_OFF_WRREQ_EN             REG_MDP(0xb00A8)

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

#define VIDEO_COMPRESSION_MODE_CTRL		0x2A0
#define VIDEO_COMPRESSION_MODE_CTRL_2		0x2A4
#define CMD_COMPRESSION_MODE_CTRL		0x2A8
#define CMD_COMPRESSION_MODE_CTRL_2		0x2Ac
#define CMD_COMPRESSION_MODE_CTRL_3		0x2B0

#define QPNP_LED_CTRL_BASE          0xD000
#define QPNP_BLUE_LPG_CTRL_BASE     0xB100
#define QPNP_GREEN_LPG_CTRL_BASE    0xB200
#define QPNP_RED_LPG_CTRL_BASE      0xB300

#define APSS_WDOG_BASE              0x9830000
#define APPS_WDOG_BARK_VAL_REG      (APSS_WDOG_BASE + 0x10)
#define APPS_WDOG_BITE_VAL_REG      (APSS_WDOG_BASE + 0x14)
#define APPS_WDOG_RESET_REG         (APSS_WDOG_BASE + 0x04)
#define APPS_WDOG_CTL_REG           (APSS_WDOG_BASE + 0x08)

#define DDR_START                    platform_get_ddr_start()
#define ABOOT_FORCE_KERNEL_ADDR      DDR_START + 0x8000
#define ABOOT_FORCE_RAMDISK_ADDR     DDR_START + 0x2200000
#define ABOOT_FORCE_TAGS_ADDR        DDR_START + 0x2000000
#define ABOOT_FORCE_KERNEL64_ADDR    DDR_START + 0x80000
#endif
