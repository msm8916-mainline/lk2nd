// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2026 Barnabas Czeman */

#include <debug.h>
#include <libfdt.h>
#include <regulator.h>

#include "device.h"

static int lk2nd_regulator_init(const void *dtb, int node)
{
	int regulator, len;
	const uint32* val;
	const char* name;

	name = fdt_get_name(dtb, node, NULL);

	val = fdt_getprop(dtb, node, "id", &len);
	if (len < 0) {
		dprintf(CRITICAL, "Failed to get id for %s: %d\n", name, len);
		return 0;
	}
	regulator = fdt32_to_cpu(*val);

	dprintf(INFO, "Enable %s: %d\n", name, regulator);
	regulator_enable(regulator);

	return 0;
}

LK2ND_DEVICE_INIT("regulator-ldo", lk2nd_regulator_init);
