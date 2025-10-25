// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2025 exkc <exkc@exkc.moe> */

#include <debug.h>
#include <libfdt.h>

#include <lk2nd/hw/gpio.h>

#include "device.h"

static int lk2nd_regulator_init(const void *dtb, int node)
{
	int ret;

	struct gpiol_desc desc;
	dprintf(SPEW, " Initializing regulator-fixed for %s",
			fdt_get_name(dtb, node, NULL));
	ret = 	gpiol_get(dtb, node, NULL, &desc, GPIOL_FLAGS_OUT_ASSERTED);

		if (ret) {
			dprintf(CRITICAL, "regulator-gpio: Failed to get gpio for %s: %d\n",
				fdt_get_name(dtb, node, NULL), ret);
		}

	dprintf(SPEW, " | label | gpio        | active_low |\n");
	dprintf(SPEW, " | %5s | 0x%02x 0x%04x | %d          |\n",
			fdt_get_name(dtb, node, NULL), desc.dev, desc.pin, desc.active_low);
	
	return 0;
}

LK2ND_DEVICE_INIT("regulator-fixed", lk2nd_regulator_init);
