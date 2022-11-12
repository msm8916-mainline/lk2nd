/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef LK2ND_UTIL_LKFDT_H
#define LK2ND_UTIL_LKFDT_H

#include <compiler.h>
#include <stdbool.h>

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
 * lkfdt_lookup_phandle() - Read phandle from property and search for the node.
 * @fdt: Device tree blob
 * @node: Device tree node offset
 * @prop: Name of property that contains the phandle
 *
 * Return:
 * * >= 0 - The offset of the phandle node, if successful
 * *  < 0 - libfdt error (<0), otherwise
 */
int lkfdt_lookup_phandle(const void *fdt, int node, const char *prop) __PURE;

/**
 * lkfdt_setprop_by_node_paths() - Set a property on every specified node
 * @fdt: Device tree blob
 * @name: Name of the property
 * @value: Property value to be set
 * @len: Length of the property value
 * @paths: Array of device tree node paths, terminated with NULL
 *
 * Return: 0 if successful, <0 libfdt error
 */
int lkfdt_setprop_by_node_paths(void *fdt, const char *name,
				const void *value, int len,
				const char **paths);

#endif /* LK2ND_UTIL_LKFDT_H */
