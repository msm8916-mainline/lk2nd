/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#ifndef _UCS_H
#define _UCS_H

#define SCSI_MAX_DATA_TRANS_BLK_LEN    0xFFFF
#define UFS_DEFAULT_SECTORE_SIZE       4096

#define SCSI_STATUS_GOOD               0x00
#define SCSI_STATUS_CHK_COND           0x02
#define SCSI_STATUS_BUSY               0x08
#define SCSI_STATUS_TASK_SET_FULL      0x08

#define SCSI_SENSE_BUF_LEN             0x20
#define SCSI_INQUIRY_LEN               36
#define SCSI_CDB_PARAM_LEN             16

/* FLAGS for indication of read or write */
enum scsi_upiu_flags
{
	UPIU_FLAGS_READ         = 0x40,
	UPIU_FLAGS_WRITE        = 0x20,
	UPIU_FLAGS_ATTR_SIMPLE  = 0x00,
	UPIU_FLAGS_ATTR_ORDERED = 0x01,
	UPIU_FLAGS_ATTR_HOQ     = 0x02,
};

#define SCSI_READ_WRITE_10_CDB1(rd_protect, dpo, fua, fua_nv) ((rd_protect) << 5 | (dpo) << 4 | (fua) << 3 | (fua_nv) << 1)

/* SCSI commands */
enum utp_scsi_cmd_type
{
	SCSI_CMD_TEST_UNIT_RDY      = 0x00,
	SCSI_CMD_SENSE_REQ          = 0x03,
	SCSI_CMD_INQUIRY            = 0x12,
	SCSI_CMD_READ10             = 0x28,
	SCSI_CMD_READ_CAP10         = 0x25,     // Read Capacity
	SCSI_CMD_SYNC_CACHE10       = 0x35,
	SCSI_CMD_UNMAP              = 0x42,
	SCSI_CMD_WRITE10            = 0x2A,
	SCSI_CMD_SECPROT_IN         = 0xA2,     // Security Protocal in
	SCSI_CMD_SECPROT_OUT        = 0xB5,     // Security Protocal out
	SCSI_CMD_REPORT_LUNS        = 0xA0,
};

struct scsi_req_build_type
{
	addr_t               cdb;
	uint8_t              lun;
	addr_t               data_buffer_addr;
	uint32_t             data_len;
	enum scsi_upiu_flags flags;
	enum upiu_dd_type    dd;
};

struct scsi_rdwr_req
{
	uint8_t  lun;
	uint32_t start_lba;
	uint32_t num_blocks;
	uint32_t data_buffer_base;
};

struct scsi_rdwr_cdb
{
	uint8_t  opcode;
	uint8_t  cdb1;
	uint32_t lba;
	uint8_t  grp_num;
	uint16_t trans_len;
	uint8_t  control;
	uint8_t  resv[6];
}__PACKED;

struct scsi_unmap_req
{
	uint8_t  lun;
	uint64_t start_lba;
	uint32_t num_blocks;
}__PACKED;

struct unmap_blk_desc
{
	uint64_t lba;
	uint32_t num_blks;
	uint32_t reserved;
}__PACKED;

struct unmap_param_list
{
	uint16_t data_len;
	uint16_t blk_desc_data_len;
	uint32_t reserved;
	struct unmap_blk_desc blk_desc;
}__PACKED;

struct scsi_sense_cdb
{
	uint8_t  opcode;
	uint8_t  desc;
	uint16_t resv_0;
	uint8_t  alloc_len;
	uint8_t  control;
	uint8_t  resv_1[10];
}__PACKED;

struct scsi_failure_sense_data
{
	uint8_t  valid_resp_code;
	uint8_t  obsolete;
	uint16_t file_mark_eom_ili_resv_sense_key;
	uint8_t  sense_info[4];
	uint8_t  additional_sense_len;
	uint32_t csi;
	uint8_t  asc;
	uint8_t  ascq;
	uint8_t  fruc;
	uint8_t  sense_key_specific;
}__PACKED;

int ucs_scsi_send_inquiry(struct ufs_dev *dev);
int ucs_do_scsi_read(struct ufs_dev *dev, struct scsi_rdwr_req *req);
int ucs_do_scsi_write(struct ufs_dev *dev, struct scsi_rdwr_req *req);

#endif
