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
#include <openssl/evp.h>
#include <dev/fbcon.h>
#include <kernel/thread.h>
#include <display_menu.h>
#include <menu_keys_detect.h>
#include <boot_verifier.h>
#include <string.h>
#include <platform.h>

#define UNLOCK_OPTION_NUM		2
#define BOOT_VERIFY_OPTION_NUM		5

static char *unlock_menu_common_msg = "If you unlock the bootloader, "\
				"you will be able to install "\
				"custom operating system on this phone.\n\n"\
				"A custom OS is not subject to the same testing "\
				"as the original OS, "\
				"and can cause your phone and installed "\
				"applications to stop working properly.\n\n"\
				"To prevent unauthorized access to your personal data, "\
				"unlocking the bootloader will also delete all personal "\
				"data from your phone(a \"fatory data reset\").\n\n"\
				"Press the Volume Up/Down buttons to select Yes "\
				"or No. Then press the Power button to continue.\n";

static char *yellow_warning_msg = "Your device has loaded a diffrent operating "\
				"system\n\nTo learn more, visit:\n";

static char *orange_warning_msg = "Your device has been unlocker and cann't "\
				"be trusted\n\nTo learn more, visit:\n";

static char *red_warning_msg = "Your device has failed verification and may "\
				"not work properly\n\nTo learn more, visit:\n";

static bool is_thread_start = false;
struct select_msg_info msg_info;

static char *option_menu[] = {
		[POWEROFF] = "Power off\n",
		[RESTART] = "Restart\n",
		[RECOVER] = "Recovery\n",
		[FASTBOOT] = "Fastboot\n",
		[BACK] = "Back to previous page\n"};

static int big_factor = 2;
static int common_factor = 1;

void wait_for_users_action()
{
	struct select_msg_info *select_msg;
	select_msg = &msg_info;

	while(1) {
		if (select_msg->msg_timeout == true &&
			select_msg->msg_volume_key_pressed == false)
			break;
		if (select_msg->msg_power_key_pressed == true)
			break;

		thread_sleep(10);
	}
	fbcon_clear();
	display_image_on_screen();
}

static void set_message_factor()
{
	uint32_t tmp_factor = 0;
	uint32_t max_x_count = 40;
	uint32_t max_x = fbcon_get_max_x();

	max_x = fbcon_get_max_x();
	tmp_factor = max_x/max_x_count;

	if(tmp_factor <= 1) {
		big_factor = 2;
		common_factor = 1;
	} else {
		big_factor = tmp_factor*2;
		common_factor = tmp_factor;
	}
}

static void display_fbcon_menu_message(char *str, unsigned type,
	unsigned scale_factor)
{
	while(*str != 0) {
		fbcon_putc_factor(*str++, type, scale_factor);
	}
}

static char *str_align_right(char *str, int factor)
{
	uint32_t max_x = 0;
	int diff = 0;
	int i = 0;
	char *str_target = NULL;

	max_x = fbcon_get_max_x();
	if (!str_target && max_x) {
		str_target = malloc(max_x);
	}

	if (str_target) {
		memset(str_target, 0, max_x);
		if ( max_x/factor > strlen(str)) {
			if (factor == 1)
				diff = max_x/factor - strlen(str) - 1;
			else
				diff = max_x/factor - strlen(str);
			for (i = 0; i < diff; i++) {
				strcat(str_target, " ");
			}
			strcat(str_target, str);
			return str_target;
		} else {
			free(str_target);
			return str;
		}
	}
	return str;
}

void display_unlock_menu(struct select_msg_info *unlock_msg_info)
{
	fbcon_clear();
	memset(unlock_msg_info, 0, sizeof(struct select_msg_info));
	display_fbcon_menu_message("Unlock bootloader?\n",
		FBCON_UNLOCK_TITLE_MSG, big_factor);
	fbcon_draw_line();

	display_fbcon_menu_message(unlock_menu_common_msg,
		FBCON_COMMON_MSG, common_factor);
	fbcon_draw_line();
	unlock_msg_info->option_start[0] = fbcon_get_current_line();
	display_fbcon_menu_message("Yes\n",
		FBCON_COMMON_MSG, big_factor);
	unlock_msg_info->option_bg[0] = fbcon_get_current_bg();
	display_fbcon_menu_message("Unlock bootloader(may void warranty)\n",
		FBCON_COMMON_MSG, common_factor);
	unlock_msg_info->option_end[0] = fbcon_get_current_line();
	fbcon_draw_line();
	unlock_msg_info->option_start[1] = fbcon_get_current_line();
	display_fbcon_menu_message("No\n",
		FBCON_COMMON_MSG, big_factor);
	unlock_msg_info->option_bg[1] = fbcon_get_current_bg();
	display_fbcon_menu_message("Do not unlock bootloader and restart phone\n",
		FBCON_COMMON_MSG, common_factor);
	unlock_msg_info->option_end[1] = fbcon_get_current_line();
	fbcon_draw_line();

	unlock_msg_info->msg_type = DISPLAY_MENU_UNLOCK;
	unlock_msg_info->option_num = UNLOCK_OPTION_NUM;
}

void display_boot_verified_menu(struct select_msg_info *msg_info, int type)
{
	int msg_type = FBCON_COMMON_MSG;
	char *warning_msg = NULL;
	unsigned char* fp_buf = NULL;
	char fp_str_temp[EVP_MAX_MD_SIZE] = {'\0'};
	char fp_str[EVP_MAX_MD_SIZE*2] = {'\0'};
	char str_temp[8];

	char str1[]= "Start >";
	char str2[] = "Continue boot";
	char *str_target = NULL;
	uint32 fp_size = 0;
	unsigned int i = 0;

	fbcon_clear();
	memset(msg_info, 0, sizeof(struct select_msg_info));

	switch (type) {
	case DISPLAY_MENU_RED:
		msg_type = FBCON_RED_MSG;
		warning_msg = red_warning_msg;
		break;
        case DISPLAY_MENU_YELLOW:
		msg_type = FBCON_YELLOW_MSG;
		warning_msg = yellow_warning_msg;
                break;
	case DISPLAY_MENU_ORANGE:
		msg_type = FBCON_ORANGE_MSG;
		warning_msg = orange_warning_msg;
                break;
	}

	/* Align Right */
	str_target = str_align_right(str1, big_factor);
	if(str_target != NULL)
		display_fbcon_menu_message(str_target, FBCON_TITLE_MSG, big_factor);

	str_target = str_align_right(str2, common_factor);
	if(str_target != NULL)
		display_fbcon_menu_message(str_target, FBCON_TITLE_MSG, common_factor);

	display_fbcon_menu_message("\n< More options\n",
		FBCON_COMMON_MSG, common_factor);
	display_fbcon_menu_message("press VOLUME keys\n\n",
		FBCON_SUBTITLE_MSG, common_factor);
	if(warning_msg != NULL)
		display_fbcon_menu_message(warning_msg, FBCON_COMMON_MSG, common_factor);

	display_fbcon_menu_message("g.co/placeholder\n",
		msg_type, common_factor);

#if VERIFIED_BOOT
	if (type == DISPLAY_MENU_YELLOW) {
		fp_buf = get_boot_fingerprint(&fp_size);
		if (fp_buf != NULL) {
			strncpy(fp_str_temp, (char const *)fp_buf, fp_size);
			for (i = 0; i < fp_size; i++) {
				if(i == fp_size - 1)
					sprintf(str_temp, "%02x", fp_str_temp[i]);
				else
					sprintf(str_temp, "%02x-", fp_str_temp[i]);

				strcat(fp_str, str_temp);
			}
		}
		display_fbcon_menu_message("ID:", FBCON_COMMON_MSG, common_factor);
		display_fbcon_menu_message(fp_str, FBCON_COMMON_MSG, common_factor);
	}
#endif

	display_fbcon_menu_message("\n\nIf no key pressed:\n"\
		"Your device will boot in 5 seconds\n\n", FBCON_COMMON_MSG, common_factor);

	msg_info->msg_type = type;
	if(str_target) {
		free(str_target);
	}
}

void display_boot_verified_option(struct select_msg_info *msg_info)
{
	int i = 0;
	fbcon_clear();
	memset(msg_info, 0, sizeof(struct select_msg_info));

	display_fbcon_menu_message("Options menu:\n\n",
		FBCON_COMMON_MSG, big_factor);
	display_fbcon_menu_message("Press volume key to select, and "\
		"press power key to select\n\n", FBCON_COMMON_MSG, common_factor);

	for (i = 0; i < BOOT_VERIFY_OPTION_NUM; i++) {
		fbcon_draw_line();
		msg_info->option_start[i] = fbcon_get_current_line();
		display_fbcon_menu_message(option_menu[i],
			FBCON_COMMON_MSG, common_factor);
		msg_info->option_bg[i]= fbcon_get_current_bg();
		msg_info->option_end[i]= fbcon_get_current_line();
	}

	fbcon_draw_line();
	msg_info->msg_type = DISPLAY_MENU_MORE_OPTION;
	msg_info->option_num = BOOT_VERIFY_OPTION_NUM;
}

static void display_menu_thread_start(struct select_msg_info *msg_info)
{
	thread_t *thr;

	if (!is_thread_start) {
		thr = thread_create("selectkeydetect", &select_msg_keys_detect,
			(void*)msg_info, DEFAULT_PRIORITY, DEFAULT_STACK_SIZE);
		if (!thr) {
			dprintf(CRITICAL, "ERROR: creat device status detect thread failed!!\n");
			return;
		}
		thread_resume(thr);
	}

	is_thread_start = true;
}

void display_menu_thread(int type)
{
	struct select_msg_info *display_menu_msg_info;
	display_menu_msg_info = &msg_info;
	int menu_type = 0;

	set_message_factor();
	if (type == DISPLAY_THREAD_UNLOCK) {
		display_unlock_menu(display_menu_msg_info);

		dprintf(INFO, "creating oem unlock keys detect thread\n");
		display_menu_thread_start(display_menu_msg_info);
	} else {
	#if VERIFIED_BOOT
		if (boot_verify_get_state() == ORANGE) {
			menu_type = DISPLAY_MENU_ORANGE;
		} else if (boot_verify_get_state() == YELLOW) {
			menu_type = DISPLAY_MENU_YELLOW;
		} else if (boot_verify_get_state() == RED) {
			menu_type = DISPLAY_MENU_RED;
		}
		display_boot_verified_menu(display_menu_msg_info, menu_type);

		dprintf(INFO, "creating device state keys detect thread\n");
		display_menu_thread_start(display_menu_msg_info);

	#else
		dprintf(CRITICAL, "VERIFIED_BOOT is not enable!!\n");
		return;
	#endif
	}
}

