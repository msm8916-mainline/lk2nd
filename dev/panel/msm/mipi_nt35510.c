/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
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

#include <stdint.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <debug.h>
#include <target/display.h>
#include <dev/gpio.h>

int mipi_nt35510_panel_dsi_config(int on)
{
	if (on) {
		gpio_config(96, GPIO_OUTPUT);
		/*
		 * As per the specification follow the sequence to put lcd
		 * backlight in one wire mode.
		 */
		gpio_set(96, 0x1);
		udelay(190);
		gpio_set(96, 0x0);
		udelay(286);
		gpio_set(96, 0x1);
		udelay(50);

		gpio_config(35, GPIO_OUTPUT);
		gpio_set(35, 0x1);

		gpio_config(40, GPIO_OUTPUT);
		gpio_set(40, 0x1);

		gpio_config(85, GPIO_OUTPUT);
		gpio_set(85, 0x1);
		mdelay(20);
		gpio_set(85, 0x0);
		mdelay(20);
		gpio_set(85, 0x1);
		mdelay(20);
	} else {
		if (!target_cont_splash_screen()) {
			gpio_set(96, 0x0);
			gpio_set(35, 0x0);
			gpio_set(40, 0x0);
		}
	}
	return 0;
}

