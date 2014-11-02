/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

/*---------------------------------------------------------------------------*/
/* GPIO configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct gpio_pin reset_gpio = {
  "msmgpio", 96, 3, 1, 0, 1
};

static struct gpio_pin enable_gpio = {
  "msmgpio", 137, 3, 1, 0, 1
};

static struct gpio_pin bkl_gpio = {
  "msmgpio", 86, 3, 1, 0, 1
};

static struct gpio_pin pwm_gpio = {
  "pm8084", 7, 3, 1, 0, 1
};

static struct gpio_pin edp_lvl_en_gpio = {
  "msmgpio", 91, 3, 1, 0, 1
};

static struct gpio_pin edp_hpd_gpio = {	/* input */
  "msmgpio", 103, 3, 0, 0, 1
};

/* gpio name, id, strength, direction, pull, state. */
static struct gpio_pin hdmi_cec_gpio = {        /* CEC */
  "msmgpio", 31, 0, 2, 3, 1
};

static struct gpio_pin hdmi_ddc_clk_gpio = {   /* DDC CLK */
  "msmgpio", 32, 0, 2, 3, 1
};

static struct gpio_pin hdmi_ddc_data_gpio = {  /* DDC DATA */
  "msmgpio", 33, 0, 2, 3, 1
};

static struct gpio_pin hdmi_hpd_gpio = {       /* HPD, input */
  "msmgpio", 34, 7, 0, 1, 1
};

static struct gpio_pin hdmi_mux_lpm_gpio = {       /* MUX LPM */
  "msmgpio", 27, 0, 2, 0, 0
};

static struct gpio_pin hdmi_mux_en_gpio = {       /* MUX EN */
  "msmgpio", 83, 3, 2, 3, 1
};

static struct gpio_pin hdmi_mux_sel_gpio = {       /* MUX SEL */
  "msmgpio", 85, 0, 0, 1, 1
};

/*---------------------------------------------------------------------------*/
/* LDO configuration                                                         */
/*---------------------------------------------------------------------------*/
static struct ldo_entry ldo_entry_array[] = {
  { "vdd", 22, 0, 3000000, 100000, 100, 0, 20, 0, 0},
  { "vddio", 12, 0, 1800000, 100000, 100, 0, 20, 0, 0},
  { "vdda", 2, 1, 1200000, 100000, 100, 0, 0, 0, 0},
};

#define TOTAL_LDO_DEFINED 3

/*---------------------------------------------------------------------------*/
/* Target Physical configuration                                             */
/*---------------------------------------------------------------------------*/

static const uint32_t panel_strength_ctrl[] = {
  0xff, 0x06
};

static const char panel_bist_ctrl[] = {
  0x00, 0x00, 0xb1, 0xff, 0x00, 0x00
};

static const uint32_t panel_regulator_settings[] = {
  0x03, 0x09, 0x03, 0x00, 0x20, 0x07, 0x01
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
#define DISPLAY_CMDLINE_PREFIX " mdss_mdp.panel="

#define MIPI_FB_ADDR  0x03200000
#define HDMI_FB_ADDR  0x05200000

#define MIPI_HSYNC_PULSE_WIDTH       12
#define MIPI_HSYNC_BACK_PORCH_DCLK   32
#define MIPI_HSYNC_FRONT_PORCH_DCLK  144

#define MIPI_VSYNC_PULSE_WIDTH       4
#define MIPI_VSYNC_BACK_PORCH_LINES  3
#define MIPI_VSYNC_FRONT_PORCH_LINES 9

#define PWM_BL_LPG_CHAN_ID           3

#define HDMI_PANEL_NAME              "hdmi"
#define HDMI_CONTROLLER_STRING       "hdmi:0"

#endif
