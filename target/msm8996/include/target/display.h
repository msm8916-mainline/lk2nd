/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#ifndef _TARGET_DISPLAY_H
#define _TARGET_DISPLAY_H

/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include <display_resource.h>
#include <msm_panel.h>

/*---------------------------------------------------------------------------*/
/* Target Physical configuration                                             */
/*---------------------------------------------------------------------------*/

static const uint32_t panel_strength_ctrl[] = {
  0xFF, 0x06,
  0xFF, 0x06,
  0xFF, 0x06,
  0xFF, 0x06,
  0xFF, 0x00
};

static const uint32_t panel_regulator_settings[] = {
  0x1d, 0x1d, 0x1d, 0x1d, 0x1d
};

static const char panel_lane_config[] = {
  0x00, 0x00, 0x10, 0x0f,
  0x00, 0x00, 0x10, 0x0f,
  0x00, 0x00, 0x10, 0x0f,
  0x00, 0x00, 0x10, 0x0f,
  0x00, 0x00, 0x10, 0x8f,
};

static const char panel_bist_ctrl[] = { };

static const uint32_t panel_physical_ctrl[] = { };
/*---------------------------------------------------------------------------*/
/* Other Configuration                                                       */
/*---------------------------------------------------------------------------*/
#define DISPLAY_CMDLINE_PREFIX " mdss_mdp.panel="

#define MIPI_FB_ADDR  0x83400000
#define HDMI_FB_ADDR  0xB1C00000

#define MIPI_HSYNC_PULSE_WIDTH       16
#define MIPI_HSYNC_BACK_PORCH_DCLK   32
#define MIPI_HSYNC_FRONT_PORCH_DCLK  76

#define MIPI_VSYNC_PULSE_WIDTH       2
#define MIPI_VSYNC_BACK_PORCH_LINES  2
#define MIPI_VSYNC_FRONT_PORCH_LINES 4

#define PWM_BL_LPG_CHAN_ID           4	/* lpg_out<3> */

#define HDMI_PANEL_NAME              "hdmi"
#define HDMI_CONTROLLER_STRING       "hdmi:"
#define HDMI_VIC_LEN                 5

/*---------------------------------------------------------------------------*/
/* Functions		                                                     */
/*---------------------------------------------------------------------------*/
int target_display_pre_on();
int target_display_pre_off();
int target_display_post_on();
int target_display_post_off();
int target_cont_splash_screen();
int target_display_get_base_offset(uint32_t base);
void target_force_cont_splash_disable(uint8_t override);
uint8_t target_panel_auto_detect_enabled();
void target_set_switch_gpio(int enable_dsi2hdmibridge);
int target_hdmi_pll_clock(uint8_t enable, struct msm_panel_info *pinfo);
int target_hdmi_panel_clock(uint8_t enable, struct msm_panel_info *pinfo);
int target_hdmi_regulator_ctrl(uint8_t enable);
int target_hdmi_gpio_ctrl(uint8_t enable);

#endif
