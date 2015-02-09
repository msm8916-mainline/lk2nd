/* Copyright (c) 2012-2014, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
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
*/

#include <debug.h>
#include <list.h>

#ifndef __DEVICE_TREE__
#define __DEVICE_TREE__

#define DEV_TREE_SUCCESS        0
#define DEV_TREE_MAGIC          0x54444351 /* "QCDT" */
#define DEV_TREE_MAGIC_LEN      4
#define DEV_TREE_VERSION_V1     1
#define DEV_TREE_VERSION_V2     2
#define DEV_TREE_VERSION_V3     3

#define DEV_TREE_HEADER_SIZE    12

#define DTB_MAGIC               0xedfe0dd0
#define DTB_OFFSET              0x2C

#define DTB_PAD_SIZE            1024

/*
 * For DTB V1: The DTB entries would be of the format
 * qcom,msm-id = <msm8974, CDP, rev_1>; (3 * sizeof(uint32_t))
 * For DTB V2: The DTB entries would be of the format
 * qcom,msm-id   = <msm8974, rev_1>;  (2 * sizeof(uint32_t))
 * qcom,board-id = <CDP, subtype_ID>; (2 * sizeof(uint32_t))
 * The macros below are defined based on these.
 */
#define DT_ENTRY_V1_SIZE        0xC
#define PLAT_ID_SIZE            0x8
#define BOARD_ID_SIZE           0x8
#define PMIC_ID_SIZE           0x8


struct dt_entry_v2
{
	uint32_t platform_id;
	uint32_t variant_id;
	uint32_t board_hw_subtype;
	uint32_t soc_rev;
	uint32_t offset;
	uint32_t size;
};

struct dt_entry
{
	uint32_t platform_id;
	uint32_t variant_id;
	uint32_t board_hw_subtype;
	uint32_t soc_rev;
	uint32_t pmic_rev[4];
	uint32_t offset;
	uint32_t size;
};

struct dt_table
{
	uint32_t magic;
	uint32_t version;
	uint32_t num_entries;
};

struct plat_id
{
	uint32_t platform_id;
	uint32_t soc_rev;
};

struct board_id
{
	uint32_t variant_id;
	uint32_t platform_subtype;
};

struct pmic_id
{
	uint32_t pmic_version[4];
};

struct dt_mem_node_info
{
	uint32_t offset;
	uint32_t mem_info_cnt;
	uint32_t addr_cell_size;
	uint32_t size_cell_size;
};

enum dt_entry_info
{
	DTB_FOUNDRY = 0,
	DTB_SOC,
	DTB_MAJOR_MINOR,
	DTB_PMIC0,
	DTB_PMIC1,
	DTB_PMIC2,
	DTB_PMIC3,
	DTB_PMIC_MODEL,
	DTB_PANEL_TYPE,
	DTB_BOOT_DEVICE,
};

enum dt_err_codes
{
	DT_OP_SUCCESS,
	DT_OP_FAILURE = -1,
};

typedef struct dt_entry_node {
	struct list_node node;
	struct dt_entry * dt_entry_m;
}dt_node;

int dev_tree_validate(struct dt_table *table, unsigned int page_size, uint32_t *dt_hdr_size);
int dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info);
int update_device_tree(void *fdt, const char *, void *, unsigned);
int dev_tree_add_mem_info(void *fdt, uint32_t offset, uint64_t size, uint64_t addr);
void *dev_tree_appended(void *kernel, uint32_t kernel_size, uint32_t dtb_offset, void *tags);
#endif
