/* Copyright (c) 2016, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <debug.h>
#include <err.h>
#include <reg.h>
#include <smem.h>
#include <bits.h>
#include <msm_panel.h>
#include <platform/timer.h>
#include <platform/iomap.h>

/* PLL REGISTERS */
#define HDMI_PLL_BASE_OFFSET                     (0x9A0600)
#define QSERDES_COM_ATB_SEL1                     (HDMI_PLL_BASE_OFFSET + 0x000)
#define QSERDES_COM_ATB_SEL2                     (HDMI_PLL_BASE_OFFSET + 0x004)
#define QSERDES_COM_FREQ_UPDATE                  (HDMI_PLL_BASE_OFFSET + 0x008)
#define QSERDES_COM_BG_TIMER                     (HDMI_PLL_BASE_OFFSET + 0x00C)
#define QSERDES_COM_SSC_EN_CENTER                (HDMI_PLL_BASE_OFFSET + 0x010)
#define QSERDES_COM_SSC_ADJ_PER1                 (HDMI_PLL_BASE_OFFSET + 0x014)
#define QSERDES_COM_SSC_ADJ_PER2                 (HDMI_PLL_BASE_OFFSET + 0x018)
#define QSERDES_COM_SSC_PER1                     (HDMI_PLL_BASE_OFFSET + 0x01C)
#define QSERDES_COM_SSC_PER2                     (HDMI_PLL_BASE_OFFSET + 0x020)
#define QSERDES_COM_SSC_STEP_SIZE1               (HDMI_PLL_BASE_OFFSET + 0x024)
#define QSERDES_COM_SSC_STEP_SIZE2               (HDMI_PLL_BASE_OFFSET + 0x028)
#define QSERDES_COM_POST_DIV                     (HDMI_PLL_BASE_OFFSET + 0x02C)
#define QSERDES_COM_POST_DIV_MUX                 (HDMI_PLL_BASE_OFFSET + 0x030)
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN          (HDMI_PLL_BASE_OFFSET + 0x034)
#define QSERDES_COM_CLK_ENABLE1                  (HDMI_PLL_BASE_OFFSET + 0x038)
#define QSERDES_COM_SYS_CLK_CTRL                 (HDMI_PLL_BASE_OFFSET + 0x03C)
#define QSERDES_COM_SYSCLK_BUF_ENABLE            (HDMI_PLL_BASE_OFFSET + 0x040)
#define QSERDES_COM_PLL_EN                       (HDMI_PLL_BASE_OFFSET + 0x044)
#define QSERDES_COM_PLL_IVCO                     (HDMI_PLL_BASE_OFFSET + 0x048)
#define QSERDES_COM_LOCK_CMP1_MODE0              (HDMI_PLL_BASE_OFFSET + 0x04C)
#define QSERDES_COM_LOCK_CMP2_MODE0              (HDMI_PLL_BASE_OFFSET + 0x050)
#define QSERDES_COM_LOCK_CMP3_MODE0              (HDMI_PLL_BASE_OFFSET + 0x054)
#define QSERDES_COM_LOCK_CMP1_MODE1              (HDMI_PLL_BASE_OFFSET + 0x058)
#define QSERDES_COM_LOCK_CMP2_MODE1              (HDMI_PLL_BASE_OFFSET + 0x05C)
#define QSERDES_COM_LOCK_CMP3_MODE1              (HDMI_PLL_BASE_OFFSET + 0x060)
#define QSERDES_COM_LOCK_CMP1_MODE2              (HDMI_PLL_BASE_OFFSET + 0x064)
#define QSERDES_COM_CMN_RSVD0                    (HDMI_PLL_BASE_OFFSET + 0x064)
#define QSERDES_COM_LOCK_CMP2_MODE2              (HDMI_PLL_BASE_OFFSET + 0x068)
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL         (HDMI_PLL_BASE_OFFSET + 0x068)
#define QSERDES_COM_LOCK_CMP3_MODE2              (HDMI_PLL_BASE_OFFSET + 0x06C)
#define QSERDES_COM_SYSCLK_DET_COMP_STATUS       (HDMI_PLL_BASE_OFFSET + 0x06C)
#define QSERDES_COM_BG_TRIM                      (HDMI_PLL_BASE_OFFSET + 0x070)
#define QSERDES_COM_CLK_EP_DIV                   (HDMI_PLL_BASE_OFFSET + 0x074)
#define QSERDES_COM_CP_CTRL_MODE0                (HDMI_PLL_BASE_OFFSET + 0x078)
#define QSERDES_COM_CP_CTRL_MODE1                (HDMI_PLL_BASE_OFFSET + 0x07C)
#define QSERDES_COM_CP_CTRL_MODE2                (HDMI_PLL_BASE_OFFSET + 0x080)
#define QSERDES_COM_CMN_RSVD1                    (HDMI_PLL_BASE_OFFSET + 0x080)
#define QSERDES_COM_PLL_RCTRL_MODE0              (HDMI_PLL_BASE_OFFSET + 0x084)
#define QSERDES_COM_PLL_RCTRL_MODE1              (HDMI_PLL_BASE_OFFSET + 0x088)
#define QSERDES_COM_PLL_RCTRL_MODE2              (HDMI_PLL_BASE_OFFSET + 0x08C)
#define QSERDES_COM_CMN_RSVD2                    (HDMI_PLL_BASE_OFFSET + 0x08C)
#define QSERDES_COM_PLL_CCTRL_MODE0              (HDMI_PLL_BASE_OFFSET + 0x090)
#define QSERDES_COM_PLL_CCTRL_MODE1              (HDMI_PLL_BASE_OFFSET + 0x094)
#define QSERDES_COM_PLL_CCTRL_MODE2              (HDMI_PLL_BASE_OFFSET + 0x098)
#define QSERDES_COM_CMN_RSVD3                    (HDMI_PLL_BASE_OFFSET + 0x098)
#define QSERDES_COM_PLL_CNTRL                    (HDMI_PLL_BASE_OFFSET + 0x09C)
#define QSERDES_COM_PHASE_SEL_CTRL               (HDMI_PLL_BASE_OFFSET + 0x0A0)
#define QSERDES_COM_PHASE_SEL_DC                 (HDMI_PLL_BASE_OFFSET + 0x0A4)
#define QSERDES_COM_CORE_CLK_IN_SYNC_SEL         (HDMI_PLL_BASE_OFFSET + 0x0A8)
#define QSERDES_COM_BIAS_EN_CTRL_BY_PSM          (HDMI_PLL_BASE_OFFSET + 0x0A8)
#define QSERDES_COM_SYSCLK_EN_SEL                (HDMI_PLL_BASE_OFFSET + 0x0AC)
#define QSERDES_COM_CML_SYSCLK_SEL               (HDMI_PLL_BASE_OFFSET + 0x0B0)
#define QSERDES_COM_RESETSM_CNTRL                (HDMI_PLL_BASE_OFFSET + 0x0B4)
#define QSERDES_COM_RESETSM_CNTRL2               (HDMI_PLL_BASE_OFFSET + 0x0B8)
#define QSERDES_COM_RESTRIM_CTRL                 (HDMI_PLL_BASE_OFFSET + 0x0BC)
#define QSERDES_COM_RESTRIM_CTRL2                (HDMI_PLL_BASE_OFFSET + 0x0C0)
#define QSERDES_COM_RESCODE_DIV_NUM              (HDMI_PLL_BASE_OFFSET + 0x0C4)
#define QSERDES_COM_LOCK_CMP_EN                  (HDMI_PLL_BASE_OFFSET + 0x0C8)
#define QSERDES_COM_LOCK_CMP_CFG                 (HDMI_PLL_BASE_OFFSET + 0x0CC)
#define QSERDES_COM_DEC_START_MODE0              (HDMI_PLL_BASE_OFFSET + 0x0D0)
#define QSERDES_COM_DEC_START_MODE1              (HDMI_PLL_BASE_OFFSET + 0x0D4)
#define QSERDES_COM_DEC_START_MODE2              (HDMI_PLL_BASE_OFFSET + 0x0D8)
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL          (HDMI_PLL_BASE_OFFSET + 0x0D8)
#define QSERDES_COM_DIV_FRAC_START1_MODE0        (HDMI_PLL_BASE_OFFSET + 0x0DC)
#define QSERDES_COM_DIV_FRAC_START2_MODE0        (HDMI_PLL_BASE_OFFSET + 0x0E0)
#define QSERDES_COM_DIV_FRAC_START3_MODE0        (HDMI_PLL_BASE_OFFSET + 0x0E4)
#define QSERDES_COM_DIV_FRAC_START1_MODE1        (HDMI_PLL_BASE_OFFSET + 0x0E8)
#define QSERDES_COM_DIV_FRAC_START2_MODE1        (HDMI_PLL_BASE_OFFSET + 0x0EC)
#define QSERDES_COM_DIV_FRAC_START3_MODE1        (HDMI_PLL_BASE_OFFSET + 0x0F0)
#define QSERDES_COM_DIV_FRAC_START1_MODE2        (HDMI_PLL_BASE_OFFSET + 0x0F4)
#define QSERDES_COM_VCO_TUNE_MINVAL1             (HDMI_PLL_BASE_OFFSET + 0x0F4)
#define QSERDES_COM_DIV_FRAC_START2_MODE2        (HDMI_PLL_BASE_OFFSET + 0x0F8)
#define QSERDES_COM_VCO_TUNE_MINVAL2             (HDMI_PLL_BASE_OFFSET + 0x0F8)
#define QSERDES_COM_DIV_FRAC_START3_MODE2        (HDMI_PLL_BASE_OFFSET + 0x0FC)
#define QSERDES_COM_CMN_RSVD4                    (HDMI_PLL_BASE_OFFSET + 0x0FC)
#define QSERDES_COM_INTEGLOOP_INITVAL            (HDMI_PLL_BASE_OFFSET + 0x100)
#define QSERDES_COM_INTEGLOOP_EN                 (HDMI_PLL_BASE_OFFSET + 0x104)
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE0        (HDMI_PLL_BASE_OFFSET + 0x108)
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE0        (HDMI_PLL_BASE_OFFSET + 0x10C)
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE1        (HDMI_PLL_BASE_OFFSET + 0x110)
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE1        (HDMI_PLL_BASE_OFFSET + 0x114)
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE2        (HDMI_PLL_BASE_OFFSET + 0x118)
#define QSERDES_COM_VCO_TUNE_MAXVAL1             (HDMI_PLL_BASE_OFFSET + 0x118)
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE2        (HDMI_PLL_BASE_OFFSET + 0x11C)
#define QSERDES_COM_VCO_TUNE_MAXVAL2             (HDMI_PLL_BASE_OFFSET + 0x11C)
#define QSERDES_COM_RES_TRIM_CONTROL2            (HDMI_PLL_BASE_OFFSET + 0x120)
#define QSERDES_COM_VCO_TUNE_CTRL                (HDMI_PLL_BASE_OFFSET + 0x124)
#define QSERDES_COM_VCO_TUNE_MAP                 (HDMI_PLL_BASE_OFFSET + 0x128)
#define QSERDES_COM_VCO_TUNE1_MODE0              (HDMI_PLL_BASE_OFFSET + 0x12C)
#define QSERDES_COM_VCO_TUNE2_MODE0              (HDMI_PLL_BASE_OFFSET + 0x130)
#define QSERDES_COM_VCO_TUNE1_MODE1              (HDMI_PLL_BASE_OFFSET + 0x134)
#define QSERDES_COM_VCO_TUNE2_MODE1              (HDMI_PLL_BASE_OFFSET + 0x138)
#define QSERDES_COM_VCO_TUNE1_MODE2              (HDMI_PLL_BASE_OFFSET + 0x13C)
#define QSERDES_COM_VCO_TUNE_INITVAL1            (HDMI_PLL_BASE_OFFSET + 0x13C)
#define QSERDES_COM_VCO_TUNE2_MODE2              (HDMI_PLL_BASE_OFFSET + 0x140)
#define QSERDES_COM_VCO_TUNE_INITVAL2            (HDMI_PLL_BASE_OFFSET + 0x140)
#define QSERDES_COM_VCO_TUNE_TIMER1              (HDMI_PLL_BASE_OFFSET + 0x144)
#define QSERDES_COM_VCO_TUNE_TIMER2              (HDMI_PLL_BASE_OFFSET + 0x148)
#define QSERDES_COM_SAR                          (HDMI_PLL_BASE_OFFSET + 0x14C)
#define QSERDES_COM_SAR_CLK                      (HDMI_PLL_BASE_OFFSET + 0x150)
#define QSERDES_COM_SAR_CODE_OUT_STATUS          (HDMI_PLL_BASE_OFFSET + 0x154)
#define QSERDES_COM_SAR_CODE_READY_STATUS        (HDMI_PLL_BASE_OFFSET + 0x158)
#define QSERDES_COM_CMN_STATUS                   (HDMI_PLL_BASE_OFFSET + 0x15C)
#define QSERDES_COM_RESET_SM_STATUS              (HDMI_PLL_BASE_OFFSET + 0x160)
#define QSERDES_COM_RESTRIM_CODE_STATUS          (HDMI_PLL_BASE_OFFSET + 0x164)
#define QSERDES_COM_PLLCAL_CODE1_STATUS          (HDMI_PLL_BASE_OFFSET + 0x168)
#define QSERDES_COM_PLLCAL_CODE2_STATUS          (HDMI_PLL_BASE_OFFSET + 0x16C)
#define QSERDES_COM_BG_CTRL                      (HDMI_PLL_BASE_OFFSET + 0x170)
#define QSERDES_COM_CLK_SELECT                   (HDMI_PLL_BASE_OFFSET + 0x174)
#define QSERDES_COM_HSCLK_SEL                    (HDMI_PLL_BASE_OFFSET + 0x178)
#define QSERDES_COM_INTEGLOOP_BINCODE_STATUS     (HDMI_PLL_BASE_OFFSET + 0x17C)
#define QSERDES_COM_PLL_ANALOG                   (HDMI_PLL_BASE_OFFSET + 0x180)
#define QSERDES_COM_CORECLK_DIV                  (HDMI_PLL_BASE_OFFSET + 0x184)
#define QSERDES_COM_SW_RESET                     (HDMI_PLL_BASE_OFFSET + 0x188)
#define QSERDES_COM_CORE_CLK_EN                  (HDMI_PLL_BASE_OFFSET + 0x18C)
#define QSERDES_COM_C_READY_STATUS               (HDMI_PLL_BASE_OFFSET + 0x190)
#define QSERDES_COM_CMN_CONFIG                   (HDMI_PLL_BASE_OFFSET + 0x194)
#define QSERDES_COM_CMN_RATE_OVERRIDE            (HDMI_PLL_BASE_OFFSET + 0x198)
#define QSERDES_COM_SVS_MODE_CLK_SEL             (HDMI_PLL_BASE_OFFSET + 0x19C)
#define QSERDES_COM_DEBUG_BUS0                   (HDMI_PLL_BASE_OFFSET + 0x1A0)
#define QSERDES_COM_DEBUG_BUS1                   (HDMI_PLL_BASE_OFFSET + 0x1A4)
#define QSERDES_COM_DEBUG_BUS2                   (HDMI_PLL_BASE_OFFSET + 0x1A8)
#define QSERDES_COM_DEBUG_BUS3                   (HDMI_PLL_BASE_OFFSET + 0x1AC)
#define QSERDES_COM_DEBUG_BUS_SEL                (HDMI_PLL_BASE_OFFSET + 0x1B0)
#define QSERDES_COM_CMN_MISC1                    (HDMI_PLL_BASE_OFFSET + 0x1B4)
#define QSERDES_COM_CMN_MISC2                    (HDMI_PLL_BASE_OFFSET + 0x1B8)
#define QSERDES_COM_CORECLK_DIV_MODE1            (HDMI_PLL_BASE_OFFSET + 0x1BC)
#define QSERDES_COM_CORECLK_DIV_MODE2            (HDMI_PLL_BASE_OFFSET + 0x1C0)
#define QSERDES_COM_CMN_RSVD5                    (HDMI_PLL_BASE_OFFSET + 0x1C0)

/* Tx Channel base addresses */
#define HDMI_TX_L0_BASE_OFFSET                   (HDMI_PLL_BASE_OFFSET + 0x400)
#define HDMI_TX_L1_BASE_OFFSET                   (HDMI_PLL_BASE_OFFSET + 0x600)
#define HDMI_TX_L2_BASE_OFFSET                   (HDMI_PLL_BASE_OFFSET + 0x800)
#define HDMI_TX_L3_BASE_OFFSET                   (HDMI_PLL_BASE_OFFSET + 0xA00)

/* Tx Channel PHY registers */
#define QSERDES_TX_L0_BIST_MODE_LANENO                    (0x000)
#define QSERDES_TX_L0_BIST_INVERT                         (0x004)
#define QSERDES_TX_L0_CLKBUF_ENABLE                       (0x008)
#define QSERDES_TX_L0_CMN_CONTROL_ONE                     (0x00C)
#define QSERDES_TX_L0_CMN_CONTROL_TWO                     (0x010)
#define QSERDES_TX_L0_CMN_CONTROL_THREE                   (0x014)
#define QSERDES_TX_L0_TX_EMP_POST1_LVL                    (0x018)
#define QSERDES_TX_L0_TX_POST2_EMPH                       (0x01C)
#define QSERDES_TX_L0_TX_BOOST_LVL_UP_DN                  (0x020)
#define QSERDES_TX_L0_HP_PD_ENABLES                       (0x024)
#define QSERDES_TX_L0_TX_IDLE_LVL_LARGE_AMP               (0x028)
#define QSERDES_TX_L0_TX_DRV_LVL                          (0x02C)
#define QSERDES_TX_L0_TX_DRV_LVL_OFFSET                   (0x030)
#define QSERDES_TX_L0_RESET_TSYNC_EN                      (0x034)
#define QSERDES_TX_L0_PRE_STALL_LDO_BOOST_EN              (0x038)
#define QSERDES_TX_L0_TX_BAND                             (0x03C)
#define QSERDES_TX_L0_SLEW_CNTL                           (0x040)
#define QSERDES_TX_L0_INTERFACE_SELECT                    (0x044)
#define QSERDES_TX_L0_LPB_EN                              (0x048)
#define QSERDES_TX_L0_RES_CODE_LANE_TX                    (0x04C)
#define QSERDES_TX_L0_RES_CODE_LANE_RX                    (0x050)
#define QSERDES_TX_L0_RES_CODE_LANE_OFFSET                (0x054)
#define QSERDES_TX_L0_PERL_LENGTH1                        (0x058)
#define QSERDES_TX_L0_PERL_LENGTH2                        (0x05C)
#define QSERDES_TX_L0_SERDES_BYP_EN_OUT                   (0x060)
#define QSERDES_TX_L0_DEBUG_BUS_SEL                       (0x064)
#define QSERDES_TX_L0_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN    (0x068)
#define QSERDES_TX_L0_TX_POL_INV                          (0x06C)
#define QSERDES_TX_L0_PARRATE_REC_DETECT_IDLE_EN          (0x070)
#define QSERDES_TX_L0_BIST_PATTERN1                       (0x074)
#define QSERDES_TX_L0_BIST_PATTERN2                       (0x078)
#define QSERDES_TX_L0_BIST_PATTERN3                       (0x07C)
#define QSERDES_TX_L0_BIST_PATTERN4                       (0x080)
#define QSERDES_TX_L0_BIST_PATTERN5                       (0x084)
#define QSERDES_TX_L0_BIST_PATTERN6                       (0x088)
#define QSERDES_TX_L0_BIST_PATTERN7                       (0x08C)
#define QSERDES_TX_L0_BIST_PATTERN8                       (0x090)
#define QSERDES_TX_L0_LANE_MODE                           (0x094)
#define QSERDES_TX_L0_IDAC_CAL_LANE_MODE                  (0x098)
#define QSERDES_TX_L0_IDAC_CAL_LANE_MODE_CONFIGURATION    (0x09C)
#define QSERDES_TX_L0_ATB_SEL1                            (0x0A0)
#define QSERDES_TX_L0_ATB_SEL2                            (0x0A4)
#define QSERDES_TX_L0_RCV_DETECT_LVL                      (0x0A8)
#define QSERDES_TX_L0_RCV_DETECT_LVL_2                    (0x0AC)
#define QSERDES_TX_L0_PRBS_SEED1                          (0x0B0)
#define QSERDES_TX_L0_PRBS_SEED2                          (0x0B4)
#define QSERDES_TX_L0_PRBS_SEED3                          (0x0B8)
#define QSERDES_TX_L0_PRBS_SEED4                          (0x0BC)
#define QSERDES_TX_L0_RESET_GEN                           (0x0C0)
#define QSERDES_TX_L0_RESET_GEN_MUXES                     (0x0C4)
#define QSERDES_TX_L0_TRAN_DRVR_EMP_EN                    (0x0C8)
#define QSERDES_TX_L0_TX_INTERFACE_MODE                   (0x0CC)
#define QSERDES_TX_L0_PWM_CTRL                            (0x0D0)
#define QSERDES_TX_L0_PWM_ENCODED_OR_DATA                 (0x0D4)
#define QSERDES_TX_L0_PWM_GEAR_1_DIVIDER_BAND2            (0x0D8)
#define QSERDES_TX_L0_PWM_GEAR_2_DIVIDER_BAND2            (0x0DC)
#define QSERDES_TX_L0_PWM_GEAR_3_DIVIDER_BAND2            (0x0E0)
#define QSERDES_TX_L0_PWM_GEAR_4_DIVIDER_BAND2            (0x0E4)
#define QSERDES_TX_L0_PWM_GEAR_1_DIVIDER_BAND0_1          (0x0E8)
#define QSERDES_TX_L0_PWM_GEAR_2_DIVIDER_BAND0_1          (0x0EC)
#define QSERDES_TX_L0_PWM_GEAR_3_DIVIDER_BAND0_1          (0x0F0)
#define QSERDES_TX_L0_PWM_GEAR_4_DIVIDER_BAND0_1          (0x0F4)
#define QSERDES_TX_L0_VMODE_CTRL1                         (0x0F8)
#define QSERDES_TX_L0_VMODE_CTRL2                         (0x0FC)
#define QSERDES_TX_L0_TX_ALOG_INTF_OBSV_CNTL              (0x100)
#define QSERDES_TX_L0_BIST_STATUS                         (0x104)
#define QSERDES_TX_L0_BIST_ERROR_COUNT1                   (0x108)
#define QSERDES_TX_L0_BIST_ERROR_COUNT2                   (0x10C)
#define QSERDES_TX_L0_TX_ALOG_INTF_OBSV                   (0x110)

/* HDMI PHY REGISTERS */
#define HDMI_PHY_BASE_OFFSET                  (0x9A1200)
#define HDMI_PHY_CFG                          (HDMI_PHY_BASE_OFFSET + 0x00)
#define HDMI_PHY_PD_CTL                       (HDMI_PHY_BASE_OFFSET + 0x04)
#define HDMI_PHY_MODE                         (HDMI_PHY_BASE_OFFSET + 0x08)
#define HDMI_PHY_MISR_CLEAR                   (HDMI_PHY_BASE_OFFSET + 0x0C)
#define HDMI_PHY_TX0_TX1_BIST_CFG0            (HDMI_PHY_BASE_OFFSET + 0x10)
#define HDMI_PHY_TX0_TX1_BIST_CFG1            (HDMI_PHY_BASE_OFFSET + 0x14)
#define HDMI_PHY_TX0_TX1_PRBS_SEED_BYTE0      (HDMI_PHY_BASE_OFFSET + 0x18)
#define HDMI_PHY_TX0_TX1_PRBS_SEED_BYTE1      (HDMI_PHY_BASE_OFFSET + 0x1C)
#define HDMI_PHY_TX0_TX1_BIST_PATTERN0        (HDMI_PHY_BASE_OFFSET + 0x20)
#define HDMI_PHY_TX0_TX1_BIST_PATTERN1        (HDMI_PHY_BASE_OFFSET + 0x24)
#define HDMI_PHY_TX2_TX3_BIST_CFG0            (HDMI_PHY_BASE_OFFSET + 0x28)
#define HDMI_PHY_TX2_TX3_BIST_CFG1            (HDMI_PHY_BASE_OFFSET + 0x2C)
#define HDMI_PHY_TX2_TX3_PRBS_SEED_BYTE0      (HDMI_PHY_BASE_OFFSET + 0x30)
#define HDMI_PHY_TX2_TX3_PRBS_SEED_BYTE1      (HDMI_PHY_BASE_OFFSET + 0x34)
#define HDMI_PHY_TX2_TX3_BIST_PATTERN0        (HDMI_PHY_BASE_OFFSET + 0x38)
#define HDMI_PHY_TX2_TX3_BIST_PATTERN1        (HDMI_PHY_BASE_OFFSET + 0x3C)
#define HDMI_PHY_DEBUG_BUS_SEL                (HDMI_PHY_BASE_OFFSET + 0x40)
#define HDMI_PHY_TXCAL_CFG0                   (HDMI_PHY_BASE_OFFSET + 0x44)
#define HDMI_PHY_TXCAL_CFG1                   (HDMI_PHY_BASE_OFFSET + 0x48)
#define HDMI_PHY_TX0_TX1_LANE_CTL             (HDMI_PHY_BASE_OFFSET + 0x4C)
#define HDMI_PHY_TX2_TX3_LANE_CTL             (HDMI_PHY_BASE_OFFSET + 0x50)
#define HDMI_PHY_LANE_BIST_CONFIG             (HDMI_PHY_BASE_OFFSET + 0x54)
#define HDMI_PHY_CLOCK                        (HDMI_PHY_BASE_OFFSET + 0x58)
#define HDMI_PHY_MISC1                        (HDMI_PHY_BASE_OFFSET + 0x5C)
#define HDMI_PHY_MISC2                        (HDMI_PHY_BASE_OFFSET + 0x60)
#define HDMI_PHY_TX0_TX1_BIST_STATUS0         (HDMI_PHY_BASE_OFFSET + 0x64)
#define HDMI_PHY_TX0_TX1_BIST_STATUS1         (HDMI_PHY_BASE_OFFSET + 0x68)
#define HDMI_PHY_TX0_TX1_BIST_STATUS2         (HDMI_PHY_BASE_OFFSET + 0x6C)
#define HDMI_PHY_TX2_TX3_BIST_STATUS0         (HDMI_PHY_BASE_OFFSET + 0x70)
#define HDMI_PHY_TX2_TX3_BIST_STATUS1         (HDMI_PHY_BASE_OFFSET + 0x74)
#define HDMI_PHY_TX2_TX3_BIST_STATUS2         (HDMI_PHY_BASE_OFFSET + 0x78)
#define HDMI_PHY_PRE_MISR_STATUS0             (HDMI_PHY_BASE_OFFSET + 0x7C)
#define HDMI_PHY_PRE_MISR_STATUS1             (HDMI_PHY_BASE_OFFSET + 0x80)
#define HDMI_PHY_PRE_MISR_STATUS2             (HDMI_PHY_BASE_OFFSET + 0x84)
#define HDMI_PHY_PRE_MISR_STATUS3             (HDMI_PHY_BASE_OFFSET + 0x88)
#define HDMI_PHY_POST_MISR_STATUS0            (HDMI_PHY_BASE_OFFSET + 0x8C)
#define HDMI_PHY_POST_MISR_STATUS1            (HDMI_PHY_BASE_OFFSET + 0x90)
#define HDMI_PHY_POST_MISR_STATUS2            (HDMI_PHY_BASE_OFFSET + 0x94)
#define HDMI_PHY_POST_MISR_STATUS3            (HDMI_PHY_BASE_OFFSET + 0x98)
#define HDMI_PHY_STATUS                       (HDMI_PHY_BASE_OFFSET + 0x9C)
#define HDMI_PHY_MISC3_STATUS                 (HDMI_PHY_BASE_OFFSET + 0xA0)
#define HDMI_PHY_MISC4_STATUS                 (HDMI_PHY_BASE_OFFSET + 0xA4)
#define HDMI_PHY_DEBUG_BUS0                   (HDMI_PHY_BASE_OFFSET + 0xA8)
#define HDMI_PHY_DEBUG_BUS1                   (HDMI_PHY_BASE_OFFSET + 0xAC)
#define HDMI_PHY_DEBUG_BUS2                   (HDMI_PHY_BASE_OFFSET + 0xB0)
#define HDMI_PHY_DEBUG_BUS3                   (HDMI_PHY_BASE_OFFSET + 0xB4)
#define HDMI_PHY_PHY_REVISION_ID0             (HDMI_PHY_BASE_OFFSET + 0xB8)
#define HDMI_PHY_PHY_REVISION_ID1             (HDMI_PHY_BASE_OFFSET + 0xBC)
#define HDMI_PHY_PHY_REVISION_ID2             (HDMI_PHY_BASE_OFFSET + 0xC0)
#define HDMI_PHY_PHY_REVISION_ID3             (HDMI_PHY_BASE_OFFSET + 0xC4)

#define HDMI_PLL_POLL_MAX_READS                2500
#define HDMI_PLL_POLL_TIMEOUT_US               150000
#define HDMI_CLK_RATE_148_MHZ		       148500000
#define HDMI_CLK_RATE_74_MHZ		       74250000
#define HDMI_CLK_RATE_25_MHZ		       25200000
#define HDMI_CLK_RATE_297_MHZ		       297000000
#define HDMI_CLK_RATE_594_MHZ		       594000000

#define SW_RESET BIT(2)
#define SW_RESET_PLL BIT(0)

struct hdmi_8996_phy_pll_reg_cfg {
	uint32_t tx_l0_lane_mode;
	uint32_t tx_l2_lane_mode;
	uint32_t tx_l0_tx_band;
	uint32_t tx_l1_tx_band;
	uint32_t tx_l2_tx_band;
	uint32_t tx_l3_tx_band;
	uint32_t com_svs_mode_clk_sel;
	uint32_t com_hsclk_sel;
	uint32_t com_pll_cctrl_mode0;
	uint32_t com_pll_rctrl_mode0;
	uint32_t com_cp_ctrl_mode0;
	uint32_t com_dec_start_mode0;
	uint32_t com_div_frac_start1_mode0;
	uint32_t com_div_frac_start2_mode0;
	uint32_t com_div_frac_start3_mode0;
	uint32_t com_integloop_gain0_mode0;
	uint32_t com_integloop_gain1_mode0;
	uint32_t com_lock_cmp_en;
	uint32_t com_lock_cmp1_mode0;
	uint32_t com_lock_cmp2_mode0;
	uint32_t com_lock_cmp3_mode0;
	uint32_t com_core_clk_en;
	uint32_t com_coreclk_div;
	uint32_t com_restrim_ctrl;
	uint32_t com_vco_tune_ctrl;
	uint32_t tx_l0_tx_drv_lvl;
	uint32_t tx_l0_tx_emp_post1_lvl;
	uint32_t tx_l1_tx_drv_lvl;
	uint32_t tx_l1_tx_emp_post1_lvl;
	uint32_t tx_l2_tx_drv_lvl;
	uint32_t tx_l2_tx_emp_post1_lvl;
	uint32_t tx_l3_tx_drv_lvl;
	uint32_t tx_l3_tx_emp_post1_lvl;
	uint32_t tx_l0_vmode_ctrl1;
	uint32_t tx_l0_vmode_ctrl2;
	uint32_t tx_l1_vmode_ctrl1;
	uint32_t tx_l1_vmode_ctrl2;
	uint32_t tx_l2_vmode_ctrl1;
	uint32_t tx_l2_vmode_ctrl2;
	uint32_t tx_l3_vmode_ctrl1;
	uint32_t tx_l3_vmode_ctrl2;
	uint32_t tx_l0_res_code_lane_tx;
	uint32_t tx_l1_res_code_lane_tx;
	uint32_t tx_l2_res_code_lane_tx;
	uint32_t tx_l3_res_code_lane_tx;
	uint32_t phy_mode;
};

void hdmi_phy_reset(void)
{
	uint32_t phy_reset_polarity = 0x0;
	uint32_t pll_reset_polarity = 0x0;
	uint32_t val;

	val = readl(HDMI_PHY_CTRL);

	phy_reset_polarity = val >> 3 & 0x1;
	pll_reset_polarity = val >> 1 & 0x1;

	if (phy_reset_polarity == 0)
		writel(val | SW_RESET, HDMI_PHY_CTRL);
	else
		writel(val & (~SW_RESET), HDMI_PHY_CTRL);

	if (pll_reset_polarity == 0)
		writel(val | SW_RESET_PLL, HDMI_PHY_CTRL);
	else
		writel(val & (~SW_RESET_PLL), HDMI_PHY_CTRL);

	if (phy_reset_polarity == 0)
		writel(val & (~SW_RESET), HDMI_PHY_CTRL);
	else
		writel(val | SW_RESET, HDMI_PHY_CTRL);

	if (pll_reset_polarity == 0)
		writel(val & (~SW_RESET_PLL), HDMI_PHY_CTRL);
	else
		writel(val | SW_RESET_PLL, HDMI_PHY_CTRL);

	udelay(100);
}

static int get_pll_settings(uint32_t tmds_clk_rate,
		struct hdmi_8996_phy_pll_reg_cfg *cfg)
{
	switch (tmds_clk_rate) {
	case HDMI_CLK_RATE_148_MHZ:
		cfg->tx_l0_lane_mode = 0x43;
		cfg->tx_l2_lane_mode = 0x43;
		cfg->tx_l0_tx_band = 0x04;
		cfg->tx_l1_tx_band = 0x04;
		cfg->tx_l2_tx_band = 0x04;
		cfg->tx_l3_tx_band = 0x04;
		cfg->com_svs_mode_clk_sel = 0x2;
		cfg->com_hsclk_sel = 0x21;
		cfg->com_pll_cctrl_mode0 = 0x28;
		cfg->com_pll_rctrl_mode0 = 0x16;
		cfg->com_cp_ctrl_mode0 = 0xb;
		cfg->com_dec_start_mode0 = 0x74;
		cfg->com_div_frac_start1_mode0 = 0x0;
		cfg->com_div_frac_start2_mode0 = 0x40;
		cfg->com_div_frac_start3_mode0 = 0x0;
		cfg->com_integloop_gain0_mode0 = 0x0;
		cfg->com_integloop_gain1_mode0 = 0x1;
		cfg->com_lock_cmp_en = 0x0;
		cfg->com_lock_cmp1_mode0 = 0xef;
		cfg->com_lock_cmp2_mode0 = 0x1e;
		cfg->com_lock_cmp3_mode0 = 0x0;
		cfg->com_core_clk_en = 0x2c;
		cfg->com_coreclk_div = 0x5;
		cfg->com_restrim_ctrl = 0x0;
		cfg->com_vco_tune_ctrl = 0x0;
		cfg->tx_l0_tx_drv_lvl = 0x25;
		cfg->tx_l0_tx_emp_post1_lvl = 0x23;
		cfg->tx_l1_tx_drv_lvl = 0x25;
		cfg->tx_l1_tx_emp_post1_lvl = 0x23;
		cfg->tx_l2_tx_drv_lvl = 0x25;
		cfg->tx_l2_tx_emp_post1_lvl = 0x23;
		cfg->tx_l3_tx_drv_lvl = 0x25;
		cfg->tx_l3_tx_emp_post1_lvl = 0x23;
		cfg->tx_l0_vmode_ctrl1 = 0x0;
		cfg->tx_l0_vmode_ctrl2 = 0xd;
		cfg->tx_l1_vmode_ctrl1 = 0x0;
		cfg->tx_l1_vmode_ctrl2 = 0xd;
		cfg->tx_l2_vmode_ctrl1 = 0x0;
		cfg->tx_l2_vmode_ctrl2 = 0xd;
		cfg->tx_l3_vmode_ctrl1 = 0x0;
		cfg->tx_l3_vmode_ctrl2 = 0x0;
		cfg->tx_l0_res_code_lane_tx = 0x0;
		cfg->tx_l1_res_code_lane_tx = 0x0;
		cfg->tx_l2_res_code_lane_tx = 0x0;
		cfg->tx_l3_res_code_lane_tx = 0x0;
		cfg->phy_mode = 0x0;
		break;
	case HDMI_CLK_RATE_74_MHZ:
		cfg->tx_l0_lane_mode = 0x43;
		cfg->tx_l2_lane_mode = 0x43;
		cfg->tx_l0_tx_band = 0x04;
		cfg->tx_l1_tx_band = 0x04;
		cfg->tx_l2_tx_band = 0x04;
		cfg->tx_l3_tx_band = 0x04;
		cfg->com_svs_mode_clk_sel = 0x2;
		cfg->com_hsclk_sel = 0x29;
		cfg->com_pll_cctrl_mode0 = 0x28;
		cfg->com_pll_rctrl_mode0 = 0x16;
		cfg->com_cp_ctrl_mode0 = 0xb;
		cfg->com_dec_start_mode0 = 0x74;
		cfg->com_div_frac_start1_mode0 = 0x0;
		cfg->com_div_frac_start2_mode0 = 0x40;
		cfg->com_div_frac_start3_mode0 = 0x0;
		cfg->com_integloop_gain0_mode0 = 0x0;
		cfg->com_integloop_gain1_mode0 = 0x1;
		cfg->com_lock_cmp_en = 0x0;
		cfg->com_lock_cmp1_mode0 = 0x77;
		cfg->com_lock_cmp2_mode0 = 0xf;
		cfg->com_lock_cmp3_mode0 = 0x0;
		cfg->com_core_clk_en = 0x2c;
		cfg->com_coreclk_div = 0x5;
		cfg->com_restrim_ctrl = 0x0;
		cfg->com_vco_tune_ctrl = 0x0;
		cfg->tx_l0_tx_drv_lvl = 0x20;
		cfg->tx_l0_tx_emp_post1_lvl = 0x20;
		cfg->tx_l1_tx_drv_lvl = 0x20;
		cfg->tx_l1_tx_emp_post1_lvl = 0x20;
		cfg->tx_l2_tx_drv_lvl = 0x20;
		cfg->tx_l2_tx_emp_post1_lvl = 0x20;
		cfg->tx_l3_tx_drv_lvl = 0x20;
		cfg->tx_l3_tx_emp_post1_lvl = 0x20;
		cfg->tx_l0_vmode_ctrl1 = 0x0;
		cfg->tx_l0_vmode_ctrl2 = 0xe;
		cfg->tx_l1_vmode_ctrl1 = 0x0;
		cfg->tx_l1_vmode_ctrl2 = 0xe;
		cfg->tx_l2_vmode_ctrl1 = 0x0;
		cfg->tx_l2_vmode_ctrl2 = 0xe;
		cfg->tx_l3_vmode_ctrl1 = 0x0;
		cfg->tx_l3_vmode_ctrl2 = 0xe;
		cfg->tx_l0_res_code_lane_tx = 0x0;
		cfg->tx_l1_res_code_lane_tx = 0x0;
		cfg->tx_l2_res_code_lane_tx = 0x0;
		cfg->tx_l3_res_code_lane_tx = 0x0;
		cfg->phy_mode = 0x0;
		break;
	case HDMI_CLK_RATE_25_MHZ:
		cfg->tx_l0_lane_mode = 0x43;
		cfg->tx_l2_lane_mode = 0x43;
		cfg->tx_l0_tx_band = 0x7;
		cfg->tx_l1_tx_band = 0x7;
		cfg->tx_l2_tx_band = 0x7;
		cfg->tx_l3_tx_band = 0x7;
		cfg->com_svs_mode_clk_sel = 0x2;
		cfg->com_hsclk_sel = 0x28;
		cfg->com_pll_cctrl_mode0 = 0x1;
		cfg->com_pll_rctrl_mode0 = 0x10;
		cfg->com_cp_ctrl_mode0 = 0x23;
		cfg->com_dec_start_mode0 = 0x69;
		cfg->com_div_frac_start1_mode0 = 0x0;
		cfg->com_div_frac_start2_mode0 = 0x0;
		cfg->com_div_frac_start3_mode0 = 0x0;
		cfg->com_integloop_gain0_mode0 = 0x10;
		cfg->com_integloop_gain1_mode0 = 0x3;
		cfg->com_lock_cmp_en = 0x0;
		cfg->com_lock_cmp1_mode0 = 0xff;
		cfg->com_lock_cmp2_mode0 = 0x29;
		cfg->com_lock_cmp3_mode0 = 0x0;
		cfg->com_core_clk_en = 0x2c;
		cfg->com_coreclk_div = 0x5;
		cfg->com_restrim_ctrl = 0x0;
		cfg->com_vco_tune_ctrl = 0x0;
		cfg->tx_l0_tx_drv_lvl = 0x20;
		cfg->tx_l0_tx_emp_post1_lvl = 0x20;
		cfg->tx_l1_tx_drv_lvl = 0x20;
		cfg->tx_l1_tx_emp_post1_lvl = 0x20;
		cfg->tx_l2_tx_drv_lvl = 0x20;
		cfg->tx_l2_tx_emp_post1_lvl = 0x20;
		cfg->tx_l3_tx_drv_lvl = 0x20;
		cfg->tx_l3_tx_emp_post1_lvl = 0x20;
		cfg->tx_l0_vmode_ctrl1 = 0x0;
		cfg->tx_l0_vmode_ctrl2 = 0xe;
		cfg->tx_l1_vmode_ctrl1 = 0x0;
		cfg->tx_l1_vmode_ctrl2 = 0xe;
		cfg->tx_l2_vmode_ctrl1 = 0x0;
		cfg->tx_l2_vmode_ctrl2 = 0xe;
		cfg->tx_l3_vmode_ctrl1 = 0x0;
		cfg->tx_l3_vmode_ctrl2 = 0xe;
		cfg->tx_l0_res_code_lane_tx = 0x0;
		cfg->tx_l1_res_code_lane_tx = 0x0;
		cfg->tx_l2_res_code_lane_tx = 0x0;
		cfg->tx_l3_res_code_lane_tx = 0x0;
		cfg->phy_mode = 0x0;
		break;
	case HDMI_CLK_RATE_297_MHZ:
		cfg->tx_l0_lane_mode = 0x43;
		cfg->tx_l2_lane_mode = 0x43;
		cfg->tx_l0_tx_band = 0x4;
		cfg->tx_l1_tx_band = 0x4;
		cfg->tx_l2_tx_band = 0x4;
		cfg->tx_l3_tx_band = 0x4;
		cfg->com_svs_mode_clk_sel = 0x1;
		cfg->com_hsclk_sel = 0x24;
		cfg->com_pll_cctrl_mode0 = 0x28;
		cfg->com_pll_rctrl_mode0 = 0x16;
		cfg->com_cp_ctrl_mode0 = 0xb;
		cfg->com_dec_start_mode0 = 0x74;
		cfg->com_div_frac_start1_mode0 = 0x0;
		cfg->com_div_frac_start2_mode0 = 0x40;
		cfg->com_div_frac_start3_mode0 = 0x0;
		cfg->com_integloop_gain0_mode0 = 0x80;
		cfg->com_integloop_gain1_mode0 = 0x0;
		cfg->com_lock_cmp_en = 0x0;
		cfg->com_lock_cmp1_mode0 = 0xdf;
		cfg->com_lock_cmp2_mode0 = 0x3d;
		cfg->com_lock_cmp3_mode0 = 0x0;
		cfg->com_core_clk_en = 0x2c;
		cfg->com_coreclk_div = 0x5;
		cfg->com_restrim_ctrl = 0x0;
		cfg->com_vco_tune_ctrl = 0x0;
		cfg->tx_l0_tx_drv_lvl = 0x25;
		cfg->tx_l0_tx_emp_post1_lvl = 0x23;
		cfg->tx_l1_tx_drv_lvl = 0x25;
		cfg->tx_l1_tx_emp_post1_lvl = 0x23;
		cfg->tx_l2_tx_drv_lvl = 0x25;
		cfg->tx_l2_tx_emp_post1_lvl = 0x23;
		cfg->tx_l3_tx_drv_lvl = 0x25;
		cfg->tx_l3_tx_emp_post1_lvl = 0x23;
		cfg->tx_l0_vmode_ctrl1 = 0x0;
		cfg->tx_l0_vmode_ctrl2 = 0xd;
		cfg->tx_l1_vmode_ctrl1 = 0x0;
		cfg->tx_l1_vmode_ctrl2 = 0xd;
		cfg->tx_l2_vmode_ctrl1 = 0x0;
		cfg->tx_l2_vmode_ctrl2 = 0xd;
		cfg->tx_l3_vmode_ctrl1 = 0x0;
		cfg->tx_l3_vmode_ctrl2 = 0x0;
		cfg->tx_l0_res_code_lane_tx = 0x0;
		cfg->tx_l1_res_code_lane_tx = 0x0;
		cfg->tx_l2_res_code_lane_tx = 0x0;
		cfg->tx_l3_res_code_lane_tx = 0x0;
		cfg->phy_mode = 0x00;
		break;
	case HDMI_CLK_RATE_594_MHZ:
		cfg->tx_l0_lane_mode = 0x43;
		cfg->tx_l2_lane_mode = 0x43;
		cfg->tx_l0_tx_band = 0x4;
		cfg->tx_l1_tx_band = 0x4;
		cfg->tx_l2_tx_band = 0x4;
		cfg->tx_l3_tx_band = 0x4;
		cfg->com_svs_mode_clk_sel = 0x1;
		cfg->com_hsclk_sel = 0x20;
		cfg->com_pll_cctrl_mode0 = 0x28;
		cfg->com_pll_rctrl_mode0 = 0x16;
		cfg->com_cp_ctrl_mode0 = 0xb;
		cfg->com_dec_start_mode0 = 0x9a;
		cfg->com_div_frac_start1_mode0 = 0x0;
		cfg->com_div_frac_start2_mode0 = 0x0;
		cfg->com_div_frac_start3_mode0 = 0xb;
		cfg->com_integloop_gain0_mode0 = 0x80;
		cfg->com_integloop_gain1_mode0 = 0x0;
		cfg->com_lock_cmp_en = 0x0;
		cfg->com_lock_cmp1_mode0 = 0xbf;
		cfg->com_lock_cmp2_mode0 = 0x7b;
		cfg->com_lock_cmp3_mode0 = 0x0;
		cfg->com_core_clk_en = 0x2c;
		cfg->com_coreclk_div = 0x5;
		cfg->com_restrim_ctrl = 0x0;
		cfg->com_vco_tune_ctrl = 0x0;
		cfg->tx_l0_tx_drv_lvl = 0x25;
		cfg->tx_l0_tx_emp_post1_lvl = 0x23;
		cfg->tx_l1_tx_drv_lvl = 0x25;
		cfg->tx_l1_tx_emp_post1_lvl = 0x23;
		cfg->tx_l2_tx_drv_lvl = 0x25;
		cfg->tx_l2_tx_emp_post1_lvl = 0x23;
		cfg->tx_l3_tx_drv_lvl = 0x22;
		cfg->tx_l3_tx_emp_post1_lvl = 0x27;
		cfg->tx_l0_vmode_ctrl1 = 0x0;
		cfg->tx_l0_vmode_ctrl2 = 0xd;
		cfg->tx_l1_vmode_ctrl1 = 0x0;
		cfg->tx_l1_vmode_ctrl2 = 0xd;
		cfg->tx_l2_vmode_ctrl1 = 0x0;
		cfg->tx_l2_vmode_ctrl2 = 0xd;
		cfg->tx_l3_vmode_ctrl1 = 0x0;
		cfg->tx_l3_vmode_ctrl2 = 0x0;
		cfg->tx_l0_res_code_lane_tx = 0x0;
		cfg->tx_l1_res_code_lane_tx = 0x0;
		cfg->tx_l2_res_code_lane_tx = 0x0;
		cfg->tx_l3_res_code_lane_tx = 0x0;
		cfg->phy_mode = 0x10;
		break;
	default:
		return ERROR;
	}

	return NO_ERROR;

}

uint32_t hdmi_pll_config(uint32_t tmds_clk_rate)
{
	struct hdmi_8996_phy_pll_reg_cfg cfg = {0};
	int rc = NO_ERROR;

	rc = get_pll_settings(tmds_clk_rate, &cfg);
	if (rc) {
		dprintf(CRITICAL, "%s: Unsupported clock rate %u\n", __func__, tmds_clk_rate);
		return rc;
	}

	/* Initially shut down PHY */
	writel(0x0, HDMI_PHY_PD_CTL);
	udelay(500);

	/* Power up sequence */
	writel(0x04, QSERDES_COM_BG_CTRL);

	writel(0x1, HDMI_PHY_PD_CTL);
	writel(0x20, QSERDES_COM_RESETSM_CNTRL);
	writel(0x0F, HDMI_PHY_TX0_TX1_LANE_CTL);
	writel(0x0F, HDMI_PHY_TX2_TX3_LANE_CTL);
	writel(0x03, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_CLKBUF_ENABLE);
	writel(0x03, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_CLKBUF_ENABLE);
	writel(0x03, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_CLKBUF_ENABLE);
	writel(0x03, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_CLKBUF_ENABLE);

	writel(cfg.tx_l0_lane_mode, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_LANE_MODE);
	writel(cfg.tx_l2_lane_mode, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_LANE_MODE);

	writel(cfg.tx_l0_tx_band, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_TX_BAND);
	writel(cfg.tx_l1_tx_band, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_TX_BAND);
	writel(cfg.tx_l2_tx_band, HDMI_TX_L2_BASE_OFFSET +
		    QSERDES_TX_L0_TX_BAND);
	writel(cfg.tx_l3_tx_band, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_TX_BAND);

	writel(0x03, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_RESET_TSYNC_EN);
	writel(0x03, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_RESET_TSYNC_EN);
	writel(0x03, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_RESET_TSYNC_EN);
	writel(0x03, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_RESET_TSYNC_EN);

	writel(0x1E, QSERDES_COM_SYSCLK_BUF_ENABLE);
	writel(0x07, QSERDES_COM_BIAS_EN_CLKBUFLR_EN);
	writel(0x37, QSERDES_COM_SYSCLK_EN_SEL);
	writel(0x02, QSERDES_COM_SYS_CLK_CTRL);
	writel(0x0E, QSERDES_COM_CLK_ENABLE1);

	/* Bypass VCO calibration */
	writel(cfg.com_svs_mode_clk_sel, QSERDES_COM_SVS_MODE_CLK_SEL);

	writel(0x0F, QSERDES_COM_BG_TRIM);
	writel(0x0F, QSERDES_COM_PLL_IVCO);
	writel(cfg.com_vco_tune_ctrl, QSERDES_COM_VCO_TUNE_CTRL);

	writel(0x06, QSERDES_COM_BG_CTRL);

	writel(0x30, QSERDES_COM_CLK_SELECT);
	writel(cfg.com_hsclk_sel, QSERDES_COM_HSCLK_SEL);

	writel(cfg.com_lock_cmp_en, QSERDES_COM_LOCK_CMP_EN);

	writel(cfg.com_pll_cctrl_mode0, QSERDES_COM_PLL_CCTRL_MODE0);
	writel(cfg.com_pll_rctrl_mode0, QSERDES_COM_PLL_RCTRL_MODE0);
	writel(cfg.com_cp_ctrl_mode0, QSERDES_COM_CP_CTRL_MODE0);
	writel(cfg.com_dec_start_mode0, QSERDES_COM_DEC_START_MODE0);
	writel(cfg.com_div_frac_start1_mode0, QSERDES_COM_DIV_FRAC_START1_MODE0);
	writel(cfg.com_div_frac_start2_mode0, QSERDES_COM_DIV_FRAC_START2_MODE0);
	writel(cfg.com_div_frac_start3_mode0, QSERDES_COM_DIV_FRAC_START3_MODE0);

	writel(cfg.com_integloop_gain0_mode0, QSERDES_COM_INTEGLOOP_GAIN0_MODE0);
	writel(cfg.com_integloop_gain1_mode0, QSERDES_COM_INTEGLOOP_GAIN1_MODE0);

	writel(cfg.com_lock_cmp1_mode0, QSERDES_COM_LOCK_CMP1_MODE0);
	writel(cfg.com_lock_cmp2_mode0, QSERDES_COM_LOCK_CMP2_MODE0);
	writel(cfg.com_lock_cmp3_mode0, QSERDES_COM_LOCK_CMP3_MODE0);

	writel(0x00, QSERDES_COM_VCO_TUNE_MAP);
	writel(cfg.com_core_clk_en, QSERDES_COM_CORE_CLK_EN);
	writel(cfg.com_coreclk_div, QSERDES_COM_CORECLK_DIV);
	writel(0x02, QSERDES_COM_CMN_CONFIG);

	writel(0x15, QSERDES_COM_RESCODE_DIV_NUM);

	/* TX lanes setup (TX 0/1/2/3) */
	writel(cfg.tx_l0_tx_drv_lvl, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_TX_DRV_LVL);
	writel(cfg.tx_l0_tx_emp_post1_lvl, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_TX_EMP_POST1_LVL);

	writel(cfg.tx_l1_tx_drv_lvl, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_TX_DRV_LVL);
	writel(cfg.tx_l1_tx_emp_post1_lvl, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_TX_EMP_POST1_LVL);

	writel(cfg.tx_l2_tx_drv_lvl, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_TX_DRV_LVL);
	writel(cfg.tx_l2_tx_emp_post1_lvl, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_TX_EMP_POST1_LVL);

	writel(cfg.tx_l3_tx_drv_lvl, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_TX_DRV_LVL);
	writel(cfg.tx_l3_tx_emp_post1_lvl, HDMI_TX_L3_BASE_OFFSET +
		    QSERDES_TX_L0_TX_EMP_POST1_LVL);

	writel(cfg.tx_l0_vmode_ctrl1, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL1);
	writel(cfg.tx_l0_vmode_ctrl2, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL2);

	writel(cfg.tx_l1_vmode_ctrl1, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL1);
	writel(cfg.tx_l1_vmode_ctrl2, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL2);

	writel(cfg.tx_l2_vmode_ctrl1, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL1);
	writel(cfg.tx_l2_vmode_ctrl2, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL2);

	writel(cfg.tx_l3_vmode_ctrl1, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL1);
	writel(cfg.tx_l3_vmode_ctrl2, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_VMODE_CTRL2);

	writel(0x00, HDMI_TX_L0_BASE_OFFSET +
		       QSERDES_TX_L0_TX_DRV_LVL_OFFSET);
	writel(0x00, HDMI_TX_L1_BASE_OFFSET +
		       QSERDES_TX_L0_TX_DRV_LVL_OFFSET);
	writel(0x00, HDMI_TX_L2_BASE_OFFSET +
		       QSERDES_TX_L0_TX_DRV_LVL_OFFSET);
	writel(0x00, HDMI_TX_L3_BASE_OFFSET +
		       QSERDES_TX_L0_TX_DRV_LVL_OFFSET);

	writel(0x00, HDMI_TX_L0_BASE_OFFSET +
		       QSERDES_TX_L0_RES_CODE_LANE_OFFSET);
	writel(0x00, HDMI_TX_L1_BASE_OFFSET +
		       QSERDES_TX_L0_RES_CODE_LANE_OFFSET);
	writel(0x00, HDMI_TX_L2_BASE_OFFSET +
		       QSERDES_TX_L0_RES_CODE_LANE_OFFSET);
	writel(0x00, HDMI_TX_L3_BASE_OFFSET +
		       QSERDES_TX_L0_RES_CODE_LANE_OFFSET);

	writel(cfg.phy_mode, HDMI_PHY_MODE);
	writel(0x1F, HDMI_PHY_PD_CTL);

	writel(0x03, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_TRAN_DRVR_EMP_EN);
	writel(0x03, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_TRAN_DRVR_EMP_EN);
	writel(0x03, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_TRAN_DRVR_EMP_EN);
	writel(0x03, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_TRAN_DRVR_EMP_EN);

	writel(0x40, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_PARRATE_REC_DETECT_IDLE_EN);
	writel(0x40, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_PARRATE_REC_DETECT_IDLE_EN);
	writel(0x40, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_PARRATE_REC_DETECT_IDLE_EN);
	writel(0x40, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_PARRATE_REC_DETECT_IDLE_EN);

	writel(0x0C, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_HP_PD_ENABLES);
	writel(0x0C, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_HP_PD_ENABLES);
	writel(0x0C, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_HP_PD_ENABLES);
	writel(0x03, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_HP_PD_ENABLES);

	return NO_ERROR;
}

int hdmi_vco_enable(void)
{
	uint32_t pll_locked = 0;
	uint32_t phy_ready = 0;
	uint32_t status = 0;
	uint32_t num_reads = 0;

	writel(0x1, HDMI_PHY_CFG);
	udelay(100);

	writel(0x19, HDMI_PHY_CFG);
	udelay(100);

	num_reads = HDMI_PLL_POLL_MAX_READS;
	status = readl(QSERDES_COM_C_READY_STATUS);
	while (!(status & BIT(0)) && num_reads) {
		status = readl(QSERDES_COM_C_READY_STATUS);
		num_reads--;
		udelay(HDMI_PLL_POLL_TIMEOUT_US);
	}

	if ((status & BIT(0)) == 1 && num_reads) {
		pll_locked = 1;
	} else {
		pll_locked = 0;
	}

	dprintf(INFO, "%s: pll_locked = %d\n", __func__, pll_locked);

	writel(0x6F, HDMI_TX_L0_BASE_OFFSET +
			QSERDES_TX_L0_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN);
	writel(0x6F, HDMI_TX_L1_BASE_OFFSET +
			QSERDES_TX_L0_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN);
	writel(0x6F, HDMI_TX_L2_BASE_OFFSET +
			QSERDES_TX_L0_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN);
	writel(0x6F, HDMI_TX_L3_BASE_OFFSET +
			QSERDES_TX_L0_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN);

	/* Disable SSC */
	writel(0x0, QSERDES_COM_SSC_PER1);
	writel(0x0, QSERDES_COM_SSC_PER2);
	writel(0x0, QSERDES_COM_SSC_STEP_SIZE1);
	writel(0x0, QSERDES_COM_SSC_STEP_SIZE2);
	writel(0x2, QSERDES_COM_SSC_EN_CENTER);

	num_reads = HDMI_PLL_POLL_MAX_READS;
	status = readl(HDMI_PHY_STATUS);
	while (!(status & BIT(0)) && num_reads) {
		status = readl(HDMI_PHY_STATUS);
		num_reads--;
		udelay(HDMI_PLL_POLL_TIMEOUT_US);
	}

	if ((status & BIT(0)) == 1 && num_reads) {
		phy_ready = 1;
	} else {
		phy_ready = 0;
	}

	dprintf(INFO, "%s: phy_ready = %d\n", __func__, phy_ready);

	/* Restart the retiming buffer */
	writel(0x18, HDMI_PHY_CFG);
	udelay(1);
	writel(0x19, HDMI_PHY_CFG);

	return NO_ERROR;
}

int hdmi_vco_disable(void)
{
	writel(0x0, HDMI_PHY_PD_CTL);
	udelay(500);

	return NO_ERROR;
}
