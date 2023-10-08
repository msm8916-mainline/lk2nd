/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_BOOT_BOOT_H
#define LK2ND_BOOT_BOOT_H

#include <list.h>
#include <string.h>

#include <lk2nd/boot.h>

/* util.c */
void lk2nd_print_file_tree(char *root, char *prefix);

/* extlinux.c */
void lk2nd_try_extlinux(const char *mountpoint);

#endif /* LK2ND_BOOT_BOOT_H */
