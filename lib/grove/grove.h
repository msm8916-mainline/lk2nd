/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef GROVE_H
#define GROVE_H

#define GROVE_VERSION 1

/* Methods can pass libfdt error codes, grove-specific errors start at 1000 */
#define GROVE_EBADVER	1001
	/* GROVE_EBADVER: The container version is incompatible. */
#define GROVE_ENOSIZE	1002
	/* GROVE_ENOSIZE: Target has not enough size for the dtb. */
#define GROVE_EBADCOMP	1003
	/* GROVE_EBADCOMP: Compression method is incompatible. */
#define GROVE_ECOMPFAIL	1004
	/* GROVE_ECOMPFAIL: Decompression failed. */

int grove_check(const void *grove);

int grove_extract(const void *grove,
		  void *buf,
		  int bufsize,
		  const char *compatible);

char * grove_get_label(const void *grove);

#endif /* GROVE_H */
