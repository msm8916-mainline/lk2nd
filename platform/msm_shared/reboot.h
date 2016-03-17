/*
 * Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#ifndef __REBOOT_H__
#define __REBOOT_H__

enum reboot_reason {
#if USE_PON_REBOOT_REG
	/* hard reset reason */
	REBOOT_MODE_UNKNOWN	= 0x00,
	RECOVERY_MODE		= 0x01,
	FASTBOOT_MODE		= 0x02,
	ALARM_BOOT		= 0x03,
#if ENABLE_VB_ATTEST
	DM_VERITY_EIO		= 0x04,
#else
	DM_VERITY_LOGGING	= 0x04,
#endif
	DM_VERITY_ENFORCING	= 0x05,
	DM_VERITY_KEYSCLEAR	= 0x06,
#else
	REBOOT_MODE_UNKNOWN	= 0x00,
	RECOVERY_MODE		= 0x77665502,
	FASTBOOT_MODE		= 0x77665500,
	ALARM_BOOT		= 0x77665503,
#if ENABLE_VB_ATTEST
	DM_VERITY_EIO	        = 0x77665508,
#else
	DM_VERITY_LOGGING	= 0x77665508,
#endif
	DM_VERITY_ENFORCING	= 0x77665509,
	DM_VERITY_KEYSCLEAR	= 0x7766550A,
#endif
	/* Don't write the reason to PON reg or SMEM
	 * if the value is more than 0xF0000000
	 */
	NORMAL_DLOAD		= 0xF0000001,
	EMERGENCY_DLOAD,
};

#define RTC_TRG           4
#define PON_SOFT_RB_SPARE 0x88F

#if USER_FORCE_RESET_SUPPORT
/* Return 1 if it is a force resin triggered by user. */
uint32_t is_user_force_reset(void);
#endif

unsigned check_reboot_mode(void);

unsigned check_hard_reboot_mode(void);

uint32_t check_alarm_boot(void);

void reboot_device(unsigned reboot_reason);
void shutdown_device();

#endif
