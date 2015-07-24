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

#include "include/panel.h"
#include "panel_display.h"

/*---------------------------------------------------------------------------*/
/* GCDB Panel Database                                                       */
/*---------------------------------------------------------------------------*/
#include "include/panel_sharp_wqxga_dualdsi_video.h"
#include "include/panel_jdi_qhd_dualdsi_video.h"
#include "include/panel_jdi_qhd_dualdsi_cmd.h"
#include "include/panel_jdi_4k_dualdsi_video.h"
#include "include/panel_jdi_1080p_video.h"
#include "include/panel_sharp_1080p_cmd.h"
#include "include/panel_hx8379a_truly_fwvga_video.h"
#include "include/panel_nt35597_wqxga_video.h"
#include "include/panel_nt35597_wqxga_cmd.h"

/*---------------------------------------------------------------------------*/
/* static panel selection variable                                           */
/*---------------------------------------------------------------------------*/
enum {
SHARP_WQXGA_DUALDSI_VIDEO_PANEL,
JDI_QHD_DUALDSI_VIDEO_PANEL,
JDI_QHD_DUALDSI_CMD_PANEL,
JDI_4K_DUALDSI_VIDEO_PANEL,
JDI_1080P_VIDEO_PANEL,
SHARP_1080P_CMD_PANEL,
HX8379A_TRULY_FWVGA_VIDEO_PANEL,
NOVATEK_WQXGA_VIDEO_PANEL,
NOVATEK_WQXGA_CMD_PANEL,
UNKNOWN_PANEL
};

/*
 * The list of panels that are supported on this target.
 * Any panel in this list can be selected using fastboot oem command.
 */
static struct panel_list supp_panels[] = {
	{"sharp_wqxga_dualdsi_video", SHARP_WQXGA_DUALDSI_VIDEO_PANEL},
	{"jdi_qhd_dualdsi_video", JDI_QHD_DUALDSI_VIDEO_PANEL},
	{"jdi_qhd_dualdsi_cmd", JDI_QHD_DUALDSI_CMD_PANEL},
	{"jdi_4k_dualdsi_video", JDI_4K_DUALDSI_VIDEO_PANEL},
	{"jdi_1080p_video", JDI_1080P_VIDEO_PANEL},
	{"sharp_1080p_cmd", SHARP_1080P_CMD_PANEL},
	{"hx8379a_truly_fwvga_video", HX8379A_TRULY_FWVGA_VIDEO_PANEL},
	{"nt35597_wqxga_video", NOVATEK_WQXGA_VIDEO_PANEL},
	{"nt35597_wqxga_cmd", NOVATEK_WQXGA_CMD_PANEL},
};

static uint32_t panel_id;

int oem_panel_rotation()
{
	/* OEM can keep there panel specific on instructions in this
	function */
	return NO_ERROR;
}

int oem_panel_on()
{
	/* OEM can keep there panel specific on instructions in this
	function */
	if (panel_id == JDI_QHD_DUALDSI_CMD_PANEL) {
		/* needs extra delay to avoid unexpected artifacts */
		mdelay(JDI_QHD_DUALDSI_CMD_PANEL_ON_DELAY);

	}
	return NO_ERROR;
}

int oem_panel_off()
{
	/* OEM can keep there panel specific off instructions in this
	function */
	return NO_ERROR;
}

static bool init_panel_data(struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	int pan_type;

	switch (panel_id) {
	case SHARP_WQXGA_DUALDSI_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &sharp_wqxga_dualdsi_video_panel_data;
		panelstruct->paneldata->panel_operating_mode = 11;
		panelstruct->paneldata->panel_with_enable_gpio = 0;

		/*
		 * Even though this panel can be supported with a single pipe,
		 * enable ping-pong split and use two pipes for simplicity sake.
		 */
		if (platform_is_msm8992())
			panelstruct->paneldata->panel_operating_mode |= DST_SPLIT_FLAG;

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
			sharp_wqxga_dualdsi_video_timings, TIMING_SIZE);
		pinfo->dfps.panel_dfps = sharp_wqxga_dualdsi_video_dfps;
		pinfo->mipi.tx_eot_append = true;
		break;
	case JDI_QHD_DUALDSI_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		panelstruct->paneldata    = &jdi_qhd_dualdsi_video_panel_data;

		if (platform_is_msm8992())
			panelstruct->paneldata->panel_operating_mode |= DST_SPLIT_FLAG;

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
			jdi_qhd_dualdsi_video_timings, TIMING_SIZE);
		break;
	case JDI_QHD_DUALDSI_CMD_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		panelstruct->paneldata    = &jdi_qhd_dualdsi_cmd_panel_data;

		if (platform_is_msm8992())
			panelstruct->paneldata->panel_operating_mode |= DST_SPLIT_FLAG;

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
			jdi_qhd_dualdsi_cmd_timings, TIMING_SIZE);
		break;
	case JDI_4K_DUALDSI_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		pinfo->mipi.cmds_post_tg = 1;
		panelstruct->paneldata    = &jdi_4k_dualdsi_video_panel_data;

		if (platform_is_msm8992())
			panelstruct->paneldata->panel_operating_mode |= DST_SPLIT_FLAG;

		panelstruct->panelres     = &jdi_4k_dualdsi_video_panel_res;
		panelstruct->color        = &jdi_4k_dualdsi_video_color;
		panelstruct->videopanel   = &jdi_4k_dualdsi_video_video_panel;
		panelstruct->commandpanel = &jdi_4k_dualdsi_video_command_panel;
		panelstruct->state        = &jdi_4k_dualdsi_video_state;
		panelstruct->laneconfig   = &jdi_4k_dualdsi_video_lane_config;
		panelstruct->paneltiminginfo
			= &jdi_4k_dualdsi_video_timing_info;
		panelstruct->panelresetseq
					 = &jdi_4k_dualdsi_video_reset_seq;
		panelstruct->backlightinfo = &jdi_4k_dualdsi_video_backlight;
		pinfo->mipi.panel_on_cmds
			= jdi_4k_dualdsi_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= JDI_4K_DUALDSI_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= jdi_4k_dualdsi_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= JDI_4K_DUALDSI_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			jdi_4k_dualdsi_video_timings, TIMING_SIZE);

		pinfo->compression_mode = COMPRESSION_FBC;
		memcpy(&panelstruct->fbcinfo, &jdi_4k_dualdsi_video_fbc,
				sizeof(struct fb_compression));
		break;
	case JDI_1080P_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		panelstruct->paneldata    = &jdi_1080p_video_panel_data;
		panelstruct->panelres     = &jdi_1080p_video_panel_res;
		panelstruct->color        = &jdi_1080p_video_color;
		panelstruct->videopanel   = &jdi_1080p_video_video_panel;
		panelstruct->commandpanel = &jdi_1080p_video_command_panel;
		panelstruct->state        = &jdi_1080p_video_state;
		panelstruct->laneconfig   = &jdi_1080p_video_lane_config;
		panelstruct->paneltiminginfo
			= &jdi_1080p_video_timing_info;
		panelstruct->panelresetseq
					 = &jdi_1080p_video_panel_reset_seq;
		panelstruct->backlightinfo = &jdi_1080p_video_backlight;
		pinfo->mipi.panel_on_cmds
			= jdi_1080p_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= JDI_1080P_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= jdi_1080p_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= JDI_1080P_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			jdi_1080p_video_timings, TIMING_SIZE);
		break;
	case SHARP_1080P_CMD_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &sharp_1080p_cmd_panel_data;
		panelstruct->panelres     = &sharp_1080p_cmd_panel_res;
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
	case HX8379A_TRULY_FWVGA_VIDEO_PANEL:
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		panelstruct->paneldata    = &hx8379a_truly_fwvga_video_panel_data;
		panelstruct->panelres     = &hx8379a_truly_fwvga_video_panel_res;
		panelstruct->color        = &hx8379a_truly_fwvga_video_color;
		panelstruct->videopanel   = &hx8379a_truly_fwvga_video_video_panel;
		panelstruct->commandpanel = &hx8379a_truly_fwvga_video_command_panel;
		panelstruct->state        = &hx8379a_truly_fwvga_video_state;
		panelstruct->laneconfig   = &hx8379a_truly_fwvga_video_lane_config;
		panelstruct->paneltiminginfo
			= &hx8379a_truly_fwvga_video_timing_info;
		panelstruct->panelresetseq
			= &hx8379a_truly_fwvga_video_reset_seq;
		panelstruct->backlightinfo = &hx8379a_truly_fwvga_video_backlight;
		pinfo->mipi.panel_on_cmds
			= hx8379a_truly_fwvga_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= HX8379A_TRULY_FWVGA_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= hx8379a_truly_fwvga_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= HX8379A_TRULY_FWVGA_VIDEO_OFF_COMMAND;
		pinfo->mipi.broadcast = 0;
		memcpy(phy_db->timing,
					hx8379a_truly_fwvga_video_timings, TIMING_SIZE);
	case NOVATEK_WQXGA_VIDEO_PANEL:
		dprintf(ALWAYS, " Novatek 35597 panel selected\n");
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 0;
		panelstruct->paneldata    = &nt35597_wqxga_video_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 0;
		panelstruct->panelres     = &nt35597_wqxga_video_panel_res;
		panelstruct->color        = &nt35597_wqxga_video_color;
		panelstruct->videopanel   = &nt35597_wqxga_video_video_panel;
		panelstruct->commandpanel = &nt35597_wqxga_video_command_panel;
		panelstruct->state        = &nt35597_wqxga_video_state;
		panelstruct->laneconfig   = &nt35597_wqxga_video_lane_config;
		panelstruct->paneltiminginfo
			= &nt35597_wqxga_video_timing_info;
		panelstruct->panelresetseq
					 = &nt35597_wqxga_video_reset_seq;
		panelstruct->backlightinfo = &nt35597_wqxga_video_backlight;
		pinfo->mipi.panel_on_cmds
			= nt35597_wqxga_video_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= NT35597_WQXGA_VIDEO_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= nt35597_wqxga_video_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= NT35597_WQXGA_VIDEO_OFF_COMMAND;
		memcpy(phy_db->timing,
			nt35597_wqxga_video_timings, TIMING_SIZE);
		memcpy(&panelstruct->fbcinfo, &nt35597_wqxga_video_fbc,
				sizeof(struct fb_compression));
		break;
	case NOVATEK_WQXGA_CMD_PANEL:
		dprintf(ALWAYS, " Novatek 35597 command mode panel selected\n");
		pan_type = PANEL_TYPE_DSI;
		pinfo->lcd_reg_en = 1;
		panelstruct->paneldata    = &nt35597_wqxga_cmd_panel_data;
		panelstruct->paneldata->panel_with_enable_gpio = 0;
		panelstruct->panelres     = &nt35597_wqxga_cmd_panel_res;
		panelstruct->color        = &nt35597_wqxga_cmd_color;
		panelstruct->videopanel   = &nt35597_wqxga_cmd_video_panel;
		panelstruct->commandpanel = &nt35597_wqxga_cmd_command_panel;
		panelstruct->state        = &nt35597_wqxga_cmd_state;
		panelstruct->laneconfig   = &nt35597_wqxga_cmd_lane_config;
		panelstruct->paneltiminginfo
			= &nt35597_wqxga_cmd_timing_info;
		panelstruct->panelresetseq
					 = &nt35597_wqxga_cmd_reset_seq;
		panelstruct->backlightinfo = &nt35597_wqxga_cmd_backlight;
		pinfo->mipi.panel_on_cmds
			= nt35597_wqxga_cmd_on_command;
		pinfo->mipi.num_of_panel_on_cmds
			= NT35597_WQXGA_CMD_ON_COMMAND;
		pinfo->mipi.panel_off_cmds
			= nt35597_wqxga_cmd_off_command;
		pinfo->mipi.num_of_panel_off_cmds
			= NT35597_WQXGA_CMD_OFF_COMMAND;
		memcpy(phy_db->timing,
			nt35597_wqxga_cmd_timings, TIMING_SIZE);
		memcpy(&panelstruct->fbcinfo, &nt35597_wqxga_cmd_fbc,
				sizeof(struct fb_compression));
		break;
	default:
	case UNKNOWN_PANEL:
		pan_type = PANEL_TYPE_UNKNOWN;
		break;
	}
	return pan_type;
}

int oem_panel_select(const char *panel_name, struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	uint32_t hw_id = board_hardware_id();
	int32_t panel_override_id;

	phy_db->pll_type = DSI_PLL_TYPE_20NM;

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
	case HW_PLATFORM_LIQUID:
		panel_id = JDI_4K_DUALDSI_VIDEO_PANEL;
		break;
	default:
		dprintf(CRITICAL, "Display not enabled for %d HW type\n"
					, hw_id);
		return PANEL_TYPE_UNKNOWN;
	}

panel_init:
	if (panel_id == JDI_4K_DUALDSI_VIDEO_PANEL || panel_id == HX8379A_TRULY_FWVGA_VIDEO_PANEL)
		phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
	return init_panel_data(panelstruct, pinfo, phy_db);
}
