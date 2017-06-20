/*
 * Copyright (c) 2017, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of The Linux Foundation nor
 *       the names of its contributors may be used to endorse or promote
 *       products derived from this software without specific prior written
 *       permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <crc32.h>
#include <ab_partition_parser.h>
#include <partition_parser.h>

//#define AB_DEBUG

/* Slot suffix */
const char *suffix_slot[] = {"_a",
		"_b"};

/* local global variables */
static signed active_slot = INVALID;		/* to store current active slot */
static bool attributes_updated = false;		/* to store if we need to update partition table */
static bool multislot_support = false;		/* to store if multislot support is present */

static int boot_slot_index[AB_SUPPORTED_SLOTS];	/* store index for boot parition */

/* local functions. */
static void attributes_update();

/*
	Function: To read slot attribute of
		of the partition_entry
*/
inline bool slot_is_active(struct partition_entry *partition_entries,
					unsigned index)
{
	if ((partition_entries[index].attribute_flag &
		PART_ATT_ACTIVE_VAL)>>PART_ATT_ACTIVE_BIT)
		return true;
	else
		return false;
}

inline bool slot_is_sucessful(struct partition_entry *partition_entries,
						unsigned index)
{
	if ((partition_entries[index].attribute_flag &
		PART_ATT_SUCCESSFUL_VAL)>>PART_ATT_SUCCESS_BIT)
		return true;
	else
		return false;
}

inline unsigned slot_retry_count(struct partition_entry *partition_entries,
						unsigned index)
{
	return ((partition_entries[index].attribute_flag
		& PART_ATT_MAX_RETRY_COUNT_VAL) >> PART_ATT_MAX_RETRY_CNT_BIT);
}

inline unsigned slot_priority(struct partition_entry *partition_entries,
						unsigned index)
{
	return ((partition_entries[index].attribute_flag
		& PART_ATT_PRIORITY_VAL)>>PART_ATT_PRIORITY_BIT);
}

inline bool slot_is_bootable(struct partition_entry *partition_entries,
						unsigned index)
{
	if ((partition_entries[index].attribute_flag &
		PART_ATT_UNBOOTABLE_VAL)>>PART_ATT_UNBOOTABLE_BIT)
		return false;
	else
		return true;
}

/*
	Function scan boot partition to find SLOT_A/SLOT_B suffix.
	If found than make multislot_boot flag true and
	scans another partition.
*/
bool partition_scan_for_multislot()
{
	int i, j, count = 0;
	char *pname = NULL;
	int strlen_boot = strlen("boot");
	int partition_count = partition_get_partition_count();
	struct partition_entry *partition_entries =
				partition_get_partition_entries();

	multislot_support = false;

	if (partition_count > NUM_PARTITIONS)
	{
		dprintf(CRITICAL, "ERROR: partition_count more than supported.\n");
		return multislot_support;
	}

	for (i = 0; i < partition_count; i++)
	{
		pname = (char *)partition_entries[i].name;
#ifdef AB_DEBUG
		dprintf(INFO, "Transversing partition %s\n", pname);
#endif
		if (!strncmp((const char *)partition_entries[i].name, "boot", strlen_boot))
		{
			pname += strlen_boot;
			if (*pname)
			{
#ifdef AB_DEBUG
		dprintf(INFO, "Suffix: %s\n", pname);
#endif
				for (j =0; j<AB_SUPPORTED_SLOTS; j++)
				{
					if (!strcmp(pname, suffix_slot[j]))
					{
						/* cache these variables as they are used multiple times */
						boot_slot_index[j] = i;
						if (!multislot_support)
							multislot_support =true;
						count ++;
					}
				}
				/* Break out of loop if all slot index are found*/
				if (count == AB_SUPPORTED_SLOTS)
					break;
			}
			else
			{
#ifdef AB_DEBUG
		dprintf(INFO, "Partition Table is not a/b supported\n");
#endif
				break;
			}
		}
	}
	return multislot_support;
}

/*
	Function: To reset partition attributes
	This function reset partition_priority, retry_count
	and clear successful and bootable bits.
*/
void partition_reset_attributes(unsigned index)
{
	struct partition_entry *partition_entries =
					partition_get_partition_entries();

	partition_entries[index].attribute_flag |= (PART_ATT_PRIORITY_VAL |
				PART_ATT_MAX_RETRY_COUNT_VAL);

	partition_entries[index].attribute_flag &= ((~PART_ATT_SUCCESSFUL_VAL) &
						(~PART_ATT_UNBOOTABLE_VAL));

	if (!attributes_updated)
		attributes_updated = true;

	/* Make attributes persistant */
	partition_mark_active_slot(active_slot);
}

/*
	Function: Switch active partitions.
*/
void partition_switch_slots(int old_slot, int new_slot)
{
#ifdef AB_DEBUG
	dprintf(INFO, "Switching slots %s to %s\n",
				SUFFIX_SLOT(old_slot), SUFFIX_SLOT(new_slot));
#endif
	struct partition_entry *partition_entries =
					partition_get_partition_entries();
	int old_slot_index = boot_slot_index[old_slot];
	int new_slot_index = boot_slot_index[new_slot];

	/* Mark current slot inactive, keeping all other attributes intact */
	partition_entries[old_slot_index].attribute_flag &= ~PART_ATT_ACTIVE_VAL;

	/* Mark new slot active */
	partition_entries[new_slot_index].attribute_flag |=
						((PART_ATT_PRIORITY_VAL |
						PART_ATT_ACTIVE_VAL |
						PART_ATT_MAX_RETRY_COUNT_VAL));
	partition_entries[new_slot_index].attribute_flag &=
						(~PART_ATT_SUCCESSFUL_VAL
						& ~PART_ATT_UNBOOTABLE_VAL);

	if (!attributes_updated)
		attributes_updated = true;

	/* Update active slot and gpt table */
	partition_mark_active_slot(new_slot);
	return;
}

/*
	This function returns the most priority and active slot,
	also you need to update the global state seperately.

*/
int partition_find_active_slot()
{
	unsigned current_priority;
	int i, count = 0;
	bool current_bootable_bit;
	bool current_active_bit;
	unsigned boot_priority;
	struct partition_entry *partition_entries = partition_get_partition_entries();

	/* Return current active slot if already found */
	if (active_slot != INVALID)
		return active_slot;

	for (boot_priority = MAX_PRIORITY;
			boot_priority > 0; boot_priority--)
	{
		/* Search valid boot slot with highest priority */
		for (i = 0; i < AB_SUPPORTED_SLOTS; i++)
		{
			current_priority = slot_priority(partition_entries, boot_slot_index[i]);
			current_active_bit = slot_is_active(partition_entries, boot_slot_index[i]);
			current_bootable_bit = slot_is_bootable(partition_entries, boot_slot_index[i]);

			/* Count number of slots with all attributes as zero */
			if ( !current_priority &&
				!current_active_bit &&
				current_bootable_bit)
			{
				count ++;
				continue;
			}

#ifdef AB_DEBUG
	dprintf(INFO, "Slot:Priority:Active:Bootable %s:%d:%d:%d \n",
						partition_entries[boot_slot_index[i]].name,
						current_priority,
						current_active_bit,
						current_bootable_bit);
#endif
			if (boot_priority == current_priority)
			{
				if (current_active_bit &&
					current_bootable_bit)
				{
#ifdef AB_DEBUG
	dprintf(INFO, "Slot (%s) is Valid High Priority Slot\n", SUFFIX_SLOT(i));
#endif
					return i;
				}
			}
		}

		/* All slots are zeroed, this is first bootup */
		/* Marking and trying SLOT 0 as default */
		if (count == AB_SUPPORTED_SLOTS)
		{
			/* Update the priority of the boot slot */
			partition_entries[boot_slot_index[SLOT_A]].attribute_flag |=
							((PART_ATT_PRIORITY_VAL |
							PART_ATT_ACTIVE_VAL |
							PART_ATT_MAX_RETRY_COUNT_VAL) &
							(~PART_ATT_SUCCESSFUL_VAL &
							~PART_ATT_UNBOOTABLE_VAL));
			if (!attributes_updated)
				attributes_updated = true;
			return SLOT_A;
		}
	}
	/* If no valid slot */
	return INVALID;
}

static int
next_active_bootable_slot(struct partition_entry *ptn_entry)
{
	int i, slt_index;
	for (i = 0; i < AB_SUPPORTED_SLOTS; i++)
	{
		slt_index = boot_slot_index[i];
		if (slot_is_bootable(ptn_entry, slt_index))
			return i;
	}

	/* NO Bootable slot */
	panic("ERROR: Unable to find any bootable slot");
	return 0;
}

int partition_find_boot_slot()
{
	int boot_slot;
	int slt_index;
	uint64_t boot_retry_count;
	struct partition_entry *partition_entries = partition_get_partition_entries();

	boot_retry_count = 0;
	boot_slot = partition_find_active_slot();

	if (boot_slot == INVALID)
		goto out;

	slt_index = boot_slot_index[boot_slot];

	/*  Boot if partition flag is set to sucessful */
	if (partition_entries[slt_index].attribute_flag & PART_ATT_SUCCESSFUL_VAL)
		goto out;

	boot_retry_count = slot_retry_count(partition_entries, slt_index);

#ifdef AB_DEBUG
	dprintf(INFO, "Boot Partition:RetryCount %s:%lld\n", partition_entries[slt_index].name,
							boot_retry_count);
#endif
	if (!boot_retry_count)
	{
		/* Mark slot invalide and unbootable */
		partition_entries[slt_index].attribute_flag |=
					(PART_ATT_UNBOOTABLE_VAL &
					~PART_ATT_ACTIVE_VAL &
					~PART_ATT_PRIORITY_VAL);

		partition_switch_slots(boot_slot, next_active_bootable_slot(partition_entries));

		reboot_device(0);
	}
	else
	{
		/* Do normal boot */
		/* Decrement retry count */
		partition_entries[slt_index].attribute_flag =
					(partition_entries[slt_index].attribute_flag
					& ~PART_ATT_MAX_RETRY_COUNT_VAL)
					| ((boot_retry_count-1) << PART_ATT_MAX_RETRY_CNT_BIT);

		if (!attributes_updated)
			attributes_updated = true;

		goto out;
	}

out:
#ifdef AB_DEBUG
	dprintf(INFO, "Booting SLOT %d\n", boot_slot);
#endif
	return boot_slot;
}

static
void guid_update(struct partition_entry *partition_entries,
		unsigned old_index,
		unsigned new_index)
{
	unsigned char tmp_guid[PARTITION_TYPE_GUID_SIZE];

	memcpy(tmp_guid, partition_entries[old_index].type_guid,
				PARTITION_TYPE_GUID_SIZE);
	memcpy(partition_entries[old_index].type_guid,
			partition_entries[new_index].type_guid,
			PARTITION_TYPE_GUID_SIZE);
	memcpy(partition_entries[new_index].type_guid, tmp_guid,
				PARTITION_TYPE_GUID_SIZE);
	return;
}

/*
	Function to swap guids of slots
*/
static void
swap_guid(int old_slot,	int new_slot)
{
	unsigned i, j, tmp_strlen;
	unsigned partition_cnt = partition_get_partition_count();
	struct partition_entry *partition_entries =
			partition_get_partition_entries();
	const char *ptr_pname, *ptr_suffix;

	if ( old_slot == new_slot)
		return;

	for(i = 0; i < partition_cnt; i++)
	{
		ptr_pname = (const char *)partition_entries[i].name;

	        /* Search for suffix in partition name */
		if ((ptr_suffix = strstr(ptr_pname, SUFFIX_SLOT(new_slot))))
		{
			for (j = i+1; j < partition_cnt; j++)
			{
				tmp_strlen = strlen(ptr_pname)-strlen(SUFFIX_SLOT(new_slot));
				if (!strncmp((const char*)partition_entries[j].name, ptr_pname, tmp_strlen) &&
					strstr((const char*)partition_entries[j].name, SUFFIX_SLOT(old_slot)))
					guid_update(partition_entries, j, i);
			}
		}
		else if ((ptr_suffix = strstr(ptr_pname, SUFFIX_SLOT(old_slot))))
		{
			for (j = i+1; j < partition_cnt; j++)
			{
				tmp_strlen = strlen(ptr_pname)-strlen(SUFFIX_SLOT(old_slot));
				if (!strncmp((const char *)partition_entries[j].name, ptr_pname, tmp_strlen) &&
					strstr((const char *)partition_entries[j].name, SUFFIX_SLOT(new_slot)))
					guid_update(partition_entries, i, j);
			}
		}
	}
}

/*
	Function: Mark the slot to be active and also conditionally
	update the slot parameters if there is a change.
*/
void partition_mark_active_slot(signed slot)
{
	if (active_slot == slot)
		goto out;

	switch (active_slot)
	{
		case INVALID:
			if (slot != SLOT_A)
				swap_guid(SLOT_A, slot);
			goto out;
		default:
			if (slot == INVALID)
				swap_guid(active_slot, SLOT_A);
			else
				swap_guid(active_slot, slot);
	}
	active_slot = slot;
out:
	if (attributes_updated)
		attributes_update();
	return;
}

/* Function to find if multislot is supported */
bool partition_multislot_is_supported()
{
	return multislot_support;
}

/*
	Function to populate partition meta used
	for fastboot get var info publication.

	Input partition_entries, partition_count and
	buffer to fill information.

*/
int partition_fill_partition_meta(char has_slot_pname[][MAX_GET_VAR_NAME_SIZE],
					char has_slot_reply[][MAX_RSP_SIZE],
					int array_size)
{
	int i,j,tmp;
	int count = 0;
	char *pname = NULL;
	int pname_size;
	struct partition_entry *partition_entries =
				partition_get_partition_entries();
	int partition_count = partition_get_partition_count();
	char *suffix_str;

	for (i=0; i<partition_count; i++)
	{
		pname = (char *)partition_entries[i].name;
		pname_size = strlen(pname);
		suffix_str = NULL;
 #ifdef AB_DEBUG
	dprintf(INFO, "Transversing partition %s\n", pname);
 #endif
		/* 1. Find partition, if it is A/B enabled. */
		for ( j = 0; j<AB_SUPPORTED_SLOTS; j++)
		{
			suffix_str = strstr(pname, SUFFIX_SLOT(j));
			if (suffix_str)
				break;
		}

		if (suffix_str)
		{
			if (!strcmp(suffix_str, SUFFIX_SLOT(SLOT_A)))
			{
				/* 2. put the partition name in array */
				tmp = pname_size-strlen(suffix_str);
				strlcpy(has_slot_pname[count], pname, tmp+1);
				strlcpy(has_slot_reply[count], " Yes", MAX_RSP_SIZE);
				count++;
			}
		}
		else
		{
			strlcpy(has_slot_pname[count], pname, MAX_GET_VAR_NAME_SIZE);
			strlcpy(has_slot_reply[count], " No", MAX_RSP_SIZE);
			count++;
		}

		/* Avoid over population of array provided */
		if (count >= array_size)
		{
			dprintf(CRITICAL, "ERROR: Not able to parse all partitions\n");
			return count;
		}
	}
#ifdef AB_DEBUG
	for (i=0; i<count; i++)
		dprintf(INFO, "has-slot:%s:%s\n", has_slot_pname[i], has_slot_reply[i]);
#endif
	return count;
}

/*
	Function to populate the slot meta used
	for fastboot get var info publication.
*/
void partition_fill_slot_meta(struct ab_slot_info *slot_info)
{
	int i, current_slot_index;
	struct partition_entry *ptn_entries = partition_get_partition_entries();
	char buff[3];

	/* Update slot info */
	for(i=0; i<AB_SUPPORTED_SLOTS; i++)
	{
		current_slot_index = boot_slot_index[i];
		strlcpy(slot_info[i].slot_is_unbootable_rsp,
				slot_is_bootable(ptn_entries, current_slot_index)?"No":"Yes",
				MAX_RSP_SIZE);
		strlcpy(slot_info[i].slot_is_active_rsp,
				slot_is_active(ptn_entries, current_slot_index)?"Yes":"No",
				MAX_RSP_SIZE);
		strlcpy(slot_info[i].slot_is_succesful_rsp,
				slot_is_sucessful(ptn_entries, current_slot_index)?"Yes":"No",
				MAX_RSP_SIZE);
		itoa(slot_retry_count(ptn_entries, current_slot_index),
				(unsigned char *)buff, 2, 10);
		strlcpy(slot_info[i].slot_retry_count_rsp, buff, MAX_RSP_SIZE);
	}
}

/*
	Function to read and update the attributes of
	GPT
*/
static int
update_gpt(uint64_t gpt_start_addr,
		uint64_t gpt_hdr_offset,
		uint64_t gpt_entries_offset)
{
	char *buffer = NULL;
	char *gpt_entries_ptr, *gpt_hdr_ptr, *tmp = NULL;
	struct partition_entry *partition_entries = partition_get_partition_entries();
	uint32_t partition_count = partition_get_partition_count();
	unsigned i,max_partition_count = 0;
	unsigned partition_entry_size = 0;
	uint32_t block_size = mmc_get_device_blocksize();
	uint32_t crc_val = 0;
	int ret = 0;
	uint64_t max_gpt_size_bytes =
		(PARTITION_ENTRY_SIZE*NUM_PARTITIONS + GPT_HEADER_BLOCKS*block_size);

	buffer = memalign(CACHE_LINE, ROUNDUP(max_gpt_size_bytes, CACHE_LINE));
	if (!buffer)
	{
		dprintf(CRITICAL, "update_gpt: Failed at memory allocation\n");
		goto out;
	}

	ret = mmc_read(gpt_start_addr, (uint32_t *)buffer,
				max_gpt_size_bytes);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to read GPT\n");
		goto out;
	}

	/* 0. Intialise ptrs for header and entries */
	gpt_entries_ptr = buffer + gpt_entries_offset*block_size;
	gpt_hdr_ptr = buffer + gpt_hdr_offset*block_size;

	/* 1. Update attributes_flag of partition entry */
	tmp = gpt_entries_ptr;
	for (i=0;i<partition_count;i++)
	{
		/* Update the partition attributes */
		PUT_LONG_LONG(&tmp[ATTRIBUTE_FLAG_OFFSET],
			partition_entries[i].attribute_flag);
		memscpy(tmp, PARTITION_TYPE_GUID_SIZE, partition_entries[i].type_guid,
				PARTITION_TYPE_GUID_SIZE);

		/* point to the next partition entry */
		tmp += PARTITION_ENTRY_SIZE;
	}

	/* Calculate and update CRC of partition entries array */
	max_partition_count =
		    GET_LWORD_FROM_BYTE(&gpt_hdr_ptr[PARTITION_COUNT_OFFSET]);
	partition_entry_size =
		    GET_LWORD_FROM_BYTE(&gpt_hdr_ptr[PENTRY_SIZE_OFFSET]);
	crc_val  = crc32(~0L, gpt_entries_ptr, ((max_partition_count) *
				(partition_entry_size))) ^ (~0L);
	PUT_LONG(&gpt_hdr_ptr[PARTITION_CRC_OFFSET], crc_val);


	/* Write CRC to 0 before we calculate the crc of the GPT header */
	crc_val = 0;
	PUT_LONG(&gpt_hdr_ptr[HEADER_CRC_OFFSET], crc_val);
	crc_val  = crc32(~0L,gpt_hdr_ptr, GPT_HEADER_SIZE) ^ (~0L);
	PUT_LONG(&gpt_hdr_ptr[HEADER_CRC_OFFSET], crc_val);

	/* write to mmc */
	ret = mmc_write(gpt_start_addr, max_gpt_size_bytes, buffer);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to write gpt\n");
		goto out;
	}
out:
	if (buffer)
		free(buffer);
	return ret;
}

/**
	Function to update the backup and primary gpt
	partition.
**/
static void attributes_update()
{
	uint64_t offset;
	uint64_t gpt_entries_offset, gpt_hdr_offset;
	uint64_t gpt_start_addr;
	int ret;
	uint32_t block_size = mmc_get_device_blocksize();
	unsigned max_entries_size_bytes = PARTITION_ENTRY_SIZE*NUM_PARTITIONS;
	unsigned max_entries_blocks = max_entries_size_bytes/block_size;
	unsigned max_gpt_blocks = GPT_HEADER_BLOCKS + max_entries_blocks;

	/* Update Primary GPT */
	offset = 0x01;	/*  offset is 0x1 for primary GPT */
	gpt_start_addr = offset*block_size;
	/* Take gpt_start_addr as start and calculate offset from that in block sz*/
	gpt_hdr_offset = 0; /* For primary partition offset is zero */
	gpt_entries_offset = GPT_HEADER_BLOCKS;

	ret = update_gpt(gpt_start_addr, gpt_hdr_offset, gpt_entries_offset);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to update Primary GPT\n");
		return;
	}

	/* Update Secondary GPT */
	offset = ((mmc_get_device_capacity()/block_size) - max_gpt_blocks);
	gpt_start_addr = offset*block_size;
	gpt_hdr_offset = max_entries_blocks;
	gpt_entries_offset = 0; /* For secondary GPT entries offset is zero */

	ret = update_gpt(gpt_start_addr, gpt_hdr_offset, gpt_entries_offset);
	if (ret)
	{
		dprintf(CRITICAL, "Failed to update Secondary GPT\n");
		return;
	}
	return;
}
