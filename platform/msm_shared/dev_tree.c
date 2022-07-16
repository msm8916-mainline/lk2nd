/* Copyright (c) 2012-2015,2017-2021 The Linux Foundation. All rights reserved.
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

#include <libfdt.h>
#include <dev_tree.h>
#include <lib/ptable.h>
#include <malloc.h>
#include <qpic_nand.h>
#include <stdlib.h>
#include <string.h>
#include <platform.h>
#include <board.h>
#include <list.h>
#include <kernel/thread.h>
#include <kernel/event.h>
#include <target.h>
#include <partial_goods.h>
#include <boot_device.h>
#include <platform.h>
#include <scm.h>
#include <partition_parser.h>
#if WITH_LIB_LIBUFDT
#include <libufdt_sysdeps.h>
#include <ufdt_overlay.h>
#endif
#include <boot_stats.h>
#include <verifiedboot.h>

#define NODE_PROPERTY_MAX_LEN   64
#define ADD_OF(a, b) (UINT_MAX - b > a) ? (a + b) : UINT_MAX
#define ADDR_ALIGNMENT 16
/** 512KB stack **/
#define DTBO_STACK_SIZE (524288)
#define MAX_DTBO_SZ 2097152

static bool  dtbo_needed = true;
static void *board_dtb = NULL;
static void *soc_dtb = NULL;
static void *final_dtb_hdr = NULL;
static event_t dtbo_event;

dtbo_error ret = DTBO_SUCCESS;

struct dt_entry_v1
{
	uint32_t platform_id;
	uint32_t variant_id;
	uint32_t soc_rev;
	uint32_t offset;
	uint32_t size;
};

#if ENABLE_BOOTDEVICE_MOUNT || DYNAMIC_PARTITION_SUPPORT
/* Look up table for fstab node */
struct fstab_node
{
        const char *parent_node;
        const char *node_prop;
        const char *device_path_id;
};

static struct fstab_node dynamic_fstab_table =
{
	"/firmware/android/fstab", "status", ""
};

static struct fstab_node fstab_table =
{
	"/firmware/android/fstab", "dev", "/soc/"
};
#endif

static struct dt_mem_node_info mem_node;
static int platform_dt_absolute_match(struct dt_entry *cur_dt_entry, struct dt_entry_node *dt_list);
static struct dt_entry *platform_dt_match_best(struct dt_entry_node *dt_list);
static int update_dtb_entry_node(struct dt_entry_node *dt_list, uint32_t dtb_info);
extern int target_is_emmc_boot(void);
extern uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset);
static int update_fstab_node(void *fdt);

/* TODO: This function needs to be moved to target layer to check violations
 * against all the other regions as well.
 */
extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);

static int dtbo_idx = INVALID_PTN;
int get_dtbo_idx (void)
{
   return dtbo_idx;
}

static int dtb_idx = INVALID_PTN;
int get_dtb_idx (void)
{
   return dtb_idx;
}

int fdt_check_header_ext(const void *fdt)
{
	uintptr_t fdt_start, fdt_end;
	fdt_start = (uintptr_t)fdt;
	if(fdt_start + fdt_totalsize(fdt) < fdt_start)
	{
		dprintf(CRITICAL,"Integer over in fdt header %s\t%d",__func__,__LINE__);
		return FDT_ERR_BADOFFSET;
	}
	fdt_end = fdt_start + fdt_totalsize(fdt);

	if (((uint64_t)fdt_start + (uint64_t)fdt_off_dt_struct(fdt) + (uint64_t)fdt_size_dt_struct(fdt)) > UINT_MAX)
		return FDT_ERR_BADOFFSET;

	if ((fdt_start + fdt_off_dt_struct(fdt) + fdt_size_dt_struct(fdt)) > fdt_end)
		return FDT_ERR_BADOFFSET;

	if (((uint64_t)fdt_start + (uint64_t)fdt_off_dt_strings(fdt) + (uint64_t)fdt_size_dt_strings(fdt)) > UINT_MAX)
		return FDT_ERR_BADOFFSET;

	if ((fdt_start + fdt_off_dt_strings(fdt) + fdt_size_dt_strings(fdt)) > fdt_end)
		return FDT_ERR_BADOFFSET;

	return 0;
}

/* Returns soc version if platform id and hardware id matches
   otherwise return 0xFFFFFFFF */
#define INVALID_SOC_REV_ID 0XFFFFFFFF

/* Add function to allocate dt entry list, used for recording
*  the entry which conform to platform_dt_absolute_match()
*/
static struct dt_entry_node *dt_entry_list_init(void)
{
	struct dt_entry_node *dt_node_member = NULL;

	dt_node_member = (struct dt_entry_node *)
		malloc(sizeof(struct dt_entry_node));

	ASSERT(dt_node_member);

	list_clear_node(&dt_node_member->node);
	dt_node_member->dt_entry_m = (struct dt_entry *)
			malloc(sizeof(struct dt_entry));
	ASSERT(dt_node_member->dt_entry_m);

	memset(dt_node_member->dt_entry_m ,0 ,sizeof(struct dt_entry));
	return dt_node_member;
}

/*
 * Function to validate dtbo image.
 * return: TRUE or FALSE.
 */
dtbo_error load_validate_dtbo_image(void **dtbo_buf, uint32_t *dtbo_image_sz)
{
	uint64_t dtbo_total_size = 0;
	static void *dtbo_image_buf = NULL;
	unsigned int dtbo_image_buf_size;
	unsigned int dtbo_partition_size;
	unsigned long long ptn, boot_img_sz;
	int index = INVALID_PTN;
	int page_size = mmc_page_size();
	struct dtbo_table_hdr *dtbo_table_header = NULL;
	dtbo_error ret = DTBO_SUCCESS;
	static bool dtbo_loaded = false;
	static unsigned long long ptn_size = 0;
	uint32_t recovery_dtbo_size = 0;
	void *recovery_appended_dtbo = NULL;

	/* If dtbo loaded skip loading */
	if (dtbo_loaded)
		goto out;

	get_recovery_dtbo_info(&recovery_dtbo_size, &recovery_appended_dtbo);

	/* Intialize dtbo for recovery header v1 */
	if (recovery_dtbo_size && recovery_appended_dtbo) {
		dtbo_image_buf = recovery_appended_dtbo;
		dtbo_partition_size = recovery_dtbo_size;
		ptn_size = recovery_dtbo_size;
	} else {
		index = partition_get_index("dtbo");


		/* Immediately return if dtbo is not supported */
		if (index == INVALID_PTN)
		{
			ret = DTBO_NOT_SUPPORTED;
			goto out;
		}

		ptn = partition_get_offset(index);
		if(!ptn)
		{
			dprintf(CRITICAL, "ERROR: dtbo parition failed to get offset. \n");
			ret = DTBO_ERROR;
			goto out;
		}

		ptn_size = partition_get_size(index);
		if (ptn_size > MAX_SUPPORTED_DTBO_IMG_BUF)
		{
			dprintf(CRITICAL, "ERROR: dtbo parition size is greater than supported.\n");
			ret = DTBO_ERROR;
			goto out;
		}

		/*
		Read dtbo image into scratch region after kernel image.
		dtbo_image_buf_size = total_scratch_region_size - boot_img_sz
		*/
		boot_img_sz = partition_get_size(partition_get_index("boot"));
		if (!boot_img_sz)
		{
			dprintf(CRITICAL, "ERROR: Unable to get boot partition size\n");
			ret = DTBO_NOT_SUPPORTED;
			goto out;
		}

		dtbo_image_buf_size = target_get_max_flash_size() - boot_img_sz;
		dtbo_partition_size = ptn_size + ADDR_ALIGNMENT;
		dtbo_partition_size = ROUND_TO_PAGE(dtbo_partition_size, (page_size - 1)); /* Maximum dtbo size possible */
		if (dtbo_partition_size == UINT_MAX ||
			dtbo_image_buf_size < dtbo_partition_size)
		{
			dprintf(CRITICAL, "ERROR: Invalid DTBO partition size\n");
			ret = DTBO_NOT_SUPPORTED;
			goto out;
		}

		mmc_set_lun(partition_get_lun(index));
		/* read dtbo at last 10MB of scratch */
		dtbo_image_buf = target_get_scratch_address() +
					(target_get_max_flash_size() - DTBO_IMG_BUF);
		dtbo_image_buf =
			(void *)ROUND_TO_PAGE((addr_t)dtbo_image_buf, (ADDR_ALIGNMENT-1));
		if(dtbo_image_buf == (void *)UINT_MAX)
		{
			dprintf(CRITICAL, "ERROR: Invalid DTBO image buf addr\n");
			ret = DTBO_NOT_SUPPORTED;
			goto out;
		}

		/* Add offset for salt buffer for verification */
		dtbo_image_buf += SALT_BUFF_OFFSET;
		/* Read dtbo partition with header */
		if (mmc_read(ptn, (uint32_t *)(dtbo_image_buf), dtbo_partition_size))
		{
			dprintf(CRITICAL, "ERROR: dtbo partition mmc read failure \n");
			ret = DTBO_ERROR;
			goto out;
		}
	}

	/* validate the dtbo image, before reading complete image */
	dtbo_table_header = (struct dtbo_table_hdr *)dtbo_image_buf;

	/*Check for dtbo magic*/
	if (fdt32_to_cpu(dtbo_table_header->magic) != DTBO_TABLE_MAGIC)
	{
		dprintf(CRITICAL, "dtbo header magic mismatch %x, with %x\n",
			fdt32_to_cpu(dtbo_table_header->magic), DTBO_TABLE_MAGIC);
		ret = DTBO_ERROR;
		goto out;
	}

	/*Check for dtbo image table size*/
	if (fdt32_to_cpu(dtbo_table_header->hdr_size) != sizeof(struct dtbo_table_hdr))
	{
		dprintf(CRITICAL, "dtbo table header size got corrupted\n");
		ret = DTBO_ERROR;
		goto out;
	}

	/*Check for dt entry size of dtbo image*/
	if (fdt32_to_cpu(dtbo_table_header->dt_entry_size) != sizeof(struct dtbo_table_entry))
	{
		dprintf(CRITICAL, "dtbo table dt entry size got corrupted\n");
		ret = DTBO_ERROR;
		goto out;
	}

	/*Total size of dtbo */
	dtbo_total_size = fdt32_to_cpu(dtbo_table_header->total_size);
	if (dtbo_total_size > dtbo_partition_size || dtbo_total_size == 0)
	{
		dprintf(CRITICAL, "dtbo table total size exceeded the dtbo buffer allocated\n");
		ret = DTBO_ERROR;
		goto out;
	}

	/* Total size of dtbo entries */
	dtbo_total_size = fdt32_to_cpu(dtbo_table_header->dt_entry_count) *
				fdt32_to_cpu(dtbo_table_header->dt_entry_size);
	if (dtbo_total_size > dtbo_partition_size || dtbo_total_size == 0)
	{
		dprintf(CRITICAL, "dtbo table total size exceeded the dtbo buffer allocated\n");
		ret = DTBO_ERROR;
		goto out;
	}

	/* Offset should be less than image size */
	if (fdt32_to_cpu(dtbo_table_header->dt_entry_offset) > dtbo_partition_size)
	{
		dprintf(CRITICAL, "dtbo offset exceeds the dtbo buffer allocated\n");
		ret = DTBO_ERROR;
		goto out;
	}

	/* Offset + total size is less than image size */
	if (dtbo_total_size + fdt32_to_cpu(dtbo_table_header->dt_entry_offset) > dtbo_partition_size)
	{
		dprintf(CRITICAL, "dtbo size exceeded the dtbo buffer allocated\n");
		ret = DTBO_ERROR;
	}

out:
	if (ret == DTBO_SUCCESS)
	{
		*dtbo_buf = dtbo_image_buf;
		*dtbo_image_sz = (uint32_t)ptn_size;
		dtbo_loaded = true;
	}
	else
	{
		*dtbo_buf = NULL;
		*dtbo_image_sz = 0;
	}
	return ret;
}

static bool check_all_bits_set(uint32_t matchdt_value)
{
	return (matchdt_value & ALL_BITS_SET) == (ALL_BITS_SET);
}

/* Dt selection table for quick reference
  | SNO | Dt Property   | CDT Property    | Exact | Best | Default |
  |-----+---------------+-----------------+-------+------+---------+
  |     | qcom, msm-id  |                 |       |      |         |
  |     |               | PlatformId      | Y     | N    | N       |
  |     |               | SocRev          | N     | Y    | N       |
  |     |               | FoundryId       | Y     | N    | 0       |
  |     | qcom,board-id |                 |       |      |         |
  |     |               | VariantId       | Y     | N    | N       |
  |     |               | VariantMajor    | N     | Y    | N       |
  |     |               | VariantMinor    | N     | Y    | N       |
  |     |               | PlatformSubtype | Y     | N    | 0       |
  |     | qcom,pmic-id  |                 |       |      |         |
  |     |               | PmicModelId     | Y     | N    | 0       |
  |     |               | PmicMetalRev    | N     | Y    | N       |
  |     |               | PmicLayerRev    | N     | Y    | N       |
  |     |               | PmicVariantRev  | N     | Y    | N       |
*/

static boolean dtb_read_find_match(dt_info *current_dtb_info, dt_info *best_dtb_info, uint32_t exact_match)
{
	int board_id_len;
	int platform_id_len = 0;
	int pmic_id_len;
	int root_offset = 0;
	void *dtb = current_dtb_info->aligned_dtb;
	uint32_t idx;
	const char *platform_prop = NULL;
	const char *board_prop = NULL;
	const char *pmic_prop = NULL;
	boolean find_best_match = false;

	current_dtb_info->dt_match_val = 0;
	root_offset = fdt_path_offset(dtb, "/");
	if (root_offset < 0) {
		dprintf(CRITICAL, "ERROR: Unable to locate root node\n");
		return false;
	}

	/* Get the msm-id prop from DTB and find best match */
	platform_prop = (const char *)fdt_getprop(dtb, root_offset, "qcom,msm-id", &platform_id_len);
	if (platform_prop && (platform_id_len > 0) && (!(platform_id_len % PLAT_ID_SIZE))) {
		/*Compare msm-id of the dtb vs board*/
		current_dtb_info->dt_platform_id = fdt32_to_cpu(((struct plat_id *)platform_prop)->platform_id);
		dprintf(SPEW, "Board SOC ID = %x | DT SOC ID = %x\n", (board_platform_id() & SOC_MASK),
								(current_dtb_info->dt_platform_id & SOC_MASK));
		if ((board_platform_id() & SOC_MASK) == (current_dtb_info->dt_platform_id & SOC_MASK)) {
			current_dtb_info->dt_match_val |= BIT(SOC_MATCH);
		} else {
			dprintf(SPEW, "qcom,msm-id does not match\n");
			 /* If soc doesn't match, don't select dtb */
			current_dtb_info->dt_match_val = BIT(NONE_MATCH);
			goto cleanup;
		}
		/*Compare soc rev of the dtb vs board*/
		current_dtb_info->dt_soc_rev = fdt32_to_cpu(((struct plat_id *)platform_prop)->soc_rev);
		dprintf(SPEW, "Board SOC Rev = %x | DT SOC Rev =%x\n",  board_soc_version(),
									current_dtb_info->dt_soc_rev);
		if (current_dtb_info->dt_soc_rev ==  board_soc_version()) {
			current_dtb_info->dt_match_val |= BIT(VERSION_EXACT_MATCH);
		} else if (current_dtb_info->dt_soc_rev <  board_soc_version()) {
			current_dtb_info->dt_match_val |= BIT(VERSION_BEST_MATCH);
		} else if (current_dtb_info->dt_soc_rev) {
			dprintf(SPEW, "SOC version does not match\n");
		}
		/*Compare Foundry Id of the dtb vs Board*/
		current_dtb_info->dt_foundry_id = fdt32_to_cpu(((struct plat_id *)platform_prop)->platform_id) & FOUNDRY_ID_MASK;
		dprintf (SPEW, "Board Foundry id = %x | DT Foundry id = %x\n", (board_foundry_id() << PLATFORM_FOUNDRY_SHIFT), current_dtb_info->dt_foundry_id);
		if (current_dtb_info->dt_foundry_id == (board_foundry_id() << PLATFORM_FOUNDRY_SHIFT)) {
			current_dtb_info->dt_match_val |= BIT (FOUNDRYID_EXACT_MATCH);
		} else if (current_dtb_info->dt_foundry_id == 0 ){
			current_dtb_info->dt_match_val |= BIT (FOUNDRYID_DEFAULT_MATCH);
		} else {
			dprintf(SPEW, "soc foundry does not match\n");
			/* If soc doesn't match, don't select dtb */
			current_dtb_info->dt_match_val = BIT(NONE_MATCH);
			goto cleanup;
		}
	} else {
		dprintf(SPEW, "qcom,msm-id does not exist (or) is (%d) not a multiple of (%d)\n", platform_id_len, PLAT_ID_SIZE);
	}

	/* Get the properties like variant id, subtype from DTB and compare the DTB vs Board */
	board_prop = (const char *)fdt_getprop(dtb, root_offset, "qcom,board-id", &board_id_len);
	if (board_prop && (board_id_len > 0) && (!(board_id_len % BOARD_ID_SIZE))) {
		current_dtb_info->dt_variant_id = fdt32_to_cpu(((struct board_id *)board_prop)->variant_id);
		current_dtb_info->dt_platform_subtype = fdt32_to_cpu(((struct board_id *)board_prop)->platform_subtype);
		if (current_dtb_info->dt_platform_subtype == 0)
			current_dtb_info->dt_platform_subtype = fdt32_to_cpu(((struct board_id *)board_prop)->variant_id) >> PLATFORM_SUBTYPE_SHIFT_ID;

		dprintf(SPEW, "Board variant id = %x | DT variant id = %x\n",board_hardware_id(), current_dtb_info->dt_variant_id);

		current_dtb_info->dt_variant_major = current_dtb_info->dt_variant_id & VARIANT_MAJOR_MASK;
		current_dtb_info->dt_variant_minor = current_dtb_info->dt_variant_id & VARIANT_MINOR_MASK;
		current_dtb_info->dt_variant_id = current_dtb_info->dt_variant_id & VARIANT_MASK;

		if (current_dtb_info->dt_variant_id == board_hardware_id()) {
			current_dtb_info->dt_match_val |= BIT(VARIANT_MATCH);
		} else if (current_dtb_info->dt_variant_id) {
			dprintf(SPEW, "qcom,board-id doesnot match\n");
			/* If board variant doesn't match, don't select dtb */
			current_dtb_info->dt_match_val = BIT(NONE_MATCH);
			goto cleanup;
		}

		if (current_dtb_info->dt_variant_major == (board_target_id() & VARIANT_MAJOR_MASK)) {
			current_dtb_info->dt_match_val |= BIT(VARIANT_MAJOR_EXACT_MATCH);
		} else if (current_dtb_info->dt_variant_major < (board_target_id() & VARIANT_MAJOR_MASK)) {
			current_dtb_info->dt_match_val |= BIT(VARIANT_MAJOR_BEST_MATCH);
		} else if (current_dtb_info->dt_variant_major) {
			dprintf(SPEW, "qcom,board-id major version doesnot match\n");
		}

		if (current_dtb_info->dt_variant_minor == (board_target_id() & VARIANT_MINOR_MASK)) {
			current_dtb_info->dt_match_val |= BIT(VARIANT_MINOR_EXACT_MATCH);
		} else if (current_dtb_info->dt_variant_minor < (board_target_id() & VARIANT_MINOR_MASK)) {
			current_dtb_info->dt_match_val |= BIT(VARIANT_MINOR_BEST_MATCH);
		} else if (current_dtb_info->dt_variant_minor) {
			dprintf(SPEW, "qcom,board-id minor version doesnot match\n");
		}

		dprintf(SPEW, "Board platform subtype = %x | DT platform subtype = %x\n",
					board_hardware_subtype(), current_dtb_info->dt_platform_subtype);
		if (current_dtb_info->dt_platform_subtype == board_hardware_subtype()) {
			current_dtb_info->dt_match_val |= BIT(SUBTYPE_EXACT_MATCH);
		} else if (current_dtb_info->dt_platform_subtype == 0) {
			current_dtb_info->dt_match_val |= BIT(SUBTYPE_DEFAULT_MATCH);
		} else if (current_dtb_info->dt_platform_subtype) {
			dprintf(SPEW, "subtype id doesnot match\n");
			/* If board platform doesn't match, don't select dtb */
			current_dtb_info->dt_match_val = BIT(NONE_MATCH);
			goto cleanup;
		}
	} else {
		dprintf(SPEW, "qcom,board-id does not exist (or)(%d) is not a multiple of (%d)\n",
									board_id_len, BOARD_ID_SIZE);
	}

	/* Get the pmic property from DTB then compare the DTB vs Board */
	pmic_prop = (const char *)fdt_getprop(dtb, root_offset, "qcom,pmic-id", &pmic_id_len);
	if (pmic_prop && (pmic_id_len > 0) && (!(pmic_id_len % sizeof(struct pmic_id))))
	{
		pmic_info curr_pmic_info;
		pmic_info best_pmic_info;
		unsigned int pmic_entry_indx;
		unsigned int pmic_entries_count;

		/* Find all pmic entries count */
		pmic_entries_count = pmic_id_len/sizeof(struct pmic_id);

		memset(&best_pmic_info, 0, sizeof(pmic_info));
		for (pmic_entry_indx = 0; pmic_entry_indx < pmic_entries_count; pmic_entry_indx++)
		{
			memset(&curr_pmic_info, 0, sizeof(pmic_info));

			/* Read all pmic info */
			/* Compare with board pmic */
			for (idx = 0; idx < MAX_PMIC_IDX; idx++)
			{
				curr_pmic_info.dt_pmic_model[idx] =
					fdt32_to_cpu (((struct pmic_id *)pmic_prop)->pmic_version[idx]);
				dprintf(SPEW, "pmic_data[%u]:%x\n", idx, curr_pmic_info.dt_pmic_model[idx]);
				curr_pmic_info.dt_pmic_rev[idx] =
					curr_pmic_info.dt_pmic_model[idx] & PMIC_REV_MASK;
				curr_pmic_info.dt_pmic_model[idx] =
					curr_pmic_info.dt_pmic_model[idx] & PMIC_MODEL_MASK;

				/* Compare with board pmic information & Update bit mask */
				if (curr_pmic_info.dt_pmic_model[idx] == (board_pmic_target(idx) & PMIC_MODEL_MASK))
					curr_pmic_info.dt_match_val |= BIT (PMIC_MATCH_EXACT_MODEL_IDX0 + idx * PMIC_SHIFT_IDX);
				else if (curr_pmic_info.dt_pmic_model[idx] == 0)
					curr_pmic_info.dt_match_val |= BIT (PMIC_MATCH_DEFAULT_MODEL_IDX0 + idx * PMIC_SHIFT_IDX);
				else
				{
					curr_pmic_info.dt_match_val |= BIT (NONE_MATCH);
					dprintf(SPEW, "PMIC Model doesn't match\n");
					break; /* go to next pmic entry */
				}

				if (curr_pmic_info.dt_pmic_rev[idx] == (board_pmic_target(idx) & PMIC_REV_MASK))
					curr_pmic_info.dt_match_val |= BIT(PMIC_MATCH_EXACT_REV_IDX0 + idx * PMIC_SHIFT_IDX);
				else if (curr_pmic_info.dt_pmic_rev[idx] < (board_pmic_target(idx) & PMIC_REV_MASK))
					curr_pmic_info.dt_match_val |= BIT(PMIC_MATCH_BEST_REV_IDX0 + idx * PMIC_SHIFT_IDX);
				else {
					dprintf(SPEW, "PMIC revision doesn't match\n");
					break; /* go to next pmic entry */
				}
			}

			dprintf(SPEW, "Bestpmicinfo.dtmatchval : %x | cur_pmic_info.dtmatchval: %x\n",
				best_pmic_info.dt_match_val, curr_pmic_info.dt_match_val);

			/* Update best pmic info, if required */
			if(best_pmic_info.dt_match_val < curr_pmic_info.dt_match_val)
				memscpy(&best_pmic_info, sizeof(pmic_info), &curr_pmic_info, sizeof(pmic_info));
			else if (best_pmic_info.dt_match_val == curr_pmic_info.dt_match_val)
			{
				if (best_pmic_info.dt_pmic_rev[PMIC_IDX0] < curr_pmic_info.dt_pmic_rev[PMIC_IDX0])
					memscpy(&best_pmic_info, sizeof(pmic_info), &curr_pmic_info, sizeof(pmic_info));
				else if (best_pmic_info.dt_pmic_rev[PMIC_IDX1] < curr_pmic_info.dt_pmic_rev[PMIC_IDX1])
					memscpy(&best_pmic_info, sizeof(pmic_info), &curr_pmic_info, sizeof(pmic_info));
				else if (best_pmic_info.dt_pmic_rev[PMIC_IDX2] < curr_pmic_info.dt_pmic_rev[PMIC_IDX2])
					memscpy(&best_pmic_info, sizeof(pmic_info), &curr_pmic_info, sizeof(pmic_info));
				else if (best_pmic_info.dt_pmic_rev[PMIC_IDX3] < curr_pmic_info.dt_pmic_rev[PMIC_IDX3])
					memscpy(&best_pmic_info, sizeof(pmic_info), &curr_pmic_info, sizeof(pmic_info));
			}

			/* Increment to next pmic entry */
			pmic_prop += sizeof (struct pmic_id);
		}

		dprintf(SPEW, "Best pmic info 0x%0x/0x%x/0x%x/0x%0x for current dt\n",
					best_pmic_info.dt_pmic_model[PMIC_IDX0],
					best_pmic_info.dt_pmic_model[PMIC_IDX1],
					best_pmic_info.dt_pmic_model[PMIC_IDX2],
					best_pmic_info.dt_pmic_model[PMIC_IDX3]);

		current_dtb_info->dt_match_val |= best_pmic_info.dt_match_val;
		current_dtb_info->dt_pmic_rev[PMIC_IDX0] = best_pmic_info.dt_pmic_rev[PMIC_IDX0];
		current_dtb_info->dt_pmic_model[PMIC_IDX0] = best_pmic_info.dt_pmic_model[PMIC_IDX0];
		current_dtb_info->dt_pmic_rev[PMIC_IDX1] = best_pmic_info.dt_pmic_rev[PMIC_IDX1];
		current_dtb_info->dt_pmic_model[PMIC_IDX1] = best_pmic_info.dt_pmic_model[PMIC_IDX1];
		current_dtb_info->dt_pmic_rev[PMIC_IDX2] = best_pmic_info.dt_pmic_rev[PMIC_IDX2];
		current_dtb_info->dt_pmic_model[PMIC_IDX2] = best_pmic_info.dt_pmic_model[PMIC_IDX2];
		current_dtb_info->dt_pmic_rev[PMIC_IDX3] = best_pmic_info.dt_pmic_rev[PMIC_IDX3];
		current_dtb_info->dt_pmic_model[PMIC_IDX3] = best_pmic_info.dt_pmic_model[PMIC_IDX3];
	}
	else
	{
		dprintf(SPEW, "qcom,pmic-id does not exit (or) is (%d) not a multiple of (%d)\n",
										pmic_id_len, PMIC_ID_SIZE);
	}

cleanup:
	if (current_dtb_info->dt_match_val & BIT(exact_match)) {
		if (best_dtb_info->dt_match_val < current_dtb_info->dt_match_val) {
			memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			find_best_match = true;
		} else if (best_dtb_info->dt_match_val == current_dtb_info->dt_match_val) {
			find_best_match = true;
			if (best_dtb_info->dt_soc_rev < current_dtb_info->dt_soc_rev)
				memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			else if (best_dtb_info->dt_variant_major < current_dtb_info->dt_variant_major)
				memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			else if (best_dtb_info->dt_variant_minor < current_dtb_info->dt_variant_minor)
				memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			else if (best_dtb_info->dt_pmic_rev[0] < current_dtb_info->dt_pmic_rev[0])
				memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			else if (best_dtb_info->dt_pmic_rev[1] < current_dtb_info->dt_pmic_rev[1])
				memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			else if (best_dtb_info->dt_pmic_rev[2] < current_dtb_info->dt_pmic_rev[2])
				memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			else if (best_dtb_info->dt_pmic_rev[3] < current_dtb_info->dt_pmic_rev[3])
				memscpy(best_dtb_info, sizeof(dt_info), current_dtb_info, sizeof(dt_info));
			else
				find_best_match = false;
		}
	}

	return find_best_match;
}

void *get_soc_dtb(void *kernel, void *tags, uint32_t kernel_size, uint32_t dtb_offset)
{
	uintptr_t kernel_end_offset = (uintptr_t)kernel + kernel_size;
	void *dtb = NULL;
	struct fdt_header dtb_header __ALIGNED(8);
	uint32_t dtb_size = 0;
	dt_info cur_dtb_info = {0};
	dt_info best_dtb_info = {0};
	uint32_t dtb_cnt = 0;
	boolean find_best_dtb = false;

	if (!dtb_offset){
		dprintf(CRITICAL, "DTB offset is NULL\n");
		return NULL;
	}
	if (((uintptr_t)kernel + (uintptr_t)dtb_offset) < (uintptr_t)kernel) {
		return NULL;
	}
	dtb = kernel + dtb_offset;
	while (((uintptr_t)dtb + sizeof(struct fdt_header)) < (uintptr_t)kernel_end_offset) {
		/* the DTB could be unaligned, so extract the header,
		 * and operate on it separately */
		memscpy(&dtb_header, sizeof(struct fdt_header), dtb, sizeof(struct fdt_header));
		dtb_size = fdt_totalsize((const void *)&dtb_header);
		if (fdt_check_header((const void *)&dtb_header) != 0 ||
		fdt_check_header_ext((void *)&dtb_header) != 0 ||
		((uintptr_t)dtb + dtb_size < (uintptr_t)dtb) ||
		((uintptr_t)dtb + dtb_size > (uintptr_t)kernel_end_offset))
		break;

		cur_dtb_info.dtb = dtb;
		cur_dtb_info.aligned_dtb = dtb;
		cur_dtb_info.dtb_size = dtb_size;

		/* Need to copy DTB if not aligned properly :/ */
		if ((uintptr_t)dtb & 7) {
			if (dtb_size > MAX_DTBO_SZ) {
				dprintf(CRITICAL, "SoC DTB too large: %d\n", dtb_size);
				return NULL;
			}
			if (best_dtb_info.aligned_dtb == tags)
				best_dtb_info.aligned_dtb = NULL;
			memcpy(tags, dtb, dtb_size);
			cur_dtb_info.aligned_dtb = tags;
		}

		find_best_dtb = dtb_read_find_match(&cur_dtb_info, &best_dtb_info, SOC_MATCH);
		if (cur_dtb_info.dt_match_val) {
			if (cur_dtb_info.dt_match_val & BIT(SOC_MATCH)) {
				if (check_all_bits_set(cur_dtb_info.dt_match_val)) {
					dprintf(CRITICAL, "Exact DTB match found. dtbo search is not required\n");
					dtbo_needed = false;
				}
			}
		}

		dprintf(SPEW, "dtb count = %u local_soc_dt_match val = %x\n", dtb_cnt, best_dtb_info.dt_match_val);
		dtb += dtb_size;

		if (find_best_dtb) {
			dtb_idx = dtb_cnt;
		}
		dtb_cnt++;

	}
	if (!best_dtb_info.dtb) {
		dprintf(CRITICAL, "No match found for soc dtb type\n");
		return NULL;
	}
	/* Need to copy DTB if not aligned properly :/ */
	dtb = best_dtb_info.aligned_dtb;
	if (!dtb) {
		memcpy(tags, best_dtb_info.dtb, best_dtb_info.dtb_size);
		dtb = tags;
	}
	return dtb;
}

void *get_board_dtb(void *dtbo_image_buf, uint32_t dtbo_image_sz)
{
	struct dtbo_table_hdr *dtbo_table_header = dtbo_image_buf;
	struct dtbo_table_entry *dtb_table_entry = NULL;
	uint32_t dtbo_count = 0;
	void *board_dtb = NULL;
	uint32_t dtbo_table_entries_count = 0;
	uint32_t first_dtbo_table_entry_offset = 0;
	struct fdt_header dtb_header __ALIGNED(8);
	uint32_t dtb_size = 0;
	dt_info cur_dtb_info = {0};
	dt_info best_dtb_info = {0};
	boolean find_best_dtb = false;
	void *tmp; uint32_t max_tmp_size;

	if (!dtbo_image_buf) {
		dprintf(CRITICAL, "dtbo image buffer is NULL\n");
		return NULL;
	}

	/* Temporary copy DTB after DTBO for alignment */
	tmp = (void*)ROUND_TO_PAGE((addr_t)dtbo_image_buf + dtbo_image_sz, (ADDR_ALIGNMENT-1));
	max_tmp_size = target_get_scratch_address() + target_get_max_flash_size() - tmp;

	first_dtbo_table_entry_offset = fdt32_to_cpu(dtbo_table_header->dt_entry_offset);
	if ((uintptr_t)dtbo_image_buf > ((uintptr_t)dtbo_image_buf + (uintptr_t)first_dtbo_table_entry_offset))
	{
		dprintf(CRITICAL, "dtbo table entry offset is invalid\n");
		return NULL;
	}
	dtb_table_entry = (struct dtbo_table_entry *)(dtbo_image_buf + first_dtbo_table_entry_offset);
	dtbo_table_entries_count = fdt32_to_cpu(dtbo_table_header->dt_entry_count);
	for (dtbo_count = 0; dtbo_count < dtbo_table_entries_count; dtbo_count++) {
		board_dtb = dtbo_image_buf + fdt32_to_cpu(dtb_table_entry->dt_offset);
		/* The DTB could be unaligned, so extract the header,
                 * and operate on it separately */
		memscpy(&dtb_header, sizeof(struct fdt_header), board_dtb, sizeof(struct fdt_header));
		dtb_size = fdt_totalsize((const void *)&dtb_header);
		if (fdt_check_header((const void *)&dtb_header) != 0 ||
			fdt_check_header_ext((void *)&dtb_header) != 0 ||
			((uintptr_t)board_dtb + dtb_size < (uintptr_t)board_dtb)) {
			dprintf(CRITICAL, "No valid board dtb found\n");
	                break;
		}
		dprintf(SPEW, "Valid board dtb is found\n");
		cur_dtb_info.dtb = board_dtb;
		cur_dtb_info.aligned_dtb = board_dtb;
		cur_dtb_info.dtb_size = dtb_size;

		/* Need to copy DTB if not aligned properly :/ */
		if ((uintptr_t)board_dtb & 7) {
			if (dtb_size > max_tmp_size) {
				dprintf(CRITICAL, "Board DTB too large: %d\n", dtb_size);
				return NULL;
			}
			if (best_dtb_info.aligned_dtb == tmp)
				best_dtb_info.aligned_dtb = NULL;
			memcpy(tmp, board_dtb, dtb_size);
			cur_dtb_info.aligned_dtb = tmp;
		}

		find_best_dtb = dtb_read_find_match(&cur_dtb_info, &best_dtb_info, VARIANT_MATCH);
		dprintf(SPEW, "dtbo count = %u local_board_dt_match =%x\n",dtbo_count, cur_dtb_info.dt_match_val);
		dtb_table_entry++;

		if (find_best_dtb) {
			dtbo_idx = dtbo_count;
		}
	}
	if (!best_dtb_info.dtb) {
		dprintf(CRITICAL, "Unable to find the board dtb\n");
		return NULL;
	}
	/* Need to copy DTB if not aligned properly :/ */
	board_dtb = best_dtb_info.aligned_dtb;
	if (!board_dtb) {
		memcpy(tmp, best_dtb_info.dtb, best_dtb_info.dtb_size);
		board_dtb = tmp;
	}
	return board_dtb;
}

static void insert_dt_entry_in_queue(struct dt_entry_node *dt_list, struct dt_entry_node *dt_node_member)
{
	list_add_tail(&dt_list->node, &dt_node_member->node);
}

static void dt_entry_list_delete(struct dt_entry_node *dt_node_member)
{
	if (list_in_list(&dt_node_member->node)) {
			list_delete(&dt_node_member->node);
			free(dt_node_member->dt_entry_m);
			free(dt_node_member);
	}
}

static int dev_tree_compatible(void *dtb, void *real_dtb, uint32_t dtb_size,
			       struct dt_entry_node *dtb_list)
{
	int root_offset;
	const void *prop = NULL;
	const char *plat_prop = NULL;
	const char *board_prop = NULL;
	const char *pmic_prop = NULL;
	char *model = NULL;
	struct dt_entry *cur_dt_entry;
	struct dt_entry *dt_entry_array = NULL;
	struct board_id *board_data = NULL;
	struct plat_id *platform_data = NULL;
	struct pmic_id *pmic_data = NULL;
	int len;
	int len_board_id;
	int len_plat_id;
	int min_plat_id_len = 0;
	int len_pmic_id;
	uint32_t dtb_ver;
	uint32_t num_entries = 0;
	uint32_t i, j, k, n;
	uint32_t msm_data_count;
	uint32_t board_data_count;
	uint32_t pmic_data_count;
	uint32_t dtb_count = 0;;

	root_offset = fdt_path_offset(dtb, "/");
	if (root_offset < 0)
		return false;

	prop = fdt_getprop(dtb, root_offset, "model", &len);
	if (prop && len > 0) {
		model = (char *) malloc(sizeof(char) * len);
		ASSERT(model);
		strlcpy(model, prop, len);
	} else {
		dprintf(INFO, "model does not exist in device tree\n");
	}
	/* Find the pmic-id prop from DTB , if pmic-id is present then
	* the DTB is version 3, otherwise find the board-id prop from DTB ,
	* if board-id is present then the DTB is version 2 */
	pmic_prop = (const char *)fdt_getprop(dtb, root_offset, "qcom,pmic-id", &len_pmic_id);
	board_prop = (const char *)fdt_getprop(dtb, root_offset, "qcom,board-id", &len_board_id);
	if (pmic_prop && (len_pmic_id > 0) && board_prop && (len_board_id > 0)) {
		if ((len_pmic_id % PMIC_ID_SIZE) || (len_board_id % BOARD_ID_SIZE))
		{
			dprintf(CRITICAL, "qcom,pmic-id(%d) or qcom,board-id(%d) in device tree is not a multiple of (%d %d)\n",
				len_pmic_id, len_board_id, PMIC_ID_SIZE, BOARD_ID_SIZE);
			return false;
		}
		dtb_ver = DEV_TREE_VERSION_V3;
		min_plat_id_len = PLAT_ID_SIZE;
	} else if (board_prop && len_board_id > 0) {
		if (len_board_id % BOARD_ID_SIZE)
		{
			dprintf(CRITICAL, "qcom,board-id in device tree is (%d) not a multiple of (%d)\n",
				len_board_id, BOARD_ID_SIZE);
			return false;
		}
		dtb_ver = DEV_TREE_VERSION_V2;
		min_plat_id_len = PLAT_ID_SIZE;
	} else {
		dtb_ver = DEV_TREE_VERSION_V1;
		min_plat_id_len = DT_ENTRY_V1_SIZE;
	}

	/* Get the msm-id prop from DTB */
	plat_prop = (const char *)fdt_getprop(dtb, root_offset, "qcom,msm-id", &len_plat_id);
	if (!plat_prop || len_plat_id <= 0) {
		dprintf(INFO, "qcom,msm-id entry not found\n");
		return false;
	} else if (len_plat_id % min_plat_id_len) {
		dprintf(INFO, "qcom,msm-id in device tree is (%d) not a multiple of (%d)\n",
			len_plat_id, min_plat_id_len);
		return false;
	}

	/*
	 * If DTB version is '1' look for <x y z> pair in the DTB
	 * x: platform_id
	 * y: variant_id
	 * z: SOC rev
	 */
	if (dtb_ver == DEV_TREE_VERSION_V1) {
		cur_dt_entry = (struct dt_entry *)
				malloc(sizeof(struct dt_entry));

		if (!cur_dt_entry) {
			dprintf(CRITICAL, "Out of memory\n");
			return false;
		}
		memset(cur_dt_entry, 0, sizeof(struct dt_entry));

		while (len_plat_id) {
			cur_dt_entry->platform_id = fdt32_to_cpu(((const struct dt_entry_v1 *)plat_prop)->platform_id);
			cur_dt_entry->variant_id = fdt32_to_cpu(((const struct dt_entry_v1 *)plat_prop)->variant_id);
			cur_dt_entry->soc_rev = fdt32_to_cpu(((const struct dt_entry_v1 *)plat_prop)->soc_rev);
			cur_dt_entry->board_hw_subtype =
				fdt32_to_cpu(((const struct dt_entry_v1 *)plat_prop)->variant_id) >> 0x18;
			cur_dt_entry->pmic_rev[0] = board_pmic_target(0);
			cur_dt_entry->pmic_rev[1] = board_pmic_target(1);
			cur_dt_entry->pmic_rev[2] = board_pmic_target(2);
			cur_dt_entry->pmic_rev[3] = board_pmic_target(3);
			cur_dt_entry->offset = (uint32_t)real_dtb;
			cur_dt_entry->size = dtb_size;

			dprintf(SPEW, "Found an appended flattened device tree (%s - %u %u 0x%x)\n",
				model ? model : "unknown",
				cur_dt_entry->platform_id, cur_dt_entry->variant_id, cur_dt_entry->soc_rev);

			if (platform_dt_absolute_match(cur_dt_entry, dtb_list)) {
				dprintf(SPEW, "Device tree exact match the board: <%u %u 0x%x> != <%u %u 0x%x>\n",
					cur_dt_entry->platform_id,
					cur_dt_entry->variant_id,
					cur_dt_entry->soc_rev,
					board_platform_id(),
					board_hardware_id(),
					board_soc_version());

			} else {
				dprintf(SPEW, "Device tree's msm_id doesn't match the board: <%u %u 0x%x> != <%u %u 0x%x>\n",
					cur_dt_entry->platform_id,
					cur_dt_entry->variant_id,
					cur_dt_entry->soc_rev,
					board_platform_id(),
					board_hardware_id(),
					board_soc_version());
				plat_prop += DT_ENTRY_V1_SIZE;
				len_plat_id -= DT_ENTRY_V1_SIZE;
				continue;
			}
		}
		free(cur_dt_entry);

	}
	/*
	 * If DTB Version is '3' then we have split DTB with board & msm data & pmic
	 * populated saperately in board-id & msm-id & pmic-id prop respectively.
	 * Extract the data & prepare a look up table
	 */
	else if (dtb_ver == DEV_TREE_VERSION_V2 || dtb_ver == DEV_TREE_VERSION_V3) {
		board_data_count = (len_board_id / BOARD_ID_SIZE);
		msm_data_count = (len_plat_id / PLAT_ID_SIZE);
		/* If dtb version is v2.0, the pmic_data_count will be <= 0 */
		pmic_data_count = (len_pmic_id / PMIC_ID_SIZE);

		/* If we are using dtb v3.0, then we have split board, msm & pmic data in the DTB
		*  If we are using dtb v2.0, then we have split board & msmdata in the DTB
		*/
		board_data = (struct board_id *) malloc(sizeof(struct board_id) * (len_board_id / BOARD_ID_SIZE));
		ASSERT(board_data);
		platform_data = (struct plat_id *) malloc(sizeof(struct plat_id) * (len_plat_id / PLAT_ID_SIZE));
		ASSERT(platform_data);
		if (dtb_ver == DEV_TREE_VERSION_V3) {
			pmic_data = (struct pmic_id *) malloc(sizeof(struct pmic_id) * (len_pmic_id / PMIC_ID_SIZE));
			ASSERT(pmic_data);
		}
		i = 0;

		/* Extract board data from DTB */
		for(i = 0 ; i < board_data_count; i++) {
			board_data[i].variant_id = fdt32_to_cpu(((struct board_id *)board_prop)->variant_id);
			board_data[i].platform_subtype = fdt32_to_cpu(((struct board_id *)board_prop)->platform_subtype);
			/* For V2/V3 version of DTBs we have platform version field as part
			 * of variant ID, in such case the subtype will be mentioned as 0x0
			 * As the qcom, board-id = <0xSSPMPmPH, 0x0>
			 * SS -- Subtype
			 * PM -- Platform major version
			 * Pm -- Platform minor version
			 * PH -- Platform hardware CDP/MTP
			 * In such case to make it compatible with LK algorithm move the subtype
			 * from variant_id to subtype field
			 */
			if (board_data[i].platform_subtype == 0)
				board_data[i].platform_subtype =
					fdt32_to_cpu(((struct board_id *)board_prop)->variant_id) >> 0x18;

			len_board_id -= sizeof(struct board_id);
			board_prop += sizeof(struct board_id);
		}

		/* Extract platform data from DTB */
		for(i = 0 ; i < msm_data_count; i++) {
			platform_data[i].platform_id = fdt32_to_cpu(((struct plat_id *)plat_prop)->platform_id);
			platform_data[i].soc_rev = fdt32_to_cpu(((struct plat_id *)plat_prop)->soc_rev);
			len_plat_id -= sizeof(struct plat_id);
			plat_prop += sizeof(struct plat_id);
		}

		if (dtb_ver == DEV_TREE_VERSION_V3 && pmic_prop) {
			/* Extract pmic data from DTB */
			for(i = 0 ; i < pmic_data_count; i++) {
				pmic_data[i].pmic_version[0]= fdt32_to_cpu(((struct pmic_id *)pmic_prop)->pmic_version[0]);
				pmic_data[i].pmic_version[1]= fdt32_to_cpu(((struct pmic_id *)pmic_prop)->pmic_version[1]);
				pmic_data[i].pmic_version[2]= fdt32_to_cpu(((struct pmic_id *)pmic_prop)->pmic_version[2]);
				pmic_data[i].pmic_version[3]= fdt32_to_cpu(((struct pmic_id *)pmic_prop)->pmic_version[3]);
				len_pmic_id -= sizeof(struct pmic_id);
				pmic_prop += sizeof(struct pmic_id);
			}

			/* We need to merge board & platform data into dt entry structure */
			num_entries = msm_data_count * board_data_count * pmic_data_count;
		} else {
			/* We need to merge board & platform data into dt entry structure */
			num_entries = msm_data_count * board_data_count;
		}

		if ((((uint64_t)msm_data_count * (uint64_t)board_data_count * (uint64_t)pmic_data_count) !=
			msm_data_count * board_data_count * pmic_data_count) ||
			(((uint64_t)msm_data_count * (uint64_t)board_data_count) != msm_data_count * board_data_count)) {

			free(board_data);
			free(platform_data);
			if (pmic_data)
				free(pmic_data);
			if (model)
				free(model);
			return false;
		}

		dt_entry_array = (struct dt_entry*) malloc(sizeof(struct dt_entry) * num_entries);
		ASSERT(dt_entry_array);

		/* If we have '<X>; <Y>; <Z>' as platform data & '<A>; <B>; <C>' as board data.
		 * Then dt entry should look like
		 * <X ,A >;<X, B>;<X, C>;
		 * <Y ,A >;<Y, B>;<Y, C>;
		 * <Z ,A >;<Z, B>;<Z, C>;
		 */
		i = 0;
		k = 0;
		n = 0;
		dtb_count++;
		for (i = 0; i < msm_data_count; i++) {
			for (j = 0; j < board_data_count; j++) {
				dt_entry_array[k].idx = dtb_count;
				if (dtb_ver == DEV_TREE_VERSION_V3 && pmic_prop){
					for (n = 0; n < pmic_data_count; n++) {
						dt_entry_array[k].platform_id = platform_data[i].platform_id;
						dt_entry_array[k].soc_rev = platform_data[i].soc_rev;
						dt_entry_array[k].variant_id = board_data[j].variant_id;
						dt_entry_array[k].board_hw_subtype = board_data[j].platform_subtype;
						dt_entry_array[k].pmic_rev[0]= pmic_data[n].pmic_version[0];
						dt_entry_array[k].pmic_rev[1]= pmic_data[n].pmic_version[1];
						dt_entry_array[k].pmic_rev[2]= pmic_data[n].pmic_version[2];
						dt_entry_array[k].pmic_rev[3]= pmic_data[n].pmic_version[3];
						dt_entry_array[k].offset = (uint32_t)real_dtb;
						dt_entry_array[k].size = dtb_size;
						k++;
					}

				} else {
					dt_entry_array[k].platform_id = platform_data[i].platform_id;
					dt_entry_array[k].soc_rev = platform_data[i].soc_rev;
					dt_entry_array[k].variant_id = board_data[j].variant_id;
					dt_entry_array[k].board_hw_subtype = board_data[j].platform_subtype;
					dt_entry_array[k].pmic_rev[0]= board_pmic_target(0);
					dt_entry_array[k].pmic_rev[1]= board_pmic_target(1);
					dt_entry_array[k].pmic_rev[2]= board_pmic_target(2);
					dt_entry_array[k].pmic_rev[3]= board_pmic_target(3);
					dt_entry_array[k].offset = (uint32_t)real_dtb;
					dt_entry_array[k].size = dtb_size;
					k++;
				}
			}
		}

		for (i=0 ;i < num_entries; i++) {
			dprintf(SPEW, "Found an appended flattened device tree (%s - %u %u %u 0x%x)\n",
				model ? model : "unknown",
				dt_entry_array[i].platform_id, dt_entry_array[i].variant_id, dt_entry_array[i].board_hw_subtype, dt_entry_array[i].soc_rev);

			if (platform_dt_absolute_match(&(dt_entry_array[i]), dtb_list)) {
				dprintf(SPEW, "Device tree exact match the board: <%u %u %u 0x%x> == <%u %u %u 0x%x>\n",
					dt_entry_array[i].platform_id,
					dt_entry_array[i].variant_id,
					dt_entry_array[i].soc_rev,
					dt_entry_array[i].board_hw_subtype,
					board_platform_id(),
					board_hardware_id(),
					board_hardware_subtype(),
					board_soc_version());

			} else {
				dprintf(SPEW, "Device tree's msm_id doesn't match the board: <%u %u %u 0x%x> != <%u %u %u 0x%x>\n",
					dt_entry_array[i].platform_id,
					dt_entry_array[i].variant_id,
					dt_entry_array[i].soc_rev,
					dt_entry_array[i].board_hw_subtype,
					board_platform_id(),
					board_hardware_id(),
					board_hardware_subtype(),
					board_soc_version());
			}
		}

		free(board_data);
		free(platform_data);
		if (pmic_data)
			free(pmic_data);
		free(dt_entry_array);
	}
	if (model)
		free(model);
	return true;
}

/* function to handle the overlay in independent thread */
static int dtb_overlay_handler(void *args)
{
	dprintf(SPEW, "thread %s() started\n", __func__);

#if WITH_LIB_LIBUFDT
	void *soc_dtb_hdr;

	soc_dtb_hdr = ufdt_install_blob(soc_dtb, fdt_totalsize(soc_dtb));
	if(!soc_dtb_hdr)
	{
		dprintf(CRITICAL, "ERROR: Install Blob failed\n");
		ret = DTBO_ERROR;
		goto out;
	}
	final_dtb_hdr = ufdt_apply_overlay(soc_dtb_hdr, fdt_totalsize(soc_dtb_hdr),board_dtb,
							fdt_totalsize(board_dtb));
	if (!final_dtb_hdr)
	{
		dprintf(CRITICAL, "ERROR: UFDT apply overlay failed\n");
		ret = DTBO_ERROR;
		goto out;
	}
#else /* !WITH_LIB_LIBUFDT */
	int val;

	val = fdt_open_into(soc_dtb, final_dtb_hdr, MAX_DTBO_SZ);
	if (val) {
		dprintf(CRITICAL, "ERROR: fdt_open_into failed: %d\n", val);
		ret = DTBO_ERROR;
		goto out;
	}

	val = fdt_overlay_apply(final_dtb_hdr, board_dtb);
	if (val) {
		dprintf(CRITICAL, "ERROR: FDT overlay apply failed: %d\n", val);
		ret = DTBO_ERROR;
		goto out;
	}
#endif

out:
	/* This flag can only be updated here, hence it is not protected */
	event_signal(&dtbo_event, true);
	thread_exit(0);
	return 0;
}

/*
 * Function to check and form new dtb with Overlay support.
*/
dtbo_error dev_tree_appended_with_dtbo(void *kernel, uint32_t kernel_size,
					uint32_t dtb_offset, void *tags)
{
	void *dtbo_image_buf = NULL;
	uint32_t dtbo_image_sz = 0;

	if (!target_is_emmc_boot())
		return DTBO_NOT_SUPPORTED;

	bs_set_timestamp(BS_DTB_OVERLAY_START);
	ret = load_validate_dtbo_image(&dtbo_image_buf, &dtbo_image_sz);
	if (ret == DTBO_SUCCESS)
	{
		final_dtb_hdr = soc_dtb = get_soc_dtb(kernel, tags,
						kernel_size, dtb_offset);
		if(!soc_dtb)
		{
			ret = DTBO_ERROR;
			goto out;
		}

		if (dtbo_needed)
		{
			board_dtb = get_board_dtb(dtbo_image_buf, dtbo_image_sz);
			if(!board_dtb)
			{
				ret = DTBO_ERROR;
				goto out;
			}

			if((ADD_OF((fdt_totalsize(soc_dtb)), (fdt_totalsize(board_dtb)))) > MAX_DTBO_SZ)
			{
				dprintf(CRITICAL, "ERROR: dtb greater than max supported.\n");
				ret = DTBO_ERROR;
				goto out;
			}

			/*
			spawn a seperate thread for dtbo overlay with indpendent,
			stack to avoid issues with stack corruption seen during flattening,
			of dtb in overlay functionality
			*/
			final_dtb_hdr = tags;
			{
				thread_t *thr = NULL;
				event_init(&dtbo_event, 0, EVENT_FLAG_AUTOUNSIGNAL);
				thr = thread_create("dtb_overlay", dtb_overlay_handler, 0,
							DEFAULT_PRIORITY, DTBO_STACK_SIZE);
				if (!thr)
				{
					dprintf(CRITICAL, "ERROR: Failed to create DTBO thread.\n");
					ret = DTBO_ERROR;
					goto out;
				}
				thread_resume(thr);

				/* block current thread, untill woken up by dtb_overlay_handler. */
				event_wait(&dtbo_event);

				/* ret is updated by dtb overlay thread, in case of error */
				if(ret == DTBO_ERROR)
				{
					goto out;
				}
			} /* dtbo_overlay_handler exited */

		}
		if (final_dtb_hdr != tags)
			memscpy(tags, fdt_totalsize(final_dtb_hdr), final_dtb_hdr,
						fdt_totalsize(final_dtb_hdr));
		dprintf(INFO, "DTB overlay is successful\n");
	}
	else
	{
		dprintf(INFO, "ERROR: DTBO read is not valid\n DTB Overlay failed.\n");
		ret = DTBO_NOT_SUPPORTED;
	}
out:
	bs_set_timestamp(BS_DTB_OVERLAY_END);
	return ret;
}
/*
 * Will relocate the DTB to the tags addr if the device tree is found and return
 * its address
 *
 * Arguments:    kernel - Start address of the kernel/bootimage loaded in RAM
 *               tags - Start address of the tags loaded in RAM
 *               kernel_size - Size of the kernel/bootimage size in bytes
 *
 * Return Value: DTB address : If appended device tree is found
 *               'NULL'         : Otherwise
 */
void *dev_tree_appended(void *kernel, uint32_t kernel_size, uint32_t dtb_offset, void *tags)
{
	uintptr_t kernel_end = (uintptr_t)kernel + kernel_size;
	uint32_t app_dtb_offset = 0;
	void *dtb = NULL;
	void *bestmatch_tag = NULL;
	struct dt_entry *best_match_dt_entry = NULL;
	uint32_t bestmatch_tag_size;
	struct dt_entry_node *dt_entry_queue = NULL;
	struct dt_entry_node *dt_node_tmp1 = NULL;
	struct dt_entry_node *dt_node_tmp2 = NULL;
	dtbo_error ret = DTBO_NOT_SUPPORTED;
	unsigned dtb_count = 0;

	if (dtb_offset)
		app_dtb_offset = dtb_offset;
	else
		memcpy((void*) &app_dtb_offset, (void*) (kernel + DTB_OFFSET), sizeof(uint32_t));

	/* Check for dtbo support */
#if !DTBO_DISABLED
	ret = dev_tree_appended_with_dtbo(kernel, kernel_size, app_dtb_offset, tags);
#endif
	if (ret == DTBO_SUCCESS)
		return tags;
	else if (ret == DTBO_ERROR)
		return NULL;

	/* Initialize the dtb entry node*/
	dt_entry_queue = (struct dt_entry_node *)
				malloc(sizeof(struct dt_entry_node));

	if (!dt_entry_queue) {
		dprintf(CRITICAL, "Out of memory\n");
		return NULL;
	}
	list_initialize(&dt_entry_queue->node);

	if (((uintptr_t)kernel + (uintptr_t)app_dtb_offset) < (uintptr_t)kernel) {
		return NULL;
	}
	dtb = (void *)((uintptr_t)kernel + app_dtb_offset);

	while (((uintptr_t)dtb + sizeof(struct fdt_header)) < (uintptr_t)kernel_end) {
		struct fdt_header dtb_hdr __ALIGNED(8);
		void *dtb_aligned = dtb;
		uint32_t dtb_size;

		/* the DTB could be unaligned, so extract the header,
		 * and operate on it separately */
		memcpy(&dtb_hdr, dtb, sizeof(struct fdt_header));
		if (fdt_check_header((const void *)&dtb_hdr) != 0 ||
		    fdt_check_header_ext((const void *)&dtb_hdr) != 0 ||
		    ((uintptr_t)dtb + (uintptr_t)fdt_totalsize((const void *)&dtb_hdr) < (uintptr_t)dtb) ||
			((uintptr_t)dtb + (uintptr_t)fdt_totalsize((const void *)&dtb_hdr) > (uintptr_t)kernel_end))
			break;
		dtb_size = fdt_totalsize(&dtb_hdr);

		/* Need to copy DTB if not aligned properly :/ */
		if ((uintptr_t)dtb & 7) {
			if (check_aboot_addr_range_overlap((uintptr_t)tags, dtb_size)) {
				dprintf(CRITICAL, "Tags addresses overlap with aboot addresses.\n");
				return NULL;
			}
			memcpy(tags, dtb, dtb_size);
			dtb_aligned = tags;
		}

		dev_tree_compatible(dtb_aligned, dtb, dtb_size, dt_entry_queue);

		/* goto the next device tree if any */
		dtb += dtb_size;
		dtb_count++;
	}

	if (list_is_empty(&dt_entry_queue->node) && (dtb_count == 1)) {
		/* Special case: If we only have one DTB appended which has not
		 * been skales-generated, consider this DTB as the one which is
		 * expected to be used regardless ok LK compatibility check.
		 * Force it as the bestmatch.
		 */
		dprintf(INFO, "Only one appended non-skales DTB, select it.\n");

		bestmatch_tag = kernel + app_dtb_offset;
		bestmatch_tag_size = dtb - (kernel + app_dtb_offset);
	}

	best_match_dt_entry = platform_dt_match_best(dt_entry_queue);
	if (best_match_dt_entry){
		bestmatch_tag = (void *)best_match_dt_entry->offset;
		bestmatch_tag_size = best_match_dt_entry->size;
		dtb_idx = best_match_dt_entry->idx;
		dprintf(INFO, "Best match DTB tags %u/%u/%08x/0x%08x/%x/%x/%x/%x/%x/%x/%x\n",
			best_match_dt_entry->idx, best_match_dt_entry->platform_id,
			best_match_dt_entry->variant_id, best_match_dt_entry->board_hw_subtype,
			best_match_dt_entry->soc_rev, best_match_dt_entry->pmic_rev[0],
			best_match_dt_entry->pmic_rev[1], best_match_dt_entry->pmic_rev[2],
			best_match_dt_entry->pmic_rev[3], best_match_dt_entry->offset,
			best_match_dt_entry->size);
		dprintf(INFO, "Using pmic info 0x%0x/0x%x/0x%x/0x%0x for device 0x%0x/0x%x/0x%x/0x%0x\n",
			best_match_dt_entry->pmic_rev[0], best_match_dt_entry->pmic_rev[1],
			best_match_dt_entry->pmic_rev[2], best_match_dt_entry->pmic_rev[3],
			board_pmic_target(0), board_pmic_target(1),
			board_pmic_target(2), board_pmic_target(3));
	}
	/* free queue's memory */
	list_for_every_entry(&dt_entry_queue->node, dt_node_tmp1, dt_node, node) {
		dt_node_tmp2 = (struct dt_entry_node *) dt_node_tmp1->node.prev;
		dt_entry_list_delete(dt_node_tmp1);
		dt_node_tmp1 = dt_node_tmp2;
	}

	if(bestmatch_tag) {
		if (check_aboot_addr_range_overlap((uintptr_t)tags, bestmatch_tag_size)) {
			dprintf(CRITICAL, "Tags addresses overlap with aboot addresses.\n");
			return NULL;
		}
		memcpy(tags, bestmatch_tag, bestmatch_tag_size);
		/* clear out the old DTB magic so kernel doesn't find it */
		*((uint32_t *)(kernel + app_dtb_offset)) = 0;
		return tags;
	}

	dprintf(CRITICAL, "DTB offset is incorrect, kernel image does not have appended DTB\n");

	dprintf(INFO, "Device info 0x%08x/%08x/0x%08x/%u, pmic 0x%0x/0x%x/0x%x/0x%0x\n",
			board_platform_id(), board_soc_version(),
			board_target_id(), board_hardware_subtype(),
			board_pmic_target(0), board_pmic_target(1),
			board_pmic_target(2), board_pmic_target(3));
	return NULL;
}

/* Returns 0 if the device tree is valid. */
int dev_tree_validate(struct dt_table *table, unsigned int page_size, uint32_t *dt_hdr_size)
{
	int dt_entry_size;
	uint64_t hdr_size;

	/* Validate the device tree table header */
	if(table->magic != DEV_TREE_MAGIC) {
		dprintf(CRITICAL, "ERROR: Bad magic in device tree table \n");
		return -1;
	}

	if (table->version == DEV_TREE_VERSION_V1) {
		dt_entry_size = sizeof(struct dt_entry_v1);
	} else if (table->version == DEV_TREE_VERSION_V2) {
		dt_entry_size = sizeof(struct dt_entry_v2);
	} else if (table->version == DEV_TREE_VERSION_V3) {
		dt_entry_size = DEV_TREE_DT_ENTRY_SIZE_V3;
	} else {
		dprintf(CRITICAL, "ERROR: Unsupported version (%d) in DT table \n",
				table->version);
		return -1;
	}

	hdr_size = (uint64_t)table->num_entries * dt_entry_size + DEV_TREE_HEADER_SIZE;

	/* Roundup to page_size. */
	hdr_size = ROUNDUP(hdr_size, page_size);

	if (hdr_size > UINT_MAX)
		return -1;
	else
		*dt_hdr_size = hdr_size & UINT_MAX;

	return 0;
}

static int platform_dt_absolute_match(struct dt_entry *cur_dt_entry, struct dt_entry_node *dt_list)
{
	uint32_t cur_dt_hlos_ddr;
	uint32_t cur_dt_hw_platform;
	uint32_t cur_dt_hw_subtype;
	uint32_t cur_dt_msm_id;
	dt_node *dt_node_tmp = NULL;

	/* Platform-id
	* bit no |31	 24|23	16|15	0|
	*        |reserved|foundry-id|msm-id|
	*/
	cur_dt_msm_id = (cur_dt_entry->platform_id & 0x0000ffff);
	cur_dt_hw_platform = (cur_dt_entry->variant_id & 0x000000ff);
	cur_dt_hw_subtype = (cur_dt_entry->board_hw_subtype & 0xff);

	/* Determine the bits 10:8 to check the DT with the DDR Size */
	cur_dt_hlos_ddr = (cur_dt_entry->board_hw_subtype & 0x700);

	/* 1. must match the msm_id, platform_hw_id, platform_subtype and DDR size
	*  soc, board major/minor, pmic major/minor must less than board info
	*  2. find the matched DTB then return 1
	*  3. otherwise return 0
	*/
	if((cur_dt_msm_id == (board_platform_id() & 0x0000ffff)) &&
		(cur_dt_hw_platform == board_hardware_id()) &&
		(cur_dt_hw_subtype == board_hardware_subtype()) &&
		(cur_dt_hlos_ddr == (target_get_hlos_subtype() & 0x700)) &&
		(cur_dt_entry->soc_rev <= board_soc_version()) &&
		((cur_dt_entry->variant_id & 0x00ffff00) <= (board_target_id() & 0x00ffff00)) &&
		((cur_dt_entry->pmic_rev[0] & 0x00ffff00) <= (board_pmic_target(0) & 0x00ffff00)) &&
		((cur_dt_entry->pmic_rev[1] & 0x00ffff00) <= (board_pmic_target(1) & 0x00ffff00)) &&
		((cur_dt_entry->pmic_rev[2] & 0x00ffff00) <= (board_pmic_target(2) & 0x00ffff00)) &&
		((cur_dt_entry->pmic_rev[3] & 0x00ffff00) <= (board_pmic_target(3) & 0x00ffff00))) {

		dt_node_tmp = dt_entry_list_init();
		memcpy((char*)dt_node_tmp->dt_entry_m,(char*)cur_dt_entry, sizeof(struct dt_entry));

		dprintf(SPEW, "Add DTB entry %u/%08x/0x%08x/%x/%x/%x/%x/%x/%x/%x\n",
			dt_node_tmp->dt_entry_m->platform_id, dt_node_tmp->dt_entry_m->variant_id,
			dt_node_tmp->dt_entry_m->board_hw_subtype, dt_node_tmp->dt_entry_m->soc_rev,
			dt_node_tmp->dt_entry_m->pmic_rev[0], dt_node_tmp->dt_entry_m->pmic_rev[1],
			dt_node_tmp->dt_entry_m->pmic_rev[2], dt_node_tmp->dt_entry_m->pmic_rev[3],
			dt_node_tmp->dt_entry_m->offset, dt_node_tmp->dt_entry_m->size);

		insert_dt_entry_in_queue(dt_list, dt_node_tmp);
		return 1;
	}
	return 0;
}

static int platform_dt_absolute_compat_match(struct dt_entry_node *dt_list, uint32_t dtb_info) {
	struct dt_entry_node *dt_node_tmp1 = NULL;
	struct dt_entry_node *dt_node_tmp2 = NULL;
	uint32_t current_info = 0;
	uint32_t board_info = 0;
	uint32_t best_info = 0;
	uint32_t current_pmic_model[4] = {0, 0, 0, 0};
	uint32_t board_pmic_model[4] = {0, 0, 0, 0};
	uint32_t best_pmic_model[4] = {0, 0, 0, 0};
	uint32_t delete_current_dt = 0;
	uint32_t i;

	/* start to select the exact entry
	* default to exact match 0, if find current DTB entry info is the same as board info,
	* then exact match board info.
	*/
	list_for_every_entry(&dt_list->node, dt_node_tmp1, dt_node, node) {
		if (!dt_node_tmp1){
			dprintf(SPEW, "Current node is the end\n");
			break;
		}
		switch(dtb_info) {
		case DTB_FOUNDRY:
			current_info = ((dt_node_tmp1->dt_entry_m->platform_id) & 0x00ff0000);
			board_info = board_foundry_id() << 16;
			break;
		case DTB_PMIC_MODEL:
			for (i = 0; i < 4; i++) {
				current_pmic_model[i] = (dt_node_tmp1->dt_entry_m->pmic_rev[i] & 0xff);
				board_pmic_model[i] = (board_pmic_target(i) & 0xff);
			}
			break;
		case DTB_PANEL_TYPE:
			current_info = ((dt_node_tmp1->dt_entry_m->board_hw_subtype) & 0x1800);
			board_info = (target_get_hlos_subtype() & 0x1800);
			break;
		case DTB_BOOT_DEVICE:
			current_info = ((dt_node_tmp1->dt_entry_m->board_hw_subtype) & 0xf0000);
			board_info = (target_get_hlos_subtype() & 0xf0000);
			break;
		default:
			dprintf(CRITICAL, "ERROR: Unsupported version (%d) in dt node check \n",
					dtb_info);
			return 0;
		}

		if (dtb_info == DTB_PMIC_MODEL) {
			if ((current_pmic_model[0] == board_pmic_model[0]) &&
				(current_pmic_model[1] == board_pmic_model[1]) &&
				(current_pmic_model[2] == board_pmic_model[2]) &&
				(current_pmic_model[3] == board_pmic_model[3])) {

				for (i = 0; i < 4; i++) {
					best_pmic_model[i] = current_pmic_model[i];
				}
				break;
			}
		} else {
			if (current_info == board_info) {
				best_info = current_info;
				break;
			}
		}
	}

	list_for_every_entry(&dt_list->node, dt_node_tmp1, dt_node, node) {
		if (!dt_node_tmp1){
			dprintf(SPEW, "Current node is the end\n");
			break;
		}
		switch(dtb_info) {
		case DTB_FOUNDRY:
			current_info = ((dt_node_tmp1->dt_entry_m->platform_id) & 0x00ff0000);
			break;
		case DTB_PMIC_MODEL:
			for (i = 0; i < 4; i++) {
				current_pmic_model[i] = (dt_node_tmp1->dt_entry_m->pmic_rev[i] & 0xff);
			}
			break;
		case DTB_PANEL_TYPE:
			current_info = ((dt_node_tmp1->dt_entry_m->board_hw_subtype) & 0x1800);
			break;
		case DTB_BOOT_DEVICE:
			current_info = ((dt_node_tmp1->dt_entry_m->board_hw_subtype) & 0xf0000);
			break;
		default:
			dprintf(CRITICAL, "ERROR: Unsupported version (%d) in dt node check \n",
					dtb_info);
			return 0;
		}

		if (dtb_info == DTB_PMIC_MODEL) {
			if ((current_pmic_model[0] != best_pmic_model[0]) ||
				(current_pmic_model[1] != best_pmic_model[1]) ||
				(current_pmic_model[2] != best_pmic_model[2]) ||
				(current_pmic_model[3] != best_pmic_model[3])) {

				delete_current_dt = 1;
			}
		} else {
			if (current_info != best_info) {
				delete_current_dt = 1;
			}
		}

		if (delete_current_dt) {
			dprintf(SPEW, "Delete don't fit DTB entry %u/%08x/0x%08x/%x/%x/%x/%x/%x/%x/%x\n",
				dt_node_tmp1->dt_entry_m->platform_id, dt_node_tmp1->dt_entry_m->variant_id,
				dt_node_tmp1->dt_entry_m->board_hw_subtype, dt_node_tmp1->dt_entry_m->soc_rev,
				dt_node_tmp1->dt_entry_m->pmic_rev[0], dt_node_tmp1->dt_entry_m->pmic_rev[1],
				dt_node_tmp1->dt_entry_m->pmic_rev[2], dt_node_tmp1->dt_entry_m->pmic_rev[3],
				dt_node_tmp1->dt_entry_m->offset, dt_node_tmp1->dt_entry_m->size);

			dt_node_tmp2 = (struct dt_entry_node *) dt_node_tmp1->node.prev;
			dt_entry_list_delete(dt_node_tmp1);
			dt_node_tmp1 = dt_node_tmp2;
			delete_current_dt = 0;
		}
	}

	return 1;
}

static int update_dtb_entry_node(struct dt_entry_node *dt_list, uint32_t dtb_info) {
	struct dt_entry_node *dt_node_tmp1 = NULL;
	struct dt_entry_node *dt_node_tmp2 = NULL;
	uint32_t current_info = 0;
	uint32_t board_info = 0;
	uint32_t best_info = 0;

	/* start to select the best entry*/
	list_for_every_entry(&dt_list->node, dt_node_tmp1, dt_node, node) {
		if (!dt_node_tmp1){
			dprintf(SPEW, "Current node is the end\n");
			break;
		}
		switch(dtb_info) {
		case DTB_SOC:
			current_info = dt_node_tmp1->dt_entry_m->soc_rev;
			board_info = board_soc_version();
			break;
		case DTB_MAJOR_MINOR:
			current_info = ((dt_node_tmp1->dt_entry_m->variant_id) & 0x00ffff00);
			board_info = (board_target_id() & 0x00ffff00);
			break;
		case DTB_PMIC0:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[0]) & 0x00ffff00);
			board_info = (board_pmic_target(0) & 0x00ffff00);
			break;
		case DTB_PMIC1:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[1]) & 0x00ffff00);
			board_info = (board_pmic_target(1) & 0x00ffff00);
			break;
		case DTB_PMIC2:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[2]) & 0x00ffff00);
			board_info = (board_pmic_target(2) & 0x00ffff00);
			break;
		case DTB_PMIC3:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[3]) & 0x00ffff00);
			board_info = (board_pmic_target(3) & 0x00ffff00);
			break;
		default:
			dprintf(CRITICAL, "ERROR: Unsupported version (%d) in dt node check \n",
					dtb_info);
			return 0;
		}

		if (current_info == board_info) {
			best_info = current_info;
			break;
		}
		if ((current_info < board_info) && (current_info > best_info)) {
			best_info = current_info;
		}
		if (current_info < best_info) {
			dprintf(SPEW, "Delete don't fit DTB entry %u/%08x/0x%08x/%x/%x/%x/%x/%x/%x/%x\n",
				dt_node_tmp1->dt_entry_m->platform_id, dt_node_tmp1->dt_entry_m->variant_id,
				dt_node_tmp1->dt_entry_m->board_hw_subtype, dt_node_tmp1->dt_entry_m->soc_rev,
				dt_node_tmp1->dt_entry_m->pmic_rev[0], dt_node_tmp1->dt_entry_m->pmic_rev[1],
				dt_node_tmp1->dt_entry_m->pmic_rev[2], dt_node_tmp1->dt_entry_m->pmic_rev[3],
				dt_node_tmp1->dt_entry_m->offset, dt_node_tmp1->dt_entry_m->size);

			dt_node_tmp2 = (struct dt_entry_node *) dt_node_tmp1->node.prev;
			dt_entry_list_delete(dt_node_tmp1);
			dt_node_tmp1 = dt_node_tmp2;
		}
	}

	list_for_every_entry(&dt_list->node, dt_node_tmp1, dt_node, node) {
		if (!dt_node_tmp1){
			dprintf(SPEW, "Current node is the end\n");
			break;
		}
		switch(dtb_info) {
		case DTB_SOC:
			current_info = dt_node_tmp1->dt_entry_m->soc_rev;
			break;
		case DTB_MAJOR_MINOR:
			current_info = ((dt_node_tmp1->dt_entry_m->variant_id) & 0x00ffff00);
			break;
		case DTB_PMIC0:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[0]) & 0x00ffff00);
			break;
		case DTB_PMIC1:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[1]) & 0x00ffff00);
			break;
		case DTB_PMIC2:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[2]) & 0x00ffff00);
			break;
		case DTB_PMIC3:
			current_info = ((dt_node_tmp1->dt_entry_m->pmic_rev[3]) & 0x00ffff00);
			break;
		default:
			dprintf(CRITICAL, "ERROR: Unsupported version (%d) in dt node check \n",
					dtb_info);
			return 0;
		}

		if (current_info != best_info) {
			dprintf(SPEW, "Delete don't fit DTB entry %u/%08x/0x%08x/%x/%x/%x/%x/%x/%x/%x\n",
				dt_node_tmp1->dt_entry_m->platform_id, dt_node_tmp1->dt_entry_m->variant_id,
				dt_node_tmp1->dt_entry_m->board_hw_subtype, dt_node_tmp1->dt_entry_m->soc_rev,
				dt_node_tmp1->dt_entry_m->pmic_rev[0], dt_node_tmp1->dt_entry_m->pmic_rev[1],
				dt_node_tmp1->dt_entry_m->pmic_rev[2], dt_node_tmp1->dt_entry_m->pmic_rev[3],
				dt_node_tmp1->dt_entry_m->offset, dt_node_tmp1->dt_entry_m->size);

			dt_node_tmp2 = (struct dt_entry_node *) dt_node_tmp1->node.prev;
			dt_entry_list_delete(dt_node_tmp1);
			dt_node_tmp1 = dt_node_tmp2;
		}
	}
	return 1;
}

static struct dt_entry *platform_dt_match_best(struct dt_entry_node *dt_list)
{
	struct dt_entry_node *dt_node_tmp1 = NULL;

	/* check Foundry id
	* the foundry id must exact match board founddry id, this is compatibility check,
	* if couldn't find the exact match from DTB, will exact match 0x0.
	*/
	if (!platform_dt_absolute_compat_match(dt_list, DTB_FOUNDRY))
		return NULL;

	/* check PMIC model
	* the PMIC model must exact match board PMIC model, this is compatibility check,
	* if couldn't find the exact match from DTB, will exact match 0x0.
	*/
	if (!platform_dt_absolute_compat_match(dt_list, DTB_PMIC_MODEL))
		return NULL;

	/* check panel type
	* the panel  type must exact match board panel type, this is compatibility check,
	* if couldn't find the exact match from DTB, will exact match 0x0.
	*/
	if (!platform_dt_absolute_compat_match(dt_list, DTB_PANEL_TYPE))
		return NULL;

	/* check boot device subtype
	* the boot device subtype must exact match board boot device subtype, this is compatibility check,
	* if couldn't find the exact match from DTB, will exact match 0x0.
	*/
	if (!platform_dt_absolute_compat_match(dt_list, DTB_BOOT_DEVICE))
		return NULL;

	/* check soc version
	* the suitable soc version must less than or equal to board soc version
	*/
	if (!update_dtb_entry_node(dt_list, DTB_SOC))
		return NULL;

	/*check major and minor version
	* the suitable major&minor version must less than or equal to board major&minor version
	*/
	if (!update_dtb_entry_node(dt_list, DTB_MAJOR_MINOR))
		return NULL;

	/*check pmic info
	* the suitable pmic major&minor info must less than or equal to board pmic major&minor version
	*/
	if (!update_dtb_entry_node(dt_list, DTB_PMIC0))
		return NULL;
	if (!update_dtb_entry_node(dt_list, DTB_PMIC1))
		return NULL;
	if (!update_dtb_entry_node(dt_list, DTB_PMIC2))
		return NULL;
	if (!update_dtb_entry_node(dt_list, DTB_PMIC3))
		return NULL;

	list_for_every_entry(&dt_list->node, dt_node_tmp1, dt_node, node) {
		if (!dt_node_tmp1) {
			dprintf(CRITICAL, "ERROR: Couldn't find the suitable DTB!\n");
			return NULL;
		}
		if (dt_node_tmp1->dt_entry_m)
			return dt_node_tmp1->dt_entry_m;
	}

	return NULL;
}

/* Function to obtain the index information for the correct device tree
 *  based on the platform data.
 *  If a matching device tree is found, the information is returned in the
 *  "dt_entry_info" out parameter and a function value of 0 is returned, otherwise
 *  a non-zero function value is returned.
 */
int dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info)
{
	uint32_t i;
	unsigned char *table_ptr = NULL;
	struct dt_entry dt_entry_buf_1;
	struct dt_entry *cur_dt_entry = NULL;
	struct dt_entry *best_match_dt_entry = NULL;
	struct dt_entry_v1 *dt_entry_v1 = NULL;
	struct dt_entry_v2 *dt_entry_v2 = NULL;
	struct dt_entry_node *dt_entry_queue = NULL;
	struct dt_entry_node *dt_node_tmp1 = NULL;
	struct dt_entry_node *dt_node_tmp2 = NULL;
	uint32_t found = 0;

	if (!dt_entry_info) {
		dprintf(CRITICAL, "ERROR: Bad parameter passed to %s \n",
				__func__);
		return -1;
	}

	table_ptr = (unsigned char *)table + DEV_TREE_HEADER_SIZE;
	cur_dt_entry = &dt_entry_buf_1;
	best_match_dt_entry = NULL;
	dt_entry_queue = (struct dt_entry_node *)
				malloc(sizeof(struct dt_entry_node));

	if (!dt_entry_queue) {
		dprintf(CRITICAL, "Out of memory\n");
		return -1;
	}

	list_initialize(&dt_entry_queue->node);
	dprintf(INFO, "DTB Total entry: %d, DTB version: %d\n", table->num_entries, table->version);
	for(i = 0; found == 0 && i < table->num_entries; i++)
	{
		memset(cur_dt_entry, 0, sizeof(struct dt_entry));
		switch(table->version) {
		case DEV_TREE_VERSION_V1:
			dt_entry_v1 = (struct dt_entry_v1 *)table_ptr;
			cur_dt_entry->platform_id = dt_entry_v1->platform_id;
			cur_dt_entry->variant_id = dt_entry_v1->variant_id;
			cur_dt_entry->soc_rev = dt_entry_v1->soc_rev;
			cur_dt_entry->board_hw_subtype = (dt_entry_v1->variant_id >> 0x18);
			cur_dt_entry->pmic_rev[0] = board_pmic_target(0);
			cur_dt_entry->pmic_rev[1] = board_pmic_target(1);
			cur_dt_entry->pmic_rev[2] = board_pmic_target(2);
			cur_dt_entry->pmic_rev[3] = board_pmic_target(3);
			cur_dt_entry->offset = dt_entry_v1->offset;
			cur_dt_entry->size = dt_entry_v1->size;
			table_ptr += sizeof(struct dt_entry_v1);
			break;
		case DEV_TREE_VERSION_V2:
			dt_entry_v2 = (struct dt_entry_v2*)table_ptr;
			cur_dt_entry->platform_id = dt_entry_v2->platform_id;
			cur_dt_entry->variant_id = dt_entry_v2->variant_id;
			cur_dt_entry->soc_rev = dt_entry_v2->soc_rev;
			/* For V2 version of DTBs we have platform version field as part
			 * of variant ID, in such case the subtype will be mentioned as 0x0
			 * As the qcom, board-id = <0xSSPMPmPH, 0x0>
			 * SS -- Subtype
			 * PM -- Platform major version
			 * Pm -- Platform minor version
			 * PH -- Platform hardware CDP/MTP
			 * In such case to make it compatible with LK algorithm move the subtype
			 * from variant_id to subtype field
			 */
			if (dt_entry_v2->board_hw_subtype == 0)
				cur_dt_entry->board_hw_subtype = (cur_dt_entry->variant_id >> 0x18);
			else
				cur_dt_entry->board_hw_subtype = dt_entry_v2->board_hw_subtype;
			cur_dt_entry->pmic_rev[0] = board_pmic_target(0);
			cur_dt_entry->pmic_rev[1] = board_pmic_target(1);
			cur_dt_entry->pmic_rev[2] = board_pmic_target(2);
			cur_dt_entry->pmic_rev[3] = board_pmic_target(3);
			cur_dt_entry->offset = dt_entry_v2->offset;
			cur_dt_entry->size = dt_entry_v2->size;
			table_ptr += sizeof(struct dt_entry_v2);
			break;
		case DEV_TREE_VERSION_V3:
			memcpy(cur_dt_entry, (struct dt_entry *)table_ptr,
				   sizeof(struct dt_entry));
			/* For V3 version of DTBs we have platform version field as part
			 * of variant ID, in such case the subtype will be mentioned as 0x0
			 * As the qcom, board-id = <0xSSPMPmPH, 0x0>
			 * SS -- Subtype
			 * PM -- Platform major version
			 * Pm -- Platform minor version
			 * PH -- Platform hardware CDP/MTP
			 * In such case to make it compatible with LK algorithm move the subtype
			 * from variant_id to subtype field
			 */
			if (cur_dt_entry->board_hw_subtype == 0)
				cur_dt_entry->board_hw_subtype = (cur_dt_entry->variant_id >> 0x18);

			table_ptr += DEV_TREE_DT_ENTRY_SIZE_V3;
			break;
		default:
			dprintf(CRITICAL, "ERROR: Unsupported version (%d) in DT table \n",
					table->version);
			free(dt_entry_queue);
			return -1;
		}

		/* DTBs must match the platform_id, platform_hw_id, platform_subtype and DDR size.
		* The satisfactory DTBs are stored in dt_entry_queue
		*/
		platform_dt_absolute_match(cur_dt_entry, dt_entry_queue);

	}
	best_match_dt_entry = platform_dt_match_best(dt_entry_queue);
	if (best_match_dt_entry) {
		*dt_entry_info = *best_match_dt_entry;
		found = 1;
	}

	if (found != 0) {
		dprintf(INFO, "Using DTB entry 0x%08x/%08x/0x%08x/%u for device 0x%08x/%08x/0x%08x/%u\n",
				dt_entry_info->platform_id, dt_entry_info->soc_rev,
				dt_entry_info->variant_id, dt_entry_info->board_hw_subtype,
				board_platform_id(), board_soc_version(),
				board_target_id(), board_hardware_subtype());
		if (dt_entry_info->pmic_rev[0] == 0 && dt_entry_info->pmic_rev[0] == 0 &&
			dt_entry_info->pmic_rev[0] == 0 && dt_entry_info->pmic_rev[0] == 0) {
			dprintf(SPEW, "No maintain pmic info in DTB, device pmic info is 0x%0x/0x%x/0x%x/0x%0x\n",
					board_pmic_target(0), board_pmic_target(1),
					board_pmic_target(2), board_pmic_target(3));
		} else {
			dprintf(INFO, "Using pmic info 0x%0x/0x%x/0x%x/0x%0x for device 0x%0x/0x%x/0x%x/0x%0x\n",
					dt_entry_info->pmic_rev[0], dt_entry_info->pmic_rev[1],
					dt_entry_info->pmic_rev[2], dt_entry_info->pmic_rev[3],
					board_pmic_target(0), board_pmic_target(1),
					board_pmic_target(2), board_pmic_target(3));
		}
		return 0;
	}

	dprintf(CRITICAL, "ERROR: Unable to find suitable device tree for device (%u/0x%08x/0x%08x/%u)\n",
			board_platform_id(), board_soc_version(),
			board_target_id(), board_hardware_subtype());

	list_for_every_entry(&dt_entry_queue->node, dt_node_tmp1, dt_node, node) {
		/* free node memory */
		dt_node_tmp2 = (struct dt_entry_node *) dt_node_tmp1->node.prev;
		dt_entry_list_delete(dt_node_tmp1);
		dt_node_tmp1 = dt_node_tmp2;
	}
	free(dt_entry_queue);
	return -1;
}

/* Function to add the first RAM partition info to the device tree.
 * Note: The function replaces the reg property in the "/memory" node
 * with the addr and size provided.
 */
int dev_tree_add_first_mem_info(uint32_t *fdt, uint32_t offset, uint32_t addr, uint32_t size)
{
	int ret;

	ret = fdt_setprop_u32(fdt, offset, "reg", addr);

	if (ret)
	{
		dprintf(CRITICAL, "Failed to add the memory information addr: %d\n",
				ret);
	}

	ret = fdt_appendprop_u32(fdt, offset, "reg", size);

	if (ret)
	{
		dprintf(CRITICAL, "Failed to add the memory information size: %d\n",
				ret);
	}

	return ret;
}

static int dev_tree_query_memory_cell_sizes(void *fdt, struct dt_mem_node_info *mem_node, uint32_t mem_node_offset)
{
	int      len;
	uint32_t *valp;
	int      ret;
	uint32_t offset;

	mem_node->offset = mem_node_offset;

	/* Get offset of the root node */
	ret = fdt_path_offset(fdt, "/");
	if (ret < 0)
	{
		dprintf(CRITICAL, "Could not find memory node.\n");
		return ret;
	}

	offset = ret;

	/* Find the #address-cells size. */
	valp = (uint32_t*)fdt_getprop(fdt, offset, "#address-cells", &len);
	if (len <= 0 || !valp)
	{
		if (len == -FDT_ERR_NOTFOUND)
		{
			/* Property not found.
			* Assume standard sizes.
			*/
			mem_node->addr_cell_size = 2;
			dprintf(CRITICAL, "Using default #addr_cell_size: %u\n", mem_node->addr_cell_size);
		}
		else
		{
			dprintf(CRITICAL, "Error finding the #address-cells property\n");
			return len;
		}
	}
	else
		mem_node->addr_cell_size = fdt32_to_cpu(*valp);

	/* Find the #size-cells size. */
	valp = (uint32_t*)fdt_getprop(fdt, offset, "#size-cells", &len);
	if (len <= 0 || !valp)
	{
		if (len == -FDT_ERR_NOTFOUND)
		{
			/* Property not found.
			* Assume standard sizes.
			*/
			mem_node->size_cell_size = 1;
			dprintf(CRITICAL, "Using default #size_cell_size: %u\n", mem_node->size_cell_size);
		}
		else
		{
			dprintf(CRITICAL, "Error finding the #size-cells property\n");
			return len;
		}
	}
	else
		mem_node->size_cell_size = fdt32_to_cpu(*valp);

	return 0;
}

static void dev_tree_update_memory_node(uint32_t offset)
{
	mem_node.offset = offset;
	mem_node.addr_cell_size = 1;
	mem_node.size_cell_size = 1;
}

/* Function to add the subsequent RAM partition info to the device tree. */
int dev_tree_add_mem_info(void *fdt, uint32_t offset, uint64_t addr, uint64_t size)
{
	int ret = 0;

	if(smem_get_ram_ptable_version() >= 1)
	{
		ret = dev_tree_query_memory_cell_sizes(fdt, &mem_node, offset);
		if (ret < 0)
		{
			dprintf(CRITICAL, "Could not find #address-cells and #size-cells properties: ret %d\n", ret);
			return ret;
		}

	}
	else
	{
		dev_tree_update_memory_node(offset);
	}

	if (!(mem_node.mem_info_cnt))
	{
		/* Replace any other reg prop in the memory node. */

		/* cell_size is the number of 32 bit words used to represent an address/length in the device tree.
		 * memory node in DT can be either 32-bit(cell-size = 1) or 64-bit(cell-size = 2).So when updating
		 * the memory node in the device tree, we write one word or two words based on cell_size = 1 or 2.
		 */

		if(mem_node.addr_cell_size == 2)
		{
			ret = fdt_setprop_u32(fdt, mem_node.offset, "reg", addr >> 32);
			if(ret)
			{
				dprintf(CRITICAL, "ERROR: Could not set prop reg for memory node\n");
				return ret;
			}

			ret = fdt_appendprop_u32(fdt, mem_node.offset, "reg", (uint32_t)addr);
			if(ret)
			{
				dprintf(CRITICAL, "ERROR: Could not append prop reg for memory node\n");
				return ret;
			}
		}
		else
		{
			ret = fdt_setprop_u32(fdt, mem_node.offset, "reg", (uint32_t)addr);
			if(ret)
			{
				dprintf(CRITICAL, "ERROR: Could not set prop reg for memory node\n");
				return ret;
			}
		}

		mem_node.mem_info_cnt = 1;
	}
	else
	{
		/* Append the mem info to the reg prop for subsequent nodes.  */
		if(mem_node.addr_cell_size == 2)
		{
			ret = fdt_appendprop_u32(fdt, mem_node.offset, "reg", addr >> 32);
			if(ret)
			{
				dprintf(CRITICAL, "ERROR: Could not append prop reg for memory node\n");
				return ret;
			}
		}

		ret = fdt_appendprop_u32(fdt, mem_node.offset, "reg", (uint32_t)addr);
		if(ret)
		{
			dprintf(CRITICAL, "ERROR: Could not append prop reg for memory node\n");
			return ret;
		}
	}

	if(mem_node.size_cell_size == 2)
	{
		ret = fdt_appendprop_u32(fdt, mem_node.offset, "reg", size>>32);
		if(ret)
		{
			dprintf(CRITICAL, "ERROR: Could not append prop reg for memory node\n");
			return ret;
		}
	}

	ret = fdt_appendprop_u32(fdt, mem_node.offset, "reg", (uint32_t)size);

	if (ret)
	{
		dprintf(CRITICAL, "Failed to add the memory information size: %d\n",
				ret);
		return ret;
	}

	return ret;
}

static int call_dt_update_handlers(void *fdt, const char *cmdline,
				   enum boot_type boot_type)
{
	extern const struct dt_update_handler __dt_update_start;
	extern const struct dt_update_handler __dt_update_end;
	const struct dt_update_handler *dtu;
	int ret;

	if (&__dt_update_start == &__dt_update_end)
		return 0;

	/* Most downstream/vendor device trees seem to have this node */
	if (fdt_path_offset(fdt, "/soc/qcom,ion") > 0)
		boot_type |= BOOT_DOWNSTREAM;

	for (dtu = &__dt_update_start; dtu < &__dt_update_end; ++dtu) {
		ret = dtu->update_dt(fdt, cmdline, boot_type);
		if (ret) {
			dprintf(CRITICAL, "%s failed: %d\n", dtu->name, ret);
			return ret;
		}
	}
	return 0;
}

/* Top level function that updates the device tree. */
int update_device_tree(void *fdt, const char *cmdline, enum boot_type boot_type,
					   void *ramdisk, uint32_t ramdisk_size)
{
	int ret = 0;
	uint32_t offset;
#if ENABLE_KASLRSEED_SUPPORT
	uint64_t kaslrseed;
#endif
	uint32_t cmdline_len = 0;

	if (cmdline)
		cmdline_len = strlen(cmdline);

	/* Check the device tree header */
	ret = fdt_check_header(fdt) || fdt_check_header_ext(fdt);
	if (ret)
	{
		dprintf(CRITICAL, "Invalid device tree header \n");
		return ret;
	}

	if (check_aboot_addr_range_overlap((uint32_t)fdt,
				(fdt_totalsize(fdt) + DTB_PAD_SIZE + cmdline_len))) {
		dprintf(CRITICAL, "Error: Fdt addresses overlap with aboot addresses.\n");
		return ret;
	}

	/* Add padding to make space for new nodes and properties. */
	ret = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + DTB_PAD_SIZE + cmdline_len);
	if (ret!= 0)
	{
		dprintf(CRITICAL, "Failed to move/resize dtb buffer: %d\n", ret);
		return ret;
	}

	/* Get offset of the memory node */
	ret = fdt_path_offset(fdt, "/memory");
	if (ret < 0)
	{
		dprintf(CRITICAL, "Could not find memory node.\n");
		return ret;
	}

	offset = ret;

	ret = target_dev_tree_mem(fdt, offset);
	if(ret)
	{
		dprintf(CRITICAL, "ERROR: Cannot update memory node\n");
		return ret;
	}

	/* Get offset of the chosen node */
	ret = fdt_path_offset(fdt, "/chosen");
	if (ret < 0)
	{
		dprintf(CRITICAL, "Could not find chosen node.\n");
		return ret;
	}

	offset = ret;
	if (cmdline)
	{
		int len;
		char *oldargs = fdt_getprop_w(fdt, offset, "bootargs", &len);

		/* Replace old null terminator so the strings are concatenated */
		if (oldargs && len >= 1 && oldargs[len-1] == '\0')
			oldargs[len-1] = ' ';

		/* Adding the cmdline to the chosen node */
		ret = fdt_appendprop_string(fdt, offset, (const char*)"bootargs", (const void*)cmdline);
		if (ret)
		{
			dprintf(CRITICAL, "ERROR: Cannot update chosen node [bootargs]\n");
			return ret;
		}
	}

#if ENABLE_KASLRSEED_SUPPORT
	if (!scm_random((uintptr_t *)&kaslrseed, sizeof(kaslrseed))) {
		/* Adding Kaslr Seed to the chosen node */
		ret = fdt_appendprop_u64 (fdt, offset, (const char *)"kaslr-seed", (uint64_t)kaslrseed);
		if (ret)
			dprintf(CRITICAL, "ERROR: Cannot update chosen node [kaslr-seed] - 0x%x\n", ret);
		else
			dprintf(CRITICAL, "kaslr-Seed is added to chosen node\n");
	} else {
		dprintf(CRITICAL, "ERROR: Cannot generate Kaslr Seed\n");
	}
#endif

	if (ramdisk_size) {
		/* Adding the initrd-start to the chosen node */
		ret = fdt_setprop_u32(fdt, offset, "linux,initrd-start",
				      (uint32_t)ramdisk);
		if (ret)
		{
			dprintf(CRITICAL, "ERROR: Cannot update chosen node [linux,initrd-start]\n");
			return ret;
		}

		/* Adding the initrd-end to the chosen node */
		ret = fdt_setprop_u32(fdt, offset, "linux,initrd-end",
				      ((uint32_t)ramdisk + ramdisk_size));
		if (ret)
		{
			dprintf(CRITICAL, "ERROR: Cannot update chosen node [linux,initrd-end]\n");
			return ret;
		}
	}

#if ENABLE_BOOTDEVICE_MOUNT || DYNAMIC_PARTITION_SUPPORT
	/* Update fstab node */
	dprintf(SPEW, "Start of fstab node update:%zu ms\n", platform_get_sclk_count());
	if (update_fstab_node(fdt) != 0) {
		dprintf(CRITICAL, "ERROR: Cannot update fstab node\n");
		return ret;
	}
	dprintf(SPEW, "End of fstab node update:%zu ms\n", platform_get_sclk_count());
#endif

	ret = call_dt_update_handlers(fdt, cmdline, boot_type);
	if (ret)
		return ret;

	fdt_pack(fdt);

#if ENABLE_PARTIAL_GOODS_SUPPORT
	update_partial_goods_dtb_nodes(fdt);
#endif

	return ret;
}

#if ENABLE_BOOTDEVICE_MOUNT || DYNAMIC_PARTITION_SUPPORT
/*Update device tree for fstab node */
static int update_fstab_node(void *fdt)
{
	int ret = 0;
	int str_len = 0;
	int parent_offset = 0;
	int subnode_offset = 0;
	int prop_length = 0;
	int prefix_string_len = 0;
	char *node_name = NULL;
	char *boot_dev_buf = NULL;
	char *new_str = NULL;
	char *prefix_str = NULL;
	char *suffix_str = NULL;
	const struct fdt_property *prop = NULL;
	struct fstab_node table = target_dynamic_partition_supported() ?
						dynamic_fstab_table :
						fstab_table;

	/* Find the parent node */
	parent_offset = fdt_path_offset(fdt, table.parent_node);
	if (parent_offset < 0) {
		dprintf(CRITICAL, "Failed to get parent node: fstab error: %d\n", parent_offset);
		return -1;
	}
	dprintf(SPEW, "Node: %s found.\n", fdt_get_name(fdt, parent_offset, NULL));

	if (!target_dynamic_partition_supported())
	{
		/* Get boot device type */
		boot_dev_buf = (char *) malloc(sizeof(char) * BOOT_DEV_MAX_LEN);
		if (!boot_dev_buf) {
			dprintf(CRITICAL, "Failed to allocate memory for boot device\n");
			return -1;
		}

		new_str = (char *) malloc(sizeof(char) * NODE_PROPERTY_MAX_LEN);
		if (!new_str) {
			dprintf(CRITICAL, "Failed to allocate memory for node property string\n");
			return -1;
		}

		platform_boot_dev_cmdline(boot_dev_buf, sizeof(char) * BOOT_DEV_MAX_LEN);
	}

	/* Get properties of all sub nodes */
	for (subnode_offset = fdt_first_subnode(fdt, parent_offset);
		subnode_offset >= 0;
		subnode_offset = fdt_next_subnode(fdt, subnode_offset)) {
		prop = fdt_get_property(fdt, subnode_offset, table.node_prop, &prop_length);
		node_name = (char *)(uintptr_t)fdt_get_name(fdt, subnode_offset, NULL);
		if (!prop) {
			dprintf(CRITICAL, "Property:%s is not found for sub node:%s\n", table.node_prop, node_name);
		} else {
			dprintf(CRITICAL, "Property:%s found for sub node:%s\tproperty:%s\n", table.node_prop, node_name, prop->data);

			/* For Dynamic partition support disable firmware fstab nodes. */
			if (target_dynamic_partition_supported())
			{
				dprintf (INFO, "Disabling node status :%s\n", node_name);
				ret = fdt_setprop (fdt, subnode_offset, table.node_prop, "disabled",
						(strlen("disabled") + 1));
				if (ret)
				{
					dprintf(CRITICAL, "ERROR: Failed to disable Node: %s\n", node_name);
				}
				continue;
			}

			/* Pointer to fdt 'dev' property string that needs to update based on the 'androidboot.bootdevice' */
			memset(new_str, 0, NODE_PROPERTY_MAX_LEN);
			prefix_str = (char *)prop->data;
			if (strlen(prefix_str) > NODE_PROPERTY_MAX_LEN) {
				dprintf(CRITICAL, "Property string length is greater than node property max length\n");
				continue;
			}
			suffix_str = strstr(prefix_str, table.device_path_id);
			if (!suffix_str) {
				dprintf(CRITICAL, "Property is not proper to update\n");
				continue;
			}
			suffix_str += strlen(table.device_path_id);
			prefix_string_len = strlen(prefix_str) - (strlen(suffix_str) - 1);
			suffix_str = strstr((suffix_str + 1), "/");
			str_len = strlcpy(new_str, prefix_str, prefix_string_len);
			if (!str_len) {
				dprintf(CRITICAL, "Property length is not proper to update\n");
				continue;
			}
			str_len = strlcat(new_str, boot_dev_buf, strlen(prefix_str));
			if (!str_len) {
				dprintf(CRITICAL, "Property length is not proper to update\n");
				continue;
			}
			str_len = strlcat(new_str, suffix_str, strlen(prefix_str));
			if (!str_len) {
				dprintf(CRITICAL, "Property length  is not proper to update\n");
				continue;
			}
			/* Update the new property in the memory */
			memscpy(prefix_str, strlen(prefix_str), new_str, strlen(new_str) + 1);
			/* Update the property with new value */
			ret = fdt_setprop(fdt, subnode_offset, table.node_prop, (const void *)prefix_str, strlen(prefix_str) + 1);
			if(ret) {
				dprintf(CRITICAL, "Failed to update the node with new property\n");
				continue;
			}
			dprintf(CRITICAL, "Updated %s with new property %s\n", node_name, prop->data);
		}
	}
	if (boot_dev_buf)
	        free(boot_dev_buf);
	if (new_str)
		free(new_str);
        return ret;
}
#endif
