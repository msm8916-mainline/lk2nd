/* Copyright (c) 2015, The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <dev/fbcon.h>
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <stdlib.h>
#include "mdtp.h"

#define MDTP_IMAGE_WIDTH 500
#define MDTP_IMAGE_HEIGHT 800
#define MDTP_UX_DELAY 1000
#define MDTP_OK_OFFSET 0x0
#define MDTP_CHECKING_OFFSET 0x200000
#define MDTP_INVALID_OFFSET 0x400000
#define MDTP_RECOVERED_OFFSET 0x600000

extern uint32_t target_volume_down(void);
extern void fbcon_putImage(struct fbimage *fbimg, bool flag);
extern void mdelay(unsigned msecs);

static struct fbimage mdtp_header;/* = {0};*/

/********************************************************************************/

/* Load the "Firmware Valid" image from EMMC */
static struct fbimage* mdtp_images_mmc_OK()
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	struct fbcon_config *fb_display = NULL;
	struct fbimage *logo = &mdtp_header;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition table not found\n");
		return NULL;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition invalid\n");
		return NULL;
	}

	fb_display = fbcon_display();
	if (fb_display)
	{
		uint8_t *base = (uint8_t *) fb_display->base;
		base += LOGO_IMG_OFFSET;

		if (mmc_read(ptn, (void*)base, ROUNDUP(MDTP_IMAGE_WIDTH*MDTP_IMAGE_HEIGHT*3, block_size))) {
				fbcon_clear();
				dprintf(CRITICAL, "ERROR: mdtp image read failed\n");
				return NULL;
		}
		logo->image = base;
	}

	return logo;
}

/* Load the "Firmware Invalid" image from EMMC */
static struct fbimage* mdtp_images_mmc_INVALID()
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	struct fbcon_config *fb_display = NULL;
	struct fbimage *logo = &mdtp_header;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition table not found\n");
		return NULL;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition invalid\n");
		return NULL;
	}

	fb_display = fbcon_display();
	if (fb_display)
	{
		uint8_t *base = (uint8_t *) fb_display->base;
		base += LOGO_IMG_OFFSET;

		if (mmc_read(ptn+MDTP_INVALID_OFFSET, (void*)base, ROUNDUP(MDTP_IMAGE_WIDTH*MDTP_IMAGE_HEIGHT*3, block_size))) {
				fbcon_clear();
				dprintf(CRITICAL, "ERROR: mdtp image read failed\n");
				return NULL;
		}
		logo->image = base;
	}

	return logo;
}

/* Load the "Verifying Firmware" image from EMMC */
static struct fbimage* mdtp_images_mmc_CHECKING()
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	struct fbcon_config *fb_display = NULL;
	struct fbimage *logo = &mdtp_header;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition table not found\n");
		return NULL;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition invalid\n");
		return NULL;
	}

	fb_display = fbcon_display();
	if (fb_display)
	{
		uint8_t *base = (uint8_t *) fb_display->base;
		base += LOGO_IMG_OFFSET;

		if (mmc_read(ptn+MDTP_CHECKING_OFFSET, (void*)base, ROUNDUP(MDTP_IMAGE_WIDTH*MDTP_IMAGE_HEIGHT*3, block_size))) {
				fbcon_clear();
				dprintf(CRITICAL, "ERROR: mdtp image read failed\n");
				return NULL;
		}
		logo->image = base;
	}

	return logo;
}

/* Load the "Verifying Firmware" image from EMMC */
static struct fbimage* mdtp_images_mmc_RECOVERED()
{
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	struct fbcon_config *fb_display = NULL;
	struct fbimage *logo = &mdtp_header;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition table not found\n");
		return NULL;
	}

	ptn = partition_get_offset(index);
	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition invalid\n");
		return NULL;
	}

	fb_display = fbcon_display();
	if (fb_display)
	{
		uint8_t *base = (uint8_t *) fb_display->base;
		base += LOGO_IMG_OFFSET;

		if (mmc_read(ptn+MDTP_RECOVERED_OFFSET, (void*)base, ROUNDUP(MDTP_IMAGE_WIDTH*MDTP_IMAGE_HEIGHT*3, block_size))) {
				fbcon_clear();
				dprintf(CRITICAL, "ERROR: mdtp image read failed\n");
				return NULL;
		}
		logo->image = base;
	}

	return logo;
}

/* Show the "Firmware Valid" image */
static void display_image_on_screen_OK()
{
	struct fbimage *fbimg;

	fbcon_clear();
	fbimg = mdtp_images_mmc_OK();
	fbimg->header.width = MDTP_IMAGE_WIDTH;
	fbimg->header.height = MDTP_IMAGE_HEIGHT;

	dprintf(CRITICAL, "display_image_on_screen_OK\n");
	fbcon_putImage(fbimg, true);
}

/* Show the "Firmware Invalid" image */
static void display_image_on_screen_INVALID()
{
	struct fbimage *fbimg;

	fbcon_clear();
	fbimg = mdtp_images_mmc_INVALID();
	fbimg->header.width = MDTP_IMAGE_WIDTH;
	fbimg->header.height = MDTP_IMAGE_HEIGHT;

	dprintf(CRITICAL, "display_image_on_screen_INVALID\n");
	fbcon_putImage(fbimg, true);
}

/* Show the "Verifying Firmware" image */
static void display_image_on_screen_CHECKING()
{
	struct fbimage *fbimg;

	fbcon_clear();
	fbimg = mdtp_images_mmc_CHECKING();
	fbimg->header.width = MDTP_IMAGE_WIDTH;
	fbimg->header.height = MDTP_IMAGE_HEIGHT;

	dprintf(CRITICAL, "display_image_on_screen_CHECKING\n");
	fbcon_putImage(fbimg, true);
}

/* Show the "Verifying Firmware" image */
static void display_image_on_screen_RECOVERED()
{
	struct fbimage *fbimg;

	fbcon_clear();
	fbimg = mdtp_images_mmc_RECOVERED();
	fbimg->header.width = MDTP_IMAGE_WIDTH;
	fbimg->header.height = MDTP_IMAGE_HEIGHT;

	dprintf(CRITICAL, "display_image_on_screen_RECOVERED\n");
	fbcon_putImage(fbimg, true);
}

/* Display the "Firmware Valid" screen */
void show_OK_msg()
{
	display_image_on_screen_OK();
	mdelay(MDTP_UX_DELAY);

	return;
}

/* Display the "Firmware Invalid" screen */
void show_invalid_msg()
{
	display_image_on_screen_INVALID();

	while (1)
	{
		if(target_volume_down())
		{
			display_image_on_screen_RECOVERED();
			mdelay(MDTP_UX_DELAY);
			break;
		}
	}

	return;
}

/* Display the "Verifying Firmware" screen */
void show_checking_msg()
{
	display_image_on_screen_CHECKING();
	return;
}

/* Display the "Verifying Firmware" screen */
void show_recovered_msg()
{
	display_image_on_screen_RECOVERED();
	return;
}

