/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <edp.h>
#include <msm_panel.h>

static void edp_auo_1080p_init_edid_data(struct edp_edid_x *edid)
{
	edid->id_name[0] = 'A';
	edid->id_name[0] = 'U';
	edid->id_name[0] = 'O';
	edid->id_name[0] = 0;
	edid->id_product = 0x305D;
	edid->version = 1;
	edid->revision = 4;
	edid->ext_block_cnt = 0;
	edid->video_digital = 0x5;
	edid->color_depth = 6;
	edid->dpm = 0;
	edid->color_format = 0;
	edid->timing[0].pclk = 138500000;
	edid->timing[0].h_addressable = 1920;
	edid->timing[0].h_blank = 160;
	edid->timing[0].v_addressable = 1080;
	edid->timing[0].v_blank = 30;
	edid->timing[0].h_fporch = 48;
	edid->timing[0].h_sync_pulse = 32;
	edid->timing[0].v_sync_pulse = 14;
	edid->timing[0].v_fporch = 8;
	edid->timing[0].width_mm =  256;
	edid->timing[0].height_mm = 144;
	edid->timing[0].h_border = 0;
	edid->timing[0].v_border = 0;
	edid->timing[0].interlaced = 0;
	edid->timing[0].stereo = 0;
	edid->timing[0].sync_type = 1;
	edid->timing[0].sync_separate = 1;
	edid->timing[0].vsync_pol = 0;
	edid->timing[0].hsync_pol = 0;

}

static void edp_auo_1080p_init_dpcd_data(struct dpcd_cap_x *cap)
{
	cap->max_lane_count = 2;
	cap->max_link_clk = 270;
}

void edp_auo_1080p_init(struct edp_panel_data *edp_panel)
{
	if (!edp_panel->panel_data) {
		dprintf(CRITICAL, "%s(), panel_data is null", __func__);
		return;
	}

	edp_auo_1080p_init_edid_data(&(edp_panel->edid));
	edp_auo_1080p_init_dpcd_data(&(edp_panel->dpcd));
	edp_edid2pinfo(edp_panel);

	edp_panel->panel_data->panel_info.on = edp_on;
	edp_panel->panel_data->panel_info.off = edp_off;
}
