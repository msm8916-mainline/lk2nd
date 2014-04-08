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

#ifndef _TARGET_MSM8610_DISPLAY_H
#define _TARGET_MSM8610_DISPLAY_H
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include <display_resource.h>

/*---------------------------------------------------------------------------*/
/* GPIO configuration                                                        */
/*---------------------------------------------------------------------------*/
static struct gpio_pin reset_gpio = {
  "msmgpio", 41, 3, 1, 0, 1
};

static struct gpio_pin enable_gpio = {
  0, 0, 0, 0, 0, 0
};

static struct gpio_pin te_gpio = {
  "msmgpio", 12, 0, 2, 0, 1
};

static struct gpio_pin pwm_gpio = {
  0, 0, 0, 0, 0, 0
};

static struct gpio_pin mode_gpio = {
  "msmgpio", 7, 3, 1, 0, 1
};


/*---------------------------------------------------------------------------*/
/* Supply configuration                                                      */
/*---------------------------------------------------------------------------*/
static struct ldo_entry ldo_entry_array[] = {
{ "vddio", 14, 0, 1800000, 100000, 100, 0, 0, 0, 0},
{ "vdda", 19, 0, 2850000, 100000, 100, 0, 0, 0, 0},
};

#define TOTAL_LDO_DEFINED 2

/*---------------------------------------------------------------------------*/
/* Target Physical configuration                                             */
/*---------------------------------------------------------------------------*/

static const uint32_t panel_strength_ctrl[] = {
  0xff, 0x06
};

static const char panel_bist_ctrl[] = {
  0x03, 0x03, 0x00, 0x00, 0x0f, 0x00
};

static const uint32_t panel_regulator_settings[] = {
  0x02, 0x08, 0x05, 0x00, 0x20, 0x03, 0x00
};

static const char panel_lane_config[] = {
	0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
	0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
	0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
	0x80, 0x45, 0x00, 0x00, 0x00, 0x01, 0x66, 0x00, 0x00,
	0x40, 0x67, 0x00, 0x00, 0x00, 0x01, 0x88, 0x00, 0x00
};

static const uint32_t panel_physical_ctrl[] = {
  0x7f, 0x00, 0x00, 0x00
};

/*---------------------------------------------------------------------------*/
/* Other Configuration                                                       */
/*---------------------------------------------------------------------------*/
#define DISPLAY_CMDLINE_PREFIX " mdss_mdp3.panel="

#define MIPI_FB_ADDR  0x03200000

#define MIPI_HSYNC_PULSE_WIDTH       8
#define MIPI_HSYNC_BACK_PORCH_DCLK   40
#define MIPI_HSYNC_FRONT_PORCH_DCLK  160

#define MIPI_VSYNC_PULSE_WIDTH       2
#define MIPI_VSYNC_BACK_PORCH_LINES  10
#define MIPI_VSYNC_FRONT_PORCH_LINES 12

#endif
