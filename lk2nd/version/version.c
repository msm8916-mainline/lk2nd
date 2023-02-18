// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023, Nikita Travkin <nikita@trvn.ru> */

#include <boot.h>
#include <libfdt.h>
#include <fastboot.h>

#include <lk2nd/version.h>

/*
 * version.c - Pass lk2nd build information to the OS.
 *
 * Per DT spec /chosen is used for various firmware
 * parameters so we place our version here in case OS wants
 * to use these to i.e. OTA update the lk2nd.
 */

#define xstr(s)	str(s)
#define str(s)	#s

static int lk2nd_version_dt_update(void *dtb, const char *cmdline,
				    enum boot_type boot_type)
{
	int ret, offset;

	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	offset = fdt_path_offset(dtb, "/chosen");
	if (offset < 0)
		return 0;

	ret = fdt_setprop_string(dtb, offset, "lk2nd,project", xstr(BOARD));
	if (ret < 0)
		return 0;

	ret = fdt_setprop_string(dtb, offset, "lk2nd,version", LK2ND_VERSION);
	if (ret < 0)
		return 0;

	return 0;
}
DEV_TREE_UPDATE(lk2nd_version_dt_update);

static void lk2nd_version_publish(void)
{
	fastboot_publish("lk2nd:version", LK2ND_VERSION);
}
FASTBOOT_INIT(lk2nd_version_publish);
