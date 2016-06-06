/* Copyright (c) 2012-2016, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <debug.h>
#include <board.h>
#include <smem.h>
#include <baseband.h>
#include <boot_device.h>

static uint16_t format_major = 0;
static uint16_t format_minor = 0;

static struct board_data board = {UNKNOWN,
	0,
	0,
	0,
	HW_PLATFORM_UNKNOWN,
	HW_PLATFORM_SUBTYPE_UNKNOWN,
	LINUX_MACHTYPE_UNKNOWN,
	BASEBAND_MSM,
	{{PMIC_IS_INVALID, 0, 0}, {PMIC_IS_INVALID, 0, 0},
	{PMIC_IS_INVALID, 0, 0}},
	0,
	0,
	0,
	NULL,
};

static void platform_detect()
{
	struct smem_board_info_v6 board_info_v6;
	struct smem_board_info_v7 board_info_v7;
	struct smem_board_info_v8 board_info_v8;
	struct smem_board_info_v9 board_info_v9;
	struct smem_board_info_v10 board_info_v10;
	struct smem_board_info_v11 board_info_v11;
	unsigned int board_info_len = 0;
	unsigned ret = 0;
	unsigned format = 0;
	unsigned pmic_type = 0;
	uint8_t i;

	ret = smem_read_alloc_entry_offset(SMEM_BOARD_INFO_LOCATION,
						   &format, sizeof(format), 0);
	if (ret)
		return;

	/* Extract the major & minor version info,
	 * Upper two bytes: major info
	 * Lower two byets: minor info
	 */
	format_major = (format & 0xffff0000) >> 16;
	format_minor = format & 0x0000ffff;

	if (format_major == 0x0)
	{
		if (format_minor == 6)
		{
			board_info_len = sizeof(board_info_v6);

			ret = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info_v6,
					board_info_len);
			if (ret)
				return;

			board.platform = board_info_v6.board_info_v3.msm_id;
			board.platform_version = board_info_v6.board_info_v3.msm_version;
			board.platform_hw = board_info_v6.board_info_v3.hw_platform;
			board.platform_subtype = board_info_v6.platform_subtype;
		}
		else if (format_minor == 7)
		{
			board_info_len = sizeof(board_info_v7);

			ret = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info_v7,
					board_info_len);
			if (ret)
				return;

			board.platform = board_info_v7.board_info_v3.msm_id;
			board.platform_version = board_info_v7.board_info_v3.msm_version;
			board.platform_hw = board_info_v7.board_info_v3.hw_platform;
			board.platform_subtype = board_info_v7.platform_subtype;
			board.pmic_info[0].pmic_type = board_info_v7.pmic_type;
			board.pmic_info[0].pmic_version = board_info_v7.pmic_version;
		}
		else if (format_minor == 8)
		{
			board_info_len = sizeof(board_info_v8);

			ret = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info_v8,
					board_info_len);
			if (ret)
				return;

			board.platform = board_info_v8.board_info_v3.msm_id;
			board.platform_version = board_info_v8.board_info_v3.msm_version;
			board.platform_hw = board_info_v8.board_info_v3.hw_platform;
			board.platform_subtype = board_info_v8.platform_subtype;

			/*
			 * fill in board.target with variant_id information
			 * bit no         |31  24 | 23   16            | 15   8             |7         0|
			 * board.target = |subtype| plat_hw_ver major  | plat_hw_ver minor  |hw_platform|
			 *
			 */
			board.target = (((board_info_v8.platform_subtype & 0xff) << 24) |
						   (((board_info_v8.platform_version >> 16) & 0xff) << 16) |
						   ((board_info_v8.platform_version & 0xff) << 8) |
						   (board_info_v8.board_info_v3.hw_platform & 0xff));

			for (i = 0; i < SMEM_V8_SMEM_MAX_PMIC_DEVICES; i++) {
				board.pmic_info[i].pmic_type = board_info_v8.pmic_info[i].pmic_type;
				board.pmic_info[i].pmic_version = board_info_v8.pmic_info[i].pmic_version;

				/*
				 * fill in pimc_board_info with pmic type and pmic version information
				 * bit no  		  	    |31  24   | 23  16 	    | 15   8 	     |7		  0|
				 * pimc_board_info = |Unused | Major version | Minor version|PMIC_MODEL|
				 *
				 */
				pmic_type = board_info_v8.pmic_info[i].pmic_type == PMIC_IS_INVALID? 0 : board_info_v8.pmic_info[i].pmic_type;

				board.pmic_info[i].pmic_target = (((board_info_v8.pmic_info[i].pmic_version >> 16) & 0xff) << 16) |
					   ((board_info_v8.pmic_info[i].pmic_version & 0xff) << 8) | (pmic_type & 0xff);
			}
		}
		else if (format_minor == 0x9)
		{
			board_info_len = sizeof(board_info_v9);

			ret = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info_v9,
					board_info_len);
			if (ret)
				return;

			board.platform = board_info_v9.board_info_v3.msm_id;
			board.platform_version = board_info_v9.board_info_v3.msm_version;
			board.platform_hw = board_info_v9.board_info_v3.hw_platform;
			board.platform_subtype = board_info_v9.platform_subtype;

			/*
			 * fill in board.target with variant_id information
			 * bit no         |31  24 | 23   16            | 15   8             |7         0|
			 * board.target = |subtype| plat_hw_ver major  | plat_hw_ver minor  |hw_platform|
			 *
			 */
			board.target = (((board_info_v9.platform_subtype & 0xff) << 24) |
						   (((board_info_v9.platform_version >> 16) & 0xff) << 16) |
						   ((board_info_v9.platform_version & 0xff) << 8) |
						   (board_info_v9.board_info_v3.hw_platform & 0xff));

			for (i = 0; i < SMEM_V8_SMEM_MAX_PMIC_DEVICES; i++) {
				board.pmic_info[i].pmic_type = board_info_v9.pmic_info[i].pmic_type;
				board.pmic_info[i].pmic_version = board_info_v9.pmic_info[i].pmic_version;

				/*
				 * fill in pimc_board_info with pmic type and pmic version information
				 * bit no  		  	    |31  24   | 23  16 	    | 15   8 	     |7		  0|
				 * pimc_board_info = |Unused | Major version | Minor version|PMIC_MODEL|
				 *
				 */
				pmic_type = board_info_v9.pmic_info[i].pmic_type == PMIC_IS_INVALID? 0 : board_info_v9.pmic_info[i].pmic_type;

				board.pmic_info[i].pmic_target = (((board_info_v9.pmic_info[i].pmic_version >> 16) & 0xff) << 16) |
					   ((board_info_v9.pmic_info[i].pmic_version & 0xff) << 8) | (pmic_type & 0xff);
			}
			board.foundry_id = board_info_v9.foundry_id;
		}
		else if (format_minor == 0xA)
		{
			board_info_len = sizeof(board_info_v10);

			ret = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info_v10,
					board_info_len);
			if (ret)
				return;

			board.platform = board_info_v10.board_info_v3.msm_id;
			board.platform_version = board_info_v10.board_info_v3.msm_version;
			board.platform_hw = board_info_v10.board_info_v3.hw_platform;
			board.platform_subtype = board_info_v10.platform_subtype;

			/*
			 * fill in board.target with variant_id information
			 * bit no         |31  24 | 23   16            | 15   8             |7         0|
			 * board.target = |subtype| plat_hw_ver major  | plat_hw_ver minor  |hw_platform|
			 *
			 */
			board.target = (((board_info_v10.platform_subtype & 0xff) << 24) |
						   (((board_info_v10.platform_version >> 16) & 0xff) << 16) |
						   ((board_info_v10.platform_version & 0xff) << 8) |
						   (board_info_v10.board_info_v3.hw_platform & 0xff));

			for (i = 0; i < SMEM_V8_SMEM_MAX_PMIC_DEVICES; i++) {
				board.pmic_info[i].pmic_type = board_info_v10.pmic_info[i].pmic_type;
				board.pmic_info[i].pmic_version = board_info_v10.pmic_info[i].pmic_version;

				/*
				 * fill in pimc_board_info with pmic type and pmic version information
				 * bit no  		  	    |31  24   | 23  16 	    | 15   8 	     |7		  0|
				 * pimc_board_info = |Unused | Major version | Minor version|PMIC_MODEL|
				 *
				 */
				pmic_type = board_info_v10.pmic_info[i].pmic_type == PMIC_IS_INVALID? 0 : board_info_v10.pmic_info[i].pmic_type;

				board.pmic_info[i].pmic_target = (((board_info_v10.pmic_info[i].pmic_version >> 16) & 0xff) << 16) |
					   ((board_info_v10.pmic_info[i].pmic_version & 0xff) << 8) | (pmic_type & 0xff);
			}
			board.foundry_id = board_info_v10.foundry_id;
			board.chip_serial = board_info_v10.chip_serial;
		}
		else if (format_minor >= 0xB)
		{
			board_info_len = sizeof(board_info_v11);

			ret = smem_read_alloc_entry(SMEM_BOARD_INFO_LOCATION,
					&board_info_v11,
					board_info_len);
			if (ret)
				return;

			board.platform = board_info_v11.board_info_v3.msm_id;
			board.platform_version = board_info_v11.board_info_v3.msm_version;
			board.platform_hw = board_info_v11.board_info_v3.hw_platform;
			board.platform_subtype = board_info_v11.platform_subtype;
			/*
			 * fill in board.target with variant_id information
			 * bit no         |31  24 |23             16|15              8|7         0|
			 * board.target = |subtype|plat_hw_ver major|plat_hw_ver minor|hw_platform|
			 *
			 */

			board.target = (((board_info_v11.platform_subtype & 0xff) << 24) |
						   (((board_info_v11.platform_version >> 16) & 0xff) << 16) |
						   ((board_info_v11.platform_version & 0xff) << 8) |
						   (board_info_v11.board_info_v3.hw_platform & 0xff));

			board.foundry_id = board_info_v11.foundry_id;
			board.chip_serial = board_info_v11.chip_serial;
			board.num_pmics = board_info_v11.num_pmics;
			board.pmic_array_offset = board_info_v11.pmic_array_offset;
		}

		/* HLOS subtype
		 * bit no                        |31    20 | 19        16|15    13 |12      11 | 10          8 | 7     0|
		 * board.platform_hlos_subtype = |reserved | Boot device |Reserved | Panel     | DDR detection | subtype|
		 *                               |  bits   |             |  bits   | Detection |
		 */
		board.platform_hlos_subtype = (board_get_ddr_subtype() << 8) | (platform_get_boot_dev() << 16) | (platform_detect_panel() << 11);
	}
	else
	{
		dprintf(CRITICAL, "Unsupported board info format %u.%u\n", format_major, format_minor);
		ASSERT(0);
	}
}

void pmic_info_populate()
{
	uint32_t pmic_type = 0;
	void *smem_board_info_addr = NULL;
	uint32_t smem_board_info_size = 0;

	if (format_minor < 0xB)
	{
		// not needed for smem versions < 0xB
		return;
	}
	smem_board_info_addr = smem_get_alloc_entry(SMEM_BOARD_INFO_LOCATION, &smem_board_info_size);
	if (smem_board_info_addr == NULL)
	{
		dprintf(CRITICAL, "Error reading the smem board info address\n");
		ASSERT(0);
	}

	if (smem_board_info_size < board.pmic_array_offset)
	{
		dprintf(CRITICAL, "Invalid SMEM board info\n");
		ASSERT(0);
	}

	if(!(board.pmic_info_array = malloc(board.num_pmics * sizeof(struct board_pmic_data))))
	{
		dprintf(CRITICAL, "Error allocating memory for pmic info array\n");
		ASSERT(0);
	}

	struct board_pmic_data *info = board.pmic_info_array;
	for (uint8_t i = 0; i < board.num_pmics; i++)
	{
		memcpy(info, smem_board_info_addr + board.pmic_array_offset + (i * sizeof(struct smem_pmic_info)), sizeof(struct smem_pmic_info));
		/*
		 * fill in pimc_board_info with pmic type and pmic version information
		 * bit no  		  	    |31  24|23         16|15          8|7		 0|
		 * pimc_board_info =    |Unused|Major version|Minor version|PMIC_MODEL|
		 *
		 */
		pmic_type = info->pmic_type == PMIC_IS_INVALID? 0 : info->pmic_type;
		info->pmic_target = (((info->pmic_version >> 16) & 0xff) << 16) |
			   ((info->pmic_version & 0xff) << 8) | (pmic_type & 0xff);
		info++;
	}
}

void board_init()
{
	platform_detect();
	target_detect(&board);
	target_baseband_detect(&board);
}

uint32_t board_platform_id(void)
{
	return board.platform;
}

uint32_t board_target_id()
{
	return board.target;
}

uint32_t board_baseband()
{
	return board.baseband;
}

uint32_t board_hardware_id()
{
	return board.platform_hw;
}

uint32_t board_hardware_subtype(void)
{
	return board.platform_subtype;
}

uint32_t board_foundry_id(void)
{
	return board.foundry_id;
}

uint32_t board_chip_serial(void)
{
	return board.chip_serial;
}

uint8_t board_pmic_info(struct board_pmic_data *info, uint8_t num_ent)
{
	uint8_t i;
	if (format_major == 0x0)
	{
		if (format_minor < 0xB)
		{
			for (i = 0; i < num_ent && i < SMEM_MAX_PMIC_DEVICES; i++) {
				info->pmic_type = board.pmic_info[i].pmic_type;
				info->pmic_version = board.pmic_info[i].pmic_version;
				info->pmic_target = board.pmic_info[i].pmic_target;
				info++;
			}
			return (i--);
		}
	}
	return 0;
}

bool board_pmic_type(uint32_t type)
{
	uint8_t i;
	if (format_major == 0x0 && format_minor >= 0x0B)
	{
		for (i = 0; i < SMEM_MAX_PMIC_DEVICES; i++)
		{
			if (type == (board.pmic_info_array[i].pmic_type & 0x0000ffff))
				return true;
		}
	}

	return false;
}

void board_pmi_target_set(uint8_t num_ent,uint8_t rev)
{
	if (format_major == 0x0 && num_ent < SMEM_MAX_PMIC_DEVICES)
	{
		if (format_minor < 0xB && num_ent < SMEM_V8_SMEM_MAX_PMIC_DEVICES)
		{
			board.pmic_info[num_ent].pmic_target &= 0xffffff00;
			board.pmic_info[num_ent].pmic_target |= rev;
		}
		else
		{
			if (num_ent < board.num_pmics)
			{
				board.pmic_info_array[num_ent].pmic_target &= 0xffffff00;
				board.pmic_info_array[num_ent].pmic_target |= rev;
			}
		}
	}
}

uint32_t board_pmic_target(uint8_t num_ent)
{
	if (format_major == 0x0 && num_ent < SMEM_MAX_PMIC_DEVICES)
	{
		if (format_minor < 0xB && num_ent < SMEM_V8_SMEM_MAX_PMIC_DEVICES)
		{
			return board.pmic_info[num_ent].pmic_target;
		}
		else
		{
			if (num_ent < board.num_pmics)
				return  board.pmic_info_array[num_ent].pmic_target;
		}
	}
	return 0;
}

uint32_t board_soc_version()
{
	return board.platform_version;
}

uint32_t board_get_ddr_subtype(void)
{
	ram_partition ptn_entry;
	unsigned int index;
	uint32_t ret = 0;
	uint32_t len = 0;
	unsigned ddr_size = 0;

	/* Make sure RAM partition table is initialized */
	ASSERT(smem_ram_ptable_init_v1());

	len = smem_get_ram_ptable_len();

	/* Calculating the size of the mem_info_ptr */
	for (index = 0 ; index < len; index++)
	{
		smem_get_ram_ptable_entry(&ptn_entry, index);

		if((ptn_entry.category == SDRAM) &&
			(ptn_entry.type == SYS_MEMORY))
		{
			ddr_size += ptn_entry.size;
		}
	}

	switch(ddr_size)
	{
	case DDR_512MB:
		ret = SUBTYPE_512MB;
	break;
	default:
		ret = 0;
	break;
	};

	return ret;
}

uint32_t board_hlos_subtype(void)
{
	return board.platform_hlos_subtype;
}

void board_update_boot_dev(uint32_t boot_dev)
{
	/* HLOS subtype
	 * bit no                        |31    20 | 19        16|15    13 |12      11 | 10          8 | 7     0|
	 * board.platform_hlos_subtype = |reserved | Boot device |Reserved | Panel     | DDR detection | subtype|
	 *                               |  bits   |             |  bits   | Detection |
	 */
	board.platform_hlos_subtype |= (boot_dev << 16);
}
