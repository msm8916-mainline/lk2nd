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
#include <string.h>
#include <stdlib.h>
#include <board.h>
#include <mdp5.h>
#include <platform/gpio.h>
#include <mipi_dsi.h>

#include "include/display_resource.h"
#include "include/panel.h"
#include "panel_display.h"
#include "gcdb_display.h"
#include "target/display.h"
#include "gcdb_autopll.h"

/*---------------------------------------------------------------------------*/
/* static                                                                    */
/*---------------------------------------------------------------------------*/
static struct msm_fb_panel_data panel;
struct panel_struct panelstruct;
static uint8_t display_enable;
static struct mdss_dsi_phy_ctrl dsi_video_mode_phy_db;

/*---------------------------------------------------------------------------*/
/* Extern                                                                    */
/*---------------------------------------------------------------------------*/
extern int msm_display_init(struct msm_fb_panel_data *pdata);
extern int msm_display_off();

static uint32_t panel_backlight_ctrl(uint8_t enable)
{
	return target_backlight_ctrl(panelstruct.backlightinfo, enable);
}

static uint32_t mdss_dsi_panel_reset(uint8_t enable)
{
	uint32_t ret = NO_ERROR;

	ret = target_panel_reset(enable, panelstruct.panelresetseq,
						&panel.panel_info);

	return ret;
}

static uint32_t mdss_dsi_panel_clock(uint8_t enable,
				struct msm_panel_info *pinfo)
{
	uint32_t ret = NO_ERROR;

	ret = calculate_clock_config(pinfo);
	if (ret) {
		dprintf(CRITICAL, "Clock calculation failed \n");
		/* should it stop here ? check with display team */
	}

	ret = target_panel_clock(enable, pinfo);

	return ret;
}

static int mdss_dsi_panel_power(uint8_t enable)
{
	int ret = NO_ERROR;

	if (enable) {
		ret = target_ldo_ctrl(enable);
		if (ret) {
			dprintf(CRITICAL, "LDO control enable failed\n");
			return ret;
		}

		/* Panel Reset */
		if (!panelstruct.paneldata->panel_lp11_init) {
			ret = mdss_dsi_panel_reset(enable);
			if (ret) {
				dprintf(CRITICAL, "panel reset failed\n");
				return ret;
			}
		}
		dprintf(SPEW, "Panel power on done\n");
	} else {
		/* Disable panel and ldo */
		ret = mdss_dsi_panel_reset(enable);
		if (ret) {
			dprintf(CRITICAL, "panel reset disable failed\n");
			return ret;
		}

		ret = target_ldo_ctrl(enable);
		if (ret) {
			dprintf(CRITICAL, "ldo control disable failed\n");
			return ret;
		}
		dprintf(SPEW, "Panel power off done\n");
	}

	return ret;
}

static int mdss_dsi_panel_pre_init(void)
{
	int ret = NO_ERROR;

	if (panelstruct.paneldata->panel_lp11_init) {
		ret = mdss_dsi_panel_reset(1);
		if (ret) {
			dprintf(CRITICAL, "panel reset failed\n");
			return ret;
		}
	}

	if(panelstruct.paneldata->panel_init_delay)
		udelay(panelstruct.paneldata->panel_init_delay);

	dprintf(SPEW, "Panel pre init done\n");
	return ret;
}

static int mdss_dsi_bl_enable(uint8_t enable)
{
	int ret = NO_ERROR;

	ret = panel_backlight_ctrl(enable);
	if (ret)
		dprintf(CRITICAL, "Backlight %s failed\n", enable ? "enable" :
							"disable");
	return ret;
}

bool target_display_panel_node(char *pbuf, uint16_t buf_size)
{
	char *dsi_id = NULL;
	char *panel_node = NULL;
	uint16_t dsi_id_len = 0;
	bool ret = true;
	char *default_str;

	if (panelstruct.paneldata)
	{
		dsi_id = panelstruct.paneldata->panel_controller;
		panel_node = panelstruct.paneldata->panel_node_id;
	}
	else
	{
		if (target_is_edp())
		{
			default_str = "0:edp:";
		}
		else
		{
			default_str = "0:dsi:0:";
		}
		strlcpy(pbuf, default_str, buf_size);
		return true;
	}

	if (dsi_id == NULL || panel_node == NULL)
		return false;

	dsi_id_len = strlen(dsi_id);

	if (buf_size < (strlen(panel_node) + dsi_id_len +
			MAX_PANEL_FORMAT_STRING + 1) ||
		!strlen(panel_node) ||
		!strlen(dsi_id))
	{
		ret = false;
	}
	else
	{
		pbuf[0] = '1'; // 1 indicates that LK is overriding the panel
		pbuf[1] = ':'; // seperator
		pbuf += MAX_PANEL_FORMAT_STRING;
		buf_size -= MAX_PANEL_FORMAT_STRING;

		strlcpy(pbuf, dsi_id, buf_size);
		pbuf += dsi_id_len;
		buf_size -= dsi_id_len;

		strlcpy(pbuf, panel_node, buf_size);
	}
	return ret;
}


static void init_platform_data()
{
	memcpy(dsi_video_mode_phy_db.regulator, panel_regulator_settings,
							REGULATOR_SIZE);
	memcpy(dsi_video_mode_phy_db.ctrl, panel_physical_ctrl,
							PHYSICAL_SIZE);
	memcpy(dsi_video_mode_phy_db.strength, panel_strength_ctrl,
							STRENGTH_SIZE);
	memcpy(dsi_video_mode_phy_db.bistCtrl, panel_bist_ctrl, BIST_SIZE);
	memcpy(dsi_video_mode_phy_db.laneCfg, panel_lane_config, LANE_SIZE);
}

int gcdb_display_init(uint32_t rev, void *base)
{
	int ret = NO_ERROR;

	if (!oem_panel_select(&panelstruct, &(panel.panel_info),
				 &dsi_video_mode_phy_db)) {
		dprintf(CRITICAL, "Target panel init not found!\n");
		ret = ERR_NOT_SUPPORTED;
		goto error_gcdb_display_init;
	}
	init_platform_data();

	if (dsi_panel_init(&(panel.panel_info), &panelstruct)) {
		dprintf(CRITICAL, "DSI panel init failed!\n");
		ret = ERROR;
		goto error_gcdb_display_init;
	}

	panel.panel_info.mipi.mdss_dsi_phy_db = &dsi_video_mode_phy_db;

	panel.pll_clk_func = mdss_dsi_panel_clock;
	panel.power_func = mdss_dsi_panel_power;
	panel.pre_init_func = mdss_dsi_panel_pre_init;
	panel.bl_func = mdss_dsi_bl_enable;
	panel.fb.base = base;
	panel.fb.width =  panel.panel_info.xres;
	panel.fb.height =  panel.panel_info.yres;
	panel.fb.stride =  panel.panel_info.xres;
	panel.fb.bpp =  panel.panel_info.bpp;
	panel.fb.format = panel.panel_info.mipi.dst_format;
	panel.mdp_rev = rev;

	ret = msm_display_init(&panel);

error_gcdb_display_init:
	display_enable = ret ? 0 : 1;
	return ret;
}

void gcdb_display_shutdown(void)
{
	if (display_enable)
		msm_display_off();
}
