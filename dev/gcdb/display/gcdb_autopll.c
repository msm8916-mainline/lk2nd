/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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
#include <smem.h>
#include <msm_panel.h>
#include <mipi_dsi.h>

#include "gcdb_autopll.h"

static struct mdss_dsi_pll_config pll_data;

static void calculate_bitclock(struct msm_panel_info *pinfo)
{
	uint32_t h_period = 0, v_period = 0;
	uint32_t width = pinfo->xres;

	if (pinfo->mipi.dual_dsi)
		width /= 2;

	if (pinfo->fbc.enabled && pinfo->fbc.comp_ratio)
		width /= pinfo->fbc.comp_ratio;

	h_period = width + pinfo->lcdc.h_back_porch +
		pinfo->lcdc.h_front_porch + pinfo->lcdc.h_pulse_width +
		pinfo->lcdc.xres_pad;

	v_period = pinfo->yres + pinfo->lcdc.v_back_porch +
		pinfo->lcdc.v_front_porch + pinfo->lcdc.v_pulse_width +
		pinfo->lcdc.yres_pad;

	/* Pixel clock rate */
	pll_data.pixel_clock = h_period * v_period * pinfo->mipi.frame_rate;

	/* Store all bit clock form data */
	if (pinfo->mipi.bitclock == 0)
		pll_data.bit_clock = (pll_data.pixel_clock * pinfo->bpp) /
					pinfo->mipi.num_of_lanes;
	else
		pll_data.bit_clock = pinfo->mipi.bitclock;

	pll_data.byte_clock = pll_data.bit_clock >> 3;

	pll_data.halfbit_clock = pll_data.bit_clock >> 1;
}

static uint32_t calculate_div1()
{
	uint32_t ret = NO_ERROR;

	/* div1 - there is divide by 2 logic present */
	if (pll_data.halfbit_clock > HALFBIT_CLOCK1) {
		pll_data.posdiv1    = 0x0; /*div 1 */
		pll_data.vco_clock  = pll_data.halfbit_clock << 1;
	} else if (pll_data.halfbit_clock > HALFBIT_CLOCK2) {
		pll_data.posdiv1    = 0x1; /*div 2 */
		pll_data.vco_clock  = pll_data.halfbit_clock << 2;
	} else if (pll_data.halfbit_clock > HALFBIT_CLOCK3) {
		pll_data.posdiv1    = 0x3; /*div 4 */
		pll_data.vco_clock  = pll_data.halfbit_clock << 3;
	} else if (pll_data.halfbit_clock > HALFBIT_CLOCK4) {
		pll_data.posdiv1    = 0x4; /*div 5 */
		pll_data.vco_clock  = pll_data.halfbit_clock * 10;
	} else {
		dprintf(CRITICAL, "Not able to calculate posdiv1\n");
	}

	return ret;
}

static uint32_t calculate_div3(uint8_t bpp, uint8_t num_of_lanes)
{
	uint32_t ret = NO_ERROR;
	pll_data.pclk_m = 0x1; /* M = 1, N= 1 */
	pll_data.pclk_n = 0xFF; /* ~ (N-M) = 0xff */
	pll_data.pclk_d = 0xFF; /* ~N = 0xFF */

	/* formula is ( vco_clock / pdiv_digital) / mnd = pixel_clock */

	/* div3  */
	switch (bpp) {
	case BITS_18:
		if (num_of_lanes == 3) {
			pll_data.posdiv3 = pll_data.vco_clock /
					 pll_data.pixel_clock;
		} else {
			pll_data.posdiv3 = (pll_data.pixel_clock * 2 / 9) /
					 pll_data.vco_clock;
			pll_data.pclk_m = 0x2; /* M = 2,N = 9 */
			pll_data.pclk_n = 0xF8;
			pll_data.pclk_d = 0xF6;
		}
		break;
	case BITS_16:
		if (num_of_lanes == 3) {
			pll_data.posdiv3 = (pll_data.pixel_clock * 3 / 8) /
					 pll_data.vco_clock;
			pll_data.pclk_m = 0x3; /* M = 3, N = 9 */
			pll_data.pclk_n = 0xFA;
			pll_data.pclk_d = 0xF7;
		} else {
			pll_data.posdiv3 = pll_data.vco_clock /
					 pll_data.pixel_clock;
		}
		break;
	case BITS_24:
	default:
		pll_data.posdiv3 = pll_data.vco_clock /
					 pll_data.pixel_clock;
		break;
	}

	pll_data.posdiv3--;	/* Register needs one value less */
}

static uint32_t calculate_dec_frac_start()
{
	uint32_t refclk = 19200000;
	uint32_t vco_rate = pll_data.vco_clock;
	uint32_t tmp, mod;

	vco_rate /= 2;
	pll_data.dec_start = vco_rate / refclk;
	tmp = vco_rate % refclk; /* module, fraction */
	tmp /= 192;
	tmp *= 1024;
	tmp /= 100;
	tmp *= 1024;
	tmp /= 1000;
	pll_data.frac_start = tmp;

	vco_rate *= 2; /* restore */
	tmp = vco_rate / refclk;/* div 1000 first */
	mod = vco_rate % refclk;
	tmp *= 127;
	mod *= 127;
	mod /= refclk;
	tmp += mod;
	tmp /= 10;
	pll_data.lock_comp = tmp;

	dprintf(SPEW, "%s: dec_start=%u dec_frac=%u lock_comp=%u\n", __func__,
		pll_data.dec_start, pll_data.frac_start, pll_data.lock_comp);
}

static uint32_t calculate_vco_28nm(uint8_t bpp, uint8_t num_of_lanes)
{
	uint8_t  counter = 0;
	uint32_t temprate = 0;

	/* If half bitclock is more than VCO min value */
	if (pll_data.halfbit_clock > VCO_MIN_CLOCK) {

		/* Direct Mode */

		/* support vco clock to max value only */
		if (pll_data.halfbit_clock > VCO_MAX_CLOCK)
			pll_data.vco_clock = VCO_MAX_CLOCK;
		else
			pll_data.vco_clock = pll_data.halfbit_clock;
		pll_data.directpath = 0x0;
		pll_data.posdiv1    = 0x0; /*DSI spec says 0 - div 1 */
					    /*1 - div 2 */
					    /*F - div 16 */
	} else {
		/* Indirect Mode */

		pll_data.directpath = 0x02; /* set bit 1 to enable for
						 indirect path */

		calculate_div1();
	}

	/* calculate mnd and div3 for direct and indirect path */
	calculate_div3(bpp, num_of_lanes);

	return NO_ERROR;
}

static uint32_t calculate_vco_20nm(uint8_t bpp, uint8_t lanes)
{
	uint32_t vco, dsi_clk;
	int mod, ndiv, hr_oclk2, hr_oclk3;
	int m = 1;
	int n = 1;
	int bpp_m = 3;	/* bpp = 3 */
	int bpp_n = 1;

	if (bpp == BITS_18) {
		bpp_m = 9; /* bpp = 2.25 */
		bpp_n = 4;

		if (lanes == 2) {
			m = 2;
			n = 9;
		} else if (lanes == 4) {
			m = 4;
			n = 9;
		}
	} else if (bpp == BITS_16) {
		bpp_m = 2; /* bpp = 2 */
		bpp_n = 1;
		if (lanes == 3) {
			m = 3;
			n = 8;
		}
	}

	hr_oclk2 = 4;

	/* If bitclock is more than VCO min value */
	if (pll_data.halfbit_clock >= HALF_VCO_MIN_CLOCK_20NM) {
		/* Direct Mode */
		vco  = pll_data.halfbit_clock << 1;
		/* support vco clock to max value only */
		if (vco > VCO_MAX_CLOCK_20NM)
			vco = VCO_MAX_CLOCK_20NM;

		pll_data.directpath = 0x0;
		pll_data.byte_clock = vco / 2 / hr_oclk2;
		pll_data.lp_div_mux = 0x0;
		ndiv = 1;
		hr_oclk3 = hr_oclk2 * m / n * bpp_m / bpp_n / lanes;
	} else {
		/* Indirect Mode */
		mod =  VCO_MIN_CLOCK_20NM % (4 * pll_data.halfbit_clock );
		ndiv = VCO_MIN_CLOCK_20NM / (4 * pll_data.halfbit_clock );
		if (mod)
			ndiv += 1;

		vco = pll_data.halfbit_clock * 4 * ndiv;
		pll_data.lp_div_mux = 0x1;
		pll_data.directpath = 0x02; /* set bit 1 to enable for
						 indirect path */

		pll_data.byte_clock = vco / 4 / hr_oclk2 / ndiv;
		hr_oclk3 = hr_oclk2 * m / n  * ndiv * 2 * bpp_m / bpp_n / lanes;
	}

	pll_data.vco_clock = vco;
	dsi_clk = vco / 2 / hr_oclk3;
	pll_data.ndiv = ndiv;
	pll_data.hr_oclk2 = hr_oclk2 - 1; 	/* strat from 0 */
	pll_data.hr_oclk3 = hr_oclk3 - 1; 	/* strat from 0 */

	pll_data.pclk_m = m;		/* M */
	pll_data.pclk_n = ~(n - m); 	/* ~(N-M) */
	pll_data.pclk_d = ~n;		/* ~N  */

	dprintf(SPEW, "%s: oclk2=%d oclk3=%d ndiv=%d vco=%u dsi_clk=%u byte_clk=%u\n",
	__func__, hr_oclk2, hr_oclk3, ndiv, vco, dsi_clk, pll_data.byte_clock);

	calculate_dec_frac_start();

	return NO_ERROR;
}

uint32_t calculate_clock_config(struct msm_panel_info *pinfo)
{
	uint32_t ret = NO_ERROR;

	calculate_bitclock(pinfo);

	if (pinfo->mipi.mdss_dsi_phy_db->is_pll_20nm)
		ret = calculate_vco_20nm(pinfo->bpp, pinfo->mipi.num_of_lanes);
	else
		ret = calculate_vco_28nm(pinfo->bpp, pinfo->mipi.num_of_lanes);

	pinfo->mipi.dsi_pll_config = &pll_data;

	return ret;
}
