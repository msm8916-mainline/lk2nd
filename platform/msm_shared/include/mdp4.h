/* Copyright (c) 2011-2014, The Linux Foundation. All rights reserved.
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
#ifndef _PLATFORM_MSM_SHARED_MDP_4_H_
#define _PLATFORM_MSM_SHARED_MDP_4_H_

#include <dev/fbcon.h>
#include <msm_panel.h>

//TODO: Make a global PASS / FAIL define
#define PASS                        0
#define FAIL                        1

#define MDP_OVERLAYPROC0_START                REG_MDP(0x00004)
#define MDP_DMA_P_START                       REG_MDP(0x0000C)
#define MDP_DMA_S_START                       REG_MDP(0x00010)
#define MDP_AXI_RDMASTER_CONFIG               REG_MDP(0x00028)
#define MDP_AXI_WRMASTER_CONFIG               REG_MDP(0x00030)
#define MDP_DISP_INTF_SEL                     REG_MDP(0x00038)
#define MDP_MAX_RD_PENDING_CMD_CONFIG         REG_MDP(0x0004C)
#define MDP_INTR_ENABLE                       REG_MDP(0x00050)
#define MDP_DSI_CMD_MODE_ID_MAP               REG_MDP(0x000A0)
#define MDP_DSI_CMD_MODE_TRIGGER_EN           REG_MDP(0x000A4)
#define MDP_OVERLAYPROC0_CFG                  REG_MDP(0x10004)
#define MDP_OVERLAYPROC0_OPMODE               REG_MDP(0x10014)
#define MDP_LAYERMIXER_IN_CFG                 REG_MDP(0x10100)
#define MDP_OVERLAY_REG_FLUSH                 REG_MDP(0x18000)

#define MDP_RGB1_SRC_SIZE                     REG_MDP(0x40000)
#define MDP_RGB1_SRC_XY                       REG_MDP(0x40004)
#define MDP_RGB1_OUT_SIZE                     REG_MDP(0x40008)
#define MDP_RGB1_OUT_XY                       REG_MDP(0x4000C)
#define MDP_RGB1_SRCP0_ADDR                   REG_MDP(0x40010)
#define MDP_RGB1_SRCP0_ADDR_1                 REG_MDP(0x40014)
#define MDP_RGB1_SRCP0_ADDR_2                 REG_MDP(0x40018)
#define MDP_RGB1_SRC_YSTRIDE1                 REG_MDP(0x40040)
#define MDP_RGB1_SRC_FORMAT                   REG_MDP(0x40050)
#define MDP_RGB1_CONSTANT_COLOR               REG_MDP(0x41008)

#define MDP_DSI_VIDEO_EN                      REG_MDP(0xE0000)
#define MDP_DSI_VIDEO_HSYNC_CTL               REG_MDP(0xE0004)
#define MDP_DSI_VIDEO_VSYNC_PERIOD            REG_MDP(0xE0008)
#define MDP_DSI_VIDEO_VSYNC_PULSE_WIDTH       REG_MDP(0xE000C)
#define MDP_DSI_VIDEO_DISPLAY_HCTL            REG_MDP(0xE0010)
#define MDP_DSI_VIDEO_DISPLAY_V_START         REG_MDP(0xE0014)
#define MDP_DSI_VIDEO_DISPLAY_V_END           REG_MDP(0xE0018)
#define MDP_DSI_VIDEO_ACTIVE_HCTL             REG_MDP(0xE001C)
#define MDP_DSI_VIDEO_BORDER_CLR              REG_MDP(0xE0028)
#define MDP_DSI_VIDEO_HSYNC_SKEW              REG_MDP(0xE0030)
#define MDP_DSI_VIDEO_CTL_POLARITY            REG_MDP(0xE0038)
#define MDP_DSI_VIDEO_TEST_CTL                REG_MDP(0xE0034)

#define MDP_TEST_MODE_CLK                     REG_MDP(0xF0000)
#define MDP_INTR_STATUS                       REG_MDP(0x00054)

void mdp_setup_dma_p_video_config(unsigned short pack_pattern,
				unsigned short img_width,
				unsigned short img_height,
				unsigned long input_img_addr,
				unsigned short img_width_full_size,
				unsigned char ystride);
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
				unsigned char ystride);
int mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg,
			unsigned short num_of_lanes);
void mdp_shutdown(void);
void mdp_disable(void);
void mdp_start_dma(void);
int mdp_dsi_video_off();
int mdp_dsi_video_on(struct msm_panel_info *pinfo);
int mdp_dsi_video_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mdp_lcdc_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mdp_lcdc_on();
int mdp_lcdc_off();
void mdp_set_revision(int rev);
int mdp_get_revision();

/* defining no-op functions that are implemented only for mdp5 */
int mdp_edp_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mdp_edp_on(struct msm_panel_info *pinfo);
int mdp_edp_off(void);

#endif
