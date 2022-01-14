/*
 * SPDX-License-Identifier: BSD-3-Clause
 * Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
 */

#ifndef _PANEL_ADV7533_720P60_VIDEO_H_
#define _PANEL_ADV7533_720P60_VIDEO_H_

#include <mipi_dsi.h>
#include <panel_adv7533_720p60.h>
#include <panel_display.h>
#include <string.h>

static inline void panel_adv7533_720p_video_select(struct panel_struct *panel,
						   struct msm_panel_info *pinfo,
						   struct mdss_dsi_phy_ctrl *phy_db)
{
	panel->paneldata = &adv7533_720p_video_panel_data;
	panel->panelres = &adv7533_720p_video_panel_res;
	panel->color = &adv7533_720p_video_color;
	panel->videopanel = &adv7533_720p_video_video_panel;
	panel->commandpanel = &adv7533_720p_video_command_panel;
	panel->state = &adv7533_720p_video_state;
	panel->laneconfig = &adv7533_720p_video_lane_config;
	panel->paneltiminginfo = &adv7533_720p_video_timing_info;
	panel->panelresetseq = NULL;
	panel->backlightinfo = NULL;
	pinfo->adv7533.dsi_tg_i2c_cmd = adv7533_720p_tg_i2c_command;
	pinfo->adv7533.num_of_tg_i2c_cmds = ADV7533_720P_TG_COMMANDS;
	pinfo->adv7533.dsi_setup_cfg_i2c_cmd = adv7533_720p_common_cfg;
	pinfo->adv7533.num_of_cfg_i2c_cmds = ADV7533_720P_CONFIG_COMMANDS;
	memcpy(phy_db->timing, adv7533_720p_video_timings, TIMING_SIZE);
	phy_db->regulator_mode = DSI_PHY_REGULATOR_DCDC_MODE;
}

#endif /* _PANEL_ADV7533_720P60_VIDEO_H_ */
