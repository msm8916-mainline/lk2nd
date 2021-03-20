// SPDX-License-Identifier: GPL-2.0-only

#include <debug.h>
#include <pm8x41.h>
#include <pm8x41_adc.h>

#include "../../device.h"
#include "../smb1360.h"

static const struct smb1360_battery battery_tlp020k2 = {
	.name = "TLP020K2",
	.rslow_config = (const uint8_t[]) { 0x71, 0x73, 0x29, 0x78 },
};

static const struct smb1360_battery battery_tlk020kj = {
	.name = "TLK020KJ",
	.rslow_config = (const uint8_t[]) { 0xDA, 0x71, 0x34, 0x71 },
};

static int smb1360_idol347_detect_battery(const void *dtb, int node)
{
	const struct smb1360_battery *battery = NULL;
	uint32_t batt_id_uv = pm8x41_adc_channel_read(VADC_BAT_CHAN_ID);

	dprintf(INFO, "idol347: batt_id_uv = %u\n", batt_id_uv);

	if (batt_id_uv > 500000 && batt_id_uv < 700000) // ~606000
		battery = &battery_tlp020k2;
	else if (batt_id_uv > 0 && batt_id_uv < 100000) // ~32000
		battery = &battery_tlk020kj;

	return smb1360_battery_detected(battery);
}
LK2ND_DEVICE_INIT("alcatel,smb1360-idol347", smb1360_idol347_detect_battery);
