// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <string.h>

#include <libfdt.h>

#include "device.h"

static bool match_string(const char *s, const char *match, size_t len)
{
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
			size_t slen = strlen(s);
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
	node = fdt_subnode_offset(dtb, node, "panel");
	if (node < 0) {
		dprintf(CRITICAL, "No panels defined, cannot match\n");
		return false;
	}

	return fdt_subnode_offset(dtb, node, panel_name) >= 0;
}

static bool match_device_node(const void *dtb, int node)
{
	int len;
	const char *val;

	val = fdt_getprop(dtb, node, "lk2nd,match-cmdline", &len);
	if (val && len > 0) {
		if (!lk2nd_dev.cmdline)
			return false;
		return match_string(lk2nd_dev.cmdline, val, len);
	}

	val = fdt_getprop(dtb, node, "lk2nd,match-bootloader", &len);
	if (val && len > 0) {
		if (!lk2nd_dev.bootloader)
			return false;
		return match_string(lk2nd_dev.bootloader, val, len);
	}

	val = fdt_getprop(dtb, node, "lk2nd,match-device", &len);
	if (val && len > 0) {
		if (!lk2nd_dev.device)
			return false;
		return match_string(lk2nd_dev.device, val, len);
	}

	fdt_getprop(dtb, node, "lk2nd,match-panel", &len);
	if (len >= 0) {
		if (!lk2nd_dev.panel.name)
			return false;
		return match_panel(dtb, node, lk2nd_dev.panel.name);
	}

	return false;
}

int lk2nd_device2nd_match_device_node(const void *dtb, int lk2nd_node)
{
	int node;

	fdt_for_each_subnode(node, dtb, lk2nd_node)
		if (match_device_node(dtb, node))
			return node;
	return node;
}
