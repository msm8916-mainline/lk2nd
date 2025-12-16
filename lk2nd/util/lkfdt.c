// SPDX-License-Identifier: BSD-3-Clause

#if WITH_LIB_LIBFDT
#include <libfdt.h>
#include <lk2nd/util/lkfdt.h>

int lkfdt_prop_strneq(const void *fdt, int node, const char *prop, const char *cmp)
{
	const char *val;
	int len;

	val = fdt_getprop(fdt, node, prop, &len);
	if (len < 0)
		return len;

	/* String without null terminator? */
	if (len == 0 || val[len - 1] != '\0')
		return -FDT_ERR_BADVALUE;

	return strcmp(val, cmp) != 0;
}

bool lkfdt_node_is_available(const void *fdt, int node)
{
	int ret = lkfdt_prop_strneq(fdt, node, "status", "okay");
	return ret == 0 || ret == -FDT_ERR_NOTFOUND;
}

static bool read_reg(const fdt32_t *reg, int cells, uint32_t *out)
{
	/* Only handle 32-bit values for now */
	while (--cells)
		if (*reg++)
			return false;
	*out = fdt32_to_cpu(*reg);
	return true;
}

int lkfdt_get_reg(const void *fdt, int parent, int node,
		  uint32_t *addr, uint32_t *size)
{
	int len, addr_cells, size_cells;
	const fdt32_t *reg;

	reg = fdt_getprop(fdt, node, "reg", &len);
	if (len < 0)
		return len;

	addr_cells = fdt_address_cells(fdt, parent);
	if (addr_cells < 0)
		return addr_cells;
	size_cells = fdt_size_cells(fdt, parent);
	if (size_cells < 0)
		return size_cells;
	if (size && size_cells == 0)
		return -FDT_ERR_BADNCELLS;

	if (len < (addr_cells + size_cells) * (int)sizeof(*reg))
		return -FDT_ERR_BADVALUE;

	if (!read_reg(reg, addr_cells, addr))
		return -FDT_ERR_BADVALUE;
	if (size && !read_reg(reg + addr_cells, size_cells, size))
		return -FDT_ERR_BADVALUE;

	return 0;
}

int lkfdt_getprop_u32(const void *fdt, int node, const char *prop, uint32_t *val)
{
	const fdt32_t *fval;
	int len;

	fval = fdt_getprop(fdt, node, prop, &len);
	if (len < 0)
		return len;
	if (len != sizeof(*fval))
		return -FDT_ERR_BADVALUE;

	*val = fdt32_to_cpu(*fval);
	return 0;
}

int lkfdt_u32list_get(const void *fdt, int node, const char *prop,
		      int idx, uint32_t *val)
{
	const fdt32_t *fvals;
	int len;

	ASSERT(idx >= 0 && idx < (INT_MAX / (int)sizeof(*fvals)));

	fvals = fdt_getprop(fdt, node, prop, &len);
	if (len < 0)
		return len;
	if (len % sizeof(*fvals))
		return -FDT_ERR_BADVALUE;
	if (len < ((idx + 1) * (int)sizeof(*fvals)))
		return -FDT_ERR_NOTFOUND;

	*val = fdt32_to_cpu(fvals[idx]);
	return 0;
}

int lkfdt_lookup_phandle(const void *fdt, int node, const char *prop)
{
	uint32_t phandle;
	int ret;

	ret = lkfdt_getprop_u32(fdt, node, prop, &phandle);
	if (ret < 0)
		return ret;

	return fdt_node_offset_by_phandle(fdt, phandle);
}

int lkfdt_subnode_offset_by_phandle(const void *fdt, int parent, uint32_t phandle)
{
	int node;

	fdt_for_each_subnode(node, fdt, parent) {
		if (fdt_get_phandle(fdt, node) == phandle)
			return node;
	}
	return node;
}

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
 * returned via @lenp. Negative @lenp indicates error, in which case the
 * return value is NULL. The array will be terminated with an extra NULL.
 */
char **lkfdt_stringlist_get_all(const void *fdt, int node,
				      const char *prop, int *lenp)
{
	int i = 0, len, vlen;
	char **arr = NULL;
	const char *tmp;

	len = fdt_stringlist_count(fdt, node, prop);
	if (len < 0)
		goto error;

	arr = malloc((len + 1) * sizeof(*arr));
	if (!arr) {
		len = -FDT_ERR_INTERNAL;
		goto error;
	}

	for (i = 0; i < len; ++i) {
		tmp = fdt_stringlist_get(fdt, node, prop, i, &vlen);
		if (vlen < 0) {
			len = vlen;
			goto error;
		}

		arr[i] = strndup(tmp, vlen);
		if (!arr[i]) {
			len = -FDT_ERR_INTERNAL;
			goto error;
		}
	}
	arr[i] = NULL;

	if (lenp)
		*lenp = len;

	return arr;

error:
	if (lenp)
		*lenp = len;

	for (; i > 0; i--)
		free(arr[i-1]);

	free(arr);
	return NULL;

}

uint32_t lkfdt_get_phandle(void *fdt, int node)
{
	uint32_t ph = fdt_get_phandle(fdt, node);
	int ret;

	if (ph)
		return ph;

	ret = fdt_generate_phandle(fdt, &ph);
	if (ret)
		return 0;

	ret = fdt_setprop_u32(fdt, node, "phandle", ph);
	if (ret)
		return 0;

	return ph;
}

#endif /* WITH_LIB_LIBFDT */
