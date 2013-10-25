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

#include "edp.h"
#include "mdp5.h"

#define RGB_COMPONENTS		3
#define MAX_NUMBER_EDP_LANES	4

struct edp_aux_ctrl edpctrl;

static struct msm_panel_info *edp_pinfo;

static void edp_config_ctrl(void)
{
	struct edp_aux_ctrl *ep;
	struct dpcd_cap *cap;
	struct display_timing_desc *dp;
	unsigned long data = 0;

	ep = &edpctrl;

	dp = &ep->edid.timing[0];

	cap = &ep->dpcd;

	data = cap->max_lane_count - 1;
	data <<= 4;

	if (cap->enhanced_frame)
		data |= 0x40;

	if (ep->edid.color_depth == 8) {
		/* 0 == 6 bits, 1 == 8 bits */
		data |= 0x100;  /* bit 8 */
	}

	if (!dp->interlaced)    /* progressive */
		data |= 0x04;

	data |= 0x03;   /* sycn clock & static Mvid */

	dprintf(SPEW, "%s: data=%x\n", __func__, data);

	edp_write(EDP_BASE + 0xc, data); /* EDP_CONFIGURATION_CTRL */
}

static void edp_config_sw_mvid_nvid(void)
{
	edp_write(EDP_BASE + 0x14, 0x13b); /* EDP_SOFTWARE_MVID */
	edp_write(EDP_BASE + 0x18, 0x266); /* EDP_SOFTWARE_NVID */
}

void edp_clock_synchrous(void)
{
	struct edp_aux_ctrl *ep;
	unsigned long data;
	unsigned long color;

	ep = &edpctrl;

	data = 1;	/* sync */

	/* only legacy rgb mode supported */
	color = 0; /* 6 bits */
	if (ep->edid.color_depth == 8)
		color = 0x01;
	else if (ep->edid.color_depth == 10)
		color = 0x02;
	else if (ep->edid.color_depth == 12)
		color = 0x03;
	else if (ep->edid.color_depth == 16)
		color = 0x04;

	color <<= 5;    /* bit 5 to bit 7 */

	data |= color;

	dprintf(SPEW, "%s: data=%x\n", __func__, data);

	/* EDP_MISC1_MISC0 */
	edp_write(EDP_BASE + 0x2c, data);
}

static void edp_config_tu(void)
{
	/* temporary */
	edp_write(EDP_BASE + 0x160, 0x2b);
	edp_write(EDP_BASE + 0x15c, 0x00320033);
	edp_write(EDP_BASE + 0x34, 0x0023001a);

}

static void edp_config_timing(struct msm_panel_info *pinfo)
{
	unsigned long total_ver, total_hor;
	unsigned long data;

	dprintf(INFO, "%s: width=%d hporch= %d %d %d\n", __func__,
		pinfo->xres, pinfo->lcdc.h_back_porch,
		pinfo->lcdc.h_front_porch, pinfo->lcdc.h_pulse_width);

	dprintf(INFO, "%s: height=%d vporch= %d %d %d\n", __func__,
		pinfo->yres, pinfo->lcdc.v_back_porch,
		pinfo->lcdc.v_front_porch, pinfo->lcdc.v_pulse_width);

	total_hor = pinfo->xres + pinfo->lcdc.h_back_porch +
			pinfo->lcdc.h_front_porch + pinfo->lcdc.h_pulse_width;

	total_ver = pinfo->yres + pinfo->lcdc.v_back_porch +
			pinfo->lcdc.v_front_porch + pinfo->lcdc.v_pulse_width;

	data = total_ver;
	data <<= 16;
	data |= total_hor;
	edp_write(EDP_BASE + 0x1c, data); /* EDP_TOTAL_HOR_VER */

	data = (pinfo->lcdc.v_back_porch + pinfo->lcdc.v_pulse_width);
	data <<= 16;
	data |= (pinfo->lcdc.h_back_porch + pinfo->lcdc.h_pulse_width);
	edp_write(EDP_BASE + 0x20, data); /* EDP_START_HOR_VER_FROM_SYNC */

	data = pinfo->lcdc.v_pulse_width;
	data <<= 16;
	data |= pinfo->lcdc.h_pulse_width;
	edp_write(EDP_BASE + 0x24, data); /* EDP_HSYNC_VSYNC_WIDTH_POLARITY */

	data = pinfo->yres;
	data <<= 16;
	data |= pinfo->xres;
	edp_write(EDP_BASE + 0x28, data); /* EDP_ACTIVE_HOR_VER */
}

static void edp_enable(int enable)
{
	edp_write(EDP_BASE + 0x8, 0x0); /* EDP_STATE_CTRL */
	edp_write(EDP_BASE + 0x8, 0x40); /* EDP_STATE_CTRL */
	edp_write(EDP_BASE + 0x4, 0x01); /* EDP_MAINLINK_CTRL */
}

static void edp_disable(int enable)
{
	edp_write(EDP_BASE + 0x8, 0x0); /* EDP_STATE_CTRL */
	edp_write(EDP_BASE + 0x4, 0x00); /* EDP_MAINLINK_CTRL */
}

int edp_on(void)
{
	mdss_edp_pll_configure();
	mdss_edp_phy_pll_ready();
	edp_phy_vm_pe_init();
	edp_config_ctrl();
	edp_config_sw_mvid_nvid();
	edp_clock_synchrous();
	edp_config_timing(edp_pinfo);
	edp_config_tu();

	edp_config_clk();
	mdss_edp_lane_power_ctrl(1);

	edp_enable_mainlink(1);

	mdss_edp_link_train();

	edp_enable(1);

	mdss_edp_wait_for_video_ready();

	mdss_edp_irq_disable();
	dprintf(SPEW, "%s:\n", __func__);

	return 0;
}

int edp_off(void)
{
	mdss_edp_irq_disable();
	edp_enable_mainlink(0);
	edp_phy_pll_reset();
	edp_mainlink_reset();
	edp_aux_reset();

	edp_disable(1);
	edp_unconfig_clk();

	mdss_edp_lane_power_ctrl(0);
	edp_phy_powerup(0);

	dprintf(SPEW, "%s:\n", __func__);

	return 0;
}

int edp_prepare(void)
{

	mdss_edp_aux_init();
	edp_phy_pll_reset();
	edp_mainlink_reset();
	edp_aux_reset();
	edp_phy_powerup(1);
	edp_aux_enable();
	mdss_edp_irq_enable();

	mdss_edp_wait_for_hpd();

	mdss_edp_edid_read();
	mdss_edp_dpcd_cap_read();

	edp_edid2pinfo(edp_pinfo);
	edp_cap2pinfo(edp_pinfo);

	dprintf(SPEW, "%s:\n", __func__);

	return 0;
}

void edp_panel_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->lcdc.dual_pipe = 0;
	pinfo->lcdc.split_display = 0;

	edp_pinfo = pinfo;
	edp_pinfo->on = edp_on;
        edp_pinfo->off = edp_off;
	edp_pinfo->prepare = edp_prepare;
}
