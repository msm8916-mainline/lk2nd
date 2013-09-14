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
#include <board.h>
#include <mipi_dsi.h>

#include "include/panel.h"
#include "panel_display.h"

/*---------------------------------------------------------------------------*/
/* GCDB Panel Database                                                       */
/*---------------------------------------------------------------------------*/
#include "include/panel_truly_wvga_cmd.h"
#include "include/panel_truly_wvga_video.h"

/*---------------------------------------------------------------------------*/
/* static panel selection variable                                           */
/*---------------------------------------------------------------------------*/
enum {
TRULY_WVGA_CMD_PANEL,
TRULY_WVGA_VIDEO_PANEL,
};

static uint32_t panel_id;

int oem_panel_rotation()
{
	/* OEM can keep there panel spefic on instructions in this
	function */
	return NO_ERROR;
}


int oem_panel_on()
{
	/* OEM can keep there panel spefic on instructions in this
	function */
	return NO_ERROR;
}

int oem_panel_off()
{
	/* OEM can keep there panel spefic off instructions in this
	function */
	return NO_ERROR;
}

static bool init_panel_data(struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	switch (panel_id) {
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
		panelstruct->backlightinfo = &truly_wvga_cmd_backlight;
		pinfo->mipi.panel_cmds
					= truly_wvga_cmd_on_command;
		pinfo->mipi.num_of_panel_cmds
					= TRULY_WVGA_CMD_ON_COMMAND;
		memcpy(phy_db->timing,
			truly_wvga_cmd_timings, TIMING_SIZE);
		break;
	case TRULY_WVGA_VIDEO_PANEL:
		panelstruct->paneldata    = &truly_wvga_video_panel_data;
		panelstruct->panelres     = &truly_wvga_video_panel_res;
		panelstruct->color        = &truly_wvga_video_color;
		panelstruct->videopanel   = &truly_wvga_video_video_panel;
		panelstruct->commandpanel = &truly_wvga_video_command_panel;
		panelstruct->state        = &truly_wvga_video_state;
		panelstruct->laneconfig   = &truly_wvga_video_lane_config;
		panelstruct->paneltiminginfo
					 = &truly_wvga_video_timing_info;
		panelstruct->backlightinfo = &truly_wvga_video_backlight;
		pinfo->mipi.panel_cmds
					= truly_wvga_video_on_command;
		pinfo->mipi.num_of_panel_cmds
					= TRULY_WVGA_VIDEO_ON_COMMAND;
		memcpy(phy_db->timing,
				truly_wvga_video_timings, TIMING_SIZE);
		break;
	default:
		dprintf(CRITICAL, "Panel ID not detected %d\n", panel_id);
		return false;
	}
	return true;
}

bool oem_panel_select(struct panel_struct *panelstruct,
			struct msm_panel_info *pinfo,
			struct mdss_dsi_phy_ctrl *phy_db)
{
	uint32_t hw_id = board_hardware_id();

	switch (hw_id) {
	case HW_PLATFORM_QRD:
	case HW_PLATFORM_MTP:
	case HW_PLATFORM_SURF:
		panel_id = TRULY_WVGA_VIDEO_PANEL;
		break;
	default:
		dprintf(CRITICAL, "Display not enabled for %d HW type\n", hw_id);
		return false;
	}

	return init_panel_data(panelstruct, pinfo, phy_db);
}
