// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2020-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <boot.h>
#include <debug.h>
#include <libfdt.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"

static const char *getprop_str(const void *dtb, int node, const char *prop, int *len)
{
	const char *val = fdt_getprop(dtb, node, prop, len);
	if (!val || *len < 1)
		return NULL;
	*len = strnlen(val, *len);
	return val;
}

static int lk2nd_panel_detect(const void *dtb, int node)
{
	struct lk2nd_panel *panel = &lk2nd_dev.panel;
	const char *old, *new, *ts;
	int old_len, new_len, ts_len;
	bool replace_mode;

	/* Should have been set from parsed command line or display driver */
	if (!panel->name) {
		dprintf(CRITICAL, "Failed to detect display panel\n");
		return 404;
	}

	old = getprop_str(dtb, node, "compatible", &old_len);
	if (!old || old_len < 0)
		return old_len;

	replace_mode = !!fdt_getprop(dtb, node, "replace-compatible", NULL);

	node = fdt_subnode_offset(dtb, node, panel->name);
	if (node < 0) {
		dprintf(CRITICAL, "Unsupported panel: %s\n", panel->name);
		return node;
	}

	new = getprop_str(dtb, node, "compatible", &new_len);
	if (!new || new_len < 0)
		return new_len;

	/* Include space required for null-terminators */
	panel->compatible_size = ++new_len + ++old_len;

	panel->compatible = malloc(panel->compatible_size);
	ASSERT(panel->compatible);
	panel->old_compatible = panel->compatible + new_len;

	strlcpy((char *)panel->compatible, new, new_len);
	strlcpy((char *)panel->old_compatible, old, old_len);

	if (replace_mode)
		panel->compatible_size = new_len;

	ts = fdt_getprop(dtb, node, "touchscreen-compatible", &ts_len);
	if (ts && ts_len > 0)
		panel->ts_compatible = strndup(ts, ts_len);

	return 0;
}
LK2ND_DEVICE_INIT("lk2nd,panel", lk2nd_panel_detect);

static int lk2nd_panel_dt_update(void *dtb, const char *cmdline,
				 enum boot_type boot_type)
{
	struct lk2nd_panel *panel = &lk2nd_dev.panel;
	int node, ret;

	if (!panel->compatible || boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	node = fdt_node_offset_by_compatible(dtb, -1, panel->old_compatible);
	if (node < 0) {
		dprintf(CRITICAL, "Failed to find panel node with compatible %s: %d\n",
			panel->old_compatible, node);
		return 0; /* Still continue boot */
	}

	ret = fdt_setprop(dtb, node, "compatible", panel->compatible,
			  panel->compatible_size);
	if (ret < 0)
		return ret;

	/* Enable associated touchscreen if any */
	if (panel->ts_compatible) {
		node = fdt_node_offset_by_compatible(dtb, -1, panel->ts_compatible);
		if (node < 0)
			return 0;

		ret = fdt_nop_property(dtb, node, "status");
		if (ret < 0) {
			dprintf(CRITICAL,
				"WARNING!!!!\n"
				"Failed to enable %s touchscreen: %d\n"
				"Please check your device tree.\n"
				"Maybe a status property is missing from touchscreen nodes.\n",
				panel->ts_compatible,
				ret);
			return 0;
		}
	}

	return 0;
}
DEV_TREE_UPDATE(lk2nd_panel_dt_update);
