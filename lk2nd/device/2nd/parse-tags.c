// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <fastboot.h>
#include <string.h>

#include <libfdt.h>
#include <lk2nd/util/mmu.h>

#include "device.h"

static const void *parsed_tags;
static unsigned parsed_tags_size;

static const void *atags_copy;
static unsigned atags_copy_size;

/*
 * 1 MiB should be enough for DTB/ATAGS. Subtract 0x100 to avoid mapping
 * multiple pages for platforms that use this offset for the ATAGS.
 */
#define LK2ND_MAX_TAG_SIZE	(1 * 1024 * 1024 - 0x100)
#define LK2ND_MAX_ATAG_SIZE	(LK2ND_MAX_TAG_SIZE / sizeof(uint32_t))

/* DTB */
static void copy_dtb_cmdline(const void *dtb)
{
	int offset, len;
	const void *prop;

	offset = fdt_path_offset(dtb, "/chosen");
	if (offset < 0) {
		dprintf(INFO, "/chosen node not found in device tree from "
			"previous bootloader: %d\n", offset);
		return;
	}

	prop = fdt_getprop(dtb, offset, "bootargs", &len);
	if (!prop || len < 0) {
		dprintf(INFO, "bootargs not found in device tree from "
			"previous bootloader: %d\n", len);
		return;
	}

	lk2nd_dev.cmdline = strndup(prop, len);
}

static bool parse_dtb(const void *dtb)
{
	if (fdt_check_full(dtb, LK2ND_MAX_TAG_SIZE))
		return false;

	parsed_tags = dtb;
	parsed_tags_size = fdt_totalsize(dtb);
	dprintf(INFO, "Found valid DTB with %u bytes total\n", parsed_tags_size);

	copy_dtb_cmdline(dtb);
	return true;
}

/* ATAGS */
#define ATAG_NONE	0x00000000
#define ATAG_CORE	0x54410001
#define ATAG_INITRD	0x54410005
#define ATAG_INITRD2	0x54420005
#define ATAG_CMDLINE	0x54410009

struct atag {
	uint32_t size;
	uint32_t tag;
	uint32_t data[];
};
#define ATAG_SIZE(ds)	((sizeof(struct atag) + ds) / sizeof(uint32_t))

#define next_atag(t)	(struct atag *)((uint32_t *)(t) + (t)->size)

static inline size_t atag_size(const struct atag *t)
{
	return t->size * sizeof(uint32_t);
}
static inline size_t atag_data_size(const struct atag *t)
{
	return atag_size(t) - sizeof(*t);
}

/* Copy interesting ATAGS in case we want to boot downstream later */
static bool copy_atag(const struct atag *t)
{
	switch (t->tag) {
	case ATAG_CORE:
	case ATAG_INITRD:
	case ATAG_INITRD2:
	case ATAG_CMDLINE:
		return false;
	default:
		return true;
	}
}

static bool parse_atags(const void *tags)
{
	const struct atag *t = tags;
	unsigned total_size = 0, copy_size = 0;
	struct atag *copy = NULL;

	if (t->tag != ATAG_CORE)
		return false;

	for (t = tags; t->tag; t = next_atag(t)) {
		if (t->size < ATAG_SIZE(0))
			return false;
		if (t->size >= LK2ND_MAX_ATAG_SIZE)
			return false;
		total_size += atag_size(t);
		if (total_size > LK2ND_MAX_TAG_SIZE)
			return false;
		if (copy_atag(t))
			copy_size += atag_size(t);
	}

	parsed_tags = tags;
	parsed_tags_size = total_size;
	dprintf(INFO, "Found valid ATAGS with %u bytes total (copy: %u bytes)\n",
		total_size, copy_size);

	if (copy_size) {
		atags_copy = copy = malloc(copy_size);
		atags_copy_size = copy_size;
	}

	for (t = tags; t->tag; t = next_atag(t)) {
		switch (t->tag) {
		case ATAG_CMDLINE:
			lk2nd_dev.cmdline = strndup((const char *)t->data,
						    atag_data_size(t));
			break;
		}
		if (copy && copy_atag(t)) {
			memcpy(copy, t, atag_size(t));
			copy = next_atag(copy);
		}
	}

	return true;
}

const void *lk2nd_device2nd_parse_tags(void)
{
	extern uintptr_t lk_boot_args[3];
	extern uintptr_t lk_boot_addr;

	uintptr_t addr = lk_boot_args[2];
	const void *tags = (const void*)addr;

	dprintf(INFO, "Booted @ %p, r0=%#lx, r1=%#lx, r2=%#lx\n",
		(void *)lk_boot_addr, lk_boot_args[0], lk_boot_args[1], addr);

	if (lk_boot_args[0] != 0) {
		dprintf(CRITICAL, "r0 != 0, ignoring boot registers\n");
		return NULL;
	}

	if (!addr) {
		dprintf(CRITICAL, "No DTB/ATAGS provided by previous bootloader\n");
		return NULL;
	}

	if (!lk2nd_mmu_map_ram_dynamic("DTB/ATAGS", addr, LK2ND_MAX_TAG_SIZE))
		return NULL;

	if (parse_dtb(tags))
		return tags;
	if (parse_atags(tags))
		return NULL; /* Not a valid DTB */

	dprintf(CRITICAL, "Invalid DTB/ATAGS provided by previous bootloader\n");
	return NULL;
}

/* Fastboot */
static void cmd_oem_parsed_tags(const char *arg, void *data, unsigned sz)
{
	fastboot_stage(parsed_tags, parsed_tags_size);
}
static void lk2nd_device_parse_tags_register(void)
{
	if (parsed_tags)
		fastboot_register("oem parsed-tags", cmd_oem_parsed_tags);
}
FASTBOOT_INIT(lk2nd_device_parse_tags_register);
