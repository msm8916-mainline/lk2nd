// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023, Nikita Travkin <nikita@trvn.ru> */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <dev/fbcon.h>
#include <boot.h>
#include <libfdt.h>
#include <target.h>

#include <lk2nd/util/cmdline.h>
#include <lk2nd/util/lkfdt.h>

#include "cont-splash/cont-splash.h"

/*
 * simplefb.c - Pass lk2nd's framebuffer to the OS.
 *
 * It might be useful to pass already enabled framebuffer to
 * the next OS. This module creates/fills simple-framebffer
 * node if this is requested on the cmdline.
 */

static int lk2nd_simplefb_dt_update(void *dtb, const char *cmdline,
				    enum boot_type boot_type)
{
	struct fbcon_config *fb = fbcon_display();
	int ret, resmem_offset, chosen_offset, offset;
	uint32_t mem_ph, fb_size;
	char tmp[32], args[64];
	void *rel_base;

	if (!fb)
		return 0;

	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	if (!lk2nd_cmdline_scan_arg(cmdline, "lk2nd.pass-simplefb", args, sizeof(args)))
		return 0;

	if (IS_ENABLED(LK2ND_DISPLAY_CONT_SPLASH)) {
		if (strstr(args, "autorefresh")) {
			dprintf(INFO, "simplefb: Enabling autorefresh\n");
			mdp_enable_autorefresh(fb);
		}

		if (strstr(args, "relocate")) {
			rel_base = target_get_scratch_address()
				+ target_get_max_flash_size()
				- (10 * 1024 * 1024); /* 8MiB~=fhd 32bpp, +512k ramoops at the end. */

			dprintf(INFO, "simplefb: Framebuffer will be relocated to 0x%x\n", (uint32_t)rel_base);
			mdp_relocate(fb, rel_base);
		}

		if (strstr(args, "rgb565"))
			mdp_set_rgb565(fb);
		else if (strstr(args, "xrgb8888"))
			mdp_set_xrgb8888(fb);
	}

	fb_size = fb->stride * fb->bpp/8 * fb->height;

	resmem_offset = fdt_path_offset(dtb, "/reserved-memory");
	if (resmem_offset < 0)
		return 0;

	// TODO: Probably want to check if the node already exists
	snprintf(tmp, sizeof(tmp), "cont-splash@%08x", (uint32_t)fb->base);
	offset = fdt_add_subnode(dtb, resmem_offset, tmp);
	if (offset < 0)
		return 0;

	ret = fdt_appendprop_addrrange(dtb, resmem_offset, offset, "reg", (uint32_t)fb->base, fb_size);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_empty(dtb, offset, "no-map");
	if (ret < 0)
		return 0;

	mem_ph = lkfdt_get_phandle(dtb, offset);
	if (!mem_ph)
		return 0;

	chosen_offset = fdt_path_offset(dtb, "/chosen");
	if (chosen_offset < 0)
		return 0;

	if (!fdt_getprop(dtb, chosen_offset, "#address-cells", &ret) && ret == -FDT_ERR_NOTFOUND) {
		ret = fdt_setprop_u32(dtb, chosen_offset, "#address-cells", fdt_address_cells(dtb, 0));
		if (ret < 0)
			return 0;
	}

	if (!fdt_getprop(dtb, chosen_offset, "#size-cells", &ret) && ret == -FDT_ERR_NOTFOUND) {
		ret = fdt_setprop_u32(dtb, chosen_offset, "#size-cells", fdt_size_cells(dtb, 0));
		if (ret < 0)
			return 0;
	}

	ret = fdt_setprop_empty(dtb, chosen_offset, "ranges");
	if (ret < 0)
		return 0;

	offset = fdt_node_offset_by_compatible(dtb, -1, "simple-framebuffer");
	if (offset < 0 && offset != -FDT_ERR_NOTFOUND)
		return 0;

	if (offset == -FDT_ERR_NOTFOUND) {
		snprintf(tmp, sizeof(tmp), "framebuffer@%08x", (uint32_t)fb->base);
		offset = fdt_add_subnode(dtb, chosen_offset, tmp);
		if (offset < 0)
			return 0;

		ret = fdt_setprop_string(dtb, offset, "compatible", "simple-framebuffer");
		if (ret < 0)
			return 0;
	}

	ret = fdt_setprop_empty(dtb, offset, "reg");
	if (ret < 0)
		return 0;

	ret = fdt_appendprop_addrrange(dtb, chosen_offset, offset, "reg", (uint32_t)fb->base, fb_size);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "memory-region", mem_ph);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "width", fb->width);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "height", fb->height);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "stride", fb->stride * fb->bpp/8);
	if (ret < 0)
		return 0;

	switch (fb->bpp) {
		case 16:
			ret = fdt_setprop_string(dtb, offset, "format", "r5g6b5");
			break;

		case 24:
			ret = fdt_setprop_string(dtb, offset, "format", "r8g8b8");
			break;

		case 32:
			ret = fdt_setprop_string(dtb, offset, "format", "x8r8g8b8");
			break;

		default:
			return 0;
	}
	if (ret < 0)
		return 0;

	ret = fdt_setprop_string(dtb, offset, "status", "okay");
	if (ret < 0)
		return 0;

	return 0;
}
DEV_TREE_UPDATE(lk2nd_simplefb_dt_update);
