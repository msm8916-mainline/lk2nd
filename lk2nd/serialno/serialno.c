// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Jack Matthews <jm5112356@gmail.com> */

#include <boot.h>
#include <libfdt.h>

static int lk2nd_serialno_dt_update(void *dtb, const char *cmdline,
				    enum boot_type boot_type)
{
	extern char sn_buf[];

	if (sn_buf[0] == '\0' || boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	return fdt_setprop_string(dtb, 0, "serial-number", sn_buf);
}
DEV_TREE_UPDATE(lk2nd_serialno_dt_update);
