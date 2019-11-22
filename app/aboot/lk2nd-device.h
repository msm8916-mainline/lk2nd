/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LK2ND_DEVICE_H
#define __LK2ND_DEVICE_H

#include <dev_tree.h>

struct lk2nd_device {
	const char *cmdline;
	const char *bootloader;
	const char *model;
};

extern struct lk2nd_device lk2nd_dev;

void lk2nd_fdt_parse(void);
int lk2nd_fdt_parse_early_uart(void);

void lk2nd_samsung_muic_reset(const void *fdt, int offset);

#endif
