/* Copyright (c) 2012-2017, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MSM_SHARED_MSM_PANEL_H_
#define _PLATFORM_MSM_SHARED_MSM_PANEL_H_

#include <sys/types.h>
#include <stdint.h>
#include <dev/fbcon.h>
#include <sys/types.h>

#define DFPS_MAX_FRAME_RATE 20
#define DFPS_PLL_CODES_SIZE 0x1000 /* One page */

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
#define EDP_PANEL		12	/* EDP */
#define QPIC_PANEL		13	/* QPIC */

#define DISPLAY_UNKNOWN		0
#define DISPLAY_1		1
#define DISPLAY_2		2

enum mdss_mdp_pipe_type {
	MDSS_MDP_PIPE_TYPE_VIG,
	MDSS_MDP_PIPE_TYPE_RGB,
	MDSS_MDP_PIPE_TYPE_DMA,
};

enum msm_mdp_hw_revision {
	MDP_REV_20 = 1,
	MDP_REV_22,
	MDP_REV_30,
	MDP_REV_303,
	MDP_REV_304,
	MDP_REV_305,
	MDP_REV_31,
	MDP_REV_40,
	MDP_REV_41,
	MDP_REV_42,
	MDP_REV_43,
	MDP_REV_44,
	MDP_REV_50,
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

struct hdmi_panel_info {
	uint32_t h_back_porch;
	uint32_t h_front_porch;
	uint32_t h_pulse_width;
	uint32_t v_back_porch;
	uint32_t v_front_porch;
	uint32_t v_pulse_width;
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
	uint8_t dual_pipe;
	uint8_t split_display;
	uint8_t pipe_swap;
	uint8_t dst_split;
};

enum {
	COMPRESSION_NONE,
	COMPRESSION_DSC,
	COMPRESSION_FBC
};

#define DCS_HDR_LEN	4
#define DSC_PPS_LEN	128

struct msm_panel_info;

struct dsc_desc {
	int initial_lines;
	int slice_last_group_size;
	int bpp;        /* target bit per pixel */
	int bpc;        /* bit per component */
	int line_buf_depth;
	int config_by_manufacture_cmd;
	int block_pred_enable;
	int vbr_enable;
	int enable_422;
	int convert_rgb;
	int input_10_bits;
	int slice_per_pkt;

	int major;
	int minor;
	int scr_rev;
	int pps_id;

	int pic_height;
	int pic_width;
	int slice_height;
	int slice_width;
	int chunk_size;

	int pkt_per_line;
	int bytes_in_slice;
	int bytes_per_pkt;
	int eol_byte_num;
	int pclk_per_line;      /* width */

	int initial_dec_delay;
	int initial_xmit_delay;

	int initial_scale_value;
	int scale_decrement_interval;
	int scale_increment_interval;

	int first_line_bpg_offset;
	int nfl_bpg_offset;
	int slice_bpg_offset;

	int initial_offset;
	int final_offset;

	int rc_model_size;      /* rate_buffer_size */

	int det_thresh_flatness;
	int max_qp_flatness;
	int min_qp_flatness;
	int edge_factor;
	int quant_incr_limit0;
	int quant_incr_limit1;
	int tgt_offset_hi;
	int tgt_offset_lo;
	char *buf_thresh;
	char *range_min_qp;
	char *range_max_qp;
	char *range_bpg_offset;
	char pps_buf[DCS_HDR_LEN + DSC_PPS_LEN];

	void (*parameter_calc) (struct msm_panel_info *pinfo);
	int (*dsc2buf) (struct msm_panel_info *pinfo);
	void (*dsi_dsc_config) (uint32_t base, int mode, struct dsc_desc *dsc);
	void (*mdp_dsc_config) (struct msm_panel_info *pinfo,
		unsigned int pp_base, unsigned int dsc_base,
		bool mux, bool split_mode);
};

struct fbc_panel_info {
	uint32_t enabled;
	uint32_t comp_ratio;
	uint32_t comp_mode;
	uint32_t qerr_enable;
	uint32_t cd_bias;
	uint32_t pat_enable;
	uint32_t vlc_enable;
	uint32_t bflc_enable;

	uint32_t line_x_budget;
	uint32_t block_x_budget;
	uint32_t block_budget;

	uint32_t lossless_mode_thd;
	uint32_t lossy_mode_thd;
	uint32_t lossy_rgb_thd;
	uint32_t lossy_mode_idx;

	uint32_t slice_height;
	uint32_t pred_mode;
	uint32_t max_pred_err;
};


struct dfps_panel_info {
	uint32_t enabled;
	uint32_t frame_rate_cnt;
	uint32_t frame_rate[DFPS_MAX_FRAME_RATE];
};

struct dfps_pll_codes {
	uint32_t codes[2];
};

struct dfps_codes_info {
	uint32_t is_valid;
	uint32_t frame_rate;
	uint32_t clk_rate;
	struct dfps_pll_codes pll_codes;
};

struct dfps_info {
	struct dfps_panel_info panel_dfps;
	struct dfps_codes_info codes_dfps[DFPS_MAX_FRAME_RATE];
	void *dfps_fb_base;
	uint32_t chip_serial;
};

/* intf timing settings */
struct intf_timing_params {
	uint32_t width;
	uint32_t height;
	uint32_t xres;
	uint32_t yres;

	uint32_t h_back_porch;
	uint32_t h_front_porch;
	uint32_t v_back_porch;
	uint32_t v_front_porch;
	uint32_t hsync_pulse_width;
	uint32_t vsync_pulse_width;

	uint32_t border_clr;
	uint32_t underflow_clr;
	uint32_t hsync_skew;
};

struct mipi_panel_info {
	char cmds_post_tg;	/* send on commands after tg on */
	char mode;		/* video/cmd */
	char interleave_mode;
	int eof_bllp_power;
	uint32_t bitclock;
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
	struct mdss_dsi_phy_ctrl *mdss_dsi_phy_db;
	struct mdss_dsi_pll_config *dsi_pll_config;
	struct mipi_dsi_cmd *panel_on_cmds;
	int num_of_panel_on_cmds;
	struct mipi_dsi_cmd *panel_off_cmds;
	int num_of_panel_off_cmds;
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
	uint8_t dual_dsi;
	uint8_t use_dsi1_pll;
	uint8_t broadcast;
	uint8_t mode_gpio_state;
	uint32_t signature;
	uint32_t use_enable_gpio;
	uint32_t ctl_base;
	uint32_t phy_base;
	uint32_t sctl_base;
	uint32_t sphy_base;
	uint32_t reg_base;
	uint32_t sreg_base;
	uint32_t pll_base;
	uint32_t spll_base;

	struct dfps_pll_codes pll_codes;
};

struct edp_panel_info {
	int max_lane_count;
	unsigned long max_link_clk;
};

struct dsi2HDMI_panel_info {
	struct mipi_dsi_i2c_cmd *dsi_tg_i2c_cmd;
	struct mipi_dsi_i2c_cmd *dsi_setup_cfg_i2c_cmd;
	int num_of_tg_i2c_cmds;
	int num_of_cfg_i2c_cmds;
	uint8_t i2c_main_addr;
	uint8_t i2c_cec_addr;
	bool program_i2c_addr;
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

struct labibb_desc {
	char amoled_panel; /* lcd = 0, amoled = 1*/
	char force_config; /* 0 to use default value */
	uint32_t ibb_min_volt;
	uint32_t ibb_max_volt;
	uint32_t lab_min_volt;
	uint32_t lab_max_volt;
	char pwr_up_delay; /* ndx to => 1250, 2500, 5000 and 10000 us */
	char pwr_down_delay; /* ndx to => 1250, 2500, 5000 and 10000 us */
	char ibb_discharge_en;
	bool swire_control;
};

struct msm_panel_info {
	uint32_t xres;
	uint32_t yres;
	uint32_t bpp;
	uint32_t type;
	uint32_t wait_cycle;
	uint32_t clk_rate;
	uint32_t orientation;
	uint32_t dest;
	uint32_t compression_mode;
	/*  Select pipe type for handoff */
	uint32_t pipe_type;
	char     lowpowerstop;
	char     lcd_reg_en;
	uint32_t border_top;
	uint32_t border_bottom;
	uint32_t border_left;
	uint32_t border_right;

	int lm_split[2];
	int num_dsc_enc;

	struct lcd_panel_info lcd;
	struct lcdc_panel_info lcdc;
	struct fbc_panel_info fbc;
	struct dsc_desc dsc;
	struct mipi_panel_info mipi;
	struct lvds_panel_info lvds;
	struct hdmi_panel_info hdmi;
	struct edp_panel_info edp;
	struct dsi2HDMI_panel_info adv7533;
	bool has_bridge_chip;

	struct dfps_info dfps;

	struct labibb_desc *labibb;

	int (*on) (void);
	int (*off) (void);
	int (*pre_on) (void);
	int (*pre_off) (void);
	int (*prepare) (void);
	int (*early_config) (void *pdata);
	int (*config) (void *pdata);
	int (*rotate) (void);

	char autorefresh_enable;
	uint32_t autorefresh_framenum;
};

struct msm_fb_panel_data {
	struct msm_panel_info panel_info;
	struct fbcon_config fb;
	int mdp_rev;
	int rotate;

	/* function entry chain */
	int (*power_func) (uint8_t enable, struct msm_panel_info *);
	uint32_t (*clk_func) (uint8_t enable, struct msm_panel_info *pinfo);
	int (*bl_func) (uint8_t enable);
	uint32_t (*pll_clk_func) (uint8_t enable, struct msm_panel_info *);
	int (*dfps_func)(struct msm_panel_info *);
	int (*post_power_func)(int enable);
	int (*pre_init_func)(void);
	int (*update_panel_info) (void);
	int (*dsi2HDMI_config) (struct msm_panel_info *);
};

#endif
