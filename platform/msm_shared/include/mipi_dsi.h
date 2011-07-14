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

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define MIPI_DSI_BASE                         (0x04700000)

#define DSI_CLKOUT_TIMING_CTRL                (0x047000C0)
#define MMSS_DSI_PIXEL_MD                     (0x04000134)
#define MMSS_DSI_PIXEL_NS                     (0x04000138)
#define MMSS_DSI_PIXEL_CC                     (0x04000130)
#define MMSS_DSI_CC                           (0x0400004C)
#define MMSS_DSI_MD                           (0x04000050)
#define MMSS_DSI_NS                           (0x04000054)
#define MMSS_MISC_CC2                         (0x0400005C)
#define MMSS_MISC_CC                          (0x04000058)
#define DSI_PHY_SW_RESET                      (0x04700128)
#define DSI_SOFT_RESET                        (0x04700114)
#define DSI_CAL_CTRL                          (0x047000F4)

#define DSIPHY_REGULATOR_CTRL_0               (0x047002CC)
#define DSIPHY_REGULATOR_CTRL_1               (0x047002D0)
#define DSIPHY_REGULATOR_CTRL_2               (0x047002D4)
#define DSIPHY_REGULATOR_CTRL_3               (0x047002D8)
#define DSIPHY_REGULATOR_CAL_PWR_CFG          (0x04700518)

#define DSIPHY_TIMING_CTRL_0                  (0x04700260)
#define DSIPHY_TIMING_CTRL_1                  (0x04700264)
#define DSIPHY_TIMING_CTRL_2                  (0x04700268)
#define DSIPHY_TIMING_CTRL_3                  (0x0470026C)
#define DSIPHY_TIMING_CTRL_4                  (0x04700270)
#define DSIPHY_TIMING_CTRL_5                  (0x04700274)
#define DSIPHY_TIMING_CTRL_6                  (0x04700278)
#define DSIPHY_TIMING_CTRL_7                  (0x0470027C)
#define DSIPHY_TIMING_CTRL_8                  (0x04700280)
#define DSIPHY_TIMING_CTRL_9                  (0x04700284)
#define DSIPHY_TIMING_CTRL_10                 (0x04700288)

#define DSIPHY_CTRL_0                         (0x04700290)
#define DSIPHY_CTRL_1                         (0x04700294)
#define DSIPHY_CTRL_2                         (0x04700298)
#define DSIPHY_CTRL_3                         (0x0470029C)

#define DSIPHY_STRENGTH_CTRL_0                (0x047002A0)
#define DSIPHY_STRENGTH_CTRL_1                (0x047002A4)
#define DSIPHY_STRENGTH_CTRL_2                (0x047002A8)
#define DSIPHY_STRENGTH_CTRL_3                (0x047002AC)

#define DSIPHY_PLL_CTRL_0                     (0x04700200)
#define DSIPHY_PLL_CTRL_1                     (0x04700204)
#define DSIPHY_PLL_CTRL_2                     (0x04700208)
#define DSIPHY_PLL_CTRL_3                     (0x0470020C)
#define DSIPHY_PLL_CTRL_4                     (0x04700210)
#define DSIPHY_PLL_CTRL_5                     (0x04700214)
#define DSIPHY_PLL_CTRL_6                     (0x04700218)
#define DSIPHY_PLL_CTRL_7                     (0x0470021C)
#define DSIPHY_PLL_CTRL_8                     (0x04700220)
#define DSIPHY_PLL_CTRL_9                     (0x04700224)
#define DSIPHY_PLL_CTRL_10                    (0x04700228)
#define DSIPHY_PLL_CTRL_11                    (0x0470022C)
#define DSIPHY_PLL_CTRL_12                    (0x04700230)
#define DSIPHY_PLL_CTRL_13                    (0x04700234)
#define DSIPHY_PLL_CTRL_14                    (0x04700238)
#define DSIPHY_PLL_CTRL_15                    (0x0470023C)
#define DSIPHY_PLL_CTRL_16                    (0x04700240)
#define DSIPHY_PLL_CTRL_17                    (0x04700244)
#define DSIPHY_PLL_CTRL_18                    (0x04700248)
#define DSIPHY_PLL_CTRL_19                    (0x0470024C)

#define DSIPHY_PLL_RDY                        (0x04700280)

#if DISPLAY_MIPI_PANEL_TOSHIBA_MDT61
#define DSI_CMD_DMA_MEM_START_ADDR_PANEL      (0x90000000)
#else
#define DSI_CMD_DMA_MEM_START_ADDR_PANEL      (0x46000000)
#endif

#define DSI_CLK_CTRL                          (0x04700118)
#define DSI_TRIG_CTRL                         (0x04700080)
#define DSI_CTRL                              (0x04700000)
#define DSI_COMMAND_MODE_DMA_CTRL             (0x04700038)
#define DSI_COMMAND_MODE_MDP_CTRL             (0x0470003C)
#define DSI_COMMAND_MODE_MDP_DCS_CMD_CTRL     (0x04700040)
#define DSI_DMA_CMD_OFFSET                    (0x04700044)
#define DSI_DMA_CMD_LENGTH                    (0x04700048)
#define DSI_COMMAND_MODE_MDP_STREAM0_CTRL     (0x04700054)
#define DSI_COMMAND_MODE_MDP_STREAM0_TOTAL    (0x04700058)
#define DSI_COMMAND_MODE_MDP_STREAM1_CTRL     (0x0470005C)
#define DSI_COMMAND_MODE_MDP_STREAM1_TOTAL    (0x04700060)
#define DSI_ERR_INT_MASK0                     (0x04700108)
#define DSI_INT_CTRL                          (0x0470010C)

#define DSI_VIDEO_MODE_ACTIVE_H               (0x04700020)
#define DSI_VIDEO_MODE_ACTIVE_V               (0x04700024)
#define DSI_VIDEO_MODE_TOTAL                  (0x04700028)
#define DSI_VIDEO_MODE_HSYNC                  (0x0470002C)
#define DSI_VIDEO_MODE_VSYNC                  (0x04700030)
#define DSI_VIDEO_MODE_VSYNC_VPOS             (0x04700034)

#define DSI_MISR_CMD_CTRL                     (0x0470009C)
#define DSI_MISR_VIDEO_CTRL                   (0x047000A0)
#define DSI_EOT_PACKET_CTRL                   (0x047000C8)
#define DSI_VIDEO_MODE_CTRL                   (0x0470000C)
#define DSI_CAL_STRENGTH_CTRL                 (0x04700100)
#define DSI_CMD_MODE_DMA_SW_TRIGGER           (0x0470008C)
#define DSI_CMD_MODE_MDP_SW_TRIGGER           (0x04700090)

#define MDP_OVERLAYPROC0_START                (0x05100004)
#define MDP_DMA_P_START                       (0x0510000C)
#define MDP_DMA_S_START                       (0x05100010)
#define MDP_AXI_RDMASTER_CONFIG               (0x05100028)
#define MDP_AXI_WRMASTER_CONFIG               (0x05100030)
#define MDP_DISP_INTF_SEL                     (0x05100038)
#define MDP_MAX_RD_PENDING_CMD_CONFIG         (0x0510004C)
#define MDP_INTR_ENABLE                       (0x05100050)
#define MDP_DSI_CMD_MODE_ID_MAP               (0x051000A0)
#define MDP_DSI_CMD_MODE_TRIGGER_EN           (0x051000A4)
#define MDP_OVERLAYPROC0_CFG                  (0x05110004)
#define MDP_DMA_P_CONFIG                      (0x05190000)
#define MDP_DMA_P_OUT_XY                      (0x05190010)
#define MDP_DMA_P_SIZE                        (0x05190004)
#define MDP_DMA_P_BUF_ADDR                    (0x05190008)
#define MDP_DMA_P_BUF_Y_STRIDE                (0x0519000C)
#define MDP_DMA_P_OP_MODE                     (0x05190070)
#define MDP_DSI_VIDEO_EN                      (0x051E0000)
#define MDP_DSI_VIDEO_HSYNC_CTL               (0x051E0004)
#define MDP_DSI_VIDEO_VSYNC_PERIOD            (0x051E0008)
#define MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH       (0x051E000C)
#define MDP_DSI_VIDEO_DISPLAY_HCTL            (0x051E0010)
#define MDP_DSI_VIDEO_DISPLAY_V_START         (0x051E0014)
#define MDP_DSI_VIDEO_DISPLAY_V_END           (0x051E0018)
#define MDP_DSI_VIDEO_ACTIVE_HCTL             (0x051E001C)
#define MDP_DSI_VIDEO_BORDER_CLR              (0x051E0028)
#define MDP_DSI_VIDEO_HSYNC_SKEW              (0x051E0030)
#define MDP_DSI_VIDEO_CTL_POLARITY            (0x051E0038)
#define MDP_DSI_VIDEO_TEST_CTL                (0x051E0034)

#define MDP_TEST_MODE_CLK                     (0x051F0000)
#define MDP_INTR_STATUS                       (0x05100054)
#define MMSS_SFPB_GPREG                       (0x05700058)

#define MIPI_DSI_MRPS       0x04 /* Maximum Return Packet Size */
#define MIPI_DSI_REG_LEN    16   /* 4 x 4 bytes register */

#define DTYPE_GEN_WRITE2 0x23 /* 4th Byte is 0x80 */
#define DTYPE_GEN_LWRITE 0x29 /* 4th Byte is 0xc0 */
#define DTYPE_DCS_WRITE1 0x15 /* 4th Byte is 0x80 */

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
    0x03, 0x00, 0x29, 0xc0,     // 63:2lane
    0xEF, 0x60, 0x63, 0xff
};

static const unsigned char toshiba_panel_num_of_1lane[8] = {
    0x03, 0x00, 0x29, 0xc0,     // 62:1lane
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

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x2f, 0xcc, 0xff,
};

static const unsigned char toshiba_panel_set_intern_WR_clk2_wvga[8]
    = {

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x6e, 0xdd, 0xff,
};

static const unsigned char
    toshiba_panel_set_intern_WR_clk1_wqvga[8] = {

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x2f, 0x22, 0xff,
};

static const unsigned char
    toshiba_panel_set_intern_WR_clk2_wqvga[8] = {

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x6e, 0x33, 0xff,
};

static const unsigned char toshiba_panel_set_hor_addr_2A_wvga[12] = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    // 0x2A, 0x00, 0x08, 0x00,//100 = 64h
    // 0x6b, 0xFF, 0xFF, 0xFF,
    0x2A, 0x00, 0x00, 0x01,     // 0X1DF = 480-1 0X13F = 320-1
    0xdf, 0xFF, 0xFF, 0xFF,
};

static const unsigned char toshiba_panel_set_hor_addr_2B_wvga[12] = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    // 0x2B, 0x00, 0x08, 0x00,//0X355 = 854-1; 0X1DF = 480-1
    // 0x6b, 0xFF, 0xFF, 0xFF,
    0x2B, 0x00, 0x00, 0x03,     // 0X355 = 854-1; 0X1DF = 480-1
    0x55, 0xFF, 0xFF, 0xFF,
};

static const unsigned char toshiba_panel_set_hor_addr_2A_wqvga[12]
    = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    0x2A, 0x00, 0x00, 0x00,     // 0XEF = 240-1
    0xef, 0xFF, 0xFF, 0xFF,
};

static const unsigned char toshiba_panel_set_hor_addr_2B_wqvga[12]
    = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    0x2B, 0x00, 0x00, 0x01,     // 0X1aa = 427-1;
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
    0x11, 0x00, 0x05, 0x80,     // 25 Reg 0x29 < Display On>; generic write 1
                                // params
};

static const unsigned char toshiba_panel_display_on[4] = {
    // 0x29, 0x00, 0x05, 0x80,//25 Reg 0x29 < Display On>; generic write 1
    // params
    0x29, 0x00, 0x05, 0x80,     // 25 Reg 0x29 < Display On>; generic write 1
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
static char novatek_panel_sw_reset[4] = {0x01, 0x00, 0x05, 0x00}; /* DTYPE_DCS_WRITE */
static char novatek_panel_enter_sleep[4] = {0x10, 0x00, 0x05, 0x80}; /* DTYPE_DCS_WRITE */
static char novatek_panel_exit_sleep[4] = {0x11, 0x00, 0x05, 0x80}; /* DTYPE_DCS_WRITE */
static char novatek_panel_display_off[4] = {0x28, 0x00, 0x05, 0x80}; /* DTYPE_DCS_WRITE */
static char novatek_panel_display_on[4] = {0x29, 0x00, 0x05, 0x80}; /* DTYPE_DCS_WRITE */
static char novatek_panel_max_packet[4] = {0x04, 0x00, 0x37, 0x80}; /* DTYPE_SET_MAX_PACKET */

static char novatek_panel_set_onelane[4] = {0xae, 0x01, 0x15, 0x80}; /* DTYPE_DCS_WRITE1 */
static char novatek_panel_rgb_888[4] = {0x3A, 0x77, 0x15, 0x80}; /* DTYPE_DCS_WRITE1 */
static char novatek_panel_set_twolane[4] = {0xae, 0x03, 0x15, 0x80}; /* DTYPE_DCS_WRITE1 */

static char novatek_panel_manufacture_id[4] = {0x04, 0x00, 0x06, 0xA0}; /* DTYPE_DCS_READ */

/* commands by Novatke */
static char novatek_panel_f4[4] = {0xf4, 0x55, 0x15, 0x80}; /* DTYPE_DCS_WRITE1 */
static char novatek_panel_8c[20] = { /* DTYPE_DCS_LWRITE */
  0x10, 0x00, 0x39, 0xC0, 0x8C, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x08, 0x08, 0x00, 0x30, 0xC0, 0xB7, 0x37};
static char novatek_panel_ff[4] = {0xff, 0x55, 0x15, 0x80}; /* DTYPE_DCS_WRITE1 */

static char novatek_panel_set_width[12] = { /* DTYPE_DCS_LWRITE */
  0x05, 0x00, 0x39, 0xC0,//1 last packet
  0x2A, 0x00, 0x00, 0x02,//clmn:0 - 0x21B=539
  0x1B, 0xFF, 0xFF, 0xFF
}; /* 540 - 1 */
static char novatek_panel_set_height[12] = { /* DTYPE_DCS_LWRITE */
  0x05, 0x00, 0x39, 0xC0,//1 last packet
  0x2B, 0x00, 0x00, 0x03,//row:0 - 0x3BF=959
  0xBF, 0xFF, 0xFF, 0xFF,
}; /* 960 - 1 */

/* Commands to control Backlight */
static char novatek_panel_set_led_pwm1[8] = { /* DTYPE_DCS_LWRITE */
  0x02, 0x00, 0x39, 0xC0,//1 last packet
  0x51, 0xFA, 0xFF, 0xFF, // Brightness level set to 0xFA -> 250
};
static char novatek_panel_set_led_pwm2[8] = { /* DTYPE_DCS_LWRITE */
  0x02, 0x00, 0x39, 0xC0,
  0x53, 0x24, 0xFF, 0xFF,
};
static char novatek_panel_set_led_pwm3[8] = { /* DTYPE_DCS_LWRITE */
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

static const unsigned char dsi_display_exit_sleep[4] =
{
0x11, 0x00, 0x15, 0x80,
};

static const unsigned char dsi_display_display_on[4] =
{
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
};

static struct mipi_dsi_cmd toshiba_panel_video_mode_cmds[] = {
    {sizeof(toshiba_panel_mcap_off), toshiba_panel_mcap_off},
    {sizeof(toshiba_panel_ena_test_reg), toshiba_panel_ena_test_reg},
    {sizeof(toshiba_panel_num_of_1lane), toshiba_panel_num_of_1lane},
    {sizeof(toshiba_panel_non_burst_sync_pulse), toshiba_panel_non_burst_sync_pulse},
    {sizeof(toshiba_panel_set_DMODE_WVGA), toshiba_panel_set_DMODE_WVGA},
    {sizeof(toshiba_panel_set_intern_WR_clk1_wvga), toshiba_panel_set_intern_WR_clk1_wvga},
    {sizeof(toshiba_panel_set_intern_WR_clk2_wvga), toshiba_panel_set_intern_WR_clk2_wvga},
    {sizeof(toshiba_panel_set_hor_addr_2A_wvga), toshiba_panel_set_hor_addr_2A_wvga},
    {sizeof(toshiba_panel_set_hor_addr_2B_wvga), toshiba_panel_set_hor_addr_2B_wvga},
    {sizeof(toshiba_panel_IFSEL), toshiba_panel_IFSEL},
    {sizeof(toshiba_panel_exit_sleep), toshiba_panel_exit_sleep},
    {sizeof(toshiba_panel_display_on), toshiba_panel_display_on},
    {sizeof(dsi_display_config_color_mode_on), dsi_display_config_color_mode_on},
    {sizeof(dsi_display_config_color_mode_off), dsi_display_config_color_mode_off},
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
		0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03 },
#elif defined(DSI_BIT_CLK_380MHZ)
		{0x41, 0xf7, 0xb2, 0xf5, 0x00, 0x50, 0x48, 0x63,
		0x31, 0x0f, 0x07,
		0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03 },
#elif defined(DSI_BIT_CLK_400MHZ)
		{0x41, 0x8f, 0xb1, 0xda, 0x00, 0x50, 0x48, 0x63,
		0x31, 0x0f, 0x07,
		0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03 },
#else		/* 200 mhz */
		{0x41, 0x8f, 0xb1, 0xda, 0x00, 0x50, 0x48, 0x63,
		 0x33, 0x1f, 0x1f /* for 1 lane ; 0x0f for 2 lanes*/,
		0x05, 0x14, 0x03, 0x03, 0x03, 0x54, 0x06, 0x10, 0x04, 0x03 },
#endif
};


static struct mipi_dsi_cmd toshiba_mdt61_video_mode_cmds[] = {
	{sizeof(toshiba_mdt61_mcap_start), toshiba_mdt61_mcap_start},
	{sizeof(toshiba_mdt61_num_out_pixelform),toshiba_mdt61_num_out_pixelform},
	{sizeof(toshiba_mdt61_dsi_ctrl), toshiba_mdt61_dsi_ctrl},
	{sizeof(toshiba_mdt61_panel_driving), toshiba_mdt61_panel_driving},
	{sizeof(toshiba_mdt61_dispV_timing), toshiba_mdt61_dispV_timing},
	{sizeof(toshiba_mdt61_dispCtrl), toshiba_mdt61_dispCtrl},
	{sizeof(toshiba_mdt61_test_mode_c4), toshiba_mdt61_test_mode_c4},
	{sizeof(toshiba_mdt61_dispH_timing), toshiba_mdt61_dispH_timing},
	{sizeof(toshiba_mdt61_test_mode_c6), toshiba_mdt61_test_mode_c6},
	{sizeof(toshiba_mdt61_gamma_setA), toshiba_mdt61_gamma_setA},
	{sizeof(toshiba_mdt61_gamma_setB), toshiba_mdt61_gamma_setB},
	{sizeof(toshiba_mdt61_gamma_setC), toshiba_mdt61_gamma_setC},
	{sizeof(toshiba_mdt61_powerSet_ChrgPmp),toshiba_mdt61_powerSet_ChrgPmp},
	{sizeof(toshiba_mdt61_testMode_d1), toshiba_mdt61_testMode_d1},
	{sizeof(toshiba_mdt61_powerSet_SrcAmp),toshiba_mdt61_powerSet_SrcAmp},
	{sizeof(toshiba_mdt61_powerInt_PS), toshiba_mdt61_powerInt_PS},
	{sizeof(toshiba_mdt61_vreg), toshiba_mdt61_vreg},
	{sizeof(toshiba_mdt61_test_mode_d6), toshiba_mdt61_test_mode_d6},
	{sizeof(toshiba_mdt61_timingCtrl_d7), toshiba_mdt61_timingCtrl_d7},
	{sizeof(toshiba_mdt61_timingCtrl_d8), toshiba_mdt61_timingCtrl_d8},
	{sizeof(toshiba_mdt61_timingCtrl_d9), toshiba_mdt61_timingCtrl_d9},
	{sizeof(toshiba_mdt61_white_balance), toshiba_mdt61_white_balance},
	{sizeof(toshiba_mdt61_vcs_settings), toshiba_mdt61_vcs_settings},
	{sizeof(toshiba_mdt61_vcom_dc_settings), toshiba_mdt61_vcom_dc_settings},
	{sizeof(toshiba_mdt61_testMode_e3), toshiba_mdt61_testMode_e3},
	{sizeof(toshiba_mdt61_testMode_e4), toshiba_mdt61_testMode_e4},
	{sizeof(toshiba_mdt61_testMode_e5), toshiba_mdt61_testMode_e5},
	{sizeof(toshiba_mdt61_testMode_fa), toshiba_mdt61_testMode_fa},
	{sizeof(toshiba_mdt61_testMode_fd), toshiba_mdt61_testMode_fd},
	{sizeof(toshiba_mdt61_testMode_fe), toshiba_mdt61_testMode_fe},
	{sizeof(toshiba_mdt61_mcap_end), toshiba_mdt61_mcap_end},
	{sizeof(toshiba_mdt61_set_add_mode), toshiba_mdt61_set_add_mode},
	{sizeof(toshiba_mdt61_set_pixel_format), toshiba_mdt61_set_pixel_format},
	{sizeof(dsi_display_exit_sleep), dsi_display_exit_sleep},
	{sizeof(dsi_display_display_on), dsi_display_display_on},
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
		 0x00, 0x1a, 0x00, 0x00, 0x02, 0x00, 0x20, 0x00, 0x01, 0x00 },
};

static struct mipi_dsi_cmd novatek_panel_manufacture_id_cmd =
    {sizeof(novatek_panel_manufacture_id), novatek_panel_manufacture_id};

static struct mipi_dsi_cmd novatek_panel_cmd_mode_cmds[] = {
    {sizeof(novatek_panel_sw_reset), novatek_panel_sw_reset},
    {sizeof(novatek_panel_exit_sleep), novatek_panel_exit_sleep},
    {sizeof(novatek_panel_display_on), novatek_panel_display_on},
    {sizeof(novatek_panel_max_packet), novatek_panel_max_packet},
    {sizeof(novatek_panel_f4), novatek_panel_f4},
    {sizeof(novatek_panel_8c), novatek_panel_8c},
    {sizeof(novatek_panel_ff), novatek_panel_ff},
    {sizeof(novatek_panel_set_twolane), novatek_panel_set_twolane},
    {sizeof(novatek_panel_set_width), novatek_panel_set_width},
    {sizeof(novatek_panel_set_height), novatek_panel_set_height},
    {sizeof(novatek_panel_rgb_888), novatek_panel_rgb_888},
    {sizeof(novatek_panel_set_led_pwm1), novatek_panel_set_led_pwm1},
    {sizeof(novatek_panel_set_led_pwm2), novatek_panel_set_led_pwm2},
    {sizeof(novatek_panel_set_led_pwm3), novatek_panel_set_led_pwm3}
};

static struct mipi_dsi_phy_ctrl mipi_dsi_novatek_panel_phy_ctrl = {
  /* DSI_BIT_CLK at 500MHz, 2 lane, RGB888 */
  {0x03, 0x01, 0x01, 0x00},       /* regulator */
  /* timing   */
  {0x96, 0x26, 0x23, 0x00, 0x50, 0x4B, 0x1e,
   0x28, 0x28, 0x03, 0x04},
  {0x7f, 0x00, 0x00, 0x00},       /* phy ctrl */
  {0xee, 0x02, 0x86, 0x00},       /* strength */
  /* pll control */
  {0x40, 0xf9, 0xb0, 0xda, 0x00, 0x50, 0x48, 0x63,
   /* 0x30, 0x07, 0x07,  --> One lane configuration */
   0x30, 0x07, 0x03, /*  --> Two lane configuration */
   0x05, 0x14, 0x03, 0x0, 0x0, 0x54, 0x06, 0x10, 0x04, 0x0},
};

#endif
