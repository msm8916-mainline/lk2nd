// SPDX-License-Identifier: GPL-2.0-only

#include <debug.h>
#include <libfdt.h>
#include <pm8x41.h>
#include <pm8x41_adc.h>
#include "smb1360.h"

static const struct smb1360_battery batteryA = {
	.name = "Profile A",
	.profile = SMB1360_BATTERY_PROFILE_A,
};

static const struct smb1360_battery batteryB = {
	.name = "Profile B",
	.profile = SMB1360_BATTERY_PROFILE_B,
};

static unsigned int abs(int n) {
	if (n >= 0)
		return n;
	return -n;
}

static const struct smb1360_battery *smb1360_check_batt_profile(const void *fdt, int offset,
								uint32_t connected_rid)
{
	const uint32_t *prop;
	int len, loaded_profile;
	uint32_t profile_rid_a, profile_rid_b, rid10;

	prop = fdt_getprop(fdt, offset, "qcom,profile-a-rid-kohm", &len);
	if (len != sizeof(profile_rid_a))
		return NULL;
	profile_rid_a = fdt32_to_cpu(*prop);

	prop = fdt_getprop(fdt, offset, "qcom,profile-b-rid-kohm", &len);
	if (len != sizeof(profile_rid_b))
		return NULL;
	profile_rid_b = fdt32_to_cpu(*prop);

	rid10 = connected_rid / 10;

	if (abs(profile_rid_a - connected_rid) < rid10) {
		return &batteryA;
	} else if (abs(profile_rid_b - connected_rid) < rid10) {
		return &batteryB;
	} else {
		dprintf(CRITICAL, "smb1360: connected_rid: %d does not match profile A (%d) or profile B (%d)\n",
			connected_rid, profile_rid_a, profile_rid_b);
		return NULL;
	}
}

static uint32_t smb1360_parse_batt_id(const void *fdt, int offset)
{
	const uint32_t *prop;
	int64_t denom, batt_id_uv;
	int32_t len, rpull = 0, vref = 0;
	uint32_t connected_rid;

	prop = fdt_getprop(fdt, offset, "qcom,batt-id-vref-uv", &len);
	if (len != sizeof(vref))
		return 0;
	vref = fdt32_to_cpu(*prop);

	prop = fdt_getprop(fdt, offset, "qcom,batt-id-rpullup-kohm", &len);
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

	dprintf(SPEW, "smb1360: batt-id voltage: %lld, connected_rid: %d\n", batt_id_uv, connected_rid);

	return connected_rid;
}

const struct smb1360_battery *smb1360_qcom_detect_battery(const struct smb1360 *smb,
							  const void *fdt, int offset)
{
	int len;
	uint32_t connected_rid;

	fdt_getprop(fdt, offset, "qcom,batt-profile-select", &len);
	if (len < 0)
		return NULL;

	connected_rid = smb1360_parse_batt_id(fdt, offset);
	if (connected_rid == 0) {
		dprintf(CRITICAL, "smb1360: failed to detect connected_rid\n");
		return NULL;
	}

	return smb1360_check_batt_profile(fdt, offset, connected_rid);
}
