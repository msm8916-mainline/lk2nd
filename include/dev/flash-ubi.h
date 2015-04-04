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

struct __attribute__ ((packed)) ubifs_ch {
	uint32_t magic;
	uint32_t crc;
	uint64_t sqnum;
	uint32_t len;
#define UBIFS_SB_NODE	6
	uint8_t node_type;
	uint8_t group_type;
	uint8_t padding[2];
};

/* UBIFS superblock node */
struct __attribute__ ((packed)) ubifs_sb_node {
	struct ubifs_ch ch;
	uint8_t padding[2];
	uint8_t key_hash;
	uint8_t key_fmt;
#define UBIFS_FLG_SPACE_FIXUP  0x04
	uint32_t flags;
	uint32_t min_io_size;
	uint32_t leb_size;
	uint32_t leb_cnt;
	uint32_t max_leb_cnt;
	uint64_t max_bud_bytes;
	uint32_t log_lebs;
	uint32_t lpt_lebs;
	uint32_t orph_lebs;
	uint32_t jhead_cnt;
	uint32_t fanout;
	uint32_t lsave_cnt;
	uint32_t fmt_version;
	uint16_t default_compr;
	uint8_t padding1[2];
	uint32_t rp_uid;
	uint32_t rp_gid;
	uint64_t rp_size;
	uint32_t time_gran;
	uint8_t uuid[16];
	uint32_t ro_compat_version;
	uint8_t padding2[3968];
};

/* Erase counter header magic number (ASCII "UBI#") */
#define UBI_EC_HDR_MAGIC  0x55424923
/* Volume identifier header magic number (ASCII "UBI!") */
#define UBI_VID_HDR_MAGIC 0x55424921

#define UBI_MAGIC      "UBI#"
#define UBI_MAGIC_SIZE 0x04

#define UBI_VERSION 1
#define UBI_MAX_ERASECOUNTER 0x7FFFFFFF
#define UBI_IMAGE_SEQ_BASE 0x12345678
#define UBI_DEF_ERACE_COUNTER 0
#define UBI_CRC32_INIT 0xFFFFFFFFU
#define UBIFS_CRC32_INIT 0xFFFFFFFFU

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

#define UBI_MAX_VOLUMES 128
#define UBI_INTERNAL_VOL_START (0x7FFFFFFF - 4096)
#define UBI_LAYOUT_VOLUME_ID     UBI_INTERNAL_VOL_START
#define UBI_VID_DYNAMIC 1
#define UBI_LAYOUT_VOLUME_TYPE UBI_VID_DYNAMIC
#define UBI_FM_SB_VOLUME_ID	(UBI_INTERNAL_VOL_START + 1)

/* A record in the UBI volume table. */
struct __attribute__ ((packed)) ubi_vtbl_record {
	uint32_t  reserved_pebs;
	uint32_t  alignment;
	uint32_t  data_pad;
	uint8_t    vol_type;
	uint8_t    upd_marker;
	uint16_t  name_len;
#define UBI_VOL_NAME_MAX 127
	uint8_t    name[UBI_VOL_NAME_MAX+1];
	uint8_t    flags;
	uint8_t    padding[23];
	uint32_t  crc;
};
#define UBI_VTBL_RECORD_SIZE sizeof(struct ubi_vtbl_record)
#define UBI_VTBL_RECORD_SIZE_CRC (UBI_VTBL_RECORD_SIZE - sizeof(uint32_t))

/* PEB status */
enum {
	UBI_UNKNOWN = 0,
	UBI_BAD_PEB,
	UBI_FREE_PEB,
	UBI_USED_PEB,
	UBI_EMPTY_PEB
};

/**
 * struct peb_info - In RAM info on a PEB
 * @ec: erase counter
 * @status: status of this PEB: UBI_BAD_PEB/USED/FREE/EMPTY
 * @volume: if status = UBI_USED_PEB this is the volume
 * 		ID this PEB belongs to -1 for any other status
 */
struct peb_info {
	uint64_t ec;
	int status;
	int volume;
};

/**
 * struct ubi_scan_info - UBI scanning information.
 * @pebs_data: info on all of partition PEBs
 * @mean_ec: mean erase counter
 * @vtbl_peb1: number of the first PEB holding the volume table
 * 			 (relative to the beginning of the partition)
 * @vtbl_peb1: number of the second PEB holding the volume table
 * 			 (relative to the beginning of the partition)
 * @bad_cnt: count of bad eraseblocks
 * @empty_cnt: count of empty eraseblocks
 * @free_cnt: count of free eraseblocks
 * @used_cnt: count of used eraseblocks
 * @fastmap_sb: PEB number holding FM superblock. If FM is not present: -1
 * @vid_hdr_offs: volume ID header offset from the found EC headers (%-1 means
 *                undefined)
 * @data_offs: data offset from the found EC headers (%-1 means undefined)
 * @image_seq: image sequence
 * @read_image_seq: image sequence read from NAND while scanning
 */
struct ubi_scan_info {
	struct peb_info *pebs_data;
	uint64_t mean_ec;
	int vtbl_peb1;
	int vtbl_peb2;
	int bad_cnt;
	int empty_cnt;
	int free_cnt;
	int used_cnt;
	int fastmap_sb;
	unsigned vid_hdr_offs;
	unsigned data_offs;
	uint32_t  image_seq;
	uint32_t  read_image_seq;
};

int flash_ubi_img(struct ptentry *ptn, void *data, unsigned size);
int update_ubi_vol(struct ptentry *ptn, const char* vol_name,
				void *data, unsigned size);
#endif
