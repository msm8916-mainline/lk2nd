/* Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#ifndef __MMC_WRAPPER_H__
#define __MMC_WRAPPER_H__

#include <mmc_sdhci.h>

#define BOARD_KERNEL_PAGESIZE                2048
/* Wrapper APIs */

struct mmc_device *get_mmc_device();
uint32_t mmc_get_psn(void);

uint32_t mmc_read(uint64_t data_addr, uint32_t *out, uint32_t data_len);
uint32_t mmc_write(uint64_t data_addr, uint32_t data_len, void *in);
uint32_t mmc_erase_card(uint64_t, uint64_t);
uint64_t mmc_get_device_capacity(void);
uint32_t mmc_erase_card(uint64_t addr, uint64_t len);
uint32_t mmc_get_device_blocksize();
uint32_t mmc_page_size();
void mmc_device_sleep();
void mmc_set_lun(uint8_t lun);
uint8_t mmc_get_lun(void);
void  mmc_read_partition_table(uint8_t arg);
uint32_t mmc_write_protect(const char *name, int set_clr);
#endif
