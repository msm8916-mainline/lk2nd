// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2023 Nikita Travkin <nikita@trvn.ru> */

#include <boot.h>
#include <compiler.h>
#include <debug.h>
#include <fastboot.h>
#include <libfdt.h>
#include <target.h>
#include <zlib.h>

#include <lk2nd/util/cmdline.h>

#define PERSISTENT_RAM_SIG (0x43474244) /* DBGC */

struct pram_buf {
	uint32_t sig;
	uint32_t offt;
	uint32_t size;
	uint8_t  data[];
};

struct ramoops_region {
	void   *base;
	size_t size;
	size_t ecc_size;
	size_t record_size;

	size_t dump_size;
	size_t console_size;
	size_t ftrace_size;
	size_t pmsg_size;
};

static void get_ramoops_region(struct ramoops_region *region)
{
	unsigned int scratch_size = target_get_max_flash_size();
	void *scratch = target_get_scratch_address();

	region->ecc_size	= 0;
	region->size		= 512 * 1024;

	/*
	 * NOTE: Those should be powers of 2
	 */
	region->record_size	= 8 * 1024;
	region->console_size	= 256 * 1024;
	region->pmsg_size	= 0;
	region->ftrace_size	= 0;

	region->dump_size	= region->size - region->console_size - region->ftrace_size - region->pmsg_size;
	region->base		= (scratch + scratch_size - region->size);
}


static int lk2nd_ramoops_dt_update(void *dtb, const char *cmdline, enum boot_type boot_type)
{
	int ret, rmem_offset, offset;
	struct ramoops_region region;
	char tmp[32], args[16];

	if (boot_type & (BOOT_DOWNSTREAM | BOOT_LK2ND))
		return 0;

	if (!lk2nd_cmdline_scan_arg(cmdline, "lk2nd.pass-ramoops", args, sizeof(args)))
		return 0;

	get_ramoops_region(&region);

	if (!strcmp(args, "zap"))
		memset(region.base, 0, region.size);

	rmem_offset = fdt_path_offset(dtb, "/reserved-memory");
	if (rmem_offset < 0)
		return 0;

	offset = fdt_node_offset_by_compatible(dtb, rmem_offset, "ramoops");
	if (offset < 0 && offset != -FDT_ERR_NOTFOUND)
		return 0;

	if (offset == -FDT_ERR_NOTFOUND) {
		snprintf(tmp, sizeof(tmp), "ramoops@%08x", (uint32_t)region.base);
		offset = fdt_add_subnode(dtb, rmem_offset, tmp);
		if (offset < 0)
			return 0;

		ret = fdt_setprop_string(dtb, offset, "compatible", "ramoops");
		if (ret < 0)
			return 0;
	}

	ret = fdt_setprop_empty(dtb, offset, "reg");
	if (ret < 0)
		return 0;

	ret = fdt_appendprop_addrrange(dtb, rmem_offset, offset, "reg",
				       (uint32_t)region.base, region.size);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "console-size", region.console_size);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "record-size", region.record_size);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "ftrace-size", region.ftrace_size);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "pmsg-size", region.pmsg_size);
	if (ret < 0)
		return 0;

	ret = fdt_setprop_u32(dtb, offset, "ecc-size", region.ecc_size);
	if (ret < 0)
		return 0;

	return 0;
}
DEV_TREE_UPDATE(lk2nd_ramoops_dt_update);

static void cmd_oem_ramoops_raw(const char *arg, void *data, unsigned sz)
{
	struct ramoops_region region;

	get_ramoops_region(&region);
	fastboot_stage(region.base, region.size);
}
FASTBOOT_REGISTER("oem ramoops raw", cmd_oem_ramoops_raw);

#define RAMOOPS_KERNMSG_HDR 0x3d3d3d3d /* ==== */
struct kmsg_hdr {
	uint32_t magic;
	char	 time[17];
	char     dash;
	char	 compressed;
	char     newline;
	uint8_t  data[];
} __PACKED;

static void cmd_oem_ramoops_regions(const char *arg, void *data, unsigned sz)
{
	struct ramoops_region region;
	struct pram_buf *record;
	uint8_t *record_base;
	char response[MAX_RSP_SIZE], header[sizeof(struct kmsg_hdr)];
	int i, count;

	get_ramoops_region(&region);

	record_base = region.base;
	record = (struct pram_buf*)record_base;

	fastboot_info("ramoops regions:");
	if (region.dump_size) {
		fastboot_info("DUMP:");

		count = region.dump_size / region.record_size;
		for (i = 0; i < count; ++i) {
			if (record->size) {
				memcpy(header, record->data, sizeof(header));
				header[sizeof(header) - 1] = '\0';
				snprintf(response, sizeof(response), "- (0x%x) 0x%x @ 0x%x: %s",
					 record->sig, record->size, (uint32_t)record, header);
				fastboot_info(response);
			}

			record_base += region.record_size;
			record = (struct pram_buf*)record_base;
		}
	}

	if (region.console_size) {
			snprintf(response, sizeof(response), "CONSOLE: (0x%x) 0x%x @ (0x%x + 0x%x)",
				 record->sig, record->size, (uint32_t)record, record->offt);
			fastboot_info(response);

			record_base += region.console_size;
			record = (struct pram_buf*)record_base;
	}

	fastboot_okay("");

}
FASTBOOT_REGISTER("oem ramoops regions", cmd_oem_ramoops_regions);

static void cmd_oem_ramoops_console(const char *arg, void *data, unsigned sz)
{
	struct ramoops_region region;
	struct pram_buf *record;
	uint8_t *record_base;

	get_ramoops_region(&region);

	record_base = region.base + region.dump_size;
	record = (struct pram_buf*)record_base;

	fastboot_stage(record->data, record->size);
}
FASTBOOT_REGISTER("oem ramoops console", cmd_oem_ramoops_console);

static void cmd_oem_ramoops_dump(const char *arg, void *data, unsigned sz)
{
	unsigned long size = target_get_max_flash_size();
	void *scratch = target_get_scratch_address();
	struct ramoops_region region;
	char response[MAX_RSP_SIZE];
	struct pram_buf *record;
	struct kmsg_hdr *header;
	uint8_t *record_base;
	int ret;

	get_ramoops_region(&region);

	record_base = region.base;
	record = (struct pram_buf*)record_base;
	header = (struct kmsg_hdr*)record->data;

	/*
	 * FIXME: this is pretty much useless right now.
	 * 1) This only checks the first record, not all of them.
	 * 2) This can't really work without ECC...
	 */

	memset(scratch, 0, record->size);

	if (header->magic != RAMOOPS_KERNMSG_HDR)
		fastboot_fail("Dump corrupted.");

	if (header->compressed == 'C') {
		ret = uncompress(scratch, &size, header->data, record->size);
		snprintf(response, sizeof(response), "ramoops decompress is %d", ret);
		fastboot_info(response);
		if (ret < 0)
			fastboot_fail("Decompression failed.");
	} else {
		size = record->size - sizeof(struct kmsg_hdr);
		memcpy(scratch, header->data, size);
	}

	fastboot_stage(scratch, size);
}
FASTBOOT_REGISTER("oem ramoops dump", cmd_oem_ramoops_dump);
