// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <lib/bio.h>
#include <lib/fs.h>
#include <list.h>
#include <stdlib.h>
#include <target.h>

#include <lk2nd/boot.h>
#include <lk2nd/hw/bdev.h>

#include "boot.h"

#ifndef LK2ND_BOOT_MIN_SIZE
#define LK2ND_BOOT_MIN_SIZE (16 * 1024 * 1024)
#endif

/**
 * lk2nd_scan_devices() - Scan filesystems and try to boot
 */
static void lk2nd_scan_devices(void)
{
	struct bdev_struct *bdevs = bio_get_bdevs();
	char mountpoint[16];
	bdev_t *bdev;
	int ret;

	dprintf(INFO, "boot: Trying to boot from the file system...\n");

	list_for_every_entry(&bdevs->list, bdev, bdev_t, node) {

		/* Skip top level block devices. */
		if (!bdev->is_leaf)
			continue;

		/*
		 * Skip partitions that are too small to have a boot fs on.
		 *
		 * 'boot' partition is explicitly allowed to have a small fs on it
		 * in case one installs next stage bootloader package (i.e. u-boot)
		 * there but still wants to make use of lk2nd's device database.
		 */
		if (bdev->size < LK2ND_BOOT_MIN_SIZE && !!strncmp(bdev->label, "boot", strlen("boot")))
			continue;

		snprintf(mountpoint, sizeof(mountpoint), "/%s", bdev->name);
		ret = fs_mount(mountpoint, "ext2", bdev->name);
		if (ret < 0)
			continue;

		if (DEBUGLEVEL >= SPEW) {
			dprintf(SPEW, "Scanning %s ...\n", bdev->name);
			dprintf(SPEW, "%s\n", mountpoint);
			lk2nd_print_file_tree(mountpoint, " ");
		}

		lk2nd_try_extlinux(mountpoint);
	}
}

/**
 * lk2nd_boot() - Try to boot the OS.
 *
 * This method is supposed to be called from aboot.
 * If appropriate OS is found, it will be booted, and this
 * method will never return.
 */
void lk2nd_boot(void)
{
	static bool init_done = false;

	if (!init_done) {
		lk2nd_bdev_init();
		init_done = true;
	}

	lk2nd_scan_devices();
}
