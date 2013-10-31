/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

static uint32_t calculate_bitclock(struct msm_panel_info *pinfo)
{
	uint32_t ret = NO_ERROR;
	uint32_t h_period = 0, v_period = 0;
	uint32_t width = pinfo->xres;

	if (pinfo->mipi.dual_dsi)
		width = pinfo->xres / 2;

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

	return ret;
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

static uint32_t calculate_vco(uint8_t bpp, uint8_t num_of_lanes)
{
	uint32_t ret = NO_ERROR;
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

	return ret;
}

uint32_t calculate_clock_config(struct msm_panel_info *pinfo)
{
	uint32_t ret = NO_ERROR;

	calculate_bitclock(pinfo);

	calculate_vco(pinfo->bpp, pinfo->mipi.num_of_lanes);

	pinfo->mipi.dsi_pll_config = &pll_data;

	return ret;
}
