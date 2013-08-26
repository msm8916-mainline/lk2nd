/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#define LPFR_LUT_SIZE 10

#define VCO_REF_CLOCK_RATE 19200000

#define FRAC_DIVIDER 10000

typedef struct lpfr_cfg {
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

int32_t mdss_dsi_auto_pll_config(uint32_t ctl_base,
				struct mdss_dsi_pll_config *pd)
{
	uint32_t rem, divider;
	uint32_t refclk_cfg = 0, frac_n_mode = 0, ref_doubler_en_b = 0;
	uint64_t vco_clock, div_fbx;
	uint32_t ref_clk_to_pll = 0, frac_n_value = 0;
	uint32_t sdm_cfg0, sdm_cfg1, sdm_cfg2, sdm_cfg3;
	uint32_t gen_vco_clk, cal_cfg10, cal_cfg11;
	uint32_t res;
	uint8_t i, rc = NO_ERROR;

	/* Configure the Loop filter resistance */
	for (i = 0; i < LPFR_LUT_SIZE; i++)
		if (pd->vco_clock <= lpfr_lut[i].vco_rate)
			break;
	if (i == LPFR_LUT_SIZE) {
		dprintf(INFO, "unable to get loop filter resistance. vco=%d\n"
						, lpfr_lut[i].vco_rate);
		rc = ERROR;
		return rc;
	}

	mdss_dsi_phy_sw_reset(ctl_base);

	/* Loop filter resistance value */
	writel(lpfr_lut[i].resistance, ctl_base + 0x022c);
	/* Loop filter capacitance values : c1 and c2 */
	writel(0x70, ctl_base + 0x0230);
	writel(0x15, ctl_base + 0x0234);

	writel(0x02, ctl_base + 0x0208); /* ChgPump */
	/* postDiv1 - calculated in pll config*/
	writel(pd->posdiv1, ctl_base + 0x0204);
	/* postDiv2 - fixed devision 4 */
	writel(0x03, ctl_base + 0x0224);
	/* postDiv3 - calculated in pll config */
	writel(pd->posdiv3, ctl_base + 0x0228); /* postDiv3 */

	writel(0x2b, ctl_base + 0x0278); /* Cal CFG3 */
	writel(0x66, ctl_base + 0x027c); /* Cal CFG4 */
	writel(0x05, ctl_base + 0x0264); /* LKDetect CFG2 */

	rem = pd->vco_clock % VCO_REF_CLOCK_RATE;
	if (rem) {
		refclk_cfg = 0x1;
		frac_n_mode = 1;
		ref_doubler_en_b = 0;
	} else {
		refclk_cfg = 0x0;
		frac_n_mode = 0;
		ref_doubler_en_b = 1;
	}

	ref_clk_to_pll = (VCO_REF_CLOCK_RATE * 2 * refclk_cfg)
			  + (ref_doubler_en_b * VCO_REF_CLOCK_RATE);

	vco_clock = ((uint64_t) pd->vco_clock) * FRAC_DIVIDER;

	div_fbx = vco_clock / ref_clk_to_pll;

	rem = (uint32_t) (div_fbx % FRAC_DIVIDER);
	rem = rem * (1 << 16);
	frac_n_value = rem / FRAC_DIVIDER;

	divider = pd->vco_clock / ref_clk_to_pll;
	div_fbx *= ref_clk_to_pll;
	gen_vco_clk = div_fbx / FRAC_DIVIDER;

	if (frac_n_mode) {
		sdm_cfg0 = 0x0;
		sdm_cfg1 = (divider & 0x3f) - 1;
		sdm_cfg3 = frac_n_value / 256;
		sdm_cfg2 = frac_n_value % 256;
	} else {
		sdm_cfg0 = (0x1 << 5);
		sdm_cfg0 |= (divider & 0x3f) - 1;
		sdm_cfg1 = 0x0;
		sdm_cfg2 = 0;
		sdm_cfg3 = 0;
	}

	cal_cfg11 = gen_vco_clk / 256000000;
	cal_cfg10 = (gen_vco_clk % 256000000) / 1000000;

	writel(sdm_cfg1 , ctl_base + 0x023c); /* SDM CFG1 */
	writel(sdm_cfg2 , ctl_base + 0x0240); /* SDM CFG2 */
	writel(sdm_cfg3 , ctl_base + 0x0244); /* SDM CFG3 */
	writel(0x00, ctl_base + 0x0248); /* SDM CFG4 */

	udelay(10);

	writel(refclk_cfg, ctl_base + 0x0200); /* REFCLK CFG */
	writel(0x00, ctl_base + 0x0214); /* PWRGEN CFG */
	writel(0x71, ctl_base + 0x020c); /* VCOLPF CFG */
	writel(pd->directpath, ctl_base + 0x0210); /* VREG CFG */
	writel(sdm_cfg0, ctl_base + 0x0238); /* SDM CFG0 */

	writel(0x0a, ctl_base + 0x026c); /* CAL CFG0 */
	writel(0x30, ctl_base + 0x0284); /* CAL CFG6 */
	writel(0x00, ctl_base + 0x0288); /* CAL CFG7 */
	writel(0x60, ctl_base + 0x028c); /* CAL CFG8 */
	writel(0x00, ctl_base + 0x0290); /* CAL CFG9 */
	writel(cal_cfg10, ctl_base + 0x0294); /* CAL CFG10 */
	writel(cal_cfg11, ctl_base + 0x0298); /* CAL CFG11 */
	writel(0x20, ctl_base + 0x029c); /* EFUSE CFG */
}
