// SPDX-License-Identifier: BSD-3-Clause
/* Copyright (c) 2019-2022, Stephan Gerhold <stephan@gerhold.net> */

#include <debug.h>
#include <dev/flash.h>
#include <lib/ptable.h>
#include <partition_parser.h>
#include <string.h>
#include <target.h>

#include <lk2nd/init.h>

static void partition_split_mmc(const char *base_name, const char *name,
				uint32_t num_blocks, bool end)
{
	struct partition_entry *base, *split;
	int index = partition_get_index(base_name);

	if (index == INVALID_PTN) {
		dprintf(CRITICAL, "%s partition not found (as base for %s)\n",
			base_name, name);
		return;
	}
	base = &partition_get_partition_entries()[index];

	if (base->size < num_blocks) {
		dprintf(CRITICAL, "%s partition has not enough space for %s (%llu < %u)\n",
			base_name, name, base->size, num_blocks);
		return;
	}

	split = partition_allocate();
	if (split) {
		memcpy(split, base, sizeof(*split));
		strlcpy((char*)split->name, name, sizeof(split->name));
		if (end)
			split->first_lba = split->last_lba - num_blocks + 1;
		else
			split->last_lba = split->first_lba + num_blocks - 1;
		split->size = num_blocks;
	} else {
		dprintf(CRITICAL, "Too many partitions, cannot add virtual %s partition\n",
			name);
	}

	if (end)
		base->last_lba -= num_blocks;
	else
		base->first_lba += num_blocks;
	base->size -= num_blocks;
}

static void partition_split_flash(struct ptable *ptable, const char *base_name,
				  const char *name, unsigned length, bool end)
{
	struct ptentry *base = ptable_find(ptable, base_name);

	if (!base) {
		dprintf(CRITICAL, "%s partition not found (as base for %s)\n",
			base_name, name);
		return;
	}

	if (base->length < length) {
		dprintf(CRITICAL, "%s partition has not enough space for %s (%u < %u)\n",
			base_name, name, base->length, length);
		return;
	}

	if (ptable_size(ptable) < MAX_PTABLE_PARTS) {
		unsigned start = base->start;
		if (end)
			start += base->length - length;

		ptable_add(ptable, (char*)name, start, length, base->flags,
			   base->type, base->perm);
	} else {
		dprintf(CRITICAL, "Too many partitions, cannot add virtual %s partition\n",
			name);
	}

	if (!end)
		base->start += length;
	base->length -= length;
}

static void lk2nd_partition_split_mmc(void)
{
	uint32_t block_size __UNUSED = mmc_get_device_blocksize();

#ifdef LK2ND_PARTITION_SIZE
	partition_split_mmc(LK2ND_PARTITION_BASE, LK2ND_PARTITION_NAME,
			    LK2ND_PARTITION_SIZE / block_size, false);
#ifdef QHYPSTUB_PARTITION_SIZE
	partition_split_mmc(QHYPSTUB_PARTITION_BASE, QHYPSTUB_PARTITION_NAME,
			    QHYPSTUB_PARTITION_SIZE / block_size, true);
#endif
#endif
}

static void lk2nd_partition_split_flash(void)
{
	struct ptable *ptable = flash_get_ptable();
	unsigned block_size __UNUSED = flash_block_size();

	if (!ptable)
		return;

#ifdef LK2ND_PARTITION_SIZE
	partition_split_flash(ptable, LK2ND_PARTITION_BASE, LK2ND_PARTITION_NAME,
			      LK2ND_PARTITION_SIZE / block_size, false);
#endif
}

static void lk2nd_device2nd_partition_split(void)
{
	if (target_is_emmc_boot())
		lk2nd_partition_split_mmc();
	else
		lk2nd_partition_split_flash();
}
LK2ND_INIT(lk2nd_device2nd_partition_split);
