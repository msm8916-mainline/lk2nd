/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MSM8909_IOMAP_H_
#define _PLATFORM_MSM8909_IOMAP_H_

#define MSM_IOMAP_BASE              0x00000000
#define MSM_IOMAP_END               0x08000000

#define A7_SS_BASE                 0x0B000000
#define A7_SS_END                  0x0B200000

#define SYSTEM_IMEM_BASE            0x08600000
#define MSM_SHARED_IMEM_BASE        0x08600000

#define RESTART_REASON_ADDR         (MSM_SHARED_IMEM_BASE + 0x65C)
#define BS_INFO_OFFSET              (0x6B0)
#define BS_INFO_ADDR                (MSM_SHARED_IMEM_BASE + BS_INFO_OFFSET)

#define SDRAM_START_ADDR            0x80000000

#define MSM_SHARED_BASE             0x87D00000

#define MSM_NAND_BASE               0x79B0000
/* NAND BAM */
#define MSM_NAND_BAM_BASE           0x7984000

#define APPS_SS_BASE                0x0B000000

#define MSM_GIC_DIST_BASE           APPS_SS_BASE
#define MSM_GIC_CPU_BASE            (APPS_SS_BASE + 0x2000)
#define APPS_APCS_QTMR_AC_BASE      (APPS_SS_BASE + 0x00020000)
#define APPS_APCS_F0_QTMR_V1_BASE   (APPS_SS_BASE + 0x00021000)
#define APCS_ALIAS0_IPC_INTERRUPT   (APPS_SS_BASE + 0x00011008)
#define QTMR_BASE                   APPS_APCS_F0_QTMR_V1_BASE

#define PERIPH_SS_BASE              0x07800000

#define MSM_SDC1_BASE               (PERIPH_SS_BASE + 0x00024000)
#define MSM_SDC1_SDHCI_BASE         (PERIPH_SS_BASE + 0x00024900)
#define MSM_SDC2_BASE               (PERIPH_SS_BASE + 0x00064000)
#define MSM_SDC2_SDHCI_BASE         (PERIPH_SS_BASE + 0x00064900)

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
#define GCC_BLSP1_QUP2_APPS_CBCR    (CLK_CTL_BASE + 0x3010)
#define GCC_BLSP1_QUP2_CFG_RCGR     (CLK_CTL_BASE + 0x3018)
#define GCC_BLSP1_QUP2_CMD_RCGR     (CLK_CTL_BASE + 0x3014)


/* GPLL */
#define GPLL0_STATUS                (CLK_CTL_BASE + 0x21024)
#define GPLL0_MODE                  (CLK_CTL_BASE + 0x21000)
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
#define BLSP1_UART2_APPS_CBCR       (CLK_CTL_BASE + 0x302C)
#define BLSP1_UART2_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x3034)
#define BLSP1_UART2_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x3038)
#define BLSP1_UART2_APPS_M          (CLK_CTL_BASE + 0x303C)
#define BLSP1_UART2_APPS_N          (CLK_CTL_BASE + 0x3040)
#define BLSP1_UART2_APPS_D          (CLK_CTL_BASE + 0x3044)

#define BLSP1_UART1_APPS_CBCR       (CLK_CTL_BASE + 0x203C)
#define BLSP1_UART1_APPS_CMD_RCGR   (CLK_CTL_BASE + 0x2044)
#define BLSP1_UART1_APPS_CFG_RCGR   (CLK_CTL_BASE + 0x2048)
#define BLSP1_UART1_APPS_M          (CLK_CTL_BASE + 0x204C)
#define BLSP1_UART1_APPS_N          (CLK_CTL_BASE + 0x2050)
#define BLSP1_UART1_APPS_D          (CLK_CTL_BASE + 0x2054)

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
#define APP_REGION_ADDR 0x87a00000
#define APP_REGION_SIZE 0x200000


/* MDSS */
#define MIPI_DSI_BASE               (0x1AC8000)
#define MIPI_DSI0_BASE              MIPI_DSI_BASE
#define MIPI_DSI1_BASE              MIPI_DSI_BASE
#define DSI0_PHY_BASE               (0x1AC8500)
#define DSI1_PHY_BASE               DSI0_PHY_BASE
#define DSI0_PLL_BASE               (0x1AC8300)
#define DSI1_PLL_BASE               DSI0_PLL_BASE
#define REG_DSI(off)                (MIPI_DSI_BASE + 0x04 + (off))


/* MDP */
#define MDP_BASE                    0x1A00000
#define REG_MDP(off)                (MDP_BASE + (off))

#define MDP_DMA_P_CONFIG            REG_MDP(0x90000)
#define MDP_DMA_P_OUT_XY            REG_MDP(0x90010)
#define MDP_DMA_P_SIZE              REG_MDP(0x90004)
#define MDP_DMA_P_BUF_ADDR          REG_MDP(0x90008)
#define MDP_DMA_P_BUF_Y_STRIDE      REG_MDP(0x9000C)

#define MDP_DMA_P_QOS_REMAPPER      REG_MDP(0x90090)
#define MDP_DMA_P_WATERMARK_0       REG_MDP(0x90094)
#define MDP_DMA_P_WATERMARK_1       REG_MDP(0x90098)
#define MDP_DMA_P_WATERMARK_2       REG_MDP(0x9009C)
#define MDP_PANIC_ROBUST_CTRL       REG_MDP(0x900A0)
#define MDP_PANIC_LUT0              REG_MDP(0x900A4)
#define MDP_PANIC_LUT1              REG_MDP(0x900A8)
#define MDP_ROBUST_LUT              REG_MDP(0x900AC)

#define MDP_DSI_VIDEO_EN                 REG_MDP(0xF0000)
#define MDP_DSI_VIDEO_HSYNC_CTL          REG_MDP(0xF0004)
#define MDP_DSI_VIDEO_VSYNC_PERIOD       REG_MDP(0xF0008)
#define MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH  REG_MDP(0xF000C)
#define MDP_DSI_VIDEO_DISPLAY_HCTL       REG_MDP(0xF0010)
#define MDP_DSI_VIDEO_DISPLAY_V_START    REG_MDP(0xF0014)
#define MDP_DSI_VIDEO_DISPLAY_V_END      REG_MDP(0xF0018)
#define MDP_DSI_VIDEO_ACTIVE_HCTL        REG_MDP(0xF001C)
#define MDP_DSI_VIDEO_ACTIVE_V_START     REG_MDP(0xF0020)
#define MDP_DSI_VIDEO_ACTIVE_V_END       REG_MDP(0xF0024)

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

#define MDP_CGC_EN                  REG_MDP(0x100)
#define MDP_AUTOREFRESH_CONFIG_P    REG_MDP(0x34C)
#define MDP_AUTOREFRESH_EN          0x10000000
/* Auto refresh fps = Panel fps / MDP_AUTOREFRESH_FRAME_NUM */
/* Auto refresh fps = 60/10 = 6fps */
#define MDP_AUTOREFRESH_FRAME_NUM   10
#define MDP_SYNC_CONFIG_0           REG_MDP(0x300)

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

/* Boot config */
#define SEC_CTRL_CORE_BASE          0x00058000
#define BOOT_CONFIG_OFFSET          0x0000602C
#define BOOT_CONFIG_REG             (SEC_CTRL_CORE_BASE + BOOT_CONFIG_OFFSET)

#define SECURITY_CONTROL_CORE_FEATURE_CONFIG0    0x0005E004
/* EBI2 */
#define TLMM_EBI2_EMMC_GPIO_CFG     (TLMM_BASE_ADDR + 0x00111000)
#define TCSR_BOOT_MISC_DETECT       0x193D100
#endif
