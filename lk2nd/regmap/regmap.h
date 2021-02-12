/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_REGMAP_H
#define __LK2ND_REGMAP_H
#include <sys/types.h>

struct regmap {
	const char *name;
	status_t (*raw_read)(const struct regmap *map, uint8_t reg, void *val, size_t count);
	status_t (*raw_write)(const struct regmap *map, uint8_t reg, const void *val, size_t count);
};

status_t regmap_raw_read(const struct regmap *map, uint8_t reg, void *val, size_t count);
status_t regmap_raw_write(const struct regmap *map, uint8_t reg, const void *val, size_t count);

static inline status_t regmap_read(const struct regmap *map, uint8_t reg, uint8_t *val)
{
	return regmap_raw_read(map, reg, val, sizeof(*val));
}

static inline status_t regmap_write(const struct regmap *map, uint8_t reg, uint8_t val)
{
	return regmap_raw_write(map, reg, &val, sizeof(val));
}

status_t regmap_update_bits(const struct regmap *map, uint8_t reg, uint8_t mask, uint8_t val);

#endif
