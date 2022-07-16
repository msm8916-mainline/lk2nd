/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_DEVICE_H
#define LK2ND_DEVICE_H

#include <boot.h>

void lk2nd_device_init(void);
unsigned char *lk2nd_device_update_cmdline(const char *cmdline, enum boot_type boot_type);

#endif /* LK2ND_DEVICE_H */
