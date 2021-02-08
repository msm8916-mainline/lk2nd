// SPDX-License-Identifier: GPL-2.0-only

#include <debug.h>
#include <libfdt.h>
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

const struct smb1360_battery *smb1360_qcom_detect_battery(const void *fdt, int offset)
{
	int len;
	uint32_t volt;

	fdt_getprop(fdt, offset, "qcom,batt-profile-select", &len);
	if (len < 0)
		return NULL;

	// TODO: Read qcom,batt-id-vref-uv etc and do stuff

	volt = pm8x41_adc_channel_read(VADC_BAT_CHAN_ID);
	dprintf(INFO, "batt_id_v = %u\n", volt);
	// Note: batt_id_v seems close but also slightly different from mainline/downstream
	// I wonder where all the different values come from

	return &batteryA; // TODO
}
