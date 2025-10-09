/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_DEVICE_H
#define LK2ND_DEVICE_H

#include <boot.h>

unsigned char *lk2nd_device_update_cmdline(const char *cmdline, enum boot_type boot_type);

bool lk2nd_device2nd_have_atags(void) __PURE;
void lk2nd_device2nd_copy_atags(void *tags, const char *cmdline,
				void *ramdisk, unsigned ramdisk_size);

#if WITH_LK2ND_DEVICE
const char *const *lk2nd_device_get_dtb_hints(void);
const char *lk2nd_device_get_dtb_compatible(void);
#else
static inline const char *const *lk2nd_device_get_dtb_hints(void) { return NULL; };
static inline const char *lk2nd_device_get_dtb_compatible(void) { return NULL; };
#endif

#endif /* LK2ND_DEVICE_H */
