// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Jack Matthews <jm5112356@gmail.com> */

#include <boot.h>
#include <libfdt.h>

#include "device.h"

static int lk2nd_serialno_dt_update(void *dtb, const char *cmdline,
				 enum boot_type boot_type)
{
	const char *serialno = lk2nd_dev.serialno;

	int dtb_root = fdt_path_offset(dtb, "/");

	if (!serialno || boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	return fdt_setprop_string(dtb, dtb_root, "serial-number", serialno);
}
DEV_TREE_UPDATE(lk2nd_serialno_dt_update);
