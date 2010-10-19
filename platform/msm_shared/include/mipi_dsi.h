/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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

#define DSI_CMD_DMA_MEM_START_ADDR_PANEL      (0x46000000)

#define DSI_CLK_CTRL                          (0x04700118)
#define DSI_TRIG_CTRL                         (0x04700080)
#define DSI_CTRL                              (0x04700000)
#define DSI_COMMAND_MODE_DMA_CTRL             (0x04700038)
#define DSI_DMA_CMD_OFFSET                    (0x04700044)
#define DSI_DMA_CMD_LENGTH                    (0x04700048)
#define DSI_COMMAND_MODE_DMA_CTRL             (0x04700038)
#define DSI_ERR_INT_MASK0                     (0x04700108)
#define DSI_INT_CTRL                          (0x0470010C)

#define DSI_VIDEO_MODE_ACTIVE_H               (0x04700020)
#define DSI_VIDEO_MODE_ACTIVE_V               (0x04700024)
#define DSI_VIDEO_MODE_TOTAL                  (0x04700028)
#define DSI_VIDEO_MODE_HSYNC                  (0x0470002C)
#define DSI_VIDEO_MODE_VSYNC                  (0x04700030)
#define DSI_VIDEO_MODE_VSYNC_VPOS             (0x04700034)

#define DSI_MISR_VIDEO_CTRL                   (0x047000A0)
#define DSI_EOT_PACKET_CTRL                   (0x047000C8)
#define DSI_VIDEO_MODE_CTRL                   (0x0470000C)
#define DSI_CAL_STRENGTH_CTRL                 (0x04700100)
#define DSI_CMD_MODE_DMA_SW_TRIGGER           (0x0470008C)

#define MDP_AXI_RDMASTER_CONFIG               (0x05100028)
#define MDP_AXI_WRMASTER_CONFIG               (0x05100030)
#define MDP_MAX_RD_PENDING_CMD_CONFIG         (0x0510004C)
#define MDP_DISP_INTF_SEL                     (0x05100038)
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
#define MDP_DSI_VIDEO_BORDER_CLR              (0x051E0028)
#define MDP_DSI_VIDEO_HSYNC_SKEW              (0x051E0030)
#define MDP_DSI_VIDEO_CTL_POLARITY            (0x051E0038)
#define MDP_DSI_VIDEO_TEST_CTL                (0x051E0034)

#define MDP_TEST_MODE_CLK                     (0x051F0000)
#define MDP_INTR_STATUS                       (0x05100054)
#define MMSS_SFPB_GPREG                       (0x05700058)

//BEGINNING OF Tochiba Config- video mode

static const unsigned char dsi_toshiba_display_config_MCAP_off[8] = {
    0x02, 0x00, 0x29, 0xc0,
    0xb2, 0x00, 0xff, 0xff
};

static const unsigned char dsi_toshiba_display_config_ena_test_reg[8] = {
    0x03, 0x00, 0x29, 0xc0,
    0xEF, 0x01, 0x01, 0xff
};

static const unsigned char dsi_toshiba_display_config_ena_test_reg_wvga[8] = {
    0x03, 0x00, 0x29, 0xc0,
    0xEF, 0x01, 0x01, 0xff
};

static const unsigned char dsi_toshiba_display_config_num_of_2lane[8] = {
    0x03, 0x00, 0x29, 0xc0,     // 63:2lane
    0xEF, 0x60, 0x63, 0xff
};

static const unsigned char dsi_toshiba_display_config_num_of_1lane[8] = {
    0x03, 0x00, 0x29, 0xc0,     // 62:1lane
    0xEF, 0x60, 0x62, 0xff
};

static const unsigned char dsi_toshiba_display_config_non_burst_sync_pulse[8] = {
    0x03, 0x00, 0x29, 0xc0,
    0xef, 0x61, 0x09, 0xff
};

static const unsigned char dsi_toshiba_display_config_set_DMODE_WQVGA[8] = {
    0x02, 0x00, 0x29, 0xc0,
    0xB3, 0x01, 0xFF, 0xff
};

static const unsigned char dsi_toshiba_display_config_set_DMODE_WVGA[8] = {
    0x02, 0x00, 0x29, 0xc0,
    0xB3, 0x00, 0xFF, 0xff
};

static const unsigned char dsi_toshiba_display_config_set_intern_WR_clk1_wvga[8]
    = {

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x2f, 0xcc, 0xff,
};

static const unsigned char dsi_toshiba_display_config_set_intern_WR_clk2_wvga[8]
    = {

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x6e, 0xdd, 0xff,
};

static const unsigned char
    dsi_toshiba_display_config_set_intern_WR_clk1_wqvga[8] = {

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x2f, 0x22, 0xff,
};

static const unsigned char
    dsi_toshiba_display_config_set_intern_WR_clk2_wqvga[8] = {

    0x03, 0x00, 0x29, 0xC0,     // 1 last packet
    0xef, 0x6e, 0x33, 0xff,
};

static const unsigned char dsi_toshiba_display_config_set_hor_addr_2A_wvga[12] = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    // 0x2A, 0x00, 0x08, 0x00,//100 = 64h
    // 0x6b, 0xFF, 0xFF, 0xFF,
    0x2A, 0x00, 0x00, 0x01,     // 0X1DF = 480-1 0X13F = 320-1
    0xdf, 0xFF, 0xFF, 0xFF,
};

static const unsigned char dsi_toshiba_display_config_set_hor_addr_2B_wvga[12] = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    // 0x2B, 0x00, 0x08, 0x00,//0X355 = 854-1; 0X1DF = 480-1
    // 0x6b, 0xFF, 0xFF, 0xFF,
    0x2B, 0x00, 0x00, 0x03,     // 0X355 = 854-1; 0X1DF = 480-1
    0x55, 0xFF, 0xFF, 0xFF,
};

static const unsigned char dsi_toshiba_display_config_set_hor_addr_2A_wqvga[12]
    = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    0x2A, 0x00, 0x00, 0x00,     // 0XEF = 240-1
    0xef, 0xFF, 0xFF, 0xFF,
};

static const unsigned char dsi_toshiba_display_config_set_hor_addr_2B_wqvga[12]
    = {

    0x05, 0x00, 0x39, 0xC0,     // 1 last packet
    0x2B, 0x00, 0x00, 0x01,     // 0X1aa = 427-1;
    0xaa, 0xFF, 0xFF, 0xFF,
};

static const unsigned char dsi_toshiba_display_config_IFSEL[8] = {
    0x02, 0x00, 0x29, 0xc0,
    0x53, 0x01, 0xff, 0xff
};

static const unsigned char dsi_toshiba_display_config_IFSEL_cmd_mode[8] = {
    0x02, 0x00, 0x29, 0xc0,
    0x53, 0x00, 0xff, 0xff
};

static const unsigned char dsi_toshiba_display_config_exit_sleep[4] = {
    0x11, 0x00, 0x05, 0x80,     // 25 Reg 0x29 < Display On>; generic write 1
                                // params
};

static const unsigned char dsi_toshiba_display_config_display_on[4] = {
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

#endif
