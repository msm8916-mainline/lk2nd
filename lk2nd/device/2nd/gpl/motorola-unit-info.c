// SPDX-License-Identifier: GPL-2.0-only

#include <boot.h>
#include <debug.h>
#include <libfdt.h>
#include <smem.h>
#include <string.h>

#include "../../device.h"
#include "motorola-unit-info.h"

#define SMEM_ID_VENDOR0			0x86
#define SMEM_KERNEL_RESERVE		SMEM_ID_VENDOR0
#define SMEM_KERNEL_RESERVE_SIZE	1024

static const struct mmi_unit_info *mmi_unit_info;

static void readprop_u32(const void *dtb, int node, const char *name, uint32_t *val) {
	int len;
	const fdt32_t *prop = fdt_getprop(dtb, node, name, &len);
	if (len == sizeof(*val))
		*val = fdt32_to_cpu(*prop);
	else
		dprintf(CRITICAL, "Failed to get prop '%s' in device tree\n", name);
}

static void print_unit_info(const struct mmi_unit_info *info)
{
	dprintf(INFO, "Motorola unit info v%d: rev=%#04x, serial=%#08x%08x, "
		"machine='%s', barcode='%s', carrier='%s', baseband='%s', device='%s', "
		"radio='%s' (%#x), powerup_reason=%#08x\n",
		info->version, info->system_rev, info->system_serial_high, info->system_serial_low,
		info->machine, info->barcode, info->carrier, info->baseband, info->device,
		info->radio_str, info->radio, info->powerup_reason);
}

static int motorola_unit_info(const void *dtb, int node)
{
	struct mmi_unit_info *info;
	const char *baseband;
	int chosen, len;
	uint32_t size;

	info = smem_get_alloc_entry(SMEM_KERNEL_RESERVE, &size);
	if (info) {
		dprintf(INFO, "NOTE: Keeping existing Motorola unit info from previous bootloader\n");
		if (size < sizeof(*info)) {
			dprintf(CRITICAL, "Motorola unit info has wrong size: %d\n", size);
			return 1;
		}

		mmi_unit_info = info;
		print_unit_info(info);
		return 0;
	}

	chosen = fdt_path_offset(dtb, "/chosen");
	if (chosen < 0) {
		dprintf(CRITICAL, "Failed to get /chosen node in device tree: %d\n", chosen);
		return chosen;
	}

	info = smem_alloc_entry(SMEM_KERNEL_RESERVE, SMEM_KERNEL_RESERVE_SIZE);
	if (!info)
		return 1;

	memset(info, 0, SMEM_KERNEL_RESERVE_SIZE);
	info->version = MMI_UNIT_INFO_VER;

	readprop_u32(dtb, chosen, "linux,hwrev", &info->system_rev);
	readprop_u32(dtb, chosen, "linux,seriallow", &info->system_serial_low);
	readprop_u32(dtb, chosen, "linux,serialhigh", &info->system_serial_high);

	if (lk2nd_dev.serialno)
		strlcpy(info->barcode, lk2nd_dev.serialno, BARCODE_MAX_LEN);
	if (lk2nd_dev.carrier)
		strlcpy(info->carrier, lk2nd_dev.carrier, CARRIER_MAX_LEN);

	baseband = fdt_getprop(dtb, chosen, "mmi,baseband", &len);
	if (len > 0)
		strlcpy(info->baseband, baseband, MIN(len, BASEBAND_MAX_LEN));

	if (lk2nd_dev.device)
		strlcpy(info->device, lk2nd_dev.device, DEVICE_MAX_LEN);
	if (lk2nd_dev.radio) {
		info->radio = atoui(lk2nd_dev.radio);
		strlcpy(info->radio_str, lk2nd_dev.radio, RADIO_MAX_LEN);
	}

	readprop_u32(dtb, chosen, "mmi,powerup_reason", &info->powerup_reason);

	mmi_unit_info = info;
	print_unit_info(info);
	return 0;
}
LK2ND_DEVICE_INIT("motorola,unit-info", motorola_unit_info);

static int motorola_unit_info_update_dt(void *dtb, const char *cmdline,
					enum boot_type boot_type)
{
	const struct mmi_unit_info *info = mmi_unit_info;
	int chosen, len;

	if (!info || !(boot_type & BOOT_DOWNSTREAM))
		return 0;

	chosen = fdt_path_offset(dtb, "/chosen");
	if (chosen < 0) {
		dprintf(CRITICAL, "Failed to get /chosen node in device tree: %d\n", chosen);
		return 0; /* Still continue boot */
	}

	/*
	 * Copy the values from SMEM into the downstream DTB since the driver
	 * from Motorola in the downstream kernel insists on writing the whole
	 * unit info again...
	 */
	if (info->system_rev)
		fdt_setprop_u32(dtb, chosen, "linux,hwrev", info->system_rev);
	if (info->system_serial_low)
		fdt_setprop_u32(dtb, chosen, "linux,seriallow", info->system_serial_low);
	if (info->system_serial_high)
		fdt_setprop_u32(dtb, chosen, "linux,serialhigh", info->system_serial_high);

	len = strnlen(info->baseband, BASEBAND_MAX_LEN);
	if (len > 0)
		fdt_setprop(dtb, chosen, "mmi,baseband", info->baseband, len + 1);

	if (info->powerup_reason)
		fdt_setprop_u32(dtb, chosen, "mmi,powerup_reason", info->powerup_reason);

	return 0;
}
DEV_TREE_UPDATE(motorola_unit_info_update_dt);
