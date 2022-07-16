/* Copyright (c) 2012-2014,2017-2020 The Linux Foundation. All rights reserved.
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
#include <bits.h>

#ifndef __DEVICE_TREE__
#define __DEVICE_TREE__

#include <boot.h>

#define DEV_TREE_SUCCESS        0
#define DEV_TREE_MAGIC          0x54444351 /* "QCDT" */
#define DEV_TREE_MAGIC_LEN      4
#define DEV_TREE_VERSION_V1     1
#define DEV_TREE_VERSION_V2     2
#define DEV_TREE_VERSION_V3     3

#define DEV_TREE_HEADER_SIZE    12
#define DEV_TREE_DT_ENTRY_SIZE_V3 40

#define DTB_MAGIC               0xedfe0dd0
#define DTB_OFFSET              0x2C

#define DTB_PAD_SIZE            1024
#define DTBO_TABLE_MAGIC        0xD7B7AB1E
#define DTBO_CUSTOM_MAX         4
#define PLATFORM_FOUNDRY_SHIFT  16
#define SOC_MASK                  (0xffff)
#define VARIANT_MASK              (0x000000ff)
#define VARIANT_MINOR_MASK        (0x0000ff00)
#define VARIANT_MAJOR_MASK        (0x00ff0000)
#define PMIC_MODEL_MASK           (0x000000ff)
#define PMIC_REV_MASK             (0xffffff00)
#define PMIC_SHIFT_IDX            (2)
#define PLATFORM_SUBTYPE_SHIFT_ID (0x18)
#define FOUNDRY_ID_MASK           (0x00ff0000)
#define MAX_SUPPORTED_DTBO_IMG_BUF (8388608)  /* 8MB   8 * 1024 * 1024 */
#define DTBO_IMG_BUF               (10485760) /* 10MB 10 * 1024 * 1024 */
#define MAX_SUPPORTED_VBMETA_IMG_BUF (65536)  /* 64 KB 64 * 1024 */
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

typedef enum {
	NONE_MATCH,
	PMIC_MATCH_BEST_REV_IDX0,
	PMIC_MATCH_EXACT_REV_IDX0,
	PMIC_MATCH_BEST_REV_IDX1,
	PMIC_MATCH_EXACT_REV_IDX1,
	PMIC_MATCH_BEST_REV_IDX2,
	PMIC_MATCH_EXACT_REV_IDX2,
	PMIC_MATCH_BEST_REV_IDX3,
	PMIC_MATCH_EXACT_REV_IDX3,
	VARIANT_MINOR_BEST_MATCH,
	VARIANT_MINOR_EXACT_MATCH,
	VARIANT_MAJOR_BEST_MATCH,
	VARIANT_MAJOR_EXACT_MATCH,
	VERSION_BEST_MATCH,
	VERSION_EXACT_MATCH,
	FOUNDRYID_DEFAULT_MATCH,
	FOUNDRYID_EXACT_MATCH,
	PMIC_MATCH_DEFAULT_MODEL_IDX0,
	PMIC_MATCH_EXACT_MODEL_IDX0,
	PMIC_MATCH_DEFAULT_MODEL_IDX1,
	PMIC_MATCH_EXACT_MODEL_IDX1,
	PMIC_MATCH_DEFAULT_MODEL_IDX2,
	PMIC_MATCH_EXACT_MODEL_IDX2,
	PMIC_MATCH_DEFAULT_MODEL_IDX3,
	PMIC_MATCH_EXACT_MODEL_IDX3,
	SUBTYPE_DEFAULT_MATCH,
	SUBTYPE_EXACT_MATCH,
	VARIANT_MATCH,
	SOC_MATCH,
	MAX_MATCH,
}dt_match_params;

#define TOTAL_MATCH_BITS 6
#define ALL_BITS_SET  (BIT (SOC_MATCH) | BIT (VARIANT_MATCH) | \
                       BIT (SUBTYPE_EXACT_MATCH) | BIT (FOUNDRYID_EXACT_MATCH) \
                       | BIT (PMIC_MATCH_EXACT_MODEL_IDX0) | \
                       BIT (PMIC_MATCH_EXACT_MODEL_IDX1))

typedef enum {
	PMIC_IDX0,
	PMIC_IDX1,
	PMIC_IDX2,
	PMIC_IDX3,
	MAX_PMIC_IDX,
}pmic_indexes;


typedef struct dt_info
{
	uint32_t dt_platform_id;
	uint32_t dt_soc_rev;
	uint32_t dt_foundry_id;
	uint32_t dt_variant_id;
	uint32_t dt_variant_major;
	uint32_t dt_variant_minor;
	uint32_t dt_platform_subtype;
	uint32_t dt_pmic_model[MAX_PMIC_IDX];
	uint32_t dt_pmic_rev[MAX_PMIC_IDX];
	uint32_t dt_match_val;
	void *dtb, *aligned_dtb;
	uint32_t dtb_size;
}dt_info;

typedef struct pmic_info
{
	uint32_t dt_pmic_model[MAX_PMIC_IDX];
	uint32_t dt_pmic_rev[MAX_PMIC_IDX];
	uint32_t dt_match_val;
}pmic_info;

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
	uint32_t idx;
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

struct dtbo_table_hdr {
	uint32_t magic;           //dtb table magic
	uint32_t total_size;       //Includes dt_table_hdr + all dt_table_entry and all dtb/dtbo
	uint32_t hdr_size;      //sizeof(dt_table_hdr)
	uint32_t dt_entry_size;     //sizeof(dt_table_entry)
	uint32_t dt_entry_count;    //number of dt_table_entry
	uint32_t dt_entry_offset;   //offset to the first dt_table_entry
	uint32_t page_size;        //flash pagesize we assume
	uint32_t reserved[1];     //must zeros
};

struct dtbo_table_entry {
	uint32_t dt_size;
	uint32_t dt_offset;     //offset from head of dt_table_hdr
	uint32_t id;           //optional, must zero if unused
	uint32_t revision;          //optional, must zero if unused
	uint32_t custom[DTBO_CUSTOM_MAX];    //optional, must zero if unused
};

typedef enum dtbo_error
{
	DTBO_ERROR = 0,
	DTBO_NOT_SUPPORTED = 1,
	DTBO_SUCCESS = 2
}dtbo_error;

dtbo_error load_validate_dtbo_image(void **dtbo_img, uint32_t *dtbo_img_size);
void get_recovery_dtbo_info(uint32_t *dtbo_size, void **dtbo_buf);
int dev_tree_validate(struct dt_table *table, unsigned int page_size, uint32_t *dt_hdr_size);
int dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info);
int update_device_tree(void *fdt, const char *, enum boot_type, void *, unsigned);
int dev_tree_add_mem_info(void *fdt, uint32_t offset, uint64_t size, uint64_t addr);
void *dev_tree_appended(void *kernel, uint32_t kernel_size, uint32_t dtb_offset, void *tags);
int get_dtbo_idx (void);
int get_dtb_idx (void);
#endif
