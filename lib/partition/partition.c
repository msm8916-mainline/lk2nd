/*
 * Copyright (c) 2009 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <assert.h>
#include <stdlib.h>
#include <debug.h>
#include <printf.h>
#include <string.h>
#include <compiler.h>
#include <stdlib.h>
#include <arch.h>
#include <lib/bio.h>
#include <lib/partition.h>

#include "gpt.h"

struct chs {
	uint8_t c;
	uint8_t h;
	uint8_t s;
} __PACKED;

struct mbr_part {
	uint8_t status;
	struct chs start;
	uint8_t type;
	struct chs end;
	uint32_t lba_start;
	uint32_t lba_length;
} __PACKED;

struct gpt_header {
	uint64_t first_usable_lba;
	uint32_t partition_entry_size;
	uint32_t header_size;
	uint32_t max_partition_count;
};

static status_t validate_mbr_partition(bdev_t *dev, const struct mbr_part *part)
{
	/* check for invalid types */
	if (part->type == 0)
		return -1;
	/* check for invalid status */
	if (part->status != 0x80 && part->status != 0x00)
		return -1;

	/* make sure the range fits within the device */
	if (part->lba_start >= dev->block_count)
		return -1;
	if ((part->lba_start + part->lba_length) > dev->block_count)
		return -1;

	/* that's about all we can do, MBR has no other good way to see if it's valid */

	return 0;
}

/*
 * Parse the gpt header and get the required header fields
 * Return 0 on valid signature
 */
static unsigned int
partition_parse_gpt_header(unsigned char *buffer, struct gpt_header* header)
{
	/* Check GPT Signature */
	if (((uint32_t *) buffer)[0] != GPT_SIGNATURE_2 ||
	    ((uint32_t *) buffer)[1] != GPT_SIGNATURE_1)
		return 1;

	header->header_size = GET_LWORD_FROM_BYTE(&buffer[HEADER_SIZE_OFFSET]);
	header->first_usable_lba =
	    GET_LLWORD_FROM_BYTE(&buffer[FIRST_USABLE_LBA_OFFSET]);
	header->max_partition_count =
	    GET_LWORD_FROM_BYTE(&buffer[PARTITION_COUNT_OFFSET]);
	header->partition_entry_size =
	    GET_LWORD_FROM_BYTE(&buffer[PENTRY_SIZE_OFFSET]);

	return 0;
}

int partition_publish(const char *device, off_t offset)
{
	int err = 0;
	int count = 0;

	// clear any partitions that may have already existed
	partition_unpublish(device);

	bdev_t *dev = bio_open(device);
	if (!dev) {
		printf("partition_publish: unable to open device\n");
		return -1;
	}

	// get a dma aligned and padded block to read info
	STACKBUF_DMA_ALIGN(buf, dev->block_size);
	
	/* sniff for MBR partition types */
	do {
		unsigned int i, j, n;
		int gpt_partitions_exist = 0;

		err = bio_read(dev, buf, offset, 512);
		if (err < 0)
			goto err;

		/* look for the aa55 tag */
		if (buf[510] != 0x55 || buf[511] != 0xaa)
			break;

		/* see if a partition table makes sense here */
		struct mbr_part part[4];
		memcpy(part, buf + 446, sizeof(part));

#if DEBUGLEVEL >= INFO
		dprintf(INFO, "mbr partition table dump:\n");
		for (i=0; i < 4; i++) {
			dprintf(INFO, "\t%i: status 0x%hhx, type 0x%hhx, start 0x%x, len 0x%x\n", i, part[i].status, part[i].type, part[i].lba_start, part[i].lba_length);
		}
#endif

		/* validate each of the partition entries */
		for (i=0; i < 4; i++) {
			if (validate_mbr_partition(dev, &part[i]) >= 0) {
				// publish it
				char subdevice[128];

				/* Type 0xEE indicates end of MBR and GPT partitions exist */
				if(part[i].type==0xee) {
					gpt_partitions_exist = 1;
					break;
				}

				sprintf(subdevice, "%sp%d", device, i); 

				err = bio_publish_subdevice(device, subdevice, part[i].lba_start, part[i].lba_length);
				if (err < 0) {
					dprintf(INFO, "error publishing subdevice '%s'\n", subdevice);
					continue;
				}
				count++;
			}
		}

		if(!gpt_partitions_exist) break;
		dprintf(INFO, "found GPT\n");

		err = bio_read(dev, buf, offset + dev->block_size, dev->block_size);
		if (err < 0)
			goto err;

		struct gpt_header gpthdr;
		err = partition_parse_gpt_header(buf, &gpthdr);
		if (err) {
			/* Check the backup gpt */

			uint64_t backup_header_lba = dev->block_count - 1;
			err = bio_read(dev, buf, (backup_header_lba * dev->block_size), dev->block_size);
			if (err < 0) {
				dprintf(CRITICAL, "GPT: Could not read backup gpt from mmc\n");
				break;
			}

			err = partition_parse_gpt_header(buf, &gpthdr);
			if (err) {
				dprintf(CRITICAL, "GPT: Primary and backup signatures invalid\n");
				break;
			}
		}

		uint32_t part_entry_cnt = dev->block_size / ENTRY_SIZE;
		uint64_t partition_0 = GET_LLWORD_FROM_BYTE(&buf[PARTITION_ENTRIES_OFFSET]);
		/* Read GPT Entries */
		for (i = 0; i < (ROUNDUP(gpthdr.max_partition_count, part_entry_cnt)) / part_entry_cnt; i++) {
			err = bio_read(dev, buf, offset + (partition_0 * dev->block_size) + (i * dev->block_size),
							dev->block_size);

			if (err < 0) {
				dprintf(CRITICAL,
					"GPT: mmc read card failed reading partition entries.\n");
				break;
			}

			for (j = 0; j < part_entry_cnt; j++) {
				unsigned char type_guid[PARTITION_TYPE_GUID_SIZE];
				unsigned char name[MAX_GPT_NAME_SIZE];
				unsigned char UTF16_name[MAX_GPT_NAME_SIZE];
				uint64_t first_lba, last_lba, size;

				// guid
				memcpy(&type_guid,
			       &buf[(j * gpthdr.partition_entry_size)],
			       PARTITION_TYPE_GUID_SIZE);
				if (type_guid[0]==0 && type_guid[1]==0) {
					i = ROUNDUP(gpthdr.max_partition_count, part_entry_cnt);
					break;
				}

				// size
				first_lba = GET_LLWORD_FROM_BYTE(&buf[(j * gpthdr.partition_entry_size) + FIRST_LBA_OFFSET]);
				last_lba = GET_LLWORD_FROM_BYTE(&buf[(j * gpthdr.partition_entry_size) + LAST_LBA_OFFSET]);
				size = last_lba - first_lba + 1;

				// name
				memset(&UTF16_name, 0x00, MAX_GPT_NAME_SIZE);
				memcpy(UTF16_name, &buf[(j * gpthdr.partition_entry_size) +
					PARTITION_NAME_OFFSET], MAX_GPT_NAME_SIZE);

				/*
				 * Currently partition names in *.xml are UTF-8 and lowercase
				 * Only supporting english for now so removing 2nd byte of UTF-16
				 */
				for (n = 0; n < MAX_GPT_NAME_SIZE / 2; n++) {
					name[n] = UTF16_name[n * 2];
				}

				//dprintf(CRITICAL, "got part '%s' size=%llu!\n", name, size);
				char subdevice[128];
				sprintf(subdevice, "%sp%d", device, count+1);

				err = bio_publish_subdevice(device, subdevice, first_lba, size);
				if (err < 0) {
					dprintf(INFO, "error publishing subdevice '%s'\n", name);
					continue;
				}

				bdev_t *partdev = bio_open(subdevice);
				partdev->label = strdup((char*)name);
				partdev->is_gpt = true;

				/* Some linux distros make use of subpartitions.
				 * Scan some devices recursively to publish them. */
				if (strcmp(name, "system") == 0 || strcmp(name, "userdata") == 0)
					partition_publish(subdevice, 0);

				count++;
			}
		}
	} while(0);

	bio_close(dev);

err:
	return (err < 0) ? err : count;
}

int partition_unpublish(const char *device)
{
	int i;
	int count;
	bdev_t *dev;
	char devname[512];	

	count = 0;
	for (i=0; i < 16; i++) {
		sprintf(devname, "%sp%d", device, i);

		dev = bio_open(devname);
		if (!dev)
			continue;

		bio_unregister_device(dev);
		bio_close(dev);
		count++;
	}

	return count;
}

