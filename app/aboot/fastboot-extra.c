#include <arch/ops.h>
#include <debug.h>
#include <dev/fbcon.h>
#include <mmc.h>
#include <partition_parser.h>
#include <platform.h>
#include <platform/iomap.h>
#include <pm8x41_regulator.h>
#include <reg.h>
#include <target.h>
#include "fastboot.h"

static void cmd_oem_dump_partition(const char *arg, void *data, unsigned sz)
{
	struct partition_info part = partition_get_info(arg);
	int ret;

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

#if WITH_DEBUG_LOG_BUF
static void cmd_oem_lk_log(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(lk_log_getbuf(), lk_log_getsize());
}
#endif

static void cmd_oem_screenshot(const char *arg, void *data, unsigned sz)
{
	struct fbcon_config *fb = fbcon_display();
	unsigned hdr;

	if (!fb) {
		fastboot_fail("display not initialized");
		return;
	}

	if (fb->format != FB_FORMAT_RGB888 || fb->bpp != 24) {
		fastboot_fail("unsupported fb format\n");
		return;
	}

	sz = fb->width * fb->height;
	if (sz % sizeof(uint64_t) != 0) {
		fastboot_fail("unsupported display resolution\n");
		return;
	}

	/* PPM image header, see http://netpbm.sourceforge.net/doc/ppm.html */
	hdr = sprintf(data, "P6\n\n%7u %7u\n255\n", fb->width, fb->height);
	ASSERT(hdr % sizeof(uint64_t) == 0);

	sz = fb->width * fb->height;

	/* PPM expects RGB but this seems to be BGR, so do some fancy swapping! */
	//memcpy(data + hdr, fb->base, sz * 3);
	rgb888_swap(fb->base, data + hdr, sz / sizeof(uint64_t));

	fastboot_stage(data, hdr + sz*3);
}

static void cmd_oem_reboot_edl(const char *arg, void *data, unsigned sz)
{
	if (set_download_mode(EMERGENCY_DLOAD)) {
		fastboot_fail("Failed to set emergency download mode");
		return;
	}

	fastboot_okay("");
	reboot_device(DLOAD);
}

static void cmd_oem_dump_regulators(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	struct spmi_regulator *vreg = target_get_regulators();

	if (!vreg) {
		fastboot_fail("No regulators supported for this platform");
		return;
	}

	for (; vreg->name; ++vreg) {
		snprintf(response, sizeof(response), "%s: enabled: %d, voltage: %d mV (%s)",
			 vreg->name, regulator_is_enabled(vreg),
			 regulator_get_voltage(vreg), regulator_get_range_name(vreg));
		fastboot_info(response);
	}
	fastboot_okay("");
}

#ifdef BOOT_ROM_BASE
static void cmd_oem_dump_boot_rom(const char *arg, void *data, unsigned sz)
{
	fastboot_info("NOTE: Device will reboot without special firmware!");

	for (uint32_t *buf = data, *m = (uint32_t*)BOOT_ROM_BASE;
	     m < (uint32_t*)BOOT_ROM_END; ++buf, ++m)
		*buf = *m;

	fastboot_stage(data, BOOT_ROM_END - BOOT_ROM_BASE);
	fastboot_okay("");
}
#endif

#ifdef APCS_BANKED_SAW2_BASE
#define APCS_BANKED_SAW2_CFG		(APCS_BANKED_SAW2_BASE + 0x08)
#define APCS_BANKED_SAW2_SPM_CTL	(APCS_BANKED_SAW2_BASE + 0x30)
#define APCS_BANKED_SAW2_DLY		(APCS_BANKED_SAW2_BASE + 0x34)
#define APCS_BANKED_SAW2_VERSION	(APCS_BANKED_SAW2_BASE + 0xfd0)

static void cmd_oem_dump_saw2(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];
	uint32_t val, cfg, spm_ctl, dly;

	val = readl(APCS_BANKED_SAW2_VERSION);
	snprintf(response, sizeof(response),
		 "SAW2 version: v%d.%d",
		 (val >> 28) & 0xF, (val >> 16) & 0xFFF);
	fastboot_info(response);

	cfg = readl(APCS_BANKED_SAW2_CFG);
	spm_ctl = readl(APCS_BANKED_SAW2_SPM_CTL);
	dly = readl(APCS_BANKED_SAW2_DLY);
	snprintf(response, sizeof(response),
		 "cfg: %#x, spm-ctl: %#x, dly: %#x", cfg, spm_ctl, dly);
	fastboot_info(response);

	fastboot_okay("");
}
#endif

void fastboot_extra_register_commands(void) {
	fastboot_register("oem dump", cmd_oem_dump_partition);

#if WITH_DEBUG_LOG_BUF
	fastboot_register("oem lk_log", cmd_oem_lk_log);
#endif
#if DISPLAY_SPLASH_SCREEN
	fastboot_register("oem screenshot", cmd_oem_screenshot);
#endif

	fastboot_register("oem reboot-edl", cmd_oem_reboot_edl);
	fastboot_register("oem dump-regulators", cmd_oem_dump_regulators);

#ifdef BOOT_ROM_BASE
	fastboot_register("oem dump-boot-rom", cmd_oem_dump_boot_rom);
#endif

#ifdef APCS_BANKED_SAW2_BASE
	fastboot_register("oem dump-saw2", cmd_oem_dump_saw2);
#endif
}
