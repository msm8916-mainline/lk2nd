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

#ifndef _PLATFORM_MSM8916_IOMAP_H_
#define _PLATFORM_MSM8916_IOMAP_H_

#define MSM_IOMAP_BASE              0x00000000
#define MSM_IOMAP_END               0x08000000
#define A53_SS_BASE                 0x0B000000
#define A53_SS_END                  0x0B200000

#define SYSTEM_IMEM_BASE            0x08600000
#define MSM_SHARED_IMEM_BASE        0x08600000

#define RESTART_REASON_ADDR         (MSM_SHARED_IMEM_BASE + 0x65C)
#define BS_INFO_OFFSET              (0x6B0)
#define BS_INFO_ADDR                (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)
#define SDRAM_START_ADDR            0x80000000

#define MSM_SHARED_BASE             0x86300000
#define APPS_SS_BASE                0x0B000000

#define DDR_START                   get_ddr_start()
#define ABOOT_FORCE_KERNEL_ADDR     DDR_START + 0x8000
#define ABOOT_FORCE_KERNEL64_ADDR   DDR_START + 0x80000
#define ABOOT_FORCE_RAMDISK_ADDR    DDR_START + 0x2000000
#define ABOOT_FORCE_TAGS_ADDR       DDR_START + 0x1E00000

#define MSM_GIC_DIST_BASE           APPS_SS_BASE
#define MSM_GIC_CPU_BASE            (APPS_SS_BASE + 0x2000)
#define APCS_ALIAS0_IPC_INTERRUPT   (APPS_SS_BASE + 0x00011008)
#define MSM_WATCHDOG_BASE           (APPS_SS_BASE + 0x00017000)
#define MSM_WATCHDOG_RST            (MSM_WATCHDOG_BASE + 0x04)
#define MSM_WATCHDOG_EN             (MSM_WATCHDOG_BASE + 0x08)
#define APPS_APCS_QTMR_AC_BASE      (APPS_SS_BASE + 0x00020000)
#define APPS_APCS_F0_QTMR_V1_BASE   (APPS_SS_BASE + 0x00021000)
#define QTMR_BASE                   APPS_APCS_F0_QTMR_V1_BASE

#define APCS_BANKED_SAW2_BASE       (APPS_SS_BASE + 0x9000)
#define APCS_L2_SAW2_BASE           (APPS_SS_BASE + 0x12000)

#define PERIPH_SS_BASE              0x07800000

#define MSM_SDC1_BASE               (PERIPH_SS_BASE + 0x00024000)
#define MSM_SDC1_SDHCI_BASE         (PERIPH_SS_BASE + 0x00024900)
#define MSM_SDC2_BASE               (PERIPH_SS_BASE + 0x00064000)
#define MSM_SDC2_SDHCI_BASE        (PERIPH_SS_BASE + 0x00064900)

/* SDHCI */
#define SDCC_MCI_HC_MODE            (0x00000078)
#define SDCC_HC_PWRCTL_STATUS_REG   (0x000000DC)
#define SDCC_HC_PWRCTL_MASK_REG     (0x000000E0)
#define SDCC_HC_PWRCTL_CLEAR_REG    (0x000000E4)
#define SDCC_HC_PWRCTL_CTL_REG      (0x000000E8)
#define BLSP1_UART0_BASE            (PERIPH_SS_BASE + 0x000AF000)
#define BLSP1_UART1_BASE            (PERIPH_SS_BASE + 0x000B0000)
#define MSM_USB_BASE                (PERIPH_SS_BASE + 0x000D9000)

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
#define  MSM_CE1_BASE               0x073A000
#define  MSM_CE1_BAM_BASE           0x0704000
#define  GCC_CRYPTO_BCR             (CLK_CTL_BASE + 0x16000)
#define  GCC_CRYPTO_CMD_RCGR        (CLK_CTL_BASE + 0x16004)
#define  GCC_CRYPTO_CFG_RCGR        (CLK_CTL_BASE + 0x16008)
#define  GCC_CRYPTO_CBCR            (CLK_CTL_BASE + 0x1601C)
#define  GCC_CRYPTO_AXI_CBCR        (CLK_CTL_BASE + 0x16020)
#define  GCC_CRYPTO_AHB_CBCR        (CLK_CTL_BASE + 0x16024)

/* I2C */
#define BLSP_QUP_BASE(blsp_id, qup_id) (PERIPH_SS_BASE + 0xB5000 + 0x1000 * qup_id)

#define GCC_BLSP1_QUP1_APPS_CBCR    (CLK_CTL_BASE + 0x2008)
#define GCC_BLSP1_QUP1_CFG_RCGR     (CLK_CTL_BASE + 0x2010)
#define GCC_BLSP1_QUP1_CMD_RCGR     (CLK_CTL_BASE + 0x200C)

#define GCC_BLSP1_QUP2_APPS_CBCR    (CLK_CTL_BASE + 0x3010)
#define GCC_BLSP1_QUP2_CFG_RCGR     (CLK_CTL_BASE + 0x3004)
#define GCC_BLSP1_QUP2_CMD_RCGR     (CLK_CTL_BASE + 0x3000)

#define GCC_BLSP1_QUP3_APPS_CBCR    (CLK_CTL_BASE + 0x4020)
#define GCC_BLSP1_QUP3_CFG_RCGR     (CLK_CTL_BASE + 0x4004)
#define GCC_BLSP1_QUP3_CMD_RCGR     (CLK_CTL_BASE + 0x4000)

#define GCC_BLSP1_QUP4_APPS_CBCR    (CLK_CTL_BASE + 0x5020)
#define GCC_BLSP1_QUP4_CFG_RCGR     (CLK_CTL_BASE + 0x5004)
#define GCC_BLSP1_QUP4_CMD_RCGR     (CLK_CTL_BASE + 0x5000)

#define GCC_BLSP1_QUP5_APPS_CBCR    (CLK_CTL_BASE + 0x6020)
#define GCC_BLSP1_QUP5_CFG_RCGR     (CLK_CTL_BASE + 0x6004)
#define GCC_BLSP1_QUP5_CMD_RCGR     (CLK_CTL_BASE + 0x6000)

#define GCC_BLSP1_QUP6_APPS_CBCR    (CLK_CTL_BASE + 0x7020)
#define GCC_BLSP1_QUP6_CFG_RCGR     (CLK_CTL_BASE + 0x7004)
#define GCC_BLSP1_QUP6_CMD_RCGR     (CLK_CTL_BASE + 0x7000)

/* GPLL */
#define GPLL0_STATUS                (CLK_CTL_BASE + 0x2101C)
#define GPLL1_STATUS                (CLK_CTL_BASE + 0x2001C)
#define APCS_GPLL_ENA_VOTE          (CLK_CTL_BASE + 0x45000)
#define APCS_CLOCK_BRANCH_ENA_VOTE  (CLK_CTL_BASE + 0x45004)

/* SDCC */
#define SDC1_HDRV_PULL_CTL          (TLMM_BASE_ADDR + 0x10A000)
#define SDCC1_BCR                   (CLK_CTL_BASE + 0x42000) /* block reset*/
#define SDCC1_APPS_CBCR             (CLK_CTL_BASE + 0x42018) /* branch ontrol */
#define SDCC1_AHB_CBCR              (CLK_CTL_BASE + 0x4201C)
#define SDCC1_CMD_RCGR              (CLK_CTL_BASE + 0x42004) /* cmd */
#define SDCC1_CFG_RCGR              (CLK_CTL_BASE + 0x42008) /* cfg */
#define SDCC1_M                     (CLK_CTL_BASE + 0x4200C) /* m */
#define SDCC1_N                     (CLK_CTL_BASE + 0x42010) /* n */
#define SDCC1_D                     (CLK_CTL_BASE + 0x42014) /* d */

#define SDC2_HDRV_PULL_CTL          (TLMM_BASE_ADDR + 0x109000)
#define SDCC2_BCR                   (CLK_CTL_BASE + 0x43000) /* block reset */
#define SDCC2_APPS_CBCR             (CLK_CTL_BASE + 0x43018) /* branch control */
#define SDCC2_AHB_CBCR              (CLK_CTL_BASE + 0x4301C)
#define SDCC2_CMD_RCGR              (CLK_CTL_BASE + 0x43004) /* cmd */
#define SDCC2_CFG_RCGR              (CLK_CTL_BASE + 0x43008) /* cfg */
#define SDCC2_M                     (CLK_CTL_BASE + 0x4300C) /* m */
#define SDCC2_N                     (CLK_CTL_BASE + 0x43010) /* n */
#define SDCC2_D                     (CLK_CTL_BASE + 0x43014) /* d */

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


/* USB */
#define USB_HS_BCR                  (CLK_CTL_BASE + 0x41000)
#define USB_HS_SYSTEM_CBCR          (CLK_CTL_BASE + 0x41004)
#define USB_HS_AHB_CBCR             (CLK_CTL_BASE + 0x41008)
#define USB_HS_SYSTEM_CMD_RCGR      (CLK_CTL_BASE + 0x41010)
#define USB_HS_SYSTEM_CFG_RCGR      (CLK_CTL_BASE + 0x41014)


/* RPMB send receive buffer needs to be mapped
 * as device memory, define the start address
 * and size in MB
 */
#define RPMB_SND_RCV_BUF            0x90000000
#define RPMB_SND_RCV_BUF_SZ         0x1

/* QSEECOM: Secure app region notification */
#define APP_REGION_ADDR 0x86000000
#define APP_REGION_SIZE 0x300000

/* MDSS */
#define MIPI_DSI_BASE               (0x1A98000)
#define MIPI_DSI0_BASE              MIPI_DSI_BASE
#define MIPI_DSI1_BASE              (0x1AA0000)
#define DSI0_PHY_BASE               (0x1A98500)
#define DSI1_PHY_BASE               (0x1AA0500)
#define DSI0_PLL_BASE               (0x1A98300)
#define DSI1_PLL_BASE               DSI0_PLL_BASE
#define REG_DSI(off)                (MIPI_DSI_BASE + 0x04 + (off))
#define MDP_BASE                    (0x1A00000)
#define REG_MDP(off)                (MDP_BASE + (off))
#define MDP_HW_REV                              REG_MDP(0x1000)
#define MDP_VP_0_VIG_0_BASE                     REG_MDP(0x5000)
#define MDP_VP_0_VIG_1_BASE                     REG_MDP(0x7000)
#define MDP_VP_0_RGB_0_BASE                     REG_MDP(0x15000)
#define MDP_VP_0_RGB_1_BASE                     REG_MDP(0x17000)
#define MDP_VP_0_DMA_0_BASE                     REG_MDP(0x25000)
#define MDP_VP_0_DMA_1_BASE                     REG_MDP(0x27000)
#define MDP_VP_0_MIXER_0_BASE                   REG_MDP(0x45000)
#define MDP_VP_0_MIXER_1_BASE                   REG_MDP(0x46000)
#define MDP_DISP_INTF_SEL                       REG_MDP(0x1004)
#define MDP_VIDEO_INTF_UNDERFLOW_CTL            REG_MDP(0x12E0)
#define MDP_UPPER_NEW_ROI_PRIOR_RO_START        REG_MDP(0x11EC)
#define MDP_LOWER_NEW_ROI_PRIOR_TO_START        REG_MDP(0x13F8)
#define MDP_CTL_0_BASE                          REG_MDP(0x2000)
#define MDP_CTL_1_BASE                          REG_MDP(0x2200)
#define MDP_CLK_CTRL0                           REG_MDP(0x012AC)
#define MDP_CLK_CTRL1                           REG_MDP(0x012B4)
#define MDP_CLK_CTRL2                           REG_MDP(0x012BC)
#define MDP_CLK_CTRL3                           REG_MDP(0x013A8)
#define MDP_CLK_CTRL4                           REG_MDP(0x013B0)
#define MDP_CLK_CTRL5                           REG_MDP(0x013B8)

#define MDP_INTF_0_BASE                         REG_MDP(0x11F00)
#define MDP_INTF_1_BASE                         REG_MDP(0x12700)
#define MDP_INTF_2_BASE                         REG_MDP(0x12F00)

#define MDP_REG_SPLIT_DISPLAY_EN                REG_MDP(0x12f4)
#define MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL    REG_MDP(0x12F8)
#define MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL    REG_MDP(0x13F0)

#define MDP_REG_PPB0_CNTL                       REG_MDP(0x1420)
#define MDP_REG_PPB0_CONFIG                     REG_MDP(0x1424)

#define MMSS_MDP_SMP_ALLOC_W_BASE               REG_MDP(0x1080)
#define MMSS_MDP_SMP_ALLOC_R_BASE               REG_MDP(0x1130)

#define MDP_QOS_REMAPPER_CLASS_0                REG_MDP(0x11E0)

#define VBIF_VBIF_DDR_FORCE_CLK_ON              REG_MDP(0xc8004)
#define VBIF_VBIF_DDR_OUT_MAX_BURST             REG_MDP(0xc80D8)
#define VBIF_VBIF_DDR_ARB_CTRL                  REG_MDP(0xc80F0)
#define VBIF_VBIF_DDR_RND_RBN_QOS_ARB           REG_MDP(0xc8124)
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0        REG_MDP(0xc8160)
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1        REG_MDP(0xc8164)
#define VBIF_VBIF_DDR_OUT_AOOO_AXI_EN           REG_MDP(0xc8178)
#define VBIF_VBIF_DDR_OUT_AX_AOOO               REG_MDP(0xc817C)
#define VBIF_VBIF_IN_RD_LIM_CONF0               REG_MDP(0xc80B0)
#define VBIF_VBIF_IN_RD_LIM_CONF1               REG_MDP(0xc80B4)
#define VBIF_VBIF_IN_RD_LIM_CONF2               REG_MDP(0xc80B8)
#define VBIF_VBIF_IN_RD_LIM_CONF3               REG_MDP(0xc80BC)
#define VBIF_VBIF_IN_WR_LIM_CONF0               REG_MDP(0xc80C0)
#define VBIF_VBIF_IN_WR_LIM_CONF1               REG_MDP(0xc80C4)
#define VBIF_VBIF_IN_WR_LIM_CONF2               REG_MDP(0xc80C8)
#define VBIF_VBIF_IN_WR_LIM_CONF3               REG_MDP(0xc80CC)
#define VBIF_VBIF_ABIT_SHORT                    REG_MDP(0xc8070)
#define VBIF_VBIF_ABIT_SHORT_CONF               REG_MDP(0xc8074)
#define VBIF_VBIF_GATE_OFF_WRREQ_EN             REG_MDP(0xc80A8)

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

#define DMA_CMD_OFFSET              0x048
#define DMA_CMD_LENGTH              0x04C

#define INT_CTRL                    0x110
#define CMD_MODE_DMA_SW_TRIGGER     0x090

#define EOT_PACKET_CTRL             0x0CC
#define MISR_CMD_CTRL               0x0A0
#define MISR_VIDEO_CTRL             0x0A4
#define VIDEO_MODE_CTRL             0x010
#define HS_TIMER_CTRL               0x0BC

#define TCSR_TZ_WONCE               0x193D000
#define TCSR_BOOT_MISC_DETECT	    0x193D100

#define BOOT_ROM_BASE               0x00100000
#define BOOT_ROM_END                0x00124000  /* Crashes when reading more */

#define RPM_DATA_RAM                0x00290000
#endif
