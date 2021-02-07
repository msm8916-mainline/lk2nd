#include <libfdt.h>
#include <lk2nd.h>
#include <string.h>
#include "fastboot.h"

static void cmd_oem_dtb(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.fdt, fdt_totalsize(lk2nd_dev.fdt));
}

static void cmd_oem_cmdline(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk2nd_dev.cmdline, strlen(lk2nd_dev.cmdline));
}

void fastboot_lk2nd_register_commands(void) {
	if (lk2nd_dev.fdt)
		fastboot_register("oem dtb", cmd_oem_dtb);

	if (lk2nd_dev.cmdline)
		fastboot_register("oem cmdline", cmd_oem_cmdline);
}
