#include <debug.h>
#include <mmc.h>
#include <partition_parser.h>
#include <target.h>
#include "fastboot.h"

#if WITH_DEBUG_LOG_BUF
static void cmd_oem_lk_log(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk_log_getbuf(), lk_log_getsize());
}
#endif


static void cmd_oem_dump_partition(const char *arg, void *data, unsigned sz)
{
	struct partition_info part = partition_get_info(arg);

	if (!part.offset) {
		fastboot_fail("partition not found");
		return;
	}
	if (part.size > target_get_max_flash_size()) {
		fastboot_fail("partition too large");
		return;
	}

	if (mmc_read(part.offset, data, part.size)) {
		fastboot_fail("failed to read partition");
		return;
	}

	fastboot_stage(data, part.size);
	fastboot_okay("");
}

void fastboot_lk2nd_register_commands(void) {
#if WITH_DEBUG_LOG_BUF
	fastboot_register("oem lk_log", cmd_oem_lk_log);
#endif
	fastboot_register("oem dump", cmd_oem_dump_partition);
}
