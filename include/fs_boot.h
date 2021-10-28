/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _FS_BOOT_H_
#define _FS_BOOT_H_
#include <stddef.h>

enum fs_boot_dev {
	FS_BOOT_DEV_NONE,
	FS_BOOT_DEV_EMMC = 1,
	FS_BOOT_DEV_SDCARD = 2,
};

enum rproc_mode {
	RPROC_MODE_UNKNOWN,
	RPROC_MODE_ALL,
	RPROC_MODE_NO_MODEM,
	RPROC_MODE_NONE,
};

struct fs_boot_data {
	enum fs_boot_dev dev;
	enum rproc_mode rproc_mode;
};

extern struct fs_boot_data fs_boot_data;

void fsboot_test(void);
int fsboot_boot_first(void* target, size_t sz);

#endif
