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
#include <string.h>
#include <err.h>
#include <smem.h>
#include <msm_panel.h>
#include <board.h>
#include <mipi_dsi.h>
#include <qtimer.h>
#include <platform.h>

#include "gcdb_display.h"
#include "include/panel.h"
#include "target/display.h"
#include "panel_display.h"
#include <mipi_dsi.h>
#include <mdp5.h>

/*---------------------------------------------------------------------------*/
/* GCDB Panel Database                                                       */
/*---------------------------------------------------------------------------*/
#include "include/panel_nt35597_wqxga_dualdsi_video.h"
#include "include/panel_nt35597_wqxga_dualdsi_cmd.h"
#include "include/panel_nt35597_wqxga_dsc_video.h"
#include "include/panel_nt35597_wqxga_dsc_cmd.h"
#include "include/panel_sharp_wqxga_dualdsi_video.h"
#include "include/panel_jdi_qhd_dualdsi_video.h"
#include "include/panel_jdi_qhd_dualdsi_cmd.h"
#include "include/panel_r69007_wqxga_cmd.h"
#include "include/panel_jdi_4k_dualdsi_video_nofbc.h"

/*---------------------------------------------------------------------------*/
/* static panel selection variable                                           */
/*---------------------------------------------------------------------------*/
enum {
	SHARP_WQXGA_DUALDSI_VIDEO_PANEL,
	NT35597_WQXGA_DUALDSI_VIDEO_PANEL,
	NT35597_WQXGA_DUALDSI_CMD_PANEL,
	NT35597_WQXGA_DSC_VIDEO_PANEL,
	NT35597_WQXGA_DSC_CMD_PANEL,
	JDI_QHD_DUALDSI_VIDEO_PANEL,
	JDI_QHD_DUALDSI_CMD_PANEL,
	R69007_WQXGA_CMD_PANEL,
	JDI_4K_DUALDSI_VIDEO_NOFBC_PANEL,
	UNKNOWN_PANEL
};

/*
 * The list of panels that are supported on this target.
 * Any panel in this list can be selected using fastboot oem command.
 */
static struct panel_list supp_panels[] = {
	{"sharp_wqxga_dualdsi_video", SHARP_WQXGA_DUALDSI_VIDEO_PANEL},
	{"nt35597_wqxga_dualdsi_video", NT35597_WQXGA_DUALDSI_VIDEO_PANEL},
	{"nt35597_wqxga_dualdsi_cmd", NT35597_WQXGA_DUALDSI_CMD_PANEL},
	{"nt35597_wqxga_dsc_video", NT35597_WQXGA_DSC_VIDEO_PANEL},
	{"nt35597_wqxga_dsc_cmd", NT35597_WQXGA_DSC_CMD_PANEL},
	{"jdi_qhd_dualdsi_video", JDI_QHD_DUALDSI_VIDEO_PANEL},
	{"jdi_qhd_dualdsi_cmd", JDI_QHD_DUALDSI_CMD_PANEL},
	{"r69007_wqxga_cmd", R69007_WQXGA_CMD_PANEL},
	{"jdi_4k_dualdsi_video_nofbc", JDI_4K_DUALDSI_VIDEO_NOFBC_PANEL},
};

static uint32_t panel_id;

int oem_panel_rotation()
{
	return NO_ERROR;
}

int oem_panel_on()
{
	if (panel_id == JDI_QHD_DUALDSI_CMD_PANEL) {
		/* needs extra delay to avoid unexpected artifacts */
		mdelay(JDI_QHD_DUALDSI_CMD_PANEL_ON_DELAY);

	}
	return NO_ERROR;
}

int oem_panel_off()
{
	return NO_ERROR;
}

static bool init_panel_data(struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	int pan_type;
	struct oem_panel_data *oem_data = mdss_dsi_get_oem_data_ptr();

	switch (panel_id) {
	case SHARP_WQXGA_DUALDSI_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &sharp_wqxga_dualdsi_video_panel_data;
		panelstruct->paneldata->panel_operating_mode = 11;
		panelstruct->paneldata->panel_with_enable_gpio = 0;

		panelstruct->panelres     = &sharp_wqxga_dualdsi_video_panel_res;
		panelstruct->color        = &sharp_wqxga_dualdsi_video_color;
		panelstruct->videopanel   = &sharp_wqxga_dualdsi_video_video_panel;
		panelstruct->commandpanel = &sharp_wqxga_dualdsi_video_command_panel;
		panelstruct->state        = &sharp_wqxga_dualdsi_video_state;
		panelstruct->laneconfig   = &sharp_wqxga_dualdsi_video_lane_config;
		panelstruct->paneltiminginfo
			= &sharp_wqxga_dualdsi_video_timing_info;
		panelstruct->panelresetseq
					 = &sharp_wqxga_dualdsi_video_reset_seq;
		panelstruct->backlightinfo = &sharp_wqxga_dualdsi_video_backlight;

		pinfo->labibb = &sharp_wqxga_dualdsi_video_labibb;

		pinfo->mipi.panel_on_cmds
			= sharp_wqxga_dualdsi_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= SHARP_WQXGA_DUALDSI_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= sharp_wqxga_dualdsi_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= SHARP_WQXGA_DUALDSI_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			sharp_wqxga_dualdsi_thulium_video_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		pinfo->dfps.panel_dfps = sharp_wqxga_dualdsi_video_dfps;
		pinfo->mipi.tx_eot_append = true;

		/*
		 * remove DUAL_PIPE_FLAG because on this target for this panel,
		 * it will be added based on one of the selected configurations.
		 */
		panelstruct->paneldata->panel_operating_mode &= ~DUAL_PIPE_FLAG;
		panelstruct->config = &sharp_wqxga_dualdsi_video_config0;
		if (oem_data) {
			switch (oem_data->cfg_num[0]) {
			case -1: /* default */
			case 0:
				panelstruct->config =
					&sharp_wqxga_dualdsi_video_config0;
				break;
			case 1:
				panelstruct->config =
					&sharp_wqxga_dualdsi_video_config1;
				break;
			default:
				dprintf(CRITICAL, "topology config%d not supported. fallback to default config0\n",
					oem_data->cfg_num[0]);
				panelstruct->config = &sharp_wqxga_dualdsi_video_config0;
			}
		}
		pinfo->lm_split[0] = panelstruct->config->lm_split[0];
		pinfo->lm_split[1] = panelstruct->config->lm_split[1];
		break;
	case NT35597_WQXGA_DUALDSI_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &nt35597_wqxga_dualdsi_video_panel_data;
		panelstruct->panelres     = &nt35597_wqxga_dualdsi_video_panel_res;
		panelstruct->color        = &nt35597_wqxga_dualdsi_video_color;
		panelstruct->videopanel   = &nt35597_wqxga_dualdsi_video_video_panel;
		panelstruct->commandpanel = &nt35597_wqxga_dualdsi_video_command_panel;
		panelstruct->state        = &nt35597_wqxga_dualdsi_video_state;
		panelstruct->laneconfig   = &nt35597_wqxga_dualdsi_video_lane_config;
		panelstruct->paneltiminginfo
			= &nt35597_wqxga_dualdsi_video_timing_info;
		panelstruct->panelresetseq
					 = &nt35597_wqxga_dualdsi_video_reset_seq;
		panelstruct->backlightinfo = &nt35597_wqxga_dualdsi_video_backlight;

		pinfo->labibb = &nt35597_wqxga_dualdsi_video_labibb;

		pinfo->mipi.panel_on_cmds
			= nt35597_wqxga_dualdsi_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= NT35597_WQXGA_DUALDSI_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= nt35597_wqxga_dualdsi_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= NT35597_WQXGA_DUALDSI_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			nt35597_wqxga_dualdsi_thulium_video_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		pinfo->mipi.tx_eot_append = true;

		/*
		 * remove DUAL_PIPE_FLAG because on this target for this panel,
		 * it will be added based on one of the selected configurations.
		 */
		panelstruct->paneldata->panel_operating_mode &= ~DUAL_PIPE_FLAG;
		panelstruct->config = &nt35597_wqxga_dualdsi_video_config0;
		if (oem_data) {
			switch (oem_data->cfg_num[0]) {
			case -1: /* default */
			case 0:
				panelstruct->config =
					&nt35597_wqxga_dualdsi_video_config0;
				break;
			case 1:
				panelstruct->config =
					&nt35597_wqxga_dualdsi_video_config1;
				break;
			default:
				dprintf(CRITICAL, "topology config%d not supported. fallback to default config0\n",
					oem_data->cfg_num[0]);
				panelstruct->config = &nt35597_wqxga_dualdsi_video_config0;
			}
		}
		pinfo->lm_split[0] = panelstruct->config->lm_split[0];
		pinfo->lm_split[1] = panelstruct->config->lm_split[1];
		break;
	case NT35597_WQXGA_DUALDSI_CMD_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &nt35597_wqxga_dualdsi_cmd_panel_data;
		panelstruct->panelres     = &nt35597_wqxga_dualdsi_cmd_panel_res;
		panelstruct->color        = &nt35597_wqxga_dualdsi_cmd_color;
		panelstruct->videopanel   = &nt35597_wqxga_dualdsi_cmd_video_panel;
		panelstruct->commandpanel = &nt35597_wqxga_dualdsi_cmd_command_panel;
		panelstruct->state        = &nt35597_wqxga_dualdsi_cmd_state;
		panelstruct->laneconfig   = &nt35597_wqxga_dualdsi_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &nt35597_wqxga_dualdsi_cmd_timing_info;
		panelstruct->panelresetseq
					 = &nt35597_wqxga_dualdsi_cmd_reset_seq;
		panelstruct->backlightinfo = &nt35597_wqxga_dualdsi_cmd_backlight;

		pinfo->labibb = &nt35597_wqxga_dualdsi_cmd_labibb;

		pinfo->mipi.panel_on_cmds
			= nt35597_wqxga_dualdsi_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= NT35597_WQXGA_DUALDSI_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= nt35597_wqxga_dualdsi_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= NT35597_WQXGA_DUALDSI_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			nt35597_wqxga_dualdsi_thulium_cmd_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		pinfo->mipi.tx_eot_append = true;

		/*
		 * remove DUAL_PIPE_FLAG because on this target for this panel,
		 * it will be added based on one of the selected configurations.
		 */
		panelstruct->paneldata->panel_operating_mode &= ~DUAL_PIPE_FLAG;
		panelstruct->config = &nt35597_wqxga_dualdsi_cmd_config0;
		if (oem_data) {
			switch (oem_data->cfg_num[0]) {
			case -1: /* default */
			case 0:
				panelstruct->config =
					&nt35597_wqxga_dualdsi_cmd_config0;
				break;
			case 1:
				panelstruct->config =
					&nt35597_wqxga_dualdsi_cmd_config1;
				break;
			default:
				dprintf(CRITICAL, "topology config%d not supported. fallback to default config0\n",
					oem_data->cfg_num[0]);
				panelstruct->config = &nt35597_wqxga_dualdsi_cmd_config0;
			}
		}
		pinfo->lm_split[0] = panelstruct->config->lm_split[0];
		pinfo->lm_split[1] = panelstruct->config->lm_split[1];
		break;
	case NT35597_WQXGA_DSC_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &nt35597_wqxga_dsc_video_panel_data;
		panelstruct->panelres     = &nt35597_wqxga_dsc_video_panel_res;
		panelstruct->color        = &nt35597_wqxga_dsc_video_color;
		panelstruct->videopanel   = &nt35597_wqxga_dsc_video_video_panel;
		panelstruct->commandpanel = &nt35597_wqxga_dsc_video_command_panel;
		panelstruct->state        = &nt35597_wqxga_dsc_video_state;
		panelstruct->laneconfig   = &nt35597_wqxga_dsc_video_lane_config;
		panelstruct->paneltiminginfo
			= &nt35597_wqxga_dsc_video_timing_info;
		panelstruct->panelresetseq
					 = &nt35597_wqxga_dsc_video_reset_seq;
		panelstruct->backlightinfo = &nt35597_wqxga_dsc_video_backlight;

		pinfo->labibb = &nt35597_wqxga_dsc_video_labibb;

		pinfo->mipi.panel_on_cmds
			= nt35597_wqxga_dsc_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= NT35597_WQXGA_DSC_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= nt35597_wqxga_dsc_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= NT35597_WQXGA_DSC_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			nt35597_wqxga_dsc_thulium_video_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		pinfo->mipi.tx_eot_append = true;

		/*
		 * remove DUAL_PIPE_FLAG because on this target for this panel,
		 * it will be added based on one of the selected configurations.
		 */
		panelstruct->paneldata->panel_operating_mode &= ~DUAL_PIPE_FLAG;
		panelstruct->config = &nt35597_wqxga_dsc_video_config2;
		if (oem_data) {
			switch (oem_data->cfg_num[0]) {
			case -1: /* default */
			case 0:
				panelstruct->config =
					&nt35597_wqxga_dsc_video_config0;
				break;
			case 1:
				panelstruct->config =
					&nt35597_wqxga_dsc_video_config1;
				break;
			case 2:
				panelstruct->config =
					&nt35597_wqxga_dsc_video_config2;
				break;
			default:
				dprintf(CRITICAL, "topology config%d not supported. fallback to default config2\n",
					oem_data->cfg_num[0]);
				panelstruct->config = &nt35597_wqxga_dsc_video_config2;
			}
		}
		pinfo->lm_split[0] = panelstruct->config->lm_split[0];
		pinfo->lm_split[1] = panelstruct->config->lm_split[1];
		pinfo->num_dsc_enc = panelstruct->config->num_dsc_enc;

		pinfo->compression_mode = COMPRESSION_DSC;
		pinfo->dsc.parameter_calc =  mdss_dsc_parameters_calc;
		pinfo->dsc.dsc2buf = mdss_dsc_to_buf;
		pinfo->dsc.dsi_dsc_config = mdss_dsc_dsi_config;
		pinfo->dsc.mdp_dsc_config = mdss_dsc_mdp_config;
		break;
	case NT35597_WQXGA_DSC_CMD_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &nt35597_wqxga_dsc_cmd_panel_data;
		panelstruct->panelres     = &nt35597_wqxga_dsc_cmd_panel_res;
		panelstruct->color        = &nt35597_wqxga_dsc_cmd_color;
		panelstruct->videopanel   = &nt35597_wqxga_dsc_cmd_video_panel;
		panelstruct->commandpanel = &nt35597_wqxga_dsc_cmd_command_panel;
		panelstruct->state        = &nt35597_wqxga_dsc_cmd_state;
		panelstruct->laneconfig   = &nt35597_wqxga_dsc_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &nt35597_wqxga_dsc_cmd_timing_info;
		panelstruct->panelresetseq
					 = &nt35597_wqxga_dsc_cmd_reset_seq;
		panelstruct->backlightinfo = &nt35597_wqxga_dsc_cmd_backlight;

		pinfo->labibb = &nt35597_wqxga_dsc_cmd_labibb;

		pinfo->mipi.panel_on_cmds
			= nt35597_wqxga_dsc_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= NT35597_WQXGA_DSC_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= nt35597_wqxga_dsc_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= NT35597_WQXGA_DSC_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			nt35597_wqxga_dsc_thulium_cmd_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		pinfo->mipi.tx_eot_append = true;

		/*
		 * remove DUAL_PIPE_FLAG because on this target for this panel,
		 * it will be added based on one of the selected configurations.
		 */
		panelstruct->paneldata->panel_operating_mode &= ~DUAL_PIPE_FLAG;
		panelstruct->config = &nt35597_wqxga_dsc_cmd_config2;
		if (oem_data) {
			switch (oem_data->cfg_num[0]) {
			case 0:
				panelstruct->config =
					&nt35597_wqxga_dsc_cmd_config0;
				break;
			case 1:
				panelstruct->config =
					&nt35597_wqxga_dsc_cmd_config1;
				break;
			case -1: /* default */
			case 2:
				panelstruct->config =
					&nt35597_wqxga_dsc_cmd_config2;
				break;
			default:
				dprintf(CRITICAL, "topology config%d not supported. fallback to default config2\n",
					oem_data->cfg_num[0]);
				panelstruct->config = &nt35597_wqxga_dsc_cmd_config2;
			}
		}

		pinfo->lm_split[0] = panelstruct->config->lm_split[0];
		pinfo->lm_split[1] = panelstruct->config->lm_split[1];
		pinfo->num_dsc_enc = panelstruct->config->num_dsc_enc;

		pinfo->compression_mode = COMPRESSION_DSC;
		pinfo->dsc.parameter_calc =  mdss_dsc_parameters_calc;
		pinfo->dsc.dsc2buf = mdss_dsc_to_buf;
		pinfo->dsc.dsi_dsc_config = mdss_dsc_dsi_config;
		pinfo->dsc.mdp_dsc_config = mdss_dsc_mdp_config;
		break;
	case JDI_QHD_DUALDSI_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		panelstruct->paneldata    = &jdi_qhd_dualdsi_video_panel_data;

		panelstruct->panelres     = &jdi_qhd_dualdsi_video_panel_res;
		panelstruct->color        = &jdi_qhd_dualdsi_video_color;
		panelstruct->videopanel   = &jdi_qhd_dualdsi_video_video_panel;
		panelstruct->commandpanel = &jdi_qhd_dualdsi_video_command_panel;
		panelstruct->state        = &jdi_qhd_dualdsi_video_state;
		panelstruct->laneconfig   = &jdi_qhd_dualdsi_video_lane_config;
		panelstruct->paneltiminginfo
			= &jdi_qhd_dualdsi_video_timing_info;
		panelstruct->panelresetseq
					 = &jdi_qhd_dualdsi_video_reset_seq;
		panelstruct->backlightinfo = &jdi_qhd_dualdsi_video_backlight;
		pinfo->mipi.panel_on_cmds
			= jdi_qhd_dualdsi_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= JDI_QHD_DUALDSI_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= jdi_qhd_dualdsi_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= JDI_QHD_DUALDSI_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			jdi_qhd_dualdsi_thulium_cmd_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		break;
	case JDI_QHD_DUALDSI_CMD_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		panelstruct->paneldata    = &jdi_qhd_dualdsi_cmd_panel_data;

		panelstruct->panelres     = &jdi_qhd_dualdsi_cmd_panel_res;
		panelstruct->color        = &jdi_qhd_dualdsi_cmd_color;
		panelstruct->videopanel   = &jdi_qhd_dualdsi_cmd_video_panel;
		panelstruct->commandpanel = &jdi_qhd_dualdsi_cmd_command_panel;
		panelstruct->state        = &jdi_qhd_dualdsi_cmd_state;
		panelstruct->laneconfig   = &jdi_qhd_dualdsi_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &jdi_qhd_dualdsi_cmd_timing_info;
		panelstruct->panelresetseq
					 = &jdi_qhd_dualdsi_cmd_reset_seq;
		panelstruct->backlightinfo = &jdi_qhd_dualdsi_cmd_backlight;
		pinfo->mipi.panel_on_cmds
			= jdi_qhd_dualdsi_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= JDI_QHD_DUALDSI_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= jdi_qhd_dualdsi_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= JDI_QHD_DUALDSI_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			jdi_qhd_dualdsi_thulium_video_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		break;
	case R69007_WQXGA_CMD_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &r69007_wqxga_cmd_panel_data;
		panelstruct->panelres     = &r69007_wqxga_cmd_panel_res;
		panelstruct->color        = &r69007_wqxga_cmd_color;
		panelstruct->videopanel   = &r69007_wqxga_cmd_video_panel;
		panelstruct->commandpanel = &r69007_wqxga_cmd_command_panel;
		panelstruct->state        = &r69007_wqxga_cmd_state;
		panelstruct->laneconfig   = &r69007_wqxga_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &r69007_wqxga_cmd_timing_info;
		panelstruct->panelresetseq
					 = &r69007_wqxga_cmd_reset_seq;
		panelstruct->backlightinfo = &r69007_wqxga_cmd_backlight;

		pinfo->labibb = &r69007_wqxga_cmd_labibb;

		pinfo->mipi.panel_on_cmds
			= r69007_wqxga_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= R69007_WQXGA_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= r69007_wqxga_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= R69007_WQXGA_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			r69007_wqxga_thulium_cmd_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		break;
	case JDI_4K_DUALDSI_VIDEO_NOFBC_PANEL:
		pan_type = PANEL_TYPE_DSI;
		/*
		 * send on_cmds at HS mode by setting cmds_post_tg so that
		 * on_cmds are sent after timing generator on
		 */
		pinfo->mipi.cmds_post_tg = 1;
		panelstruct->paneldata    = &jdi_4k_dualdsi_video_nofbc_panel_data;

		panelstruct->panelres     = &jdi_4k_dualdsi_video_nofbc_panel_res;
		panelstruct->color        = &jdi_4k_dualdsi_video_nofbc_color;
		panelstruct->videopanel   = &jdi_4k_dualdsi_video_nofbc_video_panel;
		panelstruct->commandpanel = &jdi_4k_dualdsi_video_nofbc_command_panel;
		panelstruct->state        = &jdi_4k_dualdsi_video_nofbc_state;
		panelstruct->laneconfig   = &jdi_4k_dualdsi_video_nofbc_lane_config;
		panelstruct->paneltiminginfo
			= &jdi_4k_dualdsi_video_nofbc_timing_info;
		panelstruct->panelresetseq
				 = &jdi_4k_dualdsi_video_nofbc_reset_seq;
		panelstruct->backlightinfo = &jdi_4k_dualdsi_video_nofbc_backlight;
		pinfo->labibb = &jdi_4k_dualdsi_video_nofbc_labibb;

		pinfo->mipi.panel_on_cmds
			= jdi_4k_dualdsi_video_nofbc_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= JDI_4K_DUALDSI_VIDEO_NOFBC_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= jdi_4k_dualdsi_video_nofbc_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= JDI_4K_DUALDSI_VIDEO_NOFBC_OFF_COMMAND;
		memcpy(phy_db->timing,
			jdi_4k_dualdsi_thulium_video_nofbc_timings,
			MAX_TIMING_CONFIG * sizeof(uint32_t));
		break;
	default:
	case UNKNOWN_PANEL:
		pan_type = PANEL_TYPE_UNKNOWN;
		break;
	}

	dprintf(SPEW, "lm_split[0]=%d lm_split[1]=%d mode=0x%x\n",
		pinfo->lm_split[0], pinfo->lm_split[1],
		panelstruct->paneldata->panel_operating_mode);

	return pan_type;
}

int oem_panel_select(const char *panel_name, struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	uint32_t hw_id = board_hardware_id();
	int32_t panel_override_id;

	phy_db->pll_type = DSI_PLL_TYPE_THULIUM;

	if (panel_name) {
		panel_override_id = panel_name_to_id(supp_panels,
				ARRAY_SIZE(supp_panels), panel_name);

		if (panel_override_id < 0) {
			dprintf(CRITICAL, "Not able to search the panel:%s\n",
					 panel_name);
		} else if (panel_override_id < UNKNOWN_PANEL) {
			/* panel override using fastboot oem command */
			panel_id = panel_override_id;

			dprintf(INFO, "OEM panel override:%s\n",
					panel_name);
			goto panel_init;
		}
	}

	switch (hw_id) {
	case HW_PLATFORM_MTP:
	case HW_PLATFORM_FLUID:
	case HW_PLATFORM_SURF:
		panel_id = SHARP_WQXGA_DUALDSI_VIDEO_PANEL;
		break;
	case HW_PLATFORM_QRD:
		panel_id = R69007_WQXGA_CMD_PANEL;
		break;
	case HW_PLATFORM_LIQUID:
		panel_id = JDI_4K_DUALDSI_VIDEO_NOFBC_PANEL;
		break;
	default:
		dprintf(CRITICAL, "Display not enabled for %d HW type\n"
					, hw_id);
		return PANEL_TYPE_UNKNOWN;
	}

panel_init:
	return init_panel_data(panelstruct, pinfo, phy_db);
}
