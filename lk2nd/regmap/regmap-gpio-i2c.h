/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_REGMAP_GPIO_I2C_H
#define __LK2ND_REGMAP_GPIO_I2C_H
#include "regmap.h"
#include <dev/gpio_i2c.h>

struct regmap_gpio_i2c {
	struct regmap map;
	int bus;
	uint8_t addr;
};

status_t regmap_gpio_i2c_raw_read(const struct regmap *map, uint8_t reg, void *val, size_t count);
status_t regmap_gpio_i2c_raw_write(const struct regmap *map, uint8_t reg, const void *val, size_t count);

#define REGMAP_GPIO_I2C(_name) { \
	.name = _name, \
	.raw_read = regmap_gpio_i2c_raw_read, \
	.raw_write = regmap_gpio_i2c_raw_write, \
}

/* FIXME: Not entirely sure what's the best value for this but seems to work fine... */
#define GPIO_I2C_CLOCKS \
	.hcd = 10 /* us */, \
	.qcd =  5 /* us */

bool gpio_i2c_read_pins(const void *fdt, int offset, gpio_i2c_info_t *info);
bool regmap_gpio_i2c_read_addr(const void *fdt, int offset, struct regmap_gpio_i2c *map);

#endif
