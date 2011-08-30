/*
 * Copyright (c) 2009, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 *  * Neither the name of Google, Inc. nor the names of its contributors
 *	may be used to endorse or promote products derived from this
 *	software without specific prior written permission.
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

#include <reg.h>
#include <debug.h>
#include <lib/ptable.h>
#include <dev/flash.h>
#include <smem.h>
#include <gsbi.h>
#include <platform/iomap.h>
#include <platform.h>

#define MDM9X15_CDP	3675
#define MDM9X15_MTP	3681
#define LINUX_MACHTYPE  MDM9X15_CDP

#define VARIABLE_LENGTH		0x10101010
#define DIFF_START_ADDR		0xF0F0F0F0
#define NUM_PAGES_PER_BLOCK	0x40

static struct ptable flash_ptable;
unsigned hw_platform = 0;
unsigned target_msm_id = 0;

static const uint8_t uart_gsbi_id = GSBI_ID_4;

/* for these partitions, start will be offset by either what we get from
 * smem, or from the above offset if smem is not useful. Also, we should
 * probably have smem_ptable code populate our flash_ptable.
 *
 * When smem provides us with a full partition table, we can get rid of
 * this altogether.
 *
 */
static struct ptentry board_part_list_default[] = {
	{
		.start = 0,
		.length = 10 /* In MB */,
		.name = "boot",
	},
	{
		.start = 0xF0F0F0F0,
		.length = 5 /* In MB */,
		.name = "system",
	},
};
static int num_parts = sizeof(board_part_list_default)/sizeof(struct ptentry);

void smem_ptable_init(void);
unsigned smem_get_apps_flash_start(void);
extern void reboot(unsigned reboot_reason);

void target_init(void)
{
	unsigned offset;
	struct flash_info *flash_info;
	struct ptentry *board_part_list;
	unsigned total_num_of_blocks;
	unsigned next_ptr_start_adr = 0;
	unsigned blocks_per_1MB = 8; /* Default value of 2k page size on 256MB flash drive*/
	int i;

	dprintf(INFO, "target_init()\n");
	ptable_init(&flash_ptable);
	smem_ptable_init();

	flash_init();
	flash_info = flash_get_info();
	ASSERT(flash_info);

	offset = smem_get_apps_flash_start();
	if (offset == 0xffffffff)
			while(1);

	total_num_of_blocks = flash_info->num_blocks;
	blocks_per_1MB = (1 << 20) / (flash_info->block_size);

	//TODO: Remove this if no need for other board types
	board_part_list = board_part_list_default;

	for (i = 0; i < num_parts; i++) {
		struct ptentry *ptn = &board_part_list[i];
		unsigned len = ((ptn->length) * blocks_per_1MB);

		if(ptn->start != 0)
				ASSERT(ptn->start == DIFF_START_ADDR);

		ptn->start = next_ptr_start_adr;

		if(ptn->length == VARIABLE_LENGTH)
		{
			unsigned length_for_prt = 0;
			unsigned j;
			for (j = i+1; j < num_parts; j++)
			{
					struct ptentry *temp_ptn = &board_part_list[j];
					ASSERT(temp_ptn->length != VARIABLE_LENGTH);
					length_for_prt += ((temp_ptn->length) * blocks_per_1MB);
			}
				len = (total_num_of_blocks - 1) - (offset + ptn->start + length_for_prt);
			ASSERT(len >= 0);
		}
		next_ptr_start_adr = ptn->start + len;
		ptable_add(&flash_ptable, ptn->name, offset + ptn->start,
			   len, ptn->flags, TYPE_APPS_PARTITION, PERM_WRITEABLE);
	}

	smem_add_modem_partitions(&flash_ptable);

	ptable_dump(&flash_ptable);
	flash_set_ptable(&flash_ptable);
}

void board_info(void)
{
	struct smem_board_info_v4 board_info_v4;
	unsigned int board_info_len = 0;
	unsigned smem_status;
	unsigned format = 0;
	unsigned id = 0;

	if (hw_platform && target_msm_id)
		return;

	hw_platform = MDM9X15_CDP;
	target_msm_id = MDM9600;

	smem_status = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						&format, sizeof(format), 0);
	if(!smem_status)
	{
		if(format == 4)
		{
			board_info_len = sizeof(board_info_v4);
			smem_status = smem_read_alloc_entry(
						SMEM_BOARD_INFO_LOCATION,
						&board_info_v4, board_info_len);
			if(!smem_status)
			{
				id = board_info_v4.board_info_v3.hw_platform;
				target_msm_id =
					board_info_v4.board_info_v3.msm_id;
			}
		}
	}
	return;
}

unsigned board_machtype(void)
{
	board_info();
	return hw_platform;
}

void reboot_device(unsigned reboot_reason)
{
	//TODO: Implement
	return;
}

uint8_t target_uart_gsbi(void)
{
	return uart_gsbi_id;
}

uint32_t usb_cable_status(void) {
	//TODO: Implement
	uint32_t ret = 1;
	return ret;
}
