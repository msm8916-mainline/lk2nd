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
#include <debug.h>
#include <bits.h>
#include <reg.h>
#include <err.h>
#include <smem.h>
#include <board.h>
#include <mipi_dsi.h>
#include <mipi_dsi_autopll_thulium.h>
#include <platform/iomap.h>
#include <qtimer.h>
#include <arch/defines.h>

#define DATALANE_OFFSET_FROM_BASE_THULIUM	  0x100
#define DATALANE_SIZE_THULIUM			  0x80
#define MMSS_DSI_DSIPHY_CMN_LDO_CTRL		  0x4c

#define VCO_REF_CLK_RATE 19200000

#define CEIL(x, y)              (((x) + ((y)-1)) / (y))

static void mdss_mdp_pll_input_init(struct dsi_pll_db *pdb)
{
	pdb->in.fref = 19200000;	/* 19.2 Mhz*/
	pdb->in.fdata = 0;		/* bit clock rate */
	pdb->in.dsiclk_sel = 1;		/* 1, reg: 0x0014 */
	pdb->in.ssc_en = 1;		/* 1, reg: 0x0494, bit 0 */
	pdb->in.ldo_en = 0;		/* 0,  reg: 0x004c, bit 0 */

	/* fixed  input */
	pdb->in.refclk_dbler_en = 0;	/* 0, reg: 0x04c0, bit 1 */
	pdb->in.vco_measure_time = 5;	/* 5, unknown */
	pdb->in.kvco_measure_time = 5;	/* 5, unknown */
	pdb->in.bandgap_timer = 4;	/* 4, reg: 0x0430, bit 3 - 5 */
	pdb->in.pll_wakeup_timer = 5;	/* 5, reg: 0x043c, bit 0 - 2 */
	pdb->in.plllock_cnt = 1;	/* 1, reg: 0x0488, bit 1 - 2 */
	pdb->in.plllock_rng = 0;	/* 0, reg: 0x0488, bit 3 - 4 */
	pdb->in.ssc_center_spread = 0;	/* 0, reg: 0x0494, bit 1 */
	pdb->in.ssc_adj_per = 37;	/* 37, reg: 0x498, bit 0 - 9 */
	pdb->in.ssc_spread = 5;		/* 0.005, 5kppm */
	pdb->in.ssc_freq = 31500;	/* 31.5 khz */

	pdb->in.pll_ie_trim = 4;	/* 4, reg: 0x0400 */
	pdb->in.pll_ip_trim = 4;	/* 4, reg: 0x0404 */
	pdb->in.pll_cpcset_cur = 1;	/* 1, reg: 0x04f0, bit 0 - 2 */
	pdb->in.pll_cpmset_cur = 1;	/* 1, reg: 0x04f0, bit 3 - 5 */
	pdb->in.pll_icpmset = 4;	/* 4, reg: 0x04fc, bit 3 - 5 */
	pdb->in.pll_icpcset = 4;	/* 4, reg: 0x04fc, bit 0 - 2 */
	pdb->in.pll_icpmset_p = 0;	/* 0, reg: 0x04f4, bit 0 - 2 */
	pdb->in.pll_icpmset_m = 0;	/* 0, reg: 0x04f4, bit 3 - 5 */
	pdb->in.pll_icpcset_p = 0;	/* 0, reg: 0x04f8, bit 0 - 2 */
	pdb->in.pll_icpcset_m = 0;	/* 0, reg: 0x04f8, bit 3 - 5 */
	pdb->in.pll_lpf_res1 = 3;	/* 3, reg: 0x0504, bit 0 - 3 */
	pdb->in.pll_lpf_cap1 = 11;	/* 11, reg: 0x0500, bit 0 - 3 */
	pdb->in.pll_lpf_cap2 = 1;	/* 1, reg: 0x0500, bit 4 - 7 */
	pdb->in.pll_iptat_trim = 7;
	pdb->in.pll_c3ctrl = 2;		/* 2 */
	pdb->in.pll_r3ctrl = 1;		/* 1 */
}

static void mdss_mdp_pll_dec_frac_calc(struct dsi_pll_db *pdb,
	uint32_t vco_clk_rate, uint32_t fref)
{
	uint64_t vco_clk;
	uint64_t multiplier = BIT(20);
	uint64_t dec_start_multiple, dec_start, pll_comp_val;
	uint32_t duration, div_frac_start;

	vco_clk = vco_clk_rate * multiplier;

	dec_start_multiple = (uint64_t) vco_clk / fref;
	div_frac_start = (uint32_t) dec_start_multiple % multiplier;

	dec_start = dec_start_multiple / multiplier;

	pdb->out.dec_start = (uint32_t)dec_start;
	pdb->out.div_frac_start = div_frac_start;

	if (pdb->in.plllock_cnt == 0)
		duration = 1024;
	else if (pdb->in.plllock_cnt == 1)
		duration = 256;
	else if (pdb->in.plllock_cnt == 2)
		duration = 128;
	else
		duration = 32;

	pll_comp_val =  (uint64_t)duration * dec_start_multiple;
	pll_comp_val /= (multiplier * 10);

	pdb->out.plllock_cmp = (uint32_t)pll_comp_val;

	pdb->out.pll_txclk_en = 1;
	pdb->out.cmn_ldo_cntrl = 0x3c;
}

static void mdss_mdp_pll_ssc_calc(struct dsi_pll_db *pdb,
	uint32_t vco_clk_rate, uint32_t fref)
{
	uint32_t period, ssc_period;
	uint32_t ref, rem;
	uint64_t step_size;

	ssc_period = pdb->in.ssc_freq / 500;
	period = (unsigned long)fref / 1000;
	ssc_period  = CEIL(period, ssc_period);
	ssc_period -= 1;
	pdb->out.ssc_per = ssc_period;

	step_size = vco_clk_rate;
	ref = fref;

	ref /= 1000;
	step_size /= ref;
	step_size <<= 20;
	step_size /= 1000;
	step_size *= pdb->in.ssc_spread;
	step_size /= 1000;
	step_size *= (pdb->in.ssc_adj_per + 1);

	rem = 0;
	rem = step_size % (ssc_period + 1);
	if (rem)
		step_size++;

	step_size &= 0x0ffff;   /* take lower 16 bits */
	pdb->out.ssc_step_size = step_size;
}

static uint32_t mdss_mdp_pll_kvco_slop(uint32_t vrate)
{
	uint32_t slop = 0;

	if (vrate > 1300000000 && vrate <= 1800000000)
		slop =  600;
	else if (vrate > 1800000000 && vrate < 2300000000)
		slop = 400;
	else if (vrate > 2300000000 && vrate < 2600000000)
		slop = 280;

	return slop;
}

static inline uint32_t mdss_mdp_pll_calc_kvco_code(uint32_t vco_clk_rate)
{
	uint32_t kvco_code;

	if ((vco_clk_rate >= 2300000000ULL) &&
	    (vco_clk_rate <= 2600000000ULL))
		kvco_code = 0x2f;
	else if ((vco_clk_rate >= 1800000000ULL) &&
		 (vco_clk_rate < 2300000000ULL))
		kvco_code = 0x2c;
	else
		kvco_code = 0x28;

	return kvco_code;
}

static void mdss_mdp_pll_calc_vco_count(struct dsi_pll_db *pdb,
	uint32_t vco_clk_rate, uint32_t fref)
{
	uint64_t data;
	uint32_t cnt;

	data = fref * pdb->in.vco_measure_time;
	data /= 1000000;
	data &= 0x03ff;	/* 10 bits */
	data -= 2;
	pdb->out.pll_vco_div_ref = data;

	data = vco_clk_rate / 1000000;	/* unit is Mhz */
	data *= pdb->in.vco_measure_time;
	data /= 10;
	pdb->out.pll_vco_count = data; /* reg: 0x0474, 0x0478 */

	data = fref * pdb->in.kvco_measure_time;
	data /= 1000000;
	data &= 0x03ff;	/* 10 bits */
	data -= 1;
	pdb->out.pll_kvco_div_ref = data;

	cnt = mdss_mdp_pll_kvco_slop(vco_clk_rate);
	cnt *= 2;
	cnt /= 100;
	cnt *= pdb->in.kvco_measure_time;
	pdb->out.pll_kvco_count = cnt;

	pdb->out.pll_misc1 = 16;
	pdb->out.pll_resetsm_cntrl = 48;
	pdb->out.pll_resetsm_cntrl2 = pdb->in.bandgap_timer << 3;
	pdb->out.pll_resetsm_cntrl5 = pdb->in.pll_wakeup_timer;
	pdb->out.pll_kvco_code = mdss_mdp_pll_calc_kvco_code(vco_clk_rate);
}

static void mdss_mdp_pll_assert_and_div_cfg(uint32_t phy_base,
	struct dsi_pll_db *pdb)
{
	uint32_t n2div = 0;

	n2div = readl(phy_base + DSIPHY_CMN_CLK_CFG0);
	n2div &= ~(0xf0);
	n2div |= (pdb->out.pll_n2div << 4);
	writel(n2div, phy_base + DSIPHY_CMN_CLK_CFG0);

	dmb();
}

static void mdss_mdp_pll_nonfreq_config(uint32_t phy_base, struct dsi_pll_db *pdb)
{
	uint32_t data;

	writel(pdb->out.cmn_ldo_cntrl, phy_base + DSIPHY_CMN_LDO_CNTRL);
	writel(0x0, phy_base + DSIPHY_PLL_SYSCLK_EN_RESET);

	data = pdb->out.pll_txclk_en;
	writel(pdb->out.pll_txclk_en, phy_base + DSIPHY_PLL_TXCLK_EN);

	writel(pdb->out.pll_resetsm_cntrl, phy_base + DSIPHY_PLL_RESETSM_CNTRL);
	writel(pdb->out.pll_resetsm_cntrl2, phy_base + DSIPHY_PLL_RESETSM_CNTRL2);
	writel(pdb->out.pll_resetsm_cntrl5, phy_base + DSIPHY_PLL_RESETSM_CNTRL5);

	data = pdb->out.pll_vco_div_ref;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_VCO_DIV_REF1);
	data = (pdb->out.pll_vco_div_ref >> 8);
	data &= 0x03;
	writel(data, phy_base + DSIPHY_PLL_VCO_DIV_REF2);

	data = pdb->out.pll_kvco_div_ref;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_KVCO_DIV_REF1);
	data = (pdb->out.pll_kvco_div_ref >> 8);
	data &= 0x03;
	writel(data, phy_base + DSIPHY_PLL_KVCO_DIV_REF2);

	writel(pdb->out.pll_misc1, phy_base + DSIPHY_PLL_PLL_MISC1);

	writel(pdb->in.pll_ie_trim, phy_base + DSIPHY_PLL_IE_TRIM);

	writel(pdb->in.pll_ip_trim, phy_base + DSIPHY_PLL_IP_TRIM);

	data = ((pdb->in.pll_cpmset_cur << 3) | pdb->in.pll_cpcset_cur);
	writel(data, phy_base + DSIPHY_PLL_CP_SET_CUR);

	data = ((pdb->in.pll_icpcset_p << 3) | pdb->in.pll_icpcset_m);
	writel(data, phy_base + DSIPHY_PLL_PLL_ICPCSET);

	data = ((pdb->in.pll_icpmset_p << 3) | pdb->in.pll_icpcset_m);
	writel(data, phy_base + DSIPHY_PLL_PLL_ICPMSET);

	data = ((pdb->in.pll_icpmset << 3) | pdb->in.pll_icpcset);
	writel(data, phy_base + DSIPHY_PLL_PLL_ICP_SET);

	data = ((pdb->in.pll_lpf_cap2 << 4) | pdb->in.pll_lpf_cap1);
	writel(data, phy_base + DSIPHY_PLL_PLL_LPF1);

	writel(pdb->in.pll_iptat_trim, phy_base + DSIPHY_PLL_IPTAT_TRIM);

	data = (pdb->in.pll_c3ctrl | (pdb->in.pll_r3ctrl << 4));
	writel(data, phy_base + DSIPHY_PLL_PLL_CRCTRL);
	dmb();
}

static void mdss_mdp_pll_freq_config(uint32_t phy_base, struct dsi_pll_db *pdb)
{
	uint32_t data;

	writel(0x0, phy_base + DSIPHY_CMN_PLL_CNTRL);
	/* reset digital block */
	writel(0x20, phy_base + DSIPHY_CMN_CTRL_1);
	dmb();
	udelay(10);
	writel(0x00, phy_base + DSIPHY_CMN_CTRL_1);
	dmb();

	writel(pdb->in.dsiclk_sel, phy_base + DSIPHY_CMN_CLK_CFG1);
	writel(0xff, phy_base + DSIPHY_CMN_CTRL_0);
	writel(pdb->out.dec_start, phy_base + DSIPHY_PLL_DEC_START);

	data = pdb->out.div_frac_start;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_DIV_FRAC_START1);
	data = (pdb->out.div_frac_start >> 8);
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_DIV_FRAC_START2);
	data = (pdb->out.div_frac_start >> 16);
	data &= 0x0f;
	writel(data, phy_base + DSIPHY_PLL_DIV_FRAC_START3);

	data = pdb->out.plllock_cmp;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_PLLLOCK_CMP1);
	data = (pdb->out.plllock_cmp >> 8);
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_PLLLOCK_CMP2);
	data = (pdb->out.plllock_cmp >> 16);
	data &= 0x03;
	writel(data, phy_base + DSIPHY_PLL_PLLLOCK_CMP3);

	data = ((pdb->in.plllock_cnt << 1) | (pdb->in.plllock_rng << 3));
	writel(data, phy_base + DSIPHY_PLL_PLLLOCK_CMP_EN);

	data = pdb->out.pll_vco_count;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_VCO_COUNT1);
	data = (pdb->out.pll_vco_count >> 8);
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_VCO_COUNT2);

	data = pdb->out.pll_kvco_count;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_KVCO_COUNT1);
	data = (pdb->out.pll_kvco_count >> 8);
	data &= 0x03;
	writel(data, phy_base + DSIPHY_PLL_KVCO_COUNT2);

	data = pdb->out.pll_kvco_code;
	writel(data, phy_base + DSIPHY_PLL_KVCO_CODE);

	/*
	 * tx_band = pll_postdiv
	 * 0: divided by 1 <== for now
	 * 1: divided by 2
	 * 2: divided by 4
	 * 3: divided by 8
	 */
	if (pdb->out.pll_postdiv)
		data = (((pdb->out.pll_postdiv - 1) << 4) |
			pdb->in.pll_lpf_res1);
	else
		data = pdb->in.pll_lpf_res1;
	writel(data, phy_base + DSIPHY_PLL_PLL_LPF2_POSTDIV);

	data = (pdb->out.pll_n1div | (pdb->out.pll_n2div << 4));
	writel(data, phy_base + DSIPHY_CMN_CLK_CFG0);

	dmb();	/* make sure register committed */
}

static void mdss_mdp_pll_ssc_config(uint32_t phy_base, struct dsi_pll_db *pdb)
{
	uint32_t data;

	data = pdb->in.ssc_adj_per;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_SSC_ADJ_PER1);
	data = (pdb->in.ssc_adj_per >> 8);
	data &= 0x03;
	writel(data, phy_base + DSIPHY_PLL_SSC_ADJ_PER2);

	data = pdb->out.ssc_per;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_SSC_PER1);
	data = (pdb->out.ssc_per >> 8);
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_SSC_PER2);

	data = pdb->out.ssc_step_size;
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_SSC_STEP_SIZE1);
	data = (pdb->out.ssc_step_size >> 8);
	data &= 0x0ff;
	writel(data, phy_base + DSIPHY_PLL_SSC_STEP_SIZE2);

	data = (pdb->in.ssc_center_spread & 0x01);
	data <<= 1;
	data |= 0x01; /* enable */
	writel(data, phy_base + DSIPHY_PLL_SSC_EN_CENTER);
}

static int mdss_dsi_phy_14nm_init(struct msm_panel_info *pinfo,
	uint32_t phy_base)
{
	struct mdss_dsi_phy_ctrl *pd;
	struct mipi_panel_info *mipi;
	int j, off, ln, cnt, ln_off;
	uint32_t base;
	uint32_t data;

	mipi = &pinfo->mipi;
	pd = (mipi->mdss_dsi_phy_db);

	/* Strength ctrl 0 */
	writel(0x1c, phy_base + MMSS_DSI_DSIPHY_CMN_LDO_CTRL);
	writel(0x1, phy_base + DSIPHY_CMN_GLBL_TEST_CTRL);

	/* 4 lanes + clk lane configuration */
	for (ln = 0; ln < 5; ln++) {
		/*
		 * data lane offset frome base: 0x100
		 * data lane size: 0x80
		 */
		base = phy_base + DATALANE_OFFSET_FROM_BASE_THULIUM;
		base += (ln * DATALANE_SIZE_THULIUM); /* lane base */

		/* lane cfg, 4 * 5 */
		cnt = 4;
		ln_off = cnt * ln;
		off = 0x0;
		for (j = 0; j < cnt; j++, off += 4)
			writel(pd->laneCfg[ln_off + j], base + off);

		/* test str */
		writel(0x88, base + 0x14);

		/* phy timing, 8 * 5 */
		cnt = 8;
		ln_off = cnt * ln;
		off = 0x18;
		for (j = 0; j < cnt; j++, off += 4)
			writel(pd->timing[ln_off + j], base + off);

		/* strength, 2 * 5 */
		cnt = 2;
		ln_off = cnt * ln;
		off = 0x38;
		for (j = 0; j < cnt; j++, off += 4)
			writel(pd->strength[ln_off + j], base + off);

		/* vreg ctrl, 1 * 5 */
		off = 0x64;
		writel(pd->regulator[cnt], base + off);
	}
	dmb();

	/* reset digital block */
	writel(0x80, phy_base + DSIPHY_CMN_CTRL_1);
	dmb();
	udelay(100);
	writel(0x00, phy_base + DSIPHY_CMN_CTRL_1);

	if (pinfo->lcdc.split_display) {
		if (mipi->phy_base == phy_base) {
			writel(0x3, phy_base + DSIPHY_PLL_CLKBUFLR_EN);
			data = readl(phy_base + DSIPHY_CMN_GLBL_TEST_CTRL);
			data &= ~BIT(2);
			writel(data, phy_base + DSIPHY_CMN_GLBL_TEST_CTRL);
		} else {
			writel(0x0, phy_base + DSIPHY_PLL_CLKBUFLR_EN);
			data = readl(phy_base + DSIPHY_CMN_GLBL_TEST_CTRL);
			data |= BIT(2);
			writel(data, phy_base + DSIPHY_CMN_GLBL_TEST_CTRL);
			writel(0x3, phy_base + DSIPHY_PLL_PLL_BANDGAP);
		}
	} else {
		writel(0x1, phy_base + DSIPHY_PLL_CLKBUFLR_EN);
		data = readl(phy_base + DSIPHY_CMN_GLBL_TEST_CTRL);
		data &= ~BIT(2);
		writel(data, phy_base + DSIPHY_CMN_GLBL_TEST_CTRL);
	}

	dmb();
	return 0;
}

void mdss_dsi_auto_pll_thulium_config(struct msm_panel_info *pinfo)
{
	struct dsi_pll_db pdb;
	struct mdss_dsi_pll_config *pll_data = pinfo->mipi.dsi_pll_config;
	uint32_t phy_base = pinfo->mipi.phy_base;
	uint32_t phy_1_base = pinfo->mipi.sphy_base;

	mdss_dsi_phy_sw_reset(pinfo->mipi.ctl_base);
	if (pinfo->mipi.dual_dsi)
		mdss_dsi_phy_sw_reset(pinfo->mipi.sctl_base);

	mdss_dsi_phy_14nm_init(pinfo, phy_base);
	if (pinfo->mipi.dual_dsi)
		mdss_dsi_phy_14nm_init(pinfo, phy_1_base);

	mdss_mdp_pll_input_init(&pdb);
	pdb.out.pll_postdiv = pll_data->ndiv;
	pdb.out.pll_n1div = pll_data->n1div;
	pdb.out.pll_n2div = pll_data->n2div;

	mdss_mdp_pll_dec_frac_calc(&pdb, pll_data->vco_clock, VCO_REF_CLK_RATE);
	if (pdb.in.ssc_en)
		mdss_mdp_pll_ssc_calc(&pdb, pll_data->vco_clock,
			VCO_REF_CLK_RATE);
	mdss_mdp_pll_calc_vco_count(&pdb, pll_data->vco_clock, VCO_REF_CLK_RATE);

	/* de-assert pll and start */
	mdss_mdp_pll_assert_and_div_cfg(phy_base, &pdb);
	writel(pdb.in.dsiclk_sel, phy_base + DSIPHY_CMN_CLK_CFG1);
	if (pinfo->lcdc.split_display)
		mdss_mdp_pll_assert_and_div_cfg(phy_1_base, &pdb);

	/* configure frequence */
	mdss_mdp_pll_nonfreq_config(phy_base, &pdb);
	mdss_mdp_pll_freq_config(phy_base, &pdb);
	if (pdb.in.ssc_en)
		mdss_mdp_pll_ssc_config(phy_base, &pdb);

	if (pinfo->lcdc.split_display) {
		mdss_mdp_pll_nonfreq_config(phy_1_base, &pdb);
		mdss_mdp_pll_freq_config(phy_1_base, &pdb);
		if (pdb.in.ssc_en)
			mdss_mdp_pll_ssc_config(phy_1_base, &pdb);
	}
}
