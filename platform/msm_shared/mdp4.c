/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
#include <mdp4.h>
#include <debug.h>
#include <reg.h>
#include <target/display.h>
#include <platform/timer.h>
#include <platform/iomap.h>

void mdp_setup_dma_p_video_config(unsigned short pack_pattern,
                                    unsigned short img_width,
                                    unsigned short img_height,
                                    unsigned long input_img_addr,
                                    unsigned short img_width_full_size,
                                    unsigned char ystride){
    dprintf(SPEW, "MDP4.2 Setup for DSI Video Mode\n");

    // ----- programming MDP_AXI_RDMASTER_CONFIG --------
    /* MDP_AXI_RDMASTER_CONFIG set all master to read from AXI port 0, that's
       the only port connected */
    //TODO: Seems to still work without this
    writel(0x00290000, MDP_AXI_RDMASTER_CONFIG);
    writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
    writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);

   /* Set up CMD_INTF_SEL, VIDEO_INTF_SEL, EXT_INTF_SEL, SEC_INTF_SEL, PRIM_INTF_SEL */
   writel(0x00000049, MDP_DISP_INTF_SEL);

   /* DMA P */
   writel(0x0000000b, MDP_OVERLAYPROC0_CFG);

   /* RGB 888 */
   writel(pack_pattern << 8 | 0xbf | (0 << 25), MDP_DMA_P_CONFIG);

   writel(0x0, MDP_DMA_P_OUT_XY);

   writel(img_height << 16 | img_width, MDP_DMA_P_SIZE);

   writel(input_img_addr, MDP_DMA_P_BUF_ADDR);

   writel(img_width_full_size * ystride, MDP_DMA_P_BUF_Y_STRIDE);
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

    dprintf(SPEW, "MDP4.1 for DSI Video Mode\n");

    hsync_period = img_width + hsync_porch0_fp + hsync_porch0_bp + 1;
    vsync_period_intmd = img_height + vsync_porch0_fp + vsync_porch0_bp + 1;
    vsync_period = vsync_period_intmd * hsync_period;

    // ----- programming MDP_AXI_RDMASTER_CONFIG --------
    /* MDP_AXI_RDMASTER_CONFIG set all master to read from AXI port 0, that's
       the only port connected */
    writel(0x00290000, MDP_AXI_RDMASTER_CONFIG);
    writel(0x00000004, MDP_AXI_WRMASTER_CONFIG);
    writel(0x00007777, MDP_MAX_RD_PENDING_CMD_CONFIG);
    /* sets PRIM_INTF_SEL to 0x1 and SEC_INTF_SEL to 0x2 and DSI_VIDEO_INTF_SEL*/
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


int mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg, unsigned short num_of_lanes)
{

    int status = 0;
    unsigned long input_img_addr = MIPI_FB_ADDR;
    unsigned short image_wd = mipi_fb_cfg.width;
    unsigned short image_ht = mipi_fb_cfg.height;
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

void mdp_disable(void)
{
    writel(0x00000000, MDP_DSI_VIDEO_EN);
}

void mdp_shutdown(void)
{
    mdp_disable();
    mdelay(60);
    writel(0x00000000, MDP_INTR_ENABLE);
    writel(0x00000003, MDP_OVERLAYPROC0_CFG);
}

void mdp_start_dma(void)
{
     writel(0x00000001, MDP_DMA_P_START);
}
