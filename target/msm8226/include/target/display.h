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
#ifndef _TARGET_MSM8226_DISPLAY_H
#define _TARGET_MSM8226_DISPLAY_H

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

static struct gpio_pin enable_gpio = {
  "msmgpio", 109, 3, 1, 0, 1
};

static struct gpio_pin te_gpio = {
  0, 0, 0, 0, 0, 0
};

static struct gpio_pin pwm_gpio = {
  0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* LDO configuration                                                         */
/*---------------------------------------------------------------------------*/
static struct ldo_entry ldo_entry_array[] = {
  { "vdd", 15, 0, 2800000, 100000, 100, 0, 20, 0, 20},
{ "vddio", 8, 0, 1800000, 100000, 100, 0, 30, 0, 30},
{ "vdda", 4, 1, 1200000, 100000, 100, 0, 20, 0, 30},
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
  0x07, 0x08, 0x07, 0x00, 0x20, 0x07, 0x01
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
#define MIPI_FB_ADDR_QVGA  0x04200000

#define MIPI_HSYNC_PULSE_WIDTH       12
#define MIPI_HSYNC_BACK_PORCH_DCLK   32
#define MIPI_HSYNC_FRONT_PORCH_DCLK  144

#define MIPI_VSYNC_PULSE_WIDTH       4
#define MIPI_VSYNC_BACK_PORCH_LINES  3
#define MIPI_VSYNC_FRONT_PORCH_LINES 9

#define PWM_BL_LPG_CHAN_ID           8

enum target_subtype {
	HW_PLATFORM_SUBTYPE_720P = 0,
	HW_PLATFORM_SUBTYPE_SKUAA = 1,
	HW_PLATFORM_SUBTYPE_SKUF = 2,
	HW_PLATFORM_SUBTYPE_1080P = 2,
	HW_PLATFORM_SUBTYPE_SKUAB = 3,
	HW_PLATFORM_SUBTYPE_1080P_EXT_BUCK = 3,
	HW_PLATFORM_SUBTYPE_QVGA = 4,
	HW_PLATFORM_SUBTYPE_SKUG = 5,
};

#endif
