/* Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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

#ifndef _PLATFORM_MSM_SHARED_MIPI_DSI_H_
#define _PLATFORM_MSM_SHARED_MIPI_DSI_H_

#define PASS                        0
#define FAIL                        1

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

#define MIPI_DSI_MRPS       0x04	/* Maximum Return Packet Size */
#define MIPI_DSI_REG_LEN    16	/* 4 x 4 bytes register */

#define DTYPE_GEN_WRITE2 0x23	/* 4th Byte is 0x80 */
#define DTYPE_GEN_LWRITE 0x29	/* 4th Byte is 0xc0 */
#define DTYPE_DCS_WRITE1 0x15	/* 4th Byte is 0x80 */

//BEGINNING OF Tochiba Config- video mode

static const unsigned char toshiba_panel_mcap_off[8] = {
	0x02, 0x00, 0x29, 0xc0,
	0xb2, 0x00, 0xff, 0xff
};

static const unsigned char toshiba_panel_ena_test_reg[8] = {
	0x03, 0x00, 0x29, 0xc0,
	0xEF, 0x01, 0x01, 0xff
};

static const unsigned char toshiba_panel_ena_test_reg_wvga[8] = {
	0x03, 0x00, 0x29, 0xc0,
	0xEF, 0x01, 0x01, 0xff
};

static const unsigned char toshiba_panel_num_of_2lane[8] = {
	0x03, 0x00, 0x29, 0xc0,	// 63:2lane
	0xEF, 0x60, 0x63, 0xff
};

static const unsigned char toshiba_panel_num_of_1lane[8] = {
	0x03, 0x00, 0x29, 0xc0,	// 62:1lane
	0xEF, 0x60, 0x62, 0xff
};

static const unsigned char toshiba_panel_non_burst_sync_pulse[8] = {
	0x03, 0x00, 0x29, 0xc0,
	0xef, 0x61, 0x09, 0xff
};

static const unsigned char toshiba_panel_set_DMODE_WQVGA[8] = {
	0x02, 0x00, 0x29, 0xc0,
	0xB3, 0x01, 0xFF, 0xff
};

static const unsigned char toshiba_panel_set_DMODE_WVGA[8] = {
	0x02, 0x00, 0x29, 0xc0,
	0xB3, 0x00, 0xFF, 0xff
};

static const unsigned char toshiba_panel_set_intern_WR_clk1_wvga[8]
    = {

	0x03, 0x00, 0x29, 0xC0,	// 1 last packet
	0xef, 0x2f, 0xcc, 0xff,
};

static const unsigned char toshiba_panel_set_intern_WR_clk2_wvga[8]
    = {

	0x03, 0x00, 0x29, 0xC0,	// 1 last packet
	0xef, 0x6e, 0xdd, 0xff,
};

static const unsigned char
 toshiba_panel_set_intern_WR_clk1_wqvga[8] = {

	0x03, 0x00, 0x29, 0xC0,	// 1 last packet
	0xef, 0x2f, 0x22, 0xff,
};

static const unsigned char
 toshiba_panel_set_intern_WR_clk2_wqvga[8] = {

	0x03, 0x00, 0x29, 0xC0,	// 1 last packet
	0xef, 0x6e, 0x33, 0xff,
};

static const unsigned char toshiba_panel_set_hor_addr_2A_wvga[12] = {

	0x05, 0x00, 0x39, 0xC0,	// 1 last packet
	// 0x2A, 0x00, 0x08, 0x00,//100 = 64h
	// 0x6b, 0xFF, 0xFF, 0xFF,
	0x2A, 0x00, 0x00, 0x01,	// 0X1DF = 480-1 0X13F = 320-1
	0xdf, 0xFF, 0xFF, 0xFF,
};

static const unsigned char toshiba_panel_set_hor_addr_2B_wvga[12] = {

	0x05, 0x00, 0x39, 0xC0,	// 1 last packet
	// 0x2B, 0x00, 0x08, 0x00,//0X355 = 854-1; 0X1DF = 480-1
	// 0x6b, 0xFF, 0xFF, 0xFF,
	0x2B, 0x00, 0x00, 0x03,	// 0X355 = 854-1; 0X1DF = 480-1
	0x55, 0xFF, 0xFF, 0xFF,
};

static const unsigned char toshiba_panel_set_hor_addr_2A_wqvga[12]
    = {

	0x05, 0x00, 0x39, 0xC0,	// 1 last packet
	0x2A, 0x00, 0x00, 0x00,	// 0XEF = 240-1
	0xef, 0xFF, 0xFF, 0xFF,
};

static const unsigned char toshiba_panel_set_hor_addr_2B_wqvga[12]
    = {

	0x05, 0x00, 0x39, 0xC0,	// 1 last packet
	0x2B, 0x00, 0x00, 0x01,	// 0X1aa = 427-1;
	0xaa, 0xFF, 0xFF, 0xFF,
};

static const unsigned char toshiba_panel_IFSEL[8] = {
	0x02, 0x00, 0x29, 0xc0,
	0x53, 0x01, 0xff, 0xff
};

static const unsigned char toshiba_panel_IFSEL_cmd_mode[8] = {
	0x02, 0x00, 0x29, 0xc0,
	0x53, 0x00, 0xff, 0xff
};

static const unsigned char toshiba_panel_exit_sleep[4] = {
	0x11, 0x00, 0x05, 0x80,	// 25 Reg 0x29 < Display On>; generic write 1
	// params
};

static const unsigned char toshiba_panel_display_on[4] = {
	// 0x29, 0x00, 0x05, 0x80,//25 Reg 0x29 < Display On>; generic write 1
	// params
	0x29, 0x00, 0x05, 0x80,	// 25 Reg 0x29 < Display On>; generic write 1
	// params
};

//color mode off
static const unsigned char dsi_display_config_color_mode_off[4] = {
	0x00, 0x00, 0x02, 0x80,
};

//color mode on
static const unsigned char dsi_display_config_color_mode_on[4] = {
	0x00, 0x00, 0x12, 0x80,
};

//the end OF Tochiba Config- video mode

/* NOVATEK BLUE panel */
static char novatek_panel_sw_reset[4] = { 0x01, 0x00, 0x05, 0x00 };	/* DTYPE_DCS_WRITE */
static char novatek_panel_enter_sleep[4] = { 0x10, 0x00, 0x05, 0x80 };	/* DTYPE_DCS_WRITE */
static char novatek_panel_exit_sleep[4] = { 0x11, 0x00, 0x05, 0x80 };	/* DTYPE_DCS_WRITE */
static char novatek_panel_display_off[4] = { 0x28, 0x00, 0x05, 0x80 };	/* DTYPE_DCS_WRITE */
static char novatek_panel_display_on[4] = { 0x29, 0x00, 0x05, 0x80 };	/* DTYPE_DCS_WRITE */
static char novatek_panel_max_packet[4] = { 0x04, 0x00, 0x37, 0x80 };	/* DTYPE_SET_MAX_PACKET */

static char novatek_panel_set_onelane[4] = { 0xae, 0x01, 0x15, 0x80 };	/* DTYPE_DCS_WRITE1 */
static char novatek_panel_rgb_888[4] = { 0x3A, 0x77, 0x15, 0x80 };	/* DTYPE_DCS_WRITE1 */
static char novatek_panel_set_twolane[4] = { 0xae, 0x03, 0x15, 0x80 };	/* DTYPE_DCS_WRITE1 */

static char novatek_panel_manufacture_id[4] = { 0x04, 0x00, 0x06, 0xA0 };	/* DTYPE_DCS_READ */

/* commands by Novatke */
static char novatek_panel_f4[4] = { 0xf4, 0x55, 0x15, 0x80 };	/* DTYPE_DCS_WRITE1 */

static char novatek_panel_8c[20] = {	/* DTYPE_DCS_LWRITE */
	0x10, 0x00, 0x39, 0xC0, 0x8C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x08, 0x08, 0x00, 0x30, 0xC0, 0xB7, 0x37
};
static char novatek_panel_ff[4] = { 0xff, 0x55, 0x15, 0x80 };	/* DTYPE_DCS_WRITE1 */

static char novatek_panel_set_width[12] = {	/* DTYPE_DCS_LWRITE */
	0x05, 0x00, 0x39, 0xC0,	//1 last packet
	0x2A, 0x00, 0x00, 0x02,	//clmn:0 - 0x21B=539
	0x1B, 0xFF, 0xFF, 0xFF
};				/* 540 - 1 */

static char novatek_panel_set_height[12] = {	/* DTYPE_DCS_LWRITE */
	0x05, 0x00, 0x39, 0xC0,	//1 last packet
	0x2B, 0x00, 0x00, 0x03,	//row:0 - 0x3BF=959
	0xBF, 0xFF, 0xFF, 0xFF,
};				/* 960 - 1 */

/* Commands to control Backlight */
static char novatek_panel_set_led_pwm1[8] = {	/* DTYPE_DCS_LWRITE */
	0x02, 0x00, 0x39, 0xC0,	//1 last packet
	0x51, 0xFA, 0xFF, 0xFF,	// Brightness level set to 0xFA -> 250
};

static char novatek_panel_set_led_pwm2[8] = {	/* DTYPE_DCS_LWRITE */
	0x02, 0x00, 0x39, 0xC0,
	0x53, 0x24, 0xFF, 0xFF,
};

static char novatek_panel_set_led_pwm3[8] = {	/* DTYPE_DCS_LWRITE */
	0x02, 0x00, 0x39, 0xC0,
	0x55, 0x00, 0xFF, 0xFF,
};

/* End of Novatek Blue panel commands */

/* Toshiba mdt61 panel cmds */
static const unsigned char toshiba_mdt61_mcap_start[4] = {
	0xB0, 0x04, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_num_out_pixelform[8] = {
	0x03, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xB3, 0x00, 0x87, 0xFF
};

static const unsigned char toshiba_mdt61_dsi_ctrl[8] = {
	0x03, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xB6, 0x30, 0x83, 0xFF
};

static const unsigned char toshiba_mdt61_panel_driving[12] = {
	0x07, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xC0, 0x01, 0x00, 0x85,
	0x00, 0x00, 0x00, 0xFF
};

static const unsigned char toshiba_mdt61_dispV_timing[12] = {
	0x05, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xC1, 0x00, 0x10, 0x00,
	0x01, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_dispCtrl[8] = {
	0x03, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xC3, 0x00, 0x19, 0xFF
};

static const unsigned char toshiba_mdt61_test_mode_c4[4] = {
	0xC4, 0x03, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_dispH_timing[20] = {
	0x0F, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xC5, 0x00, 0x01, 0x05,
	0x04, 0x5E, 0x00, 0x00,
	0x00, 0x00, 0x0B, 0x17,
	0x05, 0x00, 0x00, 0xFF
};

static const unsigned char toshiba_mdt61_test_mode_c6[4] = {
	0xC6, 0x00, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_gamma_setA[20] = {
	0x0D, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xC8, 0x0A, 0x15, 0x18,
	0x1B, 0x1C, 0x0D, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_gamma_setB[20] = {
	0x0D, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xC9, 0x0D, 0x1D, 0x1F,
	0x1F, 0x1F, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_gamma_setC[20] = {
	0x0D, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xCA, 0x1E, 0x1F, 0x1E,
	0x1D, 0x1D, 0x10, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_powerSet_ChrgPmp[12] = {
	0x05, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xD0, 0x02, 0x00, 0xA3,
	0xB8, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_testMode_d1[12] = {
	0x06, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xD1, 0x10, 0x14, 0x53,
	0x64, 0x00, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_powerSet_SrcAmp[8] = {
	0x03, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xD2, 0xB3, 0x00, 0xFF
};

static const unsigned char toshiba_mdt61_powerInt_PS[8] = {
	0x03, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xD3, 0x33, 0x03, 0xFF
};

static const unsigned char toshiba_mdt61_vreg[4] = {
	0xD5, 0x00, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_test_mode_d6[4] = {
	0xD6, 0x01, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_timingCtrl_d7[16] = {
	0x09, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xD7, 0x09, 0x00, 0x84,
	0x81, 0x61, 0xBC, 0xB5,
	0x05, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_timingCtrl_d8[12] = {
	0x07, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xD8, 0x04, 0x25, 0x90,
	0x4C, 0x92, 0x00, 0xFF
};

static const unsigned char toshiba_mdt61_timingCtrl_d9[8] = {
	0x04, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xD9, 0x5B, 0x7F, 0x05
};

static const unsigned char toshiba_mdt61_white_balance[12] = {
	0x06, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xCB, 0x00, 0x00, 0x00,
	0x1C, 0x00, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_vcs_settings[4] = {
	0xDD, 0x53, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_vcom_dc_settings[4] = {
	0xDE, 0x43, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_testMode_e3[12] = {
	0x05, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xE3, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_testMode_e4[12] = {
	0x06, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xE4, 0x00, 0x00, 0x22,
	0xAA, 0x00, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_testMode_e5[4] = {
	0xE5, 0x00, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_testMode_fa[8] = {
	0x04, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xFA, 0x00, 0x00, 0x00
};

static const unsigned char toshiba_mdt61_testMode_fd[12] = {
	0x05, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xFD, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_testMode_fe[12] = {
	0x05, 0x00, DTYPE_GEN_LWRITE, 0xC0,
	0xFE, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF
};

static const unsigned char toshiba_mdt61_mcap_end[4] = {
	0xB0, 0x03, DTYPE_GEN_WRITE2, 0x80,
};

static const unsigned char toshiba_mdt61_set_add_mode[4] = {
	0x36, 0x00, DTYPE_DCS_WRITE1, 0x80,
};

static const unsigned char toshiba_mdt61_set_pixel_format[4] = {
	0x3A, 0x70, DTYPE_DCS_WRITE1, 0x80,
};

/* Done Toshiba MDT61 Panel Commands */
/* Toshiba MDT61 (R69320) End */

static const unsigned char dsi_display_exit_sleep[4] = {
	0x11, 0x00, 0x15, 0x80,
};

static const unsigned char dsi_display_display_on[4] = {
	0x29, 0x00, 0x15, 0x80,
};

#define MIPI_VIDEO_MODE	        1
#define MIPI_CMD_MODE           2

struct mipi_dsi_phy_ctrl {
	uint32_t regulator[5];
	uint32_t timing[12];
	uint32_t ctrl[4];
	uint32_t strength[4];
	uint32_t pll[21];
};

struct mipi_dsi_cmd {
	int size;
	char *payload;
};

struct mipi_dsi_panel_config {
	char mode;
	char num_of_lanes;
	struct mipi_dsi_phy_ctrl *dsi_phy_config;
	struct mipi_dsi_cmd *panel_cmds;
	int num_of_panel_cmds;
	int lane_swap;
};

static struct mipi_dsi_cmd toshiba_panel_video_mode_cmds[] = {
	{sizeof(toshiba_panel_mcap_off), (char *)toshiba_panel_mcap_off},
	{sizeof(toshiba_panel_ena_test_reg),
	 (char *)toshiba_panel_ena_test_reg},
	{sizeof(toshiba_panel_num_of_1lane),
	 (char *)toshiba_panel_num_of_1lane},
	{sizeof(toshiba_panel_non_burst_sync_pulse),
	 (char *)toshiba_panel_non_burst_sync_pulse},
	{sizeof(toshiba_panel_set_DMODE_WVGA),
	 (char *)toshiba_panel_set_DMODE_WVGA},
	{sizeof(toshiba_panel_set_intern_WR_clk1_wvga),
	 (char *)toshiba_panel_set_intern_WR_clk1_wvga},
	{sizeof(toshiba_panel_set_intern_WR_clk2_wvga),
	 (char *)toshiba_panel_set_intern_WR_clk2_wvga},
	{sizeof(toshiba_panel_set_hor_addr_2A_wvga),
	 (char *)toshiba_panel_set_hor_addr_2A_wvga},
	{sizeof(toshiba_panel_set_hor_addr_2B_wvga),
	 (char *)toshiba_panel_set_hor_addr_2B_wvga},
	{sizeof(toshiba_panel_IFSEL), (char *)toshiba_panel_IFSEL},
	{sizeof(toshiba_panel_exit_sleep), (char *)toshiba_panel_exit_sleep},
	{sizeof(toshiba_panel_display_on), (char *)toshiba_panel_display_on},
	{sizeof(dsi_display_config_color_mode_on),
	 (char *)dsi_display_config_color_mode_on},
	{sizeof(dsi_display_config_color_mode_off),
	 (char *)dsi_display_config_color_mode_off},
};

static struct mipi_dsi_phy_ctrl mipi_dsi_toshiba_panel_phy_ctrl = {
	/* 480*854, RGB888, 1 Lane 60 fps video mode */
	{0x03, 0x01, 0x01, 0x00},	/* regulator */
	/* timing   */
	{0x50, 0x0f, 0x14, 0x19, 0x23, 0x0e, 0x12, 0x16,
	 0x1b, 0x1c, 0x04},
	{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
	{0xee, 0x03, 0x86, 0x03},	/* strength */
	/* pll control */

#if defined(DSI_BIT_CLK_366MHZ)
	{0x41, 0xdb, 0xb2, 0xf5, 0x00, 0x50, 0x48, 0x63,
	 0x31, 0x0f, 0x07,
	 0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03},
#elif defined(DSI_BIT_CLK_380MHZ)
	{0x41, 0xf7, 0xb2, 0xf5, 0x00, 0x50, 0x48, 0x63,
	 0x31, 0x0f, 0x07,
	 0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03},
#elif defined(DSI_BIT_CLK_400MHZ)
	{0x41, 0x8f, 0xb1, 0xda, 0x00, 0x50, 0x48, 0x63,
	 0x31, 0x0f, 0x07,
	 0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03},
#else				/* 200 mhz */
	{0x41, 0x8f, 0xb1, 0xda, 0x00, 0x50, 0x48, 0x63,
	 0x33, 0x1f, 0x1f /* for 1 lane ; 0x0f for 2 lanes */ ,
	 0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03},
#endif
};

static struct mipi_dsi_cmd toshiba_mdt61_video_mode_cmds[] = {
	{sizeof(toshiba_mdt61_mcap_start), (char *)toshiba_mdt61_mcap_start},
	{sizeof(toshiba_mdt61_num_out_pixelform),
	 (char *)toshiba_mdt61_num_out_pixelform},
	{sizeof(toshiba_mdt61_dsi_ctrl), (char *)toshiba_mdt61_dsi_ctrl},
	{sizeof(toshiba_mdt61_panel_driving),
	 (char *)toshiba_mdt61_panel_driving},
	{sizeof(toshiba_mdt61_dispV_timing),
	 (char *)toshiba_mdt61_dispV_timing},
	{sizeof(toshiba_mdt61_dispCtrl), (char *)toshiba_mdt61_dispCtrl},
	{sizeof(toshiba_mdt61_test_mode_c4),
	 (char *)toshiba_mdt61_test_mode_c4},
	{sizeof(toshiba_mdt61_dispH_timing),
	 (char *)toshiba_mdt61_dispH_timing},
	{sizeof(toshiba_mdt61_test_mode_c6),
	 (char *)toshiba_mdt61_test_mode_c6},
	{sizeof(toshiba_mdt61_gamma_setA), (char *)toshiba_mdt61_gamma_setA},
	{sizeof(toshiba_mdt61_gamma_setB), (char *)toshiba_mdt61_gamma_setB},
	{sizeof(toshiba_mdt61_gamma_setC), (char *)toshiba_mdt61_gamma_setC},
	{sizeof(toshiba_mdt61_powerSet_ChrgPmp),
	 (char *)toshiba_mdt61_powerSet_ChrgPmp},
	{sizeof(toshiba_mdt61_testMode_d1), (char *)toshiba_mdt61_testMode_d1},
	{sizeof(toshiba_mdt61_powerSet_SrcAmp),
	 (char *)toshiba_mdt61_powerSet_SrcAmp},
	{sizeof(toshiba_mdt61_powerInt_PS), (char *)toshiba_mdt61_powerInt_PS},
	{sizeof(toshiba_mdt61_vreg), (char *)toshiba_mdt61_vreg},
	{sizeof(toshiba_mdt61_test_mode_d6),
	 (char *)toshiba_mdt61_test_mode_d6},
	{sizeof(toshiba_mdt61_timingCtrl_d7),
	 (char *)toshiba_mdt61_timingCtrl_d7},
	{sizeof(toshiba_mdt61_timingCtrl_d8),
	 (char *)toshiba_mdt61_timingCtrl_d8},
	{sizeof(toshiba_mdt61_timingCtrl_d9),
	 (char *)toshiba_mdt61_timingCtrl_d9},
	{sizeof(toshiba_mdt61_white_balance),
	 (char *)toshiba_mdt61_white_balance},
	{sizeof(toshiba_mdt61_vcs_settings),
	 (char *)toshiba_mdt61_vcs_settings},
	{sizeof(toshiba_mdt61_vcom_dc_settings),
	 (char *)toshiba_mdt61_vcom_dc_settings},
	{sizeof(toshiba_mdt61_testMode_e3), (char *)toshiba_mdt61_testMode_e3},
	{sizeof(toshiba_mdt61_testMode_e4), (char *)toshiba_mdt61_testMode_e4},
	{sizeof(toshiba_mdt61_testMode_e5), (char *)toshiba_mdt61_testMode_e5},
	{sizeof(toshiba_mdt61_testMode_fa), (char *)toshiba_mdt61_testMode_fa},
	{sizeof(toshiba_mdt61_testMode_fd), (char *)toshiba_mdt61_testMode_fd},
	{sizeof(toshiba_mdt61_testMode_fe), (char *)toshiba_mdt61_testMode_fe},
	{sizeof(toshiba_mdt61_mcap_end), (char *)toshiba_mdt61_mcap_end},
	{sizeof(toshiba_mdt61_set_add_mode),
	 (char *)toshiba_mdt61_set_add_mode},
	{sizeof(toshiba_mdt61_set_pixel_format),
	 (char *)toshiba_mdt61_set_pixel_format},
	{sizeof(dsi_display_exit_sleep), (char *)dsi_display_exit_sleep},
	{sizeof(dsi_display_display_on), (char *)dsi_display_display_on},
};

static struct mipi_dsi_phy_ctrl mipi_dsi_toshiba_mdt61_panel_phy_ctrl = {
	/* 600*1024, RGB888, 3 Lane 55 fps video mode */
	{0x03, 0x0a, 0x04, 0x00, 0x20},
	/* timing   */
	{0xab, 0x8a, 0x18, 0x00, 0x92, 0x97, 0x1b, 0x8c,
	 0x0c, 0x03, 0x04, 0xa0},
	{0x5f, 0x00, 0x00, 0x10},	/* phy ctrl */
	{0xff, 0x00, 0x06, 0x00},	/* strength */

	/* pll control 1- 19 */
	{0x01, 0x7f, 0x31, 0xda, 0x00, 0x40, 0x03, 0x62,
	 0x41, 0x0f, 0x01,
	 0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01, 0x00},
};

static struct mipi_dsi_cmd novatek_panel_manufacture_id_cmd =
    { sizeof(novatek_panel_manufacture_id), novatek_panel_manufacture_id };

static struct mipi_dsi_cmd novatek_panel_cmd_mode_cmds[] = {
	{sizeof(novatek_panel_sw_reset), novatek_panel_sw_reset}
	,
	{sizeof(novatek_panel_exit_sleep), novatek_panel_exit_sleep}
	,
	{sizeof(novatek_panel_display_on), novatek_panel_display_on}
	,
	{sizeof(novatek_panel_max_packet), novatek_panel_max_packet}
	,
	{sizeof(novatek_panel_f4), novatek_panel_f4}
	,
	{sizeof(novatek_panel_8c), novatek_panel_8c}
	,
	{sizeof(novatek_panel_ff), novatek_panel_ff}
	,
	{sizeof(novatek_panel_set_twolane), novatek_panel_set_twolane}
	,
	{sizeof(novatek_panel_set_width), novatek_panel_set_width}
	,
	{sizeof(novatek_panel_set_height), novatek_panel_set_height}
	,
	{sizeof(novatek_panel_rgb_888), novatek_panel_rgb_888}
	,
	{sizeof(novatek_panel_set_led_pwm1), novatek_panel_set_led_pwm1}
	,
	{sizeof(novatek_panel_set_led_pwm2), novatek_panel_set_led_pwm2}
	,
	{sizeof(novatek_panel_set_led_pwm3), novatek_panel_set_led_pwm3}
};

static struct mipi_dsi_phy_ctrl mipi_dsi_novatek_panel_phy_ctrl = {
	/* DSI_BIT_CLK at 500MHz, 2 lane, RGB888 */
	{0x03, 0x01, 0x01, 0x00},	/* regulator */
	/* timing   */
	{0x96, 0x26, 0x23, 0x00, 0x50, 0x4B, 0x1e,
	 0x28, 0x28, 0x03, 0x04},
	{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
	{0xee, 0x02, 0x86, 0x00},	/* strength */
	/* pll control */
	{0x40, 0xf9, 0xb0, 0xda, 0x00, 0x50, 0x48, 0x63,
	 /* 0x30, 0x07, 0x07,  --> One lane configuration */
	 0x30, 0x07, 0x03,	/*  --> Two lane configuration */
	 0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};

/* Renesas Tremelo-M panel: List of commands */

static char config_sleep_out[4] = { 0x11, 0x00, 0x05, 0x80 };
static char config_CMD_MODE[4] = { 0x40, 0x01, 0x15, 0x80 };
static char config_WRTXHT[12] =
    { 0x07, 0x00, 0x39, 0xC0, 0x92, 0x16, 0x08, 0x08, 0x00, 0x01, 0xe0, 0xff };
static char config_WRTXVT[12] =
    { 0x07, 0x00, 0x39, 0xC0, 0x8b, 0x02, 0x02, 0x02, 0x00, 0x03, 0x60, 0xff };
static char config_PLL2NR[4] = { 0xa0, 0x24, 0x15, 0x80 };
static char config_PLL2NF1[4] = { 0xa2, 0xd0, 0x15, 0x80 };
static char config_PLL2NF2[4] = { 0xa4, 0x00, 0x15, 0x80 };
static char config_PLL2BWADJ1[4] = { 0xa6, 0xd0, 0x15, 0x80 };
static char config_PLL2BWADJ2[4] = { 0xa8, 0x00, 0x15, 0x80 };
static char config_PLL2CTL[4] = { 0xaa, 0x00, 0x15, 0x80 };
static char config_DBICBR[4] = { 0x48, 0x03, 0x15, 0x80 };
static char config_DBICTYPE[4] = { 0x49, 0x00, 0x15, 0x80 };
static char config_DBICSET1[4] = { 0x4a, 0x1c, 0x15, 0x80 };
static char config_DBICADD[4] = { 0x4b, 0x00, 0x15, 0x80 };
static char config_DBICCTL[4] = { 0x4e, 0x01, 0x15, 0x80 };

/* static char config_COLMOD_565[4] = {0x3a, 0x05, 0x15, 0x80}; */
/* static char config_COLMOD_666PACK[4] = {0x3a, 0x06, 0x15, 0x80}; */
static char config_COLMOD_888[4] = { 0x3a, 0x07, 0x15, 0x80 };
static char config_MADCTL[4] = { 0x36, 0x00, 0x15, 0x80 };
static char config_DBIOC[4] = { 0x82, 0x40, 0x15, 0x80 };
static char config_CASET[12] =
    { 0x07, 0x00, 0x39, 0xC0, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x01, 0xdf, 0xff };
static char config_PASET[12] =
    { 0x07, 0x00, 0x39, 0xC0, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x03, 0x5f, 0xff };
static char config_TXON[4] = { 0x81, 0x00, 0x05, 0x80 };
static char config_BLSET_TM[4] = { 0xff, 0x6c, 0x15, 0x80 };

static char config_AGCPSCTL_TM[4] = { 0x56, 0x08, 0x15, 0x80 };

static char config_DBICADD70[4] = { 0x4b, 0x70, 0x15, 0x80 };
static char config_DBICSET_15[4] = { 0x4a, 0x15, 0x15, 0x80 };
static char config_DBICADD72[4] = { 0x4b, 0x72, 0x15, 0x80 };

static char config_Power_Ctrl_2a_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x10, 0xff };
static char config_Auto_Sequencer_Setting_a_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char Driver_Output_Ctrl_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x01, 0xff };
static char Driver_Output_Ctrl_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x10, 0xff };
static char config_LCD_drive_AC_Ctrl_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x02, 0xff };
static char config_LCD_drive_AC_Ctrl_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x00, 0xff };
static char config_Entry_Mode_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x03, 0xff };
static char config_Entry_Mode_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_Display_Ctrl_1_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x07, 0xff };
static char config_Display_Ctrl_1_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_Display_Ctrl_2_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x08, 0xff };
static char config_Display_Ctrl_2_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x04, 0xff };
static char config_Display_Ctrl_3_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x09, 0xff };
static char config_Display_Ctrl_3_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x0c, 0xff };
static char config_Display_IF_Ctrl_1_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x0c, 0xff };
static char config_Display_IF_Ctrl_1_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x10, 0xff };
static char config_Display_IF_Ctrl_2_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x0e, 0xff };
static char config_Display_IF_Ctrl_2_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };

static char config_Panel_IF_Ctrl_1_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x20, 0xff };
static char config_Panel_IF_Ctrl_1_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x3f, 0xff };
static char config_Panel_IF_Ctrl_3_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x22, 0xff };
static char config_Panel_IF_Ctrl_3_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x76, 0x00, 0xff };
static char config_Panel_IF_Ctrl_4_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x23, 0xff };
static char config_Panel_IF_Ctrl_4_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x1c, 0x0a, 0xff };
static char config_Panel_IF_Ctrl_5_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x24, 0xff };
static char config_Panel_IF_Ctrl_5_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x1c, 0x2c, 0xff };
static char config_Panel_IF_Ctrl_6_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x25, 0xff };
static char config_Panel_IF_Ctrl_6_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x1c, 0x4e, 0xff };
static char config_Panel_IF_Ctrl_8_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x27, 0xff };
static char config_Panel_IF_Ctrl_8_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_Panel_IF_Ctrl_9_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x28, 0xff };
static char config_Panel_IF_Ctrl_9_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x76, 0x0c, 0xff };

static char config_gam_adjust_00_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x00, 0xff };
static char config_gam_adjust_00_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_01_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x01, 0xff };
static char config_gam_adjust_01_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x05, 0x02, 0xff };
static char config_gam_adjust_02_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x02, 0xff };
static char config_gam_adjust_02_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x05, 0xff };
static char config_gam_adjust_03_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x03, 0xff };
static char config_gam_adjust_03_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_04_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x04, 0xff };
static char config_gam_adjust_04_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x00, 0xff };
static char config_gam_adjust_05_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x05, 0xff };
static char config_gam_adjust_05_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_06_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x06, 0xff };
static char config_gam_adjust_06_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x10, 0x10, 0xff };
static char config_gam_adjust_07_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x07, 0xff };
static char config_gam_adjust_07_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x02, 0xff };
static char config_gam_adjust_08_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x08, 0xff };
static char config_gam_adjust_08_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x04, 0xff };
static char config_gam_adjust_09_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x09, 0xff };
static char config_gam_adjust_09_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_0A_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0a, 0xff };
static char config_gam_adjust_0A_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_0B_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0b, 0xff };
static char config_gam_adjust_0B_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_0C_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0c, 0xff };
static char config_gam_adjust_0C_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_0D_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0d, 0xff };
static char config_gam_adjust_0D_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x10, 0x10, 0xff };
static char config_gam_adjust_10_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x10, 0xff };
static char config_gam_adjust_10_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x04, 0xff };
static char config_gam_adjust_11_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x11, 0xff };
static char config_gam_adjust_11_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x05, 0x03, 0xff };
static char config_gam_adjust_12_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x12, 0xff };
static char config_gam_adjust_12_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x04, 0xff };
static char config_gam_adjust_15_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x15, 0xff };
static char config_gam_adjust_15_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x04, 0xff };
static char config_gam_adjust_16_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x16, 0xff };
static char config_gam_adjust_16_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x1c, 0xff };
static char config_gam_adjust_17_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x17, 0xff };
static char config_gam_adjust_17_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x04, 0xff };
static char config_gam_adjust_18_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x18, 0xff };
static char config_gam_adjust_18_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x04, 0x02, 0xff };
static char config_gam_adjust_19_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x19, 0xff };
static char config_gam_adjust_19_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x05, 0xff };
static char config_gam_adjust_1C_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x1c, 0xff };
static char config_gam_adjust_1C_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_1D_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x1D, 0xff };
static char config_gam_adjust_1D_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x1f, 0xff };
static char config_gam_adjust_20_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x20, 0xff };
static char config_gam_adjust_20_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x05, 0x07, 0xff };
static char config_gam_adjust_21_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x21, 0xff };
static char config_gam_adjust_21_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x06, 0x04, 0xff };
static char config_gam_adjust_22_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x22, 0xff };
static char config_gam_adjust_22_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x04, 0x05, 0xff };
static char config_gam_adjust_27_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x27, 0xff };
static char config_gam_adjust_27_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x03, 0xff };
static char config_gam_adjust_28_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x28, 0xff };
static char config_gam_adjust_28_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x00, 0xff };
static char config_gam_adjust_29_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x29, 0xff };
static char config_gam_adjust_29_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x02, 0xff };

static char config_Power_Ctrl_1_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x00, 0xff };
static char config_Power_Ctrl_1b_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x36, 0x3c, 0xff };
static char config_Power_Ctrl_2_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x01, 0xff };
static char config_Power_Ctrl_2b_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x03, 0xff };
static char config_Power_Ctrl_3_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x02, 0xff };
static char config_Power_Ctrl_3a_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x01, 0xff };
static char config_Power_Ctrl_4_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x03, 0xff };
static char config_Power_Ctrl_4a_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x3c, 0x58, 0xff };
static char config_Power_Ctrl_6_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x0c, 0xff };
static char config_Power_Ctrl_6a_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x35, 0xff };

static char config_Auto_Sequencer_Setting_b_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x02, 0xff };

static char config_Panel_IF_Ctrl_10_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x29, 0xff };
static char config_Panel_IF_Ctrl_10a_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0xbf, 0xff };
static char config_Auto_Sequencer_Setting_indx[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x06, 0xff };
static char config_Auto_Sequencer_Setting_c_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x03, 0xff };
static char config_Power_Ctrl_2c_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x10, 0xff };

static char config_VIDEO[4] = { 0x40, 0x00, 0x15, 0x80 };

static char config_Panel_IF_Ctrl_10_indx_off[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4C, 0x00, 0x29, 0xff };

static char config_Panel_IF_Ctrl_10b_cmd_off[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4C, 0x00, 0x02, 0xff };

static char config_Power_Ctrl_1a_cmd[8] =
    { 0x03, 0x00, 0x39, 0xC0, 0x4C, 0x30, 0x00, 0xff };

static struct mipi_dsi_cmd renesas_panel_video_mode_cmds[] = {
	{sizeof(config_sleep_out), config_sleep_out}
	,
	{sizeof(config_CMD_MODE), config_CMD_MODE}
	,
	{sizeof(config_WRTXHT), config_WRTXHT}
	,
	{sizeof(config_WRTXVT), config_WRTXVT}
	,
	{sizeof(config_PLL2NR), config_PLL2NR}
	,
	{sizeof(config_PLL2NF1), config_PLL2NF1}
	,
	{sizeof(config_PLL2NF2), config_PLL2NF2}
	,
	{sizeof(config_PLL2BWADJ1), config_PLL2BWADJ1}
	,
	{sizeof(config_PLL2BWADJ2), config_PLL2BWADJ2}
	,
	{sizeof(config_PLL2CTL), config_PLL2CTL}
	,
	{sizeof(config_DBICBR), config_DBICBR}
	,
	{sizeof(config_DBICTYPE), config_DBICTYPE}
	,
	{sizeof(config_DBICSET1), config_DBICSET1}
	,
	{sizeof(config_DBICADD), config_DBICADD}
	,
	{sizeof(config_DBICCTL), config_DBICCTL}
	,
	{sizeof(config_COLMOD_888), config_COLMOD_888}
	,
	/* Choose config_COLMOD_565 or config_COLMOD_666PACK for other modes */
	{sizeof(config_MADCTL), config_MADCTL}
	,
	{sizeof(config_DBIOC), config_DBIOC}
	,
	{sizeof(config_CASET), config_CASET}
	,
	{sizeof(config_PASET), config_PASET}
	,
	{sizeof(config_TXON), config_TXON}
	,
	{sizeof(config_BLSET_TM), config_BLSET_TM}
	,
	{sizeof(config_AGCPSCTL_TM), config_AGCPSCTL_TM}
	,
	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_1_indx), config_Power_Ctrl_1_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_1a_cmd), config_Power_Ctrl_1a_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_2_indx), config_Power_Ctrl_2_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_2a_cmd), config_Power_Ctrl_2a_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Auto_Sequencer_Setting_indx),
	 config_Auto_Sequencer_Setting_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Auto_Sequencer_Setting_a_cmd),
	 config_Auto_Sequencer_Setting_a_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(Driver_Output_Ctrl_indx), Driver_Output_Ctrl_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(Driver_Output_Ctrl_cmd),
	 Driver_Output_Ctrl_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_LCD_drive_AC_Ctrl_indx),
	 config_LCD_drive_AC_Ctrl_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_LCD_drive_AC_Ctrl_cmd),
	 config_LCD_drive_AC_Ctrl_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Entry_Mode_indx),
	 config_Entry_Mode_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Entry_Mode_cmd),
	 config_Entry_Mode_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Display_Ctrl_1_indx),
	 config_Display_Ctrl_1_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Display_Ctrl_1_cmd),
	 config_Display_Ctrl_1_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Display_Ctrl_2_indx),
	 config_Display_Ctrl_2_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Display_Ctrl_2_cmd),
	 config_Display_Ctrl_2_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Display_Ctrl_3_indx),
	 config_Display_Ctrl_3_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Display_Ctrl_3_cmd),
	 config_Display_Ctrl_3_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Display_IF_Ctrl_1_indx),
	 config_Display_IF_Ctrl_1_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Display_IF_Ctrl_1_cmd),
	 config_Display_IF_Ctrl_1_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Display_IF_Ctrl_2_indx),
	 config_Display_IF_Ctrl_2_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Display_IF_Ctrl_2_cmd),
	 config_Display_IF_Ctrl_2_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_1_indx),
	 config_Panel_IF_Ctrl_1_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_1_cmd),
	 config_Panel_IF_Ctrl_1_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_3_indx),
	 config_Panel_IF_Ctrl_3_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_3_cmd),
	 config_Panel_IF_Ctrl_3_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_4_indx),
	 config_Panel_IF_Ctrl_4_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_4_cmd),
	 config_Panel_IF_Ctrl_4_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_5_indx),
	 config_Panel_IF_Ctrl_5_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_5_cmd),
	 config_Panel_IF_Ctrl_5_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_6_indx),
	 config_Panel_IF_Ctrl_6_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_6_cmd),
	 config_Panel_IF_Ctrl_6_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_8_indx),
	 config_Panel_IF_Ctrl_8_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_8_cmd),
	 config_Panel_IF_Ctrl_8_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_9_indx),
	 config_Panel_IF_Ctrl_9_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_9_cmd),
	 config_Panel_IF_Ctrl_9_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_00_indx),
	 config_gam_adjust_00_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_00_cmd),
	 config_gam_adjust_00_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_01_indx),
	 config_gam_adjust_01_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_01_cmd),
	 config_gam_adjust_01_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_02_indx),
	 config_gam_adjust_02_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_02_cmd),
	 config_gam_adjust_02_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_03_indx),
	 config_gam_adjust_03_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_03_cmd),
	 config_gam_adjust_03_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_04_indx), config_gam_adjust_04_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_04_cmd), config_gam_adjust_04_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_05_indx), config_gam_adjust_05_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_05_cmd), config_gam_adjust_05_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_06_indx), config_gam_adjust_06_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_06_cmd), config_gam_adjust_06_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_07_indx), config_gam_adjust_07_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_07_cmd), config_gam_adjust_07_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_08_indx), config_gam_adjust_08_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_08_cmd), config_gam_adjust_08_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_09_indx), config_gam_adjust_09_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_09_cmd), config_gam_adjust_09_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_0A_indx), config_gam_adjust_0A_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_0A_cmd), config_gam_adjust_0A_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_0B_indx), config_gam_adjust_0B_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_0B_cmd), config_gam_adjust_0B_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_0C_indx), config_gam_adjust_0C_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_0C_cmd), config_gam_adjust_0C_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_0D_indx), config_gam_adjust_0D_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_0D_cmd), config_gam_adjust_0D_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_10_indx), config_gam_adjust_10_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_10_cmd), config_gam_adjust_10_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_11_indx), config_gam_adjust_11_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_11_cmd), config_gam_adjust_11_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_12_indx), config_gam_adjust_12_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_12_cmd), config_gam_adjust_12_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_15_indx), config_gam_adjust_15_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_15_cmd), config_gam_adjust_15_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_16_indx), config_gam_adjust_16_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_16_cmd), config_gam_adjust_16_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_17_indx), config_gam_adjust_17_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_17_cmd), config_gam_adjust_17_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_18_indx), config_gam_adjust_18_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_18_cmd), config_gam_adjust_18_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_19_indx), config_gam_adjust_19_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_19_cmd), config_gam_adjust_19_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_1C_indx), config_gam_adjust_1C_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_1C_cmd), config_gam_adjust_1C_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_1D_indx), config_gam_adjust_1D_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_1D_cmd), config_gam_adjust_1D_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_20_indx), config_gam_adjust_20_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_20_cmd), config_gam_adjust_20_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_21_indx), config_gam_adjust_21_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_21_cmd), config_gam_adjust_21_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_22_indx), config_gam_adjust_22_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_22_cmd), config_gam_adjust_22_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_27_indx), config_gam_adjust_27_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_27_cmd), config_gam_adjust_27_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_28_indx), config_gam_adjust_28_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_28_cmd), config_gam_adjust_28_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_gam_adjust_29_indx), config_gam_adjust_29_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_gam_adjust_29_cmd), config_gam_adjust_29_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_1_indx), config_Power_Ctrl_1_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_1b_cmd), config_Power_Ctrl_1b_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_2_indx), config_Power_Ctrl_2_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_2b_cmd), config_Power_Ctrl_2b_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_3_indx), config_Power_Ctrl_3_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_3a_cmd), config_Power_Ctrl_3a_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_4_indx), config_Power_Ctrl_4_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_4a_cmd), config_Power_Ctrl_4a_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_6_indx), config_Power_Ctrl_6_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_6a_cmd), config_Power_Ctrl_6a_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Auto_Sequencer_Setting_indx),
	 config_Auto_Sequencer_Setting_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Auto_Sequencer_Setting_b_cmd),
	 config_Auto_Sequencer_Setting_b_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Panel_IF_Ctrl_10_indx),
	 config_Panel_IF_Ctrl_10_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Panel_IF_Ctrl_10a_cmd),
	 config_Panel_IF_Ctrl_10a_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Auto_Sequencer_Setting_indx),
	 config_Auto_Sequencer_Setting_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Auto_Sequencer_Setting_c_cmd),
	 config_Auto_Sequencer_Setting_c_cmd}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,

	{sizeof(config_DBICADD70), config_DBICADD70}
	,
	{sizeof(config_Power_Ctrl_2_indx),
	 config_Power_Ctrl_2_indx}
	,
	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_DBICADD72), config_DBICADD72}
	,
	{sizeof(config_Power_Ctrl_2c_cmd),
	 config_Power_Ctrl_2c_cmd}
	,

	{sizeof(config_DBICSET_15), config_DBICSET_15}
	,
	{sizeof(config_VIDEO), config_VIDEO}

};

static struct mipi_dsi_phy_ctrl mipi_dsi_renesas_panel_phy_ctrl = {
	/* DSI_BIT_CLK at 500MHz, 2 lane, RGB888 */
	{0x03, 0x01, 0x01, 0x00},	/* regulator */
	/* timing   */
	{0xb9, 0x8e, 0x1f, 0x00, 0x98, 0x9c, 0x22,
	 0x90, 0x18, 0x03, 0x04},
	{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
	{0xbb, 0x02, 0x06, 0x00},	/* strength */
	/* pll control */
	{0x00, 0xec, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	 0x01, 0x0f, 0x07,	/*  --> Two lane configuration */
	 0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
};

#endif
