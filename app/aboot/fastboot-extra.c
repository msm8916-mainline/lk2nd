#include <arch/ops.h>
#include <debug.h>
#include <dev/fbcon.h>
#include <platform.h>
#include "fastboot.h"

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

void fastboot_extra_register_commands(void) {
#if WITH_DEBUG_LOG_BUF
	fastboot_register("oem lk_log", cmd_oem_lk_log);
#endif
#if DISPLAY_SPLASH_SCREEN
	fastboot_register("oem screenshot", cmd_oem_screenshot);
#endif

	fastboot_register("oem reboot-edl", cmd_oem_reboot_edl);
}
