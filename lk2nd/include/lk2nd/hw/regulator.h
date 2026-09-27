/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_HW_REGULATOR_H
#define LK2ND_HW_REGULATOR_H

/*
 * Naming is similar to the regulator subsystem in Linux to simplify porting
 * of existing regulator drivers. In detail it is quite different though, e.g.
 * there is no struct regulator, only struct regulator_dev. struct regulator_dev
 * is actually more like struct regulator_desc etc.
 */

enum regulator_mode {
	REGULATOR_MODE_INVALID,
	REGULATOR_MODE_FAST,
	REGULATOR_MODE_NORMAL,
	REGULATOR_MODE_IDLE,
};

struct regulator_ops;

struct regulator_dev {
	const char *name;
	const struct regulator_ops *ops;
	unsigned n_voltages;
	const char *driver_type;
	struct regulator_dev *next;
};

struct regulator_ops {
	int (*list_voltage)(struct regulator_dev *, unsigned selector);
	int (*get_voltage_sel)(struct regulator_dev *);
	int (*is_enabled)(struct regulator_dev *);
	unsigned int (*get_mode)(struct regulator_dev *);
	int (*get_bypass)(struct regulator_dev *, bool *enable);
};

static inline int regulator_get_voltage(struct regulator_dev *rdev)
{
	int sel;

	if (!rdev->ops || !rdev->ops->get_voltage_sel)
		return -1;

	sel = rdev->ops->get_voltage_sel(rdev);
	if (sel < 0)
		return sel;

	return rdev->ops->list_voltage(rdev, sel);
}

static inline int regulator_is_enabled(struct regulator_dev *rdev)
{
	if (!rdev->ops || !rdev->ops->is_enabled)
		return -1;
	return rdev->ops->is_enabled(rdev);
}

static inline enum regulator_mode regulator_get_mode(struct regulator_dev *rdev)
{
	if (!rdev->ops || !rdev->ops->get_mode)
		return REGULATOR_MODE_INVALID;
	return rdev->ops->get_mode(rdev);
}

static inline int regulator_is_bypassed(struct regulator_dev *rdev)
{
	bool enable;
	int ret;

	if (!rdev->ops || !rdev->ops->get_bypass)
		return -1;

	ret = rdev->ops->get_bypass(rdev, &enable);
	if (ret < 0)
		return ret;

	return enable;
}

/* SPMI regulator driver (GPL) */
struct regulator_dev *spmi_regulator_probe(uint8_t subtype);

#endif /* LK2ND_HW_REGULATOR_H */
