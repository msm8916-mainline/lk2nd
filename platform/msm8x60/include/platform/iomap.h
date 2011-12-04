/* Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PLATFORM_MSM8X60_IOMAP_H_
#define _PLATFORM_MSM8X60_IOMAP_H_

#define MSM_IOMAP_BASE      0x00100000
#define MSM_IOMAP_END       0x28000000

#define MSM_USB_BASE	0x12500000
#define MSM_UART3_BASE	0xA9C00000

#define MSM_VIC_BASE	0x02080000

#define MSM_TMR_BASE      0x02000000
#define MSM_GPT_BASE      (MSM_TMR_BASE + 0x04)
#define MSM_DGT_BASE      (MSM_TMR_BASE + 0x24)
#define SPSS_TIMER_STATUS (MSM_TMR_BASE + 0x88)

#define GPT_REG(off)      (MSM_GPT_BASE + (off))
#define DGT_REG(off)      (MSM_DGT_BASE + (off))

#define GPT_MATCH_VAL      GPT_REG(0x0000)
#define GPT_COUNT_VAL      GPT_REG(0x0004)
#define GPT_ENABLE         GPT_REG(0x0008)
#define GPT_CLEAR          GPT_REG(0x000C)

#define DGT_MATCH_VAL      DGT_REG(0x0000)
#define DGT_COUNT_VAL      DGT_REG(0x0004)
#define DGT_ENABLE         DGT_REG(0x0008)
#define DGT_CLEAR          DGT_REG(0x000C)
#define DGT_CLK_CTL        DGT_REG(0x0010)

#define MSM_TCSR_BASE       0x16B00000
#define MSM_GIC_CPU_BASE    0x02081000
#define MSM_GIC_DIST_BASE   0x02080000

#define MSM_TCSR_SIZE   4096
#define MSM_GPT_BASE    (MSM_TMR_BASE + 0x04)
#define MSM_CSR_BASE    0x02081000
#define MSM_GCC_BASE	0x02082000
#define MSM_ACC0_BASE	0x02041000
#define MSM_ACC1_BASE	0x02051000

#define TLMM_BASE_ADDR      0x00800000

#define TCSR_WDOG_CFG   0x30
#define MSM_WDT0_RST    (MSM_TMR_BASE + 0x38)
#define MSM_WDT0_EN     (MSM_TMR_BASE + 0x40)
#define MSM_WDT0_BT     (MSM_TMR_BASE + 0x4C)
#define MSM_PSHOLD_CTL_SU   (TLMM_BASE_ADDR + 0x820)

#define MSM_SDC1_BASE       0x12400000
#define MSM_CRYPTO_BASE     0x18500000

#define MSM_SHARED_BASE     0x40000000

#define SURF_DEBUG_LED_ADDR    0x1D000202

#define TLMM_BASE_ADDR       0x00800000
#define GPIO_CONFIG_ADDR(x) (TLMM_BASE_ADDR + 0x1000 + (x)*0x10)
#define GPIO_IN_OUT_ADDR(x) (TLMM_BASE_ADDR + 0x1004 + (x)*0x10)

#define GPIO_CFG133_ADDR    0x00801850
#define GPIO_CFG135_ADDR    0x00801870
#define GPIO_CFG136_ADDR    0x00801880
#define GPIO_CFG137_ADDR    0x00801890
#define GPIO_CFG138_ADDR    0x008018A0
#define GPIO_CFG139_ADDR    0x008018B0
#define GPIO_CFG140_ADDR    0x008018C0
#define GPIO_CFG141_ADDR    0x008018D0
#define GPIO_CFG142_ADDR    0x008018E0
#define GPIO_CFG143_ADDR    0x008018F0
#define GPIO_CFG144_ADDR    0x00801900
#define GPIO_CFG145_ADDR    0x00801910
#define GPIO_CFG146_ADDR    0x00801920
#define GPIO_CFG147_ADDR    0x00801930
#define GPIO_CFG148_ADDR    0x00801940
#define GPIO_CFG149_ADDR    0x00801950
#define GPIO_CFG150_ADDR    0x00801960
#define GPIO_CFG151_ADDR    0x00801970
#define GPIO_CFG152_ADDR    0x00801980
#define GPIO_CFG153_ADDR    0x00801990
#define GPIO_CFG154_ADDR    0x008019A0
#define GPIO_CFG155_ADDR    0x008019B0
#define GPIO_CFG156_ADDR    0x008019C0
#define GPIO_CFG157_ADDR    0x008019D0
#define GPIO_CFG158_ADDR    0x008019E0

#define GSBI_BASE(id)         ((id) <= 7 ? (0x16000000 + (((id)-1) << 20)) : \
                                           (0x19800000 + (((id)-8) << 20)))
#define GSBI_UART_DM_BASE(id) (GSBI_BASE(id) + 0x40000)
#define QUP_BASE(id)          (GSBI_BASE(id) + 0x80000)

#define CLK_CTL_BASE                 0x00900000
#define SDC_MD(n)                   (CLK_CTL_BASE + 0x2828 + (32 * ((n) - 1)))
#define SDC_NS(n)                   (CLK_CTL_BASE + 0x282C + (32 * ((n) - 1)))
#define USB_HS1_HCLK_CTL            (CLK_CTL_BASE + 0x2900)
#define USB_HS1_XCVR_FS_CLK_MD      (CLK_CTL_BASE + 0x2908)
#define USB_HS1_XCVR_FS_CLK_NS      (CLK_CTL_BASE + 0x290C)
#define MSM_BOOT_PLL_ENABLE_SC0     (CLK_CTL_BASE + 0x34C0)
#define MSM_BOOT_PLL8_STATUS        (CLK_CTL_BASE + 0x3158)
#define GSBIn_HCLK_CTL(n)           (CLK_CTL_BASE + 0x29C0 + (32 * ((n) - 1)))
#define GSBIn_HCLK_FS(n)            (CLK_CTL_BASE + 0x29C4 + (32 * ((n) - 1)))
#define GSBIn_QUP_APPS_MD(n)        (CLK_CTL_BASE + 0x29C8 + (32 * ((n) - 1)))
#define GSBIn_QUP_APPS_NS(n)        (CLK_CTL_BASE + 0x29CC + (32 * ((n) - 1)))
#define GSBIn_UART_APPS_MD(n)       (CLK_CTL_BASE + 0x29D0 + (32 * ((n) - 1)))
#define GSBIn_UART_APPS_NS(n)       (CLK_CTL_BASE + 0x29D4 + (32 * ((n) - 1)))

/* Defines for the GPIO EXPANDER chip, SX1509QIULTRT */
#define GPIO_EXPANDER_REG_OPEN_DRAIN_A  (0x0B)
#define GPIO_EXPANDER_REG_DIR_B     (0x0E)
#define GPIO_EXPANDER_REG_DIR_A     (0x0F)
#define GPIO_EXPANDER_REG_DATA_B    (0x10)
#define GPIO_EXPANDER_REG_DATA_A    (0x11)
#define CORE_GPIO_EXPANDER_I2C_ADDRESS  (0x3E)
#define EEPROM_I2C_ADDRESS              (0x52)

#define EBI2_CHIP_SELECT_CFG0    0x1A100000
#define EBI2_XMEM_CS3_CFG1       0x1A110034

#define MSM_ADM_BASE            0x18400000
#define MSM_ADM_SD_OFFSET       0x00020800

/* MMSS CLK CTR base address */
#define MSM_MMSS_CLK_CTL_BASE 0x04000000

#define MIPI_DSI_BASE                         (0x04700000)
#define REG_DSI(off)                          (MIPI_DSI_BASE + (off))

#define DSIPHY_REGULATOR_BASE                 (0x2CC)
#define DSIPHY_TIMING_BASE                    (0x260)
#define DSIPHY_CTRL_BASE                      (0x290)
#define DSIPHY_PLL_BASE                       (0x200)
#define DSIPHY_STRENGTH_BASE                  (0x2A0)

/* Range 0 - 4 */
#define DSIPHY_REGULATOR_CTRL(x) REG_DSI(DSIPHY_REGULATOR_BASE + (x) * 4)
/* Range 0 - 11 */
#define DSIPHY_TIMING_CTRL(x)    REG_DSI(DSIPHY_TIMING_BASE + (x) * 4)
/* Range 0 - 3 */
#define DSIPHY_CTRL(x)           REG_DSI(DSIPHY_CTRL_BASE + (x) * 4)
/* Range 0 - 2 */
#define DSIPHY_STRENGTH_CTRL(x)  REG_DSI(DSIPHY_STRENGTH_BASE + (x) * 4)
/* Range 0 - 19 */
#define DSIPHY_PLL_CTRL(x)       REG_DSI(DSIPHY_PLL_BASE + (x) * 4)

//TODO: Use mem on the stack
#define DSI_CMD_DMA_MEM_START_ADDR_PANEL      (0x46000000)

#define MDP_BASE                              (0x05100000)
#define REG_MDP(off)                          (MDP_BASE + (off))

//TODO: Where does this belong?
#define MMSS_SFPB_GPREG                       (0x05700058)

/* HDMI base addresses */
#define MSM_HDMI_BASE           0x04A00000
#define DTV_BASE                0xD0000

#define HDMI_USEC_REFTIMER      (MSM_HDMI_BASE + 0x0208)
#define HDMI_CTRL               (MSM_HDMI_BASE + 0x0000)

#define HDMI_PHY_REG_0          (MSM_HDMI_BASE + 0x00000300)
#define HDMI_PHY_REG_1          (MSM_HDMI_BASE + 0x00000304)
#define HDMI_PHY_REG_2          (MSM_HDMI_BASE + 0x00000308)
#define HDMI_PHY_REG_3          (MSM_HDMI_BASE + 0x0000030c)
#define HDMI_PHY_REG_4          (MSM_HDMI_BASE + 0x00000310)
#define HDMI_PHY_REG_9          (MSM_HDMI_BASE + 0x00000324)
#define HDMI_PHY_REG_11         (MSM_HDMI_BASE + 0x0000032c)
#define HDMI_PHY_REG_12         (MSM_HDMI_BASE + 0x00000330)
#define HDMI_TOTAL              (MSM_HDMI_BASE + 0x000002C0)
#define HDMI_ACTIVE_HSYNC       (MSM_HDMI_BASE + 0x000002B4)
#define HDMI_ACTIVE_VSYNC       (MSM_HDMI_BASE + 0x000002B8)
#define HDMI_VSYNC_TOTAL_F2     (MSM_HDMI_BASE + 0x000002C4)
#define HDMI_VSYNC_ACTIVE_F2    (MSM_HDMI_BASE + 0x000002BC)
#define HDMI_FRAME_CTRL         (MSM_HDMI_BASE + 0x000002C8)

#endif
