// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2025 Yang Xiwen <forbidden405@outlook.com> */

#include <debug.h>
#include <libfdt.h>

#include <lk2nd/hw/gpio.h>

#include "device.h"

static int lk2nd_leds_init(const void *dtb, int node)
{
	int i = 0, subnode, len;
	struct gpiol_desc desc;
	const char *state;

	dprintf(SPEW, " Initializing LEDs\n");
	dprintf(SPEW, " | label | gpio        | state |\n");

	fdt_for_each_subnode(subnode, dtb, node) {
		state = fdt_getprop(dtb, subnode, "default-state", &len);
		if (len < 0 || state[len - 1] != '\0') {
			dprintf(CRITICAL, "leds: Failed to get property \"default-state\". Broken dtb?\n");
			continue;
		}

		if (!strcmp(state, "on")) {
			gpiol_get(dtb, subnode, NULL, &desc, GPIOL_FLAGS_OUT_ASSERTED);
		} else if (!strcmp(state, "off")) {
			gpiol_get(dtb, subnode, NULL, &desc, GPIOL_FLAGS_OUT_DEASSERTED);
		} else if (!strcmp(state, "keep")) {
			/* do nothing */
		} else {
			dprintf(CRITICAL, "leds: Unknown \"default-state\" value: %s\n", state);
			continue;
		}

		i++;

		dprintf(SPEW, " | %5s | 0x%02x 0x%04x | %5s |\n",
			fdt_get_name(dtb, subnode, NULL), desc.dev, desc.pin, state);
	}
	dprintf(INFO, "leds: %d leds configured\n", i);

	if (subnode < 0 && subnode != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "leds: Failed to parse subnodes: %d\n", subnode);
		return subnode;
	}

	return 0;
}

LK2ND_DEVICE_INIT("gpio-leds", lk2nd_leds_init);
