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

static void edp_config_sync(void)
{
	int ret = 0;

	ret = edp_read(EDP_BASE + 0xc); /* EDP_CONFIGURATION_CTRL */
	ret &= ~0x733;
	ret |= (0x55 & 0x733);
	edp_write(EDP_BASE + 0xc, ret);
	edp_write(EDP_BASE + 0xc, 0x55); /* EDP_CONFIGURATION_CTRL */
}

static void edp_config_sw_div(void)
{
	edp_write(EDP_BASE + 0x14, 0x13b); /* EDP_SOFTWARE_MVID */
	edp_write(EDP_BASE + 0x18, 0x266); /* EDP_SOFTWARE_NVID */
}

static void edp_config_static_mdiv(void)
{
	int ret = 0;

	ret = edp_read(EDP_BASE + 0xc); /* EDP_CONFIGURATION_CTRL */
	edp_write(EDP_BASE + 0xc, ret | 0x2); /* EDP_CONFIGURATION_CTRL */
	edp_write(EDP_BASE + 0xc, 0x57); /* EDP_CONFIGURATION_CTRL */
}

static void edp_config_timing(void)
{
	edp_write(EDP_BASE + 0x98, 0);
	edp_write(EDP_BASE + 0x9C, 0x8200020); /* EDP_HSYNC_CTL */
	edp_write(EDP_BASE + 0x100, 0x233AC0); /* EDP_VSYNC_PERIOD_F0 */
	edp_write(EDP_BASE + 0x104, 0x0);
	edp_write(EDP_BASE + 0x10C, 0x0);
	edp_write(EDP_BASE + 0x130, 0x7ef0070); /* EDP_DISPLAY_HCTL */
	edp_write(EDP_BASE + 0x134, 0x0); /* EDP_ACTIVE_HCTL */
	edp_write(EDP_BASE + 0x110, 0xB330); /* EDP_DISPLAY_V_START_F0 */
	edp_write(EDP_BASE + 0x118, 0x22f98f); /* EDP_DISPLAY_V_END_F0 */
	edp_write(EDP_BASE + 0x114, 0x0);
	edp_write(EDP_BASE + 0x11C, 0x0);
	edp_write(EDP_BASE + 0x120, 0x0); /* EDP_ACTIVE_V_START_F0 */
	edp_write(EDP_BASE + 0x128, 0x0); /* EDP_ACTIVE_V_END_F0 */
	edp_write(EDP_BASE + 0x124, 0x0);
	edp_write(EDP_BASE + 0x12C, 0x0);
}

static void edp_enable(int enable)
{
	edp_write(EDP_BASE + 0x8, 0x0); /* EDP_STATE_CTRL */
	edp_write(EDP_BASE + 0x8, 0x40); /* EDP_STATE_CTRL */
	edp_write(EDP_BASE + 0x94, enable); /* EDP_TIMING_ENGINE_EN */
	edp_write(EDP_BASE + 0x4, enable); /* EDP_MAINLINK_CTRL */
}

/*
 * Converts from EDID struct to msm_panel_info
 */
void edp_edid2pinfo(struct edp_panel_data *edp_panel)
{
	struct display_timing_desc *dp;
	struct msm_panel_info *pinfo;

	dp = &edp_panel->edid.timing[0];
	pinfo = &edp_panel->panel_data->panel_info;

	pinfo->clk_rate = dp->pclk;

	pinfo->xres = dp->h_addressable + dp->h_border * 2;
	pinfo->yres = dp->v_addressable + dp->v_border * 2;

	pinfo->lcdc.h_back_porch = dp->h_blank - dp->h_fporch \
		- dp->h_sync_pulse;
	pinfo->lcdc.h_front_porch = dp->h_fporch;
	pinfo->lcdc.h_pulse_width = dp->h_sync_pulse;

	pinfo->lcdc.v_back_porch = dp->v_blank - dp->v_fporch \
		- dp->v_sync_pulse;
	pinfo->lcdc.v_front_porch = dp->v_fporch;
	pinfo->lcdc.v_pulse_width = dp->v_sync_pulse;

	pinfo->type = EDP_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;

	pinfo->lcdc.border_clr = 0;	 /* black */
	pinfo->lcdc.underflow_clr = 0xff; /* blue */
	pinfo->lcdc.hsync_skew = 0;
}

int edp_on(void)
{
	int i;

	edp_phy_sw_reset();
	edp_pll_configure();
	edp_config_clk();
	edp_phy_misc_cfg();
	edp_config_sync();
	edp_config_sw_div();
	edp_config_static_mdiv();
	edp_config_timing();

	edp_hw_powerup(1);

	for (i = 0; i < MAX_NUMBER_EDP_LANES; ++i)
		edp_enable_lane_bist(i, 1);

	edp_enable_mainlink(1);
	edp_enable(1);

	return 0;
}

int edp_off(void)
{
	int i;

	mdp_edp_off();
	edp_enable(0);
	edp_unconfig_clk();
	edp_enable_mainlink(0);

	for (i = 0; i < MAX_NUMBER_EDP_LANES; ++i)
		edp_enable_lane_bist(i, 0);

	edp_hw_powerup(0);

	return 0;
}
