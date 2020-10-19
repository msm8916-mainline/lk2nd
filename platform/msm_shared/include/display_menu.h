/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*     * Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
*	copyright notice, this list of conditions and the following
*	disclaimer in the documentation and/or other materials provided
*	with the distribution.
*     * Neither the name of The Linux Foundation nor the names of its
*	contributors may be used to endorse or promote products derived
*	from this software without specific prior written permission.
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

#ifndef __PLATFORM_MSM_SHARED_DISPLAY_MENU_H
#define __PLATFORM_MSM_SHARED_DISPLAY_MENU_H

#if VERIFIED_BOOT
#include <openssl/evp.h>
#endif
#include <kernel/mutex.h>

#define SELECT_OPTION_MAX	5

enum display_menu_type {
	DISPLAY_MENU_YELLOW = 0,
	DISPLAY_MENU_ORANGE,
	DISPLAY_MENU_RED,
	DISPLAY_MENU_MORE_OPTION,
	DISPLAY_MENU_UNLOCK,
	DISPLAY_MENU_FASTBOOT,
	DISPLAY_MENU_UNLOCK_CRITICAL,
	DISPLAY_MENU_LOGGING,
	DISPLAY_MENU_EIO,
};

struct menu_info {
	uint32_t	option_start[SELECT_OPTION_MAX];
	uint32_t	option_end[SELECT_OPTION_MAX];
	uint32_t	option_bg[SELECT_OPTION_MAX];
	uint32_t	option_num;
	uint32_t	option_index;
	uint32_t	msg_type;
	uint32_t	timeout_time;
	bool		is_exit;
	bool		rel_exit;
};

struct select_msg_info {
	struct menu_info	info;
	uint32_t		last_msg_type;
	mutex_t			msg_lock;
};

void wait_for_users_action(void);
void display_unlock_menu_renew(struct select_msg_info *msg_info, int type);
void display_bootverify_menu_renew(struct select_msg_info *msg_info, int type);
void display_bootverify_option_menu_renew(struct select_msg_info *msg_info);
void display_fastboot_menu_renew(struct select_msg_info *fastboot_msg_info);
void display_bootverify_menu(int type);
void display_fastboot_menu();
void display_unlock_menu(int type);
void msg_lock_init();
void exit_menu_keys_detection();
#endif				/* __PLATFORM_MSM_SHARED_DISPLAY_MENU_H */
