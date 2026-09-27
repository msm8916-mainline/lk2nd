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
 *
 */
#include <debug.h>
#include <reg.h>
#include <err.h>
#include <smem.h>
#include <mipi_dsi.h>
#include <platform/iomap.h>
#include <platform/timer.h>

#define LPFR_LUT_SIZE 10

#define VCO_REF_CLOCK_RATE 19200000

#define FRAC_DIVIDER 10000

struct lpfr_cfg {
	uint32_t vco_rate;
	uint8_t  resistance;
};

static struct lpfr_cfg lpfr_lut[LPFR_LUT_SIZE] = {
		{479500000, 8},
		{480000000, 11},
		{575500000, 8},
		{576000000, 12},
		{610500000, 8},
		{659500000, 9},
		{671500000, 10},
		{672000000, 14},
		{708500000, 10},
		{750000000, 11},
	};

uint64_t div_s64(uint64_t dividend, uint32_t divisor, uint32_t *remainder)
{
	*remainder = dividend % divisor;

	return dividend / divisor;
}

void mdss_dsi_uniphy_pll_lock_detect_setting(uint32_t pll_base)
{
	writel(0x0c, pll_base + 0x0064); /* LKDetect CFG2 */
	udelay(100);
	writel(0x0d, pll_base + 0x0064); /* LKDetect CFG2 */
	mdelay(1);
}

void mdss_dsi_uniphy_pll_sw_reset(uint32_t pll_base)
{
	writel(0x01, pll_base + 0x0068); /* PLL TEST CFG */
	udelay(1);
	writel(0x00, pll_base + 0x0068); /* PLL TEST CFG */
	udelay(1);
}

uint64_t div_round_closest_unsigned(uint64_t dividend, uint64_t divisor)
{
	return ((dividend + (divisor / 2)) / divisor);
}

static int32_t mdss_dsi_pll_vco_rate_calc(struct mdss_dsi_pll_config *pd,
	struct mdss_dsi_vco_calc *vco_calc)
{
	uint8_t i;
	int8_t rc = NO_ERROR;
	uint32_t rem;
	uint64_t frac_n_mode = 0, ref_doubler_en_b = 0;
	uint64_t div_fb = 0, frac_n_value = 0, ref_clk_to_pll = 0;

	/* Configure the Loop filter resistance */
	for (i = 0; i < LPFR_LUT_SIZE; i++)
		if (pd->vco_clock <= lpfr_lut[i].vco_rate)
			break;
	if (i == LPFR_LUT_SIZE) {
		dprintf(INFO, "unable to get loop filter resistance. vco=%d\n"
						, lpfr_lut[i-1].vco_rate);
		rc = ERROR;
		return rc;
	}
	vco_calc->lpfr_lut_res = lpfr_lut[i].resistance;

	rem = pd->vco_clock % VCO_REF_CLOCK_RATE;
	if (rem) {
		vco_calc->refclk_cfg = 0x1;
		frac_n_mode = 1;
		ref_doubler_en_b = 0;
	} else {
		vco_calc->refclk_cfg = 0x0;
		frac_n_mode = 0;
		ref_doubler_en_b = 1;
	}

	ref_clk_to_pll = (VCO_REF_CLOCK_RATE * 2 * vco_calc->refclk_cfg)
			  + (ref_doubler_en_b * VCO_REF_CLOCK_RATE);
	div_fb = div_s64(pd->vco_clock, ref_clk_to_pll, &rem);
	frac_n_value = ((uint64_t) rem * (1 << 16)) / ref_clk_to_pll;
	vco_calc->gen_vco_clk = pd->vco_clock;

	if (frac_n_mode) {
		vco_calc->sdm_cfg0 = 0x0;
		vco_calc->sdm_cfg1 = (div_fb & 0x3f) - 1;
		vco_calc->sdm_cfg3 = frac_n_value / 256;
		vco_calc->sdm_cfg2 = frac_n_value % 256;
	} else {
		vco_calc->sdm_cfg0 = (0x1 << 5);
		vco_calc->sdm_cfg0 |= (div_fb & 0x3f) - 1;
		vco_calc->sdm_cfg1 = 0x0;
		vco_calc->sdm_cfg2 = 0;
		vco_calc->sdm_cfg3 = 0;
	}

	vco_calc->cal_cfg11 = vco_calc->gen_vco_clk / 256000000;
	vco_calc->cal_cfg10 = (vco_calc->gen_vco_clk % 256000000) / 1000000;

	return NO_ERROR;

}

static void mdss_dsi_ssc_param_calc(struct mdss_dsi_pll_config *pd,
	struct mdss_dsi_vco_calc *vco_calc)
{
	uint64_t ppm_freq, incr, spread_freq, div_rf, frac_n_value;
	uint32_t rem;

	vco_calc->ssc.kdiv = div_round_closest_unsigned(VCO_REF_CLOCK_RATE,
		1000000) - 1;
	vco_calc->ssc.triang_steps = div_round_closest_unsigned(
		VCO_REF_CLOCK_RATE, pd->ssc_freq * (vco_calc->ssc.kdiv + 1));
	ppm_freq = (vco_calc->gen_vco_clk * pd->ssc_ppm) / 1000000;
	incr = (ppm_freq * 65536) / (VCO_REF_CLOCK_RATE * 2 *
		vco_calc->ssc.triang_steps);

	vco_calc->ssc.triang_inc_7_0 = incr & 0xff;
	vco_calc->ssc.triang_inc_9_8 = (incr >> 8) & 0x3;

	if (!pd->is_center_spread)
		spread_freq = vco_calc->gen_vco_clk - ppm_freq;
	else
		spread_freq = vco_calc->gen_vco_clk - (ppm_freq / 2);

	div_rf = spread_freq / (2 * VCO_REF_CLOCK_RATE);
	vco_calc->ssc.dc_offset = (div_rf - 1);

	div_s64(spread_freq, 2 * VCO_REF_CLOCK_RATE, &rem);
	frac_n_value = ((uint64_t) rem * 65536) / (2 * VCO_REF_CLOCK_RATE);

	vco_calc->ssc.freq_seed_7_0 = frac_n_value & 0xff;
	vco_calc->ssc.freq_seed_15_8 = (frac_n_value >> 8) & 0xff;

}

static void mdss_dsi_pll_vco_config(uint32_t pll_base, struct mdss_dsi_pll_config *pd,
	struct mdss_dsi_vco_calc *vco_calc)
{
	/* Loop filter resistance value */
	writel(vco_calc->lpfr_lut_res, pll_base + 0x002c);
	/* Loop filter capacitance values : c1 and c2 */
	writel(0x70, pll_base + 0x0030);
	writel(0x15, pll_base + 0x0034);

	writel(0x02, pll_base + 0x0008); /* ChgPump */
	/* postDiv1 - calculated in pll config*/
	writel(pd->posdiv1, pll_base + 0x0004);
	/* postDiv2 - fixed devision 4 */
	writel(0x03, pll_base + 0x0024);
	/* postDiv3 - calculated in pll config */
	writel(pd->posdiv3, pll_base + 0x0028); /* postDiv3 */

	writel(0x2b, pll_base + 0x0078); /* Cal CFG3 */
	writel(0x66, pll_base + 0x007c); /* Cal CFG4 */
	writel(0x05, pll_base + 0x0064); /* LKDetect CFG2 */

	if (!pd->ssc_en) {
		writel(vco_calc->sdm_cfg1 , pll_base + 0x003c); /* SDM CFG1 */
		writel(vco_calc->sdm_cfg2 , pll_base + 0x0040); /* SDM CFG2 */
		writel(vco_calc->sdm_cfg3 , pll_base + 0x0044); /* SDM CFG3 */
	} else {
		writel(vco_calc->ssc.dc_offset , pll_base + 0x003c); /* SDM CFG1 */
		writel(vco_calc->ssc.freq_seed_7_0, pll_base + 0x0040); /* SDM CFG2 */
		writel(vco_calc->ssc.freq_seed_15_8 , pll_base + 0x0044); /* SDM CFG3 */
		writel(vco_calc->ssc.kdiv, pll_base + 0x4c); /* SSC CFG0 */
		writel(vco_calc->ssc.triang_inc_7_0, pll_base + 0x50); /* SSC CFG1 */
		writel(vco_calc->ssc.triang_inc_9_8, pll_base + 0x54); /* SSC CFG2 */
		writel(vco_calc->ssc.triang_steps, pll_base + 0x58); /* SSC CFG3 */
	}

	writel(0x00, pll_base + 0x0048); /* SDM CFG4 */

	if (pd->vco_delay)
		udelay(pd->vco_delay);
	else
		udelay(10);

	writel(vco_calc->refclk_cfg, pll_base + 0x0000); /* REFCLK CFG */
	writel(0x00, pll_base + 0x0014); /* PWRGEN CFG */
	writel(0x71, pll_base + 0x000c); /* VCOLPF CFG */
	writel(pd->directpath, pll_base + 0x0010); /* VREG CFG */
	writel(vco_calc->sdm_cfg0, pll_base + 0x0038); /* SDM CFG0 */

	writel(0x0a, pll_base + 0x006c); /* CAL CFG0 */
	writel(0x30, pll_base + 0x0084); /* CAL CFG6 */
	writel(0x00, pll_base + 0x0088); /* CAL CFG7 */
	writel(0x60, pll_base + 0x008c); /* CAL CFG8 */
	writel(0x00, pll_base + 0x0090); /* CAL CFG9 */
	writel(vco_calc->cal_cfg10, pll_base + 0x0094); /* CAL CFG10 */
	writel(vco_calc->cal_cfg11, pll_base + 0x0098); /* CAL CFG11 */
	writel(0x20, pll_base + 0x009c); /* EFUSE CFG */
}

int32_t mdss_dsi_auto_pll_config(uint32_t pll_base, uint32_t ctl_base,
				struct mdss_dsi_pll_config *pd)
{
	int rc = NO_ERROR;
	struct mdss_dsi_vco_calc vco_calc;


	rc = mdss_dsi_pll_vco_rate_calc(pd, &vco_calc);
	if (rc)
		return rc;

	mdss_dsi_ssc_param_calc(pd, &vco_calc);

	mdss_dsi_phy_sw_reset(ctl_base);

	mdss_dsi_pll_vco_config(pll_base, pd, &vco_calc);

	return rc;
}
