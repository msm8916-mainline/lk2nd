/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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
#include <clock.h>
#include <msm_panel.h>
#include <string.h>
#include <stdlib.h>
#include <board.h>
#include <mdp5.h>
#include <qtimer.h>
#include <platform/gpio.h>
#include <mipi_dsi.h>
#include <partition_parser.h>

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
	uint32_t ret = NO_ERROR;
	if (panelstruct.backlightinfo)
		ret = target_backlight_ctrl(panelstruct.backlightinfo, enable);
	return ret;
}

static uint32_t mdss_dsi_panel_reset(uint8_t enable)
{
	uint32_t ret = NO_ERROR;
	if (panelstruct.panelresetseq)
		ret = target_panel_reset(enable, panelstruct.panelresetseq,
							&panel.panel_info);
	return ret;
}

static uint32_t mdss_dsi_panel_clock(uint8_t enable,
				struct msm_panel_info *pinfo)
{
	uint32_t ret = NO_ERROR;

	ret = calculate_clock_config(pinfo);
	if (ret)
		dprintf(CRITICAL, "Clock calculation failed\n");
	else
		ret = target_panel_clock(enable, pinfo);

	return ret;
}

static int mdss_dsi_panel_power(uint8_t enable,
				struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;

	if (enable) {
		ret = target_ldo_ctrl(enable, pinfo);
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

		ret = target_ldo_ctrl(enable, pinfo);
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

static int mdss_dsi_dfps_get_pll_codes_cal(struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	uint32_t fps_bak;
	uint32_t i;

	fps_bak = pinfo->mipi.frame_rate;

	for (i = 0; i < pinfo->dfps.panel_dfps.frame_rate_cnt; i++) {
		int err;
		pinfo->mipi.frame_rate = pinfo->dfps.panel_dfps.frame_rate[i];

		err = mdss_dsi_panel_clock(1, pinfo);
		if (!err) {
			pinfo->dfps.codes_dfps[i].is_valid = 1;
			pinfo->dfps.codes_dfps[i].frame_rate =
				pinfo->mipi.frame_rate;
			pinfo->dfps.codes_dfps[i].frame_rate =
				pinfo->mipi.frame_rate;
			pinfo->dfps.codes_dfps[i].clk_rate =
				pinfo->mipi.dsi_pll_config->vco_clock;
			pinfo->dfps.codes_dfps[i].pll_codes =
				pinfo->mipi.pll_codes;

			mdss_dsi_panel_clock(0, pinfo);
		} else {
			ret = err;
			pinfo->dfps.codes_dfps[i].is_valid = 0;
			dprintf(CRITICAL, "frame_rate=%d failed!\n",
				pinfo->mipi.frame_rate);
		}
	}

	pinfo->mipi.frame_rate = fps_bak;

	return ret;
}

static int mdss_dsi_dfps_get_stored_pll_codes(struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	int index;
	unsigned long long ptn;
	uint32_t blocksize;
	struct dfps_info *dfps;

	index = partition_get_index("splash");
	if (index == INVALID_PTN) {
		dprintf(INFO, "%s: splash partition table not found\n", __func__);
		ret = NO_ERROR;
		goto splash_err;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: splash Partition invalid offset\n");
		ret = ERROR;
		goto splash_err;
	}

	mmc_set_lun(partition_get_lun(index));

	blocksize = mmc_get_device_blocksize();
	if (blocksize == 0) {
		dprintf(CRITICAL, "ERROR:splash Partition invalid blocksize\n");
		ret = ERROR;
		goto splash_err;
	}

	dfps = (struct dfps_info *)memalign(CACHE_LINE, ROUNDUP(PAGE_SIZE,
			CACHE_LINE));
	if (!dfps) {
		dprintf(CRITICAL, "ERROR:splash Partition invalid memory\n");
		ret = ERROR;
		goto splash_err;
	}

	if (mmc_read(ptn, (uint32_t *) dfps, blocksize)) {
		dprintf(CRITICAL, "mmc read splash failure%d\n", PAGE_SIZE);
		ret = ERROR;
		free(dfps);
		goto splash_err;
	}

	dprintf(SPEW, "enable=%d cnt=%d\n", dfps->panel_dfps.enabled,
		dfps->panel_dfps.frame_rate_cnt);

	if (!dfps->panel_dfps.enabled || dfps->panel_dfps.frame_rate_cnt >
		DFPS_MAX_FRAME_RATE) {
		ret = ERROR;
		free(dfps);
		goto splash_err;
	}

	pinfo->dfps = *dfps;
	free(dfps);

splash_err:
	return ret;
}

static int mdss_dsi_dfps_store_pll_codes(struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	int index;
	unsigned long long ptn;

	index = partition_get_index("splash");
	if (index == INVALID_PTN) {
		dprintf(INFO, "%s: splash partition table not found\n", __func__);
		ret = NO_ERROR;
		goto store_err;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: splash Partition invalid offset\n");
		ret = ERROR;
		goto store_err;
	}

	mmc_set_lun(partition_get_lun(index));

	ret = mmc_write(ptn, sizeof(uint32_t), &pinfo->dfps);
	if (ret)
		dprintf(CRITICAL, "mmc write failed!\n");

store_err:
	return ret;
}

static int mdss_dsi_mipi_dfps_config(struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;

	if (!pinfo)
		return ERROR;

	if (!pinfo->dfps.panel_dfps.enabled)
		goto dfps_done;

	if (!mdss_dsi_dfps_get_stored_pll_codes(pinfo)) {
		dprintf(SPEW, "Found stored PLL codes!\n");
		goto dfps_cal_done;
	}

	ret = mdss_dsi_dfps_get_pll_codes_cal(pinfo);
	if (ret) {
		dprintf(CRITICAL, "Cannot cal pll codes!\n");
		goto dfps_done;
	} else {
		dprintf(SPEW, "Calibrate all pll codes!\n");
	}

	ret = mdss_dsi_dfps_store_pll_codes(pinfo);
	if (ret)
		dprintf(CRITICAL, "Cannot store pll codes!\n");

dfps_cal_done:
	if (pinfo->dfps.dfps_fb_base)
		memcpy(pinfo->dfps.dfps_fb_base, &pinfo->dfps,
			sizeof(struct dfps_info));
dfps_done:
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

static bool mdss_dsi_set_panel_node(char *panel_name, char **dsi_id,
		char **panel_node, char **slave_panel_node, int *panel_mode)
{
	if (!strcmp(panel_name, SIM_VIDEO_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_VIDEO_PANEL_NODE;
		*panel_mode = 0;
	} else if (!strcmp(panel_name, SIM_DUALDSI_VIDEO_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_DUALDSI_VIDEO_PANEL_NODE;
		*slave_panel_node = SIM_DUALDSI_VIDEO_SLAVE_PANEL_NODE;
		*panel_mode = 1;
	} else if (!strcmp(panel_name, SIM_CMD_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_CMD_PANEL_NODE;
		*panel_mode = 0;
	} else if (!strcmp(panel_name, SIM_DUALDSI_CMD_PANEL)) {
		*dsi_id = SIM_DSI_ID;
		*panel_node = SIM_DUALDSI_CMD_PANEL_NODE;
		*slave_panel_node = SIM_DUALDSI_CMD_SLAVE_PANEL_NODE;
		*panel_mode = 1;
	} else {
		return false;
	}
	return true;
}

bool gcdb_display_cmdline_arg(char *panel_name, char *pbuf, uint16_t buf_size)
{
	char *dsi_id = NULL;
	char *panel_node = NULL;
	char *slave_panel_node = NULL;
	uint16_t dsi_id_len = 0, panel_node_len = 0, slave_panel_node_len = 0;
	uint32_t arg_size = 0;
	bool ret = true;
	bool rc;
	char *default_str;
	int panel_mode = SPLIT_DISPLAY_FLAG | DUAL_PIPE_FLAG | DST_SPLIT_FLAG;
	int prefix_string_len = strlen(DISPLAY_CMDLINE_PREFIX);
	char *sctl_string;

	panel_name += strspn(panel_name, " ");

	rc = mdss_dsi_set_panel_node(panel_name, &dsi_id, &panel_node,
			&slave_panel_node, &panel_mode);
	if (!rc) {
		if (panelstruct.paneldata && target_cont_splash_screen()) {
			dsi_id = panelstruct.paneldata->panel_controller;
			panel_node = panelstruct.paneldata->panel_node_id;
			panel_mode =
				panelstruct.paneldata->panel_operating_mode &
								panel_mode;
			slave_panel_node =
				panelstruct.paneldata->slave_panel_node_id;
		} else {
			if (target_is_edp())
				default_str = "0:edp:";
			else
				default_str = "0:dsi:0:";

			arg_size = prefix_string_len + strlen(default_str);
			if (buf_size < arg_size) {
				dprintf(CRITICAL, "display command line buffer is small\n");
				return false;
			}

			strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
			pbuf += prefix_string_len;
			buf_size -= prefix_string_len;

			strlcpy(pbuf, default_str, buf_size);
			return true;
		}
	}

	if (dsi_id == NULL || panel_node == NULL) {
		dprintf(CRITICAL, "panel node or dsi ctrl not present\n");
		return false;
	}

	if (((panel_mode & SPLIT_DISPLAY_FLAG) ||
	     (panel_mode & DST_SPLIT_FLAG)) && slave_panel_node == NULL) {
		dprintf(CRITICAL, "slave node not present in dual dsi case\n");
		return false;
	}

	dsi_id_len = strlen(dsi_id);
	panel_node_len = strlen(panel_node);
	if (!slave_panel_node || !strcmp(slave_panel_node, ""))
		slave_panel_node = NO_PANEL_CONFIG;
	slave_panel_node_len = strlen(slave_panel_node);

	arg_size = prefix_string_len + dsi_id_len + panel_node_len +
						LK_OVERRIDE_PANEL_LEN + 1;

	if (!strcmp(panelstruct.paneldata->panel_destination, "DISPLAY_2"))
		sctl_string = DSI_0_STRING;
	else
		sctl_string = DSI_1_STRING;

	arg_size += strlen(sctl_string) + slave_panel_node_len;

	if (buf_size < arg_size) {
		dprintf(CRITICAL, "display command line buffer is small\n");
		ret = false;
	} else {
		strlcpy(pbuf, DISPLAY_CMDLINE_PREFIX, buf_size);
		pbuf += prefix_string_len;
		buf_size -= prefix_string_len;

		strlcpy(pbuf, LK_OVERRIDE_PANEL, buf_size);
		pbuf += LK_OVERRIDE_PANEL_LEN;
		buf_size -= LK_OVERRIDE_PANEL_LEN;

		strlcpy(pbuf, dsi_id, buf_size);
		pbuf += dsi_id_len;
		buf_size -= dsi_id_len;

		strlcpy(pbuf, panel_node, buf_size);

		pbuf += panel_node_len;
		buf_size -= panel_node_len;

		strlcpy(pbuf, sctl_string, buf_size);
		pbuf += strlen(sctl_string);
		buf_size -= strlen(sctl_string);
		strlcpy(pbuf, slave_panel_node, buf_size);
	}
	return ret;
}


static void init_platform_data()
{
	if (dsi_video_mode_phy_db.pll_type == DSI_PLL_TYPE_THULIUM)
		return;

	memcpy(dsi_video_mode_phy_db.regulator, panel_regulator_settings,
							REGULATOR_SIZE);
	memcpy(dsi_video_mode_phy_db.ctrl, panel_physical_ctrl,
							PHYSICAL_SIZE);
	memcpy(dsi_video_mode_phy_db.strength, panel_strength_ctrl,
							STRENGTH_SIZE);
	memcpy(dsi_video_mode_phy_db.bistCtrl, panel_bist_ctrl, BIST_SIZE);
	memcpy(dsi_video_mode_phy_db.laneCfg, panel_lane_config, LANE_SIZE);
}

static void mdss_edp_panel_init(struct msm_panel_info *pinfo)
{
	return target_edp_panel_init(pinfo);
}

static uint32_t mdss_edp_panel_clock(uint8_t enable,
				struct msm_panel_info *pinfo)
{
	return target_edp_panel_clock(enable, pinfo);
}

static uint32_t mdss_edp_panel_enable(void)
{
	return target_edp_panel_enable();
}

static uint32_t mdss_edp_panel_disable(void)
{
	return target_edp_panel_disable();
}

static int mdss_edp_panel_power(uint8_t enable,
				struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;

	if (enable) {
		ret = target_ldo_ctrl(enable, pinfo);
		if (ret) {
			dprintf(CRITICAL, "LDO control enable failed\n");
			return ret;
		}

		ret = mdss_edp_panel_enable();
		if (ret) {
			dprintf(CRITICAL, "%s: panel enable failed\n", __func__);
			return ret;
		}
		dprintf(SPEW, "EDP Panel power on done\n");
	} else {
		/* Disable panel and ldo */
		ret = mdss_edp_panel_disable();
		if (ret) {
			dprintf(CRITICAL, "%s: panel disable failed\n", __func__);
			return ret;
		}

		ret = target_ldo_ctrl(enable, pinfo);
		if (ret) {
			dprintf(CRITICAL, "%s: ldo control disable failed\n", __func__);
			return ret;
		}
		dprintf(SPEW, "EDP Panel power off done\n");
	}

	return ret;
}

static int mdss_edp_bl_enable(uint8_t enable)
{
	int ret = NO_ERROR;

	ret = target_edp_bl_ctrl(enable);
	if (ret)
		dprintf(CRITICAL, "Backlight %s failed\n", enable ? "enable" :
							"disable");
	return ret;
}

static int mdss_dsi2HDMI_config (struct msm_panel_info *pinfo)
{
	return target_display_dsi2hdmi_config(pinfo);
}

int gcdb_display_init(const char *panel_name, uint32_t rev, void *base)
{
	int ret = NO_ERROR;
	int pan_type;

	dsi_video_mode_phy_db.pll_type = DSI_PLL_TYPE_28NM;
	pan_type = oem_panel_select(panel_name, &panelstruct, &(panel.panel_info),
				 &dsi_video_mode_phy_db);

	if (pan_type == PANEL_TYPE_DSI) {
		init_platform_data();
		if (dsi_panel_init(&(panel.panel_info), &panelstruct)) {
			dprintf(CRITICAL, "DSI panel init failed!\n");
			ret = ERROR;
			goto error_gcdb_display_init;
		}

		panel.panel_info.mipi.mdss_dsi_phy_db = &dsi_video_mode_phy_db;
		panel.pll_clk_func = mdss_dsi_panel_clock;
		panel.dfps_func = mdss_dsi_mipi_dfps_config;
		panel.power_func = mdss_dsi_panel_power;
		panel.pre_init_func = mdss_dsi_panel_pre_init;
		panel.bl_func = mdss_dsi_bl_enable;
		panel.dsi2HDMI_config = mdss_dsi2HDMI_config;
		/*
		 * If dfps enabled, reserve fb memory to store pll
		 * codes and pass pll codes values to kernel.
		 */
		if (panel.panel_info.dfps.panel_dfps.enabled) {
			panel.panel_info.dfps.dfps_fb_base = base;
			base += DFPS_PLL_CODES_SIZE;
			dprintf(SPEW, "fb_base=0x%p!\n", base);
		}

		panel.fb.base = base;
		panel.fb.width =  panel.panel_info.xres;
		panel.fb.height =  panel.panel_info.yres;
		panel.fb.stride =  panel.panel_info.xres;
		panel.fb.bpp =  panel.panel_info.bpp;
		panel.fb.format = panel.panel_info.mipi.dst_format;
	} else if (pan_type == PANEL_TYPE_EDP) {
		mdss_edp_panel_init(&(panel.panel_info));
		/* prepare func is set up at edp_panel_init */
                panel.clk_func = mdss_edp_panel_clock;
                panel.power_func = mdss_edp_panel_power;
		panel.bl_func = mdss_edp_bl_enable;
                panel.fb.format = FB_FORMAT_RGB888;
	} else {
		dprintf(CRITICAL, "Target panel init not found!\n");
		ret = ERR_NOT_SUPPORTED;
		goto error_gcdb_display_init;

	}

	panel.fb.base = base;
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
