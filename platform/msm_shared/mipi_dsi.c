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

#if DISPLAY_MIPI_PANEL_TOSHIBA
static struct fbcon_config mipi_fb_cfg = {
    .height = TSH_MIPI_FB_HEIGHT,
    .width = TSH_MIPI_FB_WIDTH,
    .stride = TSH_MIPI_FB_WIDTH,
    .format = FB_FORMAT_RGB888,
    .bpp = 24,
    .update_start = NULL,
    .update_done = NULL,
};
#elif DISPLAY_MIPI_PANEL_NOVATEK_BLUE
static struct fbcon_config mipi_fb_cfg = {
    .height = NOV_MIPI_FB_HEIGHT,
    .width = NOV_MIPI_FB_WIDTH,
    .stride = NOV_MIPI_FB_WIDTH,
    .format = FB_FORMAT_RGB888,
    .bpp = 24,
    .update_start = NULL,
    .update_done = NULL,
};
#else
static struct fbcon_config mipi_fb_cfg = {
    .height = 0,
    .width = 0,
    .stride = 0,
    .format = 0,
    .bpp = 0,
    .update_start = NULL,
    .update_done = NULL,
};
#endif

static int cmd_mode_status = 0;

void configure_dsicore_dsiclk()
{
    unsigned char mnd_mode, root_en, clk_en;
    unsigned long src_sel = 0x3;    // dsi_phy_pll0_src
    unsigned long pre_div_func = 0x00;  // predivide by 1
    unsigned long pmxo_sel;

    writel(pre_div_func << 14 | src_sel, MMSS_DSI_NS);
    mnd_mode = 0;               // Bypass MND
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

    mnd_mode = 0;               // Bypass MND
    root_en = 1;
    clk_en = 1;
    writel(mnd_mode << 6, MMSS_DSI_PIXEL_CC);
    writel(readl(MMSS_DSI_PIXEL_CC) | root_en << 2, MMSS_DSI_PIXEL_CC);
    writel(readl(MMSS_DSI_PIXEL_CC) | clk_en, MMSS_DSI_PIXEL_CC);
}

int mipi_dsi_phy_ctrl_config(struct mipi_dsi_panel_config *pinfo)
{

    unsigned char lane_1 = 1;
    unsigned char lane_2 = 2;
    unsigned i;
    unsigned off = 0;
    struct mipi_dsi_phy_ctrl *pd;

    writel(0x00000001, DSI_PHY_SW_RESET);
    mdelay(50);
    writel(0x00000000, DSI_PHY_SW_RESET);

    pd = (pinfo->dsi_phy_config);

    off = 0x02cc;               /* regulator ctrl 0 */
    for (i = 0; i < 4; i++) {
        writel(pd->regulator[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    off = 0x0260;               /* phy timig ctrl 0 */
    for (i = 0; i < 11; i++) {
        writel(pd->timing[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    // T_CLK_POST, T_CLK_PRE for CLK lane P/N HS 200 mV timing length should >
    // data lane HS timing length
    writel(0xa1e, DSI_CLKOUT_TIMING_CTRL);

    off = 0x0290;               /* ctrl 0 */
    for (i = 0; i < 4; i++) {
        writel(pd->ctrl[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    off = 0x02a0;               /* strength 0 */
    for (i = 0; i < 4; i++) {
        writel(pd->strength[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    off = 0x0204;               /* pll ctrl 1, skip 0 */
    for (i = 1; i < 21; i++) {
        writel(pd->pll[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    /* pll ctrl 0 */
    writel(pd->pll[0], MIPI_DSI_BASE + 0x200);
    writel((pd->pll[0] | 0x01), MIPI_DSI_BASE + 0x200);

    return (0);
}

struct mipi_dsi_panel_config *get_panel_info(void)
{
#if  DISPLAY_MIPI_PANEL_TOSHIBA
    return &toshiba_panel_info;
#elif DISPLAY_MIPI_PANEL_NOVATEK_BLUE
    return &novatek_panel_info;
#endif
    return NULL;

}

int dsi_cmd_dma_trigger_for_panel()
{
    unsigned long ReadValue;
    unsigned long count = 0;
    int status = 0;

    writel(0x03030303, DSI_INT_CTRL);
    mdelay(10);
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

int mipi_dsi_cmds_tx(struct mipi_dsi_cmd *cmds, int count)
{
    int ret = 0;
    struct mipi_dsi_cmd *cm;
    int i = 0;

    cm = cmds;
    for (i = 0; i < count; i++) {
        memcpy(DSI_CMD_DMA_MEM_START_ADDR_PANEL, (cm->payload), cm->size);
        writel(DSI_CMD_DMA_MEM_START_ADDR_PANEL, DSI_DMA_CMD_OFFSET);
        writel(cm->size, DSI_DMA_CMD_LENGTH);   // reg 0x48 for this build
        ret += dsi_cmd_dma_trigger_for_panel();
        mdelay(10);
        cm++;
    }
    return ret;
}

int mipi_dsi_panel_initialize(struct mipi_dsi_panel_config *pinfo)
{
    unsigned char DMA_STREAM1 = 0;  // for mdp display processor path
    unsigned char EMBED_MODE1 = 1;  // from frame buffer
    unsigned char POWER_MODE2 = 1;  // from frame buffer
    unsigned char PACK_TYPE1 = 1;   // long packet
    unsigned char VC1 = 0;
    unsigned char DT1 = 0;      // non embedded mode
    unsigned short WC1 = 0;     // for non embedded mode only
    int status = 0;
    unsigned char DLNx_EN;
    unsigned char lane_1 = 1;
    unsigned char lane_2 = 2;

    switch (pinfo->num_of_lanes) {
    default:
    case 1:
        DLNx_EN = 1;            // 1 lane
        break;
    case 2:
        DLNx_EN = 3;            // 2 lane
        break;
    case 3:
        DLNx_EN = 7;            // 3 lane
        break;
    }

    writel(0x0001, DSI_SOFT_RESET);
    writel(0x0000, DSI_SOFT_RESET);

    writel((0 << 16) | 0x3f, DSI_CLK_CTRL); // reg:0x118
    writel(DMA_STREAM1 << 8 | 0x04, DSI_TRIG_CTRL); // reg 0x80 dma trigger: sw
    // trigger 0x4; dma stream1
    writel(0 << 30 | DLNx_EN << 4 | 0x105, DSI_CTRL);   // reg 0x00 for this
    // build
    writel(EMBED_MODE1 << 28 | POWER_MODE2 << 26
           | PACK_TYPE1 << 24 | VC1 << 22 | DT1 << 16 | WC1,
           DSI_COMMAND_MODE_DMA_CTRL);

    status = mipi_dsi_cmds_tx(pinfo->panel_cmds, pinfo->num_of_panel_cmds);

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
    mdelay(10);

    return status;
}

int config_dsi_cmd_mode(unsigned short disp_width, unsigned short disp_height,
                        unsigned short img_width, unsigned short img_height,
                        unsigned short dst_format,
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
    unsigned char ystride = 0x03;
    // disable mdp first

    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000000, DSI_CLK_CTRL);
    writel(0x00000002, DSI_CLK_CTRL);
    writel(0x00000006, DSI_CLK_CTRL);
    writel(0x0000000e, DSI_CLK_CTRL);
    writel(0x0000001e, DSI_CLK_CTRL);
    writel(0x0000003e, DSI_CLK_CTRL);

    writel(0x10000000, DSI_ERR_INT_MASK0);

    // writel(0, DSI_CTRL);

    // writel(0, DSI_ERR_INT_MASK0);

    DST_FORMAT = 8;             // RGB888
    printf("\nDSI_Cmd_Mode - Dst Format: RGB888");

    DLNx_EN = 3;                // 2 lane with clk programming
    printf("\nData Lane: 2 lane\n");

    TRAFIC_MODE = 0;            // non burst mode with sync pulses
    printf("\nTraffic mode: non burst mode with sync pulses\n");

    writel(0x02020202, DSI_INT_CTRL);

    writel(0x00100000 | DST_FORMAT, DSI_COMMAND_MODE_MDP_CTRL);
    writel((img_width * ystride + 1) << 16 | 0x0039,
           DSI_COMMAND_MODE_MDP_STREAM0_CTRL);
    writel((img_width * ystride + 1) << 16 | 0x0039,
           DSI_COMMAND_MODE_MDP_STREAM1_CTRL);
    writel(img_height << 16 | img_width, DSI_COMMAND_MODE_MDP_STREAM0_TOTAL);
    writel(img_height << 16 | img_width, DSI_COMMAND_MODE_MDP_STREAM1_TOTAL);
    writel(0xEE, DSI_CAL_STRENGTH_CTRL);
    writel(0x80000000, DSI_CAL_CTRL);
    writel(0x40, DSI_TRIG_CTRL);
    writel(0x13c2c, DSI_COMMAND_MODE_MDP_DCS_CMD_CTRL);
    writel(interleav << 30 | 0 << 24 | 0 << 20 | DLNx_EN << 4 | 0x105,
           DSI_CTRL);
    mdelay(10);
    writel(0x10000000, DSI_COMMAND_MODE_DMA_CTRL);
    writel(0x10000000, DSI_MISR_CMD_CTRL);
    writel(0x00000040, DSI_ERR_INT_MASK0);
    writel(0x1, DSI_EOT_PACKET_CTRL);
    // writel(0x0, MDP_OVERLAYPROC0_START);
    writel(0x00000001, MDP_DMA_P_START);
    mdelay(10);
    writel(0x1, DSI_CMD_MODE_MDP_SW_TRIGGER);

    status = 1;
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

int mipi_dsi_video_config(unsigned short num_of_lanes)
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

int mipi_dsi_cmd_config(unsigned short num_of_lanes)
{

    int status = 0;
    unsigned long ReadValue;
    unsigned long count = 0;
    unsigned long input_img_addr = MIPI_FB_ADDR;
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

    writel(0x03ffffff, MDP_INTR_ENABLE);
    writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

    // ------------- programming MDP_DMA_P_CONFIG ---------------------
    writel(pack_pattern << 8 | 0x3f | (0 << 25), MDP_DMA_P_CONFIG); // rgb888

    writel(0x00000000, MDP_DMA_P_OUT_XY);
    writel(image_ht << 16 | image_wd, MDP_DMA_P_SIZE);
    writel(input_img_addr, MDP_DMA_P_BUF_ADDR);

    writel(image_wd * ystride, MDP_DMA_P_BUF_Y_STRIDE);

    writel(0x00000000, MDP_DMA_P_OP_MODE);

    writel(0x10, MDP_DSI_CMD_MODE_ID_MAP);
    writel(0x01, MDP_DSI_CMD_MODE_TRIGGER_EN);

    writel(0x0001a000, MDP_AXI_RDMASTER_CONFIG);
    writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
    writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);
    writel(0x8a, MDP_DISP_INTF_SEL);

    return status;
}

int is_cmd_mode_enabled(void)
{
    return cmd_mode_status;
}

void mipi_dsi_cmd_mode_trigger(void)
{
    int status = 0;
    unsigned short display_wd = mipi_fb_cfg.width;
    unsigned short display_ht = mipi_fb_cfg.height;
    unsigned short image_wd = mipi_fb_cfg.width;
    unsigned short image_ht = mipi_fb_cfg.height;
    unsigned short dst_format = 0;
    unsigned short traffic_mode = 0;
    struct mipi_dsi_panel_config *panel_info = &novatek_panel_info;
    status += mipi_dsi_cmd_config(panel_info->num_of_lanes);
    mdelay(50);
    config_dsi_cmd_mode(display_wd, display_ht, image_wd, image_ht,
                        dst_format, traffic_mode,
                        panel_info->num_of_lanes /* num_of_lanes */ );
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
    struct mipi_dsi_panel_config *panel_info = get_panel_info();
    writel(0x00001800, MMSS_SFPB_GPREG);
    configure_dsicore_dsiclk();
    configure_dsicore_byteclk();
    configure_dsicore_pclk();
    mipi_dsi_phy_ctrl_config(panel_info);
    status += mipi_dsi_panel_initialize(panel_info);
    mipi_fb_cfg.base = MIPI_FB_ADDR;

    if (panel_info->mode == MIPI_VIDEO_MODE)
        status += mipi_dsi_video_config(panel_info->num_of_lanes);

    if (panel_info->mode == MIPI_CMD_MODE)
        cmd_mode_status = 1;

    return &mipi_fb_cfg;
}
