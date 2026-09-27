// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <bits.h>
#include <debug.h>
#include <stdint.h>

#include <dev/pm8921.h>

#include <lk2nd/hw/gpio.h>
#include <lk2nd/util/minmax.h>

#include "supplier.h"

int lk2nd_gpio_pmic_config(uint32_t num, int flags)
{
	struct pm8921_gpio pm_gpio = {
		.function	= !!(flags & GPIOL_FLAGS_ASSERTED),
		.vin_sel	= PMIC_FLAGS_VIN_SEL(flags),
		.out_strength	= clamp(GPIOL_CONF_DRVSTR(flags), 0, 3),
	};

	if (flags & GPIO_PULL_UP)
		pm_gpio.pull = PM_GPIO_PULL_UP_30;
	else if (flags & GPIO_PULL_DOWN)
		pm_gpio.pull = PM_GPIO_PULL_DN;
	else
		pm_gpio.pull = PM_GPIO_PULL_NO;

	if (!(flags & PMIC_NON_DEFAULT_VIN_SEL))
		pm_gpio.vin_sel = 2;

	if (flags & GPIO_SINGLE_ENDED) {
		if (flags & GPIO_LINE_OPEN_DRAIN) {
			pm_gpio.output_buffer = PM_GPIO_OUT_BUF_OPEN_DRAIN;
		} else {
			dprintf(CRITICAL, "gpio_pm8921: LINE_OPEN_SOURCE is not supported!\n");
			return -1;
		}
	}

	if ((flags & (GPIOL_FLAGS_IN|GPIOL_FLAGS_OUT)) == (GPIOL_FLAGS_IN|GPIOL_FLAGS_OUT))
		pm_gpio.direction = PM_GPIO_DIR_BOTH;
	else if (flags & GPIOL_FLAGS_OUT)
		pm_gpio.direction = PM_GPIO_DIR_OUT;
	else
		pm_gpio.direction = PM_GPIO_DIR_IN;

	return pm8921_gpio_config(PMIC_GPIO_NUM_PIN(num), &pm_gpio);
}

int lk2nd_gpio_pmic_set_dir(uint32_t num, bool oe, bool on)
{
	dprintf(CRITICAL, "gpio_pm8921: set_dir is not supported!\n");
	return -1;
}

void lk2nd_gpio_pmic_set(uint32_t num, bool on)
{
	dprintf(CRITICAL, "gpio_pm8921: set is not supported!\n");
}

bool lk2nd_gpio_pmic_get(uint32_t num)
{
	uint8_t status;

	pm8921_gpio_get(PMIC_GPIO_NUM_PIN(num), &status);

	return status;
}

bool lk2nd_gpio_pmic_pon_get(uint32_t num)
{
	uint8_t status = 0;

	switch (PMIC_GPIO_NUM_PIN(num)) {
		case GPIO_PMIC_PWRKEY:
			pm8921_pwrkey_status(&status);
			return status;
	}

	dprintf(CRITICAL, "gpio_pm8921: PON key %d is unknown.\n", PMIC_GPIO_NUM_PIN(num));
	return false;
}
