// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2021-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <fastboot.h>
#include <libfdt.h>

#include "../device/device.h"

static void cmd_oem_dtb(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.dtb, fdt_totalsize(lk2nd_dev.dtb));
}
FASTBOOT_REGISTER("oem dtb", cmd_oem_dtb);
