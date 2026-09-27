// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2021-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <printf.h>

#include <dev/fbcon.h>
#include <fastboot.h>

extern void *rgb565_to_rgb888(void *out, const void *in, uint32_t npixels);
extern void *rgb888_swap(void *out, const void *in, uint32_t npixels);
extern void *rgb8888_swap_to_rgb888(void *out, const void *in, uint32_t npixels);

static void cmd_oem_screenshot(const char *arg, void *data, unsigned sz)
{
	struct fbcon_config *fb = fbcon_display();
	unsigned hdr;
	void *end;

	if (!fb) {
		fastboot_fail("display not initialized");
		return;
	}

	sz = fb->width * fb->height;
	if (sz % sizeof(uint64_t) != 0) {
		fastboot_fail("unsupported display resolution");
		return;
	}

	/* PPM image header, see http://netpbm.sourceforge.net/doc/ppm.html */
	hdr = sprintf(data, "P6\n\n%7u %7u\n255\n", fb->width, fb->height);
	ASSERT(hdr % sizeof(uint64_t) == 0);

	/* Convert to RGB888, swap to change color order for PPM */
	switch (fb->bpp) {
	case 16:
		end = rgb565_to_rgb888(data + hdr, fb->base, sz);
		break;
	case 24:
		end = rgb888_swap(data + hdr, fb->base, sz);
		break;
	case 32:
		end = rgb8888_swap_to_rgb888(data + hdr, fb->base, sz);
		break;
	default:
		fastboot_fail("unsupported display bpp");
		return;
	}

	fastboot_stage(data, end - data);
}
FASTBOOT_REGISTER("oem screenshot", cmd_oem_screenshot);
