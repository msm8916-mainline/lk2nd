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
 *
 */

#ifndef __MIPI_DSI_AUTOPLL_THULIUM_H
#define __MIPI_DSI_AUTOPLL_THULIUM_H

#define DSIPHY_CMN_CLK_CFG0		0x0010
#define DSIPHY_CMN_CLK_CFG1		0x0014
#define DSIPHY_CMN_GLBL_TEST_CTRL	0x0018

#define DSIPHY_CMN_PLL_CNTRL		0x0048
#define DSIPHY_CMN_CTRL_0		0x001c
#define DSIPHY_CMN_CTRL_1		0x0020

#define DSIPHY_CMN_LDO_CNTRL		0x004c

#define DSIPHY_PLL_IE_TRIM		0x0400
#define DSIPHY_PLL_IP_TRIM		0x0404
#define DSIPHY_CMN_PLL_PLL_CTRL		0x0408

#define DSIPHY_PLL_IPTAT_TRIM		0x0410

#define DSIPHY_PLL_CLKBUFLR_EN		0x041c

#define DSIPHY_PLL_SYSCLK_EN_RESET	0x0428
#define DSIPHY_PLL_RESETSM_CNTRL	0x042c
#define DSIPHY_PLL_RESETSM_CNTRL2	0x0430
#define DSIPHY_PLL_RESETSM_CNTRL3	0x0434
#define DSIPHY_PLL_RESETSM_CNTRL4	0x0438
#define DSIPHY_PLL_RESETSM_CNTRL5	0x043c
#define DSIPHY_PLL_KVCO_DIV_REF1	0x0440
#define DSIPHY_PLL_KVCO_DIV_REF2	0x0444

#define DSIPHY_PLL_KVCO_COUNT1		0x0448
#define DSIPHY_PLL_KVCO_COUNT2		0x044c
#define DSIPHY_PLL_KVCO_CODE		0x0458

#define DSIPHY_PLL_VCO_DIV_REF1		0x046c
#define DSIPHY_PLL_VCO_DIV_REF2		0x0470
#define DSIPHY_PLL_VCO_COUNT1		0x0474
#define DSIPHY_PLL_VCO_COUNT2		0x0478
#define DSIPHY_PLL_PLLLOCK_CMP1		0x047c
#define DSIPHY_PLL_PLLLOCK_CMP2		0x0480
#define DSIPHY_PLL_PLLLOCK_CMP3		0x0484
#define DSIPHY_PLL_PLLLOCK_CMP_EN	0x0488

#define DSIPHY_PLL_DEC_START		0x0490
#define DSIPHY_PLL_SSC_EN_CENTER	0x0494
#define DSIPHY_PLL_SSC_ADJ_PER1		0x0498
#define DSIPHY_PLL_SSC_ADJ_PER2		0x049c
#define DSIPHY_PLL_SSC_PER1		0x04a0
#define DSIPHY_PLL_SSC_PER2		0x04a4
#define DSIPHY_PLL_SSC_STEP_SIZE1	0x04a8
#define DSIPHY_PLL_SSC_STEP_SIZE2	0x04ac
#define DSIPHY_PLL_DIV_FRAC_START1	0x04b4
#define DSIPHY_PLL_DIV_FRAC_START2	0x04b8
#define DSIPHY_PLL_DIV_FRAC_START3	0x04bc
#define DSIPHY_PLL_TXCLK_EN		0x04c0
#define DSIPHY_PLL_PLL_CRCTRL		0x04c4

#define DSIPHY_PLL_RESET_SM_READY_STATUS 0x04cc

#define DSIPHY_PLL_PLL_MISC1		0x04e8

#define DSIPHY_PLL_CP_SET_CUR		0x04f0
#define DSIPHY_PLL_PLL_ICPMSET		0x04f4
#define DSIPHY_PLL_PLL_ICPCSET		0x04f8
#define DSIPHY_PLL_PLL_ICP_SET		0x04fc
#define DSIPHY_PLL_PLL_LPF1		0x0500
#define DSIPHY_PLL_PLL_LPF2_POSTDIV	0x0504
#define DSIPHY_PLL_PLL_BANDGAP		0x0508

struct dsi_pll_input {
	uint32_t fref;	/* 19.2 Mhz, reference clk */
	uint32_t fdata;	/* bit clock rate */
	uint32_t dsiclk_sel; /* 1, reg: 0x0014 */
	uint32_t n2div;	/* 1, reg: 0x0010, bit 4-7 */
	uint32_t ssc_en;	/* 1, reg: 0x0494, bit 0 */
	uint32_t ldo_en;	/* 0,  reg: 0x004c, bit 0 */

	/* fixed  */
	uint32_t refclk_dbler_en;	/* 0, reg: 0x04c0, bit 1 */
	uint32_t vco_measure_time;	/* 5, unknown */
	uint32_t kvco_measure_time;	/* 5, unknown */
	uint32_t bandgap_timer;	/* 4, reg: 0x0430, bit 3 - 5 */
	uint32_t pll_wakeup_timer;	/* 5, reg: 0x043c, bit 0 - 2 */
	uint32_t plllock_cnt;	/* 1, reg: 0x0488, bit 1 - 2 */
	uint32_t plllock_rng;	/* 1, reg: 0x0488, bit 3 - 4 */
	uint32_t ssc_center_spread;	/* 0, reg: 0x0494, bit 1 */
	uint32_t ssc_adj_per;	/* 37, reg: 0x498, bit 0 - 9 */
	uint32_t ssc_spread;		/* 0.005  */
	uint32_t ssc_freq;		/* unknown */
	uint32_t pll_ie_trim;	/* 4, reg: 0x0400 */
	uint32_t pll_ip_trim;	/* 4, reg: 0x0404 */
	uint32_t pll_iptat_trim;	/* reg: 0x0410 */
	uint32_t pll_cpcset_cur;	/* 1, reg: 0x04f0, bit 0 - 2 */
	uint32_t pll_cpmset_cur;	/* 1, reg: 0x04f0, bit 3 - 5 */

	uint32_t pll_icpmset;	/* 4, reg: 0x04fc, bit 3 - 5 */
	uint32_t pll_icpcset;	/* 4, reg: 0x04fc, bit 0 - 2 */

	uint32_t pll_icpmset_p;	/* 0, reg: 0x04f4, bit 0 - 2 */
	uint32_t pll_icpmset_m;	/* 0, reg: 0x04f4, bit 3 - 5 */

	uint32_t pll_icpcset_p;	/* 0, reg: 0x04f8, bit 0 - 2 */
	uint32_t pll_icpcset_m;	/* 0, reg: 0x04f8, bit 3 - 5 */

	uint32_t pll_lpf_res1;	/* 3, reg: 0x0504, bit 0 - 3 */
	uint32_t pll_lpf_cap1;	/* 11, reg: 0x0500, bit 0 - 3 */
	uint32_t pll_lpf_cap2;	/* 1, reg: 0x0500, bit 4 - 7 */
	uint32_t pll_c3ctrl;		/* 2, reg: 0x04c4 */
	uint32_t pll_r3ctrl;		/* 1, reg: 0x04c4 */
};

struct dsi_pll_output {
	uint32_t pll_txclk_en;	/* reg: 0x04c0 */
	uint32_t dec_start;		/* reg: 0x0490 */
	uint32_t div_frac_start;	/* reg: 0x04b4, 0x4b8, 0x04bc */
	uint32_t ssc_per;		/* reg: 0x04a0, 0x04a4 */
	uint32_t ssc_step_size;	/* reg: 0x04a8, 0x04ac */
	uint32_t plllock_cmp;	/* reg: 0x047c, 0x0480, 0x0484 */
	uint32_t pll_vco_div_ref;	/* reg: 0x046c, 0x0470 */
	uint32_t pll_vco_count;	/* reg: 0x0474, 0x0478 */
	uint32_t pll_kvco_div_ref;	/* reg: 0x0440, 0x0444 */
	uint32_t pll_kvco_count;	/* reg: 0x0448, 0x044c */
	uint32_t pll_misc1;		/* reg: 0x04e8 */
	uint32_t pll_lpf2_postdiv;	/* reg: 0x0504 */
	uint32_t pll_resetsm_cntrl;	/* reg: 0x042c */
	uint32_t pll_resetsm_cntrl2;	/* reg: 0x0430 */
	uint32_t pll_resetsm_cntrl5;	/* reg: 0x043c */
	uint32_t pll_kvco_code;		/* reg: 0x0458 */

	uint32_t cmn_clk_cfg0;	/* reg: 0x0010 */
	uint32_t cmn_clk_cfg1;	/* reg: 0x0014 */
	uint32_t cmn_ldo_cntrl;	/* reg: 0x004c */

	uint32_t pll_postdiv;	/* vco */
	uint32_t pll_n1div;		/* vco */
	uint32_t pll_n2div;		/* hr_oclk3, pixel */
	uint32_t fcvo;
};

struct dsi_pll_db {
	struct dsi_pll_input in;
	struct dsi_pll_output out;
};

void mdss_dsi_auto_pll_thulium_config(struct msm_panel_info *pinfo);

#endif
