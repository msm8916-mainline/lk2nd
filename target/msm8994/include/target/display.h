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
  "msmgpio", 78, 3, 1, 0, 1
};

static struct gpio_pin lcd_reg_en = {	/* boost regulator */
  "pm8994_gpios", 14, 3, 1, 0, 1
};

static struct gpio_pin bklt_gpio = {	/* lcd_bklt_reg_en */
  "pmi8994_gpios", 2, 3, 1, 0, 1
};

/*---------------------------------------------------------------------------*/
/* LDO configuration                                                         */
/*---------------------------------------------------------------------------*/
static struct ldo_entry ldo_entry_array[] = {
  { "vdd", 14, 0, 1800000, 100000, 100, 0, 20, 0, 0},
  { "vddio", 12, 0, 1800000, 100000, 100, 0, 20, 0, 0},
  { "vdda", 2, 1, 1250000, 100000, 100, 0, 0, 0, 0},
  { "vcca", 28, 1, 1000000, 10000, 100, 0, 0, 0, 0},
};

#define TOTAL_LDO_DEFINED 3

/*---------------------------------------------------------------------------*/
/* Target Physical configuration                                             */
/*---------------------------------------------------------------------------*/

static const uint32_t panel_strength_ctrl[] = {
  0x77, 0x06
};

static const char panel_bist_ctrl[] = {
  0x00, 0x00, 0xb1, 0xff, 0x00, 0x00
};

static const uint32_t panel_regulator_settings[] = {
  0x03, 0x05, 0x03, 0x00, 0x20, 0x07, 0x01
};

static const char panel_lane_config[] = {
0x02, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x01, 0x88,
0x02, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x01, 0x88,
0x02, 0x00, 0x00, 0x40, 0x20, 0x00, 0x00, 0x01, 0x88,
0x02, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x01, 0x88,
0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x01, 0x88
};

static const uint32_t panel_physical_ctrl[] = {
  0x5f, 0x00, 0x00, 0x10
};

/*---------------------------------------------------------------------------*/
/* Other Configuration                                                       */
/*---------------------------------------------------------------------------*/
#define DISPLAY_CMDLINE_PREFIX " mdss_mdp.panel="

#define MIPI_FB_ADDR  0x03400000

#define MIPI_HSYNC_PULSE_WIDTH       16
#define MIPI_HSYNC_BACK_PORCH_DCLK   32
#define MIPI_HSYNC_FRONT_PORCH_DCLK  76

#define MIPI_VSYNC_PULSE_WIDTH       2
#define MIPI_VSYNC_BACK_PORCH_LINES  2
#define MIPI_VSYNC_FRONT_PORCH_LINES 4

#define PWM_BL_LPG_CHAN_ID           4	/* lpg_out<3> */

#endif
