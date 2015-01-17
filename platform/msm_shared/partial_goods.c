/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <platform/partial_goods.h>
#include <libfdt.h>
#include <reg.h>
#include <debug.h>

void update_partial_goods_dtb_nodes(void *fdt)
{
	int i;
	int tbl_sz = sizeof(table) / sizeof(struct partial_goods);
	int parent_offset = 0;
	int subnode_offset = 0;
	int ret = 0;
	int prop_len = 0;
	uint32_t reg = readl(QFPROM_PTE_PART_ADDR);
	uint32_t prop_type = 0;
	struct subnode_list *subnode_lst = NULL;
	const struct fdt_property *prop = NULL;
	const char *replace_str = NULL;

	/*
	 * The PTE register bits 23 to 27 have the partial goods
	 * info, extract the partial goods value before using
	 */
	reg = (reg & 0x0f800000) >> 23;

	/* If none of the DTB needs update */
	if (!reg)
		return;

	ret = fdt_open_into(fdt, fdt, fdt_totalsize(fdt));
	if (ret != 0)
	{
		dprintf(CRITICAL, "Failed to move/resize dtb buffer: %d\n", ret);
		ASSERT(0);
	}

	for (i = 0; i < tbl_sz; i++)
	{
		if (reg == table[i].val)
		{
			/* Find the Parent node */
			ret = fdt_path_offset(fdt, table[i].parent_node);
			if (ret < 0)
			{
				dprintf(CRITICAL, "Failed to get parent node: %s\terrno:%d\n", table[i].parent_node, ret);
				ASSERT(0);
			}
			parent_offset = ret;

			/* Find the subnode */
			subnode_lst = table[i].subnode;

			while (subnode_lst->subnode)
			{
				ret = fdt_subnode_offset(fdt, parent_offset, subnode_lst->subnode);
				if (ret < 0)
				{
					dprintf(CRITICAL, "Failed to get subnode: %s\terrno:%d\n", subnode_lst->subnode, ret);
					ASSERT(0);
				}
				subnode_offset = ret;

				/* Find the property node and its length */
				prop = fdt_get_property(fdt, subnode_offset, subnode_lst->property, &prop_len);
				if (!prop)
				{
					dprintf(CRITICAL, "Failed to get property: %s\terrno: %d\n", subnode_lst->property, prop_len);
					ASSERT(0);
				}

				/*
				 * Replace the property value based on the property
				 * length and type
				 */
				if (!(strncmp(subnode_lst->property, "device_type", sizeof(subnode_lst->property))))
					prop_type = DEVICE_TYPE;
				else if ((!strncmp(subnode_lst->property, "status", sizeof(subnode_lst->property))))
					prop_type = STATUS_TYPE;
				else
				{
					dprintf(CRITICAL, "%s: Property type is not supported\n", subnode_lst->property);
					ASSERT(0);
				}

				switch (prop_type)
				{
					case DEVICE_TYPE:
						replace_str = "nak";
						break;
					case STATUS_TYPE:
						if (prop_len == sizeof("ok"))
							replace_str = "no";
						else if (prop_len == sizeof("okay"))
							replace_str = "dsbl";
						else
						{
							dprintf(CRITICAL, "Property value length: %u is invalid for property: %s\n", prop_len, subnode_lst->property);
							ASSERT(0);
						}
						break;
					default:
						/* Control would not come here, as this gets taken care while setting property type */
						break;
				};

				/* Replace the property with new value */
				ret = fdt_setprop_inplace(fdt, subnode_offset, subnode_lst->property, (const void *)replace_str, prop_len);
				if (!ret)
					dprintf(INFO, "Updated device tree property: %s @ %s node\n", subnode_lst->property, subnode_lst->subnode);
				else
				{
					dprintf(CRITICAL, "Failed to update property: %s: error no: %d\n", subnode_lst->property, ret);
					ASSERT(0);
				}

				subnode_lst++;
			}
		}
	}

	fdt_pack(fdt);
}
