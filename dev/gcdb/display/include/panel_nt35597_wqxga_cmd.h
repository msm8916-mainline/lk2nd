/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_NT35597_WQXGA_CMD_H_
#define _PANEL_NT35597_WQXGA_CMD_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config nt35597_wqxga_cmd_panel_data = {
	"qcom,dsi_nt35597_wqxga_cmd", /* panel_node_id */
	"dsi:0:", /* panel_controller */
	"qcom,mdss-dsi-panel", /* panel_compatible */
	11,  /* panel_interface */
	1,  /* panel_type */
	"DISPLAY_1",  /* panel_destination */
	0, /* panel_orientation */
	0, /* panel_clockrate */
	60, /* panel_framerate */
	0, /* panel_channelid */
	0, /* dsi_virtualchannel_id */
	0, /* panel_broadcast_mode */
	0, /* panel_lp11_init */
	0, /* panel_init_delay */
	0, /* dsi_stream */
	0, /*  interleave_mode */
	0, /* panel_bitclock_freq */
	0, /* panel_operating_mode */
	0, /* panel_with_enable_gpio */
	0, /* mode_gpio_state */
	0, /* slave_panel_node_id */
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution nt35597_wqxga_cmd_panel_res = {
	1440, /* panel_width */
	2560, /* panel_height */
	152, /* hfront_porch */
	160, /* hback_porch */
	12, /* hpulse_width */
	0, /* hsync_skew */
	10, /* vfront_porch */
	9, /* vback_porch */
	1, /* vpulse_width */
	0, /* hleft_border */
	0, /* hright_border */
	0, /* vtop_border */
	0, /* vbottom_border */
	0, /* hactive_res */
	0, /* uint16_t vactive_res */
	0, /* invert_data_polarity */
	0, /* invert_vsync_polarity */
	0, /* invert_hsync_polarity */
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info nt35597_wqxga_cmd_color = {
	24, /* color_format */
	0, /* color_order */
	0xff, /* underflow_color */
	0, /* border_color */
	0, /* pixel_packing */
	0, /* pixel_alignment */
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char nt35597_wqxga_cmd_on_cmd0[] = {
	0xff, 0x10, 0x15, 0x80,
};

static char nt35597_wqxga_cmd_on_cmd1[] = {
	0xbb, 0x10, 0x15, 0x80,
};

static char nt35597_wqxga_cmd_on_cmd2[] = {
	0x06, 0x00, 0x39, 0x80,
	0x3b, 0x03, 0x09, 0x0a,
	0x98, 0xa0,
};

static char nt35597_wqxga_cmd_on_cmd3[] = {
	0xe5, 0x00, 0x15, 0x80
};

static char nt35597_wqxga_cmd_on_cmd4[] = {
	0xc0, 0x01, 0x15, 0x80,
};

static char nt35597_wqxga_cmd_on_cmd5[] = {
	0xfb, 0x01, 0x15, 0x80,
};

static char nt35597_wqxga_cmd_on_cmd6[] = {
	0x11, 0x00, 0x05, 0x80
};

static char nt35597_wqxga_cmd_on_cmd7[] = {
	0x29, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd nt35597_wqxga_cmd_on_command[] = {
	{0x4, nt35597_wqxga_cmd_on_cmd0, 0x00},
	{0x4, nt35597_wqxga_cmd_on_cmd1, 0x00},
	{0xA, nt35597_wqxga_cmd_on_cmd2, 0x00},
	{0x4, nt35597_wqxga_cmd_on_cmd3, 0x00},
	{0x4, nt35597_wqxga_cmd_on_cmd4, 0x00},
	{0x4, nt35597_wqxga_cmd_on_cmd5, 0x00},
	{0x4, nt35597_wqxga_cmd_on_cmd6, 0x78},
	{0x4, nt35597_wqxga_cmd_on_cmd7, 0x78}
};

#define NT35597_WQXGA_CMD_ON_COMMAND 8


static char nt35597_wqxga_cmdoff_cmd0[] = {
	0x28, 0x00, 0x05, 0x80
};

static char nt35597_wqxga_cmdoff_cmd1[] = {
	0x10, 0x00, 0x05, 0x80
};

static struct mipi_dsi_cmd nt35597_wqxga_cmd_off_command[] = {
	{0x4, nt35597_wqxga_cmdoff_cmd0, 0x02},
	{0x4, nt35597_wqxga_cmdoff_cmd1, 0x78}
};

#define NT35597_WQXGA_CMD_OFF_COMMAND 2


static struct command_state nt35597_wqxga_cmd_state = {
	0, 1
};

/*---------------------------------------------------------------------------*/
/* Command mode panel information                                            */
/*---------------------------------------------------------------------------*/
static struct commandpanel_info nt35597_wqxga_cmd_command_panel = {
	//1, 1, 1, 0, 0, 0x2c, 0, 0, 0, 1, 0, 0
	0, /* techeck_enable*/
	0, /* tepin_select */
	0, /* teusing_tepin */
	0, /* autorefresh_enable */
	0, /* autorefresh_framenumdiv */
	0x2c, /* tevsync_rdptr_irqline */
	0, /* tevsync_continue_lines */
	0, /* tevsync_startline_divisor */
	0, /* tepercent_variance */
	0, /* tedcs_command */
	0, /* disable_eotafter_hsxfer */
	0, /* cmdmode_idletime */
};

/*---------------------------------------------------------------------------*/
/* Video mode panel information                                              */
/*---------------------------------------------------------------------------*/
static struct videopanel_info nt35597_wqxga_cmd_video_panel = {
	0, /* hsync_pulse */
	0, /* hfp_power_mode */
	0, /* hbp_power_mode */
	0, /* hsa_power_mode */
	1, /* bllp_eof_power_mode */
	1, /* bllp_power_mode */
	1, /* traffic_mode */
	0, /* dma_delayafter_vsync */
	0x9, /* bllp_eof_power */
};

/*---------------------------------------------------------------------------*/
/* Lane configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct lane_configuration nt35597_wqxga_cmd_lane_config = {
	4, 0, 1, 1, 1, 1, 0
};

/*---------------------------------------------------------------------------*/
/* Panel timing                                                              */
/*---------------------------------------------------------------------------*/
static const uint32_t nt35597_wqxga_cmd_timings[] = {
	0xC4, 0x2E, 0x20, 0x00, 0x5C, 0x5E, 0x24, 0x32, 0x24, 0x03, 0x04, 0x00
};

static struct panel_timing nt35597_wqxga_cmd_timing_info = {
	0x00, /* dsi_mdp_trigger */
	0x04, /* dsi_dma_trigger */
	0x03, /* tclk_post */
	0x25  /* tclk_pre */
};

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence nt35597_wqxga_cmd_reset_seq = {
	{1, 0, 1, }, {10, 10, 10, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight nt35597_wqxga_cmd_backlight = {
	0, 1, 4095, 100, 1, "PMIC_8941"
};

static struct fb_compression nt35597_wqxga_cmd_fbc = {
	1, /* enabled */
	3, /* comp_ratio */
	1, /* comp_mode */
	1, /* qerr_enable */
	2, /* cd_bias */
	1, /* pat_enable */
	1, /* vlc_enable */
	1, /* bflc_enable */
	900, /* line_x_budget */
	5, /* block_x_budget */
	59, /* block_budget */
	0x200, /* lossless_mode_thd */
	192, /* lossy_mode_thd */
	4, /* lossy_rgb_thd */
	3, /* lossy_mode_idx */
	4, /* slice_height */
	1, /* pred_mode */
	2, /* max_pred_err */
};

#endif /*_PANEL_NT35597_WQXGA_CMD_H_*/
