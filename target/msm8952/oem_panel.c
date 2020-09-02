/* Copyright (c) 2015-2016, 2018-2020 The Linux Foundation. All rights reserved.
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
#include <qtimer.h>
#include <mipi_dsi.h>
#include <mdp5.h>
#include <target/display.h>

#include "gcdb_display.h"
#include "include/panel.h"
#include "panel_display.h"

/*---------------------------------------------------------------------------*/
/* GCDB Panel Database                                                       */
/*---------------------------------------------------------------------------*/
#include "include/panel_truly_1080p_video.h"
#include "include/panel_truly_1080p_cmd.h"
#include "include/panel_otm1906c_1080p_cmd.h"
#include "include/panel_sharp_1080p_cmd.h"
#include "include/panel_nt35597_wqxga_dualdsi_video.h"
#include "include/panel_nt35597_wqxga_dualdsi_cmd.h"
#include "include/panel_hx8399a_1080p_video.h"
#include "include/panel_nt35597_wqxga_dsc_video.h"
#include "include/panel_nt35597_wqxga_dsc_cmd.h"
#include "include/panel_hx8394d_720p_video.h"
#include "include/panel_byd_1200p_video.h"
#include "include/panel_r69006_1080p_cmd.h"
#include "include/panel_r69006_1080p_video.h"
#include "include/panel_hx8394f_720p_video.h"
#include "include/panel_truly_720p_video.h"
#include "include/panel_truly_wuxga_video.h"
#include "include/panel_truly_720p_cmd.h"
#include "include/panel_lead_fl10802_fwvga_video.h"
#include "include/panel_hx8399c_fhd_pluse_video.h"
#include "include/panel_hx8399c_hd_plus_video.h"
#include "include/panel_edo_rm67162_qvga_cmd.h"
#include "include/panel_truly_rm69090_qvga_cmd.h"
#include "include/panel_nt35695b_truly_fhd_video.h"
#include "include/panel_nt35695b_truly_fhd_cmd.h"
#include "include/panel_st7789v2_320p_spi_cmd.h"

/*---------------------------------------------------------------------------*/
/* static panel selection variable                                           */
/*---------------------------------------------------------------------------*/
enum {
	TRULY_1080P_VIDEO_PANEL,
	TRULY_1080P_CMD_PANEL,
	OTM1906C_1080P_CMD_PANEL,
	SHARP_1080P_CMD_PANEL,
	NT35597_WQXGA_DUALDSI_VIDEO_PANEL,
	NT35597_WQXGA_DUALDSI_CMD_PANEL,
	HX8399A_1080P_VIDEO_PANEL,
	NT35597_WQXGA_DSC_VIDEO_PANEL,
	NT35597_WQXGA_DSC_CMD_PANEL,
	HX8394D_720P_VIDEO_PANEL,
	BYD_1200P_VIDEO_PANEL,
	R69006_1080P_CMD_PANEL,
	R69006_1080P_VIDEO_PANEL,
	HX8394F_720P_VIDEO_PANEL,
	TRULY_720P_VIDEO_PANEL,
	TRULY_WUXGA_VIDEO_PANEL,
	TRULY_720P_CMD_PANEL,
	LEAD_FL10802_FWVGA_VIDEO_PANEL,
	HX8399C_FHD_PLUSE_VIDEO_PANEL,
	HX8399C_HD_PLUS_VIDEO_PANEL,
	NT35695B_TRULY_FHD_VIDEO_PANEL,
	NT35695B_TRULY_FHD_CMD_PANEL,
	RM67162_QVGA_CMD_PANEL,
	RM69090_QVGA_CMD_PANEL,
	ST7789v2_320P_SPI_CMD_PANEL,
	UNKNOWN_PANEL
};

uint32_t panel_regulator_settings[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/*
 * The list of panels that are supported on this target.
 * Any panel in this list can be selected using fastboot oem command.
 */
static struct panel_list supp_panels[] = {
	{"truly_1080p_video", TRULY_1080P_VIDEO_PANEL},
	{"truly_1080p_cmd", TRULY_1080P_CMD_PANEL},
	{"sharp_1080p_cmd", SHARP_1080P_CMD_PANEL},
	{"nt35597_wqxga_dualdsi_video", NT35597_WQXGA_DUALDSI_VIDEO_PANEL},
	{"nt35597_wqxga_dualdsi_cmd", NT35597_WQXGA_DUALDSI_CMD_PANEL},
	{"otm1906c_1080p_cmd", OTM1906C_1080P_CMD_PANEL},
	{"hx8399a_1080p_video", HX8399A_1080P_VIDEO_PANEL},
	{"nt35597_wqxga_dsc_video", NT35597_WQXGA_DSC_VIDEO_PANEL},
	{"nt35597_wqxga_dsc_cmd", NT35597_WQXGA_DSC_CMD_PANEL},
	{"hx8394d_720p_video", HX8394D_720P_VIDEO_PANEL},
	{"byd_1200p_video", BYD_1200P_VIDEO_PANEL},
	{"r69006_1080p_cmd",R69006_1080P_CMD_PANEL},
	{"r69006_1080p_video",R69006_1080P_VIDEO_PANEL},
	{"hx8394f_720p_video", HX8394F_720P_VIDEO_PANEL},
	{"truly_720p_video", TRULY_720P_VIDEO_PANEL},
	{"truly_wuxga_video", TRULY_WUXGA_VIDEO_PANEL},
	{"truly_720p_cmd", TRULY_720P_CMD_PANEL},
	{"lead_fl10802_fwvga_video", LEAD_FL10802_FWVGA_VIDEO_PANEL},
	{"hx8399c_fhd_plus_video", HX8399C_FHD_PLUSE_VIDEO_PANEL},
	{"hx8399c_hd_plus_video", HX8399C_HD_PLUS_VIDEO_PANEL},
	{"nt35695b_truly_fhd_video", NT35695B_TRULY_FHD_VIDEO_PANEL},
	{"nt35695b_truly_fhd_cmd", NT35695B_TRULY_FHD_CMD_PANEL},
	{"rm67162_qvga_cmd", RM67162_QVGA_CMD_PANEL},
	{"rm69090_qvga_cmd", RM69090_QVGA_CMD_PANEL},
	{"st7789v2_320p_cmd", ST7789v2_320P_SPI_CMD_PANEL},
};

static uint32_t panel_id;

#define TRULY_1080P_PANEL_ON_DELAY 40

int oem_panel_rotation()
{
	return NO_ERROR;
}

int oem_panel_on()
{
	/*
	 *OEM can keep their panel specific on instructions in this
	 *function
	*/
	if (panel_id == OTM1906C_1080P_CMD_PANEL) {
		/* needs extra delay to avoid unexpected artifacts */
		mdelay(OTM1906C_1080P_CMD_PANEL_ON_DELAY);
	} else if (panel_id == TRULY_1080P_CMD_PANEL ||
			panel_id == TRULY_1080P_VIDEO_PANEL) {
		mdelay(TRULY_1080P_PANEL_ON_DELAY);
	}else if (panel_id == R69006_1080P_CMD_PANEL) {
		mdelay(R69006_1080P_CMD_PANEL_ON_DELAY);
	}

	return NO_ERROR;
}

int oem_panel_off()
{
	/* OEM can keep their panel specific off instructions
	 * in this function
	 */
	return NO_ERROR;
}

static int init_panel_data(struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	int pan_type = PANEL_TYPE_DSI;
	struct oem_panel_data *oem_data = mdss_dsi_get_oem_data_ptr();

	switch (panel_id) {
	case TRULY_1080P_VIDEO_PANEL:
		panelstruct->paneldata    = &truly_1080p_video_panel_data;
		panelstruct->backlightinfo = &truly_1080p_video_backlight;
		if (platform_is_sdm439() || platform_is_sdm429()) {
			panelstruct->paneldata->panel_with_enable_gpio = 0;
			panelstruct->backlightinfo->bl_interface_type = 0;
		} else {
			panelstruct->paneldata->panel_with_enable_gpio = 1;
		}
		panelstruct->panelres     = &truly_1080p_video_panel_res;
		panelstruct->color        = &truly_1080p_video_color;
		panelstruct->videopanel   = &truly_1080p_video_video_panel;
		panelstruct->commandpanel = &truly_1080p_video_command_panel;
		panelstruct->state        = &truly_1080p_video_state;
		panelstruct->laneconfig   = &truly_1080p_video_lane_config;
		panelstruct->paneltiminginfo
			= &truly_1080p_video_timing_info;
		panelstruct->panelresetseq
					 = &truly_1080p_video_panel_reset_seq;
		pinfo->labibb = &truly_1080p_video_labibb;
		pinfo->mipi.panel_on_cmds
			= truly_1080p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= TRULY_1080P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= truly_1080p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= TRULY_1080P_VIDEO_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				truly_1080p_video_12nm_timings,
				TIMING_SIZE_12NM);
		else
			memcpy(phy_db->timing,
				truly_1080p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= TRULY_1080P_VIDEO_SIGNATURE;
		break;
	case TRULY_1080P_CMD_PANEL:
		panelstruct->paneldata    = &truly_1080p_cmd_panel_data;
		panelstruct->backlightinfo = &truly_1080p_cmd_backlight;
		if (platform_is_sdm439() || platform_is_sdm429()) {
			panelstruct->paneldata->panel_with_enable_gpio = 0;
			panelstruct->backlightinfo->bl_interface_type = 0;
		} else {
			panelstruct->paneldata->panel_with_enable_gpio = 1;
		}
		panelstruct->panelres     = &truly_1080p_cmd_panel_res;
		panelstruct->color        = &truly_1080p_cmd_color;
		panelstruct->videopanel   = &truly_1080p_cmd_video_panel;
		panelstruct->commandpanel = &truly_1080p_cmd_command_panel;
		panelstruct->state        = &truly_1080p_cmd_state;
		panelstruct->laneconfig   = &truly_1080p_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &truly_1080p_cmd_timing_info;
		panelstruct->panelresetseq
					 = &truly_1080p_cmd_panel_reset_seq;
		pinfo->labibb = &truly_1080p_cmd_labibb;
		pinfo->mipi.panel_on_cmds
			= truly_1080p_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= TRULY_1080P_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= truly_1080p_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= TRULY_1080P_CMD_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				truly_1080p_cmd_12nm_timings,
				TIMING_SIZE_12NM);
		else
			memcpy(phy_db->timing,
				truly_1080p_cmd_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= TRULY_1080P_CMD_SIGNATURE;
		break;
	case OTM1906C_1080P_CMD_PANEL:
		panelstruct->paneldata    = &otm1906c_1080p_cmd_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &otm1906c_1080p_cmd_panel_res;
		panelstruct->color        = &otm1906c_1080p_cmd_color;
		panelstruct->videopanel   = &otm1906c_1080p_cmd_video_panel;
		panelstruct->commandpanel = &otm1906c_1080p_cmd_command_panel;
		panelstruct->state        = &otm1906c_1080p_cmd_state;
		panelstruct->laneconfig   = &otm1906c_1080p_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &otm1906c_1080p_cmd_timing_info;
		panelstruct->panelresetseq
					 = &otm1906c_1080p_cmd_panel_reset_seq;
		panelstruct->backlightinfo = &otm1906c_1080p_cmd_backlight;
		pinfo->mipi.panel_on_cmds
			= otm1906c_1080p_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= OTM1906C_1080P_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= otm1906c_1080p_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= OTM1906C_1080P_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			otm1906c_1080p_cmd_timings, TIMING_SIZE);
		pinfo->mipi.signature = OTM1906C_1080P_CMD_SIGNATURE;
		break;
	case HX8399A_1080P_VIDEO_PANEL:
		panelstruct->paneldata    = &hx8399a_1080p_video_panel_data;
		panelstruct->panelres     = &hx8399a_1080p_video_panel_res;
		panelstruct->color        = &hx8399a_1080p_video_color;
		panelstruct->videopanel   = &hx8399a_1080p_video_video_panel;
		panelstruct->commandpanel = &hx8399a_1080p_video_command_panel;
		panelstruct->state        = &hx8399a_1080p_video_state;
		panelstruct->laneconfig   = &hx8399a_1080p_video_lane_config;
		panelstruct->paneltiminginfo
			= &hx8399a_1080p_video_timing_info;
		panelstruct->panelresetseq
					 = &hx8399a_1080p_video_reset_seq;
		panelstruct->backlightinfo = &hx8399a_1080p_video_backlight;
		pinfo->mipi.panel_on_cmds
			= hx8399a_1080p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= HX8399A_1080P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= hx8399a_1080p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= HX8399A_1080P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			hx8399a_1080p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8399A_1080P_VIDEO_SIGNATURE;
		break;
	case SHARP_1080P_CMD_PANEL:
		panelstruct->paneldata    = &sharp_1080p_cmd_panel_data;
		panelstruct->panelres     = &sharp_1080p_cmd_panel_res;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->color        = &sharp_1080p_cmd_color;
		panelstruct->videopanel   = &sharp_1080p_cmd_video_panel;
		panelstruct->commandpanel = &sharp_1080p_cmd_command_panel;
		panelstruct->state        = &sharp_1080p_cmd_state;
		panelstruct->laneconfig   = &sharp_1080p_cmd_lane_config;
		panelstruct->paneltiminginfo
					= &sharp_1080p_cmd_timing_info;
		panelstruct->panelresetseq
					= &sharp_1080p_cmd_panel_reset_seq;
		panelstruct->backlightinfo = &sharp_1080p_cmd_backlight;
		pinfo->mipi.panel_on_cmds
					= sharp_1080p_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
					= SHARP_1080P_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= sharp_1080p_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= SHARP_1080P_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
				sharp_1080p_cmd_timings, TIMING_SIZE);
		break;
	case NT35597_WQXGA_DUALDSI_VIDEO_PANEL:
		panelstruct->paneldata    = &nt35597_wqxga_dualdsi_video_panel_data;
		panelstruct->paneldata->panel_operating_mode = DST_SPLIT_FLAG |
					SPLIT_DISPLAY_FLAG | DUAL_DSI_FLAG;
		panelstruct->paneldata->panel_with_enable_gpio = 0;

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
		memcpy(phy_db->timing, nt35597_wqxga_dualdsi_video_timings,
			TIMING_SIZE);
		pinfo->mipi.tx_eot_append = true;
		break;
	case NT35597_WQXGA_DUALDSI_CMD_PANEL:
		panelstruct->paneldata    = &nt35597_wqxga_dualdsi_cmd_panel_data;
		panelstruct->paneldata->panel_operating_mode = DST_SPLIT_FLAG |
					SPLIT_DISPLAY_FLAG | DUAL_DSI_FLAG;
		panelstruct->paneldata->panel_with_enable_gpio = 0;

		panelstruct->panelres     = &nt35597_wqxga_dualdsi_cmd_panel_res;
		panelstruct->color        = &nt35597_wqxga_dualdsi_cmd_color;
		panelstruct->videopanel   = &nt35597_wqxga_dualdsi_cmd_video_panel;
		panelstruct->commandpanel = &nt35597_wqxga_dualdsi_cmd_command_panel;
		panelstruct->state        = &nt35597_wqxga_dualdsi_cmd_state;
		panelstruct->laneconfig   = &nt35597_wqxga_dualdsi_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &nt35597_wqxga_dualdsi_cmd_timing_info;
		/* Clkout timings are different for this panel on 8956 */
		panelstruct->paneltiminginfo->tclk_post = 0x2b;
		panelstruct->paneltiminginfo->tclk_pre = 0x28;
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
		memcpy(phy_db->timing, nt35597_wqxga_dualdsi_cmd_timings,
			TIMING_SIZE);
		pinfo->mipi.tx_eot_append = true;
		break;
	case NT35597_WQXGA_DSC_VIDEO_PANEL:
		panelstruct->paneldata    = &nt35597_wqxga_dsc_video_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 0;
		panelstruct->paneldata->panel_operating_mode = USE_DSI1_PLL_FLAG;
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
		memcpy(phy_db->timing, nt35597_wqxga_dsc_video_timings,
			TIMING_SIZE);
		/* Clkout timings are different for this panel on 8956 */
		panelstruct->paneltiminginfo->tclk_post = 0x04;
		panelstruct->paneltiminginfo->tclk_pre = 0x20;
		pinfo->mipi.tx_eot_append = true;

		panelstruct->paneldata->panel_operating_mode &= ~DUAL_PIPE_FLAG;
		panelstruct->config = &nt35597_wqxga_dsc_video_config0;
		if (oem_data) {
			switch (oem_data->cfg_num[0]) {
			case -1: /* default */
			case 0:
				panelstruct->config =
					&nt35597_wqxga_dsc_video_config0;
				break;
			default:
				dprintf(CRITICAL, "topology config%d not supported. fallback to default config0\n",
					oem_data->cfg_num[0]);
				panelstruct->config = &nt35597_wqxga_dsc_video_config0;
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
		panelstruct->paneldata    = &nt35597_wqxga_dsc_cmd_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 0;
		panelstruct->paneldata->panel_operating_mode = USE_DSI1_PLL_FLAG;
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
		memcpy(phy_db->timing, nt35597_wqxga_dsc_cmd_timings,
			TIMING_SIZE);
		/* Clkout timings are different for this panel on 8956 */
		panelstruct->paneltiminginfo->tclk_post = 0x04;
		panelstruct->paneltiminginfo->tclk_pre = 0x20;
		pinfo->mipi.tx_eot_append = true;

		panelstruct->paneldata->panel_operating_mode &= ~DUAL_PIPE_FLAG;
		panelstruct->config = &nt35597_wqxga_dsc_cmd_config0;
		if (oem_data) {
			switch (oem_data->cfg_num[0]) {
			case -1: /* default */
			case 0:
				panelstruct->config =
					&nt35597_wqxga_dsc_cmd_config0;
				break;
			default:
				dprintf(CRITICAL, "topology config%d not supported. fallback to default config0\n",
					oem_data->cfg_num[0]);
				panelstruct->config = &nt35597_wqxga_dsc_cmd_config0;
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
	case HX8394D_720P_VIDEO_PANEL:
		panelstruct->paneldata	  = &hx8394d_720p_video_panel_data;
		panelstruct->panelres	  = &hx8394d_720p_video_panel_res;
		panelstruct->color		  = &hx8394d_720p_video_color;
		panelstruct->videopanel   = &hx8394d_720p_video_video_panel;
		panelstruct->commandpanel = &hx8394d_720p_video_command_panel;
		panelstruct->state		  = &hx8394d_720p_video_state;
		panelstruct->laneconfig   = &hx8394d_720p_video_lane_config;
		panelstruct->paneltiminginfo
					 = &hx8394d_720p_video_timing_info;
		panelstruct->panelresetseq
					 = &hx8394d_720p_video_panel_reset_seq;
		panelstruct->backlightinfo = &hx8394d_720p_video_backlight;
		pinfo->mipi.panel_on_cmds
					= hx8394d_720p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
					= HX8394D_720P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
					= hx8394d_720p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
					= HX8394D_720P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
				hx8394d_720p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8394D_720P_VIDEO_SIGNATURE;
		break;
	case R69006_1080P_CMD_PANEL:
		panelstruct->paneldata    = &r69006_1080p_cmd_panel_data;
		panelstruct->panelres     = &r69006_1080p_cmd_panel_res;
		panelstruct->color                = &r69006_1080p_cmd_color;
		panelstruct->videopanel   = &r69006_1080p_cmd_video_panel;
		panelstruct->commandpanel = &r69006_1080p_cmd_command_panel;
		panelstruct->state                = &r69006_1080p_cmd_state;
		panelstruct->laneconfig   = &r69006_1080p_cmd_lane_config;
		panelstruct->paneltiminginfo
		                         = &r69006_1080p_cmd_timing_info;
		panelstruct->panelresetseq
		                         = &r69006_1080p_cmd_reset_seq;
		panelstruct->backlightinfo = &r69006_1080p_cmd_backlight;
		pinfo->labibb = &r69006_1080p_cmd_labibb;
		pinfo->mipi.panel_on_cmds
		                        = r69006_1080p_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
		                        = R69006_1080P_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
		                        = r69006_1080p_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
		                        = R69006_1080P_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
		                r69006_1080p_cmd_timings, TIMING_SIZE);
		pinfo->mipi.signature = R69006_1080P_CMD_SIGNATURE;
		pinfo->mipi.tx_eot_append = true;
		pinfo->mipi.rx_eot_ignore = true;
		break;
	case R69006_1080P_VIDEO_PANEL:
		panelstruct->paneldata	  = &r69006_1080p_video_panel_data;
		panelstruct->panelres	  = &r69006_1080p_video_panel_res;
		panelstruct->color				  = &r69006_1080p_video_color;
		panelstruct->videopanel   = &r69006_1080p_video_video_panel;
		panelstruct->commandpanel = &r69006_1080p_video_command_panel;
		panelstruct->state				  = &r69006_1080p_video_state;
		panelstruct->laneconfig   = &r69006_1080p_video_lane_config;
		panelstruct->paneltiminginfo
								 = &r69006_1080p_video_timing_info;
		panelstruct->panelresetseq
								 = &r69006_1080p_video_reset_seq;
		panelstruct->backlightinfo = &r69006_1080p_video_backlight;
		pinfo->labibb = &r69006_1080p_video_labibb;
		pinfo->mipi.panel_on_cmds
								= r69006_1080p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
								= R69006_1080P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
								= r69006_1080p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
								= R69006_1080P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
						r69006_1080p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = R69006_1080P_VIDEO_SIGNATURE;
		break;
	case HX8394F_720P_VIDEO_PANEL:
		panelstruct->paneldata	  = &hx8394f_720p_video_panel_data;
		panelstruct->panelres	  = &hx8394f_720p_video_panel_res;
		panelstruct->color				  = &hx8394f_720p_video_color;
		panelstruct->videopanel   = &hx8394f_720p_video_video_panel;
		panelstruct->commandpanel = &hx8394f_720p_video_command_panel;
		panelstruct->state				  = &hx8394f_720p_video_state;
		panelstruct->laneconfig   = &hx8394f_720p_video_lane_config;
		panelstruct->paneltiminginfo
								 = &hx8394f_720p_video_timing_info;
		panelstruct->panelresetseq
								 = &hx8394f_720p_video_reset_seq;
		panelstruct->backlightinfo = &hx8394f_720p_video_backlight;
		pinfo->mipi.panel_on_cmds
								= hx8394f_720p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
								= HX8394F_720P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
								= hx8394f_720p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
								= HX8394F_720P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
						hx8394f_720p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8394F_720P_VIDEO_SIGNATURE;
		if ((board_platform_id() == SDM429) || (board_platform_id() == SDM439) ||
				(board_platform_id() == SDA429) || (board_platform_id() == SDA439))
			pinfo->disable_wled_labibb = true;
		break;
	case BYD_1200P_VIDEO_PANEL:
		panelstruct->paneldata    = &byd_1200p_video_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &byd_1200p_video_panel_res;
		panelstruct->color        = &byd_1200p_video_color;
		panelstruct->videopanel   = &byd_1200p_video_video_panel;
		panelstruct->commandpanel = &byd_1200p_video_command_panel;
		panelstruct->state        = &byd_1200p_video_state;
		panelstruct->laneconfig   = &byd_1200p_video_lane_config;
		panelstruct->paneltiminginfo
			= &byd_1200p_video_timing_info;
		panelstruct->panelresetseq
					 = &byd_1200p_video_panel_reset_seq;
		panelstruct->backlightinfo = &byd_1200p_video_backlight;
		pinfo->mipi.panel_on_cmds
			= byd_1200p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= BYD_1200P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= byd_1200p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= BYD_1200P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			byd_1200p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= BYD_1200P_VIDEO_SIGNATURE;
		phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
		break;
	case TRULY_720P_VIDEO_PANEL:
		panelstruct->paneldata    = &truly_720p_video_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &truly_720p_video_panel_res;
		panelstruct->color        = &truly_720p_video_color;
		panelstruct->videopanel   = &truly_720p_video_video_panel;
		panelstruct->commandpanel = &truly_720p_video_command_panel;
		panelstruct->state        = &truly_720p_video_state;
		panelstruct->laneconfig   = &truly_720p_video_lane_config;
		panelstruct->paneltiminginfo
			= &truly_720p_video_timing_info;
		panelstruct->panelresetseq
					 = &truly_720p_video_panel_reset_seq;
		panelstruct->backlightinfo = &truly_720p_video_backlight;
		pinfo->mipi.panel_on_cmds
			= truly_720p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= TRULY_720P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= truly_720p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= TRULY_720P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			truly_720p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= TRULY_720P_VIDEO_SIGNATURE;
		pinfo->mipi.tx_eot_append = true;
		break;
	case TRULY_WUXGA_VIDEO_PANEL:
		panelstruct->paneldata    = &truly_wuxga_video_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &truly_wuxga_video_panel_res;
		panelstruct->color        = &truly_wuxga_video_color;
		panelstruct->videopanel   = &truly_wuxga_video_video_panel;
		panelstruct->commandpanel = &truly_wuxga_video_command_panel;
		panelstruct->state        = &truly_wuxga_video_state;
		panelstruct->laneconfig   = &truly_wuxga_video_lane_config;
		panelstruct->paneltiminginfo
			= &truly_wuxga_video_timing_info;
		panelstruct->panelresetseq
					 = &truly_wuxga_video_panel_reset_seq;
		panelstruct->backlightinfo = &truly_wuxga_video_backlight;
		pinfo->mipi.panel_on_cmds
			= truly_wuxga_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= TRULY_WUXGA_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= truly_wuxga_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= TRULY_WUXGA_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			truly_wuxga_video_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= TRULY_WUXGA_VIDEO_SIGNATURE;
		break;
	case TRULY_720P_CMD_PANEL:
		panelstruct->paneldata    = &truly_720p_cmd_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 1;
		panelstruct->panelres     = &truly_720p_cmd_panel_res;
		panelstruct->color        = &truly_720p_cmd_color;
		panelstruct->videopanel   = &truly_720p_cmd_video_panel;
		panelstruct->commandpanel = &truly_720p_cmd_command_panel;
		panelstruct->state        = &truly_720p_cmd_state;
		panelstruct->laneconfig   = &truly_720p_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &truly_720p_cmd_timing_info;
		panelstruct->panelresetseq
					 = &truly_720p_cmd_panel_reset_seq;
		panelstruct->backlightinfo = &truly_720p_cmd_backlight;
		pinfo->mipi.panel_on_cmds
			= truly_720p_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= TRULY_720P_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= truly_720p_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= TRULY_720P_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			truly_720p_cmd_timings, TIMING_SIZE);
		pinfo->mipi.signature 	= TRULY_720P_CMD_SIGNATURE;
		pinfo->mipi.tx_eot_append = true;
		break;
	case LEAD_FL10802_FWVGA_VIDEO_PANEL:
		panelstruct->paneldata = &lead_fl10802_fwvga_video_panel_data;
		panelstruct->panelres = &lead_fl10802_fwvga_video_panel_res;
		panelstruct->color = &lead_fl10802_fwvga_video_color;
		panelstruct->videopanel = &lead_fl10802_fwvga_video_video_panel;
		panelstruct->commandpanel
				= &lead_fl10802_fwvga_video_command_panel;
		panelstruct->state = &lead_fl10802_fwvga_video_state;
		panelstruct->laneconfig = &lead_fl10802_fwvga_video_lane_config;
		panelstruct->paneltiminginfo
				= &lead_fl10802_fwvga_video_timing_info;
		panelstruct->panelresetseq
				= &lead_fl10802_fwvga_video_reset_seq;
		panelstruct->backlightinfo
				= &lead_fl10802_fwvga_video_backlight;
		pinfo->mipi.panel_on_cmds
				= lead_fl10802_fwvga_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
				= LEAD_FL10802_FWVGA_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
				= lead_fl10802_fwvga_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
				= LEAD_FL10802_FWVGA_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
				lead_fl10802_fwvga_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = LEAD_FL10802_FWVGA_VIDEO_SIGNATURE;
		pinfo->mipi.cmds_post_tg = 1;
		break;
	case HX8399C_FHD_PLUSE_VIDEO_PANEL:
		panelstruct->paneldata    = &hx8399c_fhd_pluse_video_panel_data;
		panelstruct->panelres     = &hx8399c_fhd_pluse_video_panel_res;
		panelstruct->color        = &hx8399c_fhd_pluse_video_color;
		panelstruct->videopanel   =
				&hx8399c_fhd_pluse_video_video_panel;
		panelstruct->commandpanel =
				&hx8399c_fhd_pluse_video_command_panel;
		panelstruct->state        = &hx8399c_fhd_pluse_video_state;
		panelstruct->laneconfig   =
				&hx8399c_fhd_pluse_video_lane_config;
		panelstruct->paneltiminginfo
				= &hx8399c_fhd_pluse_video_timing_info;
		panelstruct->panelresetseq
				= &hx8399c_fhd_pluse_video_panel_reset_seq;
		panelstruct->backlightinfo = &hx8399c_fhd_pluse_video_backlight;
		pinfo->labibb = &hx8399c_fhd_pluse_video_labibb;
		pinfo->mipi.panel_on_cmds
				= hx8399c_fhd_pluse_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
				= HX8399C_FHD_PLUSE_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
				= hx8399c_fhd_pluse_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
				= HX8399C_FHD_PLUSE_VIDEO_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				hx8399c_fhd_pluse_video_12nm_timings,
				TIMING_SIZE_12NM);
		else
			memcpy(phy_db->timing, hx8399c_fhd_pluse_video_timings,
				TIMING_SIZE);
		pinfo->mipi.signature    = HX8399C_FHD_PLUSE_VIDEO_SIGNATURE;
		break;
	case HX8399C_HD_PLUS_VIDEO_PANEL:
		panelstruct->paneldata    = &hx8399c_hd_plus_video_panel_data;
		panelstruct->panelres     = &hx8399c_hd_plus_video_panel_res;
		panelstruct->color        = &hx8399c_hd_plus_video_color;
		panelstruct->videopanel   =
				&hx8399c_hd_plus_video_video_panel;
		panelstruct->commandpanel =
				&hx8399c_hd_plus_video_command_panel;
		panelstruct->state        = &hx8399c_hd_plus_video_state;
		panelstruct->laneconfig   =
				&hx8399c_hd_plus_video_lane_config;
		panelstruct->paneltiminginfo
				= &hx8399c_hd_plus_video_timing_info;
		panelstruct->panelresetseq
				= &hx8399c_hd_plus_video_panel_reset_seq;
		panelstruct->backlightinfo = &hx8399c_hd_plus_video_backlight;
		pinfo->labibb = &hx8399c_hd_plus_video_labibb;
		pinfo->mipi.panel_on_cmds
				= hx8399c_hd_plus_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
				= HX8399C_HD_PLUS_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
				= hx8399c_hd_plus_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
				= HX8399C_HD_PLUS_VIDEO_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				hx8399c_hd_plus_video_12nm_timings,
				TIMING_SIZE_12NM);
		else
			memcpy(phy_db->timing, hx8399c_hd_plus_video_timings,
				TIMING_SIZE);
		pinfo->mipi.signature    = HX8399C_HD_PLUS_VIDEO_SIGNATURE;
		pinfo->mipi.tx_eot_append = true;
		break;
	case RM67162_QVGA_CMD_PANEL:
		panelstruct->paneldata    = &edo_rm67162_qvga_cmd_panel_data;
		panelstruct->panelres     = &edo_rm67162_qvga_cmd_panel_res;
		panelstruct->color        = &edo_rm67162_qvga_cmd_color;
		panelstruct->videopanel   =
				&edo_rm67162_qvga_cmd_video_panel;
		panelstruct->commandpanel =
				&edo_rm67162_qvga_cmd_command_panel;
		panelstruct->state        = &edo_rm67162_qvga_cmd_state;
		panelstruct->laneconfig   =
				&edo_rm67162_qvga_cmd_lane_config;
		panelstruct->paneltiminginfo
				= &edo_rm67162_qvga_cmd_timing_info;
		panelstruct->panelresetseq
				= &edo_rm67162_qvga_cmd_reset_seq;
		panelstruct->backlightinfo = &edo_rm67162_qvga_cmd_backlight;
		pinfo->labibb = NULL;
		pinfo->mipi.panel_on_cmds
				= edo_rm67162_qvga_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
				= EDO_RM67162_QVGA_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
				= edo_rm67162_qvga_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
				= EDO_RM67162_QVGA_CMD_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				edo_rm67162_qvga_cmd_12nm_timings,
				TIMING_SIZE_12NM);
		pinfo->mipi.tx_eot_append = true;
		break;
	case RM69090_QVGA_CMD_PANEL:
		panelstruct->paneldata    = &truly_rm69090_qvga_cmd_panel_data;
		panelstruct->panelres     = &truly_rm69090_qvga_cmd_panel_res;
		panelstruct->color        = &truly_rm69090_qvga_cmd_color;
		panelstruct->videopanel   =
				&truly_rm69090_qvga_cmd_video_panel;
		panelstruct->commandpanel =
				&truly_rm69090_qvga_cmd_command_panel;
		panelstruct->state        = &truly_rm69090_qvga_cmd_state;
		panelstruct->laneconfig   =
				&truly_rm69090_qvga_cmd_lane_config;
		panelstruct->paneltiminginfo
				= &truly_rm69090_qvga_cmd_timing_info;
		panelstruct->panelresetseq
				= &truly_rm69090_qvga_cmd_reset_seq;
		panelstruct->backlightinfo = &truly_rm69090_qvga_cmd_backlight;
		pinfo->labibb = NULL;
		pinfo->mipi.panel_on_cmds
				= truly_rm69090_qvga_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
				= TRULY_RM69090_QVGA_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
				= truly_rm69090_qvga_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
				= TRULY_RM69090_QVGA_CMD_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				truly_rm69090_qvga_cmd_12nm_timings,
				TIMING_SIZE_12NM);
		pinfo->mipi.tx_eot_append = true;
		break;
	case NT35695B_TRULY_FHD_VIDEO_PANEL:
		panelstruct->paneldata    = &nt35695b_truly_fhd_video_panel_data;
		panelstruct->panelres     = &nt35695b_truly_fhd_video_panel_res;
		panelstruct->color        = &nt35695b_truly_fhd_video_color;
		panelstruct->videopanel   =
				&nt35695b_truly_fhd_video_video_panel;
		panelstruct->commandpanel =
				&nt35695b_truly_fhd_video_command_panel;
		panelstruct->state        = &nt35695b_truly_fhd_video_state;
		panelstruct->laneconfig   =
				&nt35695b_truly_fhd_video_lane_config;
		panelstruct->paneltiminginfo
				= &nt35695b_truly_fhd_video_timing_info;
		panelstruct->panelresetseq
				= &nt35695b_truly_fhd_video_panel_reset_seq;
		panelstruct->backlightinfo = &nt35695b_truly_fhd_video_backlight;
		pinfo->labibb = &nt35695b_truly_fhd_video_labibb;
		pinfo->mipi.panel_on_cmds
				= nt35695b_truly_fhd_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
				= NT35695B_TRULY_FHD_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
				= nt35695b_truly_fhd_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
				= NT35695B_TRULY_FHD_VIDEO_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				nt35695b_truly_fhd_video_12nm_timings,
				TIMING_SIZE_12NM);
		else
			memcpy(phy_db->timing, nt35695b_truly_fhd_video_timings,
				TIMING_SIZE);
		pinfo->mipi.signature    = NT35695B_TRULY_FHD_VIDEO_SIGNATURE;
		pinfo->mipi.tx_eot_append = true;
		break;
	case NT35695B_TRULY_FHD_CMD_PANEL:
		panelstruct->paneldata    = &nt35695b_truly_fhd_cmd_panel_data;
		panelstruct->panelres     = &nt35695b_truly_fhd_cmd_panel_res;
		panelstruct->color        = &nt35695b_truly_fhd_cmd_color;
		panelstruct->videopanel   =
				&nt35695b_truly_fhd_cmd_video_panel;
		panelstruct->commandpanel =
				&nt35695b_truly_fhd_cmd_command_panel;
		panelstruct->state        = &nt35695b_truly_fhd_cmd_state;
		panelstruct->laneconfig   =
				&nt35695b_truly_fhd_cmd_lane_config;
		panelstruct->paneltiminginfo
				= &nt35695b_truly_fhd_cmd_timing_info;
		panelstruct->panelresetseq
				= &nt35695b_truly_fhd_cmd_panel_reset_seq;
		panelstruct->backlightinfo = &nt35695b_truly_fhd_cmd_backlight;
		pinfo->labibb = &nt35695b_truly_fhd_cmd_labibb;
		pinfo->mipi.panel_on_cmds
				= nt35695b_truly_fhd_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
				= NT35695B_TRULY_FHD_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
				= nt35695b_truly_fhd_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
				= NT35695B_TRULY_FHD_CMD_OFF_COMMAND;
		if (phy_db->pll_type == DSI_PLL_TYPE_12NM)
			memcpy(phy_db->timing,
				nt35695b_truly_fhd_cmd_12nm_timings,
				TIMING_SIZE_12NM);
		else
			memcpy(phy_db->timing, nt35695b_truly_fhd_cmd_timings,
				TIMING_SIZE);
		pinfo->mipi.signature    = NT35695B_TRULY_FHD_CMD_SIGNATURE;
		pinfo->mipi.tx_eot_append = true;
		break;
	case ST7789v2_320P_SPI_CMD_PANEL:
		panelstruct->paneldata          = &st7789v2_320p_cmd_panel_data;
		panelstruct->panelres           = &st7789v2_320p_cmd_panel_res;
		panelstruct->color              = &st7789v2_320p_cmd_color;
		panelstruct->panelresetseq      = &st7789v2_320p_cmd_reset_seq;
		panelstruct->backlightinfo      = &st7789v2_320p_cmd_backlight;
		pinfo->spi.panel_cmds           = st7789v2_320p_cmd_on_command;
		pinfo->spi.num_of_panel_cmds    = ST7789v2_320p_CMD_ON_COMMAND;
		pinfo->spi.signature_addr       = &st7789v2_signature_addr;
		pinfo->spi.signature            = st7789v2_signature;
		pan_type = PANEL_TYPE_SPI;
		break;
	case UNKNOWN_PANEL:
	default:
		memset(panelstruct, 0, sizeof(struct panel_struct));
		memset(pinfo->mipi.panel_on_cmds, 0,
					sizeof(struct mipi_dsi_cmd));
		pinfo->mipi.num_of_panel_on_cmds = 0;
		memset(pinfo->mipi.panel_off_cmds, 0,
					sizeof(struct mipi_dsi_cmd));
		pinfo->mipi.num_of_panel_off_cmds = 0;
		memset(phy_db->timing, 0, TIMING_SIZE);
		pan_type = PANEL_TYPE_UNKNOWN;
		break;
	}

	dprintf(SPEW, "lm_split[0]=%d lm_split[1]=%d mode=0x%x\n",
		pinfo->lm_split[0], pinfo->lm_split[1],
		panelstruct->paneldata->panel_operating_mode);

	return pan_type;
}

#define DISPLAY_MAX_PANEL_DETECTION 2
static uint32_t auto_pan_loop = 0;

uint32_t oem_panel_max_auto_detect_panels()
{
	return target_panel_auto_detect_enabled() ?
		DISPLAY_MAX_PANEL_DETECTION : 0;
}

int oem_panel_select(const char *panel_name, struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	uint32_t hw_id = board_hardware_id();
	uint32_t hw_subtype = board_hardware_subtype();
	int32_t panel_override_id;
	uint32_t target_id, plat_hw_ver_major;

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
		if (platform_is_msm8956())
			panel_id = NT35597_WQXGA_DUALDSI_VIDEO_PANEL;
		else if (platform_is_msm8917())
			panel_id = TRULY_720P_VIDEO_PANEL;
		else if (platform_is_sdm439())
			panel_id = HX8399C_FHD_PLUSE_VIDEO_PANEL;
		else if (platform_is_sdm429())
			panel_id = HX8399C_HD_PLUS_VIDEO_PANEL;
		else
			panel_id = TRULY_1080P_VIDEO_PANEL;
		break;
	case HW_PLATFORM_SURF:
	case HW_PLATFORM_RCM:
		if (platform_is_msm8956())
			panel_id = NT35597_WQXGA_DUALDSI_VIDEO_PANEL;
		else if (platform_is_msm8917())
			panel_id = TRULY_720P_VIDEO_PANEL;
		else if (platform_is_sdm439())
			panel_id = HX8399C_FHD_PLUSE_VIDEO_PANEL;
		else if (platform_is_sdm429())
			panel_id = HX8399C_HD_PLUS_VIDEO_PANEL;
		else
			panel_id = TRULY_1080P_VIDEO_PANEL;
		break;
	case HW_PLATFORM_QRD:
		if (platform_is_qm215()) {
			panel_id = HX8399C_HD_PLUS_VIDEO_PANEL;
			break;
		}

		if (hw_subtype == HW_PLATFORM_SUBTYPE_POLARIS) {
			panel_id = BYD_1200P_VIDEO_PANEL;
			break;
		}

		target_id = board_target_id();
		plat_hw_ver_major = ((target_id >> 16) & 0xFF);

		/*
		 * 8952 SKUM DVT2 - HX8399A 1080p video panel
		 * 8952 SKUM EVT1/EVT2 - OTM1906C 1080p cmd panel
		 */
		if (plat_hw_ver_major >= 4)
			panel_id = HX8399A_1080P_VIDEO_PANEL;
		else
			panel_id = OTM1906C_1080P_CMD_PANEL;

		if (platform_is_msm8937()){
			if (hw_subtype == 0x80) {
				/* 8940 SKU7 uses HX8394F */
				panel_id = HX8394F_720P_VIDEO_PANEL;
			} else {
				/* 8937 SKU1 uses R69006, SKU2 uses HX8394F */
				if (plat_hw_ver_major > 16)
					panel_id = HX8394F_720P_VIDEO_PANEL;
				else
					panel_id = R69006_1080P_CMD_PANEL;
			}
		}

		if (platform_is_sdm439()) {
			panel_id = HX8399C_FHD_PLUSE_VIDEO_PANEL;
		}

		if (platform_is_sdm429() || platform_is_sdm429w() || platform_is_sda429w()) {
			if (hw_subtype == HW_PLATFORM_SUBTYPE_429W_PM660) /* WTP 2700 DVT */
			  panel_id = RM67162_QVGA_CMD_PANEL;
			else if (hw_subtype == HW_PLATFORM_SUBTYPE_429W_PM660_WDP) /* WDP 2700 */
			  panel_id = RM69090_QVGA_CMD_PANEL;
      else
				panel_id = HX8399C_HD_PLUS_VIDEO_PANEL;
		}

		/* QRD EVT1 uses OTM1906C, and EVT2 uses HX8394F */
		if (platform_is_msm8956()) {
			switch (auto_pan_loop) {
				case 0:
					panel_id = HX8399A_1080P_VIDEO_PANEL;
					break;
				case 1:
					panel_id = OTM1906C_1080P_CMD_PANEL;
					break;
				default:
					panel_id = UNKNOWN_PANEL;
					dprintf(CRITICAL, "Unknown panel\n");
					return PANEL_TYPE_UNKNOWN;
			}
			auto_pan_loop++;
		} else if (platform_is_msm8917()) {
			if (hw_subtype == 0x0A) /* TMO target */
				panel_id = LEAD_FL10802_FWVGA_VIDEO_PANEL;
			else
				panel_id = HX8394F_720P_VIDEO_PANEL;
		}

		break;
	default:
		dprintf(CRITICAL, "Display not enabled for %d HW type\n",
			hw_id);
		return PANEL_TYPE_UNKNOWN;
	}

panel_init:
	/*
	 * Update all data structures after 'panel_init' label. Only panel
	 * selection is supposed to happen before that.
	 */
	if (platform_is_sdm439() || platform_is_sdm429() || platform_is_sdm429w() || platform_is_sda429w()) {
		phy_db->pll_type = DSI_PLL_TYPE_12NM;
		pinfo->lane_config = mdss_dsi_lane_config;
		goto end;
	}

	if (platform_is_msm8956())
		memcpy(panel_regulator_settings,
			dcdc_regulator_settings_hpm, REGULATOR_SIZE);
	else
		memcpy(panel_regulator_settings,
			dcdc_regulator_settings_lpm, REGULATOR_SIZE);
end:
	pinfo->pipe_type = MDSS_MDP_PIPE_TYPE_RGB;
	return init_panel_data(panelstruct, pinfo, phy_db);
}
