/* Copyright (c) 2010-2017,2019,2021 The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
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

#include <debug.h>
#include <arch/arm.h>
#include <dev/udc.h>
#include <string.h>
#include <kernel/thread.h>
#include <arch/ops.h>
#include <arch/defines.h>
#include <malloc.h>

#include <dev/flash.h>
#include <lib/ptable.h>
#include <dev/keys.h>
#include <platform.h>
#include <target.h>
#include <partition_parser.h>
#include <mmc.h>
#include <malloc.h>
#include <stdlib.h>
#include "recovery.h"
#include "bootimg.h"
#include "smem.h"

#define BOOT_FLAGS	1
#define UPDATE_STATUS	2

static const int MISC_COMMAND_PAGE = 1;		// bootloader command is this page
static char buf[4096];

extern uint32_t get_page_size(void);
extern void reset_device_info(void);
extern void set_device_root(void);

int get_recovery_message(struct recovery_message *out)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();

	ptable = flash_get_ptable();

	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}
	ptn = ptable_find(ptable, "misc");

	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No misc partition found\n");
		return -1;
	}

	offset += (pagesize * MISC_COMMAND_PAGE);
	if (flash_read(ptn, offset, (void *) buf, pagesize)) {
		dprintf(CRITICAL, "ERROR: Cannot read recovery_header\n");
		return -1;
	}
	memcpy(out, buf, sizeof(*out));
	return 0;
}

int set_recovery_message(const struct recovery_message *in)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned offset = 0;
	unsigned pagesize = flash_page_size();
	unsigned n = 0;
	void *scratch_addr = target_get_scratch_address();

	ptable = flash_get_ptable();

	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}
	ptn = ptable_find(ptable, "misc");

	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No misc partition found\n");
		return -1;
	}

	n = pagesize * (MISC_COMMAND_PAGE + 1);

	if (flash_read(ptn, offset, scratch_addr, n)) {
		dprintf(CRITICAL, "ERROR: Cannot read recovery_header\n");
		return -1;
	}

	offset += (pagesize * MISC_COMMAND_PAGE);
	offset += (unsigned) scratch_addr;
	memcpy((void *) offset, in, sizeof(*in));
	if (flash_write(ptn, 0, scratch_addr, n)) {
		dprintf(CRITICAL, "ERROR: flash write fail!\n");
		return -1;
	}
	return 0;
}

static int set_ssd_radio_update (char *name)
{
	struct ptentry *ptn;
	struct ptable *ptable;
	unsigned int *ssd_cookie;
	unsigned pagesize = flash_page_size();

	ptable = flash_get_ptable();
	if (ptable == NULL) {
		dprintf(CRITICAL, "ERROR: Partition table not found\n");
		return -1;
	}

	ssd_cookie = malloc(pagesize);
	if (!ssd_cookie){
		dprintf(CRITICAL, "ERROR: Memory allocation failure\n");
		return -1;
	}
	memset(ssd_cookie, 0, pagesize);
	ssd_cookie[0] = 0x53534443;
	ssd_cookie[1] = 0x4F4F4B49;

	ptn = ptable_find(ptable, name);
	if (ptn == NULL) {
		dprintf(CRITICAL, "ERROR: No %s partition found\n", name);
		goto out;
	}

	if (flash_write(ptn, 0, ssd_cookie, pagesize)) {
		dprintf(CRITICAL, "ERROR: flash write fail!\n");
		goto out;
	}

	free(ssd_cookie);
	dprintf(INFO, "FOTA partition written successfully!");
	return 0;
out:
	free(ssd_cookie);
	return -1;
}

int get_boot_info_apps (char type, unsigned int *status)
{
	boot_info_for_apps apps_boot_info;
	int ret = 0;

	ret = smem_read_alloc_entry(SMEM_BOOT_INFO_FOR_APPS,
			&apps_boot_info, sizeof(apps_boot_info));
	if (ret)
	{
		dprintf(CRITICAL, "ERROR: unable to read shared memory for apps boot info %d\n",ret);
		return ret;
	}

	dprintf(INFO,"boot flag %x update status %x\n",apps_boot_info.boot_flags,
			apps_boot_info.status.update_status);

	if(type == BOOT_FLAGS)
		*status = apps_boot_info.boot_flags;
	else if(type == UPDATE_STATUS)
		*status = apps_boot_info.status.update_status;

	return ret;
}

/* Bootloader / Recovery Flow
 *
 * On every boot, the bootloader will read the recovery_message
 * from flash and check the command field.  The bootloader should
 * deal with the command field not having a 0 terminator correctly
 * (so as to not crash if the block is invalid or corrupt).
 *
 * The bootloader will have to publish the partition that contains
 * the recovery_message to the linux kernel so it can update it.
 *
 * if command == "boot-recovery" -> boot recovery.img
 * else if command == "update-radio" -> update radio image (below)
 * else -> boot boot.img (normal boot)
 *
 * Radio Update Flow
 * 1. the bootloader will attempt to load and validate the header
 * 2. if the header is invalid, status="invalid-update", goto #8
 * 3. display the busy image on-screen
 * 4. if the update image is invalid, status="invalid-radio-image", goto #8
 * 5. attempt to update the firmware (depending on the command)
 * 6. if successful, status="okay", goto #8
 * 7. if failed, and the old image can still boot, status="failed-update"
 * 8. write the recovery_message, leaving the recovery field
 *    unchanged, updating status, and setting command to
 *    "boot-recovery"
 * 9. reboot
 *
 * The bootloader will not modify or erase the cache partition.
 * It is recovery's responsibility to clean up the mess afterwards.
 */

int recovery_init (void)
{
	struct recovery_message msg;
	char partition_name[32];
	unsigned valid_command = 0;
	int update_status = 0;

	// get recovery message
	if (get_recovery_message(&msg))
		return -1;
	msg.command[sizeof(msg.command)-1] = '\0'; //Ensure termination
	if (msg.command[0] != 0 && msg.command[0] != 255) {
		dprintf(INFO,"Recovery command: %d %s\n",
			sizeof(msg.command), msg.command);
	}

	if (!strcmp(RECOVERY_BOOT_RECOVERY_CMD, msg.command))
	{
		if(!strcmp("RADIO",msg.status))
		{
			/* We're now here due to radio update, so check for update status */
			int ret = get_boot_info_apps(UPDATE_STATUS, (unsigned int *) &update_status);

			if(!ret && (update_status & 0x01))
			{
				dprintf(INFO,"radio update success\n");
				strlcpy(msg.status, "OKAY", sizeof(msg.status));
			}
			else
			{
				dprintf(INFO,"radio update failed\n");
				strlcpy(msg.status, "failed-update", sizeof(msg.status));
			}
			strlcpy(msg.command, "", sizeof(msg.command));	// clearing recovery command
			set_recovery_message(&msg);	// send recovery message
			boot_into_recovery = 1;		// Boot in recovery mode
			return 0;
		}
		boot_into_recovery = 1;		// Boot in recovery mode
		return 0;
	}

	if (target_dynamic_partition_supported() &&
		!strcmp(RECOVERY_BOOT_FASTBOOT_CMD, msg.command)) {
		boot_into_recovery = 1;		// Boot in userspace fastboot mode
		return 0;
	}

	if (!strcmp("update-radio",msg.command)) {
		dprintf(INFO,"start radio update\n");
		valid_command = 1;
		strlcpy(partition_name, "FOTA", sizeof(partition_name));
	}

	//Todo: Add support for bootloader update too.

	if(!valid_command) {
		//We need not to do anything
		return 0; // Boot in normal mode
	}

	if (set_ssd_radio_update(partition_name)) {
		/* If writing to FOTA partition fails */
		strlcpy(msg.command, "", sizeof(msg.command));
		strlcpy(msg.status, "failed-update", sizeof(msg.status));
		goto SEND_RECOVERY_MSG;
	}
	else {
		/* Setting this to check the radio update status */
		strlcpy(msg.command, "boot-recovery", sizeof(msg.command));
		strlcpy(msg.status, "RADIO", sizeof(msg.status));
		goto SEND_RECOVERY_MSG;
	}
	strlcpy(msg.status, "OKAY", sizeof(msg.status));

SEND_RECOVERY_MSG:
	set_recovery_message(&msg);	// send recovery message
	boot_into_recovery = 1;		// Boot in recovery mode
	reboot_device(0);
	return 0;
}

static int emmc_set_recovery_msg(struct recovery_message *out)
{
	char *ptn_name = "misc";
	unsigned long long ptn = 0;
	unsigned blocksize = mmc_get_device_blocksize();
	unsigned int size = ROUND_TO_PAGE(sizeof(*out), (unsigned)blocksize - 1);
	unsigned char *data = NULL;
	int ret = 0;
	int index = INVALID_PTN;

	data = malloc(size);
	if(!data)
	{
		dprintf(CRITICAL,"memory allocation error \n");
		ret = -1;
		goto out;
	}

	index = partition_get_index((const char *) ptn_name);
	ptn = partition_get_offset(index);
	mmc_set_lun(partition_get_lun(index));
	if(ptn == 0) {
		dprintf(CRITICAL,"partition %s doesn't exist\n",ptn_name);
		ret = -1;
		goto out;
	}
	memset(data, 0, size);
	memcpy(data, out, sizeof(*out));
	if (mmc_write(ptn , size, (unsigned int*)data)) {
		dprintf(CRITICAL,"mmc write failure %s %d\n",ptn_name, sizeof(*out));
		ret = -1;
		goto out;
	}
out:
	if (data)
		free(data);
	return ret;
}

static int emmc_get_recovery_msg(struct recovery_message *in)
{
	char *ptn_name = "misc";
	unsigned long long ptn = 0;
	unsigned int size;
	int index = INVALID_PTN;

	size = mmc_get_device_blocksize();
	index = partition_get_index((const char *) ptn_name);
	if (index < 0)
	{
		dprintf(CRITICAL, "%s: Partition not found\n", ptn_name);
		return -1;
	}

	ptn = partition_get_offset(index);
	mmc_set_lun(partition_get_lun(index));
	if(ptn == 0) {
		dprintf(CRITICAL,"partition %s doesn't exist\n",ptn_name);
		return -1;
	}
	if (mmc_read(ptn , (unsigned int*)in, size)) {
		dprintf(CRITICAL,"mmc read failure %s %d\n",ptn_name, size);
		return -1;
	}

	return 0;
}

/* Generic funcition to write misc commands. */
int send_recovery_cmd(const char *command)
{
	struct recovery_message msg;
	int status = 0;
	memset(&msg, 0, sizeof(msg));

	/* Populate command to msg */
	snprintf(msg.command,
		 sizeof(msg.command),
		 command);

	dprintf(INFO,"Recovery command: %s\n", msg.command);
	if (target_is_emmc_boot())
		/* Update emmc partition */
		status = emmc_set_recovery_msg(&msg);
	else
		status = set_recovery_message(&msg);

	return status;
}

int _emmc_recovery_init(void)
{
	int update_status = 0;
	struct recovery_message *msg;
	uint32_t block_size = 0;

	block_size = mmc_get_device_blocksize();

	// get recovery message
	msg = (struct recovery_message *)memalign(CACHE_LINE, block_size);
	ASSERT(msg);

	if(emmc_get_recovery_msg(msg))
	{
		if(msg)
			free(msg);
		return -1;
	}

	msg->command[sizeof(msg->command)-1] = '\0'; //Ensure termination
	if (msg->command[0] != 0 && msg->command[0] != 255) {
		dprintf(INFO,"Recovery command: %d %s\n",
			sizeof(msg->command), msg->command);
	}

	if (!strcmp(msg->command, RECOVERY_BOOT_RECOVERY_CMD)) {
		boot_into_recovery = 1;
	}

	if (target_dynamic_partition_supported() &&
		!strcmp(msg->command, RECOVERY_BOOT_FASTBOOT_CMD)) {
		boot_into_recovery = 1;
	}

	if (!strcmp("update-radio",msg->command))
	{
		/* We're now here due to radio update, so check for update status */
		int ret = get_boot_info_apps(UPDATE_STATUS, (unsigned int *) &update_status);

		if(!ret && (update_status & 0x01))
		{
			dprintf(INFO,"radio update success\n");
			strlcpy(msg->status, "OKAY", sizeof(msg->status));
		}
		else
		{
			dprintf(INFO,"radio update failed\n");
			strlcpy(msg->status, "failed-update", sizeof(msg->status));
		}
		boot_into_recovery = 1;		// Boot in recovery mode
	}
	if (!strcmp("reset-device-info",msg->command))
	{
		reset_device_info();
	}
	if (!strcmp("root-detect",msg->command))
	{
		set_device_root();
	}
	else
		goto out;// do nothing

	strlcpy(msg->command, "", sizeof(msg->command));	// clearing recovery command
	emmc_set_recovery_msg(msg);	// send recovery message

out:
	if(msg)
		free(msg);
	return 0;
}

static int read_misc(unsigned page_offset, void *buf, unsigned size)
{
	const char *ptn_name = "misc";
	unsigned offset;

	if (size == 0 || buf == NULL)
		return -1;

#if VIRTUAL_AB_OTA
		offset = page_offset;
#else
		uint32_t pagesize = get_page_size();
		offset = page_offset * pagesize;
#endif

	if (target_is_emmc_boot())
	{
		int index;
		unsigned long long ptn;
		unsigned long long ptn_size;

		index = partition_get_index(ptn_name);
		if (index == INVALID_PTN)
		{
			dprintf(CRITICAL, "No '%s' partition found\n", ptn_name);
			return -1;
		}

		ptn = partition_get_offset(index);
		ptn_size = partition_get_size(index);

		mmc_set_lun(partition_get_lun(index));

		if (ptn_size < offset + size)
		{
			dprintf(CRITICAL, "Read request out of '%s' boundaries\n",
					ptn_name);
			return -1;
		}

		if (mmc_read(ptn + offset, (unsigned int *)buf, size))
		{
			dprintf(CRITICAL, "Reading MMC failed\n");
			return -1;
		}
	}
	else
	{
		dprintf(CRITICAL, "Misc partition not supported for NAND targets.\n");
		return -1;
	}

	return 0;
}

int write_misc(unsigned page_offset, void *buf, unsigned size)
{
	const char *ptn_name = "misc";
	void *scratch_addr = target_get_scratch_address();
	unsigned offset;
	unsigned aligned_size;

	if (size == 0 || buf == NULL || scratch_addr == NULL)
		return -1;

	if (target_is_emmc_boot())
	{
		int index;
		unsigned long long ptn;
		unsigned long long ptn_size;

		index = partition_get_index(ptn_name);
		if (index == INVALID_PTN)
		{
			dprintf(CRITICAL, "No '%s' partition found\n", ptn_name);
			return -1;
		}

		ptn = partition_get_offset(index);
		ptn_size = partition_get_size(index);
#if VIRTUAL_AB_OTA
		offset = page_offset;
#else
		offset = page_offset * BLOCK_SIZE;
#endif
		aligned_size = ROUND_TO_PAGE(size, (unsigned)BLOCK_SIZE - 1);
		if (ptn_size < offset + aligned_size)
		{
			dprintf(CRITICAL, "Write request out of '%s' boundaries\n",
					ptn_name);
			return -1;
		}

		/* This will ensure, we zeored out any extra bytes
		   we will push to emmc, to prevent information leak */
		if (aligned_size > size)
			memset((scratch_addr + size), 0, (aligned_size-size));

		if (scratch_addr != buf)
			memcpy(scratch_addr, buf, size);

		/* Set Lun for misc partition */
		mmc_set_lun(partition_get_lun(index));

		if (mmc_write(ptn + offset, aligned_size, (unsigned int *)scratch_addr))
		{
			dprintf(CRITICAL, "Writing MMC failed\n");
			return -1;
		}
	}
	else
	{
		struct ptentry *ptn;
		struct ptable *ptable;
		unsigned pagesize = flash_page_size();

		ptable = flash_get_ptable();
		if (ptable == NULL)
		{
			dprintf(CRITICAL, "Partition table not found\n");
			return -1;
		}

		ptn = ptable_find(ptable, ptn_name);
		if (ptn == NULL)
		{
			dprintf(CRITICAL, "No '%s' partition found\n", ptn_name);
			return -1;
		}

		offset = page_offset * pagesize;
		aligned_size = ROUND_TO_PAGE(size, pagesize - 1);
		if (ptn->length < offset + aligned_size)
		{
			dprintf(CRITICAL, "Write request out of '%s' boundaries\n",
					ptn_name);
			return -1;
		}

		/* This will ensure, we zeored out any extra bytes
		   we will push, to prevent information leak */
		if (aligned_size > size)
			memset((scratch_addr + size), 0, (aligned_size-size));

		if (scratch_addr != buf)
			memcpy(scratch_addr, buf, size);

		if (flash_write(ptn, offset, scratch_addr, aligned_size)) {
			dprintf(CRITICAL, "Writing flash failed\n");
			return -1;
		}
	}

	return 0;
}

static MiscVirtualABMessage *VirtualAbMsg = NULL;

int SetSnapshotMergeStatus (VirtualAbMergeStatus MergeStatus)
{
	int Status = 1;
	VirtualAbMergeStatus OldMergeStatus;

	if (target_is_emmc_boot())
	{
		OldMergeStatus = VirtualAbMsg->MergeStatus;
		VirtualAbMsg->MergeStatus = MergeStatus;

		Status = write_misc(MISC_VIRTUALAB_OFFSET, &VirtualAbMsg, sizeof(VirtualAbMsg));
		if (Status != 0) {
			dprintf(CRITICAL, "Write the VirtualAbMsg failed\n");
			VirtualAbMsg->MergeStatus = OldMergeStatus;
		}
	}
	return Status;
}

VirtualAbMergeStatus GetSnapshotMergeStatus (void)
{
	VirtualAbMergeStatus MergeStatus = NONE_MERGE_STATUS;
	uint32_t pagesize = get_page_size();

	if (target_is_emmc_boot())
	{
	    if (VirtualAbMsg == NULL) {
	        if(read_misc(MISC_VIRTUALAB_OFFSET, (void *)&VirtualAbMsg,
					pagesize))
		{
			dprintf(CRITICAL,"Error reading virtualab msg from misc partition\n");
			return MergeStatus;
		}

		if (VirtualAbMsg->Magic != MISC_VIRTUAL_AB_MAGIC_HEADER ||
			VirtualAbMsg->Version != MISC_VIRTUAL_AB_MESSAGE_VERSION) {

			dprintf(CRITICAL,"Error read virtualab msg version:%u magic:%u not valid\n",
					VirtualAbMsg->Version,VirtualAbMsg->Magic);

			free(VirtualAbMsg);
			VirtualAbMsg = NULL;
		}
		else
		{
			dprintf(CRITICAL,"read virtualab MergeStatus:%x\n", VirtualAbMsg->MergeStatus);
		}
	    }

	    if (VirtualAbMsg)
	        MergeStatus = VirtualAbMsg->MergeStatus;
	}
	return MergeStatus;
}

int get_ffbm(char *ffbm, unsigned size)
{
	const char *ffbm_cmd = "ffbm-";
	uint32_t page_size = get_page_size();
	char *ffbm_page_buffer = NULL;
	int retval = 0;
	if (size < FFBM_MODE_BUF_SIZE || size >= page_size)
	{
		dprintf(CRITICAL, "Invalid size argument passed to get_ffbm\n");
		retval = -1;
		goto cleanup;
	}
	ffbm_page_buffer = (char*)memalign(CACHE_LINE, page_size);
	if (!ffbm_page_buffer)
	{
		dprintf(CRITICAL, "Failed to alloc buffer for ffbm cookie\n");
		retval = -1;
		goto cleanup;
	}
	if (read_misc(0, ffbm_page_buffer, page_size))
	{
		dprintf(CRITICAL, "Error reading MISC partition\n");
		retval = -1;
		goto cleanup;
	}
	ffbm_page_buffer[size] = '\0';
	if (strncmp(ffbm_cmd, ffbm_page_buffer, strlen(ffbm_cmd)))
	{
		retval = 0;
		goto cleanup;
	}
	else
	{
		if (strlcpy(ffbm, ffbm_page_buffer, size) <
				FFBM_MODE_BUF_SIZE -1)
		{
			dprintf(CRITICAL, "Invalid string in misc partition\n");
			retval = -1;
		}
		else
			retval = 1;
	}
cleanup:
	if(ffbm_page_buffer)
		free(ffbm_page_buffer);
	return retval;
}


