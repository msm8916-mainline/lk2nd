/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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

/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include <stdint.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <sys/types.h>
#include <platform/iomap.h>
#include <err.h>
#include <reg.h>
#include <mdp5.h>
#include <string.h>


/*---------------------------------------------------------------------------*/
/* Panel Header                                                              */
/*---------------------------------------------------------------------------*/
#include "panel_display.h"
#include "include/panel.h"

/*---------------------------------------------------------------------------*/
/* Panel Init                                                                */
/*---------------------------------------------------------------------------*/
int dsi_panel_init(struct msm_panel_info *pinfo,
			struct panel_struct *pstruct)
{
	/* Resolution setting*/
	pinfo->xres = pstruct->panelres->panel_width;
	pinfo->yres = pstruct->panelres->panel_height;
	pinfo->lcdc.h_back_porch = pstruct->panelres->hback_porch;
	pinfo->lcdc.h_front_porch = pstruct->panelres->hfront_porch;
	pinfo->lcdc.h_pulse_width = pstruct->panelres->hpulse_width;
	pinfo->lcdc.v_back_porch = pstruct->panelres->vback_porch;
	pinfo->lcdc.v_front_porch = pstruct->panelres->vfront_porch;
	pinfo->lcdc.v_pulse_width = pstruct->panelres->vpulse_width;
	pinfo->lcdc.hsync_skew = pstruct->panelres->hsync_skew;
	pinfo->lcdc.xres_pad = pstruct->panelres->hleft_border +
				pstruct->panelres->hright_border;
	pinfo->lcdc.yres_pad = pstruct->panelres->vtop_border +
				 pstruct->panelres->vbottom_border;

	if (pstruct->paneldata->panel_operating_mode & DUAL_PIPE_FLAG)
		pinfo->lcdc.dual_pipe = 1;
	if (pstruct->paneldata->panel_operating_mode & PIPE_SWAP_FLAG)
		pinfo->lcdc.pipe_swap = 1;
	if (pstruct->paneldata->panel_operating_mode & SPLIT_DISPLAY_FLAG)
		pinfo->lcdc.split_display = 1;
	if (pstruct->paneldata->panel_operating_mode & DST_SPLIT_FLAG)
		pinfo->lcdc.dst_split = 1;

	/* Color setting*/
	pinfo->lcdc.border_clr = pstruct->color->border_color;
	pinfo->lcdc.underflow_clr = pstruct->color->underflow_color;
	pinfo->mipi.rgb_swap = pstruct->color->color_order;
	switch (pinfo->bpp) {
	case BPP_16:
		pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB565;
		break;
	case BPP_18:
		if (pstruct->color->pixel_packing)
			pinfo->mipi.dst_format
				= DSI_VIDEO_DST_FORMAT_RGB666_LOOSE;
		else
			pinfo->mipi.dst_format
				 = DSI_VIDEO_DST_FORMAT_RGB666;
		break;
	case BPP_24:
	default:
		pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
		break;
	}

	/* Panel generic info */
	pinfo->mipi.mode = pstruct->paneldata->panel_type;
	if (pinfo->mipi.mode) {
		pinfo->type = MIPI_CMD_PANEL;
	} else {
		pinfo->type = MIPI_VIDEO_PANEL;
	}
	pinfo->bpp = pstruct->color->color_format;
	pinfo->clk_rate = pstruct->paneldata->panel_clockrate;
	pinfo->orientation = pstruct->paneldata->panel_orientation;
	pinfo->mipi.interleave_mode = pstruct->paneldata->interleave_mode;
	pinfo->mipi.broadcast = pstruct->paneldata->panel_broadcast_mode;
	pinfo->mipi.vc = pstruct->paneldata->dsi_virtualchannel_id;
	pinfo->mipi.frame_rate = pstruct->paneldata->panel_framerate;
	pinfo->mipi.stream = pstruct->paneldata->dsi_stream;
	if (pstruct->paneldata->panel_operating_mode & DUAL_DSI_FLAG)
		pinfo->mipi.dual_dsi = 1;
	pinfo->mipi.mode_gpio_state = pstruct->paneldata->mode_gpio_state;
	pinfo->mipi.bitclock = pstruct->paneldata->panel_bitclock_freq;
	pinfo->mipi.use_enable_gpio =
		pstruct->paneldata->panel_with_enable_gpio;

	/* Video Panel configuration */
	pinfo->mipi.pulse_mode_hsa_he = pstruct->videopanel->hsync_pulse;
	pinfo->mipi.hfp_power_stop = pstruct->videopanel->hfp_power_mode;
	pinfo->mipi.hbp_power_stop = pstruct->videopanel->hbp_power_mode;
	pinfo->mipi.hsa_power_stop = pstruct->videopanel->hsa_power_mode;
	pinfo->mipi.eof_bllp_power_stop
			 = pstruct->videopanel->bllp_eof_power_mode;
	pinfo->mipi.bllp_power_stop = pstruct->videopanel->bllp_power_mode;
	pinfo->mipi.traffic_mode = pstruct->videopanel->traffic_mode;
	pinfo->mipi.eof_bllp_power = pstruct->videopanel->bllp_eof_power;

	/* Command Panel configuratoin */
	pinfo->mipi.insert_dcs_cmd = pstruct->commandpanel->tedcs_command;
	pinfo->mipi.wr_mem_continue
			 = pstruct->commandpanel->tevsync_continue_lines;
	pinfo->mipi.wr_mem_start
			 = pstruct->commandpanel->tevsync_rdptr_irqline;
	pinfo->mipi.te_sel = pstruct->commandpanel->tepin_select;

	/* Data lane configuraiton */
	pinfo->mipi.num_of_lanes = pstruct->laneconfig->dsi_lanes;
	pinfo->mipi.data_lane0 = pstruct->laneconfig->lane0_state;
	pinfo->mipi.data_lane1 = pstruct->laneconfig->lane1_state;
	pinfo->mipi.data_lane2 = pstruct->laneconfig->lane2_state;
	pinfo->mipi.data_lane3 = pstruct->laneconfig->lane3_state;
	pinfo->mipi.lane_swap = pstruct->laneconfig->dsi_lanemap;
	pinfo->mipi.force_clk_lane_hs = pstruct->laneconfig->force_clk_lane_hs;

	pinfo->mipi.t_clk_post = pstruct->paneltiminginfo->tclk_post;
	pinfo->mipi.t_clk_pre = pstruct->paneltiminginfo->tclk_pre;
	pinfo->mipi.mdp_trigger = pstruct->paneltiminginfo->dsi_mdp_trigger;
	pinfo->mipi.dma_trigger = pstruct->paneltiminginfo->dsi_dma_trigger;

	pinfo->fbc.enabled = pstruct->fbcinfo.enabled;
	if (pinfo->fbc.enabled) {
		pinfo->fbc.enabled = pstruct->fbcinfo.enabled;
		pinfo->fbc.comp_ratio= pstruct->fbcinfo.comp_ratio;
		pinfo->fbc.comp_mode = pstruct->fbcinfo.comp_mode;
		pinfo->fbc.qerr_enable = pstruct->fbcinfo.qerr_enable;
		pinfo->fbc.cd_bias = pstruct->fbcinfo.cd_bias;
		pinfo->fbc.pat_enable = pstruct->fbcinfo.pat_enable;
		pinfo->fbc.vlc_enable = pstruct->fbcinfo.vlc_enable;
		pinfo->fbc.bflc_enable = pstruct->fbcinfo.bflc_enable;
		pinfo->fbc.line_x_budget = pstruct->fbcinfo.line_x_budget;
		pinfo->fbc.block_x_budget = pstruct->fbcinfo.block_x_budget;
		pinfo->fbc.block_budget = pstruct->fbcinfo.block_budget;
		pinfo->fbc.lossless_mode_thd = pstruct->fbcinfo.lossless_mode_thd;
		pinfo->fbc.lossy_mode_thd = pstruct->fbcinfo.lossy_mode_thd;
		pinfo->fbc.lossy_rgb_thd = pstruct->fbcinfo.lossy_rgb_thd;
		pinfo->fbc.lossy_mode_idx = pstruct->fbcinfo.lossy_mode_idx;
	} else {
		pinfo->fbc.comp_ratio = 1;
	}

	pinfo->pre_on = dsi_panel_pre_on;
	pinfo->pre_off = dsi_panel_pre_off;
	pinfo->on = dsi_panel_post_on;
	pinfo->off = dsi_panel_post_off;
	pinfo->rotate = dsi_panel_rotation;
	pinfo->config = dsi_panel_config;

	return NO_ERROR;
}

/*---------------------------------------------------------------------------*/
/* Panel Callbacks                                                           */
/*---------------------------------------------------------------------------*/

int dsi_panel_pre_on()
{
	return target_display_pre_on();
}

int dsi_panel_pre_off()
{
	return target_display_pre_off();
}

int dsi_panel_post_on()
{
	int ret = NO_ERROR;

	ret = target_display_post_on();
	if (ret)
		return ret;

	return oem_panel_on();
}

int dsi_panel_post_off()
{
	int ret = NO_ERROR;

	ret = target_display_post_off();
	if (ret)
		return ret;

	return oem_panel_off();
}

int dsi_panel_rotation()
{
	return oem_panel_rotation();
}

int dsi_video_panel_config(struct msm_panel_info *pinfo,
			  struct lcdc_panel_info *plcdc
			   )
{
	int ret = NO_ERROR;
	uint8_t lane_enable = 0;
	uint32_t panel_width = pinfo->xres;
	uint32_t final_xres, final_yres, final_width;
	uint32_t final_height, final_hbp, final_hfp,final_vbp;
	uint32_t final_vfp, final_hpw, final_vpw;

	if (pinfo->mipi.dual_dsi)
		panel_width = panel_width / 2;

	if (pinfo->mipi.data_lane0)
		lane_enable |= (1 << 0);
	if (pinfo->mipi.data_lane1)
		lane_enable |= (1 << 1);
	if (pinfo->mipi.data_lane2)
		lane_enable |= (1 << 2);
	if (pinfo->mipi.data_lane3)
		lane_enable |= (1 << 3);

	final_xres = panel_width;
	final_width = panel_width + pinfo->lcdc.xres_pad;

	if (pinfo->fbc.enabled && pinfo->fbc.comp_ratio) {
		final_xres /= pinfo->fbc.comp_ratio;
		final_width /=	pinfo->fbc.comp_ratio;
		dprintf(SPEW, "DSI xres =%d final_width=%d\n", final_xres,
				final_width);
	}
	final_yres = pinfo->yres;
	final_height = pinfo->yres + pinfo->lcdc.yres_pad;
	final_hbp = pinfo->lcdc.h_back_porch;
	final_hfp = pinfo->lcdc.h_front_porch;
	final_vbp = pinfo->lcdc.v_back_porch;
	final_vfp = pinfo->lcdc.v_front_porch;
	final_hpw = pinfo->lcdc.h_pulse_width;
	final_vpw = pinfo->lcdc.v_pulse_width;

	ret = mdss_dsi_video_mode_config(final_width, final_height,
			final_xres, final_yres,
			final_hfp, final_hbp + final_hpw,
			final_vfp, final_vbp + final_vpw,
			final_hpw, final_vpw,
			pinfo->mipi.dst_format,
			pinfo->mipi.traffic_mode,
			lane_enable,
			pinfo->mipi.hsa_power_stop,
			pinfo->mipi.eof_bllp_power,
			pinfo->mipi.interleave_mode,
			MIPI_DSI0_BASE);

	if (pinfo->mipi.dual_dsi)
		ret = mdss_dsi_video_mode_config(final_width, final_height,
				final_xres, final_yres,
				final_hfp, final_hbp + final_hpw,
				final_vfp, final_vbp + final_vpw,
				final_hpw, final_vpw,
				pinfo->mipi.dst_format,
				pinfo->mipi.traffic_mode,
				lane_enable,
				pinfo->mipi.hsa_power_stop,
				pinfo->mipi.eof_bllp_power,
				pinfo->mipi.interleave_mode,
				MIPI_DSI1_BASE);

	return ret;
}

int dsi_cmd_panel_config (struct msm_panel_info *pinfo,
			struct lcdc_panel_info *plcdc)
{
	int ret = NO_ERROR;
	uint8_t lane_en = 0;
	uint8_t ystride = pinfo->bpp / 8;
	uint32_t panel_width = pinfo->xres;

	if (pinfo->mipi.dual_dsi)
		panel_width = panel_width / 2;

	if (pinfo->mipi.data_lane0)
		lane_en |= (1 << 0);
	if (pinfo->mipi.data_lane1)
		lane_en |= (1 << 1);
	if (pinfo->mipi.data_lane2)
		lane_en |= (1 << 2);
	if (pinfo->mipi.data_lane3)
		lane_en |= (1 << 3);

	ret = mdss_dsi_cmd_mode_config((panel_width + plcdc->xres_pad),
			(pinfo->yres + plcdc->yres_pad),
			panel_width, (pinfo->yres),
			pinfo->mipi.dst_format,
			ystride, lane_en,
			pinfo->mipi.interleave_mode,
			MIPI_DSI0_BASE);

	if (pinfo->mipi.dual_dsi)
		ret = mdss_dsi_cmd_mode_config((panel_width + plcdc->xres_pad),
			(pinfo->yres + plcdc->yres_pad),
			panel_width, (pinfo->yres),
			pinfo->mipi.dst_format,
			ystride, lane_en,
			pinfo->mipi.interleave_mode,
			MIPI_DSI1_BASE);

	return ret;
}


int dsi_panel_config(void *pdata)
{
	int ret = NO_ERROR;
	struct msm_panel_info *pinfo = (struct msm_panel_info *)pdata;
	struct lcdc_panel_info *plcdc = NULL;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	plcdc =  &(pinfo->lcdc);
	if (plcdc == NULL)
		return ERR_INVALID_ARGS;


	if (pinfo->mipi.mode == DSI_VIDEO_MODE) {
		ret = dsi_video_panel_config(pinfo, plcdc);
	} else {
		ret = dsi_cmd_panel_config(pinfo, plcdc);
	}

	return ret;
}

int32_t panel_name_to_id(struct panel_list supp_panels[],
			  uint32_t supp_panels_size,
			  const char *panel_name)
{
	uint32_t i;
	int32_t panel_id = ERR_NOT_FOUND;

	if (!panel_name) {
		dprintf(CRITICAL, "Invalid panel name\n");
		return panel_id;
	}

	/* Remove any leading whitespaces */
	panel_name += strspn(panel_name, " ");
	for (i = 0; i < supp_panels_size; i++) {
		if (!strncmp(panel_name, supp_panels[i].name,
			MAX_PANEL_ID_LEN)) {
			panel_id = supp_panels[i].id;
			break;
		}
	}

	return panel_id;
}
