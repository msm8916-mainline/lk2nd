// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <dev/keys.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <target.h>

#include <libfdt.h>

#include <lk2nd/device/keys.h>
#include <lk2nd/hw/gpio.h>
#include <lk2nd/init.h>

#include "device.h"

#define LK2ND_MAX_KEYS 4

struct lk2nd_keymap {
	uint32_t keycode;
	struct gpiol_desc gpio;
};
static struct lk2nd_keymap keymap[LK2ND_MAX_KEYS] = {0};

static bool target_powerkey(void)
{
	extern uint32_t pm8x41_get_pwrkey_is_pressed(void);

	if (!IS_ENABLED(WITH_DEV_PMIC_PM8X41) && !IS_ENABLED(WITH_DEV_PMIC_PM8921))
		return false;
	return pm8x41_get_pwrkey_is_pressed();
}

bool lk2nd_keys_pressed(uint32_t keycode)
{
	bool found = false;
	int i;

	for (i = 0; i < LK2ND_MAX_KEYS; ++i) {
		if (!keymap[i].keycode)
			break;

		if (keymap[i].keycode == keycode) {
			found = true;
			if (gpiol_is_asserted(keymap[i].gpio))
				return true;
		}
	}

	if (found)
		return false;

	/* fallback to default handlers only if the keycode wasn't set in the DT */
	switch (keycode) {
		case KEY_VOLUMEDOWN:
			return target_volume_down();
		case KEY_VOLUMEUP:
			return target_volume_up();
		case KEY_POWER:
			return target_powerkey();
		default:
			return false;
	}
}

static const uint16_t published_keys[] = {
	KEY_VOLUMEUP,
	KEY_VOLUMEDOWN,
	KEY_POWER,
	KEY_HOME,
};

static void lk2nd_keys_publish(void)
{
	unsigned int i;

	keys_init();

	for (i = 0; i < ARRAY_SIZE(published_keys); ++i) {
		keys_post_event(published_keys[i],
				lk2nd_keys_pressed(published_keys[i]));
	}
}
LK2ND_INIT(lk2nd_keys_publish);

static int lk2nd_keys_init(const void *dtb, int node)
{
	int i = 0, subnode, keycode, len, ret;
	struct gpiol_desc gpio;
	const uint32_t *val;

	dprintf(SPEW, " | label | code  | gpio        |\n");

	fdt_for_each_subnode(subnode, dtb, node) {
		if (i == LK2ND_MAX_KEYS) {
			dprintf(CRITICAL, "keys: Too many keys defined, ignoring rest\n");
			break;
		}

		ret = gpiol_get(dtb, subnode, NULL, &gpio, GPIOL_FLAGS_IN);
		if (ret) {
			dprintf(CRITICAL, "keys: Failed to get gpio for %s: %d\n",
				fdt_get_name(dtb, subnode, NULL), ret);
			continue;
		}

		val = fdt_getprop(dtb, subnode, "lk2nd,code", &len);
		if (len < 0) {
			dprintf(CRITICAL, "keys: Failed to get keycode for %s: %d\n",
				fdt_get_name(dtb, subnode, NULL), len);
			continue;
		}
		keycode = fdt32_to_cpu(*val);

		dprintf(SPEW, " | %5s | %5d | 0x%02x 0x%04x |\n",
			fdt_get_name(dtb, subnode, NULL), keycode, gpio.dev, gpio.pin);

		keymap[i].keycode = keycode;
		keymap[i].gpio = gpio;
		i++;
	}
	dprintf(INFO, "keys: %d keymap overrides applied\n", i);

	if (subnode < 0 && subnode != -FDT_ERR_NOTFOUND) {
		dprintf(CRITICAL, "keys: Failed to parse subnodes: %d\n", subnode);
		return subnode;
	}

	return 0;
}
LK2ND_DEVICE_INIT("gpio-keys", lk2nd_keys_init);
