// SPDX-License-Identifier: GPL-2.0-only

#include <debug.h>
#include <libfdt.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "lk2nd-device.h"
#include "lk2nd-motorola.h"
#include "smem.h"

#define SMEM_ID_VENDOR0          0x86
#define SMEM_KERNEL_RESERVE      SMEM_ID_VENDOR0
#define SMEM_KERNEL_RESERVE_SIZE 1024

static void fdt_getprop_u32(const void *fdt, int offset, const char *name, uint32_t *val) {
	int len;
	const uint32_t *prop = fdt_getprop(fdt, offset, name, &len);
	if (len == sizeof(*val))
		*val = fdt32_to_cpu(*prop);
	else
		dprintf(CRITICAL, "Failed to get prop %s in device tree\n", name);
}

void lk2nd_motorola_smem_write_unit_info(const void *fdt, int offset)
{
	int chosen_offset, len, ret = 0;
	const char *baseband;
	struct mmi_unit_info *unit_info;

	fdt_getprop(fdt, offset, "lk2nd,motorola-unitinfo", &len);
	if (len < 0)
		return;

	chosen_offset = fdt_path_offset(fdt, "/chosen");
	if (chosen_offset < 0) {
		dprintf(CRITICAL, "Failed to get /chosen node in device tree\n");
		return;
	}

	unit_info = malloc(SMEM_KERNEL_RESERVE_SIZE);
	memset(unit_info, 0, SMEM_KERNEL_RESERVE_SIZE);

	unit_info->version = MMI_UNIT_INFO_VER;

	if (lk2nd_dev.serialno)
		strlcpy(unit_info->barcode, lk2nd_dev.serialno, BARCODE_MAX_LEN);
	if (lk2nd_dev.carrier)
		strlcpy(unit_info->carrier, lk2nd_dev.carrier, CARRIER_MAX_LEN);
	if (lk2nd_dev.device)
		strlcpy(unit_info->device, lk2nd_dev.device, DEVICE_MAX_LEN);
	if (lk2nd_dev.radio) {
		unit_info->radio = atoui(lk2nd_dev.radio);
		strlcpy(unit_info->radio_str, lk2nd_dev.radio, RADIO_MAX_LEN);
	}

	fdt_getprop_u32(fdt, chosen_offset, "linux,hwrev", &unit_info->system_rev);
	fdt_getprop_u32(fdt, chosen_offset, "linux,seriallow", &unit_info->system_serial_low);
	fdt_getprop_u32(fdt, chosen_offset, "linux,serialhigh", &unit_info->system_serial_high);
	fdt_getprop_u32(fdt, chosen_offset, "mmi,powerup_reason", &unit_info->powerup_reason);

	baseband = fdt_getprop(fdt, chosen_offset, "mmi,baseband", &len);
	if (len >= 0)
		strlcpy(unit_info->baseband, baseband, MIN(len, BASEBAND_MAX_LEN));

	dprintf(INFO, "Setting motorola unit info: version = 0x%02x, device = '%s', radio = 0x%x, radio_str = '%s', system_rev = 0x%04x, system_serial = 0x%08x%08x, machine = '%s', barcode = '%s', baseband = '%s', carrier = '%s', pu_reason = 0x%08x\n",
		unit_info->version,
		unit_info->device,
		unit_info->radio,
		unit_info->radio_str,
		unit_info->system_rev,
		unit_info->system_serial_high, unit_info->system_serial_low,
		unit_info->machine, unit_info->barcode,
		unit_info->baseband, unit_info->carrier,
		unit_info->powerup_reason);

	ret = smem_alloc_write_entry(SMEM_KERNEL_RESERVE, unit_info, SMEM_KERNEL_RESERVE_SIZE);
	if (ret != 0)
		dprintf(CRITICAL, "Failed to write motorola unit info to smem: %d\n", ret);
	else
		dprintf(INFO, "Wrote motorola unit info\n");

	free(unit_info);
}
