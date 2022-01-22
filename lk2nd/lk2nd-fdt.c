// SPDX-License-Identifier: GPL-2.0-only

#include <lk2nd.h>
#include <libfdt.h>

int lkfdt_prop_strcmp(const void *fdt, int node, const char *prop, const char *cmp)
{
	const char *val;
	int len, cmplen;

	val = fdt_getprop(fdt, node, prop, &len);
	if (len < 0)
		return len;

	/* String without null terminator? */
	if (len == 0 || val[len - 1] != '\0')
		return -FDT_ERR_BADVALUE;

	return strcmp(val, cmp);
}

bool lkfdt_node_is_available(const void *fdt, int node)
{
	int ret = lkfdt_prop_strcmp(fdt, node, "status", "okay");
	return ret == 0 || ret == -FDT_ERR_NOTFOUND;
}
