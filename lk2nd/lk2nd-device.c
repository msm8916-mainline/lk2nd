// SPDX-License-Identifier: GPL-2.0-only

#include <arch/arm.h>
#include <board.h>
#include <debug.h>
#include <libfdt.h>
#include <stdlib.h>
#include <string.h>

#include <lk2nd.h>
#include "smb1360/smb1360.h"

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
	const char *panel_name = NULL;

	while (arg) {
		const char *suffix;

		if (suffix = strpresuf(arg, "androidboot.")) {
			parse_arg(suffix, "device=", &lk2nd_dev.device);
			parse_arg(suffix, "bootloader=", &lk2nd_dev.bootloader);
			parse_arg(suffix, "serialno=", &lk2nd_dev.serialno);
			parse_arg(suffix, "carrier=", &lk2nd_dev.carrier);
			parse_arg(suffix, "radio=", &lk2nd_dev.radio);
		} else if (suffix = strpresuf(arg, "lk2nd.")) {
			parse_arg(suffix, "compatible=", &lk2nd_dev.compatible);
			parse_arg(suffix, "panel=", &lk2nd_dev.panel.name);
		} else {
			parse_arg(arg, "mdss_mdp.panel=", &panel_name);
		}

		arg = strtok_r(NULL, " ", &saveptr);
	}
	free(args);

	if (!lk2nd_dev.panel.name)
		lk2nd_dev.panel.name = parse_panel(panel_name);
}

static char *strcpy_check_end(char *dst, const char *end, const char *src)
{
	for (; *src && dst < end; src++, dst++)
		*dst = *src;
	return dst;
}

#define LK1ST_MAX_CMDLINE_SIZE	256

char *genlk1st2lk2ndcmdline(void)
{
	char *cmdline = malloc(LK1ST_MAX_CMDLINE_SIZE);
	char *dst = cmdline;
	char *end = cmdline + LK1ST_MAX_CMDLINE_SIZE - 1; /* null terminator */

	ASSERT(cmdline);

	if (lk2nd_dev.compatible) {
		dst = strcpy_check_end(dst, end, "lk2nd.compatible=");
		dst = strcpy_check_end(dst, end, lk2nd_dev.compatible);
		*dst++ = ' ';
	}
	if (lk2nd_dev.panel.name) {
		dst = strcpy_check_end(dst, end, "lk2nd.panel=");
		dst = strcpy_check_end(dst, end, lk2nd_dev.panel.name);
		*dst++ = ' ';
	}

	/* Replace last space with null terminator */
	if (dst > cmdline)
		*--dst = '\0';

	return cmdline;
}

static const char *lkfdt_copyprop_str(const void *fdt, int offset, const char *prop)
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

	if (lk2nd_dev.compatible)
		return fdt_node_check_compatible(fdt, offset, lk2nd_dev.compatible) == 0;

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

static const char *lkfdt_getprop_str(const void *fdt, int offset, const char *prop, int *len)
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
	const char *old, *new, *ts;
	int old_len, new_len, ts_len;

	offset = fdt_subnode_offset(fdt, offset, "panel");
	if (offset < 0)
		return;

	old = lkfdt_getprop_str(fdt, offset, "compatible", &old_len);
	if (!old || old_len < 1)
		return;

	offset = fdt_subnode_offset(fdt, offset, panel->name);
	if (offset < 0) {
		dprintf(CRITICAL, "Unsupported panel: %s\n", panel->name);
		return;
	}

	new = lkfdt_getprop_str(fdt, offset, "compatible", &new_len);
	if (!new || new_len < 1)
		return;

	/* Include space required for null-terminators */
	panel->compatible_size = ++new_len + ++old_len;

	panel->compatible = malloc(panel->compatible_size);
	ASSERT(panel->compatible);
	panel->old_compatible = panel->compatible + new_len;

	strlcpy((char*) panel->compatible, new, new_len);
	strlcpy((char*) panel->old_compatible, old, old_len);

	ts = lkfdt_getprop_str(fdt, offset, "touchscreen-compatible", &ts_len);
	if (!ts || ts_len < 1)
		return;

	panel->ts_compatible = malloc(ts_len);
	ASSERT(panel->ts_compatible);
	strlcpy((char*) panel->ts_compatible, ts, ts_len + 1);

	dprintf(INFO, "Found touchscreen-compatible: %s\n", panel->ts_compatible);
}

static struct lk2nd_keymap* lk2nd_parse_keys(const void *fdt, int offset)
{
	int len;
	const uint32_t *val;
	struct lk2nd_keymap *map = NULL;

#define KEY_SIZE (3 * sizeof(uint32_t))
	int i = 0;
	val = fdt_getprop(fdt, offset, "lk2nd,keys", &len);
	if (len > 0 && len % KEY_SIZE == 0) {
		len /= KEY_SIZE;
		/* last element indicates end of the array with key=0 */
		map = calloc(len + 1, sizeof(struct lk2nd_keymap));
		for (int i = 0; i < len; i++) {
			map[i].key    = fdt32_to_cpu(val[i*3]);
			map[i].gpio   = fdt32_to_cpu(val[i*3 + 1]) & 0xFFFF;
			map[i].type   = fdt32_to_cpu(val[i*3 + 1]) >> 16;
			map[i].pull   = fdt32_to_cpu(val[i*3 + 2]) & 0xFF;
			map[i].active = fdt32_to_cpu(val[i*3 + 2]) >> 8;
		}
	}

	dprintf(INFO, "Device keymap:\n");
	while (map && map[i].key) {
		dprintf(INFO, "key=0x%X, gpio=%x, type=%d, pull=%d, active=%d\n",
			map[i].key, map[i].gpio, map[i].type, map[i].pull, map[i].active);
		i++;
	}

	return map;
}

static void lk2nd_parse_device_node(const void *fdt)
{
	int offset = lk2nd_find_device_offset(fdt);
	if (offset < 0) {
		dprintf(CRITICAL, "Failed to find matching lk2nd,device node: %d\n", offset);

		/* Still try to parse which DTB was selected so we can display it */
		dev_tree_get_dt_entry(fdt, 0, &lk2nd_dev.dt_entry);
		return;
	}

#ifdef GPIO_I2C_BUS_COUNT
	lk2nd_samsung_muic_reset(fdt, offset);
#endif
	lk2nd_motorola_smem_write_unit_info(fdt, offset);
	lk2nd_smd_rpm_hack_opening(fdt, offset);

	lk2nd_dev.model = lkfdt_copyprop_str(fdt, offset, "model");
	if (lk2nd_dev.model)
		dprintf(INFO, "Device model: %s\n", lk2nd_dev.model);
	else
		dprintf(CRITICAL, "Device node is missing 'model' property\n");

	/* Check if the bootloader selected a weird DTB and we need to override */
	if (!dev_tree_get_dt_entry(fdt, offset, &lk2nd_dev.dt_entry) && offset)
		/* Try again on root node */
		dev_tree_get_dt_entry(fdt, 0, &lk2nd_dev.dt_entry);

	if (lk2nd_dev.panel.name)
		lk2nd_parse_panels(fdt, offset);

	lk2nd_dev.keymap = lk2nd_parse_keys(fdt, offset);

#if TARGET_MSM8916
	smb1360_detect_battery(fdt, offset);
#endif
}

#ifdef LK1ST_DTB
INCFILE(lk1st_dtb, lk1st_dtb_size, LK1ST_DTB);
#endif

static void *lk2nd_get_fdt(void)
{
#ifdef LK1ST_DTB
	return lk1st_dtb;
#else
	return (void*) lk_boot_args[2];
#endif
}

int lk2nd_fdt_parse_early_uart(void)
{
	int offset, len;
	const uint32_t *val;
	void *fdt = lk2nd_get_fdt();

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
	void *fdt = lk2nd_get_fdt();
	if (!fdt)
		return;

	if (dev_tree_check_header(fdt)) {
		dprintf(INFO, "Invalid device tree provided by primary bootloader\n");
		return;
	}

	lk2nd_dev.fdt = fdt;
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
	dprintf(INFO, "### Ohai, this is lk2nd (or lk1st?) ###\n");

#ifdef LK1ST_COMPATIBLE
	lk2nd_dev.compatible = LK1ST_COMPATIBLE;
#endif

	dump_board();
	lk2nd_fdt_parse();
	lk2nd_target_keystatus();
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

	/* Enable associated touchscreen if any */
	if (panel->ts_compatible) {
		offset = fdt_node_offset_by_compatible(fdt, -1, panel->ts_compatible);
		if (offset < 0)
			return;

		ret = fdt_nop_property(fdt, offset, "status");
		if (ret)
			dprintf(CRITICAL, "Failed to NOP touchscreen status: %d\n", ret);
	}
}

bool lk2nd_cmdline_scan(const char *cmdline, const char *arg)
{
	const char *argp;

	while (true) {
		/* Skip spaces */
		for (; *cmdline == ' '; cmdline++);

		/* Compare argument */
		for (argp = arg; *argp && *argp == *cmdline; argp++, cmdline++);
		if (*argp == '\0' && (*cmdline == '\0' || *cmdline == ' '))
			return true;

		/* Skip non-spaces (the rest of the "wrong argument") */
		for (; *cmdline != ' '; cmdline++)
			if (*cmdline == '\0')
				return false;
	}
}

/*
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

static void test_scan(const char *cmdline, const char *arg) {
	printf("lk2nd_cmdline_scan(\"%s\", \"%s\") = %d\n",
	       cmdline, arg, lk2nd_cmdline_scan(cmdline, arg));
}

int main(int argc, char *argv[]) {
	test_scan("lk2nd", "lk2nd");
	test_scan(" lk2nd", "lk2nd");
	test_scan("lk2nd ", "lk2nd");
	test_scan("    lk2nd     ", "lk2nd");

	test_scan("lk2nd lk2nd.spin-table=force", "lk2nd");
	test_scan("lk2nd.spin-table=force lk2nd", "lk2nd");

	test_scan("lk2nd.spin-table=force", "lk2nd");
	test_scan("this.is.lk2nd", "lk2nd");
	test_scan("this.is.lk2nd ", "lk2nd");

	test_scan("lk1st", "lk2nd");
	test_scan(" lk1st lk2nd ", "lk2nd");

	return 0;
}
*/

void lk2nd_update_device_tree(void *fdt, const char *cmdline, bool arm64)
{
	/* Don't touch lk2nd/downstream dtb */
	if (lk2nd_cmdline_scan(cmdline, "androidboot.hardware=qcom") ||
	    lk2nd_cmdline_scan(cmdline, "androidboot.hardware=bacon") ||
	    lk2nd_cmdline_scan(cmdline, "lk2nd"))
		return;

	if (lk2nd_dev.panel.compatible)
		lk2nd_update_panel_compatible(fdt);

#if TARGET_MSM8916
	smb1360_update_device_tree(fdt);
#endif
	lk2nd_rproc_update_dev_tree(fdt);

#ifdef SMP_SPIN_TABLE_BASE
	smp_spin_table_setup((struct smp_spin_table*)SMP_SPIN_TABLE_BASE, fdt, arm64,
			     lk2nd_cmdline_scan(cmdline, "lk2nd.spin-table=force"));
#endif
}
