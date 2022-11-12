// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Otto Pflüger <affenull2345@gmail.com> */

#include <platform/partial_goods.h>
#include <boot.h>
#include <lk2nd/util/lkfdt.h>

#if TARGET_MSM8909
static const char *cpu_disable_paths[] = {
	"/cpus/cpu@2",
	"/soc@0/power-manager@b0a8000",
	"/soc@0/power-manager@b0a9000",
	"/cpus/cpu@3",
	"/soc@0/power-manager@b0b8000",
	"/soc@0/power-manager@b0b9000",
	NULL,
};
#endif

static int lk2nd_partial_goods_dt_update(void *dtb, const char *cmdline,
					 enum boot_type boot_type)
{
#if TARGET_MSM8909
	int flag = platform_partial_goods_val();

	switch (flag) {
		case 0:
		case 1:
			/* Dual-core MSM8905 variant */
			lkfdt_setprop_by_node_paths(dtb,
				"status", "fail", sizeof("fail"),
				cpu_disable_paths);
			break;
		default:
			/* Full-featured SoC, no missing cores */
			break;
	}
#endif
	return 0;
}
DEV_TREE_UPDATE(lk2nd_partial_goods_dt_update);
