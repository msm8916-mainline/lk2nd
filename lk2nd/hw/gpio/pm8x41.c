// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <bits.h>
#include <debug.h>
#include <stdint.h>

#include <pm8x41.h>
#include <pm8x41_hw.h>

#include <lk2nd/hw/gpio.h>
#include <lk2nd/util/minmax.h>

#include "supplier.h"

int lk2nd_gpio_pmic_config(uint32_t num, int flags)
{
	struct pm8x41_gpio pm_gpio = {
		.function	= !!(flags & GPIOL_FLAGS_ASSERTED),
		.vin_sel	= PMIC_FLAGS_VIN_SEL(flags),
		.out_strength	= clamp(GPIOL_CONF_DRVSTR(flags), 1, 3),
	};

	if (flags & GPIO_PULL_UP)
		pm_gpio.pull = PM_GPIO_PULL_UP_1_5;
	else if (flags & GPIO_PULL_DOWN)
		pm_gpio.pull = PM_GPIO_PULLDOWN_10;
	else
		pm_gpio.pull = PM_GPIO_NO_PULL;

	if (!(flags & PMIC_NON_DEFAULT_VIN_SEL))
		pm_gpio.vin_sel = 2;

	if (flags & GPIO_SINGLE_ENDED) {
		if (flags & GPIO_LINE_OPEN_DRAIN)
			pm_gpio.output_buffer = PM_GPIO_OUT_DRAIN_NMOS;
		else
			pm_gpio.output_buffer = PM_GPIO_OUT_DRAIN_PMOS;
	}

	if ((flags & (GPIOL_FLAGS_IN|GPIOL_FLAGS_OUT)) == (GPIOL_FLAGS_IN|GPIOL_FLAGS_OUT))
		pm_gpio.direction = PM_GPIO_DIR_BOTH;
	else if (flags & GPIOL_FLAGS_OUT)
		pm_gpio.direction = PM_GPIO_DIR_OUT;
	else
		pm_gpio.direction = PM_GPIO_DIR_IN;

	return pm8x41_gpio_config_sid(PMIC_GPIO_NUM_SID(num), PMIC_GPIO_NUM_PIN(num), &pm_gpio);
}

#define OUT_LOW		0
#define OUT_HIGH	1
#define DIGITAL_INPUT	0
#define DIGITAL_OUTPUT	(1 << 4)

int lk2nd_gpio_pmic_set_dir(uint32_t num, bool oe, bool on)
{
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(PMIC_GPIO_NUM_PIN(num));
	uint8_t val = 0;

	gpio_base &= 0x0ffff;
	gpio_base |= (PMIC_GPIO_NUM_SID(num) << 16);

	if (oe)
		val = DIGITAL_OUTPUT | on;

	REG_WRITE(gpio_base + GPIO_MODE_CTL, val);

	return 0;
}

void lk2nd_gpio_pmic_set(uint32_t num, bool on)
{
	pm8x41_gpio_set_sid(PMIC_GPIO_NUM_SID(num), PMIC_GPIO_NUM_PIN(num), on);
}

bool lk2nd_gpio_pmic_get(uint32_t num)
{
	uint8_t status;

	pm8x41_gpio_get_sid(PMIC_GPIO_NUM_SID(num), PMIC_GPIO_NUM_PIN(num), &status);

	return status;
}

bool lk2nd_gpio_pmic_pon_get(uint32_t num)
{
	switch (PMIC_GPIO_NUM_PIN(num)) {
		case GPIO_PMIC_PWRKEY:
			return pm8x41_get_pwrkey_is_pressed();

		case GPIO_PMIC_RESIN:
			return pm8x41_resin_status();
	}

	dprintf(CRITICAL, "gpio_pm8x41: PON key %d is unknown.\n", PMIC_GPIO_NUM_PIN(num));
	return false;
}
