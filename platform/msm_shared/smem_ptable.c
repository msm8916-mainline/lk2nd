/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <debug.h>
#include <reg.h>
#include <string.h>
#include <sys/types.h>
#include <platform/iomap.h>
#include <lib/ptable.h>

#include "board.h"
#include "smem.h"


/* partition table from SMEM */
static struct smem_ptable smem_ptable;
static unsigned smem_apps_flash_start = 0xFFFFFFFF;

static ram_partition_table ptable;

static void dump_smem_ptable(void)
{
	unsigned i;

	for (i = 0; i < smem_ptable.len; i++) {
		struct smem_ptn *p = &smem_ptable.parts[i];
		if (p->name[0] == '\0')
			continue;
		dprintf(SPEW, "%d: %s offs=0x%08x size=0x%08x attr: 0x%08x\n",
			i, p->name, p->start, p->size, p->attr);
	}
}

void smem_ptable_init(void)
{
	unsigned i;
	unsigned ret;
	unsigned len;

	/* Read only the header portion of ptable */
	ret = smem_read_alloc_entry_offset(SMEM_AARM_PARTITION_TABLE,
									   &smem_ptable,
									   SMEM_PTABLE_HDR_LEN,
									   0);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to read ptable hdr (%d)", ret);
		ASSERT(0);
	}

	/* Verify ptable magic */
	if (smem_ptable.magic[0] != _SMEM_PTABLE_MAGIC_1 ||
	    smem_ptable.magic[1] != _SMEM_PTABLE_MAGIC_2)
		return;

	/* Ensure that # of partitions is less than the max we have allocated. */
	ASSERT(smem_ptable.len <= SMEM_PTABLE_MAX_PARTS);

	/* Find out length of partition data based on table version. */
	if (smem_ptable.version <= 3)
	{
		len = SMEM_PTABLE_HDR_LEN + SMEM_PTABLE_MAX_PARTS_V3*sizeof(struct smem_ptn);
	}
	else if (smem_ptable.version == 4)
	{
		len = SMEM_PTABLE_HDR_LEN + SMEM_PTABLE_MAX_PARTS_V4*sizeof(struct smem_ptn);
	}
	else
	{
		dprintf(CRITICAL, "Unknown ptable version (%d)", smem_ptable.version);
		ASSERT(0);
	}

	ret = smem_read_alloc_entry(SMEM_AARM_PARTITION_TABLE,
								&smem_ptable,
								len);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to read ptable (%d)", ret);
		ASSERT(0);
	}

	dump_smem_ptable();
	dprintf(INFO, "smem ptable found: ver: %d len: %d\n",
			smem_ptable.version, smem_ptable.len);

	for (i = 0; i < smem_ptable.len; i++) {
		if (!strcmp(smem_ptable.parts[i].name, "0:APPS"))
			break;
	}
	if (i == smem_ptable.len)
		return;

	smem_apps_flash_start = smem_ptable.parts[i].start;
}

unsigned smem_get_apps_flash_start(void)
{
	return smem_apps_flash_start;
}

void smem_add_modem_partitions(struct ptable *flash_ptable)
{
	unsigned i;

	if (smem_ptable.magic[0] != _SMEM_PTABLE_MAGIC_1 ||
	    smem_ptable.magic[1] != _SMEM_PTABLE_MAGIC_2)
		return;

	for (i = 0; i < smem_ptable.len; i++) {
		char *token;
		char *pname = NULL;
		char *sp;
		struct smem_ptn *p = &smem_ptable.parts[i];
		if (p->name[0] == '\0')
			continue;
		token = strtok_r(p->name, ":", &sp);
		while (token) {
			pname = token;
			token = strtok_r(NULL, ":", &sp);
		}
		if (pname) {
			ptable_add(flash_ptable, pname, p->start,
				   p->size, 0, TYPE_MODEM_PARTITION,
				   PERM_WRITEABLE);
		}
	}
}

static void smem_copy_ram_ptable(void *buf)
{
	struct smem_ram_ptable *table_v0;
	struct smem_ram_ptable_v1 *table_v1;
	uint32_t pentry = 0;

	ptable.hdr = *(struct smem_ram_ptable_hdr*)buf;

	/* Perform member to member copy from smem_ram_ptable to wrapper struct ram_ptable */
	if(ptable.hdr.version == SMEM_RAM_PTABLE_VERSION_1)
	{
		table_v1 = (struct smem_ram_ptable_v1*)buf;

		memcpy(&ptable, table_v1, sizeof(ram_partition_table));
	}
	else if(ptable.hdr.version == SMEM_RAM_PTABLE_VERSION_0)
	{
		table_v0 = (struct smem_ram_ptable*)buf;

		for(pentry = 0; pentry < ((struct smem_ram_ptable_hdr*)buf)->len; pentry++)
		{
			ptable.parts[pentry].start          = table_v0->parts[pentry].start;
			ptable.parts[pentry].size           = table_v0->parts[pentry].size;
			ptable.parts[pentry].attr           = table_v0->parts[pentry].attr;
			ptable.parts[pentry].category       = table_v0->parts[pentry].category;
			ptable.parts[pentry].domain         = table_v0->parts[pentry].domain;
			ptable.parts[pentry].type           = table_v0->parts[pentry].type;
			ptable.parts[pentry].num_partitions = table_v0->parts[pentry].num_partitions;
		}

	}
	else
	{
		dprintf(CRITICAL,"ERROR: Unknown smem ram ptable version: %u", ptable.hdr.version);
		ASSERT(0);
	}
}

/* RAM Partition table from SMEM */
int smem_ram_ptable_init(struct smem_ram_ptable *smem_ram_ptable)
{
	unsigned i;

	i = smem_read_alloc_entry(SMEM_USABLE_RAM_PARTITION_TABLE,
				  smem_ram_ptable,
				  sizeof(struct smem_ram_ptable));
	if (i != 0)
		return 0;

	if (smem_ram_ptable->magic[0] != _SMEM_RAM_PTABLE_MAGIC_1 ||
	    smem_ram_ptable->magic[1] != _SMEM_RAM_PTABLE_MAGIC_2)
		return 0;

	dprintf(SPEW, "smem ram ptable found: ver: %d len: %d\n",
		smem_ram_ptable->version, smem_ram_ptable->len);

	smem_copy_ram_ptable((void*)smem_ram_ptable);

	return 1;
}

/* RAM Partition table from SMEM */
static uint32_t buffer[sizeof(struct smem_ram_ptable_v1)];
int smem_ram_ptable_init_v1()
{
	uint32_t i;
	uint32_t ret;
	uint32_t version;
	uint32_t smem_ram_ptable_size;
	struct smem_ram_ptable_hdr *ram_ptable_hdr;

	/* Check smem ram partition table version and decide on length of ram_ptable */
	ret = smem_read_alloc_entry_offset(SMEM_USABLE_RAM_PARTITION_TABLE,
						&version,
						sizeof(version),
						SMEM_RAM_PTABLE_VERSION_OFFSET);

	if(ret)
		return 0;

	if(version == SMEM_RAM_PTABLE_VERSION_1)
		smem_ram_ptable_size = sizeof(struct smem_ram_ptable_v1);
	else if(version == SMEM_RAM_PTABLE_VERSION_0)
		smem_ram_ptable_size = sizeof(struct smem_ram_ptable);
	else
	{
		dprintf(CRITICAL,"ERROR: Wrong smem_ram_ptable version: %u", version);
		ASSERT(0);
	}

	i = smem_read_alloc_entry(SMEM_USABLE_RAM_PARTITION_TABLE,
				  (void*)buffer,
				  smem_ram_ptable_size);
	if (i != 0)
		return 0;

	ram_ptable_hdr = (struct smem_ram_ptable_hdr *)buffer;

	if (ram_ptable_hdr->magic[0] != _SMEM_RAM_PTABLE_MAGIC_1 ||
	    ram_ptable_hdr->magic[1] != _SMEM_RAM_PTABLE_MAGIC_2)
		return 0;

	smem_copy_ram_ptable((void*)buffer);

	dprintf(SPEW, "smem ram ptable found: ver: %u len: %u\n",
		ram_ptable_hdr->version, ram_ptable_hdr->len);

	return 1;
}

void smem_get_ram_ptable_entry(ram_partition *ptn, uint32_t entry)
{
	memcpy(ptn, &(ptable.parts[entry]), sizeof(ram_partition));
}

uint32_t smem_get_ram_ptable_len(void)
{
	return ptable.hdr.len;
}

uint32_t smem_get_ram_ptable_version(void)
{
	return ptable.hdr.version;
}

uint32_t get_ddr_start()
{
	uint32_t i;
	ram_partition ptn_entry;
	uint32_t len = 0;

	ASSERT(smem_ram_ptable_init_v1());

	len = smem_get_ram_ptable_len();

	/* Determine the Start addr of the DDR RAM */
	for(i = 0; i < len; i++)
	{
		smem_get_ram_ptable_entry(&ptn_entry, i);
		if(ptn_entry.type == SYS_MEMORY)
		{
			if((ptn_entry.category == SDRAM) ||
			   (ptn_entry.category == IMEM))
			{
				/* Check to ensure that start address is 1MB aligned */
				ASSERT((ptn_entry.start & (MB-1)) == 0);
				return ptn_entry.start;
			}
		}
	}
	ASSERT("DDR Start Mem Not found\n");
	return 0;
}

uint64_t smem_get_ddr_size()
{
	uint32_t i;
	ram_partition ptn_entry;
	uint32_t len = 0;
	uint64_t size = 0;

	ASSERT(smem_ram_ptable_init_v1());

	len = smem_get_ram_ptable_len();

	/* Determine the Start addr of the DDR RAM */
	for(i = 0; i < len; i++)
	{
		smem_get_ram_ptable_entry(&ptn_entry, i);
		if(ptn_entry.type == SYS_MEMORY && ptn_entry.category == SDRAM)
			size += ptn_entry.size;
	}

	return size;
}
