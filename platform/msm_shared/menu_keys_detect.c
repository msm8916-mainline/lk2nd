/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
#include <pm8x41.h>
#include <pm8x41_hw.h>
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
#include <../../../app/aboot/recovery.h>

#define KEY_DETECT_FREQUENCY		50
#define KEY_PRESS_TIMEOUT		5000

#define RECOVERY_MODE   0x77665502
#define FASTBOOT_MODE   0x77665500

static uint32_t wait_time = 0;
static int old_device_type = -1;

extern int target_volume_up();
extern uint32_t target_volume_down();
extern void reboot_device(unsigned reboot_reason);
extern void shutdown_device();

typedef uint32_t (*keys_detect_func)(void);
typedef uint32_t (*keys_action_func)(struct select_msg_info* msg_info,
	uint32_t option_index);

struct keys_stru {
	int type;
	keys_detect_func keys_pressed_func;
};

struct keys_stru keys[] = {
	{VOLUME_UP, (uint32_t (*)(void))target_volume_up},
	{VOLUME_DOWN, target_volume_down},
	{POWER_KEY, pm8x41_get_pwrkey_is_pressed},
};

struct pages_action {
	keys_action_func up_action_func;
	keys_action_func down_action_func;
	keys_action_func enter_action_func;
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

static void update_device_status(unsigned reason)
{
	if (reason == RECOVER) {
		/* wipe data */
		struct recovery_message msg;

		snprintf(msg.recovery, sizeof(msg.recovery), "recovery\n--wipe_data");
		write_misc(0, &msg, sizeof(msg));

		reboot_device(RECOVERY_MODE);
	} else if (reason == RESTART) {
		reboot_device(0);
	} else if (reason == POWEROFF) {
		shutdown_device();
	} else if (reason == FASTBOOT) {
		reboot_device(FASTBOOT_MODE);
	} else if (reason == CONTINUE) {
		fbcon_clear();
		display_image_on_screen();
	}
}

static void update_volume_up_bg(struct select_msg_info* msg_info, uint32_t option_index)
{
	if (option_index == msg_info->option_num - 1) {
		fbcon_draw_msg_background(msg_info->option_start[0],
			msg_info->option_end[0],
			msg_info->option_bg[0], 0);

		fbcon_draw_msg_background(msg_info->option_start[msg_info->option_num - 1],
			msg_info->option_end[msg_info->option_num - 1],
			msg_info->option_bg[msg_info->option_num - 1], 1);
	} else {
		fbcon_draw_msg_background(msg_info->option_start[option_index],
			msg_info->option_end[option_index],
			msg_info->option_bg[option_index], 1);

		fbcon_draw_msg_background(msg_info->option_start[option_index + 1],
			msg_info->option_end[option_index + 1],
			msg_info->option_bg[option_index + 1], 0);
	}
}

static void update_volume_down_bg(struct select_msg_info* msg_info, uint32_t option_index)
{
	if (option_index == 0) {
		fbcon_draw_msg_background(msg_info->option_start[0],
			msg_info->option_end[0],
			msg_info->option_bg[0], 1);

		fbcon_draw_msg_background(msg_info->option_start[msg_info->option_num - 1],
			msg_info->option_end[msg_info->option_num - 1],
			msg_info->option_bg[msg_info->option_num - 1], 0);
	} else {
		fbcon_draw_msg_background(msg_info->option_start[option_index],
			msg_info->option_end[option_index],
			msg_info->option_bg[option_index], 1);

		fbcon_draw_msg_background(msg_info->option_start[option_index - 1],
			msg_info->option_end[option_index - 1],
			msg_info->option_bg[option_index - 1], 0);
	}
}

/* update select option's background when volume up key is pressed */
static uint32_t menu_volume_up_func (struct select_msg_info* msg_info,
	uint32_t option_index)
{
	if (option_index == msg_info->option_num ||
		option_index == 0) {
		option_index = msg_info->option_num - 1;
	} else if (option_index > 0) {
		option_index--;
	}

	update_volume_up_bg(msg_info, option_index);

	return option_index;
}

/* update select option's background when volume down key is pressed */
static uint32_t menu_volume_down_func (struct select_msg_info* msg_info,
	uint32_t option_index)
{
	option_index++;
	if (option_index >= msg_info->option_num)
		option_index = 0;

	update_volume_down_bg(msg_info, option_index);

	return option_index;
}

/* enter to boot verify page2 if volume key is pressed */
static uint32_t boot_page1_volume_keys_func (struct select_msg_info* msg_info,
	uint32_t option_index)
{
	keys_detect_init();
	old_device_type = msg_info->msg_type;
	display_boot_verified_option(msg_info);
	msg_info->msg_volume_key_pressed = true;
	option_index = msg_info->option_num;

	return option_index;
}

/* update device's status via select option */
static uint32_t unlock_power_key_func (struct select_msg_info* msg_info,
	uint32_t option_index)
{
	int device_state = -1;
	if (option_index == 0)
		device_state = RECOVER;
	else if (option_index == 1)
		device_state = RESTART;

	update_device_status(device_state);
	return 0;
}

/* continue booting when power key is pressed at boot-verify page1 */
static uint32_t boot_page1_power_key_func (struct select_msg_info* msg_info,
	uint32_t option_index){
	update_device_status(CONTINUE);
	return option_index;
}

/* update device's status via select option */
static uint32_t boot_page2_power_key_func (struct select_msg_info* msg_info,
	uint32_t option_index)
{
	if (option_index == BACK) {
		wait_time = 0;
		msg_info->msg_timeout = false;
		option_index = msg_info->option_num;
		display_boot_verified_menu(msg_info,
			old_device_type);
	} else {
		msg_info->msg_power_key_pressed = true;
		update_device_status(option_index);
	}
	return option_index;
}

/* initialize different page's function
 * UNLOCK_PAGE/BOOT_VERIFY_PAGE2:
 *	up_action_func: update select option's background when volume up
 *	is pressed
 *	down_action_func: update select option's background when volume up
 *	is pressed
 *	enter_action_func: update device's status via select option
 * BOOT_VERIFY_PAGE1:
 *	up_action_func/down_action_func: enter BOOT_VERIFY_PAGE2 when volume
 *	key is pressed
 *	enter_action_func: continue booting
 */
static struct pages_action menu_pages_action[] = {
	[UNLOCK_PAGE] = {
		menu_volume_up_func,
		menu_volume_down_func,
		unlock_power_key_func,
	},
	[BOOT_VERIFY_PAGE1] = {
		boot_page1_volume_keys_func,
		boot_page1_volume_keys_func,
		boot_page1_power_key_func,
	},
	[BOOT_VERIFY_PAGE2] = {
		menu_volume_up_func,
		menu_volume_down_func,
		boot_page2_power_key_func,
	},
};

void keys_detect_init()
{
	wait_time = 0;
}

int select_msg_keys_detect(void *param) {
	struct select_msg_info *msg_info = (struct select_msg_info*)param;
	uint32_t current_page_index;
	uint32_t option_index = msg_info->option_num;

	keys_detect_init();
	while(1) {
		/* get page's index via different message type */
		switch(msg_info->msg_type) {
		case DISPLAY_MENU_UNLOCK:
			current_page_index = UNLOCK_PAGE;
			break;
		case DISPLAY_MENU_MORE_OPTION:
			current_page_index = BOOT_VERIFY_PAGE2;
			break;
		default:
			current_page_index = BOOT_VERIFY_PAGE1;
			break;
		}

		/* device will continue booting when user has no action
		 * on BOOT_VERIFY_PAGE1
		 */
		if (wait_time > KEY_PRESS_TIMEOUT)
			msg_info->msg_timeout = true;

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
			option_index =
				menu_pages_action[current_page_index].up_action_func(msg_info, option_index);
		} else if (is_key_pressed(VOLUME_DOWN)) {
			option_index =
				menu_pages_action[current_page_index].down_action_func(msg_info, option_index);
		} else if (is_key_pressed(POWER_KEY)) {
			option_index =
				menu_pages_action[current_page_index].enter_action_func(msg_info, option_index);
		}

		wait_time += KEY_DETECT_FREQUENCY;
		thread_sleep(KEY_DETECT_FREQUENCY);
	}

	return 0;
}
