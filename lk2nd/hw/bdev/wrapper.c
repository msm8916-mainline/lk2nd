// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru> */

#include <stdlib.h>
#include <debug.h>
#include <partition_parser.h>
#include <lib/bio.h>
#include <lib/partition.h>

#include <lk2nd/init.h>

#include "bdev.h"

static ssize_t lk2nd_wrapper_bdev_read_block(struct bdev *bdev, void *buf, bnum_t block, uint count)
{
	return mmc_read((uint64_t)block * bdev->block_size, buf, count * bdev->block_size);
}

static void lk2nd_wrapper_publish_subdevices(bdev_t *bdev)
{
	struct partition_entry* entries = partition_get_partition_entries();
	unsigned int i, count = partition_get_partition_count();
	bdev_t *subdev;
	char name[32];

	for (i = 0; i < count; ++i) {
		snprintf(name, sizeof(name), "%sp%d", bdev->name, i);
		bio_publish_subdevice(bdev->name, name, entries[i].first_lba, entries[i].size);

		subdev = bio_open(name);
		subdev->label = (char *)entries[i].name;
		bio_close(subdev);

		/* There may be subpartitions... */
		partition_publish(name, 0);
	}
}

void lk2nd_wrapper_bio_register(void)
{
	uint32_t block_size = mmc_get_device_blocksize();
	uint64_t card_capacity = mmc_get_device_capacity();
	bdev_t *bdev = malloc(sizeof(*bdev));
	char name[32] = "wrp0";

	dprintf(INFO, "Registering wrapper bio devices...\n");
	bio_initialize_bdev(bdev, name, block_size, card_capacity / block_size);

	bdev->read_block = lk2nd_wrapper_bdev_read_block;

	bio_register_device(bdev);

	/*
	 * FIXME: the lk's partition module is not GPT aware yet.
	 */
	lk2nd_wrapper_publish_subdevices(bdev);
}
