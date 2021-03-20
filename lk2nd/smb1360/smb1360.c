// SPDX-License-Identifier: GPL-2.0-only
#include <debug.h>
#include <libfdt.h>
#include <lk2nd.h>
#include "smb1360.h"

struct smb1360_battery_detector {
	const char *compatible;
	const struct smb1360_battery *(*detect)(const struct smb1360 *smb,
						const void *fdt, int offset);
};

static const struct smb1360_battery_detector detectors[] = {
	{ "alcatel,smb1360-idol347", smb1360_idol347_detect_battery },
	{ "wingtech,smb1360-wt88047", smb1360_wt88047_detect_battery },
	{ "qcom,smb1360", smb1360_qcom_detect_battery }
};

static const struct smb1360_battery_detector *smb1360_match_detector(const void *fdt, int offset)
{
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(detectors); ++i) {
		ret = fdt_node_check_compatible(fdt, offset, detectors[i].compatible);
		switch (ret) {
		case 0:
			return &detectors[i];
		case 1:
			continue;
		default:
			dprintf(CRITICAL, "Failed to check smb1360 compatible: %d\n", ret);
			return NULL;
		}
	}

	dprintf(CRITICAL, "No matching detector found for smb1360 node\n");
	return NULL;
}

extern const struct smb1360 *smb1360_setup_i2c(const void *fdt, int offset);

void smb1360_detect_battery(const void *fdt, int offset)
{
	const struct smb1360_battery_detector *detector;
	const struct smb1360_battery *battery;

	offset = fdt_subnode_offset(fdt, offset, "smb1360");
	if (offset < 0)
		return;

	lk2nd_dev.smb1360 = smb1360_setup_i2c(fdt, offset);

	detector = smb1360_match_detector(fdt, offset);
	if (!detector)
		return;

	battery = detector->detect(lk2nd_dev.smb1360, fdt, offset);
	if (!battery) {
		dprintf(CRITICAL, "Failed to detect smb1360 battery\n");
		lk2nd_dev.battery = "ERROR";
		return;
	}

	dprintf(INFO, "Detected smb1360 battery: %s\n", battery->name);
	lk2nd_dev.battery = battery->name;
	lk2nd_dev.smb1360_battery = battery;
}

static int smb1360_update_u32(void *fdt, int offset, const char *name, uint32_t val)
{
	int ret;

	/* Only update if we have a new value */
	if (!val)
		return 0;

	ret = fdt_setprop_u32(fdt, offset, name, val);
	if (ret < 0)
		dprintf(CRITICAL, "Failed to set smb1360 %s to %#x: %d\n", name, val, ret);
	return ret;
}

void smb1360_update_device_tree(void *fdt)
{
	const struct smb1360_battery *battery = lk2nd_dev.smb1360_battery;
	int offset, ret;

	if (!battery)
		return;

	/* Try to find smb1360 node */
	offset = fdt_node_offset_by_compatible(fdt, -1, "qcom,smb1360");
	if (offset < 0) {
		dprintf(CRITICAL, "Failed to find qcom,smb1360 node\n");
		return;
	}

	if (battery->profile) {
		/* qcom,battery-profile = <0> (A), qcom,battery-profile = <1> (B) */
		ret = fdt_setprop_u32(fdt, offset, "qcom,battery-profile", battery->profile - 1);
		if (ret < 0) {
			dprintf(CRITICAL, "Failed to set smb1360 qcom,battery-profile: %d\n", ret);
			return;
		}
	}

	if (battery->rslow_config) {
		ret = fdt_setprop(fdt, offset, "qcom,otp-rslow-config", battery->rslow_config, 4);
		if (ret < 0) {
			dprintf(CRITICAL, "Failed to set smb1360 qcom,otp-rslow-config: %d\n", ret);
			return;
		}
	}

	if (smb1360_update_u32(fdt, offset, "qcom,fg-batt-capacity-mah", battery->capacity_mah))
		return;
	if (smb1360_update_u32(fdt, offset, "qcom,fg-cc-soc-coeff", battery->cc_soc_coeff))
		return;
	if (smb1360_update_u32(fdt, offset, "qcom,thermistor-c1-coeff", battery->therm_coeff))
		return;

	/* Finally, enable smb1360 */
	ret = fdt_setprop_string(fdt, offset, "status", "okay");
	if (ret) {
		dprintf(CRITICAL, "Failed to set smb1360 status to 'okay': %s\n", ret);
		return;
	}
}
