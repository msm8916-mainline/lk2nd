#include <string.h>
#include <debug.h>
#include <libfdt.h>
#include <lk2nd-device.h>
#include "fastboot.h"

#if WITH_DEBUG_LOG_BUF
static void cmd_oem_lk_log(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk_log_getbuf(), lk_log_getsize());
}
#endif

static void cmd_oem_dtb(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.fdt, fdt_totalsize(lk2nd_dev.fdt));
}

static void cmd_oem_cmdline(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.cmdline, strlen(lk2nd_dev.cmdline));
}

void fastboot_lk2nd_register_commands(void) {
#if WITH_DEBUG_LOG_BUF
	fastboot_register("oem lk_log", cmd_oem_lk_log);
#endif

	if (lk2nd_dev.fdt)
		fastboot_register("oem dtb", cmd_oem_dtb);

	if (lk2nd_dev.cmdline)
		fastboot_register("oem cmdline", cmd_oem_cmdline);

#if TARGET_MSM8916
	fastboot_lk2nd_register_commands_msm8916();
#endif
}
