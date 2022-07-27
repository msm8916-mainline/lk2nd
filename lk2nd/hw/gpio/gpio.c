// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <stdint.h>
#include <string.h>
#include <libfdt.h>

#include <lk2nd/hw/gpio.h>

#define LK2ND_GPIOL_PROP_LEN 32

#define GPIOL_RESERVED_FLAGS_MASK 0x00ff0000

int gpiol_get(const void *dtb, int node, const char *name,
	      struct gpiol_desc *desc, uint32_t flags)
{
	char prop_name[LK2ND_GPIOL_PROP_LEN + sizeof("-gpios")] = "gpios";
	struct gpiol_desc pin = {0};
	const fdt32_t *prop_val;
	uint32_t config;
	int len, ret;

	if (name) {
		strncpy(prop_name, name, LK2ND_GPIOL_PROP_LEN);
		strcat(prop_name, "-gpios");
	}

	prop_val = fdt_getprop(dtb, node, prop_name, &len);
	if (len < 0) {
		dprintf(CRITICAL, "gpiol_get failed: Unable to get property: %d\n", len);
		return len;
	} else if (len != 3 * sizeof(*prop_val)) {
		dprintf(CRITICAL, "gpiol_get failed: gpios arrays are not supported. (len=%d)\n", len);
		return -1;
	}

	pin.pin = fdt32_to_cpu(prop_val[1]);
	pin.dev = fdt32_to_cpu(prop_val[0]) & 0xff;
	config = fdt32_to_cpu(prop_val[2]);
	config &= ~GPIOL_RESERVED_FLAGS_MASK;

	ret = gpiol_set_config(&pin, config | (flags & GPIOL_RESERVED_FLAGS_MASK));
	if (ret < 0) {
		dprintf(CRITICAL, "gpiol_get failed: Unable to configure gpio: %d\n", ret);
		return ret;
	}

	*desc = pin;
	return 0;
}

int gpiol_direction_input(struct gpiol_desc desc)
{
	switch (desc.dev) {
	default:
		dprintf(CRITICAL, "%s: device %d is not known.\n", __func__, desc.dev);
		return -1;
	}
}

int gpiol_direction_output(struct gpiol_desc desc, bool value)
{
	if (desc.active_low)
		value = !value;

	switch (desc.dev) {
	default:
		dprintf(CRITICAL, "%s: device %d is not known.\n", __func__, desc.dev);
		return -1;
	}
}

bool gpiol_is_asserted(struct gpiol_desc desc)
{
	bool val;

	switch (desc.dev) {
	default:
		dprintf(CRITICAL, "%s: device %d is not known.\n", __func__, desc.dev);
		return false;
	}

	return desc.active_low ? !val : val;
}

void gpiol_set_asserted(struct gpiol_desc desc, bool value)
{
	if (desc.active_low)
		value = !value;

	switch (desc.dev) {
	default:
		dprintf(CRITICAL, "%s: device %d is not known.\n", __func__, desc.dev);
		return;
	}
}

int gpiol_set_config(struct gpiol_desc *desc, uint32_t config)
{
	if (config & GPIO_ACTIVE_LOW)
		desc->active_low = 1;

	switch (desc->dev) {
	default:
		dprintf(CRITICAL, "%s: device %d is not known.\n", __func__, desc->dev);
		return -1;
	}
}
