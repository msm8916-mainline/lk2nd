// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <bits.h>
#include <debug.h>
#include <platform/iomap.h>
#include <reg.h>
#include <stdint.h>

#include <lk2nd/hw/gpio.h>
#include <lk2nd/util/minmax.h>

#include "supplier.h"

enum tlmm_pull {
	TLMM_NO_PULL,
	TLMM_PULL_DOWN,
	TLMM_KEEPER,
	TLMM_PULL_UP,
};

#define TLMM_DRIVE_STRENGTH(n)	(clamp(n, 2, 16) / 2 - 1)

#define TLMM_IN		BIT(0)
#define TLMM_OUT	BIT(1)

#define TLMM_CFG(pull, func, drv_str, oe, hihys) \
	((pull) | (func) << 2 | (drv_str) << 6 | (oe) << 9 | (hihys) << 10)

#define TLMM_CFG_OE	BIT(9)

int lk2nd_gpio_tlmm_config(uint32_t num, int flags)
{
	uint32_t val, pull;

	if (flags & GPIO_PULL_UP)
		pull = TLMM_PULL_UP;
	else if (flags & GPIO_PULL_DOWN)
		pull = TLMM_PULL_DOWN;
	else
		pull = TLMM_NO_PULL;

	val = TLMM_CFG(pull, 0, TLMM_DRIVE_STRENGTH(GPIOL_CONF_DRVSTR(flags)),
		       !!(flags & GPIOL_FLAGS_OUT), 0);

	lk2nd_gpio_tlmm_set(num, !!(flags & GPIOL_FLAGS_ASSERTED));
	writel(val, GPIO_CONFIG_ADDR(num));

	return 0;
}

void lk2nd_gpio_tlmm_output_enable(uint32_t num, bool oe)
{
	uint32_t val = readl(GPIO_CONFIG_ADDR(num));

	if (oe)
		val |= TLMM_CFG_OE;
	else
		val &= ~TLMM_CFG_OE;

	writel(val, GPIO_CONFIG_ADDR(num));
}

void lk2nd_gpio_tlmm_set(uint32_t num, bool on)
{
	writel(on ? TLMM_OUT : 0, GPIO_IN_OUT_ADDR(num));
}

bool lk2nd_gpio_tlmm_get(uint32_t num)
{
	return readl(GPIO_IN_OUT_ADDR(num)) & TLMM_IN;
}
