// SPDX-License-Identifier: GPL-2.0-only

#include "grove.h"

#include <libfdt.h>
#include <decompress.h>

/*
 * grove is a multi-dtb container that uses fdt format to incapsulate the blobs.
 * It's loosely inspired by u-boot FIT uImage but only targets the Device-Tree
 * selection.
 *
 * It's assumed that the bootloader has libfdt available and also already
 * knows the compatible string of the device.
 *
 * gzip implementation is required to support compressed dtb blobs.
 */

/**
 * grove_check() - Check if the given fdt is compatible.
 * @grove: pointer to the multi-dtb container
 *
 * Return: 1 if compatible, 0 or negative errno otherwise.
 */
int grove_check(const void *grove)
{
	int ret;
	const uint32_t *val;

	// Sanity check the container.
	ret = fdt_check_header(grove);
	if (ret)
		return ret;

	// Check if the given FDT is actually the container
	// and it's version is compatible.
	val = fdt_getprop(grove, 0, "grove,version", &ret);
	if (!val && ret == -FDT_ERR_NOTFOUND)
		return 0;
	else if (!val)
		return ret;
	else if (fdt32_to_cpu(*val) != GROVE_VERSION)
		return -GROVE_EBADVER;

	return 1;
}

/**
 * grove_extract() - Copy a compatible device-tree blob from the container
 * @grove: pointer to the multi-dtb container
 * @buf: pointer to memory where the selected device is to be moved
 * @bufsize: size of the memory space at buf
 * @compatible: compatible string to match the device tree in the contaienr
 *
 * This method finds the dtb that is compatible with the device and copies it
 * into given buffer, possibly unpacking it.
 *
 * Return: Extracted size on success, negative errno or zero if fdt is not a
 * compatible container.
 */
int grove_extract(const void *grove,
		  void *buf,
		  int bufsize,
		  const char *compatible)
{
	int ret, len, chosen_node;
	const void *dtb;
	const char *compression;

	ret = grove_check(grove);
	if (ret != 1)
		return ret;

	// 0 offset so we don't match the root node.
	chosen_node = fdt_node_offset_by_compatible(grove, 0, compatible);
	if (ret < 0)
		return ret;

	dtb = fdt_getprop(grove, chosen_node, "dtb", &ret);
	if (ret < 0)
		return ret;

	if (fdt_check_header(dtb) == 0) {
		if (ret > bufsize)
			return -GROVE_ENOSIZE;

		memcpy(buf, dtb, ret);
		return ret;
	}
	else if (is_gzip_package(dtb, ret)) {
		ret = decompress(dtb, ret, buf, bufsize, NULL, &len);
		if (ret)
			return -GROVE_ECOMPFAIL;
		return len;
	}

	return -GROVE_EBADCOMP;
}

/**
 * grove_get_label() - Get a label string from the container
 * @grove: pointer to the multi-dtb container
 *
 * Return: Pointer to the label in the container or NULL.
 */
char * grove_get_label(const void *grove)
{
	int ret;

	ret = grove_check(grove);
	if (ret != 1)
		return NULL;

	return (char *)fdt_getprop(grove, 0, "grove,label", NULL);
}
