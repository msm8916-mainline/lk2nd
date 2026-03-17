/* Copyright (c) 2015, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include <stdlib.h>
#include <debug.h>
#include "devinfo.h"
#include "fastboot.h"
#include "fastboot_test.h"
#include <app/tests.h>
#include <target.h>
#include <boot_device.h>
#include "mdtp.h"
#include <ufs.h>
#if USE_RPMB_FOR_DEVINFO
#include <rpmb.h>
#endif
/* main function that calls into the tests */

extern void ramdump_table_map(void);
extern void kauth_test(void);
extern int ufs_get_boot_lun(void);
extern int ufs_set_boot_lun(uint32_t bootlunid);
extern int fastboot_init(void *xfer_buffer, unsigned max);
static bool enable_test_mode = false;

bool is_test_mode_enabled(void)
{
	return enable_test_mode;
}

void cmd_oem_runtests(const char *arg, void *data, unsigned sz)
{
	dprintf(INFO, "Running LK tests ... \n");
	enable_test_mode = true;
	// Test boot lun enable for UFS
	if (!platform_boot_dev_isemmc())
	{
		int ret = 0;
		uint32_t set_lun = 0x2, get_lun;
		ret = ufs_set_boot_lun(set_lun);
		if (ret == UFS_SUCCESS)
		{
			get_lun = ufs_get_boot_lun();
			if (get_lun == set_lun)
			{
				dprintf(INFO, "UFS Boot LUN En TEST: [ PASS ]\n");
				set_lun = 0x1; // default is 0x1 LUN A, revert back to 0x1
				ret = ufs_set_boot_lun(set_lun);
			}
			else
				dprintf(INFO, "UFS Boot LUN En TEST: [ FAIL ]\n");
		}
		else
			dprintf(INFO, "UFS Boot LUN En TEST: [ FAIL ]\n");
	}


#if LPAE
	ramdump_table_map();
#endif

#if USE_RPMB_FOR_DEVINFO
	dprintf(INFO, "Running RPMB test case, please make sure RPMB key is provisioned ...\n");
	struct device_info *write_info = memalign(PAGE_SIZE, 4096);
	struct device_info *read_info = memalign(PAGE_SIZE, 4096);

	if((write_info == NULL)||(read_info == NULL))
	{
		dprintf(CRITICAL, "Failed to allocate memory for devinfo %s %d \n",__FUNCTION__,__LINE__);
		goto err;
	}

	if((read_device_info_rpmb((void*) read_info, PAGE_SIZE)) < 0)
		dprintf(INFO, "RPMB READ TEST : [ FAIL ]\n");

	write_info->is_unlocked = !read_info->is_unlocked;

	if((write_device_info_rpmb((void*) write_info, PAGE_SIZE)) < 0)
		dprintf(INFO, "RPMB WRITE TEST : [ FAIL ]\n");

	if((read_device_info_rpmb((void*) read_info, PAGE_SIZE)) < 0)
		dprintf(INFO, "RPMB READ TEST : [ FAIL ]\n");

	if (read_info->is_unlocked == write_info->is_unlocked)
		dprintf(INFO, "RPMB READ/WRITE TEST: [ PASS ]\n");
	else
		dprintf(INFO, "RPMB READ/WRITE TEST: [ FAIL ]\n");

err:
	free(read_info);
	free(write_info);
#endif

#if VERIFIED_BOOT
	if (!boot_linux_from_mmc())
		dprintf(INFO, "Verifid Boot authentication test: [ PASS ]\n");
	else
		dprintf(INFO, "Verifid Boot authentication test: [ FAIL ]\n");

	kauth_test();
#endif

	if (!thread_tests())
		dprintf(INFO, "Thread Test: [ PASS ]\n");
	else
		dprintf(INFO, "Thread Test: [ FAIL ]\n");

	printf_tests();

#ifdef MDTP_SUPPORT
	dprintf(INFO, "Running mdtp LK tests ... \n");
	cmd_mdtp_runtests();
#endif

	fastboot_okay("");
	enable_test_mode = false;
}
