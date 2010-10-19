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

#include <reg.h>
#include <mipi_dsi.h>
#include <dev/fbcon.h>
#include <target/display.h>

#define MIPI_FB_ADDR  0x43E00000

static struct fbcon_config mipi_fb_cfg = {
    .height = TSH_MIPI_FB_HEIGHT,
    .width = TSH_MIPI_FB_WIDTH,
    .stride = TSH_MIPI_FB_WIDTH,
    .format = FB_FORMAT_RGB888,
    .bpp = 24,
    .update_start = NULL,
    .update_done = NULL,
};

void configure_dsicore_dsiclk()
{
    unsigned char mnd_mode, root_en, clk_en;
    unsigned long src_sel = 0x3;    //dsi_phy_pll0_src
    unsigned long pre_div_func = 0x00;  // predivide by 1
    unsigned long pmxo_sel;

    writel(pre_div_func << 14 | src_sel, MMSS_DSI_NS);
    mnd_mode = 0;               //Bypass MND
    root_en = 1;
    clk_en = 1;
    pmxo_sel = 0;

    writel((pmxo_sel << 8) | (mnd_mode << 6), MMSS_DSI_CC);
    writel(readl(MMSS_DSI_CC) | root_en << 2, MMSS_DSI_CC);
    writel(readl(MMSS_DSI_CC) | clk_en, MMSS_DSI_CC);
}

void configure_dsicore_byteclk(void)
{
    writel(0x00400401, MMSS_MISC_CC2);  // select pxo
}

void configure_dsicore_pclk(void)
{

    unsigned char mnd_mode, root_en, clk_en;
    unsigned long src_sel = 0x3;    // dsi_phy_pll0_src
    unsigned long pre_div_func = 0x01;  // predivide by 2

    writel(pre_div_func << 12 | src_sel, MMSS_DSI_PIXEL_NS);

    mnd_mode = 0;              // Bypass MND
    root_en = 1;
    clk_en = 1;
    writel(mnd_mode << 6, MMSS_DSI_PIXEL_CC);
    writel(readl(MMSS_DSI_PIXEL_CC) | root_en << 2, MMSS_DSI_PIXEL_CC);
    writel(readl(MMSS_DSI_PIXEL_CC) | clk_en, MMSS_DSI_PIXEL_CC);

}

int dsi_dsiphy_reg_bitclk_200MHz_toshiba_rgb888(unsigned char lane_num_hs)
{

    unsigned char lane_1 = 1;
    unsigned char lane_2 = 2;

    writel(0x00000001, DSI_PHY_SW_RESET);
    mdelay(100);
    writel(0x00000000, DSI_PHY_SW_RESET);

    writel(0x00000003, DSIPHY_REGULATOR_CTRL_0);
    writel(0x00000001, DSIPHY_REGULATOR_CTRL_1);
    writel(0x00000001, DSIPHY_REGULATOR_CTRL_2);
    writel(0x00000000, DSIPHY_REGULATOR_CTRL_3);

    writel(0x50, DSIPHY_TIMING_CTRL_0);
    writel(0x0f, DSIPHY_TIMING_CTRL_1);
    writel(0x14, DSIPHY_TIMING_CTRL_2);
    writel(0x19, DSIPHY_TIMING_CTRL_4);
    writel(0x23, DSIPHY_TIMING_CTRL_5);
    writel(0x0e, DSIPHY_TIMING_CTRL_6);
    writel(0x12, DSIPHY_TIMING_CTRL_7);
    writel(0x16, DSIPHY_TIMING_CTRL_8);
    writel(0x1b, DSIPHY_TIMING_CTRL_9);
    writel(0x1c, DSIPHY_TIMING_CTRL_10);

    // T_CLK_POST, T_CLK_PRE for CLK lane P/N HS 200 mV timing length should >
    // data lane HS timing length
    writel(0x90f, DSI_CLKOUT_TIMING_CTRL);

    writel(0x7f, DSIPHY_CTRL_0);
    writel(0x00, DSIPHY_CTRL_1);
    writel(0x00, DSIPHY_CTRL_2);
    writel(0x00, DSIPHY_CTRL_3);

    writel(0xEE, DSIPHY_STRENGTH_CTRL_0);
    writel(0x86, DSIPHY_STRENGTH_CTRL_0);

    writel(0x8f, DSIPHY_PLL_CTRL_1);    // vco=400*2=800Mhz

    writel(0xb1, DSIPHY_PLL_CTRL_2);
    writel(0xda, DSIPHY_PLL_CTRL_3);
    writel(0x00, DSIPHY_PLL_CTRL_4);
    writel(0x50, DSIPHY_PLL_CTRL_5);
    writel(0x48, DSIPHY_PLL_CTRL_6);
    writel(0x63, DSIPHY_PLL_CTRL_7);

    writel(0x33, DSIPHY_PLL_CTRL_8);    // bit clk 800/4=200mhz
    writel(0x1f, DSIPHY_PLL_CTRL_9);    // byte clk 800/32=25mhz (200/8=25)

    if (lane_num_hs == lane_1) {
        printf("\nData Lane: 1 lane");
        writel(0x1f, DSIPHY_PLL_CTRL_10);   // 1 lane dsi clk 800/32=25mhz
    } else if (lane_num_hs == lane_2) {
        printf("\nData Lane: 2 lane");
        writel(0x0f, DSIPHY_PLL_CTRL_10);   // 2 lane dsi clk 800/16=50mhz
    }

    writel(0x05, DSIPHY_PLL_CTRL_11);
    writel(0x14, DSIPHY_PLL_CTRL_12);
    writel(0x03, DSIPHY_PLL_CTRL_13);
    writel(0x54, DSIPHY_PLL_CTRL_16);
    writel(0x06, DSIPHY_PLL_CTRL_17);
    writel(0x10, DSIPHY_PLL_CTRL_18);
    writel(0x04, DSIPHY_PLL_CTRL_19);
    writel(0x00000040, DSIPHY_PLL_CTRL_0);
    writel(0x00000041, DSIPHY_PLL_CTRL_0);  // dsipll en

    return (0);
}

int dsi_cmd_dma_trigger_for_panel()
{
    unsigned long ReadValue;
    unsigned long count = 0;
    int status = 0;

    writel(0x03030303, DSI_INT_CTRL);
    mdelay(1);
    writel(0x1, DSI_CMD_MODE_DMA_SW_TRIGGER);
    ReadValue = readl(DSI_INT_CTRL) & 0x00000001;
    while (ReadValue != 0x00000001) {
        ReadValue = readl(DSI_INT_CTRL) & 0x00000001;
        count++;
        if (count > 0xffff) {
            status = FAIL;
            printf("\n\nThis command mode dma test is failed");
            return status;
        }
    }

    writel((readl(DSI_INT_CTRL) | 0x01000001), DSI_INT_CTRL);
    printf
        ("\n\nThis command mode is tested successfully, continue on next command mode test");
    return status;
}

int dsi_toshiba_panel_config_video_mode_wvga(unsigned char lane_num)
{

    unsigned char DMA_STREAM1 = 0;  // for mdp display processor path
    unsigned char EMBED_MODE1 = 1;  // from frame buffer
    unsigned char POWER_MODE2 = 1;  // from frame buffer
    unsigned char PACK_TYPE1 = 1;   // long packet
    unsigned char VC1 = 0;
    unsigned char DT1 = 0;      // non embedded mode
    unsigned short WC1 = 0;     // for non embedded mode only
    int status = 0;
    unsigned char DLNx_EN = 1;
    unsigned char lane_1 = 1;
    unsigned char lane_2 = 2;

    writel((0 << 16) | 0x3f, DSI_CLK_CTRL); // reg:0x118
    writel(DMA_STREAM1 << 8 | 0x04, DSI_TRIG_CTRL); // reg 0x80 dma trigger: sw
                                                    // trigger 0x4; dma stream1
    writel(0 << 30 | DLNx_EN << 4 | 0x105, DSI_CTRL);   // reg 0x00 for this
                                                        // build
    writel(EMBED_MODE1 << 28 | POWER_MODE2 << 26
           | PACK_TYPE1 << 24 | VC1 << 22 | DT1 << 16 | WC1,
           DSI_COMMAND_MODE_DMA_CTRL);
    writel(0x15000000, DSI_COMMAND_MODE_DMA_CTRL);  // reg 0x38 wc=4; DT=09;
                                                    // embedded mode=0 from the
                                                    // reg.

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_MCAP_off, 8);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_ena_test_reg, 8);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    if (lane_num == lane_1) {
        memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
               &dsi_toshiba_display_config_num_of_1lane, 8);
    } else if (lane_num == lane_2) {

        memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
               &dsi_toshiba_display_config_num_of_2lane, 8);
    }

    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_non_burst_sync_pulse, 8);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_set_DMODE_WVGA, 8);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_set_intern_WR_clk1_wvga, 8);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_set_intern_WR_clk2_wvga, 8);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_set_hor_addr_2A_wvga, 12);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(12, DSI_DMA_CMD_LENGTH); // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_set_hor_addr_2B_wvga, 12);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(12, DSI_DMA_CMD_LENGTH); // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL, &dsi_toshiba_display_config_IFSEL,
           8);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(8, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_exit_sleep, 4);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(4, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    writel(0x14000000, DSI_COMMAND_MODE_DMA_CTRL);
    status += dsi_cmd_dma_trigger_for_panel();

    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL,
           &dsi_toshiba_display_config_display_on, 4);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(4, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    // dsi_display_config_color_mode_on - low power
    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL, &dsi_display_config_color_mode_on,
           4);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(4, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    // dsi_display_config_color_mode_off - back to normal
    memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL, &dsi_display_config_color_mode_off,
           4);
    writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
    writel(4, DSI_DMA_CMD_LENGTH);  // reg 0x48 for this build
    status += dsi_cmd_dma_trigger_for_panel();

    writel(0x0000, DSI_CTRL);
    writel(0x0001, DSI_SOFT_RESET);
    writel(0x0000, DSI_SOFT_RESET);

    return status;
}

int config_dsi_video_mode(unsigned short disp_width, unsigned short disp_height,
                          unsigned short img_width, unsigned short img_height,
                          unsigned short hsync_porch0_fp,
                          unsigned short hsync_porch0_bp,
                          unsigned short vsync_porch0_fp,
                          unsigned short vsync_porch0_bp,
                          unsigned short hsync_width,
                          unsigned short vsync_width, unsigned short dst_format,
                          unsigned short traffic_mode,
                          unsigned short datalane_num)
{

    unsigned char DST_FORMAT;
    unsigned char TRAFIC_MODE;
    unsigned char DLNx_EN;
    // video mode data ctrl
    int status = 0;
    unsigned long low_pwr_stop_mode = 0;
    unsigned char eof_bllp_pwr = 0x9;
    unsigned char interleav = 0;

    // disable mdp first
    writel(0x00000000, MDP_DSI_VIDEO_EN);

    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000002, DSI_CLK_CTRL);
    writel(0x00000006, DSI_CLK_CTRL);
    writel(0x0000000e, DSI_CLK_CTRL);
    writel(0x0000001e, DSI_CLK_CTRL);
    writel(0x0000003e, DSI_CLK_CTRL);

    writel(0, DSI_CTRL);

    writel(0, DSI_ERR_INT_MASK0);

    DST_FORMAT = 0;             // RGB565
    printf("\nDSI_Video_Mode - Dst Format: RGB565");

    DLNx_EN = 1;                // 1 lane with clk programming
    printf("\nData Lane: 1 lane\n");

    TRAFIC_MODE = 0;            // non burst mode with sync pulses
    printf("\nTraffic mode: non burst mode with sync pulses\n");

    writel(0x02020202, DSI_INT_CTRL);

    writel(((img_width + hsync_porch0_bp) << 16) | hsync_porch0_bp,
           DSI_VIDEO_MODE_ACTIVE_H);

    writel(((img_height + vsync_porch0_bp) << 16) | (vsync_porch0_bp),
           DSI_VIDEO_MODE_ACTIVE_V);

    writel(((img_height + vsync_porch0_fp + vsync_porch0_bp) << 16)
           | img_width + hsync_porch0_fp + hsync_porch0_bp,
           DSI_VIDEO_MODE_TOTAL);

    writel((hsync_width << 16) | 0, DSI_VIDEO_MODE_HSYNC);

    writel(0 << 16 | 0, DSI_VIDEO_MODE_VSYNC);

    writel(vsync_width << 16 | 0, DSI_VIDEO_MODE_VSYNC_VPOS);

    writel(1, DSI_EOT_PACKET_CTRL);

    writel(0x00000100, DSI_MISR_VIDEO_CTRL);

    writel(low_pwr_stop_mode << 16 | eof_bllp_pwr << 12 | TRAFIC_MODE << 8
           | DST_FORMAT << 4 | 0x0, DSI_VIDEO_MODE_CTRL);

    writel(0x67, DSI_CAL_STRENGTH_CTRL);

    writel(0x80006711, DSI_CAL_CTRL);

    writel(0x00010100, DSI_MISR_VIDEO_CTRL);

    writel(0x00010100, DSI_INT_CTRL);
    writel(0x02010202, DSI_INT_CTRL);

    writel(0x02030303, DSI_INT_CTRL);

    writel(interleav << 30 | 0 << 24 | 0 << 20 | DLNx_EN << 4
           | 0x103, DSI_CTRL);
    mdelay(1);

    return status;
}

int mdp_setup_dma_p_video_mode(unsigned short disp_width,
                               unsigned short disp_height,
                               unsigned short img_width,
                               unsigned short img_height,
                               unsigned short hsync_porch0_fp,
                               unsigned short hsync_porch0_bp,
                               unsigned short vsync_porch0_fp,
                               unsigned short vsync_porch0_bp,
                               unsigned short hsync_width,
                               unsigned short vsync_width,
                               unsigned long input_img_addr,
                               unsigned short img_width_full_size,
                               unsigned short pack_pattern,
                               unsigned char ystride)
{

    // unsigned long mdp_intr_status;
    int status = FAIL;
    unsigned long hsync_period;
    unsigned long vsync_period;
    unsigned long vsync_period_intmd;

    printf("\nHi setup MDP4.1 for DSI Video Mode\n");

    hsync_period = img_width + hsync_porch0_fp + hsync_porch0_bp + 1;
    vsync_period_intmd = img_height + vsync_porch0_fp + vsync_porch0_bp + 1;
    vsync_period = vsync_period_intmd * hsync_period;

    // ----- programming MDP_AXI_RDMASTER_CONFIG --------
    /* MDP_AXI_RDMASTER_CONFIG set all master to read from AXI port 0, that's
       the only port connected */
    writel(0x00290000, MDP_AXI_RDMASTER_CONFIG);
    writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
    writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);
    writel(0x00000049, MDP_DISP_INTF_SEL);
    writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

    // ------------- programming MDP_DMA_P_CONFIG ---------------------
    writel(pack_pattern << 8 | 0xbf | (0 << 25), MDP_DMA_P_CONFIG); // rgb888

    writel(0x00000000, MDP_DMA_P_OUT_XY);
    writel(img_height << 16 | img_width, MDP_DMA_P_SIZE);
    writel(input_img_addr, MDP_DMA_P_BUF_ADDR);
    writel(img_width_full_size * ystride, MDP_DMA_P_BUF_Y_STRIDE);
    writel(0x00ff0000, MDP_DMA_P_OP_MODE);
    writel(hsync_period << 16 | hsync_width, MDP_DSI_VIDEO_HSYNC_CTL);
    writel(vsync_period, MDP_DSI_VIDEO_VSYNC_PERIOD);
    writel(vsync_width * hsync_period, MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH);
    writel((img_width + hsync_porch0_bp - 1) << 16 | hsync_porch0_bp,
           MDP_DSI_VIDEO_DISPLAY_HCTL);
    writel(vsync_porch0_bp * hsync_period, MDP_DSI_VIDEO_DISPLAY_V_START);
    writel((img_height + vsync_porch0_bp) * hsync_period,
           MDP_DSI_VIDEO_DISPLAY_V_END);
    writel(0x00ABCDEF, MDP_DSI_VIDEO_BORDER_CLR);
    writel(0x00000000, MDP_DSI_VIDEO_HSYNC_SKEW);
    writel(0x00000000, MDP_DSI_VIDEO_CTL_POLARITY);
    // end of cmd mdp

    writel(0x00000001, MDP_DSI_VIDEO_EN);   // MDP_DSI_EN ENABLE

    status = PASS;
    return status;
}

int mipi_dsi_config(unsigned short num_of_lanes)
{

    int status = 0;
    unsigned long ReadValue;
    unsigned long count = 0;
    unsigned long low_pwr_stop_mode = 0;    // low power mode 0x1111 start from
                                            // bit16, high spd mode 0x0
    unsigned char eof_bllp_pwr = 0x9;   // bit 12, 15, 1:low power stop mode or
                                        // let cmd mode eng send packets in hs
                                        // or lp mode
    unsigned short display_wd = mipi_fb_cfg.width;
    unsigned short display_ht = mipi_fb_cfg.height;
    unsigned short image_wd = mipi_fb_cfg.width;
    unsigned short image_ht = mipi_fb_cfg.height;
    unsigned short hsync_porch_fp = MIPI_HSYNC_FRONT_PORCH_DCLK;
    unsigned short hsync_porch_bp = MIPI_HSYNC_BACK_PORCH_DCLK;
    unsigned short vsync_porch_fp = MIPI_VSYNC_FRONT_PORCH_LINES;
    unsigned short vsync_porch_bp = MIPI_VSYNC_BACK_PORCH_LINES;
    unsigned short hsync_width = MIPI_HSYNC_PULSE_WIDTH;
    unsigned short vsync_width = MIPI_VSYNC_PULSE_WIDTH;
    unsigned short dst_format = 0;
    unsigned short traffic_mode = 0;
    unsigned short pack_pattern = 0x12;
    unsigned char ystride = 3;

    low_pwr_stop_mode = 0x1111; // low pwr mode bit16:HSA, bit20:HBA,
                                // bit24:HFP, bit28:PULSE MODE, need enough
                                // time for swithc from LP to HS
    eof_bllp_pwr = 0x9;         // low power stop mode or let cmd mode eng send
                                // packets in hs or lp mode

    status += config_dsi_video_mode(display_wd, display_ht, image_wd, image_ht,
                                    hsync_porch_fp, hsync_porch_bp,
                                    vsync_porch_fp, vsync_porch_bp, hsync_width,
                                    vsync_width, dst_format, traffic_mode,
                                    num_of_lanes);

    status +=
        mdp_setup_dma_p_video_mode(display_wd, display_ht, image_wd, image_ht,
                                   hsync_porch_fp, hsync_porch_bp,
                                   vsync_porch_fp, vsync_porch_bp, hsync_width,
                                   vsync_width, MIPI_FB_ADDR, image_wd,
                                   pack_pattern, ystride);

    ReadValue = readl(DSI_INT_CTRL) & 0x00010000;
    while (ReadValue != 0x00010000) {
        ReadValue = readl(DSI_INT_CTRL) & 0x00010000;
        count++;
        if (count > 0xffff) {
            status = FAIL;
            printf("\nToshiba Video 565 pulse 1 lane test is failed\n");
            return status;
        }
    }

    printf("\nToshiba Video 565 pulse 1 lane is tested successfully \n");
    return status;
}

void mipi_dsi_shutdown(void)
{
    writel(0, DSI_CTRL);
    writel(0x00000001, DSI_PHY_SW_RESET);
    writel(0x0, DSI_INT_CTRL);
    writel(0x00000000, MDP_DSI_VIDEO_EN);
}

struct fbcon_config *mipi_init(void)
{
    int status = 0;
    unsigned char num_of_lanes = 1;
    writel(0x00001800, MMSS_SFPB_GPREG);
    configure_dsicore_dsiclk();
    configure_dsicore_byteclk();
    configure_dsicore_pclk();
    dsi_dsiphy_reg_bitclk_200MHz_toshiba_rgb888(num_of_lanes);
    status += dsi_toshiba_panel_config_video_mode_wvga(num_of_lanes);
    mipi_fb_cfg.base = MIPI_FB_ADDR;

    status += mipi_dsi_config(num_of_lanes);
    return &mipi_fb_cfg;
}
