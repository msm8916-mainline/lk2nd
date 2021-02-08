/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_SMB1360_H
#define __LK2ND_SMB1360_H

struct smb1360_battery {
	const char *name;
	uint16_t capacity_mah;
	uint16_t cc_soc_coeff;
	uint16_t therm_coeff;
};

void smb1360_detect_battery(const void *fdt, int offset);
void smb1360_update_device_tree(void *fdt);

const struct smb1360_battery *smb1360_wt88047_detect_battery(const void *fdt, int offset);

#endif
