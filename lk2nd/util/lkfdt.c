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

int lkfdt_lookup_phandle(const void *fdt, int node, const char *prop)
{
	const fdt32_t *phandle;
	int len;

	phandle = fdt_getprop(fdt, node, prop, &len);
	if (len < 0)
		return len;
	if (len != sizeof(*phandle))
		return -FDT_ERR_BADVALUE;

	return fdt_node_offset_by_phandle(fdt, fdt32_to_cpu(*phandle));
}
#endif /* WITH_LIB_LIBFDT */
