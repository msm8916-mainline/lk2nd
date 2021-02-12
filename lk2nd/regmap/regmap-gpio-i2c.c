#include <debug.h>
#include <libfdt.h>
#include "regmap-gpio-i2c.h"

status_t regmap_gpio_i2c_raw_read(const struct regmap *map, uint8_t reg, void *val, size_t count)
{
	const struct regmap_gpio_i2c *gpio_i2c = (const struct regmap_gpio_i2c*)map;
	return gpio_i2c_read_reg_bytes(gpio_i2c->bus, gpio_i2c->addr, reg, val, count);
}

status_t regmap_gpio_i2c_raw_write(const struct regmap *map, uint8_t reg, const void *val, size_t count)
{
	const struct regmap_gpio_i2c *gpio_i2c = (const struct regmap_gpio_i2c*)map;
	return gpio_i2c_write_reg_bytes(gpio_i2c->bus, gpio_i2c->addr, reg, val, count);
}

bool gpio_i2c_read_pins(const void *fdt, int offset, gpio_i2c_info_t *info)
{
	const uint32_t *i2c_gpios;
	int len;

	i2c_gpios = fdt_getprop(fdt, offset, "i2c-gpio-pins", &len);
	if (len != 2 * sizeof(*i2c_gpios)) {
		dprintf(CRITICAL, "Invalid/missing 'i2c-gpio-pins' (len %d)\n", len);
		return false;
	}

	info->sda = fdt32_to_cpu(i2c_gpios[0]);
	info->scl = fdt32_to_cpu(i2c_gpios[1]);
	return true;
}

bool regmap_gpio_i2c_read_addr(const void *fdt, int offset, struct regmap_gpio_i2c *gpio_i2c)
{
	const uint32_t *i2c_addr;
	int len;

	i2c_addr = fdt_getprop(fdt, offset, "i2c-address", &len);
	if (len != sizeof(*i2c_addr)) {
		dprintf(CRITICAL, "Invalid/missing 'i2c-address' for regmap %s (len %d)\n",
			gpio_i2c->map.name, len);
		return false;
	}

	gpio_i2c->addr = fdt32_to_cpu(*i2c_addr);
	return true;
}
