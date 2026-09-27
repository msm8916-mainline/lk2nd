// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <platform.h>
#include <string.h>

#include <libfdt.h>

#include "device.h"

static bool match_string(const char *s, const char *match, int len)
{
	if (!s || !match || len <= 0)
		return false;

	len = strnlen(match, len);
	if (len > 0) {
		if (match[len-1] == '*') {
			if (len > 1 && match[0] == '*')
				// *contains*
				return !!strstrn(s, match + 1, len - 2);

			// prefix* (starts with)
			len -= 2; // Don't match null terminator and '*'
		} else if (match[0] == '*') {
			// *suffix (ends with)
			int slen = strlen(s);
			if (slen < --len)
				return false;

			++match; // Skip '*'
			s += slen - len; // Move to end of string
		}
	}
	return strncmp(s, match, len + 1) == 0;
}

static bool match_panel(const void *dtb, int node, const char *panel_name)
{
	if (!panel_name)
		return false;

	node = fdt_subnode_offset(dtb, node, "panel");
	if (node < 0) {
		dprintf(CRITICAL, "No panels defined, cannot match\n");
		return false;
	}

	return fdt_subnode_offset(dtb, node, panel_name) >= 0;
}

static bool match_device_node(const void *dtb, int node)
{
	bool matched = false;
	const fdt32_t *num;
	const char *str;
	int len;

	str = fdt_getprop(dtb, node, "lk2nd,match-bootloader", &len);
	if (len != -FDT_ERR_NOTFOUND) {
		if (!match_string(lk2nd_dev.bootloader, str, len))
			return false;
		matched = true;
	}

	str = fdt_getprop(dtb, node, "lk2nd,match-cmdline", &len);
	if (len != -FDT_ERR_NOTFOUND) {
		if (!match_string(lk2nd_dev.cmdline, str, len))
			return false;
		matched = true;
	}

	str = fdt_getprop(dtb, node, "lk2nd,match-device", &len);
	if (len != -FDT_ERR_NOTFOUND) {
		if (!match_string(lk2nd_dev.device, str, len))
			return false;
		matched = true;
	}

	num = fdt_getprop(dtb, node, "lk2nd,match-machtype", &len);
	if (len != -FDT_ERR_NOTFOUND) {
		if (len != sizeof(*num) || board_machtype() != fdt32_to_cpu(*num))
			return false;
		matched = true;
	}

	fdt_getprop(dtb, node, "lk2nd,match-panel", &len);
	if (len != -FDT_ERR_NOTFOUND) {
		if (len < 0 || !match_panel(dtb, node, lk2nd_dev.panel.name))
			return false;
		matched = true;
	}

	return matched;
}

int lk2nd_device2nd_match_device_node(const void *dtb, int lk2nd_node)
{
	int node;

	fdt_for_each_subnode(node, dtb, lk2nd_node)
		if (match_device_node(dtb, node))
			return node;
	return node;
}
