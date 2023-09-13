// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022-2023, Otto Pflüger <affenull2345@gmail.com> */

#include <boot.h>
#include <libfdt.h>
#include <platform/partial_goods.h>
#include <lk2nd/util/lkfdt.h>

#if !TARGET_MSM8909
#error partial-goods not supported for this platform
#endif

static void cpu_disable(void *dtb, const char *path, bool is_downstream)
{
	int tmp, offset = fdt_path_offset(dtb, path);

	if (offset < 0)
		return;

	if (is_downstream) {
		fdt_setprop_string(dtb, offset, "device_type", "nak");
	} else {
		fdt_setprop_string(dtb, offset, "status", "fail");

		tmp = lkfdt_lookup_phandle(dtb, offset, "qcom,acc");
		if (tmp >= 0)
			fdt_setprop_string(dtb, tmp, "status", "fail");

		tmp = lkfdt_lookup_phandle(dtb, offset, "qcom,saw");
		if (tmp >= 0)
			fdt_setprop_string(dtb, tmp, "status", "fail");
	}
}

static int lk2nd_partial_goods_dt_update(void *dtb, const char *cmdline,
					 enum boot_type boot_type)
{
	bool is_downstream = boot_type & BOOT_DOWNSTREAM;

	if (boot_type & BOOT_LK2ND)
		return 0;

	switch (platform_partial_goods_val()) {
		case 0:
		case 1:
			cpu_disable(dtb, "/cpus/cpu@2", is_downstream);
			cpu_disable(dtb, "/cpus/cpu@3", is_downstream);
			return 0;
		default:
			/* full-featured SoC, no CPUs disabled */
			return 0;
	}
}
DEV_TREE_UPDATE(lk2nd_partial_goods_dt_update);
