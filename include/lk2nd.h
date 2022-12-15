/* SPDX-License-Identifier: GPL-2.0-only */

#if !defined(__LK2ND_DEVICE_H) && WITH_LK2ND
#define __LK2ND_DEVICE_H

#include <dev_tree.h>

struct smb1360;
struct smb1360_battery;

struct lk2nd_panel {
	const char *name;
	const char *old_compatible;
	const char *compatible;
	int compatible_size;
	const char *ts_compatible;
};

struct lk2nd_keymap {
	uint32_t key;
	uint32_t gpio;
	enum {
		KEY_GPIO = 0,
		KEY_PM_GPIO = 1,
		KEY_RESIN = 2,
		KEY_PWR = 3,
	} type;
	uint8_t pull;
	uint8_t active;
};

struct lk2nd_device {
	void *fdt;
	struct dt_entry dt_entry;

	const char *model;
	const char *cmdline;

	const char *device;
	const char *compatible;
	const char *bootloader;
	const char *serialno;
	const char *carrier;
	const char *radio;

	const char *battery;
	const struct smb1360 *smb1360;
	const struct smb1360_battery *smb1360_battery;

	struct lk2nd_panel panel;
	struct lk2nd_keymap *keymap;
};

extern struct lk2nd_device lk2nd_dev;

void lk2nd_init(void);
int lk2nd_fdt_parse_early_uart(void);
void lk2nd_target_keystatus();
char *genlk1st2lk2ndcmdline(void);
bool lk2nd_cmdline_scan(const char *cmdline, const char *arg);

void lk2nd_samsung_muic_reset(const void *fdt, int offset);
void lk2nd_motorola_smem_write_unit_info(const void *fdt, int offset);
void lk2nd_smd_rpm_hack_opening(const void *fdt, int offset);

void lk2nd_update_device_tree(void *fdt, const char *cmdline, bool arm64);
void lk2nd_rproc_update_dev_tree(void *fdt);

struct smp_spin_table;
void smp_spin_table_setup(struct smp_spin_table *table, void *fdt, bool arm64, bool force);

int lkfdt_prop_strcmp(const void *fdt, int node, const char *prop, const char *cmp);
bool lkfdt_node_is_available(const void *fdt, int node);

#endif
