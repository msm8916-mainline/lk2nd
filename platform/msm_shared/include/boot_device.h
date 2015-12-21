/* Copyright (c) 2014,2015 The Linux Foundation. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of The Linux Foundation. nor the names of its
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

#ifndef __BOOT_DEVICE_H
#define __BOOT_DEVICE_H

#include <sys/types.h>
#include <platform.h>

#if USE_MDM_BOOT_CFG
#define BOOT_DEVICE_MASK(val)   ((val & 0x1E) >> 1)
#else
#define BOOT_DEVICE_MASK(val)   boot_device_mask(val)
#endif

/* Boot device */
#if USE_MDM_BOOT_CFG
enum boot_device
{
	BOOT_DEFAULT=0, /* NAND */
	BOOT_EMMC=3,
};
#else
enum boot_device
{
	BOOT_DEFAULT=0, /* EMMC */
	BOOT_SD=1,     /* SD card*/
	BOOT_EMMC=2,
	BOOT_UFS=4,
};
#endif

void platform_read_boot_config();
uint32_t platform_get_boot_dev();
uint32_t platform_boot_dev_isemmc();
void platform_boot_dev_cmdline(char *buf);
#endif
