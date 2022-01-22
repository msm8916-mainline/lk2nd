// SPDX-License-Identifier: GPL-2.0-only

#include <lk2nd.h>
#include <libfdt.h>

bool lkfdt_node_is_available(const void *fdt, int node)
{
	const char *prop;
	int len;

	prop = fdt_getprop(fdt, node, "status", &len);
	if (len == -FDT_ERR_NOTFOUND)
		return true;

	return len == sizeof("okay") && memcmp(prop, "okay", sizeof("okay")) == 0;
}
