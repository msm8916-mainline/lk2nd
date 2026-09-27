/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_DEVICE_SDHC_H
#define LK2ND_DEVICE_SDHC_H

#include <stdint.h>

uint32_t sdhc_slot_get(const void *dtb, int node);

#endif /* LK2ND_DEVICE_SDHC_H */
