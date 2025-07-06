// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <dev_tree.h>
#include <fastboot.h>
#include <string.h>

#include <libfdt.h>
#include <lk2nd/device.h>
#include <lk2nd/init.h>
#include <lk2nd/panel.h>
#include <lk2nd/util/lkfdt.h>

#include "device.h"

struct lk2nd_device lk2nd_dev;

/**
 * lk2nd_device_get_dtb_hints() - Get a null-terminated array of DTB names.
 */
const char *const *lk2nd_device_get_dtb_hints(void)
{
	return lk2nd_dev.dtbfiles;
}

static int find_device_node(const void *dtb)
{
	int lk2nd_node, node, ret;

	lk2nd_node = fdt_path_offset(dtb, "/lk2nd");
	if (lk2nd_node < 0)
		return lk2nd_node;

	if (lk2nd_dev.compatible) {
		/* Check if root node is already compatible */
		ret = fdt_node_check_compatible(dtb, lk2nd_node, lk2nd_dev.compatible);
		if (ret == 0)
			return lk2nd_node;
		if (ret == 1) /* root node has "compatible" but is incompatible */
			return -FDT_ERR_NOTFOUND;

		/* Search in subnodes instead */
		fdt_for_each_subnode(node, dtb, lk2nd_node)
			if (fdt_node_check_compatible(dtb, node, lk2nd_dev.compatible) == 0)
				return node;
		return node;
	}

	/* If root node already has a "compatible" it always matches */
	if (fdt_getprop(dtb, lk2nd_node, "compatible", NULL))
		return lk2nd_node;

	/* Fallback to dynamic matching */
	return lk2nd_device2nd_match_device_node(dtb, lk2nd_node);
}

static bool do_device_init(const void *dtb, int node,
			   const struct lk2nd_device_init *di)
{
	int ret = fdt_node_check_compatible(dtb, node, di->compatible);
	switch (ret) {
	case 0:
		ret = di->init(dtb, node);
		if (ret)
			dprintf(CRITICAL, "lk2nd device init for %s failed: %d\n",
				di->compatible, ret);
		return true;
	case 1:
		return false;	/* Not compatible */
	case -FDT_ERR_NOTFOUND:
		return true;	/* No compatible */
	default:
		dprintf(CRITICAL, "Failed to check lk2nd device init compatible: %d\n", ret);
		return true;
	}
}

static void device_init(const void *dtb, int device_node)
{
	extern const struct lk2nd_device_init __lk2nd_device_init_start;
	extern const struct lk2nd_device_init __lk2nd_device_init_end;
	int node;

	if (&__lk2nd_device_init_start == &__lk2nd_device_init_end)
		return; /* No initializers */

	fdt_for_each_subnode(node, dtb, device_node) {
		const struct lk2nd_device_init *di;
		for (di = &__lk2nd_device_init_start; di < &__lk2nd_device_init_end; ++di)
			if (do_device_init(dtb, node, di))
				break;
	}
	if (node < 0 && node != -FDT_ERR_NOTFOUND)
		dprintf(CRITICAL, "Failed to check lk2nd device subnodes: %d\n", node);
}

static void parse_dtb(const void *dtb)
{
	int node, len;
	const char *val;

	node = find_device_node(dtb);
	if (node < 0) {
		dprintf(CRITICAL, "Failed to find matching lk2nd device node: %d\n", node);
		return;
	}

	val = fdt_getprop(dtb, node, "compatible", &len);
	if (val && len > 0)
		lk2nd_dev.compatible = strndup(val, len);
	else
		dprintf(CRITICAL, "Failed to read 'compatible': %d\n", len);

	val = fdt_getprop(dtb, node, "model", &len);
	if (val && len > 0)
		lk2nd_dev.model = strndup(val, len);
	else
		dprintf(CRITICAL, "Failed to read 'model': %d\n", len);

	lk2nd_dev.dtbfiles = (const char *const *)
		lkfdt_stringlist_get_all(dtb, node, "lk2nd,dtb-files", &len);
	if (len < 0)
		dprintf(CRITICAL, "Failed to read 'lk2nd,dtb-files': %d\n", len);

	val = fdt_getprop(dtb, node, "lk2nd,single-key-navigation", &len);
	if (len >= 0)
		lk2nd_dev.single_key = true;

	val = fdt_stringlist_get(dtb, node, "lk2nd,menu-key-strings", 0, &len);
	if (val && len > 0)
		lk2nd_dev.menu_keys.navigate = strndup(val, len);

	val = fdt_stringlist_get(dtb, node, "lk2nd,menu-key-strings", 1, &len);
	if (val && len > 0)
		lk2nd_dev.menu_keys.select = strndup(val, len);

	dprintf(INFO, "Detected device: %s (compatible: %s)\n",
		lk2nd_dev.model, lk2nd_dev.compatible);

#if DEVICE_TREE
	if (!dev_tree_override_match(dtb, node))
		/* Try again on root node */
		dev_tree_override_match(dtb, 0);
#endif

	device_init(dtb, node);
}

#ifdef LK2ND_BUNDLE_DTB
INCFILE(lk2nd_bundled_dtb, lk2nd_bundled_dtb_size, LK2ND_BUNDLE_DTB);
#endif

static void lk2nd_device_init(void)
{
	const void *dtb;

#ifdef LK2ND_COMPATIBLE
	lk2nd_dev.compatible = LK2ND_COMPATIBLE;
#endif
#ifdef LK2ND_DISPLAY
	lk2nd_dev.panel.name = lk2nd_oem_panel_name();
#endif

	dtb = lk2nd_device2nd_init();
#ifdef LK2ND_BUNDLE_DTB
	dtb = lk2nd_bundled_dtb;
#endif
	if (!dtb) {
		dprintf(CRITICAL, "No device DTB provided\n");
		return;
	}

	lk2nd_dev.dtb = dtb;
	parse_dtb(dtb);
}
LK2ND_INIT(lk2nd_device_init);

static unsigned char *concat_cmdline(const char *a, const char *b)
{
	int lenA = strlen(a), lenB = strlen(b);
	unsigned char *r = malloc(lenA + lenB + 2); /* space and null terminator */

	ASSERT(r);
	memcpy(r, a, lenA);
	r[lenA] = ' ';
	memcpy(r + lenA + 1, b, lenB + 1);
	return r;
}

unsigned char *lk2nd_device_update_cmdline(const char *cmdline, enum boot_type boot_type)
{
#ifdef LK2ND_COMPATIBLE
	/*
	 * Pass on the hardcoded compatible to the next lk2nd. Note that this
	 * replaces the original cmdline in the Android boot image, but it
	 * should be just a simple "lk2nd" anyway.
	 */
	if (boot_type & BOOT_LK2ND)
		cmdline = "lk2nd.compatible=" LK2ND_COMPATIBLE;
#endif
#if WITH_LK2ND_DEVICE_2ND
	if (lk2nd_dev.cmdline)
		return concat_cmdline(cmdline, lk2nd_dev.cmdline);
#endif
	return update_cmdline(cmdline);
}

static void lk2nd_device_fastboot_register(void)
{
	if (lk2nd_dev.compatible)
		fastboot_publish("lk2nd:compatible", lk2nd_dev.compatible);
	if (lk2nd_dev.model)
		fastboot_publish("lk2nd:model", lk2nd_dev.model);
	if (lk2nd_dev.battery)
		fastboot_publish("lk2nd:battery", lk2nd_dev.battery);
	if (lk2nd_dev.panel.name)
		fastboot_publish("lk2nd:panel", lk2nd_dev.panel.name);
}
FASTBOOT_INIT(lk2nd_device_fastboot_register);
