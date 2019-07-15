// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2012-2015, The Linux Foundation. All rights reserved.
 */

#include <debug.h>
#include <pm8x41_hw.h>
#include <pm8x41_regulator.h>

typedef uint8_t		u8;
typedef uint32_t	u16;
typedef uint32_t	u32;

#define	ENODEV		19	/* No such device */
#define	EINVAL		22	/* Invalid argument */

struct regulator_ops {
	bool (*is_enabled)(struct spmi_regulator *vreg);
	int (*get_voltage_sel)(struct spmi_regulator *vreg);
	int (*list_voltage)(struct spmi_regulator *vreg, unsigned selector);
};

/* These types correspond to unique register layouts. */
enum spmi_regulator_logical_type {
	SPMI_REGULATOR_LOGICAL_TYPE_SMPS,
	SPMI_REGULATOR_LOGICAL_TYPE_LDO,
	SPMI_REGULATOR_LOGICAL_TYPE_VS,
	SPMI_REGULATOR_LOGICAL_TYPE_BOOST,
	SPMI_REGULATOR_LOGICAL_TYPE_FTSMPS,
	SPMI_REGULATOR_LOGICAL_TYPE_BOOST_BYP,
	SPMI_REGULATOR_LOGICAL_TYPE_LN_LDO,
	SPMI_REGULATOR_LOGICAL_TYPE_ULT_LO_SMPS,
	SPMI_REGULATOR_LOGICAL_TYPE_ULT_HO_SMPS,
	SPMI_REGULATOR_LOGICAL_TYPE_ULT_LDO,
};

enum spmi_regulator_type {
	SPMI_REGULATOR_TYPE_BUCK		= 0x03,
	SPMI_REGULATOR_TYPE_LDO			= 0x04,
	SPMI_REGULATOR_TYPE_VS			= 0x05,
	SPMI_REGULATOR_TYPE_BOOST		= 0x1b,
	SPMI_REGULATOR_TYPE_FTS			= 0x1c,
	SPMI_REGULATOR_TYPE_BOOST_BYP		= 0x1f,
	SPMI_REGULATOR_TYPE_ULT_LDO		= 0x21,
	SPMI_REGULATOR_TYPE_ULT_BUCK		= 0x22,
};

enum spmi_regulator_subtype {
	SPMI_REGULATOR_SUBTYPE_GP_CTL		= 0x08,
	SPMI_REGULATOR_SUBTYPE_RF_CTL		= 0x09,
	SPMI_REGULATOR_SUBTYPE_N50		= 0x01,
	SPMI_REGULATOR_SUBTYPE_N150		= 0x02,
	SPMI_REGULATOR_SUBTYPE_N300		= 0x03,
	SPMI_REGULATOR_SUBTYPE_N600		= 0x04,
	SPMI_REGULATOR_SUBTYPE_N1200		= 0x05,
	SPMI_REGULATOR_SUBTYPE_N600_ST		= 0x06,
	SPMI_REGULATOR_SUBTYPE_N1200_ST		= 0x07,
	SPMI_REGULATOR_SUBTYPE_N900_ST		= 0x14,
	SPMI_REGULATOR_SUBTYPE_N300_ST		= 0x15,
	SPMI_REGULATOR_SUBTYPE_P50		= 0x08,
	SPMI_REGULATOR_SUBTYPE_P150		= 0x09,
	SPMI_REGULATOR_SUBTYPE_P300		= 0x0a,
	SPMI_REGULATOR_SUBTYPE_P600		= 0x0b,
	SPMI_REGULATOR_SUBTYPE_P1200		= 0x0c,
	SPMI_REGULATOR_SUBTYPE_LN		= 0x10,
	SPMI_REGULATOR_SUBTYPE_LV_P50		= 0x28,
	SPMI_REGULATOR_SUBTYPE_LV_P150		= 0x29,
	SPMI_REGULATOR_SUBTYPE_LV_P300		= 0x2a,
	SPMI_REGULATOR_SUBTYPE_LV_P600		= 0x2b,
	SPMI_REGULATOR_SUBTYPE_LV_P1200		= 0x2c,
	SPMI_REGULATOR_SUBTYPE_LV_P450		= 0x2d,
	SPMI_REGULATOR_SUBTYPE_LV100		= 0x01,
	SPMI_REGULATOR_SUBTYPE_LV300		= 0x02,
	SPMI_REGULATOR_SUBTYPE_MV300		= 0x08,
	SPMI_REGULATOR_SUBTYPE_MV500		= 0x09,
	SPMI_REGULATOR_SUBTYPE_HDMI		= 0x10,
	SPMI_REGULATOR_SUBTYPE_OTG		= 0x11,
	SPMI_REGULATOR_SUBTYPE_5V_BOOST		= 0x01,
	SPMI_REGULATOR_SUBTYPE_FTS_CTL		= 0x08,
	SPMI_REGULATOR_SUBTYPE_FTS2p5_CTL	= 0x09,
	SPMI_REGULATOR_SUBTYPE_BB_2A		= 0x01,
	SPMI_REGULATOR_SUBTYPE_ULT_HF_CTL1	= 0x0d,
	SPMI_REGULATOR_SUBTYPE_ULT_HF_CTL2	= 0x0e,
	SPMI_REGULATOR_SUBTYPE_ULT_HF_CTL3	= 0x0f,
	SPMI_REGULATOR_SUBTYPE_ULT_HF_CTL4	= 0x10,
};

enum spmi_common_regulator_registers {
	SPMI_COMMON_REG_DIG_MAJOR_REV		= 0x01,
	SPMI_COMMON_REG_TYPE			= 0x04,
	SPMI_COMMON_REG_SUBTYPE			= 0x05,
	SPMI_COMMON_REG_VOLTAGE_RANGE		= 0x40,
	SPMI_COMMON_REG_VOLTAGE_SET		= 0x41,
	SPMI_COMMON_REG_MODE			= 0x45,
	SPMI_COMMON_REG_ENABLE			= 0x46,
	SPMI_COMMON_REG_PULL_DOWN		= 0x48,
	SPMI_COMMON_REG_SOFT_START		= 0x4c,
	SPMI_COMMON_REG_STEP_CTRL		= 0x61,
};

/* Common regulator control register layout */
#define SPMI_COMMON_ENABLE_MASK			0x80
#define SPMI_COMMON_ENABLE			0x80
#define SPMI_COMMON_DISABLE			0x00
#define SPMI_COMMON_ENABLE_FOLLOW_HW_EN3_MASK	0x08
#define SPMI_COMMON_ENABLE_FOLLOW_HW_EN2_MASK	0x04
#define SPMI_COMMON_ENABLE_FOLLOW_HW_EN1_MASK	0x02
#define SPMI_COMMON_ENABLE_FOLLOW_HW_EN0_MASK	0x01
#define SPMI_COMMON_ENABLE_FOLLOW_ALL_MASK	0x0f

/* VSET value to decide the range of ULT SMPS */
#define ULT_SMPS_RANGE_SPLIT 0x60

/**
 * struct spmi_voltage_range - regulator set point voltage mapping description
 * @min_uV:		Minimum programmable output voltage resulting from
 *			set point register value 0x00
 * @max_uV:		Maximum programmable output voltage
 * @step_uV:		Output voltage increase resulting from the set point
 *			register value increasing by 1
 * @set_point_min_uV:	Minimum allowed voltage
 * @set_point_max_uV:	Maximum allowed voltage.  This may be tweaked in order
 *			to pick which range should be used in the case of
 *			overlapping set points.
 * @n_voltages:		Number of preferred voltage set points present in this
 *			range
 * @range_sel:		Voltage range register value corresponding to this range
 *
 * The following relationships must be true for the values used in this struct:
 * (max_uV - min_uV) % step_uV == 0
 * (set_point_min_uV - min_uV) % step_uV == 0*
 * (set_point_max_uV - min_uV) % step_uV == 0*
 * n_voltages = (set_point_max_uV - set_point_min_uV) / step_uV + 1
 *
 * *Note, set_point_min_uV == set_point_max_uV == 0 is allowed in order to
 * specify that the voltage range has meaning, but is not preferred.
 */
struct spmi_voltage_range {
	int					min_uV;
	int					max_uV;
	int					step_uV;
	int					set_point_min_uV;
	int					set_point_max_uV;
	unsigned				n_voltages;
	u8					range_sel;
};

/*
 * The ranges specified in the spmi_voltage_set_points struct must be listed
 * so that range[i].set_point_max_uV < range[i+1].set_point_min_uV.
 */
struct spmi_voltage_set_points {
	struct spmi_voltage_range		*range;
	int					count;
	unsigned				n_voltages;
};

struct spmi_regulator_mapping {
	enum spmi_regulator_type		type;
	enum spmi_regulator_subtype		subtype;
	enum spmi_regulator_logical_type	logical_type;
	u32					revision_min;
	u32					revision_max;
	struct regulator_ops			*ops;
	struct spmi_voltage_set_points		*set_points;
	int					hpm_min_load;
};


#define SPMI_VREG(_type, _subtype, _dig_major_min, _dig_major_max, \
		      _logical_type, _ops_val, _set_points_val, _hpm_min_load) \
	{ \
		.type		= SPMI_REGULATOR_TYPE_##_type, \
		.subtype	= SPMI_REGULATOR_SUBTYPE_##_subtype, \
		.revision_min	= _dig_major_min, \
		.revision_max	= _dig_major_max, \
		.logical_type	= SPMI_REGULATOR_LOGICAL_TYPE_##_logical_type, \
		.ops		= &spmi_##_ops_val##_ops, \
		.set_points	= &_set_points_val##_set_points, \
		.hpm_min_load	= _hpm_min_load, \
	}

#define SPMI_VREG_VS(_subtype, _dig_major_min, _dig_major_max) \
	{ \
		.type		= SPMI_REGULATOR_TYPE_VS, \
		.subtype	= SPMI_REGULATOR_SUBTYPE_##_subtype, \
		.revision_min	= _dig_major_min, \
		.revision_max	= _dig_major_max, \
		.logical_type	= SPMI_REGULATOR_LOGICAL_TYPE_VS, \
		.ops		= &spmi_vs_ops, \
	}


#define SPMI_VOLTAGE_RANGE(_range_sel, _min_uV, _set_point_min_uV, \
			_set_point_max_uV, _max_uV, _step_uV) \
	{ \
		.min_uV			= _min_uV, \
		.max_uV			= _max_uV, \
		.set_point_min_uV	= _set_point_min_uV, \
		.set_point_max_uV	= _set_point_max_uV, \
		.step_uV		= _step_uV, \
		.range_sel		= _range_sel, \
	}

#define DEFINE_SPMI_SET_POINTS(name) \
struct spmi_voltage_set_points name##_set_points = { \
	.range	= name##_ranges, \
	.count	= ARRAY_SIZE(name##_ranges), \
}

/*
 * These tables contain the physically available PMIC regulator voltage setpoint
 * ranges.  Where two ranges overlap in hardware, one of the ranges is trimmed
 * to ensure that the setpoints available to software are monotonically
 * increasing and unique.  The set_voltage callback functions expect these
 * properties to hold.
 */
static struct spmi_voltage_range pldo_ranges[] = {
	SPMI_VOLTAGE_RANGE(2,  750000,  750000, 1537500, 1537500, 12500),
	SPMI_VOLTAGE_RANGE(3, 1500000, 1550000, 3075000, 3075000, 25000),
	SPMI_VOLTAGE_RANGE(4, 1750000, 3100000, 4900000, 4900000, 50000),
};

static struct spmi_voltage_range nldo1_ranges[] = {
	SPMI_VOLTAGE_RANGE(2,  750000,  750000, 1537500, 1537500, 12500),
};

static struct spmi_voltage_range nldo2_ranges[] = {
	SPMI_VOLTAGE_RANGE(0,  375000,       0,       0, 1537500, 12500),
	SPMI_VOLTAGE_RANGE(1,  375000,  375000,  768750,  768750,  6250),
	SPMI_VOLTAGE_RANGE(2,  750000,  775000, 1537500, 1537500, 12500),
};

static struct spmi_voltage_range nldo3_ranges[] = {
	SPMI_VOLTAGE_RANGE(0,  375000,  375000, 1537500, 1537500, 12500),
	SPMI_VOLTAGE_RANGE(1,  375000,       0,       0, 1537500, 12500),
	SPMI_VOLTAGE_RANGE(2,  750000,       0,       0, 1537500, 12500),
};

static struct spmi_voltage_range ln_ldo_ranges[] = {
	SPMI_VOLTAGE_RANGE(1,  690000,  690000, 1110000, 1110000, 60000),
	SPMI_VOLTAGE_RANGE(0, 1380000, 1380000, 2220000, 2220000, 120000),
};

static struct spmi_voltage_range smps_ranges[] = {
	SPMI_VOLTAGE_RANGE(0,  375000,  375000, 1562500, 1562500, 12500),
	SPMI_VOLTAGE_RANGE(1, 1550000, 1575000, 3125000, 3125000, 25000),
};

static struct spmi_voltage_range ftsmps_ranges[] = {
	SPMI_VOLTAGE_RANGE(0,       0,  350000, 1275000, 1275000,  5000),
	SPMI_VOLTAGE_RANGE(1,       0, 1280000, 2040000, 2040000, 10000),
};

static struct spmi_voltage_range ftsmps2p5_ranges[] = {
	SPMI_VOLTAGE_RANGE(0,   80000,  350000, 1355000, 1355000,  5000),
	SPMI_VOLTAGE_RANGE(1,  160000, 1360000, 2200000, 2200000, 10000),
};

static struct spmi_voltage_range boost_ranges[] = {
	SPMI_VOLTAGE_RANGE(0, 4000000, 4000000, 5550000, 5550000, 50000),
};

static struct spmi_voltage_range boost_byp_ranges[] = {
	SPMI_VOLTAGE_RANGE(0, 2500000, 2500000, 5200000, 5650000, 50000),
};

static struct spmi_voltage_range ult_lo_smps_ranges[] = {
	SPMI_VOLTAGE_RANGE(0,  375000,  375000, 1562500, 1562500, 12500),
	SPMI_VOLTAGE_RANGE(1,  750000,       0,       0, 1525000, 25000),
};

static struct spmi_voltage_range ult_ho_smps_ranges[] = {
	SPMI_VOLTAGE_RANGE(0, 1550000, 1550000, 2325000, 2325000, 25000),
};

static struct spmi_voltage_range ult_nldo_ranges[] = {
	SPMI_VOLTAGE_RANGE(0,  375000,  375000, 1537500, 1537500, 12500),
};

static struct spmi_voltage_range ult_pldo_ranges[] = {
	SPMI_VOLTAGE_RANGE(0, 1750000, 1750000, 3337500, 3337500, 12500),
};

static DEFINE_SPMI_SET_POINTS(pldo);
static DEFINE_SPMI_SET_POINTS(nldo1);
static DEFINE_SPMI_SET_POINTS(nldo2);
static DEFINE_SPMI_SET_POINTS(nldo3);
static DEFINE_SPMI_SET_POINTS(ln_ldo);
static DEFINE_SPMI_SET_POINTS(smps);
static DEFINE_SPMI_SET_POINTS(ftsmps);
static DEFINE_SPMI_SET_POINTS(ftsmps2p5);
static DEFINE_SPMI_SET_POINTS(boost);
static DEFINE_SPMI_SET_POINTS(boost_byp);
static DEFINE_SPMI_SET_POINTS(ult_lo_smps);
static DEFINE_SPMI_SET_POINTS(ult_ho_smps);
static DEFINE_SPMI_SET_POINTS(ult_nldo);
static DEFINE_SPMI_SET_POINTS(ult_pldo);

static int spmi_vreg_read(struct spmi_regulator *vreg, u16 addr, u8 *buf,
			  int len)
{
	int i;
	for (i = 0; i < len; ++i) {
		buf[i] = REG_READ(vreg->base + addr + i);
	}
	return 0;
}

static bool regulator_is_enabled_regmap(struct spmi_regulator *vreg)
{
	u8 enabled;
	spmi_vreg_read(vreg, SPMI_COMMON_REG_ENABLE, &enabled, 1);
	return (enabled & SPMI_COMMON_ENABLE_MASK) == SPMI_COMMON_ENABLE;
}

static int spmi_hw_selector_to_sw(struct spmi_regulator *vreg, u8 hw_sel,
				  const struct spmi_voltage_range *range)
{
	unsigned sw_sel = 0;
	unsigned offset, max_hw_sel;
	const struct spmi_voltage_range *r = vreg->mapping->set_points->range;
	const struct spmi_voltage_range *end = r + vreg->mapping->set_points->count;

	for (; r < end; r++) {
		if (r == range && range->n_voltages) {
			/*
			 * hardware selectors between set point min and real
			 * min and between set point max and real max are
			 * invalid so we return an error if they're
			 * programmed into the hardware
			 */
			offset = range->set_point_min_uV - range->min_uV;
			offset /= range->step_uV;
			if (hw_sel < offset)
				return -EINVAL;

			max_hw_sel = range->set_point_max_uV - range->min_uV;
			max_hw_sel /= range->step_uV;
			if (hw_sel > max_hw_sel)
				return -EINVAL;

			return sw_sel + hw_sel - offset;
		}
		sw_sel += r->n_voltages;
	}

	return -EINVAL;
}

static const struct spmi_voltage_range *
spmi_regulator_find_range(struct spmi_regulator *vreg)
{
	u8 range_sel;
	const struct spmi_voltage_range *range, *end;

	range = vreg->mapping->set_points->range;
	end = range + vreg->mapping->set_points->count;

	spmi_vreg_read(vreg, SPMI_COMMON_REG_VOLTAGE_RANGE, &range_sel, 1);

	for (; range < end; range++)
		if (range->range_sel == range_sel)
			return range;

	return NULL;
}

static int spmi_regulator_common_get_voltage(struct spmi_regulator *vreg)
{
	const struct spmi_voltage_range *range;
	u8 voltage_sel;

	spmi_vreg_read(vreg, SPMI_COMMON_REG_VOLTAGE_SET, &voltage_sel, 1);

	range = spmi_regulator_find_range(vreg);
	if (!range)
		return -EINVAL;

	return spmi_hw_selector_to_sw(vreg, voltage_sel, range);
}

static int spmi_regulator_single_range_get_voltage(struct spmi_regulator *vreg)
{
	u8 selector;
	int ret;

	ret = spmi_vreg_read(vreg, SPMI_COMMON_REG_VOLTAGE_SET, &selector, 1);
	if (ret)
		return ret;

	return selector;
}

static int spmi_regulator_ult_lo_smps_get_voltage(struct spmi_regulator *vreg)
{
	const struct spmi_voltage_range *range;
	u8 voltage_sel;

	spmi_vreg_read(vreg, SPMI_COMMON_REG_VOLTAGE_SET, &voltage_sel, 1);

	range = spmi_regulator_find_range(vreg);
	if (!range)
		return -EINVAL;

	if (range->range_sel == 1)
		voltage_sel &= ~ULT_SMPS_RANGE_SPLIT;

	return spmi_hw_selector_to_sw(vreg, voltage_sel, range);
}

static int spmi_regulator_common_list_voltage(struct spmi_regulator *reg,
			unsigned selector)
{
	const struct spmi_regulator_mapping *vreg = reg->mapping;
	int uV = 0;
	int i;

	if (selector >= vreg->set_points->n_voltages)
		return 0;

	for (i = 0; i < vreg->set_points->count; i++) {
		if (selector < vreg->set_points->range[i].n_voltages) {
			uV = selector * vreg->set_points->range[i].step_uV
				+ vreg->set_points->range[i].set_point_min_uV;
			break;
		}

		selector -= vreg->set_points->range[i].n_voltages;
	}

	return uV;
}

static struct regulator_ops spmi_smps_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	/*.set_voltage_sel	= spmi_regulator_common_set_voltage,
	.set_voltage_time_sel	= spmi_regulator_set_voltage_time_sel,*/
	.get_voltage_sel	= spmi_regulator_common_get_voltage,
	//.map_voltage		= spmi_regulator_common_map_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	/*.set_mode		= spmi_regulator_common_set_mode,
	.get_mode		= spmi_regulator_common_get_mode,
	.set_load		= spmi_regulator_common_set_load,
	.set_pull_down		= spmi_regulator_common_set_pull_down,*/
};

static struct regulator_ops spmi_ldo_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	//.set_voltage_sel	= spmi_regulator_common_set_voltage,
	.get_voltage_sel	= spmi_regulator_common_get_voltage,
	//.map_voltage		= spmi_regulator_common_map_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	/*.set_mode		= spmi_regulator_common_set_mode,
	.get_mode		= spmi_regulator_common_get_mode,
	.set_load		= spmi_regulator_common_set_load,
	.set_bypass		= spmi_regulator_common_set_bypass,
	.get_bypass		= spmi_regulator_common_get_bypass,
	.set_pull_down		= spmi_regulator_common_set_pull_down,
	.set_soft_start		= spmi_regulator_common_set_soft_start,*/
};

static struct regulator_ops spmi_ln_ldo_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	//.set_voltage_sel	= spmi_regulator_common_set_voltage,
	.get_voltage_sel	= spmi_regulator_common_get_voltage,
	//.map_voltage		= spmi_regulator_common_map_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	/*.set_bypass		= spmi_regulator_common_set_bypass,
	.get_bypass		= spmi_regulator_common_get_bypass,*/
};

static struct regulator_ops spmi_vs_ops = {
	/*.enable			= spmi_regulator_vs_enable,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	/*.set_pull_down		= spmi_regulator_common_set_pull_down,
	.set_soft_start		= spmi_regulator_common_set_soft_start,
	.set_over_current_protection = spmi_regulator_vs_ocp,
	.set_mode		= spmi_regulator_common_set_mode,
	.get_mode		= spmi_regulator_common_get_mode,*/
};

static struct regulator_ops spmi_boost_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	//.set_voltage_sel	= spmi_regulator_single_range_set_voltage,
	.get_voltage_sel	= spmi_regulator_single_range_get_voltage,
	//.map_voltage		= spmi_regulator_single_map_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	//.set_input_current_limit = spmi_regulator_set_ilim,
};

static struct regulator_ops spmi_ftsmps_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	/*.set_voltage_sel	= spmi_regulator_common_set_voltage,
	.set_voltage_time_sel	= spmi_regulator_set_voltage_time_sel,*/
	.get_voltage_sel	= spmi_regulator_common_get_voltage,
	//.map_voltage		= spmi_regulator_common_map_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	/*.set_mode		= spmi_regulator_common_set_mode,
	.get_mode		= spmi_regulator_common_get_mode,
	.set_load		= spmi_regulator_common_set_load,
	.set_pull_down		= spmi_regulator_common_set_pull_down,*/
};

static struct regulator_ops spmi_ult_lo_smps_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	/*.set_voltage_sel	= spmi_regulator_ult_lo_smps_set_voltage,
	.set_voltage_time_sel	= spmi_regulator_set_voltage_time_sel,*/
	.get_voltage_sel	= spmi_regulator_ult_lo_smps_get_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	/*.set_mode		= spmi_regulator_common_set_mode,
	.get_mode		= spmi_regulator_common_get_mode,
	.set_load		= spmi_regulator_common_set_load,
	.set_pull_down		= spmi_regulator_common_set_pull_down,*/
};

static struct regulator_ops spmi_ult_ho_smps_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	/*.set_voltage_sel	= spmi_regulator_single_range_set_voltage,
	.set_voltage_time_sel	= spmi_regulator_set_voltage_time_sel,*/
	.get_voltage_sel	= spmi_regulator_single_range_get_voltage,
	//.map_voltage		= spmi_regulator_single_map_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	/*.set_mode		= spmi_regulator_common_set_mode,
	.get_mode		= spmi_regulator_common_get_mode,
	.set_load		= spmi_regulator_common_set_load,
	.set_pull_down		= spmi_regulator_common_set_pull_down,*/
};

static struct regulator_ops spmi_ult_ldo_ops = {
	/*.enable			= regulator_enable_regmap,
	.disable		= regulator_disable_regmap,*/
	.is_enabled		= regulator_is_enabled_regmap,
	//.set_voltage_sel	= spmi_regulator_single_range_set_voltage,
	.get_voltage_sel	= spmi_regulator_single_range_get_voltage,
	//.map_voltage		= spmi_regulator_single_map_voltage,
	.list_voltage		= spmi_regulator_common_list_voltage,
	/*.set_mode		= spmi_regulator_common_set_mode,
	.get_mode		= spmi_regulator_common_get_mode,
	.set_load		= spmi_regulator_common_set_load,
	.set_bypass		= spmi_regulator_common_set_bypass,
	.get_bypass		= spmi_regulator_common_get_bypass,
	.set_pull_down		= spmi_regulator_common_set_pull_down,
	.set_soft_start		= spmi_regulator_common_set_soft_start,*/
};

/* Maximum possible digital major revision value */
#define INF 0xFF

static const struct spmi_regulator_mapping supported_regulators[] = {
	/*           type subtype dig_min dig_max ltype ops setpoints hpm_min */
	SPMI_VREG(BUCK,  GP_CTL,   0, INF, SMPS,   smps,   smps,   100000),
	SPMI_VREG(LDO,   N300,     0, INF, LDO,    ldo,    nldo1,   10000),
	SPMI_VREG(LDO,   N600,     0,   0, LDO,    ldo,    nldo2,   10000),
	SPMI_VREG(LDO,   N1200,    0,   0, LDO,    ldo,    nldo2,   10000),
	SPMI_VREG(LDO,   N600,     1, INF, LDO,    ldo,    nldo3,   10000),
	SPMI_VREG(LDO,   N1200,    1, INF, LDO,    ldo,    nldo3,   10000),
	SPMI_VREG(LDO,   N600_ST,  0,   0, LDO,    ldo,    nldo2,   10000),
	SPMI_VREG(LDO,   N1200_ST, 0,   0, LDO,    ldo,    nldo2,   10000),
	SPMI_VREG(LDO,   N600_ST,  1, INF, LDO,    ldo,    nldo3,   10000),
	SPMI_VREG(LDO,   N1200_ST, 1, INF, LDO,    ldo,    nldo3,   10000),
	SPMI_VREG(LDO,   P50,      0, INF, LDO,    ldo,    pldo,     5000),
	SPMI_VREG(LDO,   P150,     0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG(LDO,   P300,     0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG(LDO,   P600,     0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG(LDO,   P1200,    0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG(LDO,   LN,       0, INF, LN_LDO, ln_ldo, ln_ldo,      0),
	SPMI_VREG(LDO,   LV_P50,   0, INF, LDO,    ldo,    pldo,     5000),
	SPMI_VREG(LDO,   LV_P150,  0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG(LDO,   LV_P300,  0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG(LDO,   LV_P600,  0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG(LDO,   LV_P1200, 0, INF, LDO,    ldo,    pldo,    10000),
	SPMI_VREG_VS(LV100,        0, INF),
	SPMI_VREG_VS(LV300,        0, INF),
	SPMI_VREG_VS(MV300,        0, INF),
	SPMI_VREG_VS(MV500,        0, INF),
	SPMI_VREG_VS(HDMI,         0, INF),
	SPMI_VREG_VS(OTG,          0, INF),
	SPMI_VREG(BOOST, 5V_BOOST, 0, INF, BOOST,  boost,  boost,       0),
	SPMI_VREG(FTS,   FTS_CTL,  0, INF, FTSMPS, ftsmps, ftsmps, 100000),
	SPMI_VREG(FTS, FTS2p5_CTL, 0, INF, FTSMPS, ftsmps, ftsmps2p5, 100000),
	SPMI_VREG(BOOST_BYP, BB_2A, 0, INF, BOOST_BYP, boost, boost_byp, 0),
	SPMI_VREG(ULT_BUCK, ULT_HF_CTL1, 0, INF, ULT_LO_SMPS, ult_lo_smps,
						ult_lo_smps,   100000),
	SPMI_VREG(ULT_BUCK, ULT_HF_CTL2, 0, INF, ULT_LO_SMPS, ult_lo_smps,
						ult_lo_smps,   100000),
	SPMI_VREG(ULT_BUCK, ULT_HF_CTL3, 0, INF, ULT_LO_SMPS, ult_lo_smps,
						ult_lo_smps,   100000),
	SPMI_VREG(ULT_BUCK, ULT_HF_CTL4, 0, INF, ULT_HO_SMPS, ult_ho_smps,
						ult_ho_smps,   100000),
	SPMI_VREG(ULT_LDO, N300_ST, 0, INF, ULT_LDO, ult_ldo, ult_nldo, 10000),
	SPMI_VREG(ULT_LDO, N600_ST, 0, INF, ULT_LDO, ult_ldo, ult_nldo, 10000),
	SPMI_VREG(ULT_LDO, N900_ST, 0, INF, ULT_LDO, ult_ldo, ult_nldo, 10000),
	SPMI_VREG(ULT_LDO, N1200_ST, 0, INF, ULT_LDO, ult_ldo, ult_nldo, 10000),
	SPMI_VREG(ULT_LDO, LV_P150,  0, INF, ULT_LDO, ult_ldo, ult_pldo, 10000),
	SPMI_VREG(ULT_LDO, LV_P300,  0, INF, ULT_LDO, ult_ldo, ult_pldo, 10000),
	SPMI_VREG(ULT_LDO, LV_P450,  0, INF, ULT_LDO, ult_ldo, ult_pldo, 10000),
	SPMI_VREG(ULT_LDO, P600,     0, INF, ULT_LDO, ult_ldo, ult_pldo, 10000),
	SPMI_VREG(ULT_LDO, P150,     0, INF, ULT_LDO, ult_ldo, ult_pldo, 10000),
	SPMI_VREG(ULT_LDO, P50,     0, INF, ULT_LDO, ult_ldo, ult_pldo, 5000),
};

static void spmi_calculate_num_voltages(struct spmi_voltage_set_points *points)
{
	unsigned int n;
	struct spmi_voltage_range *range = points->range;

	for (; range < points->range + points->count; range++) {
		n = 0;
		if (range->set_point_max_uV) {
			n = range->set_point_max_uV - range->set_point_min_uV;
			n = (n / range->step_uV) + 1;
		}
		range->n_voltages = n;
		points->n_voltages += n;
	}
}

static int spmi_regulator_match(struct spmi_regulator *vreg, u16 force_type)
{
	const struct spmi_regulator_mapping *mapping;
	int ret, i;
	u32 dig_major_rev;
	u8 version[SPMI_COMMON_REG_SUBTYPE - SPMI_COMMON_REG_DIG_MAJOR_REV + 1];
	u8 type, subtype;

	ret = spmi_vreg_read(vreg, SPMI_COMMON_REG_DIG_MAJOR_REV, version,
		ARRAY_SIZE(version));
	if (ret) {
		dprintf(CRITICAL, "%s: could not read version registers\n",
			vreg->name);
		return ret;
	}
	dig_major_rev	= version[SPMI_COMMON_REG_DIG_MAJOR_REV
					- SPMI_COMMON_REG_DIG_MAJOR_REV];

	if (!force_type) {
		type		= version[SPMI_COMMON_REG_TYPE -
					  SPMI_COMMON_REG_DIG_MAJOR_REV];
		subtype		= version[SPMI_COMMON_REG_SUBTYPE -
					  SPMI_COMMON_REG_DIG_MAJOR_REV];
	} else {
		type = force_type >> 8;
		subtype = force_type;
	}

	for (i = 0; i < ARRAY_SIZE(supported_regulators); i++) {
		mapping = &supported_regulators[i];
		if (mapping->type == type && mapping->subtype == subtype
		    && mapping->revision_min <= dig_major_rev
		    && mapping->revision_max >= dig_major_rev)
			goto found;
	}

	dprintf(CRITICAL,
		"unsupported regulator: name=%s type=0x%02X, subtype=0x%02X, dig major rev=0x%02X\n",
		vreg->name, type, subtype, dig_major_rev);

	return -ENODEV;

found:
	vreg->mapping = mapping;
	/*vreg->logical_type	= mapping->logical_type;
	vreg->set_points	= mapping->set_points;
	vreg->hpm_min_load	= mapping->hpm_min_load;
	vreg->desc.ops		= mapping->ops;*/

	if (mapping->set_points) {
		if (!mapping->set_points->n_voltages)
			spmi_calculate_num_voltages(mapping->set_points);
		//vreg->desc.n_voltages = mapping->set_points->n_voltages;
	}

	return 0;
}

void regulators_init(struct spmi_regulator *vregs)
{
	struct spmi_regulator *vreg;
	for (vreg = vregs; vreg->name; ++vreg) {
		if (vreg->mapping)
			continue;
		spmi_regulator_match(vreg, 0);
	}
}

bool regulator_is_enabled(struct spmi_regulator *vreg)
{
	return vreg->mapping && vreg->mapping->ops->is_enabled
		&& vreg->mapping->ops->is_enabled(vreg);
}

int regulator_get_voltage(struct spmi_regulator *vreg) {
	int sel;
	if (!vreg->mapping)
		return -ENODEV;
	if (!vreg->mapping->ops->get_voltage_sel)
		return -EINVAL;

	sel = vreg->mapping->ops->get_voltage_sel(vreg);
	if (sel < 0)
		return sel;

	return vreg->mapping->ops->list_voltage(vreg, sel);
}
