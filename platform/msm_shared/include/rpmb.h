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

#ifndef _RPMB_H_
#define _RPMB_H_

#include <qseecom_lk_api.h>
#include <mmc_sdhci.h>
#include <ufs.h>
#include <debug.h>
#include <qseecomi_lk.h>

#define RPMB_SECTOR_SIZE            256
#define RPMB_LSTNR_ID               0x2000
#define RPMB_BLK_SIZE               512
#define RPMB_FRAME_SIZE             512
#define RPMB_MIN_BLK_CNT            1
#define RPMB_MIN_BLK_SZ             512

enum device_type
{
	EMMC_RPMB = 3,
	UFS_RPMB = 9,
};

/* RPMB request and response types */
enum rpmb_rr_type {
	KEY_PROVISION = 0x1,
	READ_WRITE_COUNTER,
	AUTH_WRITE,
	AUTH_READ,
	READ_RESULT_FLAG,
};

/* List of all return codes for rpmb frame verification from response */
enum rpmb_verify_return_codes
{
	NONCE_MISMATCH = 0x100,
};

/* These are error codes returned for RPMB operations */
enum rpmb_verify_error_codes
{
	OPERATION_OK = 0,
	GENERAL_FAILURE,
	AUTH_FAILURE,
	COUNTER_FAILURE,
	ADDRESS_FAILURE,
	WRITE_FAILURE,
	READ_FAILURE,
	KEY_NOT_PROG,
	MAXED_WR_COUNTER = 0x80,
};

/* RPMB Frame */
struct rpmb_frame
{
	uint8_t  stuff_bytes[196];
	uint8_t  keyMAC[32];
	uint8_t  data[256];
	uint8_t  nonce[16];
	uint8_t writecounter[4];
	uint8_t address[2];
	uint8_t blockcount[2];
	uint8_t  result[2];
	uint8_t requestresponse[2];
};

struct rpmb_init_info
{
	uint32_t size;
	uint32_t rel_wr_count;
	uint32_t dev_type;
};

/* dump a given RPMB frame */
static inline void dump_rpmb_frame(uint8_t *frame, const char *frame_type)
{
    int i;
    char buf[512] = {0};
    char *p = buf;

    printf("Printing %s frame (in hex)\n", frame_type);
    printf("________0__1__2__3__4__5__6__7__8__9__A__B__C__D__E__F__0__1__2__3__4__5__6__7__8__9__A__B__C__D__E__F\n");
    for (i = 0; i < 512; i++) {
        if (!(i % 32)) {
            snprintf(p, 8, "| %2d | ", (i + 1) / 32);
            p += 7;
        }
        snprintf(p, 4, "%.2x ", frame[i]);
        p += 3;
        if (((i + 1) % 32) == 0) {
            printf("%s\n", buf);
            p = buf;
        }
    }
    printf("________F__E__D__C__B__A__9__8__7__6__5__4__3__2__1__0__F__E__D__C__B__A__9__8__7__6__5__4__3__2__1__0\n");
}

int read_device_info_rpmb(void *info, uint32_t sz);
int write_device_info_rpmb(void *info, uint32_t sz);

/* Function Prototypes */
int rpmb_write_emmc(struct mmc_device *dev, uint32_t *req_buf, uint32_t blk_cnt, uint32_t rel_wr_count, uint32_t *resp_buf, uint32_t *resp_len);
int rpmb_read_emmc(struct mmc_device *dev, uint32_t *req_buf, uint32_t blk_cnt, uint32_t *resp_buf, uint32_t *resp_len);
int rpmb_write_ufs(struct ufs_dev *dev, uint32_t *req_buf, uint32_t blk_cnt, uint32_t rel_wr_count, uint32_t *resp_buf, uint32_t *resp_len);
int rpmb_read_ufs(struct ufs_dev *dev, uint32_t *req_buf, uint32_t blk_cnt, uint32_t *resp_buf, uint32_t *resp_len);

/* APIs exposed to applications */
int rpmb_init();
int rpmb_uninit();
int rpmb_write(uint32_t *req_buf, uint32_t blk_cnt, uint32_t rel_wr_count, uint32_t *resp_buf, uint32_t *resp_len);
int rpmb_read(uint32_t *req_buf, uint32_t blk_cnt, uint32_t *resp_buf, uint32_t *resp_len);
struct rpmb_init_info *rpmb_get_init_info();
#endif
