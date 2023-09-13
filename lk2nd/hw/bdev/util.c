// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022 Nikita Travkin <nikita@trvn.ru> */

#include <debug.h>
#include <lib/bio.h>
#include <list.h>
#include <stdlib.h>

#include "bdev.h"

/**
 * lk2nd_bdev_dump_devices() - Log a table with all block devices.
 */
void lk2nd_bdev_dump_devices(void)
{
	struct bdev_struct *bdevs = bio_get_bdevs();
	bdev_t *entry;

	dprintf(INFO, "block devices:\n");
	dprintf(INFO, " | dev        | label           | size       | Leaf |\n");
	list_for_every_entry(&bdevs->list, entry, bdev_t, node) {
		dprintf(INFO, " | %-10s | %-15s | %6lld %s | %-4s |\n",
			entry->name,
			(entry->label ? entry->label : ""),
			entry->size / (entry->size > 1024 * 1024 ? 1024*1024 : 1024),
			(entry->size > 1024 * 1024 ? "MiB" : "KiB"),
			(entry->is_leaf ? "Yes" : "")
			);
	}
}
