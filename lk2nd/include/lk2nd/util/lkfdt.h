/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_LKFDT_H
#define LK2ND_UTIL_LKFDT_H

#include <compiler.h>
#include <stdbool.h>
#include <stdint.h>

/**
 * lkfdt_prop_strneq() - Check if property is NOT equal to the specified string.
 * @fdt: Device tree blob
 * @node: Device tree node offset
 * @prop: Property name
 * @cmp: Unexpected property value
 *
 * The function can be used similarly to strcmp().
 *
 * Return:
 * *  0 - if property value is equal to @cmp
 * *  1 - otherwise
 * * <0 - libfdt error (e.g. FDT_ERR_NOTFOUND)
 */
int lkfdt_prop_strneq(const void *fdt, int node, const char *prop, const char *cmp) __PURE;

/**
 * lkfdt_node_is_available() - Check if node is enabled (status = "okay").
 * @fdt: Device tree blob
 * @node: Device tree node offset
 *
 * Return: true if node is available, false otherwise
 */
bool lkfdt_node_is_available(const void *fdt, int node) __PURE;

/**
 * lkfdt_get_reg() - Parse "reg" address and size from device tree node.
 * @fdt: Device tree blob
 * @parent: Parent device tree node offset (should have #address/size-cells)
 * @node: Device tree node offset (should have "reg")
 * @addr: Pointer where address is wrriten to
 * @size: Pointer where size is written to (optional, may be NULL)
 *
 * #address-cells and #size-cells are read from the parent device tree node and
 * used to parse the "reg" of the child node. Note that only 32-bit addresses
 * are supported at the moment. The cells may be larger but all extra cells must
 * be zero. The size pointer may be NULL to parse "reg"s with #size-cells = <0>.
 *
 * Return: 0 if successful, <0 libfdt error
 */
int lkfdt_get_reg(const void *fdt, int parent, int node, uint32_t *addr, uint32_t *size) __PURE;

/**
 * lkfdt_getprop_u32() - Read 32-bit number (e.g. phandle) from property.
 * @fdt: Device tree blob
 * @node: Device tree node offset
 * @prop: Name of property that contains the uint32_t value
 * @val: Output variable that is filled with the decoded uint32_t value
 *
 * The property must contain only a single u32 cell.
 *
 * Return: 0 if successful, <0 libfdt error
 */
int lkfdt_getprop_u32(const void *fdt, int node, const char *prop, uint32_t *val) __PURE;

/**
 * lkfdt_u32list_get() - Read one specific 32-bit number from a list.
 * @fdt: Device tree blob
 * @node: Device tree node offset
 * @prop: Name of property that contains the uint32_t list
 * @idx: Index of uint32_t to return from the list
 * @val: Output variable that is filled with the uint32_t value
 *
 * Return: 0 if successful, <0 libfdt error (e.g. -FDT_ERR_NOTFOUND)
 */
int lkfdt_u32list_get(const void *fdt, int node, const char *prop,
		      int idx, uint32_t *val) __PURE;

/**
 * lkfdt_lookup_phandle() - Read phandle from property and search for the node.
 * @fdt: Device tree blob
 * @node: Device tree node offset
 * @prop: Name of property that contains the phandle
 *
 * Return:
 * * >= 0 - The offset of the phandle node, if successful
 * *  < 0 - libfdt error, otherwise
 */
int lkfdt_lookup_phandle(const void *fdt, int node, const char *prop) __PURE;

/**
 * lkfdt_subnode_offset_by_phandle() - Return subnode that matches a phandle.
 * @fdt: Device tree blob
 * @parent: Device tree node offset of parent
 * @phandle: The phandle to search for
 *
 * Iterate over all subnodes of the "parent" and look for the node that has
 * the specified "phandle".
 *
 * Return:
 * * >= 0 - The offset of the subnode that matches the phandle, if successful
 * *  < 0 - libfdt error, otherwise
 */
int lkfdt_subnode_offset_by_phandle(const void *fdt, int parent, uint32_t phandle) __PURE;

/**
 * lkfdt_stringlist_get_all() - Obtain array of strings for a given prop
 * @fdt: Device tree blob
 * @node: Device tree node offset
 * @prop: Name of property that contains the phandle
 * @lenp: Return location for array length or an error code.
 *
 * This function allocates and array of string pointers and
 * fills it with the stringlist values.
 *
 * See also: fdt_stringlist_get()
 *
 * Return: pointer to an array of strings, the length of the array is
 * returned via @lenp. Negative @lenp indicates error, in which case
 * partially assembled array may be returned.
 */
char **lkfdt_stringlist_get_all(const void *fdt, int node,
				      const char *prop, int *lenp);

/**
 * lkfdt_get_phandle - retrieve the phandle of a given node or add a new phandle to it.
 * @fdt:  pointer to the device tree blob
 * @node: structure block offset of the node
 *
 * This function will add a new phandle property to the node if it doesn't exist.
 *
 * Return: The phandle value or 0 on error.
 */
uint32_t lkfdt_get_phandle(void *fdt, int node);

#endif /* LK2ND_UTIL_LKFDT_H */
