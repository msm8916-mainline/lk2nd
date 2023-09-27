// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru> */

#include <compiler.h>
#include <lk2nd/hw/bdev.h>

#include "bdev.h"

/**
 * lk2nd_bdev_init() - Prepare block devices for lk2nd
 */
void lk2nd_bdev_init(void)
{
	lk2nd_wrapper_bio_register();
	if (IS_ENABLED(MMC_SDHCI_SUPPORT))
		lk2nd_mmc_sdhci_bio_register();

	lk2nd_bdev_dump_devices();
}
