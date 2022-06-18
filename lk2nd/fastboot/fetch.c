// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2022, Stephan Gerhold <stephan@gerhold.net> */

#include <stdlib.h>
#include <string.h>

#include <dev/flash.h>
#include <fastboot.h>
#include <lib/ptable.h>
#include <partition_parser.h>
#include <target.h>

extern char max_download_size[MAX_RSP_SIZE];

static bool cmd_fetch_parse_args(uint64_t *offset, uint64_t *size, char **sp)
{
	const char *token = strtok_r(NULL, ":", sp);
	unsigned long n;

	if (token) {
		n = atoull(token);
		if (n > *size) {
			fastboot_fail("offset larger than partition");
			return false;
		}
		*offset += n;
		*size -= n;

		token = strtok_r(NULL, ":", sp);
		if (token) {
			n = atoull(token);
			if (n > *size) {
				fastboot_fail("size larger than remaining partition");
				return false;
			}
			*size = n;
		}
	}

	if (*size == 0) {
		fastboot_fail("no data left to fetch");
		return false;
	}
	if (*size > target_get_max_flash_size()) {
		fastboot_fail("partition too large");
		return false;
	}

	return true;
}

static unsigned cmd_fetch_read_mmc(const char *pname, void *data, char **sp)
{
	struct partition_info part = partition_get_info(pname);

	if (!part.offset) {
		fastboot_fail("partition not found");
		return 0;
	}

	if (!cmd_fetch_parse_args(&part.offset, &part.size, sp))
		return 0;

	if (mmc_read(part.offset, data, part.size)) {
		fastboot_fail("failed to read partition");
		return 0;
	}

	return part.size;
}

static unsigned cmd_fetch_read_flash(const char *pname, void *data, char **sp)
{
	uint64_t size, offset = 0;
	struct ptable *ptable;
	struct ptentry *ptn;

	ptable = flash_get_ptable();
	if (!ptable) {
		fastboot_fail("partition table not found");
		return 0;
	}

	ptn = ptable_find(ptable, pname);
	if (!ptn) {
		fastboot_fail("partition not found");
		return 0;
	}

	size = ptn->length * flash_block_size();
	if (!cmd_fetch_parse_args(&offset, &size, sp))
		return 0;

	if (flash_read(ptn, offset, data, size)) {
		fastboot_fail("failed to read partition");
		return 0;
	}

	return size;
}

static unsigned cmd_fetch_read(const char *arg, void *data)
{
	const char *pname;
	char *sp;

	pname = strtok_r((char *)arg, ":", &sp);
	if (target_is_emmc_boot())
		return cmd_fetch_read_mmc(pname, data, &sp);
	else
		return cmd_fetch_read_flash(pname, data, &sp);
}

static void cmd_fetch(const char *arg, void *data, unsigned sz)
{
	sz = cmd_fetch_read(arg, data);
	if (sz)
		fastboot_write_data(data, sz);
}

static void cmd_oem_read_partition(const char *arg, void *data, unsigned sz)
{
	sz = cmd_fetch_read(arg, data);
	if (sz)
		fastboot_stage(data, sz);
}

static void lk2nd_fastboot_register_fetch(void)
{
	fastboot_publish("max-fetch-size", max_download_size);
	fastboot_register("oem read-partition", cmd_oem_read_partition);
	fastboot_register("fetch:", cmd_fetch);
}
FASTBOOT_INIT(lk2nd_fastboot_register_fetch);
