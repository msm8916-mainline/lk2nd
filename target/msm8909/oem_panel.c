/* Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
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
#include <board.h>
#include <mipi_dsi.h>
#include <target/display.h>
#include "include/panel.h"
#include "panel_display.h"

#include "include/panel_hx8394d_480p_video.h"
#include "include/panel_hx8394d_720p_video.h"
#include "include/panel_sharp_qhd_video.h"
#include "include/panel_truly_wvga_cmd.h"
#include "include/panel_hx8379a_fwvga_skua_video.h"
#include "include/panel_ili9806e_fwvga_video.h"
#include "include/panel_hx8394d_qhd_video.h"
#include "include/panel_hx8379c_fwvga_video.h"
#include "include/panel_fl10802_fwvga_video.h"
#include "include/panel_auo_qvga_cmd.h"
#include "include/panel_auo_cx_qvga_cmd.h"
#include "include/panel_hx8394f_720p_video.h"

#define DISPLAY_MAX_PANEL_DETECTION 0
#define ILI9806E_FWVGA_VIDEO_PANEL_POST_INIT_DELAY 68

enum {
	QRD_SKUA = 0x00,
	QRD_SKUC = 0x08,
	QRD_SKUE = 0x09,
	QRD_SKUT = 0x0A,
};

/* mtp cdp subtype for wearables */
enum {
	MTP_WEAR_REV0 = 0x05,
	SURF_WEAR = 0x03,
	SWOC_WEAR = 0x09,
	MTP_WEAR_REV1 = 0x11,
};

/*---------------------------------------------------------------------------*/
/* static panel selection variable                                           */
/*---------------------------------------------------------------------------*/
static uint32_t auto_pan_loop = 0;

enum {
	HX8394D_480P_VIDEO_PANEL,
	HX8394D_720P_VIDEO_PANEL,
	SHARP_QHD_VIDEO_PANEL,
	TRULY_WVGA_CMD_PANEL,
	HX8379A_FWVGA_SKUA_VIDEO_PANEL,
	ILI9806E_FWVGA_VIDEO_PANEL,
	HX8394D_QHD_VIDEO_PANEL,
	HX8379C_FWVGA_VIDEO_PANEL,
	FL10802_FWVGA_VIDEO_PANEL,
	AUO_QVGA_CMD_PANEL,
	AUO_CX_QVGA_CMD_PANEL,
	HX8394F_720P_VIDEO_PANEL,
	UNKNOWN_PANEL
};

/*
 * The list of panels that are supported on this target.
 * Any panel in this list can be selected using fastboot oem command.
 */
static struct panel_list supp_panels[] = {
	{"hx8394d_480p_video", HX8394D_480P_VIDEO_PANEL},
	{"hx8394d_720p_video", HX8394D_720P_VIDEO_PANEL},
	{"sharp_qhd_video", SHARP_QHD_VIDEO_PANEL},
	{"truly_wvga_cmd", TRULY_WVGA_CMD_PANEL},
	{"hx8379a_fwvga_skua_video", HX8379A_FWVGA_SKUA_VIDEO_PANEL},
	{"ili9806e_fwvga_video",ILI9806E_FWVGA_VIDEO_PANEL},
	{"hx8394d_qhd_video", HX8394D_QHD_VIDEO_PANEL},
	{"hx8379c_fwvga_video",HX8379C_FWVGA_VIDEO_PANEL},
	{"fl10802_fwvga_video", FL10802_FWVGA_VIDEO_PANEL},
	{"auo_qvga_cmd", AUO_QVGA_CMD_PANEL},
	{"auo_cx_qvga_cmd", AUO_CX_QVGA_CMD_PANEL},
	{"hx8394f_720p_video", HX8394F_720P_VIDEO_PANEL},
};

static uint32_t panel_id;

int oem_panel_rotation()
{
	return NO_ERROR;
}

int oem_panel_on()
{
	/*
	 * OEM can keep there panel specific on instructions in this
	 * function
	 */
	if (panel_id == ILI9806E_FWVGA_VIDEO_PANEL)
		mdelay(ILI9806E_FWVGA_VIDEO_PANEL_POST_INIT_DELAY);

	return NO_ERROR;
}

int oem_panel_off()
{
	/*
	 * OEM can keep their panel specific off instructions
	 * in this function
	 */
	return NO_ERROR;
}

static int init_panel_data(struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	int pan_type = PANEL_TYPE_DSI;

	switch (panel_id) {
	case HX8394D_480P_VIDEO_PANEL:
		panelstruct->paneldata	  = &hx8394d_480p_video_panel_data;
		panelstruct->panelres	  = &hx8394d_480p_video_panel_res;
		panelstruct->color		  = &hx8394d_480p_video_color;
		panelstruct->videopanel   = &hx8394d_480p_video_video_panel;
		panelstruct->commandpanel = &hx8394d_480p_video_command_panel;
		panelstruct->state		  = &hx8394d_480p_video_state;
		panelstruct->laneconfig   = &hx8394d_480p_video_lane_config;
		panelstruct->paneltiminginfo
					 = &hx8394d_480p_video_timing_info;
		panelstruct->panelresetseq
					 = &hx8394d_480p_video_panel_reset_seq;
		panelstruct->backlightinfo = &hx8394d_480p_video_backlight;
		pinfo->mipi.panel_cmds
					= hx8394d_480p_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= HX8394D_480P_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
				hx8394d_480p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8394D_480P_VIDEO_SIGNATURE;
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
		pinfo->mipi.panel_cmds
					= hx8394d_720p_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= HX8394D_720P_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
				hx8394d_720p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8394D_720P_VIDEO_SIGNATURE;
		break;
        case SHARP_QHD_VIDEO_PANEL:
		panelstruct->paneldata    = &sharp_qhd_video_panel_data;
		panelstruct->panelres     = &sharp_qhd_video_panel_res;
		panelstruct->color        = &sharp_qhd_video_color;
		panelstruct->videopanel   = &sharp_qhd_video_video_panel;
		panelstruct->commandpanel = &sharp_qhd_video_command_panel;
		panelstruct->state        = &sharp_qhd_video_state;
		panelstruct->laneconfig   = &sharp_qhd_video_lane_config;
		panelstruct->paneltiminginfo
					= &sharp_qhd_video_timing_info;
		panelstruct->panelresetseq
					= &sharp_qhd_video_panel_reset_seq;
		panelstruct->backlightinfo = &sharp_qhd_video_backlight;
		pinfo->mipi.panel_cmds
					= sharp_qhd_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= SHARP_QHD_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing, sharp_qhd_video_timings, TIMING_SIZE);
		break;
	case TRULY_WVGA_CMD_PANEL:
		panelstruct->paneldata    = &truly_wvga_cmd_panel_data;
		panelstruct->panelres     = &truly_wvga_cmd_panel_res;
		panelstruct->color        = &truly_wvga_cmd_color;
		panelstruct->videopanel   = &truly_wvga_cmd_video_panel;
		panelstruct->commandpanel = &truly_wvga_cmd_command_panel;
		panelstruct->state        = &truly_wvga_cmd_state;
		panelstruct->laneconfig   = &truly_wvga_cmd_lane_config;
		panelstruct->paneltiminginfo
					= &truly_wvga_cmd_timing_info;
		panelstruct->panelresetseq
					= &truly_wvga_cmd_reset_seq;
		panelstruct->backlightinfo = &truly_wvga_cmd_backlight;
		pinfo->mipi.panel_cmds
					= truly_wvga_cmd_on_command;
		pinfo->mipi.num_of_panel_cmds
					= TRULY_WVGA_CMD_ON_COMMAND;
		memcpy(phy_db->timing,
				truly_wvga_cmd_timings, TIMING_SIZE);
               break;
	case HX8379A_FWVGA_SKUA_VIDEO_PANEL:
		panelstruct->paneldata	  = &hx8379a_fwvga_skua_video_panel_data;
		panelstruct->panelres	  = &hx8379a_fwvga_skua_video_panel_res;
		panelstruct->color	  = &hx8379a_fwvga_skua_video_color;
		panelstruct->videopanel   = &hx8379a_fwvga_skua_video_video_panel;
		panelstruct->commandpanel = &hx8379a_fwvga_skua_video_command_panel;
		panelstruct->state	  = &hx8379a_fwvga_skua_video_state;
		panelstruct->laneconfig   = &hx8379a_fwvga_skua_video_lane_config;
		panelstruct->paneltiminginfo
					 = &hx8379a_fwvga_skua_video_timing_info;
		panelstruct->panelresetseq
					 = &hx8379a_fwvga_skua_video_reset_seq;
		panelstruct->backlightinfo = &hx8379a_fwvga_skua_video_backlight;
		pinfo->mipi.panel_cmds
					= hx8379a_fwvga_skua_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= HX8379A_FWVGA_SKUA_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
				hx8379a_fwvga_skua_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8379A_FWVGA_SKUA_VIDEO_SIGNATURE;
		break;
	case ILI9806E_FWVGA_VIDEO_PANEL:
                panelstruct->paneldata    = &ili9806e_fwvga_video_panel_data;
                panelstruct->panelres     = &ili9806e_fwvga_video_panel_res;
                panelstruct->color        = &ili9806e_fwvga_video_color;
                panelstruct->videopanel   = &ili9806e_fwvga_video_video_panel;
                panelstruct->commandpanel = &ili9806e_fwvga_video_command_panel;
                panelstruct->state        = &ili9806e_fwvga_video_state;
                panelstruct->laneconfig   = &ili9806e_fwvga_video_lane_config;
                panelstruct->paneltiminginfo
                                         = &ili9806e_fwvga_video_timing_info;
                panelstruct->panelresetseq
                                         = &ili9806e_fwvga_video_reset_seq;
                panelstruct->backlightinfo = &ili9806e_fwvga_video_backlight;
                pinfo->mipi.panel_cmds
                                        = ili9806e_fwvga_video_on_command;
                pinfo->mipi.num_of_panel_cmds
                                        = ILI9806E_FWVGA_VIDEO_ON_COMMAND;
                memcpy(phy_db->timing,
                                ili9806e_fwvga_video_timings, TIMING_SIZE);
                pinfo->mipi.signature = ILI9806E_FWVGA_VIDEO_SIGNATURE;
                break;
	case HX8394D_QHD_VIDEO_PANEL:
		panelstruct->paneldata	  = &hx8394d_qhd_video_panel_data;
		panelstruct->panelres	  = &hx8394d_qhd_video_panel_res;
		panelstruct->color		  = &hx8394d_qhd_video_color;
		panelstruct->videopanel   = &hx8394d_qhd_video_video_panel;
		panelstruct->commandpanel = &hx8394d_qhd_video_command_panel;
		panelstruct->state		  = &hx8394d_qhd_video_state;
		panelstruct->laneconfig   = &hx8394d_qhd_video_lane_config;
		panelstruct->paneltiminginfo
					 = &hx8394d_qhd_video_timing_info;
		panelstruct->panelresetseq
					 = &hx8394d_qhd_video_panel_reset_seq;
		panelstruct->backlightinfo = &hx8394d_qhd_video_backlight;
		pinfo->mipi.panel_cmds
					= hx8394d_qhd_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= HX8394D_QHD_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
				hx8394d_qhd_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8394D_QHD_VIDEO_SIGNATURE;
		break;
	case HX8379C_FWVGA_VIDEO_PANEL:
		panelstruct->paneldata    = &hx8379c_fwvga_video_panel_data;
		panelstruct->panelres     = &hx8379c_fwvga_video_panel_res;
		panelstruct->color        = &hx8379c_fwvga_video_color;
		panelstruct->videopanel   = &hx8379c_fwvga_video_video_panel;
		panelstruct->commandpanel = &hx8379c_fwvga_video_command_panel;
		panelstruct->state        = &hx8379c_fwvga_video_state;
		panelstruct->laneconfig   = &hx8379c_fwvga_video_lane_config;
		panelstruct->paneltiminginfo
					= &hx8379c_fwvga_video_timing_info;
		panelstruct->panelresetseq
					= &hx8379c_fwvga_video_reset_seq;
		panelstruct->backlightinfo = &hx8379c_fwvga_video_backlight;
		pinfo->mipi.panel_cmds
					= hx8379c_fwvga_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= HX8379C_FWVGA_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
					hx8379c_fwvga_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8379C_FWVGA_VIDEO_SIGNATURE;
		break;
	case FL10802_FWVGA_VIDEO_PANEL:
		panelstruct->paneldata	  = &fl10802_fwvga_video_panel_data;
		panelstruct->panelres	  = &fl10802_fwvga_video_panel_res;
		panelstruct->color		  = &fl10802_fwvga_video_color;
		panelstruct->videopanel   = &fl10802_fwvga_video_video_panel;
		panelstruct->commandpanel = &fl10802_fwvga_video_command_panel;
		panelstruct->state		  = &fl10802_fwvga_video_state;
		panelstruct->laneconfig   = &fl10802_fwvga_video_lane_config;
		panelstruct->paneltiminginfo
					 = &fl10802_fwvga_video_timing_info;
		panelstruct->panelresetseq
					 = &fl10802_fwvga_video_reset_seq;
		panelstruct->backlightinfo = &fl10802_fwvga_video_backlight;
		pinfo->mipi.panel_cmds
					= fl10802_fwvga_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= FL10802_FWVGA_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
				fl10802_fwvga_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = FL10802_FWVGA_VIDEO_SIGNATURE;
		pinfo->mipi.cmds_post_tg = 1;
		break;
	case AUO_QVGA_CMD_PANEL:
		panelstruct->paneldata    = &auo_qvga_cmd_panel_data;
		panelstruct->panelres     = &auo_qvga_cmd_panel_res;
		panelstruct->color        = &auo_qvga_cmd_color;
		panelstruct->videopanel   = &auo_qvga_cmd_video_panel;
		panelstruct->commandpanel = &auo_qvga_cmd_command_panel;
		panelstruct->state        = &auo_qvga_cmd_state;
		panelstruct->laneconfig   = &auo_qvga_cmd_lane_config;
		panelstruct->paneltiminginfo
					= &auo_qvga_cmd_timing_info;
		panelstruct->panelresetseq
					= &auo_qvga_cmd_panel_reset_seq;
		panelstruct->backlightinfo
					= &auo_qvga_cmd_backlight;
		pinfo->mipi.panel_cmds
					= auo_qvga_cmd_on_command;
		pinfo->mipi.num_of_panel_cmds
					= auo_QVGA_CMD_ON_COMMAND;
		memcpy(phy_db->timing, auo_qvga_cmd_timings, TIMING_SIZE);
		break;
	case AUO_CX_QVGA_CMD_PANEL:
		panelstruct->paneldata    = &auo_cx_qvga_cmd_panel_data;
		panelstruct->panelres     = &auo_cx_qvga_cmd_panel_res;
		panelstruct->color        = &auo_cx_qvga_cmd_color;
		panelstruct->videopanel   = &auo_cx_qvga_cmd_video_panel;
		panelstruct->commandpanel = &auo_cx_qvga_cmd_command_panel;
		panelstruct->state        = &auo_cx_qvga_cmd_state;
		panelstruct->laneconfig   = &auo_cx_qvga_cmd_lane_config;
		panelstruct->paneltiminginfo
					= &auo_cx_qvga_cmd_timing_info;
		panelstruct->panelresetseq
					= &auo_cx_qvga_cmd_panel_reset_seq;
		panelstruct->backlightinfo
					= &auo_cx_qvga_cmd_backlight;
		pinfo->mipi.panel_cmds
					= auo_cx_qvga_cmd_on_command;
		pinfo->mipi.num_of_panel_cmds
					= auo_cx_QVGA_CMD_ON_COMMAND;
		memcpy(phy_db->timing, auo_cx_qvga_cmd_timings, TIMING_SIZE);
		break;
	case HX8394F_720P_VIDEO_PANEL:
		panelstruct->paneldata    = &hx8394f_720p_video_panel_data;
		panelstruct->panelres     = &hx8394f_720p_video_panel_res;
		panelstruct->color        = &hx8394f_720p_video_color;
		panelstruct->videopanel   = &hx8394f_720p_video_video_panel;
		panelstruct->commandpanel = &hx8394f_720p_video_command_panel;
		panelstruct->state        = &hx8394f_720p_video_state;
		panelstruct->laneconfig   = &hx8394f_720p_video_lane_config;
		panelstruct->paneltiminginfo
					= &hx8394f_720p_video_timing_info;
		panelstruct->panelresetseq
					= &hx8394f_720p_video_reset_seq;
		panelstruct->backlightinfo = &hx8394f_720p_video_backlight;
		pinfo->mipi.panel_cmds
					= hx8394f_720p_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= HX8394F_720P_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
					hx8394f_720p_video_timings, TIMING_SIZE);
		pinfo->mipi.signature = HX8394F_720P_VIDEO_SIGNATURE;
		break;
	case UNKNOWN_PANEL:
	default:
		memset(panelstruct, 0, sizeof(struct panel_struct));
		memset(pinfo->mipi.panel_cmds, 0, sizeof(struct mipi_dsi_cmd));
		pinfo->mipi.num_of_panel_cmds = 0;
		memset(phy_db->timing, 0, TIMING_SIZE);
		pan_type = PANEL_TYPE_UNKNOWN;
		break;
	}
	return pan_type;
}

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
	uint32_t platform_type = board_platform_id();
	uint32_t platform_subtype = board_hardware_subtype();
	int32_t panel_override_id;
	uint32_t target_id = 0, plat_hw_ver_major = 0;

	if (panel_name) {
		panel_override_id = panel_name_to_id(supp_panels,
				ARRAY_SIZE(supp_panels), panel_name);

		if (panel_override_id < 0) {
			dprintf(CRITICAL, "Not able to search the panel:%s\n",
					 panel_name + strspn(panel_name, " "));
		} else if (panel_override_id < UNKNOWN_PANEL) {
			/* panel override using fastboot oem command */
			panel_id = panel_override_id;

			dprintf(INFO, "OEM panel override:%s\n",
					panel_name + strspn(panel_name, " "));
			goto panel_init;
		}
	}
	switch (hw_id) {
	case HW_PLATFORM_SURF:
	case HW_PLATFORM_MTP:
	case HW_PLATFORM_RCM:
		switch (platform_subtype) {
			case MTP_WEAR_REV0:
			case MTP_WEAR_REV1:
			case SURF_WEAR:
			case SWOC_WEAR:
				if ((platform_type == MSM8909W) ||
					(platform_type == APQ8009W))
					panel_id = AUO_CX_QVGA_CMD_PANEL;
				else
					panel_id = HX8394D_480P_VIDEO_PANEL;

				break;
			default:
				panel_id = HX8394D_720P_VIDEO_PANEL;
		}
		break;
	case HW_PLATFORM_QRD:
		switch (platform_subtype) {
			case QRD_SKUA:
				target_id = board_target_id();
				plat_hw_ver_major = ((target_id >> 16) & 0xFF);
				if (plat_hw_ver_major == 6)
					panel_id = HX8394F_720P_VIDEO_PANEL;
				else
					panel_id = HX8379A_FWVGA_SKUA_VIDEO_PANEL;
				break;
			case QRD_SKUC:
				panel_id = ILI9806E_FWVGA_VIDEO_PANEL;
				break;
			case QRD_SKUE:
				panel_id = HX8379C_FWVGA_VIDEO_PANEL;
				break;
			case QRD_SKUT:
				panel_id = FL10802_FWVGA_VIDEO_PANEL;
				break;
			default:
				dprintf(CRITICAL, "QRD Display not enabled for %d type\n",
						platform_subtype);
				return PANEL_TYPE_UNKNOWN;
		}
		break;
	default:
		dprintf(CRITICAL, "Display not enabled for %d HW type\n",
			hw_id);
		return PANEL_TYPE_UNKNOWN;
	}

panel_init:
	phy_db->regulator_mode = DSI_PHY_REGULATOR_LDO_MODE;
	return init_panel_data(panelstruct, pinfo, phy_db);
}
