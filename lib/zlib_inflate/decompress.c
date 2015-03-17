/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation, Inc. nor the names of its
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

#define NO_GZIP

#include "zutil.h"

#ifdef DYNAMIC_CRC_TABLE
#include "crc32.h"
#endif

#include "zconf.h"
#include "zlib.h"
#include "inftrees.h"
#include "inflate.h"
#include "inffast.h"
#include "decompress.h"
#include <sys/types.h>	/* for size_t */
#include <string.h>
#include <debug.h>
#include <malloc.h>

#define GZIP_HEADER_LEN 10
#define GZIP_FILENAME_LIMIT 256

static void zlib_free(voidpf qpaque, void *addr)
{
	return free(addr);
}

static void *zlib_alloc(voidpf qpaque, uInt items, size_t size)
{
	return malloc(items * size);
}

/* decompress gzip file "in_buf", return 0 if decompressed successful,
 * return -1 if decompressed failed.
 * in_buf - input gzip file
 * in_len - input the length file
 * out_buf - output the decompressed data
 * out_buf_len - the available length of out_buf
 * pos - position of the end of gzip file
 * out_len - the length of decompressed data
 */
int decompress(unsigned char *in_buf, unsigned int in_len,
		       unsigned char *out_buf,
		       unsigned int out_buf_len,
		       unsigned int *pos,
		       unsigned int *out_len) {
	struct z_stream_s *stream;
	int rc = -1;
	int i;

	if (in_len < GZIP_HEADER_LEN) {
		dprintf(INFO, "the input data is not a gzip package.\n");
		return rc;
	}
	if (out_buf_len < in_len) {
		dprintf(INFO, "the avaiable length of out_buf is not enough.\n");
		return rc;
	}

	stream = malloc(sizeof(*stream));
	if (stream == NULL) {
		dprintf(INFO, "allocating z_stream failed.\n");
		return rc;
	}

	stream->zalloc = zlib_alloc;
	stream->zfree = zlib_free;
	stream->next_out = out_buf;
	stream->avail_out = out_buf_len;

	/* skip over gzip header */
	stream->next_in = in_buf + GZIP_HEADER_LEN;
	stream->avail_in = out_buf_len - GZIP_HEADER_LEN;
	/* skip over asciz filename */
	if (in_buf[3] & 0x8) {
		for (i = 0; i < GZIP_FILENAME_LIMIT && *stream->next_in++; i++) {
			if (stream->avail_in == 0) {
				dprintf(INFO, "header error\n");
				goto gunzip_end;
			}
			--stream->avail_in;
		}
	}

	rc = inflateInit2(stream, -MAX_WBITS);
	if (rc != Z_OK) {
		dprintf(INFO, "inflateInit2 failed!\n");
		goto gunzip_end;
	}

	rc = inflate(stream, 0);
	/* Z_STREAM_END is "we unpacked it all" */
	if (rc == Z_STREAM_END) {
		rc = 0;
	} else if (rc != Z_OK) {
		dprintf(INFO, "uncompression error \n");
		rc = -1;
	}

	inflateEnd(stream);
	if (pos)
		/* alculation the length of the compressed package */
		*pos = stream->next_in - in_buf + 8;

	if (out_len)
		*out_len = stream->total_out;

gunzip_end:
	free(stream);
	return rc; /* returns 0 if decompressed successful */
}

/* check if the input "buf" file was a gzip package.
 * Return true if the input "buf" is a gzip package.
 */
int is_gzip_package(unsigned char *buf, unsigned int len)
{
	if (len < 10 || !buf || buf[0] != 0x1f ||
		buf[1] != 0x8b || buf[2] != 0x08)
	{
		dprintf(INFO, "Not a gzip file\n");
		return false;
	}

	return true;
}
