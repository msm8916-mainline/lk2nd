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

#ifndef _UFS_H_
#define _UFS_H_

#include <sys/types.h>
#include <kernel/mutex.h>
#include <kernel/event.h>
#include <list.h>

enum ufs_err
{
	UFS_SUCCESS,
	UFS_FAILURE,
	UFS_RETRY,
};

struct ufs_req_node
{
	uint32_t                  task_id;
	uint32_t                  door_bell_bit;
	event_t                   *event;
	struct list_node          list_node;
};


struct ufs_utp_req_meta_data
{
	mutex_t             bitmap_mutex;
	struct ufs_req_node list_head;
	uint32_t            bitmap;
	uint32_t            task_id;
	uint64_t            list_base_addr;
};

struct ufs_uic_meta_data
{
	mutex_t  uic_mutex;
	event_t  uic_event;
};

struct ufs_unit_desc
{
	uint8_t   desc_len;
	uint8_t   desc_type;
	uint8_t   unit_index;
	uint8_t   lu_enable;
	uint8_t   boot_lun_id;
	uint8_t   lu_wp;
	uint8_t   lu_queue_depth;
	uint8_t   resv;
	uint8_t   mem_type;
	uint8_t   data_reliability;
	uint8_t   logical_blk_size;
	uint64_t  logical_blk_cnt;
	uint32_t  erase_blk_size;
	uint8_t   provisioning_type;
	uint8_t   phy_mem_resource_cnt[8];
	uint16_t  ctx_capabilities;
	uint8_t   large_unit_size_m1;
}__PACKED;

struct ufs_dev
{
	uint8_t                      instance;
	uint32_t                     base;
	uint8_t                      num_lus;
	uint8_t                      current_lun;
	uint32_t                     serial_num;
	uint32_t                     block_size;
	uint32_t                     erase_blk_size;
	uint64_t                     capacity;
	struct ufs_unit_desc         lun_cfg[8];

	/* UTRD maintainance data structures.*/
	struct ufs_utp_req_meta_data utrd_data;
	struct ufs_utp_req_meta_data utmrd_data;

	/* UIC maintainance data structures.*/
	struct ufs_uic_meta_data     uic_data;
};

/* Define all the basic WLUN type  */
#define UFS_WLUN_REPORT          0x81
#define UFS_UFS_DEVICE           0xD0
#define UFS_WLUN_BOOT            0xB0
#define UFS_WLUN_RPMB            0xC4

int ufs_init(struct ufs_dev *dev);
int ufs_read(struct ufs_dev* dev, uint64_t start_lba, addr_t buffer, uint32_t num_blocks);
int ufs_write(struct ufs_dev* dev, uint64_t start_lba, addr_t buffer, uint32_t num_blocks);
int ufs_erase(struct ufs_dev* dev, uint64_t start_lba, uint32_t num_blocks);
uint64_t ufs_get_dev_capacity(struct ufs_dev* dev);
uint32_t ufs_get_serial_num(struct ufs_dev* dev);
uint8_t ufs_get_num_of_luns(struct ufs_dev* dev);
uint32_t ufs_get_erase_blk_size(struct ufs_dev* dev);
void ufs_dump_hc_registers(struct ufs_dev* dev);
#endif
