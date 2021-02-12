#include <debug.h>
#include "regmap.h"

#define regmap_debug(x...)	if (1) dprintf(SPEW, x)

status_t regmap_raw_read(const struct regmap *map, uint8_t reg, void *val, size_t count)
{
	status_t ret = map->raw_read(map, reg, val, count);
	regmap_debug("regmap %s read reg %#x (count: %d) ret %d\n", map->name, reg, count, ret);
	if (ret)
		dprintf(CRITICAL, "Failed to read reg %#x (count: %d) from %s: %d\n",
			reg, count, map->name, ret);
	return ret;
}

status_t regmap_raw_write(const struct regmap *map, uint8_t reg, const void *val, size_t count)
{
	status_t ret = map->raw_write(map, reg, val, count);
	regmap_debug("regmap %s write reg %#x (count: %d) ret %d\n", map->name, reg, count, ret);
	if (ret)
		dprintf(CRITICAL, "Failed to write reg %#x (count: %d) to %s: %d\n",
			reg, count, map->name, ret);
	return ret;
}

status_t regmap_update_bits(const struct regmap *map, uint8_t reg, uint8_t mask, uint8_t val)
{
	status_t ret;
	uint8_t tmp, orig;

	ret = regmap_read(map, reg, &orig);
	if (ret)
		return ret;

	tmp = orig & ~mask;
	tmp |= val & mask;
	regmap_debug("regmap %s update bits reg %#x mask %#x val %#x orig %#x tmp %#x\n",
		     map->name, reg, mask, val, orig, tmp);

	if (tmp != orig)
		ret = regmap_write(map, reg, tmp);

	return ret;
}
