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
#include <lk2nd-device.h>

struct dt_entry_v1
{
	uint32_t platform_id;
	uint32_t variant_id;
	uint32_t soc_rev;
	uint32_t offset;
	uint32_t size;
};

static struct dt_mem_node_info mem_node;
static int platform_dt_absolute_match(struct dt_entry *cur_dt_entry, struct dt_entry_node *dt_list);
static struct dt_entry *platform_dt_match_best(struct dt_entry_node *dt_list);
static int update_dtb_entry_node(struct dt_entry_node *dt_list, uint32_t dtb_info);
extern int target_is_emmc_boot(void);
extern uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset);
/* TODO: This function needs to be moved to target layer to check violations
 * against all the other regions as well.
 */
extern int check_aboot_addr_range_overlap(uintptr_t start, uint32_t size);

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

static int dev_tree_compatible(void *dtb, uint32_t dtb_size, struct dt_entry_node *dtb_list)
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
			cur_dt_entry->offset = (uint32_t)dtb;
			cur_dt_entry->size = dtb_size;

			dprintf(SPEW, "Found an appended flattened device tree (%s - %u %u 0x%x)\n",
				*model ? model : "unknown",
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
		for (i = 0; i < msm_data_count; i++) {
			for (j = 0; j < board_data_count; j++) {
				if (dtb_ver == DEV_TREE_VERSION_V3 && pmic_prop) {
					for (n = 0; n < pmic_data_count; n++) {
						dt_entry_array[k].platform_id = platform_data[i].platform_id;
						dt_entry_array[k].soc_rev = platform_data[i].soc_rev;
						dt_entry_array[k].variant_id = board_data[j].variant_id;
						dt_entry_array[k].board_hw_subtype = board_data[j].platform_subtype;
						dt_entry_array[k].pmic_rev[0]= pmic_data[n].pmic_version[0];
						dt_entry_array[k].pmic_rev[1]= pmic_data[n].pmic_version[1];
						dt_entry_array[k].pmic_rev[2]= pmic_data[n].pmic_version[2];
						dt_entry_array[k].pmic_rev[3]= pmic_data[n].pmic_version[3];
						dt_entry_array[k].offset = (uint32_t)dtb;
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
					dt_entry_array[k].offset = (uint32_t)dtb;
					dt_entry_array[k].size = dtb_size;
					k++;
				}
			}
		}

		for (i=0 ;i < num_entries; i++) {
			dprintf(SPEW, "Found an appended flattened device tree (%s - %u %u %u 0x%x)\n",
				*model ? model : "unknown",
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

/*
 * Will relocate the DTB to the tags addr if the device tree is found and return
 * its address
 *
 * Arguments:    kernel - Start address of the kernel loaded in RAM
 *               tags - Start address of the tags loaded in RAM
 *               kernel_size - Size of the kernel in bytes
 *
 * Return Value: DTB address : If appended device tree is found
 *               'NULL'         : Otherwise
 */
void *dev_tree_appended(void *kernel, uint32_t kernel_size, uint32_t dtb_offset, void *tags)
{
	void *kernel_end = kernel + kernel_size;
	uint32_t app_dtb_offset = 0;
	void *dtb = NULL;
	void *bestmatch_tag = NULL;
	struct dt_entry *best_match_dt_entry = NULL;
	uint32_t bestmatch_tag_size;
	struct dt_entry_node *dt_entry_queue = NULL;
	struct dt_entry_node *dt_node_tmp1 = NULL;
	struct dt_entry_node *dt_node_tmp2 = NULL;
	unsigned dtb_count = 0;

	/* Initialize the dtb entry node*/
	dt_entry_queue = (struct dt_entry_node *)
				malloc(sizeof(struct dt_entry_node));

	if (!dt_entry_queue) {
		dprintf(CRITICAL, "Out of memory\n");
		return NULL;
	}
	list_initialize(&dt_entry_queue->node);

	if (dtb_offset)
		app_dtb_offset = dtb_offset;
	else
		memcpy((void*) &app_dtb_offset, (void*) (kernel + DTB_OFFSET), sizeof(uint32_t));

	if (((uintptr_t)kernel + (uintptr_t)app_dtb_offset) < (uintptr_t)kernel) {
		return NULL;
	}
	dtb = kernel + app_dtb_offset;
	while (((uintptr_t)dtb + sizeof(struct fdt_header)) < (uintptr_t)kernel_end) {
		struct fdt_header dtb_hdr;
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

		if (check_aboot_addr_range_overlap((uintptr_t)tags, dtb_size)) {
			dprintf(CRITICAL, "Tags addresses overlap with aboot addresses.\n");
			return NULL;
		}

		dev_tree_compatible(dtb, dtb_size, dt_entry_queue);

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
		dprintf(INFO, "Best match DTB tags %u/%08x/0x%08x/%x/%x/%x/%x/%x/%x/%x\n",
			best_match_dt_entry->platform_id, best_match_dt_entry->variant_id,
			best_match_dt_entry->board_hw_subtype, best_match_dt_entry->soc_rev,
			best_match_dt_entry->pmic_rev[0], best_match_dt_entry->pmic_rev[1],
			best_match_dt_entry->pmic_rev[2], best_match_dt_entry->pmic_rev[3],
			best_match_dt_entry->offset, best_match_dt_entry->size);
		dprintf(INFO, "Using pmic info 0x%0x/0x%x/0x%x/0x%0x for device 0x%0x/0x%x/0x%x/0x%0x\n",
			best_match_dt_entry->pmic_rev[0], best_match_dt_entry->pmic_rev[1],
			best_match_dt_entry->pmic_rev[2], best_match_dt_entry->pmic_rev[3],
			board_pmic_target(0), board_pmic_target(1),
			board_pmic_target(2), board_pmic_target(3));
	}
	/* free queue's memory */
	list_for_every_entry(&dt_entry_queue->node, dt_node_tmp1, dt_node, node) {
		dt_node_tmp2 = dt_node_tmp1->node.prev;
		dt_entry_list_delete(dt_node_tmp1);
		dt_node_tmp1 = dt_node_tmp2;
	}

	if(bestmatch_tag) {
		memcpy(tags, bestmatch_tag, bestmatch_tag_size);
		/* clear out the old DTB magic so kernel doesn't find it */
		*((uint32_t *)(kernel + app_dtb_offset)) = 0;
		return tags;
	}

	dprintf(CRITICAL, "DTB offset is incorrect, kernel image does not have appended DTB\n");

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
		dt_entry_size = sizeof(struct dt_entry);
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

			dt_node_tmp2 = dt_node_tmp1->node.prev;
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

			dt_node_tmp2 = dt_node_tmp1->node.prev;
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

			dt_node_tmp2 = dt_node_tmp1->node.prev;
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

			table_ptr += sizeof(struct dt_entry);
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
		dt_node_tmp2 = dt_node_tmp1->node.prev;
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

/* Top level function that updates the device tree. */
int update_device_tree(void *fdt, const char *cmdline,
					   void *ramdisk, uint32_t ramdisk_size, unsigned char* mac)
{
	int ret = 0;
	uint32_t offset;

	/* Check the device tree header */
	ret = fdt_check_header(fdt) || fdt_check_header_ext(fdt);
	if (ret)
	{
		dprintf(CRITICAL, "Invalid device tree header \n");
		return ret;
	}

	if (check_aboot_addr_range_overlap((uint32_t)fdt,
				(fdt_totalsize(fdt) + DTB_PAD_SIZE))) {
		dprintf(CRITICAL, "Error: Fdt addresses overlap with aboot addresses.\n");
		return ret;
	}

	/* Add padding to make space for new nodes and properties. */
	ret = fdt_open_into(fdt, fdt, fdt_totalsize(fdt) + DTB_PAD_SIZE);
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
		/* Adding the cmdline to the chosen node */
		ret = fdt_appendprop_string(fdt, offset, (const char*)"bootargs", (const void*)cmdline);
		if (ret)
		{
			dprintf(CRITICAL, "ERROR: Cannot update chosen node [bootargs]\n");
			return ret;
		}
	}

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

	/* make sure local-mac-address is set for WCN device */
	offset = fdt_node_offset_by_compatible(fdt, -1, "qcom,wcnss-wlan");

	if (mac != NULL && offset != -FDT_ERR_NOTFOUND)
	{
		if (fdt_getprop(fdt, offset, "local-mac-address", NULL) == NULL)
		{
			dprintf(INFO, "Setting WLAN mac address in DT: %02X:%02X:%02X:%02X:%02X:%02X\n",
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			ret = fdt_setprop(fdt, offset, "local-mac-address", mac, 6);
			if (ret)
			{
				dprintf(CRITICAL, "ERROR: cannot set local-mac-address for \"qcom,wcnss-wlan\"\n");
				return ret;
			}
		}
	}

	/* make sure local-bd-address (and legacy local-mac-address) is set for WCN BT device */
	offset = fdt_node_offset_by_compatible(fdt, -1, "qcom,wcnss-bt");

	if (offset != -FDT_ERR_NOTFOUND)
	{
		if (fdt_getprop(fdt, offset, "local-bd-address", NULL) == NULL)
		{
			unsigned char bdaddr[6];
			unsigned int i;

			/* local-bd-addr is expected with least significant bytes
			 * first (little endian format), reverse mac address.
			 */
			for (i = 0; i < sizeof(bdaddr); i++)
				bdaddr[i] = mac[sizeof(bdaddr) - 1 - i];

			/* The BD address is same as WLAN MAC address but with
			 * least significant bit flipped.
			 */
			bdaddr[0] ^= 0x01;

			dprintf(INFO, "Setting Bluetooth BD address in DT:"
				" %02X:%02X:%02X:%02X:%02X:%02X\n",
				bdaddr[5], bdaddr[4], bdaddr[3],
				bdaddr[2], bdaddr[1], bdaddr[0]);

			ret = fdt_setprop(fdt, offset, "local-bd-address", bdaddr, 6);
			if (ret) {
				dprintf(CRITICAL, "ERROR: cannot set local-bd-address for \"qcom,wcnss-bt\"\n");
				return ret;
			}
		}

		/* Legacy (deprecated) local-mac-address */
		if (fdt_getprop(fdt, offset, "local-mac-address", NULL) == NULL)
		{
			/* The BT MAC address is same as WLAN MAC address but with last bit flipped */
			mac[5] = (mac[5] & 0xFE) | ((mac[5] ^ 0x1) & 0x1);

			dprintf(INFO, "Setting BT mac address in DT: %02X:%02X:%02X:%02X:%02X:%02X\n",
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
			ret = fdt_setprop(fdt, offset, "local-mac-address", mac, 6);
			if (ret)
			{
				dprintf(CRITICAL, "ERROR: cannot set local-mac-address for \"qcom,wcnss-bt\"\n");
				return ret;
			}
		}
	}

	lk2nd_update_device_tree(fdt, cmdline);
	fdt_pack(fdt);

	return ret;
}


int dev_tree_check_header(const void *fdt)
{
	return fdt_check_header(fdt) || fdt_check_header_ext(fdt);
}

const char *dev_tree_get_boot_args(const void *fdt)
{
	int offset, len;
	const char *prop;
	char *bootargs = NULL;

	offset = fdt_path_offset(fdt, "/chosen");
	if (offset < 0) {
		dprintf(INFO, "Could not find chosen node in device tree: %d\n", offset);
		return NULL;
	}

	prop = fdt_getprop(fdt, offset, "bootargs", &len);
	if (prop && len > 0) {
		bootargs = (char*) malloc(sizeof(char) * len);
		ASSERT(bootargs);
		strlcpy(bootargs, prop, len);
	} else {
		dprintf(INFO, "Boot arguments do not exist in device tree\n");
	}
	return bootargs;
}

int dev_tree_get_board_id(const void *fdt, int offset, struct board_id *board_id)
{
	int len;
	const struct board_id *board_prop;

	if (!offset)
		offset = fdt_path_offset(fdt, "/");
	if (offset < 0) {
		dprintf(INFO, "Could not find node in device tree: %d\n", offset);
		return 1;
	}

	board_prop = (const struct board_id*) fdt_getprop(fdt, offset, "qcom,board-id", &len);
	if (!board_prop || len < BOARD_ID_SIZE)
		return 1;

	board_id->variant_id = fdt32_to_cpu(board_prop->variant_id);
	board_id->platform_subtype = fdt32_to_cpu(board_prop->platform_subtype);
	return 0;
}
