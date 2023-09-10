// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022-2023, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <dev/fbcon.h>
#include <mdp4.h>
#include <string.h>
#include <target/display.h>

#include <lk2nd/init.h>

#include "device.h"

static void mdss_spi_flush(void)
{
	mdss_spi_on(NULL, fbcon_display());
}

static void lk2nd_device2nd_init_spi_display(void)
{
	static struct fbcon_config fb;
	int ret;

	if (strcmp(lk2nd_dev.panel.intf, "spi"))
		return;

	if (!lk2nd_dev.panel.initialized) {
		dprintf(CRITICAL, "SPI display not initialized by previous bootloader\n");
		return;
	}

	/*
	 * FIXME: For now we assume that all devices with SPI display use the
	 * same configuration, since this is mainly used by feature phones with
	 * similar display panel configurations. This might need additional
	 * configuration (e.g. via the device tree) in the future.
	 */
	fb.base = (void*)MIPI_FB_ADDR;
	fb.width = 240;
	fb.height = 320;
	fb.stride = fb.width;
	fb.bpp = 16;
	fb.format = FB_FORMAT_RGB565;
	fb.update_start = mdss_spi_flush;

	ret = mdss_spi_init();
	if (ret) {
		dprintf(CRITICAL, "Failed to initialize SPI bus: %d\n", ret);
		return;
	}

	fbcon_setup(&fb);
}
LK2ND_INIT(lk2nd_device2nd_init_spi_display);
