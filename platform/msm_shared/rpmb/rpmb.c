/* Copyright (c) 2013-2015,2017, The Linux Foundation. All rights reserved.
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
#include <stdlib.h>
#include <string.h>
#include <platform.h>
#include <rpmb.h>
#include <km_main.h>
#include <rpmb_listener.h>
#include <mmc_sdhci.h>
#include <boot_device.h>
#include <debug.h>
#include <target.h>
#include <secapp_loader.h>


static void *dev;
struct rpmb_init_info info;

int rpmb_init()
{
	int ret = 0;

	dev = target_mmc_device();

	/* 1. Initialize storage specific data */
	if (platform_boot_dev_isemmc())
	{
		struct mmc_device *mmc_dev = (struct mmc_device *) dev;
		info.size = mmc_dev->card.rpmb_size / RPMB_MIN_BLK_SZ;

		if ((mmc_dev->card.ext_csd !=NULL) &&
			(mmc_dev->card.ext_csd[MMC_EXT_CSD_REV] < 8))
		{
			//as per emmc spec rel_wr_count should be 1 for emmc version < 5.1
			dprintf(SPEW, "EMMC Version < 5.1\n");
			info.rel_wr_count = 1;
		}
		else
		{
			if ((mmc_dev->card.ext_csd !=NULL) &&
				((mmc_dev->card.ext_csd[MMC_EXT_CSD_EN_RPMB_REL_WR] & BIT(4)) == 0))
			{
				dprintf(SPEW, "EMMC Version >= 5.1 EN_RPMB_REL_WR = 0\n");
		       /*
        		* Some eMMC vendors violate eMMC 5.0 spec and set
        		* REL_WR_SEC_C register to 0x10 to indicate the
        		* ability of RPMB throughput improvement thus lead
        		* to failure when TZ module write data to RPMB
        		* partition. So check bit[4] of EXT_CSD[166] and
        		* if it is not set then change value of REL_WR_SEC_C
        		* to 0x1 directly ignoring value of EXT_CSD[222].
        		*/
				info.rel_wr_count = 1;
			}
			else
			{
				dprintf(SPEW, "EMMC Version >= 5.1 EN_RPMB_REL_WR = 1\n");
				// according to emmc version 5.1 and above if EN_RPMB_REL_WR in extended
				// csd is set the maximum number of frames that can be reliably written
				// to emmc would be 32
				info.rel_wr_count = 32;
			}
		}
		info.dev_type  = EMMC_RPMB;
	}
#ifdef UFS_SUPPORT
	else
	{
		struct ufs_dev *ufs_dev = (struct ufs_dev *) dev;
		ufs_rpmb_init(ufs_dev);
	/*
	 * According to JEDE UFS spec, qLogicalBlockCount in RPMB Unit Descriptor
	 * is a multiple of 256. But TZ expects the number of sectors reported
	 * with sector size in 512 bytes hence report accordingly.
	 */
		info.size = ufs_dev->rpmb_num_blocks / 2;
		info.rel_wr_count = ufs_dev->rpmb_rw_size;
		info.dev_type  = UFS_RPMB;
	}
#endif

	/* Register & start the listener */
	ret = rpmb_listener_start();
	if (ret < 0)
	{
		dprintf(CRITICAL, "Error registering the handler\n");
		goto err;
	}

err:
	return ret;
}

struct rpmb_init_info *rpmb_get_init_info()
{
	return &info;
}

int rpmb_read(uint32_t *req, uint32_t req_len, uint32_t *resp, uint32_t *resp_len)
{
	int ret = 0;
	if (platform_boot_dev_isemmc())
		ret = rpmb_read_emmc(dev, req, req_len, resp, resp_len);
#ifdef UFS_SUPPORT
	else
		ret = rpmb_read_ufs(dev, req, req_len, resp, resp_len);
#endif
	return ret;
}

int rpmb_write(uint32_t *req, uint32_t req_len, uint32_t rel_wr_count, uint32_t *resp, uint32_t *resp_len)
{
	int ret = 0;
	if (platform_boot_dev_isemmc())
		ret = rpmb_write_emmc(dev, req, req_len, rel_wr_count, resp, resp_len);
#ifdef UFS_SUPPORT
	else
		ret = rpmb_write_ufs(dev, req, req_len, rel_wr_count, resp, resp_len);
#endif
	return ret;
}

/* This API calls into TZ app to read device_info */
int read_device_info_rpmb(void *info, uint32_t sz)
{
	int ret = 0;
	struct send_cmd_req read_req = {0};
	struct send_cmd_rsp read_rsp = {0};

	read_req.cmd_id = KEYMASTER_READ_LK_DEVICE_STATE;
	read_req.data   = (uint32_t) info;
	read_req.len    = sz;

	/* Read the device info */
	arch_clean_invalidate_cache_range((addr_t) info, sz);
	ret = qseecom_send_command(get_secapp_handle(), (void*) &read_req, sizeof(read_req), (void*) &read_rsp, sizeof(read_rsp));
	arch_invalidate_cache_range((addr_t) info, sz);

	if (ret < 0 || read_rsp.status < 0)
	{
		dprintf(CRITICAL, "Reading device info failed: Error: %d\n", read_rsp.status);
		return -1;
	}

	return 0;
}

int write_device_info_rpmb(void *info, uint32_t sz)
{
	int ret = 0;

	struct send_cmd_req write_req = {0};
	struct send_cmd_rsp write_rsp = {0};

	write_req.cmd_id = KEYMASTER_WRITE_LK_DEVICE_STATE;
	write_req.data   = (uint32_t) info;
	write_req.len    = sz;

	/* Write the device info */
	arch_clean_invalidate_cache_range((addr_t) info, sz);
	ret = qseecom_send_command(get_secapp_handle(), (void *)&write_req, sizeof(write_req), (void *)&write_rsp, sizeof(write_rsp));
	arch_invalidate_cache_range((addr_t) info, sz);

	if (ret < 0 || write_rsp.status < 0)
	{
		dprintf(CRITICAL, "Writing device info failed: Error: %d\n", write_rsp.status);
		return -1;
	}

	return 0;
}

/*
 * SWP Write function is used to send a configuration block to rpmb
 * for enabling a secure write protect based on LBAs. This function
 * should is enabled by the keymaster secure app and this function
 * can only be called before we send the milestone call to keymaster.
 */
int swp_write(qsee_stor_secure_wp_info_t swp_cb)
{
	secure_write_prot_req_t *req;
	secure_write_prot_rsp_t rsp;
	int ret = 0;
	uint32_t tlen = sizeof(secure_write_prot_req_t) + sizeof(swp_cb);
	if(!(req = (secure_write_prot_req_t *) malloc(tlen)))
		ASSERT(0);
	void *cpy_ptr = (uint8_t *) req + sizeof(secure_write_prot_req_t);
	req->cmd_id = KEYMASTER_SECURE_WRITE_PROTECT;
	req->op = SWP_WRITE_CONFIG;
	req->swp_write_data_offset = sizeof(secure_write_prot_req_t);
	req->swp_write_data_len = sizeof(swp_cb);
	memcpy(cpy_ptr, (void *)&swp_cb, sizeof(swp_cb));
	ret = qseecom_send_command(get_secapp_handle(), (void *)req, tlen, (void *)&rsp, sizeof(rsp));
	if(ret < 0 || rsp.status < 0)
	{
		dprintf(CRITICAL, "Setting secure write protect configuration failed\n");
		return -1;
	}
	return 0;
}

int rpmb_uninit()
{
	int ret = 0;

	ret = rpmb_listener_stop(RPMB_LSTNR_ID);
	if (ret < 0)
	{
		dprintf(CRITICAL, "Failed to stop Qseecom Listener\n");
		return ret;
	}
	ret = qseecom_exit();

	if (ret < 0)
	{
		dprintf(CRITICAL, "Failed to exit qseecom \n");
		return ret;
	}

	return ret;
}


