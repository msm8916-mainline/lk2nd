/* Copyright (c) 2015-2018, The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <reg.h>
#include <stdlib.h>
#include <kernel/timer.h>
#include <platform/timer.h>
#include <kernel/thread.h>
#include <dev/keys.h>
#include <dev/fbcon.h>
#include <menu_keys_detect.h>
#include <display_menu.h>
#include <platform/gpio.h>
#include <platform/iomap.h>
#include <platform.h>
#include <reboot.h>
#include <sys/types.h>
#include <../../../app/aboot/recovery.h>
#include <../../../app/aboot/devinfo.h>
#include <string.h>

#define KEY_DETECT_FREQUENCY		50

static time_t before_time;

extern bool pwr_key_is_pressed;
extern int target_volume_up(void);
extern uint32_t target_volume_down(void);
extern void reboot_device(unsigned reboot_reason);
extern void shutdown_device(void);
extern uint32_t pm8x41_get_pwrkey_is_pressed(void);

typedef uint32_t (*keys_detect_func)(void);
typedef void (*keys_action_func)(struct select_msg_info* msg_info);

struct keys_stru {
	int type;
	keys_detect_func keys_pressed_func;
};

#if WITH_LK2ND_DEVICE

#include <lk2nd/keys.h>

static uint32_t lk2nd_key_vol_up(void) { return lk2nd_keys_pressed(KEY_VOLUMEUP); }
static uint32_t lk2nd_key_vol_down(void) { return lk2nd_keys_pressed(KEY_VOLUMEDOWN); }
static uint32_t lk2nd_key_power(void) { return lk2nd_keys_pressed(KEY_POWER); }

struct keys_stru keys[] = {
	{VOLUME_UP, lk2nd_key_vol_up},
	{VOLUME_DOWN, lk2nd_key_vol_down},
	{POWER_KEY, lk2nd_key_power},
};
#else
struct keys_stru keys[] = {
	{VOLUME_UP, (uint32_t (*)(void))target_volume_up},
	{VOLUME_DOWN, target_volume_down},
	{POWER_KEY, pm8x41_get_pwrkey_is_pressed},
};
#endif

struct pages_action {
	keys_action_func up_action_func;
	keys_action_func down_action_func;
	keys_action_func enter_action_func;
};

static uint32_t verify_index_action[] = {
		[0] = POWEROFF,
		[1] = RESTART,
		[2] = RECOVER,
		[3] = FASTBOOT,
		[4] = BACK,
};

static uint32_t fastboot_index_action[] = {
		[0] = RESTART,
		[1] = FASTBOOT,
		[2] = RECOVER,
		[3] = POWEROFF,
		[4] = FFBM,
};

static uint32_t unlock_index_action[] = {
		[0] = RESTART,
		[1] = RECOVER,
};

static int is_key_pressed(int keys_type)
{
	int count = 0;

	if (keys[keys_type].keys_pressed_func()) {
		/*if key is pressed, wait for 1 second to see if it is released*/
		while(count++ < 10 && keys[keys_type].keys_pressed_func())
			thread_sleep(100);
		return 1;
	}

	return 0;
}

static void update_device_status(struct select_msg_info* msg_info, int reason)
{
	char ffbm_page_buffer[FFBM_MODE_BUF_SIZE];
	fbcon_clear();
	switch (reason) {
		case RECOVER:
			if (!IS_ENABLED(ABOOT_STANDALONE))
				reset_device_unlock_status(msg_info->info.msg_type);
			reboot_device(RECOVERY_MODE);
			break;
		case RESTART:
			reboot_device(0);
			break;
		case POWEROFF:
			shutdown_device();
			break;
		case FASTBOOT:
			reboot_device(FASTBOOT_MODE);
			break;
		case CONTINUE:
			/* Continue boot, no need to detect the keys'status */
			msg_info->info.is_exit = true;
			break;
		case BACK:
#if VERIFIED_BOOT || VERIFIED_BOOT_2
			display_bootverify_menu_renew(msg_info, msg_info->last_msg_type);
#endif
			before_time = current_time();

			break;
		case FFBM:
			if (!IS_ENABLED(ABOOT_STANDALONE)) {
				memset(&ffbm_page_buffer, 0, sizeof(ffbm_page_buffer));
				snprintf(ffbm_page_buffer, sizeof(ffbm_page_buffer), "ffbm-00");
				write_misc(0, ffbm_page_buffer, sizeof(ffbm_page_buffer));
			}

			reboot_device(0);
			break;
	}
}

/* msg_lock need to be holded when call this function. */
static void update_volume_up_bg(struct select_msg_info* msg_info)
{
	if (msg_info->info.option_index == msg_info->info.option_num - 1) {
		fbcon_draw_msg_background(msg_info->info.option_start[0],
			msg_info->info.option_end[0],
			msg_info->info.option_bg[0], 0);

		fbcon_draw_msg_background(msg_info->info.option_start[msg_info->info.option_num - 1],
			msg_info->info.option_end[msg_info->info.option_num - 1],
			msg_info->info.option_bg[msg_info->info.option_num - 1], 1);
	} else {
		fbcon_draw_msg_background(msg_info->info.option_start[msg_info->info.option_index],
			msg_info->info.option_end[msg_info->info.option_index],
			msg_info->info.option_bg[msg_info->info.option_index], 1);

		fbcon_draw_msg_background(msg_info->info.option_start[msg_info->info.option_index + 1],
			msg_info->info.option_end[msg_info->info.option_index + 1],
			msg_info->info.option_bg[msg_info->info.option_index + 1], 0);
	}
}

/* msg_lock need to be holded when call this function. */
static void update_volume_down_bg(struct select_msg_info* msg_info)
{
	if (msg_info->info.option_index == 0) {
		fbcon_draw_msg_background(msg_info->info.option_start[0],
			msg_info->info.option_end[0],
			msg_info->info.option_bg[0], 1);

		fbcon_draw_msg_background(msg_info->info.option_start[msg_info->info.option_num - 1],
			msg_info->info.option_end[msg_info->info.option_num - 1],
			msg_info->info.option_bg[msg_info->info.option_num - 1], 0);
	} else {
		fbcon_draw_msg_background(msg_info->info.option_start[msg_info->info.option_index],
			msg_info->info.option_end[msg_info->info.option_index],
			msg_info->info.option_bg[msg_info->info.option_index], 1);

		fbcon_draw_msg_background(msg_info->info.option_start[msg_info->info.option_index - 1],
			msg_info->info.option_end[msg_info->info.option_index - 1],
			msg_info->info.option_bg[msg_info->info.option_index - 1], 0);
	}
}

/* update select option's background when volume up key is pressed */
static void menu_volume_up_func (struct select_msg_info* msg_info)
{
	if (msg_info->info.option_index == 0)
		msg_info->info.option_index = msg_info->info.option_num - 1;
	else
		msg_info->info.option_index--;

	if (msg_info->info.msg_type == DISPLAY_MENU_FASTBOOT) {
		display_fastboot_menu_renew(msg_info);
	} else {
		update_volume_up_bg(msg_info);
	}
}

/* update select option's background when volume down key is pressed */
static void menu_volume_down_func (struct select_msg_info* msg_info)
{
	msg_info->info.option_index++;
	if (msg_info->info.option_index >= msg_info->info.option_num)
		msg_info->info.option_index = 0;

	if (msg_info->info.msg_type == DISPLAY_MENU_FASTBOOT) {
		display_fastboot_menu_renew(msg_info);
	} else {
		update_volume_down_bg(msg_info);
	}
}

/* enter to boot verification option page if volume key is pressed */
static void boot_warning_volume_keys_func (struct select_msg_info* msg_info)
{
	msg_info->last_msg_type = msg_info->info.msg_type;
	display_bootverify_option_menu_renew(msg_info);
}

/* update device's status via select option */
static void power_key_func(struct select_msg_info* msg_info)
{
	int reason = -1;
	static boolean isreflash;

	switch (msg_info->info.msg_type) {
		case DISPLAY_MENU_YELLOW:
		case DISPLAY_MENU_ORANGE:
			if (!isreflash) {
			  /* If the power key is pressed for the first time:
			   * Update the warning message and recalculate the timeout
			   */
			  before_time = current_time();
#if VERIFIED_BOOT || VERIFIED_BOOT_2
			  display_bootverify_menu_update (msg_info);
#endif
			  isreflash = TRUE;
			} else {
			  reason = CONTINUE;
			}
			break;
		case DISPLAY_MENU_LOGGING:
			reason = CONTINUE;
			break;
		case DISPLAY_MENU_EIO:
			pwr_key_is_pressed = true;
			reason = CONTINUE;
			break;
		case DISPLAY_MENU_MORE_OPTION:
			if(msg_info->info.option_index < ARRAY_SIZE(verify_index_action))
				reason = verify_index_action[msg_info->info.option_index];
			break;
		case DISPLAY_MENU_UNLOCK:
		case DISPLAY_MENU_UNLOCK_CRITICAL:
		case DISPLAY_MENU_LOCK:
		case DISPLAY_MENU_LOCK_CRITICAL:
			if(msg_info->info.option_index < ARRAY_SIZE(unlock_index_action))
				reason = unlock_index_action[msg_info->info.option_index];
			break;
		case DISPLAY_MENU_FASTBOOT:
			if(msg_info->info.option_index < ARRAY_SIZE(fastboot_index_action))
				reason = fastboot_index_action[msg_info->info.option_index];
			break;
		default:
			dprintf(CRITICAL,"Unsupported menu type\n");
			break;
	}

	if (reason != -1) {
		update_device_status(msg_info, reason);
	}
}

/* Initialize different page's function
 * DISPLAY_MENU_UNLOCK/DISPLAY_MENU_UNLOCK_CRITICAL
 * DISPLAY_MENU_MORE_OPTION/DISPLAY_MENU_FASTBOOT:
 *	up_action_func: update select option's background when volume up
 *	is pressed
 *	down_action_func: update select option's background when volume up
 *	is pressed
 *	enter_action_func: update device's status via select option
 * DISPLAY_MENU_YELLOW/DISPLAY_MENU_ORANGE/DISPLAY_MENU_RED:
 *	up_action_func/down_action_func: enter BOOT_VERIFY_PAGE2 when volume
 *	key is pressed
 *	enter_action_func: continue booting
 */
static struct pages_action menu_pages_action[] = {
	[DISPLAY_MENU_UNLOCK] = {
		menu_volume_up_func,
		menu_volume_down_func,
		power_key_func,
	},
	[DISPLAY_MENU_UNLOCK_CRITICAL] = {
		menu_volume_up_func,
		menu_volume_down_func,
		power_key_func,
	},
	[DISPLAY_MENU_LOCK] = {
		menu_volume_up_func,
		menu_volume_down_func,
		power_key_func,
	},
	[DISPLAY_MENU_LOCK_CRITICAL] = {
		menu_volume_up_func,
		menu_volume_down_func,
		power_key_func,
	},
	[DISPLAY_MENU_YELLOW] = {
		NULL,
		NULL,
		power_key_func,
	},
	[DISPLAY_MENU_ORANGE] = {
		NULL,
		NULL,
		power_key_func,
	},
	[DISPLAY_MENU_RED] = {
		NULL,
		NULL,
		power_key_func,
	},
	[DISPLAY_MENU_LOGGING] = {
		boot_warning_volume_keys_func,
		boot_warning_volume_keys_func,
		power_key_func,
	},
	[DISPLAY_MENU_EIO] = {
		NULL,
		NULL,
		power_key_func,
	},
	[DISPLAY_MENU_MORE_OPTION] = {
		menu_volume_up_func,
		menu_volume_down_func,
		power_key_func,
	},
	[DISPLAY_MENU_FASTBOOT] = {
		menu_volume_up_func,
		menu_volume_down_func,
		power_key_func,
	},

};

void keys_detect_init(void)
{
	/* Waiting for all keys are released */
	while(1) {
		if(!keys[VOLUME_UP].keys_pressed_func() &&
			!keys[VOLUME_DOWN].keys_pressed_func() &&
			!keys[POWER_KEY].keys_pressed_func()) {
			break;
		}
		thread_sleep(KEY_DETECT_FREQUENCY);
	}

	before_time = current_time();
}

int select_msg_keys_detect(void *param) {
	struct select_msg_info *msg_info = (struct select_msg_info*)param;

	msg_lock_init();
	keys_detect_init();
	while(1) {
		/* 1: update select option's index, default it is the total option number
		 *  volume up: index decrease, the option will scroll up from
		 * 	the bottom to top if the key is pressed firstly.
		 *	eg: 5->4->3->2->1->0
		 *  volume down: index increase, the option will scroll down from
		 * 	the bottom to top if the key is pressed firstly.
		 *	eg: 5->0
		 * 2: update device's status via select option's index
		 */
		if (is_key_pressed(VOLUME_UP)) {
			mutex_acquire(&msg_info->msg_lock);
			menu_pages_action[msg_info->info.msg_type].up_action_func(msg_info);
			mutex_release(&msg_info->msg_lock);
		} else if (is_key_pressed(VOLUME_DOWN)) {
			mutex_acquire(&msg_info->msg_lock);
			menu_pages_action[msg_info->info.msg_type].down_action_func(msg_info);
			mutex_release(&msg_info->msg_lock);
		} else if (is_key_pressed(POWER_KEY)) {
			mutex_acquire(&msg_info->msg_lock);
			menu_pages_action[msg_info->info.msg_type].enter_action_func(msg_info);
			mutex_release(&msg_info->msg_lock);
		}

		mutex_acquire(&msg_info->msg_lock);
		/* Never time out if the timeout_time is 0 */
		if(msg_info->info.timeout_time) {
			if ((current_time() - before_time) > msg_info->info.timeout_time)
				msg_info->info.is_exit = true;
		}

		if (msg_info->info.is_exit) {
			msg_info->info.rel_exit = true;
			mutex_release(&msg_info->msg_lock);
			break;
		}
		mutex_release(&msg_info->msg_lock);
		thread_sleep(KEY_DETECT_FREQUENCY);
	}

	return 0;
}
