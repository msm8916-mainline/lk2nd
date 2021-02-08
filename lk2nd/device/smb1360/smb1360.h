/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_DEVICE_SMB1360_H
#define LK2ND_DEVICE_SMB1360_H

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
};

int smb1360_battery_detected(const struct smb1360_battery *battery);

#endif /* LK2ND_DEVICE_SMB1360_H */
