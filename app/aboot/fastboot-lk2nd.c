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

#if TARGET_MSM8916
extern status_t smb1360_reload(const struct smb1360 *smb);

static void cmd_oem_smb1360_reload(const char *arg, void *data, unsigned sz)
{
	status_t ret;

	fastboot_info("Reloading smb1360 charger registers from OTP...");
	if (smb1360_reload(lk2nd_dev.smb1360))
		fastboot_fail("");
	else
		fastboot_okay("");
}
#endif

void fastboot_lk2nd_register_commands(void) {
	if (lk2nd_dev.fdt)
		fastboot_register("oem dtb", cmd_oem_dtb);

	if (lk2nd_dev.cmdline)
		fastboot_register("oem cmdline", cmd_oem_cmdline);

#if TARGET_MSM8916
	if (lk2nd_dev.smb1360)
		fastboot_register("oem smb1360-reload", cmd_oem_smb1360_reload);
#endif
}
