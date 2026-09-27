// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <fastboot.h>
#include <platform.h>

#if WITH_DEBUG_LOG_BUF
static void cmd_oem_log(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(data, log_copy(data));
}
FASTBOOT_REGISTER("oem log", cmd_oem_log);
#endif

static void cmd_oem_reboot_edl(const char *arg, void *data, unsigned sz)
{
	fastboot_okay("");
	reboot_device(EMERGENCY_DLOAD);
}
FASTBOOT_REGISTER("oem reboot-edl", cmd_oem_reboot_edl);
