/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_SMB1360_H
#define __LK2ND_SMB1360_H

struct smb1360;

enum smb1360_battery_profile {
	SMB1360_BATTERY_PROFILE_KEEP,
	SMB1360_BATTERY_PROFILE_A,
	SMB1360_BATTERY_PROFILE_B,
};

struct smb1360_battery {
	const char *name;
	enum smb1360_battery_profile profile;
	uint16_t capacity_mah;
	uint16_t cc_soc_coeff;
	uint16_t therm_coeff;
	uint8_t *rslow_config;
};

void smb1360_detect_battery(const void *fdt, int offset);
void smb1360_update_device_tree(void *fdt);

const struct smb1360_battery *smb1360_idol347_detect_battery(const struct smb1360 *smb,
							     const void *fdt, int offset);
const struct smb1360_battery *smb1360_wt88047_detect_battery(const struct smb1360 *smb,
							     const void *fdt, int offset);
const struct smb1360_battery *smb1360_qcom_detect_battery(const struct smb1360 *smb,
							  const void *fdt, int offset);

#endif
