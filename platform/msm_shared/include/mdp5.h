/* Copyright (c) 2012-2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _PLATFORM_MSM_SHARED_MDP_5_H_
#define _PLATFORM_MSM_SHARED_MDP_5_H_

#include <msm_panel.h>

#define PIPE_SSPP_SRC0_ADDR                     0x14
#define PIPE_SSPP_SRC_YSTRIDE                   0x24
#define PIPE_SSPP_SRC_IMG_SIZE                  0x04
#define PIPE_SSPP_SRC_SIZE                      0x00
#define PIPE_SSPP_SRC_OUT_SIZE                  0x0C
#define PIPE_SSPP_SRC_XY                        0x08
#define PIPE_SSPP_OUT_XY                        0x10
#define PIPE_SSPP_SRC_FORMAT                    0x30
#define PIPE_SSPP_SRC_UNPACK_PATTERN            0x34
#define PIPE_SSPP_SRC_OP_MODE                   0x38
#define REQPRIORITY_FIFO_WATERMARK0        	0x50
#define REQPRIORITY_FIFO_WATERMARK1        	0x54
#define REQPRIORITY_FIFO_WATERMARK2        	0x58
#define PIPE_SW_PIXEL_EXT_C0_REQ                0x108
#define PIPE_SW_PIXEL_EXT_C1C2_REQ              0x118
#define PIPE_SW_PIXEL_EXT_C3_REQ                0x128
#define PIPE_COMP0_3_PHASE_STEP_X               0x210
#define PIPE_COMP0_3_PHASE_STEP_Y               0x214
#define PIPE_COMP1_2_PHASE_STEP_X               0x218
#define PIPE_COMP1_2_PHASE_STEP_Y               0x21c

#define LAYER_0_OUT_SIZE                        0x04
#define LAYER_0_OP_MODE                         0x00
#define LAYER_0_BORDER_COLOR_0                  0x08
#define LAYER_0_BLEND_OP                        0x20
#define LAYER_0_BLEND0_FG_ALPHA                 0x24
#define LAYER_1_BLEND_OP                        0x50
#define LAYER_1_BLEND0_FG_ALPHA                 0x54
#define LAYER_2_BLEND_OP                        0x80
#define LAYER_2_BLEND0_FG_ALPHA                 0x84
#define LAYER_3_BLEND_OP                        0xB0
#define LAYER_3_BLEND0_FG_ALPHA                 0xB4

/* HW Revisions for different MDSS targets */
#define MDSS_GET_MAJOR(rev)         ((rev) >> 28)
#define MDSS_GET_MINOR(rev)         (((rev) >> 16) & 0xFFF)
#define MDSS_GET_STEP(rev)          ((rev) & 0xFFFF)
#define MDSS_GET_MAJOR_MINOR(rev)   ((rev) >> 16)

#define MDSS_IS_MAJOR_MINOR_MATCHING(a, b) \
        (MDSS_GET_MAJOR_MINOR((a)) == MDSS_GET_MAJOR_MINOR((b)))

#define MDSS_MDP_REV(major, minor, step) \
        ((((major) & 0x000F) << 28) |    \
         (((minor) & 0x0FFF) << 16) |    \
         ((step)   & 0xFFFF))

#define MDSS_MDP_HW_REV_100    MDSS_MDP_REV(1, 0, 0) /* 8974 v1.0 */
#define MDSS_MDP_HW_REV_101    MDSS_MDP_REV(1, 1, 0) /* 8x26 v1.0 */
#define MDSS_MDP_HW_REV_101_1  MDSS_MDP_REV(1, 1, 1) /* 8x26 v2.0, 8926 v1.0 */
#define MDSS_MDP_HW_REV_102    MDSS_MDP_REV(1, 2, 0) /* 8974 v2.0 */
#define MDSS_MDP_HW_REV_102_1  MDSS_MDP_REV(1, 2, 1) /* 8974 v3.0 (Pro) */
#define MDSS_MDP_HW_REV_103    MDSS_MDP_REV(1, 3, 0) /* 8084 v1.0 */
#define MDSS_MDP_HW_REV_105    MDSS_MDP_REV(1, 5, 0) /* 8994 v1.0 */
#define MDSS_MDP_HW_REV_106    MDSS_MDP_REV(1, 6, 0) /* 8916 v1.0 */
#define MDSS_MDP_HW_REV_107    MDSS_MDP_REV(1, 7, 0) /* 8996 v1.0 */
#define MDSS_MDP_HW_REV_108    MDSS_MDP_REV(1, 8, 0) /* 8939 v1.0 */
#define MDSS_MDP_HW_REV_109    MDSS_MDP_REV(1, 9, 0) /* 8994 v2.0 */
#define MDSS_MDP_HW_REV_110    MDSS_MDP_REV(1, 10, 0) /* 8992 v1.0 */
#define MDSS_MDP_HW_REV_111    MDSS_MDP_REV(1, 11, 0) /* 8956 v1.0 */
#define MDSS_MDP_HW_REV_112    MDSS_MDP_REV(1, 12, 0) /* 8952 v1.0 */
#define MDSS_MDP_HW_REV_114    MDSS_MDP_REV(1, 14, 0) /* 8937 v1.0 */
#define MDSS_MDP_HW_REV_116    MDSS_MDP_REV(1, 16, 0) /* msm8953 */
#define MDSS_MDP_HW_REV_115    MDSS_MDP_REV(1, 15, 0) /* msm8917 v1.0 */
#define MDSS_MDP_HW_REV_200    MDSS_MDP_REV(2, 0, 0) /* 8092 v1.0 */

#define MDSS_MAX_LINE_BUF_WIDTH 2048

#define MDP_HW_REV                              REG_MDP(0x0100)
#define MDP_INTR_EN                             REG_MDP(0x0110)
#define MDP_INTR_CLEAR                          REG_MDP(0x0118)
#define MDP_HIST_INTR_EN                        REG_MDP(0x011C)

#define MDP_DISP_INTF_SEL                       REG_MDP(0x0104)
#define MDP_VIDEO_INTF_UNDERFLOW_CTL            REG_MDP(0x03E0)
#define MDP_UPPER_NEW_ROI_PRIOR_RO_START        REG_MDP(0x02EC)
#define MDP_LOWER_NEW_ROI_PRIOR_TO_START        REG_MDP(0x04F8)

#define MDP_INTF_0_TIMING_ENGINE_EN             REG_MDP(0x12500)
#define MDP_INTF_1_TIMING_ENGINE_EN             REG_MDP(0x12700)
#define MDP_INTF_2_TIMING_ENGINE_EN             REG_MDP(0x12900)
#define MDP_INTF_3_TIMING_ENGINE_EN             REG_MDP(0x12B00)

#define MDP_CTL_0_BASE                          REG_MDP(0x600)
#define MDP_CTL_1_BASE                          REG_MDP(0x700)

#define MDP_PP_0_BASE                           REG_MDP(0x12D00)
#define MDP_PP_1_BASE                           REG_MDP(0x12E00)

#define MDP_DSC_0_BASE                          REG_MDP(0x81000)
#define MDP_DSC_1_BASE                          REG_MDP(0x81400)

#define CTL_LAYER_0                             0x00
#define CTL_LAYER_1                             0x04
#define CTL_TOP                                 0x14
#define CTL_FLUSH                               0x18
#define CTL_START                               0x1C

#define MDP_REG_SPLIT_DISPLAY_EN                REG_MDP(0x3F4)
#define MDP_REG_SPLIT_DISPLAY_UPPER_PIPE_CTL    REG_MDP(0x3F8)
#define MDP_REG_SPLIT_DISPLAY_LOWER_PIPE_CTL    REG_MDP(0x4F0)

#define MDP_INTF_0_BASE                         REG_MDP(0x12500)
#define MDP_INTF_1_BASE                         REG_MDP(0x12700)
#define MDP_INTF_2_BASE                         REG_MDP(0x12900)
#define MDP_INTF_3_BASE                         REG_MDP(0x12B00)

#define MDP_INTF_CONFIG                         0x04
#define MDP_HSYNC_CTL                           0x08
#define MDP_VSYNC_PERIOD_F0                     0x0C
#define MDP_VSYNC_PERIOD_F1                     0x10
#define MDP_VSYNC_PULSE_WIDTH_F0                0x14
#define MDP_VSYNC_PULSE_WIDTH_F1                0x18
#define MDP_DISPLAY_HCTL                        0x3C
#define MDP_DISPLAY_V_START_F0                  0x1C
#define MDP_DISPLAY_V_START_F1                  0x20
#define MDP_DISPLAY_V_END_F0                    0x24
#define MDP_DISPLAY_V_END_F1                    0x28
#define MDP_ACTIVE_HCTL                         0x40
#define MDP_ACTIVE_V_START_F0                   0x2C
#define MDP_ACTIVE_V_START_F1                   0x30
#define MDP_ACTIVE_V_END_F0                     0x34
#define MDP_ACTIVE_V_END_F1                     0x38
#define MDP_UNDERFFLOW_COLOR                    0x48
#define MDP_PANEL_FORMAT                        0x90
#define MDP_PROG_FETCH_START                    0x170

#define MDP_CLK_CTRL0                           REG_MDP(0x03AC)
#define MDP_CLK_CTRL1                           REG_MDP(0x03B4)
#define MDP_CLK_CTRL2                           REG_MDP(0x03BC)
#define MDP_CLK_CTRL3                           REG_MDP(0x04A8)
#define MDP_CLK_CTRL4                           REG_MDP(0x04B0)
#define MDP_CLK_CTRL5                           REG_MDP(0x04B8)
#define MDP_CLK_CTRL6                           REG_MDP(0x03C4)
#define MDP_CLK_CTRL7                           REG_MDP(0x04D0)

#define MMSS_MDP_SMP_ALLOC_W_BASE               REG_MDP(0x0180)
#define MMSS_MDP_SMP_ALLOC_R_BASE               REG_MDP(0x0230)

/* source pipe opmode bits for flip */
#define MDSS_MDP_OP_MODE_FLIP_UD                BIT(14)
#define MDSS_MDP_OP_MODE_FLIP_LR                BIT(13)

#define MDP_QOS_REMAPPER_CLASS_0                REG_MDP(0x02E0)
#define MDP_QOS_REMAPPER_CLASS_1                REG_MDP(0x02E4)

#define VBIF_VBIF_DDR_FORCE_CLK_ON              REG_MDP(0x24004)
#define VBIF_VBIF_DDR_OUT_MAX_BURST             REG_MDP(0x240D8)
#define VBIF_VBIF_DDR_ARB_CTRL                  REG_MDP(0x240F0)
#define VBIF_VBIF_DDR_RND_RBN_QOS_ARB           REG_MDP(0x24124)
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF0        REG_MDP(0x24160)
#define VBIF_VBIF_DDR_AXI_AMEMTYPE_CONF1        REG_MDP(0x24164)
#define VBIF_VBIF_DDR_OUT_AOOO_AXI_EN           REG_MDP(0x24178)
#define VBIF_VBIF_DDR_OUT_AX_AOOO               REG_MDP(0x2417C)
#define VBIF_VBIF_IN_RD_LIM_CONF0               REG_MDP(0x240B0)
#define VBIF_VBIF_IN_RD_LIM_CONF1               REG_MDP(0x240B4)
#define VBIF_VBIF_IN_RD_LIM_CONF2               REG_MDP(0x240B8)
#define VBIF_VBIF_IN_RD_LIM_CONF3               REG_MDP(0x240BC)
#define VBIF_VBIF_IN_WR_LIM_CONF0               REG_MDP(0x240C0)
#define VBIF_VBIF_IN_WR_LIM_CONF1               REG_MDP(0x240C4)
#define VBIF_VBIF_IN_WR_LIM_CONF2               REG_MDP(0x240C8)
#define VBIF_VBIF_IN_WR_LIM_CONF3               REG_MDP(0x240CC)
#define VBIF_VBIF_ABIT_SHORT                    REG_MDP(0x24070)
#define VBIF_VBIF_ABIT_SHORT_CONF               REG_MDP(0x24074)
#define VBIF_VBIF_GATE_OFF_WRREQ_EN             REG_MDP(0x240A8)

#define MDSS_MDP_REG_PP_FBC_MODE                0x034
#define MDSS_MDP_REG_PP_FBC_BUDGET_CTL          0x038
#define MDSS_MDP_REG_PP_FBC_LOSSY_MODE          0x03C

#define MDSS_MDP_REG_DCE_SEL			REG_MDP(0x1450)

#define MDSS_MDP_PP_DSC_MODE                0x0A0
#define MDSS_MDP_PP_DCE_DATA_OUT_SWAP	0x0C8

#define MDSS_MDP_DSC_COMMON_MODE		0x00
#define MDSS_MDP_DSC_ENC			0x04
#define MDSS_MDP_DSC_PICTURE			0x08
#define MDSS_MDP_DSC_SLICE			0x0c
#define MDSS_MDP_DSC_CHUNK_SIZE			0x10
#define MDSS_MDP_DSC_DELAY			0x14
#define MDSS_MDP_DSC_SCALE_INITIAL		0x18
#define MDSS_MDP_DSC_SCALE_DEC_INTERVAL		0x1c
#define MDSS_MDP_DSC_SCALE_INC_INTERVAL		0x20
#define MDSS_MDP_DSC_FIRST_LINE_BPG_OFFSET	0x24
#define MDSS_MDP_DSC_BPG_OFFSET			0x28
#define MDSS_MDP_DSC_DSC_OFFSET			0x2c
#define MDSS_MDP_DSC_FLATNESS			0x30
#define MDSS_MDP_DSC_RC_MODEL_SIZE		0x34
#define MDSS_MDP_DSC_RC				0x38
#define MDSS_MDP_DSC_RC_BUF_THRESH		0x3c	/* 14 bytes */
#define MDSS_MDP_DSC_RANGE_MIN_QP		0x74	/* 15 bytes */
#define MDSS_MDP_DSC_RANGE_MAX_QP		0xB0	/* 15 bytes */
#define MDSS_MDP_DSC_RANGE_BPG_OFFSET		0xEc	/* 15 bytes */

/* Autorefresh related registers */
#define MDP_REG_PP_0_SYNC_CONFIG_VSYNC		REG_MDP(0x71004)
#define MDP_REG_PP_1_SYNC_CONFIG_VSYNC		REG_MDP(0x71804)
#define MDP_REG_PP_SLAVE_SYNC_CONFIG_VSYNC	REG_MDP(0x73004)
#define MDP_REG_PP_0_AUTOREFRESH_CONFIG		REG_MDP(0x71030)
#define MDP_REG_PP_1_AUTOREFRESH_CONFIG		REG_MDP(0x71830)
#define MDP_REG_PP_SLAVE_AUTOREFRESH_CONFIG	REG_MDP(0x73030)

/* Registers for programming the CDM hardware in bypass mode. */
#define CDM_HDMI_PACK_OP_MODE 		REG_MDP(0x7A400)
#define MDP_OUT_CTL_0 			REG_MDP(0x01410)
#define MDP_INTF_3_INTF_CONFIG 		REG_MDP(0x6C804)
#define CDM_CDWN2_OUT_SIZE 		REG_MDP(0x7A330)
#define CDM_CDWN2_OP_MODE 		REG_MDP(0x7A300)
#define CDM_CDWN2_CLAMP_OUT 		REG_MDP(0x7A304)
#define CDM_CSC_10_OP_MODE 		REG_MDP(0x7A200)

void mdp_set_revision(int rev);
int mdp_get_revision();
int mdp_dsi_video_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mdp_dsi_cmd_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mipi_dsi_cmd_config(struct fbcon_config mipi_fb_cfg,
			unsigned short num_of_lanes);
int mdp_dsi_video_on(struct msm_panel_info *pinfo);
int mdp_dma_on(struct msm_panel_info *pinfo);
int mdp_edp_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
int mdp_edp_on(struct msm_panel_info *pinfo);
int mdp_edp_off(void);
void mdp_disable(void);
void mdp_gdsc_ctrl(uint8_t enable);
int mdp_get_revision();
uint8_t target_is_edp();
void target_edp_panel_init(struct msm_panel_info *pinfo);
int target_edp_panel_clock(uint8_t enable, struct msm_panel_info *pinfo);
int target_edp_panel_enable(void);
int target_edp_panel_disable(void);
int target_edp_bl_ctrl(int enable);
int mdss_hdmi_init(void);
void mdss_hdmi_display_init(uint32_t rev, void *base);
int mdss_hdmi_on(struct msm_panel_info *pinfo);
int mdss_hdmi_off(struct msm_panel_info *pinfo);
int mdss_hdmi_config(struct msm_panel_info *pinfo, struct fbcon_config *fb);
void mdss_hdmi_get_vic(char *buf);
void hdmi_phy_init(void);
int msm_display_off();
void display_shutdown(void);

void mdss_dsc_parameters_calc(struct msm_panel_info *pinfo);
int mdss_dsc_to_buf(struct msm_panel_info *pinfo);
void mdss_dsc_dsi_config(uint32_t ctl_base, int mode, struct dsc_desc *dsc);
void mdss_dsc_mdp_config(struct msm_panel_info *pinfo,
	unsigned int pp_base, unsigned int dsc_base,
	bool mux, bool split_mode);

#endif
