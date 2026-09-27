// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru> */
/*
 * Based on mmc_read() from mmc_wrapper.c.
 * Copyright (c) 2013-2015, The Linux Foundation
 */

#include <debug.h>
#include <err.h>
#include <lib/bio.h>
#include <lib/partition.h>
#include <mmc_sdhci.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <target.h>

#include <lk2nd/init.h>
#include <lk2nd/util/container_of.h>

#include "bdev.h"

struct mmc_bdev {
	struct bdev dev;
	struct mmc_device *mmc;
};

static ssize_t lk2nd_mmc_sdhci_bdev_read_block(struct bdev *bdev, void *buf, bnum_t block, uint count)
{
	struct mmc_bdev *dev = container_of(bdev, struct mmc_bdev, dev);
	uint32_t block_size = dev->dev.block_size;
	uint32_t read_size = SDHCI_ADMA_MAX_TRANS_SZ;
	uint32_t data_len = count * block_size;
	uint64_t data_addr = (uint64_t)block * block_size;
	uint8_t *sptr = (uint8_t *)buf;
	uint32_t ret = 0;

	arch_clean_invalidate_cache_range((addr_t)(buf), data_len);

	while (data_len > read_size) {
		ret = mmc_sdhci_read(dev->mmc, (void *)sptr, (data_addr / block_size), (read_size / block_size));
		if (ret)
			return ERR_IO;

		sptr += read_size;
		data_addr += read_size;
		data_len -= read_size;
	}

	if (data_len) {
		ret = mmc_sdhci_read(dev->mmc, (void *)sptr, (data_addr / block_size), (data_len / block_size));
		if (ret)
			return ERR_IO;
	}

	return count * block_size;
}

void lk2nd_mmc_sdhci_bio_register(void)
{
	struct mmc_bdev *bdev = malloc(sizeof(*bdev));
	struct mmc_device *mmc;
	char name[32];

	dprintf(INFO, "Registering mmc_sdhci bio devices...\n");
	mmc = target_get_sd_mmc();

	if (!mmc) {
		dprintf(INFO, "SD card MMC is unavailable.\n");
		free(bdev);
		return;
	}

	snprintf(name, sizeof(name), "mmc%d", mmc->config.slot);
	bio_initialize_bdev(&bdev->dev, name, mmc->card.block_size, mmc->card.capacity / mmc->card.block_size);

	bdev->mmc = mmc;
	bdev->dev.read_block = lk2nd_mmc_sdhci_bdev_read_block;

	bio_register_device(&bdev->dev);
	partition_publish(name, 0);
}
