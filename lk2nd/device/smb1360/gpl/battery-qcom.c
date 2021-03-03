// SPDX-License-Identifier: GPL-2.0-only
/*
 * Based on code from smb1360-charger-fg.c in Qualcomm's msm-3.10 release:
 * Copyright (c) 2013-2015 The Linux Foundation. All rights reserved.
 */

#include <debug.h>
#include <pm8x41.h>
#include <pm8x41_adc.h>

#include <libfdt.h>

#include "../../device.h"
#include "../smb1360.h"

static const struct smb1360_battery batteryA = {
	.name = "Profile A",
	.profile = SMB1360_BATTERY_PROFILE_A,
};

static const struct smb1360_battery batteryB = {
	.name = "Profile B",
	.profile = SMB1360_BATTERY_PROFILE_B,
};

static int abs(int n) {
	if (n >= 0)
		return n;
	return -n;
}

bool smb1360_check_profile(const void *dtb, int node, const char *prop_name,
			   uint32_t connected_rid)
{
	uint32_t profile_rid;
	const fdt32_t *prop;
	int len;

	prop = fdt_getprop(dtb, node, prop_name, &len);
	if (len != sizeof(profile_rid))
		return false;
	profile_rid = fdt32_to_cpu(*prop);

	return abs(profile_rid - connected_rid) < (int)(connected_rid / 10);
}

uint32_t smb1360_connected_rid(const void *dtb, int node)
{
	int64_t denom, batt_id_uv;
	int32_t len, rpull, vref;
	uint32_t connected_rid;
	const fdt32_t *prop;

	prop = fdt_getprop(dtb, node, "qcom,batt-id-vref-uv", &len);
	if (len != sizeof(vref))
		return 0;
	vref = fdt32_to_cpu(*prop);

	prop = fdt_getprop(dtb, node, "qcom,batt-id-rpullup-kohm", &len);
	if (len != sizeof(rpull))
		return 0;
	rpull = fdt32_to_cpu(*prop);

	batt_id_uv = pm8x41_adc_channel_read(VADC_BAT_CHAN_ID);
	if (batt_id_uv == 0)
		return 0;

	denom = (vref * 1000000LL / batt_id_uv) - 1000000LL;
	if (denom == 0)
		return 0;

	connected_rid = (rpull * 1000000LL + (denom / 2)) / denom;

	dprintf(INFO, "smb1360: batt_id_uv: %llu, connected_rid: %u\n",
		batt_id_uv, connected_rid);

	return connected_rid;
}

static int smb1360_qcom_detect_battery(const void *dtb, int node)
{
	const struct smb1360_battery *battery = NULL;
	uint32_t connected_rid;
	int len;

	fdt_getprop(dtb, node, "qcom,batt-profile-select", &len);
	if (len < 0)
		return 0;

	connected_rid = smb1360_connected_rid(dtb, node);
	if (connected_rid) {
		if (smb1360_check_profile(dtb, node, "qcom,profile-a-rid-kohm",
					  connected_rid))
			battery = &batteryA;
		else if (smb1360_check_profile(dtb, node, "qcom,profile-b-rid-kohm",
					       connected_rid))
			battery = &batteryB;
	}
	return smb1360_battery_detected(battery);
}
LK2ND_DEVICE_INIT("qcom,smb1360", smb1360_qcom_detect_battery);
