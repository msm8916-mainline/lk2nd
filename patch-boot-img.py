#!/usr/bin/env python
# SPDX-License-Identifier: GPL-2.0-only
import struct
import sys
import shutil

BOOT_MAGIC = 'ANDROID!'.encode()

HEADER_FORMAT = '<8s10I16s512s32s1024s'
HEADER_SIZE = struct.calcsize(HEADER_FORMAT)


def patch_boot_img(fin, new_kernel, fout):
	header = list(struct.unpack(HEADER_FORMAT, fin.read(HEADER_SIZE)[:HEADER_SIZE]))

	# Ensure this is an Android boot image
	if header[0] != BOOT_MAGIC:
		print("Image does not appear to be an Android boot image")
		exit(1)

	page_size = header[8]
	page_mask = page_size - 1

	old_kernel_size = header[1]
	new_kernel_size = len(new_kernel)
	header[1] = new_kernel_size
	fout.write(struct.pack(HEADER_FORMAT, *header))
	fout.seek(page_size)
	fout.write(new_kernel)

	fin.seek(page_size + ((old_kernel_size + page_mask) & ~page_mask))
	fout.seek(page_size + ((new_kernel_size + page_mask) & ~page_mask))
	shutil.copyfileobj(fin, fout)

with open(sys.argv[2], 'rb') as fink:
	new_kernel = fink.read()

with open(sys.argv[1], 'rb') as fin:
	with open(sys.argv[3], 'wb') as fout:
			patch_boot_img(fin, new_kernel, fout)
