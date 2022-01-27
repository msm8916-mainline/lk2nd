// SPDX-License-Identifier: GPL-2.0-only
#include <reg.h>
#include <stdio.h>
#include "fastboot.h"

#define EFUSE	0xfc4b80b0

#define readq(a)	(*REG64(a))

static void cmd_oem_dump_speedbin(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	uint32_t redundant_fuse_select, speed_bin, pvs, pvs_version, pvs_valid;
	/*
	 * Format of efuse (8 bytes):
	 * [53] PVS valid
	 * [31] PVS (bit 3)
	 * [29:27] redundant fuse
	 * [26:24] redundant fuse select
	 * [8:6] PVS (bit 2 & 1)
	 * [5:4] PVS version
	 * [2:0] speed bin
	 */
	uint64_t efuse = readq(EFUSE);

	speed_bin = efuse & 0x7;
	pvs = ((efuse >> 28) & 0x8) | ((efuse >> 6) & 0x7);
	pvs_version = (efuse >> 4) & 0x3;
	pvs_valid = (efuse >> 53) & 0x1;

	redundant_fuse_select = (efuse >> 24) & 0x7;
	switch (redundant_fuse_select) {
	/* Use redundant fuse for speed_bin */
	case 0x1:
		speed_bin = (efuse >> 27) & 0xf;
		break;
	/* Use redundant fuse for PVS */
	case 0x2:
		pvs = (efuse >> 27) & 0xf;
		break;
	}

	snprintf(response, sizeof(response),
		 "PVS valid: %d, Redundant fuse select: %d",
		 pvs_valid, redundant_fuse_select);
	fastboot_info(response);

	snprintf(response, sizeof(response),
		 "Speed bin: %d, PVS: %d, PVS version: %d (speed%d-pvs%d-v%d)",
		 speed_bin, pvs, pvs_version, speed_bin, pvs, pvs_version);
	fastboot_info(response);

	fastboot_okay("");
}

void target_fastboot_register_commands(void) {
	fastboot_register("oem dump-speedbin", cmd_oem_dump_speedbin);
}
