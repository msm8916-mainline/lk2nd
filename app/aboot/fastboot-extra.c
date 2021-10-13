#include <arch/ops.h>
#include <debug.h>
#include <dev/fbcon.h>
#include <malloc.h>
#include <mmc.h>
#include <partition_parser.h>
#include <platform.h>
#include <platform/iomap.h>
#include <pm8x41_regulator.h>
#include <reg.h>
#include <smd.h>
#include <smem.h>
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

static void cmd_oem_dump_smd_rpm(const char *arg, void *data, unsigned sz)
{
	smd_channel_alloc_entry_t *entries;
	char response[MAX_RSP_SIZE];
	uint32_t size = 0;
	int ret, i;

	entries = smem_get_alloc_entry(SMEM_CHANNEL_ALLOC_TBL, &size);
	if (size != SMD_CHANNEL_ALLOC_MAX) {
		fastboot_fail("Failed to find smem channel alloc tbl");
		return;
	}

	for (i = 0; i < SMEM_NUM_SMD_STREAM_CHANNELS; i++) {
		smd_port_ctrl_info *info;

		if (!entries[i].name[0])
			continue;

		snprintf(response, sizeof(response),
			 "ch%d: %s, cid: %#x, flags: %#x, ref: %u",
			 i, entries[i].name, entries[i].cid, entries[i].ctype,
			 entries[i].ref_count);
		fastboot_info(response);

		/* Device hangs when reading other channel info */
		if ((entries[i].ctype & 0xff) != SMD_APPS_RPM)
			continue;

		size = 0;
		info = smem_get_alloc_entry(SMEM_SMD_BASE_ID + entries[i].cid,
					    &size);
		if (size != sizeof(*info)) {
			fastboot_info("Failed to find channel info");
			continue;
		}

		snprintf(response, sizeof(response),
			 "ch%d: tx state: %u, tx fstate: %u, rx state: %u, rx fstate: %u",
			 i, info->ch0.stream_state, info->ch0.state_updated,
			 info->ch1.stream_state, info->ch1.state_updated);
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
#define SAW2_ID			0x04
#define SAW2_CFG		0x08
#define SAW2_SPM_CTL		0x30
#define SAW2_DLY		0x34
#define SAW2_SPM_PMIC_DATA	0x40
#define SAW2_VERSION		0xfd0

/* Only valid for v3.0 */
#define SAW2_VERSION_3_0	0x30000000
#define SAW2_SPM_SLP_SEQ_ENTRY	0x400

static void dump_saw2(const char *name, uintptr_t base)
{
	char response[MAX_RSP_SIZE];
	uint32_t version, id, num, i;

	version = readl(base + SAW2_VERSION);
	snprintf(response, sizeof(response),
		 "SAW2 %s (%#x) version: v%d.%d (%#x)", name, base,
		 (version >> 28) & 0xF, (version >> 16) & 0xFFF, version);
	fastboot_info(response);

	id = readl(base + SAW2_ID);
	snprintf(response, sizeof(response),
		 "id: %#x, cfg: %#x, spm-ctl: %#x, dly: %#x", id,
		 readl(base + SAW2_CFG),
		 readl(base + SAW2_SPM_CTL),
		 readl(base + SAW2_DLY));
	fastboot_info(response);

	num = (id >> 4) & 0b111; // NUM_PMIC_DATA
	if (num) {
		snprintf(response, sizeof(response), "PMIC DATA(%d):", num);
		fastboot_info(response);

		for (i = 0; i < num; ++i) {
			snprintf(response, sizeof(response), "\t%#x",
				 readl(base + SAW2_SPM_PMIC_DATA + (i * sizeof(uint32_t))));
			fastboot_info(response);
		}
	}

	// TODO: Dump more for other SAW2 versions as well
	if (version != SAW2_VERSION_3_0)
		return;

	num = (id >> 24) & 0xff; // NUM_SPM_ENTRY
	if (num) {
		uint32_t lines = ((num * sizeof(uint32_t)) + 8 - 1) / 8;
		uint8_t *buf = calloc(lines, 8);
		uint32_t *buf32 = (uint32_t*)buf;

		snprintf(response, sizeof(response), "SPM_ENTRY(%d):", num);
		fastboot_info(response);

		for (i = 0; i < num; ++i)
			buf32[i] = readl(base + SAW2_SPM_SLP_SEQ_ENTRY + (i * sizeof(uint32_t)));
		for (i = 0; i < lines; ++i) {
			snprintf(response, sizeof(response),
				 "\t%#02x %#02x %#02x %#02x %#02x %#02x %#02x %#02x",
				 buf[0], buf[1], buf[2], buf[3],
				 buf[4], buf[5], buf[6], buf[7]);
			fastboot_info(response);
			buf += 8;
		}
		free(buf32);
	}
}


static void cmd_oem_dump_saw2(const char *arg, void *data, unsigned sz)
{
	dump_saw2("CPU", APCS_BANKED_SAW2_BASE);

#ifdef APCS_L2_SAW2_BASE
	dump_saw2("L2", APCS_L2_SAW2_BASE);
#endif

	fastboot_okay("");
}
#endif

#ifdef RPM_DATA_RAM
static void cmd_oem_dump_rpm_data_ram(const char *arg, void *data, unsigned sz)
{
	char response[MAX_RSP_SIZE];

	snprintf(response, sizeof(response), "rpm-rail-stats offset: %#x",
		 readl(RPM_DATA_RAM + 0x0c));
	fastboot_info(response);
	snprintf(response, sizeof(response), "rpm-sleep-stats offset: %#x",
		 readl(RPM_DATA_RAM + 0x14));
	fastboot_info(response);
	snprintf(response, sizeof(response), "rpm-log offset: %#x",
		 readl(RPM_DATA_RAM + 0x18));
	fastboot_info(response);
	snprintf(response, sizeof(response), "rpm free heap space: %d bytes",
		 readl(RPM_DATA_RAM + 0x1c));
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
	fastboot_register("oem dump-smd-rpm", cmd_oem_dump_smd_rpm);

#ifdef BOOT_ROM_BASE
	fastboot_register("oem dump-boot-rom", cmd_oem_dump_boot_rom);
#endif
#ifdef APCS_BANKED_SAW2_BASE
	fastboot_register("oem dump-saw2", cmd_oem_dump_saw2);
#endif
#ifdef RPM_DATA_RAM
	fastboot_register("oem dump-rpm-data-ram", cmd_oem_dump_rpm_data_ram);
#endif
}
