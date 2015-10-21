/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <reg.h>
#include <ufs_hw.h>
#include <utp.h>
#include <upiu.h>
#include <uic.h>
#include <ucs.h>
#include <dme.h>
#include <qgic.h>
#include <string.h>
#include <platform/iomap.h>
#include <platform/irqs.h>
#include <kernel/mutex.h>

static int ufs_dev_init(struct ufs_dev *dev)
{
	/* Init the mutexes. */
	mutex_init(&(dev->uic_data.uic_mutex));
	mutex_init(&(dev->utrd_data.bitmap_mutex));
	mutex_init(&(dev->utmrd_data.bitmap_mutex));

	/* Initialize wait lists. */
	list_initialize(&(dev->utrd_data.list_head.list_node));
	list_initialize(&(dev->utmrd_data.list_head.list_node));

	/* Initialize the bitmaps. */
	dev->utrd_data.bitmap  = 0;
	dev->utmrd_data.bitmap = 0;

	/* Initialize task ids. */
	dev->utrd_data.task_id  = 0;
	dev->utmrd_data.task_id = 0;

	/* Allocate memory for lists. */
	dev->utrd_data.list_base_addr  = ufs_alloc_trans_req_list();
	dev->utmrd_data.list_base_addr = ufs_alloc_task_mgmt_req_list();

	if (!dev->utrd_data.list_base_addr || !dev->utmrd_data.list_base_addr)
		return -UFS_FAILURE;

	return UFS_SUCCESS;
}

static void ufs_setup_req_lists(struct ufs_dev *dev)
{
	uint32_t val;

	writel(dev->utmrd_data.list_base_addr, UFS_UTMRLBA(dev->base));
	writel(dev->utmrd_data.list_base_addr << 32, UFS_UTMRLBAU(dev->base));

	writel(dev->utrd_data.list_base_addr, UFS_UTRLBA(dev->base));
	writel(dev->utrd_data.list_base_addr << 32, UFS_UTRLBAU(dev->base));

	writel(1, UFS_UTMRLRSR(dev->base));
	writel(1, UFS_UTRLRSR(dev->base));

	/* Enable the required irqs. */
	val = UFS_IE_UEE | UFS_IE_UCCE ;
	ufs_irq_enable(dev, val);
	// Change UFS_IRQ to level based
	qgic_change_interrupt_cfg(UFS_IRQ, INTERRUPT_LVL_N_TO_N);
}


void ufs_rpmb_init(struct ufs_dev *dev)
{
        int ret = 0;

        /*
         * Perform request sense on lun to clear
         * attention pending, other wise all the read/write
         * operations would fail with check condition error
         */
        ucs_do_request_sense(dev, UFS_WLUN_RPMB);

        // calculate the size of rpmb partition in sectors
        ret = dme_read_unit_desc(dev, UFS_WLUN_RPMB);
        if (ret != UFS_SUCCESS)
        {
                dprintf(CRITICAL, "UFS dme_read_unit_desc failed for RPMB Partition\n");
                return;
        }

        // gets the number of rpmb frames allowed in a single UPIU commands
        ret = dme_read_geo_desc(dev);
        if (ret != UFS_SUCCESS)
        {
                dprintf(CRITICAL, "UFS dme_read_geo_desc failed for RPMB Partition\n");
                return;
        }
#ifdef DEBUG_UFS
        dprintf(INFO, "RPMB: Logical Block Count: 0x%x\n", dev->rpmb_num_blocks);
        dprintf(INFO, "RPMB: RPMB Read Write Size: 0x%x\n", dev->rpmb_rw_size);
#endif
}



int ufs_read(struct ufs_dev* dev, uint64_t start_lba, addr_t buffer, uint32_t num_blocks)
{
	struct scsi_rdwr_req req;
	int                  ret;

	req.data_buffer_base = buffer;
	req.lun              = dev->current_lun;
	req.num_blocks       = num_blocks;
	req.start_lba        = start_lba / dev->block_size;

	ret = ucs_do_scsi_read(dev, &req);
	if (ret)
	{
		dprintf(CRITICAL, "UFS read failed.\n");
		ufs_dump_hc_registers(dev);
	}

	return ret;
}

int ufs_write(struct ufs_dev* dev, uint64_t start_lba, addr_t buffer, uint32_t num_blocks)
{
	struct scsi_rdwr_req req;
	int                  ret;

	req.data_buffer_base = buffer;
	req.lun              = dev->current_lun;
	req.num_blocks       = num_blocks;
	req.start_lba        = start_lba / dev->block_size;

	ret = ucs_do_scsi_write(dev, &req);
	if (ret)
	{
		dprintf(CRITICAL, "UFS write failed.\n");
		ufs_dump_hc_registers(dev);
	}

	return ret;
}

int ufs_erase(struct ufs_dev* dev, uint64_t start_lba, uint32_t num_blocks)
{
	struct scsi_unmap_req req;
	int                    ret;

	req.lun        = dev->current_lun;
	req.start_lba  = start_lba / dev->block_size;
	req.num_blocks = num_blocks;

	ret = ucs_do_scsi_unmap(dev, &req);
	if(ret)
	{
		dprintf(CRITICAL, "UFS erase failed \n");
	}

	return ret;
}

uint64_t ufs_get_dev_capacity(struct ufs_dev* dev)
{
	uint64_t capacity;
	uint8_t lun = dev->current_lun;

	capacity = dev->lun_cfg[lun].logical_blk_cnt * dev->block_size;

	return capacity;
}

uint32_t ufs_get_erase_blk_size(struct ufs_dev* dev)
{
	uint32_t erase_blk_size;
	uint8_t lun = dev->current_lun;

	erase_blk_size = dev->lun_cfg[lun].erase_blk_size;

	return erase_blk_size;
}

uint32_t ufs_get_serial_num(struct ufs_dev* dev)
{
	int ret;

	ret = dme_read_device_desc(dev);
	if (ret)
	{
		dprintf(CRITICAL, "UFS get serial number failed.\n");
		ufs_dump_hc_registers(dev);
	}

	return dev->serial_num;
}

uint32_t ufs_get_page_size(struct ufs_dev* dev)
{
	return dev->block_size;
}

uint8_t ufs_get_num_of_luns(struct ufs_dev* dev)
{
	return dev->num_lus;
}

int ufs_init(struct ufs_dev *dev)
{
	uint32_t ret = UFS_SUCCESS;
	uint64_t cap;
	uint8_t lun = 0;

	dev->block_size = 4096;

	/* Init dev struct. */
	ret = ufs_dev_init(dev);
	if (ret != UFS_SUCCESS)
	{
		dprintf(CRITICAL, "UFS dev_init failed\n");
		goto ufs_init_err;
	}

	/* Perform Data link init. */
	ret = uic_init(dev);
	if (ret != UFS_SUCCESS)
	{
		dprintf(CRITICAL, "UFS uic_init failed\n");
		goto ufs_init_err;
	}

	/* Setup request lists. */
	ufs_setup_req_lists(dev);

	/* Send NOP to check if device UTP layer is ready. */
	ret = dme_send_nop_query(dev);
	if (ret != UFS_SUCCESS)
	{
		dprintf(CRITICAL, "UFS dme_send_nop_query failed\n");
		goto ufs_init_err;
	}

	ret = dme_set_fdeviceinit(dev);
	if (ret != UFS_SUCCESS)
	{
		dprintf(CRITICAL, "UFS dme_set_fdeviceinit failed\n");
		goto ufs_init_err;
	}

	ret = ucs_scsi_send_inquiry(dev);
	if (ret != UFS_SUCCESS)
	{
		dprintf(CRITICAL, "UFS ucs_scsi_send_inquiry failed\n");
		goto ufs_init_err;
	}

	ret = dme_read_device_desc(dev);
	if (ret != UFS_SUCCESS)
	{
		dprintf(CRITICAL, "dme_read_dev_desc read failed\n");
		goto ufs_init_err;
	}


	for(lun=0; lun < dev->num_lus; lun++)
	{
		ret = dme_read_unit_desc(dev, lun);
		if (ret != UFS_SUCCESS)
		{
			dprintf(CRITICAL, "UFS dme_read_unit_desc failed\n");
			goto ufs_init_err;
		}
	}

	dprintf(CRITICAL,"UFS init success\n");

ufs_init_err:

	if(ret != UFS_SUCCESS)
	{
		ufs_dump_hc_registers(dev);
	}

	return ret;
}

void ufs_dump_is_register(struct ufs_dev *dev)
{
	uint32_t base = dev->base;
	dprintf(CRITICAL,"UFS_IS 0x%x\n",readl(UFS_IS(base)));
}

void ufs_dump_hc_registers(struct ufs_dev *dev)
{
	uint32_t base = dev->base;

	dprintf(CRITICAL,"------Host controller register dump ---------\n");
	dprintf(CRITICAL,"UFS_UECPA 0x%x\n",readl(UFS_UECPA(base)));
	dprintf(CRITICAL,"UFS_UECDL 0x%x\n",readl(UFS_UECDL(base)));
	dprintf(CRITICAL,"UFS_UECN 0x%x\n", readl(UFS_UECN(base)));
	dprintf(CRITICAL,"UFS_UECT 0x%x\n",readl(UFS_UECT(base)));
	dprintf(CRITICAL,"UFS_UECDME 0x%x\n",readl(UFS_UECDME(base)));
	dprintf(CRITICAL,"UFS_HCS 0x%x\n", readl(UFS_HCS(base)));
	dprintf(CRITICAL,"-----------End--------------------------------\n");
}
