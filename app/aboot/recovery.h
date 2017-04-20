/* Copyright (c) 2010-2017, The Linux Foundation. All rights reserved.

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

#ifndef _BOOTLOADER_RECOVERY_H
#define _BOOTLOADER_RECOVERY_H

#define UPDATE_MAGIC         "MSM-RADIO-UPDATE"
#define UPDATE_MAGIC_SIZE    16
#define UPDATE_VERSION       0x00010000
#define FFBM_MODE_BUF_SIZE   8
#define BOOTSELECT_SIGNATURE ('B' | ('S' << 8) | ('e' << 16) | ('l' << 24))
#define BOOTSELECT_VERSION   0x00010001
#define BOOTSELECT_FORMAT    (1 << 31)
#define BOOTSELECT_FACTORY   (1 << 30)

/* bootselect partition format structure */
struct boot_selection_info {
	uint32_t signature;                // Contains value BOOTSELECT_SIGNATURE defined above
	uint32_t version;
	uint32_t boot_partition_selection; // Decodes which partitions to boot: 0-Windows,1-Android
	uint32_t state_info;               // Contains factory and format bit as definded above
};

/* Recovery Message */
struct recovery_message {
	char command[32];
	char status[32];
	char recovery[1024];
};


struct update_header {
	unsigned char MAGIC[UPDATE_MAGIC_SIZE];

	unsigned version;
	unsigned size;

	unsigned image_offset;
	unsigned image_length;

	unsigned bitmap_width;
	unsigned bitmap_height;
	unsigned bitmap_bpp;

	unsigned busy_bitmap_offset;
	unsigned busy_bitmap_length;

	unsigned fail_bitmap_offset;
	unsigned fail_bitmap_length;
};

int write_misc(unsigned page_offset, void *buf, unsigned size);

int get_recovery_message(struct recovery_message *out);
int set_recovery_message(const struct recovery_message *in);

int recovery_init (void);
/* This function will look for the ffbm cookie in the misc partition.
 * Upon finding a valid cookie it will return 1 and place the cookie
 * into ffbm.If it does not find a valid cookie it will return 0.If
 * an error is hit it will return -1.If either of these return values
 * is seen the data in ffbm should not be used and should be considered
 * invalid.
 */
int get_ffbm(char *ffbm, unsigned size);

extern unsigned boot_into_recovery;

#endif
