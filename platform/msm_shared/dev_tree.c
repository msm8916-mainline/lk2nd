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

extern int target_is_emmc_boot(void);
extern uint32_t target_dev_tree_mem(void *fdt, uint32_t memory_node_offset);

/*
 * Will relocate the DTB to the tags addr if the device tree is found and return
 * its address
 *
 * Arguments:    kernel - Start address of the kernel loaded in RAM
 *               tags - Start address of the tags loaded in RAM
 * Return Value: DTB address : If appended device tree is found
 *               'NULL'         : Otherwise
 */
void *dev_tree_appended(void *kernel, void *tags)
{
	uint32_t app_dtb_offset = 0;
	uint32_t dtb_magic = 0;

	memcpy((void*) &app_dtb_offset, (void*) (kernel + DTB_OFFSET), sizeof(uint32_t));
	memcpy((void*) &dtb_magic, (void*) (kernel + app_dtb_offset), sizeof(uint32_t));

	if (dtb_magic == DTB_MAGIC) {
		void *dtb;
		int rc;

		dprintf(INFO, "Found Appeneded Flattened Device tree\n");
		dtb = kernel + app_dtb_offset;
		rc = fdt_open_into(dtb, tags, fdt_totalsize(dtb) + DTB_PAD_SIZE);
		if (rc == 0) {
			/* clear out the old DTB magic so kernel doesn't find it */
			*((uint32_t *)dtb) = 0;
			return tags;
		}
	}

	return NULL;
}

/* Function to return the pointer to the start of the correct device tree
 *  based on the platform data.
 */
struct dt_entry * dev_tree_get_entry_ptr(struct dt_table *table)
{
	uint32_t i;
	struct dt_entry *dt_entry_ptr;
	struct dt_entry *latest_dt_entry = NULL;

	dt_entry_ptr = (struct dt_entry *)((char *)table + DEV_TREE_HEADER_SIZE);

	for(i = 0; i < table->num_entries; i++)
	{
		/* DTBs are stored in the ascending order of soc revision.
		 * For eg: Rev0..Rev1..Rev2 & so on.
		 * we pickup the DTB with highest soc rev number which is less
		 * than or equal to actual hardware
		 */
		if((dt_entry_ptr->platform_id == board_platform_id()) &&
		   (dt_entry_ptr->variant_id == board_hardware_id()) &&
		   (dt_entry_ptr->soc_rev == board_soc_version()))
			{
				return dt_entry_ptr;
			}
		/* if the exact match not found, return the closest match
		 * assuming it to be the nearest soc version
		 */
		if((dt_entry_ptr->platform_id == board_platform_id()) &&
		  (dt_entry_ptr->variant_id == board_hardware_id()) &&
		  (dt_entry_ptr->soc_rev <= board_soc_version())) {
			latest_dt_entry = dt_entry_ptr;
		}
		dt_entry_ptr++;
	}

	if (latest_dt_entry) {
		dprintf(SPEW, "Loading DTB with SOC version:%x\n", latest_dt_entry->soc_rev);
		return latest_dt_entry;
	}

	return NULL;
}

/* Function to return the offset of flash node - "/qcom,mtd-partitions".
 * The function creates this node if it does not exist.
 */
static uint32_t dev_tree_get_flash_node_offset(void *fdt)
{
	uint32_t offset;
	int ret = DT_OP_SUCCESS;

	/* Find the mtd node. */
	ret = fdt_path_offset(fdt, "/qcom,mtd-partitions");

	if (ret & FDT_ERR_NOTFOUND)
	{
		/* Node not found.
		 * Add node as sub node of root.
		 */
		ret = fdt_path_offset(fdt, "/");

		if (ret < 0)
		{
			dprintf(CRITICAL, "Unable to calculate root offset\n");
			ret = DT_OP_FAILURE;
			goto dev_tree_get_flash_node_offset_err;
		}

		offset = ret;
		/* Add flash partition node. */
		ret = fdt_add_subnode(fdt, offset, "qcom,mtd-partitions");

		if (ret < 0)
		{
			dprintf(CRITICAL, "Unable to add partition node. \n");
			ret = DT_OP_FAILURE;
			goto dev_tree_get_flash_node_offset_err;
		}

		/* Save the offset of the added node. */
		offset = fdt_path_offset(fdt, "/qcom,mtd-partitions");
	}
	else if (ret != 0)
	{
		dprintf(CRITICAL, "Unable to calculate partition node offset\n");
		ret = DT_OP_FAILURE;
		goto dev_tree_get_flash_node_offset_err;
	}

dev_tree_get_flash_node_offset_err:

	return offset;
}

/* Function to add the individual nand partition nodes to the device tree.
 * Pre-condition: The function assumes the presence of
 * "/qcom,mtd-partitions" device node.
 */
static int dev_tree_add_ptable_nodes(void *fdt, uint32_t parent_offset)
{
	struct ptable *ptable;
	int n;
	unsigned char array[100];
	unsigned char *ptn_name_array;
	int dt_ret = DT_OP_SUCCESS;
	int ret;
	int i;
	uint32_t node_offset;
	uint32_t blk_size;

	n = sizeof("partition@");

	/* Allocate bytes to save partition name:
	 * Since address is of uint uint32_t,
	 * allocate twice this size for string
	 * as 1 digit occupies 1 byte is ASCII.
	 */
	ptn_name_array = (unsigned char*) malloc(sizeof(uint32_t) * 2 + n + 1);

	if (ptn_name_array == NULL)
	{
		dprintf(CRITICAL, "Failed to allocate memory for flash partition name\n");
		return -1;
	}

	strcpy((char*)ptn_name_array, (const char*)"partition@");

	/* Add ptable nodes. */
	ptable = flash_get_ptable();
	/* Get block size. */
	blk_size = flash_block_size();

	dprintf(INFO, "Add %d flash partitions to dt: start\n", ptable->count);

	for (i = 0; i < ptable->count; i++)
	{
		/* Add the partition node. */
		if (itoa(ptable->parts[i].start * blk_size, ptn_name_array + n - 1, sizeof(uint32_t) * 2 + 1, 16) < 0)
		{
			dprintf(CRITICAL, "String len exceeded for itoa\n");
			return -1;
		}

		strcpy((char *)array, (const char*)"/qcom,mtd-partitions/");
		strcat((char*)array, (const char*)ptn_name_array);

		ret = fdt_add_subnode(fdt, parent_offset, (const char*)ptn_name_array);

		if (ret < 0)
		{
			dprintf(CRITICAL, "Unable to add partition node: %s.\n",
					ptn_name_array);
			dt_ret = DT_OP_FAILURE;
			goto dev_tree_add_ptable_nodes_err;
		}

		dt_ret = fdt_path_offset(fdt, (const char*)array);

		if (dt_ret < 0)
		{
			dprintf(CRITICAL, "Unable to calculate parition node offset: %s\n",
					ptn_name_array);
			dt_ret = DT_OP_FAILURE;
			goto dev_tree_add_ptable_nodes_err;
		}

		node_offset = dt_ret;

		/* Add the partition name as label. */
		ret = fdt_setprop_string(fdt, node_offset, (const char*)"label", (const char*)ptable->parts[i].name);

		if (ret != 0)
		{
			dprintf(CRITICAL, "Unable to add label property: %s.\n",
					ptable->parts[i].name);
			dt_ret = DT_OP_FAILURE;
			goto dev_tree_add_ptable_nodes_err;
		}

		/* Add the reg values. */
		ret = fdt_setprop_u32(fdt, node_offset, (const char*)"reg", ptable->parts[i].start * blk_size);

		if (ret != 0)
		{
			dprintf(CRITICAL, "Unable to add reg property. %s\n",
					ptable->parts[i].name);
			dt_ret = DT_OP_FAILURE;
			goto dev_tree_add_ptable_nodes_err;
		}

		ret = fdt_appendprop_u32(fdt, node_offset, (const char*)"reg", ptable->parts[i].length * blk_size);

		if (ret != 0)
		{
			dprintf(CRITICAL, "Unable to add reg property. %s\n",
					ptable->parts[i].name);
			dt_ret = DT_OP_FAILURE;
			goto dev_tree_add_ptable_nodes_err;
		}

	}

dev_tree_add_ptable_nodes_err:
	dprintf(INFO, "Add %d flash partitions to dt: done\n", ptable->count);
	free(ptn_name_array);
	return dt_ret;
}

/* Top level function to add flash ptable info to the device tree. */
static int dev_tree_add_flash_ptable(void *fdt)
{
	uint32_t offset;
	int ret;
	int dt_ret = DT_OP_SUCCESS;

	dt_ret = dev_tree_get_flash_node_offset(fdt);

	if (dt_ret < 0)
	{
		dt_ret = DT_OP_FAILURE;
		goto dev_tree_add_flash_ptable_err;
	}

	offset = dt_ret;

	/* Add address and size cell properties. */
	ret = fdt_setprop_u32(fdt, offset, (const char*)"#address-cells", 1);

	if (ret != 0)
	{
		dprintf(CRITICAL, "Unable to add #address-cells property. \n");
		dt_ret = DT_OP_FAILURE;
		goto dev_tree_add_flash_ptable_err;
	}

	ret = fdt_setprop_u32(fdt, offset, (const char*)"#size-cells", 1);

	if (ret != 0)
	{
		dprintf(CRITICAL, "Unable to add #size-cells property. \n");
		dt_ret = DT_OP_FAILURE;
		goto dev_tree_add_flash_ptable_err;
	}

	ret = dev_tree_add_ptable_nodes(fdt, offset);

	if (ret < 0)
	{
		dprintf(CRITICAL, "Unable to add #size-cells property. \n");
		dt_ret = DT_OP_FAILURE;
		goto dev_tree_add_flash_ptable_err;
	}

dev_tree_add_flash_ptable_err:
	return dt_ret;
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

	/* Skip NAND partition nodes for eMMC boot */
	if (!target_is_emmc_boot()){
		dev_tree_add_flash_ptable(fdt);
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
