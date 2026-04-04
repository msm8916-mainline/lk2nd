// SPDX-License-Identifier: BSD-3-Clause

#include <dt-bindings/lk2nd/sdhc.h>

#include <libfdt.h>

#include "sdhc.h"

#define SD_MMC_SLOT_MASK 0x0000000f

/**
 * sdhc_slot_get() - Retrieve the SD MMC slot number from the DTB.
 *
 * Return: 0 if the property is not present or has an invalid value,
 * else the SD MMC slot number.
 */
uint32_t sdhc_slot_get(const void *dtb, int node)
{
    int len;
    const fdt32_t *sd_mmc_prop_val;
    uint32_t sd_mmc_dts_val = 0;
    uint32_t sd_mmc_slot_num = 0;

	sd_mmc_prop_val = fdt_getprop(dtb, node, "lk2nd,sd-mmc", &len);
	if (sd_mmc_prop_val && len == sizeof(*sd_mmc_prop_val)) {
		sd_mmc_dts_val = fdt32_to_cpu(*sd_mmc_prop_val);

        if (SDHC_DEV_MAGIC == (SDHC_DEV_MAGIC & sd_mmc_dts_val)) {
            /* The value is indeed a SDHC DEV */
            sd_mmc_slot_num = sd_mmc_dts_val & SD_MMC_SLOT_MASK;
        }
	}

    return sd_mmc_slot_num;
}
