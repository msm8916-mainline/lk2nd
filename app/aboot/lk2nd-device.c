// SPDX-License-Identifier: GPL-2.0-only

#include <arch/arm.h>
#include <board.h>
#include <debug.h>
#include <stdlib.h>
#include <string.h>
#include <libfdt.h>
#include "lk2nd-device.h"

struct lk2nd_device lk2nd_dev = {0};
extern struct board_data board;

static void update_board_id(struct board_id *board_id)
{
	uint32_t hw_id = board_id->variant_id & 0xff;
	uint32_t hw_subtype = board_id->platform_subtype & 0xff;
	uint32_t target_id = board_id->variant_id & 0xffff00;

	if (board_hardware_id() != hw_id) {
		dprintf(INFO, "Updating board hardware id: 0x%x -> 0x%x\n",
			board_hardware_id(), hw_id);
		board.platform_hw = hw_id;
	}

	if (board_hardware_subtype() != hw_subtype) {
		dprintf(INFO, "Updating board hardware subtype: 0x%x -> 0x%x\n",
			board_hardware_subtype(), hw_subtype);
		board.platform_subtype = hw_subtype;
	}

	if (!(target_id < (board_target_id() & 0xffff00))) {
		target_id |= board_target_id() & ~0xffff00;
		dprintf(INFO, "Updating board target id: 0x%x -> 0x%x\n",
			board_target_id(), target_id);
		board.target = target_id;
	}
}

static const char *strpresuf(const char *str, const char *pre)
{
	int len = strlen(pre);
	return strncmp(pre, str, len) == 0 ? str + len : NULL;
}

static void parse_boot_args(void)
{
	char *saveptr;
	char *args = strdup(lk2nd_dev.cmdline);

	char *arg = strtok_r(args, " ", &saveptr);
	while (arg) {
		const char *val;
		if ((val = strpresuf(arg, "androidboot.bootloader="))) {
			lk2nd_dev.bootloader = strdup(val);
		}

		arg = strtok_r(NULL, " ", &saveptr);
	}

	free(args);
}

static const char *fdt_copyprop_str(const void *fdt, int offset, const char *prop)
{
	int len;
	const char *val;
	char *result = NULL;

	val = fdt_getprop(fdt, offset, prop, &len);
	if (val && len > 0) {
		result = (char*) malloc(sizeof(char) * len);
		ASSERT(result);
		strlcpy(result, val, len);
	}
	return result;
}

static bool lk2nd_device_match(const void *fdt, int offset)
{
	int len;
	const char *val;

	val = fdt_getprop(fdt, offset, "lk2nd,match-bootloader", &len);
	if (val && len > 0) {
		if (!lk2nd_dev.bootloader) {
			if (lk2nd_dev.cmdline)
				dprintf(CRITICAL, "Unknown bootloader, cannot match\n");
			return false;
		}

		if (len > 1 && val[len-2] == '*')
			len -= 2; // Match prefix only
		if (strncmp(lk2nd_dev.bootloader, val, len) == 0)
			return true;

		return false;
	}

	return true; // No bootloader property
}

static int lk2nd_find_device_offset(const void *fdt)
{
	int offset;

	offset = fdt_node_offset_by_compatible(fdt, -1, "lk2nd,device");
	while (offset >= 0) {
		if (lk2nd_device_match(fdt, offset))
			return offset;

		offset = fdt_node_offset_by_compatible(fdt, offset, "lk2nd,device");
	}

	return offset;
}

void lk2nd_parse_device_node(const void *fdt)
{
	int offset = lk2nd_find_device_offset(fdt);
	if (offset < 0) {
		dprintf(CRITICAL, "Failed to find matching lk2nd,device node: %d\n", offset);
		return;
	}

	lk2nd_samsung_muic_reset(fdt, offset);

	lk2nd_dev.model = fdt_copyprop_str(fdt, offset, "model");
	if (lk2nd_dev.model)
		dprintf(INFO, "Device model: %s\n", lk2nd_dev.model);
	else
		dprintf(CRITICAL, "Device node is missing 'model' property\n");
}


int lk2nd_fdt_parse_early_uart(void)
{
	int offset, len;
	const uint32_t *val;

	void *fdt = (void*) lk_boot_args[2];
	if (!fdt || dev_tree_check_header(fdt))
		return -1; // Will be reported later again. Hopefully.

	offset = lk2nd_find_device_offset(fdt);
	if (offset < 0)
		return -1;

	/* TODO: Change this to use chosen node */
	val = fdt_getprop(fdt, offset, "lk2nd,uart", &len);
	if (len > 0)
		return fdt32_to_cpu(*val);
	return -1;
}

void lk2nd_fdt_parse(void)
{
	struct board_id board_id;
	void *fdt = (void*) lk_boot_args[2];
	if (!fdt)
		return;

	if (dev_tree_check_header(fdt)) {
		dprintf(INFO, "Invalid device tree provided by primary bootloader\n");
		return;
	}

	if (dev_tree_get_board_id(fdt, &board_id) == 0) {
		update_board_id(&board_id);
	}

	lk2nd_dev.cmdline = dev_tree_get_boot_args(fdt);
	if (lk2nd_dev.cmdline) {
		dprintf(INFO, "Command line from primary bootloader: ");
		dputs(INFO, lk2nd_dev.cmdline);
		dputc(INFO, '\n');

		parse_boot_args();
	}

	lk2nd_parse_device_node(fdt);
}
