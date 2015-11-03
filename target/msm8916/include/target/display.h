/* Copyright (c) 2013-2015, The Linux Foundation. All rights reserved.
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
#ifndef _TARGET_MSM8916_DISPLAY_H
#define _TARGET_MSM8916_DISPLAY_H

/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include <display_resource.h>

/*---------------------------------------------------------------------------*/
/* GPIO configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct gpio_pin reset_gpio = {
  "msmgpio", 25, 3, 1, 0, 1
};

static struct gpio_pin ts_reset_gpio = {
  "msmgpio", 12, 3, 1, 0, 1
};

static struct gpio_pin enable_gpio = {
  "msmgpio", 97, 3, 1, 0, 1
};

static struct gpio_pin bkl_gpio = {
  "msmgpio", 98, 3, 1, 0, 1
};

/*Use GPIO 75 for incell panel setup*/
static struct gpio_pin enable_gpio_1 = {
  "msmgpio", 75, 3, 1, 0, 1
};

static struct gpio_pin enp_gpio = {
  "msmgpio", 97, 3, 1, 0, 1
};

static struct gpio_pin enn_gpio = {
  "msmgpio", 32, 3, 1, 0, 1
};

/*Use GPIO 77 for incell panel setup*/
static struct gpio_pin enn_gpio_1 = {
  "msmgpio", 77, 3, 1, 0, 1
};

static struct gpio_pin te_gpio = {
  0, 0, 0, 0, 0, 0
};

static struct gpio_pin pwm_gpio = {
  0, 0, 0, 0, 0, 0
};

static struct gpio_pin bkl_gpio_skuk = {
  "msmgpio", 1, 3, 1, 0, 1
};

static struct gpio_pin enp_gpio_skuk = {
  "msmgpio", 97, 3, 1, 0, 1
};

static struct gpio_pin enn_gpio_skuk = {
  "msmgpio", 98, 3, 1, 0, 1
};

static struct gpio_pin enable_gpio_skut1 = {
  "msmgpio", 8, 3, 1, 0, 1
};

/*---------------------------------------------------------------------------*/
/* Target Physical configuration                                             */
/*---------------------------------------------------------------------------*/

static const uint32_t panel_strength_ctrl[] = {
  0xff, 0x06
};

static const char panel_bist_ctrl[] = {
  0x00, 0x00, 0xb1, 0xff, 0x00, 0x00
};

extern uint32_t panel_regulator_settings[7];

static const uint32_t dcdc_regulator_settings[] = {
  0x03, 0x08, 0x07, 0x00, 0x20, 0x07, 0x01
};

static const uint32_t ldo_regulator_settings[] = {
  0x00, 0x01, 0x01, 0x00, 0x20, 0x07, 0x00
};

static const char panel_lane_config[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x97,
  0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x01, 0x97,
  0x00, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x01, 0x97,
  0x00, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x00, 0x01, 0x97,
  0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xbb
};

static const uint32_t panel_physical_ctrl[] = {
  0x5f, 0x00, 0x00, 0x10
};

/*---------------------------------------------------------------------------*/
/* Other Configuration                                                       */
/*---------------------------------------------------------------------------*/
#define DISPLAY_CMDLINE_PREFIX	" mdss_mdp.panel="
#define ADV7533_CMDLINE_PREFIX	" adv7533_dsi2hdmi.panel="
#define HDMI_PANEL_NAME			"hdmi"
#define HDMI_CONTROLLER_STRING	"hdmi:0"
#define DSI_PANEL_DT_PREFIX				"1:dsi:0:qcom,mdss_dsi_"
#define ADV7533_DSI_HDMI_STRING	"dsi:0:qcom,mdss_dsi_adv7533_1080p"
#define ADV7533_DSI_HDMI_720p_STRING	"dsi:0:qcom,mdss_dsi_adv7533_720p"
#define ADV7533_I2C_HDMI_STRING			"hdmi1080p"
#define ADV7533_I2C_HDMI_720p_STRING	"hdmi720p"
#define ADV7533_DSI_DSI_STRING	"dsi:0:qcom,mdss_dsi_jdi_1080p_video"
#define ADV7533_I2C_DSI_STRING	"dsi"
#define ADV7533_MAX_STRING_LEN	73

#define MIPI_FB_ADDR 0x83200000

#define MIPI_HSYNC_PULSE_WIDTH       12
#define MIPI_HSYNC_BACK_PORCH_DCLK   32
#define MIPI_HSYNC_FRONT_PORCH_DCLK  144

#define MIPI_VSYNC_PULSE_WIDTH       4
#define MIPI_VSYNC_BACK_PORCH_LINES  3
#define MIPI_VSYNC_FRONT_PORCH_LINES 9

enum qrd_subtype
{
	HW_PLATFORM_SUBTYPE_DEFAULT = 0,
	HW_PLATFORM_SUBTYPE_CDP_1 = 1,
	HW_PLATFORM_SUBTYPE_CDP_2 = 2,
	HW_PLATFORM_SUBTYPE_MTP_3 = 3,
	HW_PLATFORM_SUBTYPE_SKUH = 4,
	HW_PLATFORM_SUBTYPE_SKUI = 5, /* msm8916 */
	HW_PLATFORM_SUBTYPE_SKUK = 5, /* msm8939 */
	HW_PLATFORM_SUBTYPE_SKUT1 = 0x40, /* msm8916 */
};

enum {
	JDI_1080P_VIDEO_PANEL,
	NT35590_720P_VIDEO_PANEL,
	NT35590_720P_CMD_PANEL,
	INNOLUX_720P_VIDEO_PANEL,
	OTM8019A_FWVGA_VIDEO_PANEL,
	OTM1283A_720P_VIDEO_PANEL,
	NT35596_1080P_VIDEO_PANEL,
	SHARP_WQXGA_DUALDSI_VIDEO_PANEL,
	JDI_FHD_VIDEO_PANEL,
	HX8379A_FWVGA_VIDEO_PANEL,
	HX8394D_720P_VIDEO_PANEL,
	NT35521_WXGA_VIDEO_PANEL,
	R61318_HD_VIDEO_PANEL,
	R63417_1080P_VIDEO_PANEL,
	JDI_A216_FHD_VIDEO_PANEL,
	UNKNOWN_PANEL
};

#endif
