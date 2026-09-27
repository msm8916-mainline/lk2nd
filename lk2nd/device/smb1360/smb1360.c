// SPDX-License-Identifier: BSD-3-Clause
#include <boot.h>
#include <debug.h>

#include <libfdt.h>
#include <lk2nd/util/lkfdt.h>

#include "../../device.h"
#include "smb1360.h"

static const struct smb1360_battery *detected_battery;

int smb1360_battery_detected(const struct smb1360_battery *battery)
{
	if (!battery) {
		dprintf(CRITICAL, "Failed to detect smb1360 battery\n");
		lk2nd_dev.battery = "ERROR";
		return 500;
	}

	dprintf(INFO, "Detected smb1360 battery: %s\n", battery->name);
	lk2nd_dev.battery = battery->name;
	detected_battery = battery;
	return 0;
}

static int smb1360_update_u32(void *dtb, int node, const char *name, uint32_t val)
{
	int ret;

	/* Only update if there is a new value */
	if (!val)
		return 0;

	ret = fdt_setprop_u32(dtb, node, name, val);
	if (ret < 0)
		dprintf(CRITICAL, "Failed to set smb1360 %s to %#x: %d\n",
			name, val, ret);
	return ret;
}

static int smb1360_update_dt(void *dtb, const char *cmdline,
			     enum boot_type boot_type)
{
	const struct smb1360_battery *battery = detected_battery;
	int node, ret;

	if (!battery || boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	node = fdt_node_offset_by_compatible(dtb, -1, "qcom,smb1360");
	if (node < 0) {
		dprintf(CRITICAL, "Failed to find qcom,smb1360 node: %d\n", node);
		return 0; /* Still continue boot */
	}

	if (battery->profile) {
		/* qcom,battery-profile = <0> (A), qcom,battery-profile = <1> (B) */
		ret = fdt_setprop_u32(dtb, node, "qcom,battery-profile",
				      battery->profile - 1);
		if (ret < 0) {
			dprintf(CRITICAL, "Failed to set smb1360 qcom,battery-profile: %d\n", ret);
			return ret;
		}
	}

	if (battery->rslow_config) {
		ret = fdt_setprop(dtb, node, "qcom,otp-rslow-config",
				  battery->rslow_config, SMB1360_RSLOW_CONFIG_SIZE);
		if (ret < 0) {
			dprintf(CRITICAL, "Failed to set smb1360 qcom,otp-rslow-config: %d\n", ret);
			return ret;
		}
	}

	ret = smb1360_update_u32(dtb, node, "qcom,fg-batt-capacity-mah",
				 battery->capacity_mah);
	if (ret < 0)
		return ret;

	ret = smb1360_update_u32(dtb, node, "qcom,fg-cc-soc-coeff",
				 battery->cc_soc_coeff);
	if (ret < 0)
		return ret;

	ret = smb1360_update_u32(dtb, node, "qcom,thermistor-c1-coeff",
				 battery->therm_coeff);
	if (ret < 0)
		return ret;

	/* Finally, enable smb1360 */
	if (!lkfdt_node_is_available(dtb, node)) {
		ret = fdt_nop_property(dtb, node, "status");
		if (ret) {
			dprintf(CRITICAL, "Failed to enable smb1360 node: %d\n", ret);
			return ret;
		}
	}
	return 0;
}
DEV_TREE_UPDATE(smb1360_update_dt);
