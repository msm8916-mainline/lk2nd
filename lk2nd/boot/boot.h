/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_BOOT_BOOT_H
#define LK2ND_BOOT_BOOT_H

#include <list.h>
#include <string.h>

#include <lk2nd/boot.h>

struct label {
	const char *name;
	const char *kernel;
	const char *initramfs;
	const char *dtb;
	const char *dtbdir;
	const char **dtboverlays;
	const char *cmdline;
};

/* util.c */
void lk2nd_print_file_tree(char *root, char *prefix);

/* extlinux.c */
void lk2nd_try_extlinux(const char *mountpoint);
int lk2nd_get_extlinux_labels(const char *root, struct label **labels, int *default_label_idx, int* labels_count);
int lk2nd_expand_conf_and_boot_label(struct label *label, const char *root);

/* boot.c */
int lk2nd_scan_devices(char **root_out, struct label **labels, int *default_label_idx, int* labels_count);

#endif /* LK2ND_BOOT_BOOT_H */
