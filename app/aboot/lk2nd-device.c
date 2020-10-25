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

static void dump_board()
{
	unsigned i;

	dprintf(INFO, "Board: platform: %u, foundry: %#x, platform_version: %#x, "
		      "platform_hw: %#x, platform_subtype: %#x, target: %#x, "
		      "baseband: %#x, platform_hlos_subtype: %#x\n",
		board.platform, board.foundry_id, board.platform_version,
		board.platform_hw, board.platform_subtype, board.target,
		board.baseband, board.platform_hlos_subtype);

	for (i = 0; i < MAX_PMIC_DEVICES; ++i) {
		if (board.pmic_info[i].pmic_type == PMIC_IS_INVALID)
			continue;

		dprintf(INFO, "pmic_info[%u]: type: %#x, version: %#x, target: %#x\n",
			i, board.pmic_info[i].pmic_type,
			board.pmic_info[i].pmic_version,
			board.pmic_info[i].pmic_target);
	}
}

static void update_board_id(struct board_id *board_id)
{
	uint32_t hw_id = board_id->variant_id & 0xff;
	uint32_t hw_subtype = board_id->platform_subtype & 0xff;
	uint32_t target_id = board_id->variant_id & 0xffff00;

	/* See platform_dt_absolute_match() for the checks made here */
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

	if (!(target_id <= (board_target_id() & 0xffff00))) {
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

static inline void parse_arg(const char *str, const char *pre, const char **out)
{
	const char *val = strpresuf(str, pre);
	if (val)
		*out = strdup(val);
}

static const char *parse_panel(const char *panel)
{
	const char *panel_name;
	char *end;

	if (!panel)
		return NULL;

	/*
	 * Clean up a bit when arg looks like 1:dsi:0:<panel-name>:...
	 * It could look different, but I don't know how to handle that yet.
	 */
	panel_name = strpresuf(panel, "1:dsi:0:");
	if (!panel_name) /* Some other format */
		return NULL;

	/* Cut off other garbage at the end of the string (e.g. :1:none) */
	end = strchr(panel_name, ':');
	if (end)
		*end = 0;

	/* If this isn't the main panel we don't really know how to deal with this */
	if (strcmp(panel_name, "none") == 0)
		return NULL;

	return panel_name;
}

static void parse_boot_args(void)
{
	char *saveptr;
	char *args = strdup(lk2nd_dev.cmdline);
	char *arg = strtok_r(args, " ", &saveptr);
	while (arg) {
		const char *aboot = strpresuf(arg, "androidboot.");

		if (aboot) {
			parse_arg(aboot, "device=", &lk2nd_dev.device);
			parse_arg(aboot, "bootloader=", &lk2nd_dev.bootloader);
			parse_arg(aboot, "serialno=", &lk2nd_dev.serialno);
			parse_arg(aboot, "carrier=", &lk2nd_dev.carrier);
			parse_arg(aboot, "radio=", &lk2nd_dev.radio);
		} else {
			parse_arg(arg, "mdss_mdp.panel=", &lk2nd_dev.panel.name);
		}

		arg = strtok_r(NULL, " ", &saveptr);
	}
	free(args);

	lk2nd_dev.panel.name = parse_panel(lk2nd_dev.panel.name);
}

static const char *fdt_copyprop_str(const void *fdt, int offset, const char *prop)
{
	int len;
	const char *val;
	char *result = NULL;

	val = fdt_getprop(fdt, offset, prop, &len);
	if (val && len > 0) {
		result = malloc(len);
		ASSERT(result);
		strlcpy(result, val, len);
	}
	return result;
}

static bool match_string(const char *s, const char *match, size_t len)
{
	len = strnlen(match, len);

	if (len > 0) {
		if (match[len-1] == '*') {
			if (len > 1 && match[0] == '*')
				// *contains*
				return !!strstrl(s, match + 1, len - 2);

			// prefix* (starts with)
			len -= 2; // Don't match null terminator and '*'
		} else if (match[0] == '*') {
			// *suffix (ends with)
			size_t slen = strlen(s);
			if (slen < --len)
				return false;

			++match; // Skip '*'
			s += slen - len; // Move to end of string
		}
	}

	return strncmp(s, match, len + 1) == 0;
}

static bool match_panel(const void *fdt, int offset, const char *panel_name)
{
	offset = fdt_subnode_offset(fdt, offset, "panel");
	if (offset < 0) {
		dprintf(CRITICAL, "No panels defined, cannot match\n");
		return false;
	}

	return fdt_subnode_offset(fdt, offset, panel_name) >= 0;
}

static bool lk2nd_device_match(const void *fdt, int offset)
{
	int len;
	const char *val;

	val = fdt_getprop(fdt, offset, "lk2nd,match-bootloader", &len);
	if (val && len > 0) {
		if (!lk2nd_dev.bootloader)
			return false;
		return match_string(lk2nd_dev.bootloader, val, len);
	}

	val = fdt_getprop(fdt, offset, "lk2nd,match-cmdline", &len);
	if (val && len > 0) {
		if (!lk2nd_dev.cmdline)
			return false;
		return match_string(lk2nd_dev.cmdline, val, len);
	}

	fdt_getprop(fdt, offset, "lk2nd,match-panel", &len);
	if (len >= 0) {
		if (!lk2nd_dev.panel.name)
			return false;
		return match_panel(fdt, offset, lk2nd_dev.panel.name);
	}

	return true; // No match property
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

static const char *fdt_getprop_str(const void *fdt, int offset, const char *prop, int *len)
{
	const char *val;

	val = fdt_getprop(fdt, offset, prop, len);
	if (!val || *len < 1)
		return NULL;
	*len = strnlen(val, *len);
	return val;
}

static void lk2nd_parse_panels(const void *fdt, int offset)
{
	struct lk2nd_panel *panel = &lk2nd_dev.panel;
	const char *old, *new;
	int old_len, new_len;

	offset = fdt_subnode_offset(fdt, offset, "panel");
	if (offset < 0)
		return;

	old = fdt_getprop_str(fdt, offset, "compatible", &old_len);
	if (!old || old_len < 1)
		return;

	offset = fdt_subnode_offset(fdt, offset, panel->name);
	if (offset < 0) {
		dprintf(CRITICAL, "Unsupported panel: %s\n", panel->name);
		return;
	}

	new = fdt_getprop_str(fdt, offset, "compatible", &new_len);
	if (!new || new_len < 1)
		return;

	/* Include space required for null-terminators */
	panel->compatible_size = ++new_len + ++old_len;

	panel->compatible = malloc(panel->compatible_size);
	ASSERT(panel->compatible);
	panel->old_compatible = panel->compatible + new_len;

	strlcpy((char*) panel->compatible, new, new_len);
	strlcpy((char*) panel->old_compatible, old, old_len);
}

static void lk2nd_parse_device_node(const void *fdt)
{
	int offset = lk2nd_find_device_offset(fdt);
	if (offset < 0) {
		dprintf(CRITICAL, "Failed to find matching lk2nd,device node: %d\n", offset);
		return;
	}

#ifdef GPIO_I2C_BUS_COUNT
	lk2nd_samsung_muic_reset(fdt, offset);
#endif
	lk2nd_motorola_smem_write_unit_info(fdt, offset);

	lk2nd_dev.model = fdt_copyprop_str(fdt, offset, "model");
	if (lk2nd_dev.model)
		dprintf(INFO, "Device model: %s\n", lk2nd_dev.model);
	else
		dprintf(CRITICAL, "Device node is missing 'model' property\n");

	if (dev_tree_get_board_id(fdt, offset, &lk2nd_dev.board_id) == 0)
		update_board_id(&lk2nd_dev.board_id);

	if (lk2nd_dev.panel.name)
		lk2nd_parse_panels(fdt, offset);
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

static void lk2nd_fdt_parse(void)
{
	void *fdt = (void*) lk_boot_args[2];
	if (!fdt)
		return;

	if (dev_tree_check_header(fdt)) {
		dprintf(INFO, "Invalid device tree provided by primary bootloader\n");
		return;
	}

	lk2nd_dev.fdt = fdt;
	if (dev_tree_get_board_id(fdt, 0, &lk2nd_dev.board_id) == 0)
		update_board_id(&lk2nd_dev.board_id);
	else
		dprintf(INFO, "No valid qcom,board-id in device tree\n");

	lk2nd_dev.cmdline = dev_tree_get_boot_args(fdt);
	if (lk2nd_dev.cmdline) {
		dprintf(INFO, "Command line from primary bootloader: ");
		dputs(INFO, lk2nd_dev.cmdline);
		dputc(INFO, '\n');

		parse_boot_args();
	}

	lk2nd_parse_device_node(fdt);
}

void lk2nd_init(void)
{
	dump_board();
	lk2nd_fdt_parse();
}

static void lk2nd_update_panel_compatible(void *fdt)
{
	struct lk2nd_panel *panel = &lk2nd_dev.panel;
	int offset, ret;

	/* Try to find panel node */
	offset = fdt_node_offset_by_compatible(fdt, -1, panel->old_compatible);
	if (offset < 0) {
		dprintf(CRITICAL, "Failed to find panel node with compatible: %s\n",
			panel->old_compatible);
		return;
	}

	ret = fdt_setprop(fdt, offset, "compatible", panel->compatible, panel->compatible_size);
	if (ret)
		dprintf(CRITICAL, "Failed to update panel compatible: %d\n", ret);
}

void lk2nd_update_device_tree(void *fdt, const char *cmdline)
{
	/* Don't touch lk2nd/downstream dtb */
	if (strstr(cmdline, "lk2nd"))
		return;

	if (lk2nd_dev.panel.compatible)
		lk2nd_update_panel_compatible(fdt);
}
