/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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

#ifndef _PLATFORM_MSM_SHARED_MSM_PANEL_H_
#define _PLATFORM_MSM_SHARED_MSM_PANEL_H_

#include <stdint.h>
#include <dev/fbcon.h>

#define TRUE	1
#define FALSE	0

/* panel type list */
#define NO_PANEL		0xffff	/* No Panel */
#define MDDI_PANEL		1	/* MDDI */
#define EBI2_PANEL		2	/* EBI2 */
#define LCDC_PANEL		3	/* internal LCDC type */
#define EXT_MDDI_PANEL		4	/* Ext.MDDI */
#define TV_PANEL		5	/* TV */
#define HDMI_PANEL		6	/* HDMI TV */
#define DTV_PANEL		7	/* DTV */
#define MIPI_VIDEO_PANEL	8	/* MIPI */
#define MIPI_CMD_PANEL		9	/* MIPI */
#define WRITEBACK_PANEL		10	/* Wifi display */
#define LVDS_PANEL		11	/* LVDS */

enum msm_mdp_hw_revision {
	MDP_REV_20 = 1,
	MDP_REV_22,
	MDP_REV_30,
	MDP_REV_303,
	MDP_REV_31,
	MDP_REV_40,
	MDP_REV_41,
	MDP_REV_42,
	MDP_REV_43,
	MDP_REV_44,
};

/* panel info type */
struct lcd_panel_info {
	uint32_t vsync_enable;
	uint32_t refx100;
	uint32_t v_back_porch;
	uint32_t v_front_porch;
	uint32_t v_pulse_width;
	uint32_t hw_vsync_mode;
	uint32_t vsync_notifier_period;
	uint32_t rev;
};

struct lcdc_panel_info {
	uint32_t h_back_porch;
	uint32_t h_front_porch;
	uint32_t h_pulse_width;
	uint32_t v_back_porch;
	uint32_t v_front_porch;
	uint32_t v_pulse_width;
	uint32_t border_clr;
	uint32_t underflow_clr;
	uint32_t hsync_skew;
	/* Pad width */
	uint32_t xres_pad;
	/* Pad height */
	uint32_t yres_pad;
};

struct mipi_panel_info {
	char mode;		/* video/cmd */
	char interleave_mode;
	char crc_check;
	char ecc_check;
	char dst_format;	/* shared by video and command */
	char num_of_lanes;
	char data_lane0;
	char data_lane1;
	char data_lane2;
	char data_lane3;
	char dlane_swap;	/* data lane swap */
	char rgb_swap;
	char b_sel;
	char g_sel;
	char r_sel;
	char rx_eot_ignore;
	char tx_eot_append;
	char t_clk_post; /* 0xc0, DSI_CLKOUT_TIMING_CTRL */
	char t_clk_pre;  /* 0xc0, DSI_CLKOUT_TIMING_CTRL */
	char vc;	/* virtual channel */
	struct mipi_dsi_phy_ctrl *dsi_phy_db;
	struct mipi_dsi_cmd *panel_cmds;
	int num_of_panel_cmds;
	/* video mode */
	char pulse_mode_hsa_he;
	char hfp_power_stop;
	char hbp_power_stop;
	char hsa_power_stop;
	char eof_bllp_power_stop;
	char bllp_power_stop;
	char traffic_mode;
	char frame_rate;
	/* command mode */
	char interleave_max;
	char insert_dcs_cmd;
	char wr_mem_continue;
	char wr_mem_start;
	char te_sel;
	char stream;	/* 0 or 1 */
	char mdp_trigger;
	char dma_trigger;
	uint32_t dsi_pclk_rate;
	/* The packet-size should not bet changed */
	char no_max_pkt_size;
	/* Clock required during LP commands */
	char force_clk_lane_hs;
	char lane_swap;
};

enum lvds_mode {
	LVDS_SINGLE_CHANNEL_MODE,
	LVDS_DUAL_CHANNEL_MODE,
};

struct lvds_panel_info {
	enum lvds_mode channel_mode;
	/* Channel swap in dual mode */
	char channel_swap;
};

struct msm_panel_info {
	uint32_t xres;
	uint32_t yres;
	uint32_t bpp;
	uint32_t type;
	uint32_t wait_cycle;
	uint32_t clk_rate;

	struct lcd_panel_info lcd;
	struct lcdc_panel_info lcdc;
	struct mipi_panel_info mipi;
	struct lvds_panel_info lvds;

	int (*on) (void);
	int (*off) (void);
	int (*config) (void *pdata);
	int (*rotate) (void);
};

struct msm_fb_panel_data {
	struct msm_panel_info panel_info;
	struct fbcon_config fb;
	int mdp_rev;
	int rotate;

	/* function entry chain */
	int (*power_func) (int enable);
	int (*clk_func) (int enable);
};


int msm_display_init(struct msm_fb_panel_data *pdata);
#endif
