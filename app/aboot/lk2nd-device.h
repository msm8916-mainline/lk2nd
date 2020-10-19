/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_DEVICE_H
#define __LK2ND_DEVICE_H

#include <dev_tree.h>

struct lk2nd_panel {
	const char *name;
	const char *old_compatible;
	const char *compatible;
	int compatible_size;
};

struct lk2nd_device {
	void *fdt;
	struct board_id board_id;

	const char *model;
	const char *cmdline;

	const char *device;
	const char *bootloader;
	const char *serialno;
	const char *carrier;
	const char *radio;

	struct lk2nd_panel panel;
};

extern struct lk2nd_device lk2nd_dev;

void lk2nd_init(void);
int lk2nd_fdt_parse_early_uart(void);

void lk2nd_samsung_muic_reset(const void *fdt, int offset);
void lk2nd_motorola_smem_write_unit_info(const void *fdt, int offset);

void lk2nd_update_device_tree(void *fdt, const char *cmdline);

#endif
