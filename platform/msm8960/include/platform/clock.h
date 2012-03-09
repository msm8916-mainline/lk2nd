/*
 * * Copyright (c) 2011-2012, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
#ifndef __PLATFORM_MSM8960_CLOCK_H
#define __PLATFORM_MSM8960_CLOCK_H

#define UART_DM_CLK_RX_TX_BIT_RATE 0xFF

#define REG(off)        (MSM_CLK_CTL_BASE + (off))
#define REG_MM(off)     (MSM_MMSS_CLK_CTL_BASE + (off))
#define REG_LPA(off)    (MSM_LPASS_CLK_CTL_BASE + (off))

/* Peripheral clock registers. */
#define CE1_HCLK_CTL_REG                    REG(0x2720)
#define CE1_CORE_CLK_CTL_REG                REG(0x2724)
#define CE3_CLK_SRC_NS_REG                  REG(0x36C0)
#define CE3_HCLK_CTL_REG                    REG(0x36C4)
#define CE3_CORE_CLK_CTL_REG                REG(0x36CC)
#define DMA_BAM_HCLK_CTL                    REG(0x25C0)
#define CLK_HALT_AFAB_SFAB_STATEB_REG       REG(0x2FC4)
#define CLK_HALT_CFPB_STATEA_REG            REG(0x2FCC)
#define CLK_HALT_CFPB_STATEB_REG            REG(0x2FD0)
#define CLK_HALT_CFPB_STATEC_REG            REG(0x2FD4)
#define CLK_HALT_GSS_KPSS_MISC_STATE_REG    REG(0x2FDC)
#define CLK_HALT_DFAB_STATE_REG             REG(0x2FC8)
#define CLK_HALT_MSS_SMPSS_MISC_STATE_REG   REG(0x2FDC)
#define CLK_HALT_SFPB_MISC_STATE_REG        REG(0x2FD8)
#define CLK_TEST_REG                        REG(0x2FA0)
#define GSBIn_HCLK_CTL_REG(n)               REG(0x29C0+(0x20*((n)-1)))
#define GSBIn_QUP_APPS_MD_REG(n)            REG(0x29C8+(0x20*((n)-1)))
#define GSBIn_QUP_APPS_NS_REG(n)            REG(0x29CC+(0x20*((n)-1)))
#define GSBIn_RESET_REG(n)                  REG(0x29DC+(0x20*((n)-1)))
#define GSBIn_UART_APPS_MD_REG(n)           REG(0x29D0+(0x20*((n)-1)))
#define GSBIn_UART_APPS_NS_REG(n)           REG(0x29D4+(0x20*((n)-1)))
#define LPASS_XO_SRC_CLK_CTL_REG            REG(0x2EC0)
#define PDM_CLK_NS_REG                      REG(0x2CC0)
#define BB_PLL_ENA_Q6_SW_REG                REG(0x3500)
#define BB_PLL_ENA_SC0_REG                  REG(0x34C0)
#define BB_PLL0_STATUS_REG                  REG(0x30D8)
#define BB_PLL5_STATUS_REG                  REG(0x30F8)
#define BB_PLL6_STATUS_REG                  REG(0x3118)
#define BB_PLL7_STATUS_REG                  REG(0x3138)
#define BB_PLL8_L_VAL_REG                   REG(0x3144)
#define BB_PLL8_M_VAL_REG                   REG(0x3148)
#define BB_PLL8_MODE_REG                    REG(0x3140)
#define BB_PLL8_N_VAL_REG                   REG(0x314C)
#define BB_PLL8_STATUS_REG                  REG(0x3158)
#define BB_PLL8_CONFIG_REG                  REG(0x3154)
#define BB_PLL8_TEST_CTL_REG                REG(0x3150)
#define BB_PLL3_MODE_REG                    REG(0x3160)
#define PLLTEST_PAD_CFG_REG                 REG(0x2FA4)
#define PMEM_ACLK_CTL_REG                   REG(0x25A0)
#define RINGOSC_NS_REG                      REG(0x2DC0)
#define RINGOSC_STATUS_REG                  REG(0x2DCC)
#define RINGOSC_TCXO_CTL_REG                REG(0x2DC4)
#define SC0_U_CLK_BRANCH_ENA_VOTE_REG       REG(0x3080)
#define SDCn_APPS_CLK_MD_REG(n)             REG(0x2828+(0x20*((n)-1)))
#define SDCn_APPS_CLK_NS_REG(n)             REG(0x282C+(0x20*((n)-1)))
#define SDCn_HCLK_CTL_REG(n)                REG(0x2820+(0x20*((n)-1)))
#define SDCn_RESET_REG(n)                   REG(0x2830+(0x20*((n)-1)))
#define SLIMBUS_XO_SRC_CLK_CTL_REG          REG(0x2628)
#define TSIF_HCLK_CTL_REG                   REG(0x2700)
#define TSIF_REF_CLK_MD_REG                 REG(0x270C)
#define TSIF_REF_CLK_NS_REG                 REG(0x2710)
#define TSSC_CLK_CTL_REG                    REG(0x2CA0)
#define USB_FSn_HCLK_CTL_REG(n)             REG(0x2960+(0x20*((n)-1)))
#define USB_FSn_RESET_REG(n)                REG(0x2974+(0x20*((n)-1)))
#define USB_FSn_SYSTEM_CLK_CTL_REG(n)       REG(0x296C+(0x20*((n)-1)))
#define USB_FSn_XCVR_FS_CLK_MD_REG(n)       REG(0x2964+(0x20*((n)-1)))
#define USB_FSn_XCVR_FS_CLK_NS_REG(n)       REG(0x2968+(0x20*((n)-1)))
#define USB_HS1_HCLK_CTL_REG                REG(0x2900)
#define USB_HS1_RESET_REG                   REG(0x2910)
#define USB_HS1_XCVR_FS_CLK_MD_REG          REG(0x2908)
#define USB_HS1_XCVR_FS_CLK_NS_REG          REG(0x290C)
#define USB_PHY0_RESET_REG                  REG(0x2E20)


/* Multimedia clock registers. */
#define AHB_EN_REG                      REG_MM(0x0008)
#define AHB_EN2_REG                     REG_MM(0x0038)
#define AHB_NS_REG                      REG_MM(0x0004)
#define AXI_NS_REG                      REG_MM(0x0014)
#define CAMCLKn_NS_REG(n)               REG_MM(0x0148+(0x14*(n)))
#define CAMCLKn_CC_REG(n)               REG_MM(0x0140+(0x14*(n)))
#define CAMCLKn_MD_REG(n)               REG_MM(0x0144+(0x14*(n)))
#define CSI0_NS_REG                     REG_MM(0x0048)
#define CSI0_CC_REG                     REG_MM(0x0040)
#define CSI0_MD_REG                     REG_MM(0x0044)
#define CSI1_NS_REG                     REG_MM(0x0010)
#define CSI1_CC_REG                     REG_MM(0x0024)
#define CSI1_MD_REG                     REG_MM(0x0028)
#define CSIPHYTIMER_CC_REG              REG_MM(0x0160)
#define CSIPHYTIMER_MD_REG              REG_MM(0x0164)
#define CSIPHYTIMER_NS_REG              REG_MM(0x0168)
#define DSI1_BYTE_NS_REG                REG_MM(0x00B0)
#define DSI1_BYTE_CC_REG                REG_MM(0x0090)
#define DSI2_BYTE_NS_REG                REG_MM(0x00BC)
#define DSI2_BYTE_CC_REG                REG_MM(0x00B4)
#define DSI1_ESC_NS_REG                 REG_MM(0x011C)
#define DSI1_ESC_CC_REG                 REG_MM(0x00CC)
#define DSI2_ESC_NS_REG                 REG_MM(0x0150)
#define DSI2_ESC_CC_REG                 REG_MM(0x013C)
#define DSI_PIXEL_CC_REG                REG_MM(0x0130)
#define DSI_PIXEL_MD_REG                REG_MM(0x0134)
#define DSI_PIXEL_NS_REG                REG_MM(0x0138)
#define DSI2_PIXEL_CC_REG               REG_MM(0x0094)
#define DSI_NS_REG                      REG_MM(0x54)
#define DSI_MD_REG                      REG_MM(0x50)
#define DSI_CC_REG                      REG_MM(0x4C)
#define DBG_BUS_VEC_A_REG               REG_MM(0x01C8)
#define DBG_BUS_VEC_B_REG               REG_MM(0x01CC)
#define DBG_BUS_VEC_C_REG               REG_MM(0x01D0)
#define DBG_BUS_VEC_D_REG               REG_MM(0x01D4)
#define DBG_BUS_VEC_E_REG               REG_MM(0x01D8)
#define DBG_BUS_VEC_F_REG               REG_MM(0x01DC)
#define DBG_BUS_VEC_G_REG               REG_MM(0x01E0)
#define DBG_BUS_VEC_H_REG               REG_MM(0x01E4)
#define DBG_BUS_VEC_I_REG               REG_MM(0x01E8)
#define DBG_CFG_REG_HS_REG              REG_MM(0x01B4)
#define DBG_CFG_REG_LS_REG              REG_MM(0x01B8)
#define GFX2D0_CC_REG                   REG_MM(0x0060)
#define GFX2D0_MD0_REG                  REG_MM(0x0064)
#define GFX2D0_MD1_REG                  REG_MM(0x0068)
#define GFX2D0_NS_REG                   REG_MM(0x0070)
#define GFX2D1_CC_REG                   REG_MM(0x0074)
#define GFX2D1_MD0_REG                  REG_MM(0x0078)
#define GFX2D1_MD1_REG                  REG_MM(0x006C)
#define GFX2D1_NS_REG                   REG_MM(0x007C)
#define GFX3D_CC_REG                    REG_MM(0x0080)
#define GFX3D_MD0_REG                   REG_MM(0x0084)
#define GFX3D_MD1_REG                   REG_MM(0x0088)
#define GFX3D_NS_REG                    REG_MM(0x008C)
#define IJPEG_CC_REG                    REG_MM(0x0098)
#define IJPEG_MD_REG                    REG_MM(0x009C)
#define IJPEG_NS_REG                    REG_MM(0x00A0)
#define JPEGD_CC_REG                    REG_MM(0x00A4)
#define JPEGD_NS_REG                    REG_MM(0x00AC)
#define MAXI_EN_REG                     REG_MM(0x0018)
#define MAXI_EN2_REG                    REG_MM(0x0020)
#define MAXI_EN3_REG                    REG_MM(0x002C)
#define MAXI_EN4_REG                    REG_MM(0x0114)
#define MDP_CC_REG                      REG_MM(0x00C0)
#define MDP_LUT_CC_REG                  REG_MM(0x016C)
#define MDP_MD0_REG                     REG_MM(0x00C4)
#define MDP_MD1_REG                     REG_MM(0x00C8)
#define MDP_NS_REG                      REG_MM(0x00D0)
#define MISC_CC_REG                     REG_MM(0x0058)
#define MISC_CC2_REG                    REG_MM(0x005C)
#define MM_PLL1_MODE_REG                REG_MM(0x031C)
#define ROT_CC_REG                      REG_MM(0x00E0)
#define ROT_NS_REG                      REG_MM(0x00E8)
#define SAXI_EN_REG                     REG_MM(0x0030)
#define SW_RESET_AHB_REG                REG_MM(0x020C)
#define SW_RESET_AHB2_REG               REG_MM(0x0200)
#define SW_RESET_ALL_REG                REG_MM(0x0204)
#define SW_RESET_AXI_REG                REG_MM(0x0208)
#define SW_RESET_CORE_REG               REG_MM(0x0210)
#define TV_CC_REG                       REG_MM(0x00EC)
#define TV_CC2_REG                      REG_MM(0x0124)
#define TV_MD_REG                       REG_MM(0x00F0)
#define TV_NS_REG                       REG_MM(0x00F4)
#define VCODEC_CC_REG                   REG_MM(0x00F8)
#define VCODEC_MD0_REG                  REG_MM(0x00FC)
#define VCODEC_MD1_REG                  REG_MM(0x0128)
#define VCODEC_NS_REG                   REG_MM(0x0100)
#define VFE_CC_REG                      REG_MM(0x0104)
#define VFE_MD_REG                      REG_MM(0x0108)
#define VFE_NS_REG                      REG_MM(0x010C)
#define VPE_CC_REG                      REG_MM(0x0110)
#define VPE_NS_REG                      REG_MM(0x0118)

/* MUX source input identifiers. */
#define pxo_to_bb_mux       0
#define cxo_to_bb_mux       pxo_to_bb_mux
#define pll0_to_bb_mux      2
#define pll8_to_bb_mux      3
#define pll6_to_bb_mux      4
#define gnd_to_bb_mux       5
#define pll3_to_bb_mux      6
#define pxo_to_mm_mux       0
#define pll1_to_mm_mux      1
#define pll2_to_mm_mux      1
#define pll8_to_mm_mux      2
#define pll0_to_mm_mux      3
#define gnd_to_mm_mux       4
#define hdmi_pll_to_mm_mux  3
#define cxo_to_xo_mux       0
#define pxo_to_xo_mux       1
#define gnd_to_xo_mux       3
#define pxo_to_lpa_mux      0
#define cxo_to_lpa_mux      1
#define pll4_to_lpa_mux     2
#define gnd_to_lpa_mux      6

/* Test Vector Macros */
#define TEST_TYPE_PER_LS    1
#define TEST_TYPE_PER_HS    2
#define TEST_TYPE_MM_LS     3
#define TEST_TYPE_MM_HS     4
#define TEST_TYPE_LPA       5
#define TEST_TYPE_CPUL2     6
#define TEST_TYPE_LPA_HS    7
#define TEST_TYPE_SHIFT     24
#define TEST_CLK_SEL_MASK   BM(23, 0)
#define TEST_VECTOR(s, t)   (((t) << TEST_TYPE_SHIFT) | BVAL(23, 0, (s)))
#define TEST_PER_LS(s)      TEST_VECTOR((s), TEST_TYPE_PER_LS)
#define TEST_PER_HS(s)      TEST_VECTOR((s), TEST_TYPE_PER_HS)
#define TEST_MM_LS(s)       TEST_VECTOR((s), TEST_TYPE_MM_LS)
#define TEST_MM_HS(s)       TEST_VECTOR((s), TEST_TYPE_MM_HS)
#define TEST_LPA(s)         TEST_VECTOR((s), TEST_TYPE_LPA)
#define TEST_LPA_HS(s)      TEST_VECTOR((s), TEST_TYPE_LPA_HS)
#define TEST_CPUL2(s)       TEST_VECTOR((s), TEST_TYPE_CPUL2)

#define MN_MODE_DUAL_EDGE 0x2

/* MD Registers */
#define MD4(m_lsb, m, n_lsb, n) \
    (BVAL((m_lsb+3), m_lsb, m) | BVAL((n_lsb+3), n_lsb, ~(n)))

#define MD8(m_lsb, m, n_lsb, n) \
    (BVAL((m_lsb+7), m_lsb, m) | BVAL((n_lsb+7), n_lsb, ~(n)))

#define MD16(m, n) (BVAL(31, 16, m) | BVAL(15, 0, ~(n)))

/* NS Registers */
#define NS(n_msb, n_lsb, n, m, mde_lsb, d_msb, d_lsb, d, s_msb, s_lsb, s) \
    (BVAL(n_msb, n_lsb, ~(n-m)) \
     | (BVAL((mde_lsb+1), mde_lsb, MN_MODE_DUAL_EDGE) * !!(n)) \
     | BVAL(d_msb, d_lsb, (d-1)) | BVAL(s_msb, s_lsb, s))

#define NS_MM(n_msb, n_lsb, n, m, d_msb, d_lsb, d, s_msb, s_lsb, s) \
    (BVAL(n_msb, n_lsb, ~(n-m)) | BVAL(d_msb, d_lsb, (d-1)) \
     | BVAL(s_msb, s_lsb, s))

#define NS_DIVSRC(d_msb , d_lsb, d, s_msb, s_lsb, s) \
    (BVAL(d_msb, d_lsb, (d-1)) | BVAL(s_msb, s_lsb, s))

#define NS_DIV(d_msb , d_lsb, d) \
    BVAL(d_msb, d_lsb, (d-1))

#define NS_SRC_SEL(s_msb, s_lsb, s) \
    BVAL(s_msb, s_lsb, s)

#define NS_MND_BANKED4(n0_lsb, n1_lsb, n, m, s0_lsb, s1_lsb, s) \
    (BVAL((n0_lsb+3), n0_lsb, ~(n-m)) \
     | BVAL((n1_lsb+3), n1_lsb, ~(n-m)) \
     | BVAL((s0_lsb+2), s0_lsb, s) \
     | BVAL((s1_lsb+2), s1_lsb, s))

#define NS_MND_BANKED8(n0_lsb, n1_lsb, n, m, s0_lsb, s1_lsb, s) \
    (BVAL((n0_lsb+7), n0_lsb, ~(n-m)) \
     | BVAL((n1_lsb+7), n1_lsb, ~(n-m)) \
     | BVAL((s0_lsb+2), s0_lsb, s) \
     | BVAL((s1_lsb+2), s1_lsb, s))

#define NS_DIVSRC_BANKED(d0_msb, d0_lsb, d1_msb, d1_lsb, d, \
        s0_msb, s0_lsb, s1_msb, s1_lsb, s) \
(BVAL(d0_msb, d0_lsb, (d-1)) | BVAL(d1_msb, d1_lsb, (d-1)) \
 | BVAL(s0_msb, s0_lsb, s) \
 | BVAL(s1_msb, s1_lsb, s))

/* CC Registers */
#define CC(mde_lsb, n) (BVAL((mde_lsb+1), mde_lsb, MN_MODE_DUAL_EDGE) * !!(n))
#define CC_BANKED(mde0_lsb, mde1_lsb, n) \
    ((BVAL((mde0_lsb+1), mde0_lsb, MN_MODE_DUAL_EDGE) \
      | BVAL((mde1_lsb+1), mde1_lsb, MN_MODE_DUAL_EDGE)) \
     * !!(n))

struct pll_rate {
    const uint32_t    l_val;
    const uint32_t    m_val;
    const uint32_t    n_val;
    const uint32_t    vco;
    const uint32_t    post_div;
    const uint32_t    i_bits;
};
#define PLL_RATE(l, m, n, v, d, i) { l, m, n, v, (d>>1), i }

/* DSI specific data */

/* Configured at 13.5 MHz */
#define ESC_NS_VAL            0x00001000
#define ESC_CC_VAL            0x00000004

#define BYTE_NS_VAL           0x00000001
#define BYTE_CC_VAL           0x00000004

#define PIXEL_NS_VAL          0x00F80003
#define PIXEL_MD_VAL          0x000001FB
#define PIXEL_CC_VAL          0x00000080

#define DSI_NS_VAL            0xFA000003
#define DSI_MD_VAL            0x000003FB
#define DSI_CC_VAL            0x00000080

void config_mmss_clk(uint32_t ns,
             uint32_t md,
             uint32_t cc,
             uint32_t ns_addr, uint32_t md_addr, uint32_t cc_addr);
void config_mdp_lut_clk(void);
void mdp_clock_init(void);

#endif
