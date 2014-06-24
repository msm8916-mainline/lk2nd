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

struct dt_entry_v1
{
	uint32_t platform_id;
	uint32_t variant_id;
	uint32_t soc_rev;
	uint32_t offset;
	uint32_t size;
};

static struct dt_mem_node_info mem_node;

static int platform_dt_match(struct dt_entry *cur_dt_entry, uint32_t target_variant_id, uint32_t subtype_mask);
extern int target_is_emmc_boot(void);
extern uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset);
/* TODO: This function needs to be moved to target layer to check violations
 * against all the other regions as well.
 */
extern int check_aboot_addr_range_overlap(uint32_t start, uint32_t size);

/* Returns soc version if platform id and hardware id matches
   otherwise return 0xFFFFFFFF */
#define INVALID_SOC_REV_ID 0XFFFFFFFF
static uint32_t dev_tree_compatible(void *dtb)
{
	int root_offset;
	const void *prop = NULL;
	const char *plat_prop = NULL;
	const char *board_prop = NULL;
	char *model = NULL;
	struct dt_entry cur_dt_entry;
	struct dt_entry *dt_entry_v2 = NULL;
	struct board_id *board_data = NULL;
	struct plat_id *platform_data = NULL;
	int len;
	int len_board_id;
	int len_plat_id;
	int min_plat_id_len = 0;
	uint32_t target_variant_id;
	uint32_t dtb_ver;
	uint32_t num_entries = 0;
	uint32_t i, j, k;
	uint32_t found = 0;
	uint32_t msm_data_count;
	uint32_t board_data_count;
	uint32_t soc_rev;

	root_offset = fdt_path_offset(dtb, "/");
	if (root_offset < 0)
		return false;

	prop = fdt_getprop(dtb, root_offset, "model", &len);
	if (prop && len > 0) {
		model = (char *) malloc(sizeof(char) * len);
		ASSERT(model);
		strlcpy(model, prop, len);
	} else {
		model[0] = '\0';
	}

	/* Find the board-id prop from DTB , if board-id is present then
	 * the DTB is version 2 */
	board_prop = (const char *)fdt_getprop(dtb, root_offset, "qcom,board-id", &len_board_id);
	if (board_prop)
	{
		dtb_ver = DEV_TREE_VERSION_V2;
		min_plat_id_len = PLAT_ID_SIZE;
	}
	else
	{
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
	if (dtb_ver == DEV_TREE_VERSION_V1)
	{
		while (len_plat_id)
		{
			cur_dt_entry.platform_id = fdt32_to_cpu(((const struct dt_entry_v1 *)plat_prop)->platform_id);
			cur_dt_entry.variant_id = fdt32_to_cpu(((const struct dt_entry_v1 *)plat_prop)->variant_id);
			cur_dt_entry.soc_rev = fdt32_to_cpu(((const struct dt_entry_v1 *)plat_prop)->soc_rev);
			cur_dt_entry.board_hw_subtype = board_hardware_subtype();

			target_variant_id = board_hardware_id();

			dprintf(SPEW, "Found an appended flattened device tree (%s - %u %u 0x%x)\n",
				*model ? model : "unknown",
				cur_dt_entry.platform_id, cur_dt_entry.variant_id, cur_dt_entry.soc_rev);

			if (platform_dt_match(&cur_dt_entry, target_variant_id, 0) == 1)
			{
				dprintf(SPEW, "Device tree's msm_id doesn't match the board: <%u %u 0x%x> != <%u %u 0x%x>\n",
							  cur_dt_entry.platform_id,
							  cur_dt_entry.variant_id,
							  cur_dt_entry.soc_rev,
							  board_platform_id(),
							  board_hardware_id(),
							  board_soc_version());
				plat_prop += DT_ENTRY_V1_SIZE;
				len_plat_id -= DT_ENTRY_V1_SIZE;
				continue;
			}
			else
			{
				found = 1;
				break;
			}
		}
	}
	/*
	 * If DTB Version is '2' then we have split DTB with board & msm data
	 * populated saperately in board-id & msm-id prop respectively.
	 * Extract the data & prepare a look up table
	 */
	else if (dtb_ver == DEV_TREE_VERSION_V2)
	{
		board_data_count = (len_board_id / BOARD_ID_SIZE);
		msm_data_count = (len_plat_id / PLAT_ID_SIZE);

		/* If we are using dtb v2.0, then we have split board & msm data in the DTB */
		board_data = (struct board_id *) malloc(sizeof(struct board_id) * (len_board_id / BOARD_ID_SIZE));
		ASSERT(board_data);
		platform_data = (struct plat_id *) malloc(sizeof(struct plat_id) * (len_plat_id / PLAT_ID_SIZE));
		ASSERT(platform_data);
		i = 0;

		/* Extract board data from DTB */
		for(i = 0 ; i < board_data_count; i++)
		{
			board_data[i].variant_id = fdt32_to_cpu(((struct board_id *)board_prop)->variant_id);
			board_data[i].platform_subtype = fdt32_to_cpu(((struct board_id *)board_prop)->platform_subtype);
			len_board_id -= sizeof(struct board_id);
			board_prop += sizeof(struct board_id);
		}

		/* Extract platform data from DTB */
		for(i = 0 ; i < msm_data_count; i++)
		{
			platform_data[i].platform_id = fdt32_to_cpu(((struct plat_id *)plat_prop)->platform_id);
			platform_data[i].soc_rev = fdt32_to_cpu(((struct plat_id *)plat_prop)->soc_rev);
			len_plat_id -= sizeof(struct plat_id);
			plat_prop += sizeof(struct plat_id);
		}

		/* We need to merge board & platform data into dt entry structure */
		num_entries = msm_data_count * board_data_count;
		dt_entry_v2 = (struct dt_entry*) malloc(sizeof(struct dt_entry) * num_entries);
		ASSERT(dt_entry_v2);

		/* If we have '<X>; <Y>; <Z>' as platform data & '<A>; <B>; <C>' as board data.
		 * Then dt entry should look like
		 * <X ,A >;<X, B>;<X, C>;
		 * <Y ,A >;<Y, B>;<Y, C>;
		 * <Z ,A >;<Z, B>;<Z, C>;
		 */
		i = 0;
		k = 0;
		for (i = 0; i < msm_data_count; i++)
		{
			for (j = 0; j < board_data_count; j++)
			{
				dt_entry_v2[k].platform_id = platform_data[i].platform_id;
				dt_entry_v2[k].soc_rev = platform_data[i].soc_rev;
				dt_entry_v2[k].variant_id = board_data[j].variant_id;
				dt_entry_v2[k].board_hw_subtype = board_data[j].platform_subtype;
				k++;
			}
		}

		/* Now find the matching entry in the merged list */
		if (board_hardware_id() == HW_PLATFORM_QRD)
			target_variant_id = board_target_id();
		else
			target_variant_id = board_hardware_id() | ((board_hardware_subtype() & 0xff) << 24);

		for (i=0 ;i < num_entries; i++)
		{
			dprintf(SPEW, "Found an appended flattened device tree (%s - %u %u %u 0x%x)\n",
				*model ? model : "unknown",
				dt_entry_v2[i].platform_id, dt_entry_v2[i].variant_id, dt_entry_v2[i].board_hw_subtype, dt_entry_v2[i].soc_rev);

			if (platform_dt_match(&dt_entry_v2[i], target_variant_id, 0xff) == 1)
			{
				dprintf(SPEW, "Device tree's msm_id doesn't match the board: <%u %u %u 0x%x> != <%u %u %u 0x%x>\n",
							  dt_entry_v2[i].platform_id,
							  dt_entry_v2[i].variant_id,
							  dt_entry_v2[i].soc_rev,
							  dt_entry_v2[i].board_hw_subtype,
							  board_platform_id(),
							  board_hardware_id(),
							  board_hardware_subtype(),
							  board_soc_version());
				continue;
			}
			else
			{
				/* If found a match, return the cur_dt_entry */
				found = 1;
				cur_dt_entry = dt_entry_v2[i];
				break;
			}
		}
	}

	if (!found)
	{
		soc_rev =  INVALID_SOC_REV_ID;
		goto end;
	}
	else
		soc_rev = cur_dt_entry.soc_rev;

	dprintf(INFO, "Device tree's msm_id matches the board: <%u %u %u 0x%x> == <%u %u %u 0x%x>\n",
		cur_dt_entry.platform_id,
		cur_dt_entry.variant_id,
		cur_dt_entry.board_hw_subtype,
		cur_dt_entry.soc_rev,
		board_platform_id(),
		board_hardware_id(),
		board_hardware_subtype(),
		board_soc_version());

end:
	free(board_data);
	free(platform_data);
	free(dt_entry_v2);
	free(model);

	return soc_rev;
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
void *dev_tree_appended(void *kernel, uint32_t kernel_size, void *tags)
{
	void *kernel_end = kernel + kernel_size;
	uint32_t app_dtb_offset = 0;
	void *dtb;
	void *bestmatch_tag = NULL;
	uint32_t bestmatch_tag_size;
	uint32_t bestmatch_soc_rev_id = INVALID_SOC_REV_ID;

	memcpy((void*) &app_dtb_offset, (void*) (kernel + DTB_OFFSET), sizeof(uint32_t));

	dtb = kernel + app_dtb_offset;
	while (dtb + sizeof(struct fdt_header) < kernel_end) {
		uint32_t dtb_soc_rev_id;
		struct fdt_header dtb_hdr;
		uint32_t dtb_size;

		/* the DTB could be unaligned, so extract the header,
		 * and operate on it separately */
		memcpy(&dtb_hdr, dtb, sizeof(struct fdt_header));
		if (fdt_check_header((const void *)&dtb_hdr) != 0 ||
		    (dtb + fdt_totalsize((const void *)&dtb_hdr) > kernel_end))
			break;
		dtb_size = fdt_totalsize(&dtb_hdr);

		/* now that we know we have a valid DTB, we need to copy
		 * it somewhere aligned, like tags */
		memcpy(tags, dtb, dtb_size);

		dtb_soc_rev_id = dev_tree_compatible(tags);
		if (dtb_soc_rev_id == board_soc_version()) {
			/* clear out the old DTB magic so kernel doesn't find it */
			*((uint32_t *)(kernel + app_dtb_offset)) = 0;
			return tags;
		} else if ((dtb_soc_rev_id != INVALID_SOC_REV_ID) &&
				   (dtb_soc_rev_id < board_soc_version())) {
			/* if current bestmatch is less than new dtb_soc_rev_id then update
			   bestmatch_tag */
			if((bestmatch_soc_rev_id == INVALID_SOC_REV_ID) ||
			   (bestmatch_soc_rev_id < dtb_soc_rev_id)) {
				bestmatch_tag = dtb;
				bestmatch_tag_size = dtb_size;
				bestmatch_soc_rev_id = dtb_soc_rev_id;
			}
		}

		/* goto the next device tree if any */
		dtb += dtb_size;
	}

	if(bestmatch_tag) {
		dprintf(INFO,"DTB found with bestmatch soc rev id 0x%x.Board soc rev id 0x%x\n",
				bestmatch_soc_rev_id, board_soc_version());
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

static int platform_dt_match(struct dt_entry *cur_dt_entry, uint32_t target_variant_id, uint32_t subtype_mask)
{
	/*
	 * 1. Check if cur_dt_entry has platform_hw_version major & minor present?
	 * 2. If present, calculate cur_dt_target_id for the current platform as:
	 * 3. bit no |31  24 | 23   16| 15   8 |7         0|
	 * 4.        |subtype| major  | minor  |hw_platform|
	 */
	uint32_t cur_dt_target_id ;
	uint32_t cur_dt_hlos_subtype;

	/*
	 * if variant_id has platform_hw_ver has major = 0xff and minor = 0xff,
	 * ignore the major & minor versions from the DTB entry
	 */
	if ((cur_dt_entry->variant_id & 0xffff00) == 0xffff00)
		cur_dt_target_id  = (cur_dt_entry->variant_id & 0xff0000ff) | (target_variant_id & 0xffff00);
	/*
	 * We have a valid platform_hw_version major & minor numbers in the board-id, so
	 * use the board-id from the DTB.
	 * Note: For some QRD platforms the format used is qcom, board-id = <0xMVmVPT 0xPS>
	 * where: MV: platform major ver, mV: platform minor ver, PT: platform type
	 * PS: platform subtype, so we need to put PS @ bit 24-31 to be backward compatible.
	 */
	else
		cur_dt_target_id = cur_dt_entry->variant_id | ((cur_dt_entry->board_hw_subtype & subtype_mask & 0xff) << 24);
	/* Determine the bits 23:8 to check the DT with the DDR Size */
	cur_dt_hlos_subtype = (cur_dt_entry->board_hw_subtype & 0xffff00);

	/* 1. must match the platform_id, platform_hw_id, platform_version
	*  2. soc rev number equal then return 0
	*  3. dt soc rev number less than cdt return -1
	*  4. otherwise return 1
	*/

	if((cur_dt_entry->platform_id == board_platform_id()) &&
		(cur_dt_target_id == target_variant_id) &&
		(cur_dt_hlos_subtype == target_get_hlos_subtype())) {

		if(cur_dt_entry->soc_rev == board_soc_version()) {
			return 0;
		} else if(cur_dt_entry->soc_rev < board_soc_version()) {
			return -1;
		}
	}

	return 1;
}

static int __dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info,
										uint32_t target_variant_id, uint32_t subtype_mask)
{
	uint32_t i;
	unsigned char *table_ptr;
	struct dt_entry dt_entry_buf_1;
	struct dt_entry dt_entry_buf_2;
	struct dt_entry *cur_dt_entry;
	struct dt_entry *best_match_dt_entry;
	struct dt_entry_v1 *dt_entry_v1;
	uint32_t found = 0;

	if (!dt_entry_info) {
		dprintf(CRITICAL, "ERROR: Bad parameter passed to %s \n",
				__func__);
		return -1;
	}

	table_ptr = (unsigned char *)table + DEV_TREE_HEADER_SIZE;
	cur_dt_entry = &dt_entry_buf_1;
	best_match_dt_entry = NULL;

	for(i = 0; found == 0 && i < table->num_entries; i++)
	{
		memset(cur_dt_entry, 0, sizeof(struct dt_entry));
		switch(table->version) {
		case DEV_TREE_VERSION_V1:
			dt_entry_v1 = (struct dt_entry_v1 *)table_ptr;
			cur_dt_entry->platform_id = dt_entry_v1->platform_id;
			cur_dt_entry->variant_id = dt_entry_v1->variant_id;
			cur_dt_entry->soc_rev = dt_entry_v1->soc_rev;
			cur_dt_entry->board_hw_subtype = board_hardware_subtype();
			cur_dt_entry->offset = dt_entry_v1->offset;
			cur_dt_entry->size = dt_entry_v1->size;
			table_ptr += sizeof(struct dt_entry_v1);
			break;
		case DEV_TREE_VERSION_V2:
			memcpy(cur_dt_entry, (struct dt_entry *)table_ptr,
				   sizeof(struct dt_entry));
			table_ptr += sizeof(struct dt_entry);
			break;
		default:
			dprintf(CRITICAL, "ERROR: Unsupported version (%d) in DT table \n",
					table->version);
			return -1;
		}

		/* DTBs are stored in the ascending order of soc revision.
		 * For eg: Rev0..Rev1..Rev2 & so on.
		 * we pickup the DTB with highest soc rev number which is less
		 * than or equal to actual hardware
		 */
		switch(platform_dt_match(cur_dt_entry, target_variant_id, subtype_mask)) {
		case 0:
			best_match_dt_entry = cur_dt_entry;
			found = 1;
			break;
		case -1:
			if (!best_match_dt_entry) {
				/* copy structure */
				best_match_dt_entry = cur_dt_entry;
				cur_dt_entry = &dt_entry_buf_2;
			} else {
				/* Swap dt_entry buffers */
				struct dt_entry *temp = cur_dt_entry;
				cur_dt_entry = best_match_dt_entry;
				best_match_dt_entry = temp;
			}
		default:
			break;
		}
	}

	if (best_match_dt_entry) {
		*dt_entry_info = *best_match_dt_entry;
		found = 1;
	}

	if (found != 0) {
		dprintf(INFO, "Using DTB entry %u/%08x/0x%08x/%u for device %u/%08x/0x%08x/%u\n",
				dt_entry_info->platform_id, dt_entry_info->soc_rev,
				dt_entry_info->variant_id, dt_entry_info->board_hw_subtype,
				board_platform_id(), board_soc_version(),
				board_target_id(), board_hardware_subtype());
		return 0;
	}

	dprintf(CRITICAL, "ERROR: Unable to find suitable device tree for device (%u/0x%08x/0x%08x/%u)\n",
			board_platform_id(), board_soc_version(),
			board_target_id(), board_hardware_subtype());
	return -1;
}

/* Function to obtain the index information for the correct device tree
 *  based on the platform data.
 *  If a matching device tree is found, the information is returned in the
 *  "dt_entry_info" out parameter and a function value of 0 is returned, otherwise
 *  a non-zero function value is returned.
 */
int dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info)
{
	uint32_t target_variant_id;

	target_variant_id = board_target_id();
	if (__dev_tree_get_entry_info(table, dt_entry_info, target_variant_id, 0xff) == 0) {
		return 0;
	}

	/*
	 * for compatible with version 1 and version 2 dtbtool
	 * will compare the subtype inside the variant id
	 */
	target_variant_id = board_hardware_id() | ((board_hardware_subtype() & 0xff) << 24);
	if (__dev_tree_get_entry_info(table, dt_entry_info, target_variant_id, 0xff) == 0) {
		return 0;
	}

	/*
	* add compatible with old device selection method which don't compare subtype
	*/
	target_variant_id = board_hardware_id();
	return __dev_tree_get_entry_info(table, dt_entry_info, target_variant_id, 0);
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
	if (len <= 0)
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
	if (len <= 0)
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
					   void *ramdisk, uint32_t ramdisk_size)
{
	int ret = 0;
	uint32_t offset;

	/* Check the device tree header */
	ret = fdt_check_header(fdt);
	if (ret)
	{
		dprintf(CRITICAL, "Invalid device tree header \n");
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
	/* Adding the cmdline to the chosen node */
	ret = fdt_setprop_string(fdt, offset, (const char*)"bootargs", (const void*)cmdline);
	if (ret)
	{
		dprintf(CRITICAL, "ERROR: Cannot update chosen node [bootargs]\n");
		return ret;
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

	fdt_pack(fdt);

	return ret;
}

