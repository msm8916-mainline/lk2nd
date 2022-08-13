/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 * Copyright (c) 2009-2013, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
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

#include <debug.h>

#include <dev/fbcon.h>
#include <dev/lcdc.h>
#include <platform.h>
#include <platform/clock.h>
#include <platform/machtype.h>
#include <platform/pmic.h>

struct fbcon_config *lcdc_init(void);

void target_display_init(const char *panel_name)
{
	struct fbcon_config *fb_cfg;
#if DISPLAY_TYPE_LCDC
	struct lcdc_timing_parameters *lcd_timing;
	mdp_clock_init();
	if (board_machtype() == LINUX_MACHTYPE_8660_FLUID) {
		mmss_pixel_clock_configure(PIXEL_CLK_INDEX_25M);
	} else {
		mmss_pixel_clock_configure(PIXEL_CLK_INDEX_54M);
	}
	lcd_timing = get_lcd_timing();
	fb_cfg = lcdc_init_set(lcd_timing);
	fbcon_setup(fb_cfg);
	fbcon_clear();
	panel_poweron();
#endif
#if DISPLAY_TYPE_MIPI
	mdp_clock_init();
	configure_dsicore_dsiclk();
	configure_dsicore_byteclk();
	configure_dsicore_pclk();

	fb_cfg = mipi_init();
	fbcon_setup(fb_cfg);
#endif
#if DISPLAY_TYPE_HDMI
	struct hdmi_disp_mode_timing_type *hdmi_timing;
	mdp_clock_init();
	hdmi_power_init();
	fb_cfg = get_fbcon();
	hdmi_set_fb_addr(fb_cfg.base);
	fbcon_setup(fb_cfg);
	hdmi_dtv_init();
	hdmi_dtv_on();
	hdmi_msm_turn_on();
#endif
}

void target_display_shutdown(void)
{
#if DISPLAY_TYPE_LCDC
	unsigned rc = 0;
	/* Turning off LCDC */
	rc = panel_set_backlight(0);
	if (rc)
		dprintf(CRITICAL, "Error in setting panel backlight\n");
	lcdc_shutdown();
	pm8901_ldo_disable(LDO_L2);
#endif
#if DISPLAY_TYPE_MIPI
	mipi_dsi_shutdown();
#endif
#if DISPLAY_TYPE_HDMI
	hdmi_display_shutdown();
#endif
}
