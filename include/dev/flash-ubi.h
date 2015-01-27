/* Copyright (c) 2014-2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
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

#ifndef _FLASH_UBI_H_
#define _FLASH_UBI_H_

#include <sys/types.h>
#include <lib/ptable.h>
#include <stdint.h>

/* Erase counter header magic number (ASCII "UBI#") */
#define UBI_EC_HDR_MAGIC  0x55424923

#define UBI_MAGIC      "UBI#"
#define UBI_MAGIC_SIZE 0x04

#define UBI_VERSION 1
#define UBI_MAX_ERASECOUNTER 0x7FFFFFFF
#define UBI_IMAGE_SEQ_BASE 0x12345678
#define UBI_DEF_ERACE_COUNTER 0
#define UBI_CRC32_INIT 0xFFFFFFFFU

/* Erase counter header fields */
struct __attribute__ ((packed)) ubi_ec_hdr {
	uint32_t  magic;
	uint8_t   version;
	uint8_t   padding1[3];
	uint64_t  ec; /* Warning: the current limit is 31-bit anyway! */
	uint32_t  vid_hdr_offset;
	uint32_t  data_offset;
	uint32_t  image_seq;
	uint8_t   padding2[32];
	uint32_t  hdr_crc;
};

/* Volume identifier header fields */
struct __attribute__ ((packed)) ubi_vid_hdr {
	uint32_t  magic;
	uint8_t    version;
	uint8_t    vol_type;
	uint8_t    copy_flag;
	uint8_t    compat;
	uint32_t  vol_id;
	uint32_t  lnum;
	uint8_t    padding1[4];
	uint32_t  data_size;
	uint32_t  used_ebs;
	uint32_t  data_pad;
	uint32_t  data_crc;
	uint8_t    padding2[4];
	uint64_t  sqnum;
	uint8_t    padding3[12];
	uint32_t  hdr_crc;
};

#define UBI_EC_HDR_SIZE  sizeof(struct ubi_ec_hdr)
#define UBI_VID_HDR_SIZE  sizeof(struct ubi_vid_hdr)
#define UBI_EC_HDR_SIZE_CRC  (UBI_EC_HDR_SIZE  - sizeof(uint32_t))
#define UBI_VID_HDR_SIZE_CRC (UBI_VID_HDR_SIZE - sizeof(uint32_t))

#define UBI_FM_SB_VOLUME_ID	(0x7FFFFFFF - 4096 + 1)
/**
 * struct ubi_scan_info - UBI scanning information.
 * @ec: erase counters or eraseblock status for all eraseblocks
 * @mean_ec: mean erase counter
 * @bad_cnt: count of bad eraseblocks
 * @good_cnt: count of non-bad eraseblocks
 * @empty_cnt: count of empty eraseblocks
 * @vid_hdr_offs: volume ID header offset from the found EC headers (%-1 means
 *                undefined)
 * @data_offs: data offset from the found EC headers (%-1 means undefined)
 * @image_seq: image sequence
 */
struct ubi_scan_info {
	uint64_t *ec;
	uint64_t mean_ec;
	int bad_cnt;
	int good_cnt;
	int empty_cnt;
	unsigned vid_hdr_offs;
	unsigned data_offs;
	uint32_t  image_seq;
};

int flash_ubi_img(struct ptentry *ptn, void *data, unsigned size);
#endif
