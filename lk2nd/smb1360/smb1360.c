// SPDX-License-Identifier: GPL-2.0-only
#include <debug.h>
#include <libfdt.h>
#include <lk2nd.h>
#include "smb1360.h"

struct smb1360_battery_detector {
	const char *compatible;
	const struct smb1360_battery *(*detect)(const void *fdt, int offset);
};

static const struct smb1360_battery_detector detectors[] = {
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

void smb1360_detect_battery(const void *fdt, int offset)
{
	const struct smb1360_battery_detector *detector;
	const struct smb1360_battery *battery;

	offset = fdt_subnode_offset(fdt, offset, "smb1360");
	if (offset < 0)
		return;

	detector = smb1360_match_detector(fdt, offset);
	if (!detector)
		return;

	battery = detector->detect(fdt, offset);
	if (!battery) {
		dprintf(CRITICAL, "Failed to detect smb1360 battery\n");
		lk2nd_dev.battery = "ERROR";
		return;
	}

	dprintf(INFO, "Detected smb1360 battery: %s\n", battery->name);
	lk2nd_dev.battery = battery->name;
	lk2nd_dev.smb1360_battery = battery;
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

	/* TODO: Update some properties here */

	/* Finally, enable smb1360 */
	ret = fdt_setprop_string(fdt, offset, "status", "okay");
	if (ret) {
		dprintf(CRITICAL, "Failed to set smb1360 status to 'okay': %s\n", ret);
		return;
	}
}
