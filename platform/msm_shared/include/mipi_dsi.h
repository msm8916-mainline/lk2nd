/* Copyright (c) 2010-2015, The Linux Foundation. All rights reserved.
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

#ifndef _PLATFORM_MSM_SHARED_MIPI_DSI_H_
#define _PLATFORM_MSM_SHARED_MIPI_DSI_H_

#include <msm_panel.h>

#define PASS                        0
#define FAIL                        1

/**********************************************************
  DSI register offset
 **********************************************************/
#define DSI_CLKOUT_TIMING_CTRL                REG_DSI(0x0C0)
#define DSI_SOFT_RESET                        REG_DSI(0x114)
#define DSI_CAL_CTRL                          REG_DSI(0x0F4)

#define DSIPHY_SW_RESET                       REG_DSI(0x128)
#define DSIPHY_PLL_RDY                        REG_DSI(0x280)
#define DSIPHY_REGULATOR_CAL_PWR_CFG          REG_DSI(0x518)

#define DSI_CLK_CTRL                          REG_DSI(0x118)
#define DSI_TRIG_CTRL                         REG_DSI(0x080)
#define DSI_CTRL                              REG_DSI(0x000)
#define DSI_COMMAND_MODE_DMA_CTRL             REG_DSI(0x038)
#define DSI_COMMAND_MODE_MDP_CTRL             REG_DSI(0x03C)
#define DSI_COMMAND_MODE_MDP_DCS_CMD_CTRL     REG_DSI(0x040)
#define DSI_DMA_CMD_OFFSET                    REG_DSI(0x044)
#define DSI_DMA_CMD_LENGTH                    REG_DSI(0x048)
#define DSI_COMMAND_MODE_MDP_STREAM0_CTRL     REG_DSI(0x054)
#define DSI_COMMAND_MODE_MDP_STREAM0_TOTAL    REG_DSI(0x058)
#define DSI_COMMAND_MODE_MDP_STREAM1_CTRL     REG_DSI(0x05C)
#define DSI_COMMAND_MODE_MDP_STREAM1_TOTAL    REG_DSI(0x060)
#define DSI_ERR_INT_MASK0                     REG_DSI(0x108)
#define DSI_INT_CTRL                          REG_DSI(0x10C)

#define DSI_VIDEO_MODE_ACTIVE_H               REG_DSI(0x020)
#define DSI_VIDEO_MODE_ACTIVE_V               REG_DSI(0x024)
#define DSI_VIDEO_MODE_TOTAL                  REG_DSI(0x028)
#define DSI_VIDEO_MODE_HSYNC                  REG_DSI(0x02C)
#define DSI_VIDEO_MODE_VSYNC                  REG_DSI(0x030)
#define DSI_VIDEO_MODE_VSYNC_VPOS             REG_DSI(0x034)

#define DSI_MISR_CMD_CTRL                     REG_DSI(0x09C)
#define DSI_MISR_VIDEO_CTRL                   REG_DSI(0x0A0)
#define DSI_EOT_PACKET_CTRL                   REG_DSI(0x0C8)
#define DSI_VIDEO_MODE_CTRL                   REG_DSI(0x00C)
#define DSI_CAL_STRENGTH_CTRL                 REG_DSI(0x100)
#define DSI_CMD_MODE_DMA_SW_TRIGGER           REG_DSI(0x08C)
#define DSI_CMD_MODE_MDP_SW_TRIGGER           REG_DSI(0x090)
#define DSI_HS_TIMER_CTRL                     REG_DSI(0x0B8)

#define DSI_LANE_CTRL                         REG_DSI(0x0A8)

#define DSI_VIDEO_MODE_DONE_MASK              BIT(17)
#define DSI_VIDEO_MODE_DONE_AK                BIT(16)
#define DSI_VIDEO_MODE_DONE_STAT              BIT(16)

/**********************************************************
  DSI register configuration options
 **********************************************************/
#define MIPI_DSI_MRPS       0x04	/* Maximum Return Packet Size */
#define MIPI_DSI_REG_LEN    16	/* 4 x 4 bytes register */

#define TIMING_FLUSH		     0x1E4
#define TIMING_DB_MODE		     0x1E8

#define DSI_HW_REV_103			0x10030000	/* 8994 */
#define DSI_HW_REV_103_1		0x10030001	/* 8936/8939 */

#define DTYPE_GEN_WRITE2 0x23	/* 4th Byte is 0x80 */
#define DTYPE_GEN_LWRITE 0x29	/* 4th Byte is 0xc0 */
#define DTYPE_DCS_WRITE1 0x15	/* 4th Byte is 0x80 */

#define RDBK_DATA0 0x06C

#define MIPI_VIDEO_MODE	        1
#define MIPI_CMD_MODE           2

#define DSI_NON_BURST_SYNCH_PULSE       0
#define DSI_NON_BURST_SYNCH_EVENT       1
#define DSI_BURST_MODE                  2

#define DSI_RGB_SWAP_RGB                0
#define DSI_RGB_SWAP_RBG                1
#define DSI_RGB_SWAP_BGR                2
#define DSI_RGB_SWAP_BRG                3
#define DSI_RGB_SWAP_GRB                4
#define DSI_RGB_SWAP_GBR                5

#define DSI_VIDEO_DST_FORMAT_RGB565     0
#define DSI_VIDEO_DST_FORMAT_RGB666     1
#define DSI_VIDEO_DST_FORMAT_RGB666_LOOSE    2
#define DSI_VIDEO_DST_FORMAT_RGB888     3

#define DSI_CMD_DST_FORMAT_RGB111       0
#define DSI_CMD_DST_FORMAT_RGB332       3
#define DSI_CMD_DST_FORMAT_RGB444       4
#define DSI_CMD_DST_FORMAT_RGB565       6
#define DSI_CMD_DST_FORMAT_RGB666       7
#define DSI_CMD_DST_FORMAT_RGB888       8

#define DSI_CMD_TRIGGER_NONE            0x0	/* mdp trigger */
#define DSI_CMD_TRIGGER_TE              0x02
#define DSI_CMD_TRIGGER_SW              0x04
#define DSI_CMD_TRIGGER_SW_SEOF         0x05	/* cmd dma only */
#define DSI_CMD_TRIGGER_SW_TE           0x06

#define DSI_DATALANE_SWAP_0123          0
#define DSI_DATALANE_SWAP_3012          1
#define DSI_DATALANE_SWAP_2301          2
#define DSI_DATALANE_SWAP_1230          3
#define DSI_DATALANE_SWAP_0321          4
#define DSI_DATALANE_SWAP_1032          5
#define DSI_DATALANE_SWAP_2103          6
#define DSI_DATALANE_SWAP_3210          7

#define MAX_REGULATOR_CONFIG            7
#define MAX_BIST_CONFIG                 6
#define MAX_TIMING_CONFIG               12
#define MAX_LANE_CONFIG                 45
#define MAX_STRENGTH_CONFIG             2
#define MAX_CTRL_CONFIG                 4

/**********************************************************
  DSI configuration structures
 **********************************************************/
struct mipi_dsi_phy_ctrl {
       uint32_t regulator[5];
       uint32_t timing[12];
       uint32_t ctrl[4];
       uint32_t strength[4];
       uint32_t pll[21];
};

enum dsi_reg_mode {
	DSI_PHY_REGULATOR_DCDC_MODE,
	DSI_PHY_REGULATOR_LDO_MODE,
};

struct mdss_dsi_phy_ctrl {
	uint32_t regulator[MAX_REGULATOR_CONFIG];
	uint32_t timing[MAX_TIMING_CONFIG];
	uint32_t ctrl[MAX_CTRL_CONFIG];
	uint32_t strength[MAX_STRENGTH_CONFIG];
	char bistCtrl[MAX_BIST_CONFIG];
	char laneCfg[MAX_LANE_CONFIG];
	enum dsi_reg_mode regulator_mode;
	int is_pll_20nm;
};

typedef struct mdss_dsi_pll_config {
	uint32_t  pixel_clock;
	uint32_t  pixel_clock_mhz;
	uint32_t  byte_clock;
	uint32_t  bit_clock;
	uint32_t  halfbit_clock;
	uint32_t  vco_clock;
	uint32_t  vco_delay;
	uint8_t   directpath;
	uint8_t   posdiv1;
	uint8_t   posdiv3;
	uint8_t   pclk_m;
	uint8_t   pclk_n;
	uint8_t   pclk_d;

	/* pll 20nm */
	uint32_t  dec_start;
	uint32_t  frac_start;
	uint32_t  lock_comp;
	uint8_t  hr_oclk2;
	uint8_t  hr_oclk3;
	uint8_t  lp_div_mux;
	uint8_t  ndiv;
};

struct mipi_dsi_cmd {
	int size;
	char *payload;
	int wait;
	uint8_t cmds_post_tg;
};

struct mipi_dsi_panel_config {
	char mode;
	char num_of_lanes;
	char lane_swap;
	char force_clk_lane_hs;
	char pack;
	uint8_t t_clk_pre;
	uint8_t t_clk_post;
	struct mipi_dsi_phy_ctrl *dsi_phy_config;
	struct mdss_dsi_phy_ctrl *mdss_dsi_phy_config;
	struct mipi_dsi_cmd *panel_cmds;
	int num_of_panel_cmds;
	uint32_t signature;
	char cmds_post_tg;
};

static char read_id_a1h_cmd[4] = { 0xA1, 0x00, 0x06, 0xA0 };	/* DTYPE_DCS_READ */
static struct mipi_dsi_cmd read_ddb_start_cmd =
	{sizeof(read_id_a1h_cmd), read_id_a1h_cmd};

enum {		/* mipi dsi panel */
	DSI_VIDEO_MODE,
	DSI_CMD_MODE,
};

/**********************************************************
  APIs
 **********************************************************/
int mipi_config(struct msm_fb_panel_data *panel);
int mdss_dsi_config(struct msm_fb_panel_data *panel);
int mdss_dsi_phy_init(struct mipi_dsi_panel_config *,
		uint32_t ctl_base, uint32_t phy_base);
void mdss_dsi_phy_contention_detection(struct mipi_dsi_panel_config *,
				uint32_t phy_base);

int mdss_dsi_video_mode_config(uint16_t disp_width,
	uint16_t disp_height,
	uint16_t img_width,
	uint16_t img_height,
	uint16_t hsync_porch0_fp,
	uint16_t hsync_porch0_bp,
	uint16_t vsync_porch0_fp,
	uint16_t vsync_porch0_bp,
	uint16_t hsync_width,
	uint16_t vsync_width,
	uint16_t dst_format,
	uint16_t traffic_mode,
	uint8_t lane_en,
	uint16_t low_pwr_stop_mode,
	uint8_t eof_bllp_pwr,
	uint8_t interleav,
	uint32_t ctl_base);

int mipi_dsi_on();
int mipi_dsi_off(struct msm_panel_info *pinfo);
int mipi_dsi_cmds_tx(struct mipi_dsi_cmd *cmds, int count);
int mipi_dsi_cmds_rx(char **rp, int len);

#endif
