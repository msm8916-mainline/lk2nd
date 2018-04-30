 /* Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
#include <debug.h>
#include <bits.h>
#include <reg.h>
#include <err.h>
#include <smem.h>
#include <clock.h>
#include <mipi_dsi.h>
#include <platform/iomap.h>
#include <qtimer.h>
#include <arch/defines.h>

#define VCO_REF_CLOCK_RATE 19200000

#define DSIPHY_T_TA_GO_TIM_COUNT			0x014
#define DSIPHY_T_TA_SURE_TIM_COUNT			0x018
#define DSIPHY_PLL_POWERUP_CTRL				0x034
#define DSIPHY_PLL_PROP_CHRG_PUMP_CTRL			0x038
#define DSIPHY_PLL_INTEG_CHRG_PUMP_CTRL			0x03c
#define DSIPHY_PLL_ANA_TST_LOCK_ST_OVR_CTRL		0x044
#define DSIPHY_PLL_VCO_CTRL				0x048
#define DSIPHY_PLL_GMP_CTRL_DIG_TST			0x04c
#define DSIPHY_PLL_PHA_ERR_CTRL_0			0x050
#define DSIPHY_PLL_LOCK_FILTER				0x054
#define DSIPHY_PLL_UNLOCK_FILTER			0x058
#define DSIPHY_PLL_INPUT_DIV_PLL_OVR			0x05c
#define DSIPHY_PLL_LOOP_DIV_RATIO_0			0x060
#define DSIPHY_PLL_INPUT_LOOP_DIV_RAT_CTRL		0x064
#define DSIPHY_PLL_PRO_DLY_RELOCK			0x06c
#define DSIPHY_PLL_CHAR_PUMP_BIAS_CTRL			0x070
#define DSIPHY_PLL_LOCK_DET_MODE_SEL			0x074
#define DSIPHY_PLL_ANA_PROG_CTRL			0x07c
#define DSIPHY_HSTX_DRIV_INDATA_CTRL_CLKLANE		0x0c0
#define DSIPHY_HSTX_DATAREV_CTRL_CLKLANE		0x0d4
#define DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE0		0x100
#define DSIPHY_HS_FREQ_RAN_SEL				0x110
#define DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE0	0x114
#define DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE1		0x140
#define DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE1	0x154
#define DSIPHY_HSTX_CLKLANE_REQSTATE_TIM_CTRL		0x180
#define DSIPHY_HSTX_CLKLANE_HS0STATE_TIM_CTRL		0x188
#define DSIPHY_HSTX_CLKLANE_TRALSTATE_TIM_CTRL		0x18c
#define DSIPHY_HSTX_CLKLANE_EXITSTATE_TIM_CTRL		0x190
#define DSIPHY_HSTX_CLKLANE_CLKPOSTSTATE_TIM_CTRL	0x194
#define DSIPHY_HSTX_DATALANE_REQSTATE_TIM_CTRL		0x1c0
#define DSIPHY_HSTX_DATALANE_HS0STATE_TIM_CTRL		0x1c8
#define DSIPHY_HSTX_DATALANE_TRAILSTATE_TIM_CTRL	0x1cc
#define DSIPHY_HSTX_DATALANE_EXITSTATE_TIM_CTRL		0x1d0
#define DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE2		0x200
#define DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE2	0x214
#define DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE3	0x254
#define DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE3		0x240
#define DSIPHY_SLEWRATE_FSM_OVR_CTRL			0x280
#define DSIPHY_SLEWRATE_DDL_LOOP_CTRL			0x28c
#define DSIPHY_SLEWRATE_DDL_CYC_FRQ_ADJ_0		0x290
#define DSIPHY_PLL_PHA_ERR_CTRL_1			0x2e4
#define DSIPHY_PLL_LOOP_DIV_RATIO_1			0x2e8
#define DSIPHY_SLEWRATE_DDL_CYC_FRQ_ADJ_1		0x328
#define DSIPHY_SSC0					0x394
#define DSIPHY_SSC9					0x3b8
#define DSIPHY_STAT0					0x3e0
#define DSIPHY_CTRL0					0x3e8
#define DSIPHY_SYS_CTRL					0x3f0
#define DSIPHY_PLL_CTRL					0x3f8
#define DSIPHY_REQ_DLY					0x3fc

struct dsi_pll_param {
	uint32_t vco_freq;
	uint32_t hsfreqrange;
	uint32_t vco_cntrl;
	uint32_t osc_freq_target;
	uint32_t m_div;
	uint32_t prop_cntrl;
	uint32_t int_cntrl;
	uint32_t gmp_cntrl;
	uint32_t cpbias_cntrl;

	/* mux and dividers */
	uint32_t gp_div_mux;
	uint32_t post_div_mux;
	uint32_t pixel_divhf;
	uint32_t fsm_ovr_ctrl;
};

static uint32_t __mdss_dsi_get_hsfreqrange(uint64_t target_freq)
{
	uint64_t bitclk_rate_mhz = ((target_freq * 2) / 1000000);

	if (bitclk_rate_mhz >= 80 && bitclk_rate_mhz < 90)
		return 0x00;
	else if (bitclk_rate_mhz >= 90 && bitclk_rate_mhz < 100)
		return 0x10;
	else if (bitclk_rate_mhz >= 100 && bitclk_rate_mhz < 110)
		return  0x20;
	else if (bitclk_rate_mhz >= 110 && bitclk_rate_mhz < 120)
		return  0x30;
	else if (bitclk_rate_mhz >= 120 && bitclk_rate_mhz < 130)
		return  0x01;
	else if (bitclk_rate_mhz >= 130 && bitclk_rate_mhz < 140)
		return  0x11;
	else if (bitclk_rate_mhz >= 140 && bitclk_rate_mhz < 150)
		return  0x21;
	else if (bitclk_rate_mhz >= 150 && bitclk_rate_mhz < 160)
		return  0x31;
	else if (bitclk_rate_mhz >= 160 && bitclk_rate_mhz < 170)
		return  0x02;
	else if (bitclk_rate_mhz >= 170 && bitclk_rate_mhz < 180)
		return  0x12;
	else if (bitclk_rate_mhz >= 180 && bitclk_rate_mhz < 190)
		return  0x22;
	else if (bitclk_rate_mhz >= 190 && bitclk_rate_mhz < 205)
		return  0x32;
	else if (bitclk_rate_mhz >= 205 && bitclk_rate_mhz < 220)
		return  0x03;
	else if (bitclk_rate_mhz >= 220 && bitclk_rate_mhz < 235)
		return  0x13;
	else if (bitclk_rate_mhz >= 235 && bitclk_rate_mhz < 250)
		return  0x23;
	else if (bitclk_rate_mhz >= 250 && bitclk_rate_mhz < 275)
		return  0x33;
	else if (bitclk_rate_mhz >= 275 && bitclk_rate_mhz < 300)
		return  0x04;
	else if (bitclk_rate_mhz >= 300 && bitclk_rate_mhz < 325)
		return  0x14;
	else if (bitclk_rate_mhz >= 325 && bitclk_rate_mhz < 350)
		return  0x25;
	else if (bitclk_rate_mhz >= 350 && bitclk_rate_mhz < 400)
		return  0x35;
	else if (bitclk_rate_mhz >= 400 && bitclk_rate_mhz < 450)
		return  0x05;
	else if (bitclk_rate_mhz >= 450 && bitclk_rate_mhz < 500)
		return  0x16;
	else if (bitclk_rate_mhz >= 500 && bitclk_rate_mhz < 550)
		return  0x26;
	else if (bitclk_rate_mhz >= 550 && bitclk_rate_mhz < 600)
		return  0x37;
	else if (bitclk_rate_mhz >= 600 && bitclk_rate_mhz < 650)
		return  0x07;
	else if (bitclk_rate_mhz >= 650 && bitclk_rate_mhz < 700)
		return  0x18;
	else if (bitclk_rate_mhz >= 700 && bitclk_rate_mhz < 750)
		return  0x28;
	else if (bitclk_rate_mhz >= 750 && bitclk_rate_mhz < 800)
		return  0x39;
	else if (bitclk_rate_mhz >= 800 && bitclk_rate_mhz < 850)
		return  0x09;
	else if (bitclk_rate_mhz >= 850 && bitclk_rate_mhz < 900)
		return  0x19;
	else if (bitclk_rate_mhz >= 900 && bitclk_rate_mhz < 950)
		return  0x29;
	else if (bitclk_rate_mhz >= 950 && bitclk_rate_mhz < 1000)
		return  0x3a;
	else if (bitclk_rate_mhz >= 1000 && bitclk_rate_mhz < 1050)
		return  0x0a;
	else if (bitclk_rate_mhz >= 1050 && bitclk_rate_mhz < 1100)
		return  0x1a;
	else if (bitclk_rate_mhz >= 1100 && bitclk_rate_mhz < 1150)
		return  0x2a;
	else if (bitclk_rate_mhz >= 1150 && bitclk_rate_mhz < 1200)
		return  0x3b;
	else if (bitclk_rate_mhz >= 1200 && bitclk_rate_mhz < 1250)
		return  0x0b;
	else if (bitclk_rate_mhz >= 1250 && bitclk_rate_mhz < 1300)
		return  0x1b;
	else if (bitclk_rate_mhz >= 1300 && bitclk_rate_mhz < 1350)
		return  0x2b;
	else if (bitclk_rate_mhz >= 1350 && bitclk_rate_mhz < 1400)
		return  0x3c;
	else if (bitclk_rate_mhz >= 1400 && bitclk_rate_mhz < 1450)
		return  0x0c;
	else if (bitclk_rate_mhz >= 1450 && bitclk_rate_mhz < 1500)
		return  0x1c;
	else if (bitclk_rate_mhz >= 1500 && bitclk_rate_mhz < 1550)
		return  0x2c;
	else if (bitclk_rate_mhz >= 1550 && bitclk_rate_mhz < 1600)
		return  0x3d;
	else if (bitclk_rate_mhz >= 1600 && bitclk_rate_mhz < 1650)
		return  0x0d;
	else if (bitclk_rate_mhz >= 1650 && bitclk_rate_mhz < 1700)
		return  0x1d;
	else if (bitclk_rate_mhz >= 1700 && bitclk_rate_mhz < 1750)
		return  0x2e;
	else if (bitclk_rate_mhz >= 1750 && bitclk_rate_mhz < 1800)
		return  0x3e;
	else if (bitclk_rate_mhz >= 1800 && bitclk_rate_mhz < 1850)
		return  0x0e;
	else if (bitclk_rate_mhz >= 1850 && bitclk_rate_mhz < 1900)
		return  0x1e;
	else if (bitclk_rate_mhz >= 1900 && bitclk_rate_mhz < 1950)
		return  0x2f;
	else if (bitclk_rate_mhz >= 1950 && bitclk_rate_mhz < 2000)
		return  0x3f;
	else if (bitclk_rate_mhz >= 2000 && bitclk_rate_mhz < 2050)
		return  0x0f;
	else if (bitclk_rate_mhz >= 2050 && bitclk_rate_mhz < 2100)
		return  0x40;
	else if (bitclk_rate_mhz >= 2100 && bitclk_rate_mhz < 2150)
		return  0x41;
	else if (bitclk_rate_mhz >= 2150 && bitclk_rate_mhz < 2200)
		return  0x42;
	else if (bitclk_rate_mhz >= 2200 && bitclk_rate_mhz < 2250)
		return  0x43;
	else if (bitclk_rate_mhz >= 2250 && bitclk_rate_mhz < 2300)
		return  0x44;
	else if (bitclk_rate_mhz >= 2300 && bitclk_rate_mhz < 2350)
		return  0x45;
	else if (bitclk_rate_mhz >= 2350 && bitclk_rate_mhz < 2400)
		return  0x46;
	else if (bitclk_rate_mhz >= 2400 && bitclk_rate_mhz < 2450)
		return  0x47;
	else if (bitclk_rate_mhz >= 2450 && bitclk_rate_mhz < 2500)
		return  0x48;
	else
		return  0x49;
}

static void __mdss_dsi_get_pll_vco_cntrl(uint64_t target_freq,
	uint32_t post_div_mux, uint32_t *vco_cntrl, uint32_t *cpbias_cntrl)
{
	uint64_t target_freq_mhz = (target_freq / 1000000);
	uint32_t p_div = BIT(post_div_mux);

	if (p_div == 1) {
		*vco_cntrl = 0x00;
		*cpbias_cntrl = 0;
	} else if (p_div == 2) {
		*vco_cntrl = 0x30;
		*cpbias_cntrl = 1;
	} else if (p_div == 4) {
		*vco_cntrl = 0x10;
		*cpbias_cntrl = 0;
	} else if (p_div == 8) {
		*vco_cntrl = 0x20;
		*cpbias_cntrl = 0;
	} else if (p_div == 16) {
		*vco_cntrl = 0x30;
		*cpbias_cntrl = 0;
	} else {
		*vco_cntrl = 0x00;
		*cpbias_cntrl = 1;
	}

	if (target_freq_mhz <= 1250 && target_freq_mhz >= 1092)
		*vco_cntrl = *vco_cntrl | 2;
	else if (target_freq_mhz < 1092 && target_freq_mhz >= 950)
		*vco_cntrl =  *vco_cntrl | 3;
	else if (target_freq_mhz < 950 && target_freq_mhz >= 712)
		*vco_cntrl = *vco_cntrl | 1;
	else if (target_freq_mhz < 712 && target_freq_mhz >= 546)
		*vco_cntrl =  *vco_cntrl | 2;
	else if (target_freq_mhz < 546 && target_freq_mhz >= 475)
		*vco_cntrl = *vco_cntrl | 3;
	else if (target_freq_mhz < 475 && target_freq_mhz >= 356)
		*vco_cntrl =  *vco_cntrl | 1;
	else if (target_freq_mhz < 356 && target_freq_mhz >= 273)
		*vco_cntrl = *vco_cntrl | 2;
	else if (target_freq_mhz < 273 && target_freq_mhz >= 237)
		*vco_cntrl =  *vco_cntrl | 3;
	else if (target_freq_mhz < 237 && target_freq_mhz >= 178)
		*vco_cntrl = *vco_cntrl | 1;
	else if (target_freq_mhz < 178 && target_freq_mhz >= 136)
		*vco_cntrl =  *vco_cntrl | 2;
	else if (target_freq_mhz < 136 && target_freq_mhz >= 118)
		*vco_cntrl = *vco_cntrl | 3;
	else if (target_freq_mhz < 118 && target_freq_mhz >= 89)
		*vco_cntrl =  *vco_cntrl | 1;
	else if (target_freq_mhz < 89 && target_freq_mhz >= 68)
		*vco_cntrl = *vco_cntrl | 2;
	else if (target_freq_mhz < 68 && target_freq_mhz >= 57)
		*vco_cntrl =  *vco_cntrl | 3;
	else if (target_freq_mhz < 57 && target_freq_mhz >= 44)
		*vco_cntrl = *vco_cntrl | 1;
	else
		*vco_cntrl =  *vco_cntrl | 2;
}

static uint32_t __mdss_dsi_get_osc_freq_target(uint64_t target_freq)
{
	uint64_t target_freq_mhz = (target_freq / 1000000);

	if (target_freq_mhz <= 1000)
		return 1315;
	else if (target_freq_mhz > 1000 && target_freq_mhz <= 1500)
		return 1839;
	else
		return 0;
}

static uint64_t __mdss_dsi_pll_get_m_div(uint64_t vco_rate)
{
	return ((vco_rate * 4) / VCO_REF_CLOCK_RATE);
}

static uint32_t __mdss_dsi_get_fsm_ovr_ctrl(uint64_t target_freq)
{
	uint64_t bitclk_rate_mhz = ((target_freq * 2) / 1000000);

	if (bitclk_rate_mhz > 1500 && bitclk_rate_mhz <= 2500)
		return 0;
	else
		return BIT(6);
}

static void mdss_dsi_pll_12nm_calc_reg(struct dsi_pll_param *param)
{
	uint64_t target_freq = 0;

	target_freq = (param->vco_freq / BIT(param->post_div_mux));

	param->hsfreqrange = __mdss_dsi_get_hsfreqrange(target_freq);
	__mdss_dsi_get_pll_vco_cntrl(target_freq, param->post_div_mux,
		&param->vco_cntrl, &param->cpbias_cntrl);
	param->osc_freq_target = __mdss_dsi_get_osc_freq_target(target_freq);
	param->m_div = (uint32_t) __mdss_dsi_pll_get_m_div(param->vco_freq);
	param->fsm_ovr_ctrl = __mdss_dsi_get_fsm_ovr_ctrl(target_freq);
	param->prop_cntrl = 0x05;
	param->int_cntrl = 0x00;
	param->gmp_cntrl = 0x1;
}

static void pll_db_commit_12nm(struct dsi_pll_param *param,
					uint32_t phy_base)
{
	uint32_t data = 0;

	writel_relaxed(0x01, phy_base + DSIPHY_CTRL0);
	writel_relaxed(0x05, phy_base + DSIPHY_PLL_CTRL);
	writel_relaxed(0x01, phy_base + DSIPHY_SLEWRATE_DDL_LOOP_CTRL);

	data = ((param->hsfreqrange & 0x7f) | BIT(7));
	writel_relaxed(data, phy_base + DSIPHY_HS_FREQ_RAN_SEL);

	data = ((param->vco_cntrl & 0x3f) | BIT(6));
	writel_relaxed(data, phy_base + DSIPHY_PLL_VCO_CTRL);

	data = (param->osc_freq_target & 0x7f);
	writel_relaxed(data, phy_base + DSIPHY_SLEWRATE_DDL_CYC_FRQ_ADJ_0);

	data = ((param->osc_freq_target & 0xf80) >> 7);
	writel_relaxed(data, phy_base + DSIPHY_SLEWRATE_DDL_CYC_FRQ_ADJ_1);
	writel_relaxed(0x30, phy_base + DSIPHY_PLL_INPUT_LOOP_DIV_RAT_CTRL);

	data = (param->m_div & 0x3f);
	writel_relaxed(data, phy_base + DSIPHY_PLL_LOOP_DIV_RATIO_0);

	data = ((param->m_div & 0xfc0) >> 6);
	writel_relaxed(data, phy_base + DSIPHY_PLL_LOOP_DIV_RATIO_1);
	writel_relaxed(0x60, phy_base + DSIPHY_PLL_INPUT_DIV_PLL_OVR);

	data = (param->prop_cntrl & 0x3f);
	writel_relaxed(data, phy_base + DSIPHY_PLL_PROP_CHRG_PUMP_CTRL);

	data = (param->int_cntrl & 0x3f);
	writel_relaxed(data, phy_base + DSIPHY_PLL_INTEG_CHRG_PUMP_CTRL);

	data = ((param->gmp_cntrl & 0x3) << 4);
	writel_relaxed(data, phy_base + DSIPHY_PLL_GMP_CTRL_DIG_TST);

	data = ((param->cpbias_cntrl & 0x1) << 6) | BIT(4);
	writel_relaxed(data, phy_base + DSIPHY_PLL_CHAR_PUMP_BIAS_CTRL);

	data = ((param->gp_div_mux & 0x7) << 5) | 0x5;
	writel_relaxed(data, phy_base + DSIPHY_PLL_CTRL);

	data = (param->pixel_divhf & 0x7f);
	writel_relaxed(data, phy_base + DSIPHY_SSC9);

	writel_relaxed(0x03, phy_base + DSIPHY_PLL_ANA_PROG_CTRL);
	writel_relaxed(0x50, phy_base + DSIPHY_PLL_ANA_TST_LOCK_ST_OVR_CTRL);
	writel_relaxed(param->fsm_ovr_ctrl,
		phy_base + DSIPHY_SLEWRATE_FSM_OVR_CTRL);
	writel_relaxed(0x01, phy_base + DSIPHY_PLL_PHA_ERR_CTRL_0);
	writel_relaxed(0x00, phy_base + DSIPHY_PLL_PHA_ERR_CTRL_1);
	writel_relaxed(0xff, phy_base + DSIPHY_PLL_LOCK_FILTER);
	writel_relaxed(0x03, phy_base + DSIPHY_PLL_UNLOCK_FILTER);
	writel_relaxed(0x0c, phy_base + DSIPHY_PLL_PRO_DLY_RELOCK);
	writel_relaxed(0x02, phy_base + DSIPHY_PLL_LOCK_DET_MODE_SEL);
	dmb(); /* make sure register committed */
}

static void mdss_dsi_phy_12nm_init(struct msm_panel_info *pinfo,
	uint32_t phy_base)
{
	uint32_t *timing = pinfo->mipi.mdss_dsi_phy_db->timing;

	/* Shutdown PHY initially */
	writel_relaxed(0x09, phy_base + DSIPHY_SYS_CTRL);
	/* CTRL0: CFG_CLK_EN */
	writel_relaxed(0x1, phy_base + DSIPHY_CTRL0);

	/* DSI PHY clock lane timings */
	writel_relaxed((timing[0] | BIT(7)),
		phy_base + DSIPHY_HSTX_CLKLANE_HS0STATE_TIM_CTRL);
	writel_relaxed((timing[1] | BIT(6)),
		phy_base + DSIPHY_HSTX_CLKLANE_TRALSTATE_TIM_CTRL);
	writel_relaxed((timing[2] | BIT(6)),
		phy_base + DSIPHY_HSTX_CLKLANE_CLKPOSTSTATE_TIM_CTRL);
	writel_relaxed(timing[3],
		phy_base + DSIPHY_HSTX_CLKLANE_REQSTATE_TIM_CTRL);
	writel_relaxed((timing[7] | BIT(6) | BIT(7)),
		phy_base + DSIPHY_HSTX_CLKLANE_EXITSTATE_TIM_CTRL);

	/* DSI PHY data lane timings */
	writel_relaxed((timing[4] | BIT(7)),
		phy_base + DSIPHY_HSTX_DATALANE_HS0STATE_TIM_CTRL);
	writel_relaxed((timing[5] | BIT(6)),
		phy_base + DSIPHY_HSTX_DATALANE_TRAILSTATE_TIM_CTRL);
	writel_relaxed(timing[6],
		phy_base + DSIPHY_HSTX_DATALANE_REQSTATE_TIM_CTRL);
	writel_relaxed((timing[7] | BIT(6) | BIT(7)),
		phy_base + DSIPHY_HSTX_DATALANE_EXITSTATE_TIM_CTRL);

	writel_relaxed(0x03, phy_base + DSIPHY_T_TA_GO_TIM_COUNT);
	writel_relaxed(0x01, phy_base + DSIPHY_T_TA_SURE_TIM_COUNT);
	writel_relaxed(0x85, phy_base + DSIPHY_REQ_DLY);

	/* DSI lane control registers */
	writel_relaxed(0x00, phy_base +
		DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE0);
	writel_relaxed(0x00, phy_base +
		DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE1);
	writel_relaxed(0x00, phy_base +
		DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE2);
	writel_relaxed(0x00, phy_base +
		DSIPHY_HSTX_READY_DLY_DATA_REV_CTRL_LANE3);
	writel_relaxed(0x00, phy_base +
		DSIPHY_HSTX_DATAREV_CTRL_CLKLANE);
	dmb(); /* make sure DSI PHY registers are programmed */
}

void mdss_dsi_auto_pll_12nm_config(struct msm_panel_info *pinfo)
{
	struct mdss_dsi_pll_config *pd = pinfo->mipi.dsi_pll_config;
	uint32_t phy_base = pinfo->mipi.phy_base;
	uint32_t sphy_base = pinfo->mipi.sphy_base;
	struct dsi_pll_param param = {0};

	param.vco_freq = pd->vco_clock;
	param.post_div_mux = pd->p_div_mux;
	param.gp_div_mux = pd->gp_div_mux;
	param.pixel_divhf = pd->divhf;

	mdss_dsi_phy_12nm_init(pinfo, phy_base);
	if (pinfo->mipi.dual_dsi)
		mdss_dsi_phy_12nm_init(pinfo, sphy_base);

	mdss_dsi_pll_12nm_calc_reg(&param);
	pll_db_commit_12nm(&param, phy_base);
}

static uint32_t is_pll_locked_12nm(uint32_t phy_base)
{
	uint32_t cnt, status;

	/* check pll lock */
	for (cnt = 0; cnt < 50; cnt++) {
		status = readl_relaxed(phy_base + DSIPHY_STAT0);
		dprintf(SPEW, "%s: phy_base=%x cnt=%d status=%x\n",
				__func__, phy_base, cnt, status);
		status &= BIT(1);
		if (status)
			break;
		udelay(500);
	}

	return status;
}

static void mdss_dsi_12nm_phy_hstx_drv_enable(uint32_t phy_base)
{
	uint32_t data = BIT(2) | BIT(3);

	writel_relaxed(data, phy_base + DSIPHY_HSTX_DRIV_INDATA_CTRL_CLKLANE);
	writel_relaxed(data, phy_base + DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE0);
	writel_relaxed(data, phy_base + DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE1);
	writel_relaxed(data, phy_base + DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE2);
	writel_relaxed(data, phy_base + DSIPHY_HSTX_DRIV_INDATA_CTRL_LANE3);
	dmb(); /* make sure DSI PHY registers are programmed */
}

bool mdss_dsi_auto_pll_12nm_enable(struct msm_panel_info *pinfo)
{
	uint32_t phy_base = pinfo->mipi.phy_base;
	uint32_t sphy_base = pinfo->mipi.sphy_base;

	writel_relaxed(0x49, phy_base + DSIPHY_SYS_CTRL);
	dmb(); /* make sure register committed */
	udelay(5); /* h/w recommended delay */
	writel_relaxed(0xc9, phy_base + DSIPHY_SYS_CTRL);
	dmb(); /* make sure register committed */
	udelay(50); /* h/w recommended delay */

	if (!is_pll_locked_12nm(phy_base)) {
		dprintf(SPEW, "DSI PLL lock failed!\n");
		return false;
	}

	dprintf(SPEW, "DSI PLL Locked!\n");

	/* Enable DSI PLL output to DSI controller */
	writel_relaxed(0x40, phy_base + DSIPHY_SSC0);

	mdss_dsi_12nm_phy_hstx_drv_enable(phy_base);
	if (pinfo->mipi.dual_dsi)
		mdss_dsi_12nm_phy_hstx_drv_enable(sphy_base);
	return true;
}
