/*
 * * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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
 */
#include <debug.h>
#include <reg.h>
#include <mdp3.h>
#include <mipi_dsi.h>
#include <platform/iomap.h>
#include <platform/clock.h>
#include <target/display.h>

int mipi_dsi_phy_init(struct mipi_dsi_panel_config *pinfo){
    struct mipi_dsi_phy_ctrl *pd;
    uint32_t i, off = 0;

    writel(0x00000001, DSI_PHY_SW_RESET);
    mdelay(50);
    writel(0x00000000, DSI_PHY_SW_RESET);

    pd = (pinfo->dsi_phy_config);

    off = 0x02cc;    /* regulator ctrl 0 */
    for (i = 0; i < 4; i++) {
            writel(pd->strength[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    off = 0x0260;   /* phy timig ctrl 0 */
    for (i = 0; i < 11; i++) {
            writel(pd->timing[i], MIPI_DSI_BASE + off);
        off += 4;
    }
        writel(0x202f, DSI_CLKOUT_TIMING_CTRL);

    off = 0x0290;    /* ctrl 0 */
    for (i = 0; i < 4; i++) {
            writel(pd->ctrl[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    off = 0x02a0;    /* strength 0 */
    for (i = 0; i < 4; i++) {
            writel(pd->strength[i], MIPI_DSI_BASE + off);
        off += 4;
    }

    /* calibration ctrl */
    writel(0x67, MIPI_DSI_BASE + 0x100);

    /* pll ctrl 0 */
    writel(pd->pll[0], MIPI_DSI_BASE + 0x200);
    writel((pd->pll[0] | 0x01), MIPI_DSI_BASE + 0x200);

    /* lane swap ctrl */
    writel(0x1, MIPI_DSI_BASE + 0xac);
    return 0;
}

void config_renesas_dsi_video_mode(void)
{

    unsigned char dst_format = 3; /* RGB888 */
    unsigned char traffic_mode = 2; /* non burst mode with sync start events */
    unsigned char lane_en = 3; /* 3 Lanes -- Enables Data Lane0, 1, 2 */
    unsigned long low_pwr_stop_mode = 1;
    unsigned char eof_bllp_pwr = 0x9; /* Needed or else will have blank line at top of display */
    unsigned char interleav = 0;

    dprintf(SPEW, "DSI_Video_Mode - Dst Format: RGB888\n");
    dprintf(SPEW, "Data Lane: 2 lane\n");
    dprintf(SPEW, "Traffic mode: burst mode\n");

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

    writel(0x02020202, DSI_INT_CTRL);

    writel(((MIPI_HSYNC_BACK_PORCH_DCLK + REN_MIPI_FB_WIDTH)<<16) | MIPI_HSYNC_BACK_PORCH_DCLK,
        DSI_VIDEO_MODE_ACTIVE_H);

    writel(((MIPI_VSYNC_BACK_PORCH_LINES +  REN_MIPI_FB_HEIGHT)<<16) | MIPI_VSYNC_BACK_PORCH_LINES,
         DSI_VIDEO_MODE_ACTIVE_V);

    writel(((REN_MIPI_FB_HEIGHT + MIPI_VSYNC_FRONT_PORCH_LINES + MIPI_VSYNC_BACK_PORCH_LINES)<< 16) |
         (REN_MIPI_FB_WIDTH + MIPI_HSYNC_FRONT_PORCH_DCLK + MIPI_HSYNC_BACK_PORCH_DCLK ),
          DSI_VIDEO_MODE_TOTAL );
    writel((MIPI_HSYNC_PULSE_WIDTH)<<16 | 0, DSI_VIDEO_MODE_HSYNC);

    writel(0<<16 | 0, DSI_VIDEO_MODE_VSYNC);

    writel(MIPI_VSYNC_PULSE_WIDTH<<16 | 0, DSI_VIDEO_MODE_VSYNC_VPOS);

    writel(1, DSI_EOT_PACKET_CTRL);

    writel(0x00000100, DSI_MISR_VIDEO_CTRL);

    writel(1 << 28 | 1 << 24 | 1 << 20 | low_pwr_stop_mode << 16 | eof_bllp_pwr << 12 | traffic_mode << 8
           | dst_format << 4 | 0x0, DSI_VIDEO_MODE_CTRL);

    writel(0x67, DSI_CAL_STRENGTH_CTRL);
    writel(0x80006711, DSI_CAL_CTRL);
    writel(0x00010100, DSI_MISR_VIDEO_CTRL);

    writel(0x00010100, DSI_INT_CTRL);
    writel(0x02010202, DSI_INT_CTRL);
    writel(0x02030303, DSI_INT_CTRL);

    writel(interleav << 30 | 0 << 24 | 0 << 20 | lane_en << 4
           | 0x103, DSI_CTRL);
}
