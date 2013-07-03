/* Copyright (c) 2012-2013, The Linux Foundation. All rights reserved.
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

extern int target_is_emmc_boot(void);
extern uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset);
/* TODO: This function needs to be moved to target layer to check violations
 * against all the other regions as well.
 */
extern int check_aboot_addr_range_overlap(uint32_t start, uint32_t size);

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
void *dev_tree_appended(void *kernel, void *tags, uint32_t kernel_size)
{
	uint32_t app_dtb_offset = 0;
	uint32_t size;

	memcpy((void*) &app_dtb_offset, (void*) (kernel + DTB_OFFSET), sizeof(uint32_t));

	/*
	 * Check if we have valid offset for the DTB, if not return error.
	 * If the kernel image does not have appeneded device tree, DTB offset
	 * might contain some random address which is not accessible & cause
	 * data abort. If kernel start + dtb offset address exceed the total
	 * size of the kernel, then we dont have an appeneded DTB.
	 */
	if (app_dtb_offset < kernel_size)
	{
		if (!fdt_check_header((void*) (kernel + app_dtb_offset)))
		{
			void *dtb;
			int rc;

			dprintf(INFO, "Found Appeneded Flattened Device tree\n");
			dtb = kernel + app_dtb_offset;
			size = fdt_totalsize(dtb);
			if (check_aboot_addr_range_overlap(tags, size))
			{
				dprintf(CRITICAL, "Appended dtb aboot overlap check failed.\n");
				return NULL;
			}
			rc = fdt_open_into(dtb, tags, size);
			if (rc == 0)
			{
				/* clear out the old DTB magic so kernel doesn't find it */
				*((uint32_t *)dtb) = 0;
				return tags;
			}
		}
	}
	else
		dprintf(CRITICAL, "DTB offset is incorrect, kernel image does not have appended DTB\n");

	return NULL;
}

/* Returns 0 if the device tree is valid. */
int dev_tree_validate(struct dt_table *table, unsigned int page_size)
{
	int dt_entry_size;

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

	/* Restriction that the device tree entry table should be less than a page*/
	ASSERT(((table->num_entries * dt_entry_size)+ DEV_TREE_HEADER_SIZE) < page_size);

	return 0;
}

static int __dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info, uint32_t target_variant_id);

/* Function to obtain the index information for the correct device tree
 *  based on the platform data.
 *  If a matching device tree is found, the information is returned in the
 *  "dt_entry_info" out parameter and a function value of 0 is returned, otherwise
 *  a non-zero function value is returned.
 */
int dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info)
{
	uint32_t target_variant_id;

	if(board_hardware_id() == HW_PLATFORM_QRD) {
		target_variant_id = board_target_id();
		if (__dev_tree_get_entry_info(table, dt_entry_info, target_variant_id) == 0) {
			return 0;
		}
	}
	/*
	* for compatible with version 1 and version 2 dtbtool
	* will compare the subtype inside the variant id
	*/
	target_variant_id = board_hardware_id() | ((board_hardware_subtype() & 0xff) << 24);

	return __dev_tree_get_entry_info(table, dt_entry_info, target_variant_id);
}

static int __dev_tree_get_entry_info(struct dt_table *table, struct dt_entry *dt_entry_info, uint32_t target_variant_id)
{
	uint32_t i;
	unsigned char *table_ptr;
	struct dt_entry dt_entry_buf_1;
	struct dt_entry dt_entry_buf_2;
	struct dt_entry *cur_dt_entry;
	struct dt_entry *best_match_dt_entry;
	struct dt_entry_v1 *dt_entry_v1;

	if (!dt_entry_info) {
		dprintf(CRITICAL, "ERROR: Bad parameter passed to %s \n",
				__func__);
		return -1;
	}

	table_ptr = (unsigned char *)table + DEV_TREE_HEADER_SIZE;
	cur_dt_entry = &dt_entry_buf_1;
	best_match_dt_entry = NULL;

	for(i = 0; i < table->num_entries; i++)
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
		if((cur_dt_entry->platform_id == board_platform_id()) &&
		   ((cur_dt_entry->variant_id | ((cur_dt_entry->board_hw_subtype & 0xff) << 24)) == target_variant_id))
		{
			if(cur_dt_entry->soc_rev == board_soc_version()) {
				best_match_dt_entry = cur_dt_entry;
				break;
			} else if (cur_dt_entry->soc_rev < board_soc_version()){
				/* Keep this as the next best candidate. */
				if (!best_match_dt_entry) {
					best_match_dt_entry = cur_dt_entry;
					cur_dt_entry = &dt_entry_buf_2;
				} else {
					/* Swap dt_entry buffers */
					struct dt_entry *temp = cur_dt_entry;
					cur_dt_entry = best_match_dt_entry;
					best_match_dt_entry = temp;
				}
			}
		}
	}

	if (best_match_dt_entry) {
		*dt_entry_info = *best_match_dt_entry;
		dprintf(INFO, "Using DTB entry %u/%08x/%u/%u for device %u/%08x/%u/%u\n",
				dt_entry_info->platform_id, dt_entry_info->soc_rev,
				dt_entry_info->variant_id, dt_entry_info->board_hw_subtype,
				board_platform_id(), board_soc_version(),
				board_hardware_id(), board_hardware_subtype());
		return 0;
	}

	dprintf(CRITICAL, "ERROR: Unable to find suitable device tree for device (%u/0x%08x/%u/%u)\n",
			board_platform_id(), board_soc_version(),
			board_hardware_id(), board_hardware_subtype());
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

/* Function to add the subsequent RAM partition info to the device tree. */
int dev_tree_add_mem_info(void *fdt, uint32_t offset, uint32_t addr, uint32_t size)
{
	static int mem_info_cnt = 0;
	int ret;

	if (!mem_info_cnt)
	{
		/* Replace any other reg prop in the memory node. */
		ret = fdt_setprop_u32(fdt, offset, "reg", addr);
		mem_info_cnt = 1;
	}
	else
	{
		/* Append the mem info to the reg prop for subsequent nodes.  */
		ret = fdt_appendprop_u32(fdt, offset, "reg", addr);
	}

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

	/* Adding the initrd-start to the chosen node */
	ret = fdt_setprop_u32(fdt, offset, "linux,initrd-start", (uint32_t)ramdisk);
	if (ret)
	{
		dprintf(CRITICAL, "ERROR: Cannot update chosen node [linux,initrd-start]\n");
		return ret;
	}

	/* Adding the initrd-end to the chosen node */
	ret = fdt_setprop_u32(fdt, offset, "linux,initrd-end", ((uint32_t)ramdisk + ramdisk_size));
	if (ret)
	{
		dprintf(CRITICAL, "ERROR: Cannot update chosen node [linux,initrd-end]\n");
		return ret;
	}

	fdt_pack(fdt);

	return ret;
}

