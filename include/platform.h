/*
 * Copyright (c) 2008 Travis Geiselbrecht
 *
 * Copyright (c) 2014-2017, The Linux Foundation. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef __PLATFORM_H
#define __PLATFORM_H

#include <dload_util.h>

#define PA(x) platform_get_virt_to_phys_mapping(x)
#define VA(x) platform_get_phys_to_virt_mapping(x)

time_t current_time(void);
bigtime_t current_time_hires(void);

/* super early platform initialization, before almost everything */
void platform_early_init(void);

/* later init, after the kernel has come up */
void platform_init(void);

/* called by the arch init code to get the platform to set up any mmu mappings it may need */
int platform_use_identity_mmu_mappings(void);
void platform_init_mmu_mappings(void);
addr_t platform_get_virt_to_phys_mapping(addr_t virt_addr);
addr_t platform_get_phys_to_virt_mapping(addr_t phys_addr);
addr_t get_bs_info_addr(void);

void display_init(void);
void display_shutdown(void);
void display_image_on_screen(void);
void display_fbcon_message(char *str);

unsigned board_machtype(void);
unsigned board_platform_id(void);
unsigned check_reboot_mode(void);
unsigned check_hard_reboot_mode(void);
uint32_t check_alarm_boot(void);
void platform_uninit_timer(void);
void reboot_device(unsigned);
int set_download_mode(enum reboot_reason mode);
uint32_t platform_get_smem_base_addr();
uint32_t platform_get_sclk_count(void);
void clock_config_cdc(uint32_t interface);
int platform_is_msm8939();
int platform_is_msm8909();
int platform_is_msm8992();
int platform_is_msm8937();
int platform_is_msm8917();
uint32_t platform_get_apcs_ipc_base();
int platform_is_msm8952();
int platform_is_msm8953();
int platform_is_msm8956();
uint32_t platform_is_msm8976_v_1_1();
uint32_t platform_get_tz_app_add();
uint32_t platform_get_tz_app_size();
int boot_device_mask(int);
uint32_t platform_detect_panel();
uint32_t platform_get_max_periph();
int platform_is_msm8996();
int platform_is_apq8096_mediabox();
bool platform_use_qmp_misc_settings();
void set_device_unlock_value(int type, bool status);
void get_product_name(unsigned char *buf);
void get_bootloader_version(unsigned char *buf);
void get_baseband_version(unsigned char *buf);
bool is_device_locked();
bool platform_is_mdm9650();
bool platform_is_sdx20();
uint64_t platform_get_ddr_start();
bool platform_is_glink_enabled();
bool platform_is_mdm9206();
int is_vb_le_enabled();
#endif
