/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <debug.h>
#include <dev/fbcon.h>
#include <target.h>
#include <mmc.h>
#include <partition_parser.h>
#include <string.h>
#include "mdtp.h"
#include "mdtp_fs.h"

/*---------------------------------------------------------
 * Global Variables
 *-------------------------------------------------------*/
static mdtp_image_t mdtp_img;

/*---------------------------------------------------------
 * Internal Functions
 *-------------------------------------------------------*/

static int mdtp_read_metadata(uint32_t offset){
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	int i = 0;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition table not found\n");
		return -1;
	}

	ptn = partition_get_offset(index);
	mmc_set_lun(partition_get_lun(index));

	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition invalid\n");
		return -1;
	}

	for(i = 0; i< MAX_PARAMS; i++) {
		mdtp_img.meta_data.params[i] = MDTP_PARAM_UNSET_VALUE; //Initialize params for error check
	}

	uint8_t *base = memalign(block_size, ROUNDUP(MDTP_HEADER_LEN, block_size));
	if (!base) {
		dprintf(CRITICAL, "ERROR: mdtp malloc failed\n");
		return -1;
	}

	// read image meta data
	if (mmc_read(ptn+offset, (void*)base, MDTP_HEADER_LEN)) {
		dprintf(CRITICAL, "ERROR: mdtp meta data read failed\n");
		free(base);
		return -1;
	}

	uint32_t params_size = MAX_PARAMS * sizeof(uint32_t);
	uint32_t images_params_size = MAX_IMAGES*sizeof(mdtp_image_params_t);
	memscpy(mdtp_img.meta_data.params, sizeof(mdtp_img.meta_data.params), base, params_size);
	memscpy(mdtp_img.meta_data.image_params, META_DATA_PARTITION_LEN,
			base + sizeof(mdtp_img.meta_data.params), images_params_size);

	dprintf(INFO, "mdtp: metadata loaded\n");

	free(base);
	return 0;
}

static void mdtp_fs_set_param(mdtp_parameter_id_t param, uint32_t value){
	mdtp_img.meta_data.params[param] = value;
}

/*---------------------------------------------------------
 * External Functions
 *-------------------------------------------------------*/

uint32_t get_image_offset(mdtp_image_id_t img){
	return mdtp_img.meta_data.image_params[img].offset;
}

uint32_t get_image_width(mdtp_image_id_t img){
	return mdtp_img.meta_data.image_params[img].width;
}

uint32_t get_image_height(mdtp_image_id_t img){
	return mdtp_img.meta_data.image_params[img].height;
}

uint32_t mdtp_fs_get_param(mdtp_parameter_id_t param){
	return mdtp_img.meta_data.params[param];
}

int mdtp_fs_init(){
	uint32_t i;
	struct fbcon_config *fb_config = NULL;
	mdtp_image_params_t image_params;
	mdtp_image_t mdtp_image_sets_metadata;
	uint32_t image_sets_num;
	uint32_t metadata_offset = 0;

	// Read image sets metadata
	if (mdtp_read_metadata(metadata_offset)){
		dprintf(CRITICAL, "ERROR: failed to read image sets metadata\n");
		return -1;
	}

	// Verify that metadata version is supported
	if (mdtp_fs_get_param(VERSION) != SUPPORTED_METADATA_VERSION){
		dprintf(CRITICAL, "ERROR: metadata version is not supported: %d\n", mdtp_fs_get_param(VERSION));
		return -1;
	}

	// Verify that metadata version is as expected
	if (mdtp_fs_get_param(TYPE) != IMAGE_SETS){
		dprintf(CRITICAL, "ERROR: unexpected type for image sets metadata: %d\n", mdtp_fs_get_param(TYPE));
		return -1;
	}

	image_sets_num = mdtp_fs_get_param(IMAGE_SETS_NUM);
	if (image_sets_num < 1) {
		dprintf(CRITICAL, "ERROR: invalid number of image sets: %d\n", image_sets_num);
		return -1;
	}

	// Image sets are sorted by screen resolution (width, height), from low to high.
	// We begin with the smallest image set, and check if bigger image sets also fit the screen.
	image_params = mdtp_img.meta_data.image_params[0];

	// Get screen configuration
	fb_config = fbcon_display();

	if (fb_config){

		for (i=1; i<image_sets_num; i++){

			//if both width and height still fit the screen, update image_params
			if (mdtp_img.meta_data.image_params[i].width <= fb_config->width &&
				mdtp_img.meta_data.image_params[i].height <= fb_config->height)
			{
				image_params = mdtp_img.meta_data.image_params[i];
			}

			// if we reached an image set in which the width is larger than
			// the screen width, no point in checking additional image sets.
			else if (mdtp_img.meta_data.image_params[i].width > fb_config->width)
				break;
		}

		dprintf(INFO, "mdtp: image set offset: 0x%x\n", image_params.offset);
		dprintf(INFO, "mdtp: image set width: %d, screen width: %d\n", image_params.width, fb_config->width);
		dprintf(INFO, "mdtp: image set height: %d, screen height: %d\n", image_params.height, fb_config->height);
	}
	else {
		// Display configuration is not available.
		// This will cause an actual error only when (and if) trying to display MDTP images.
		dprintf(INFO,"mdtp: fbcon_config struct is NULL\n");
	}

	// Backup image sets metadata for required parameters
	mdtp_image_sets_metadata = mdtp_img;

	// Read images metadata
	if (mdtp_read_metadata(image_params.offset)){
		dprintf(CRITICAL, "ERROR: failed to read images metadata\n");
		return -1;
	}

	// Verify that metadata version is supported
	if (mdtp_fs_get_param(VERSION) != SUPPORTED_METADATA_VERSION){
		dprintf(CRITICAL, "ERROR: metadata version is not supported: %d\n", mdtp_fs_get_param(VERSION));
		return -1;
	}

	// Verify that metadata version is as expected
	if (mdtp_fs_get_param(TYPE) != IMAGES){
		dprintf(CRITICAL, "ERROR: unexpected type for images metadata: %d\n", mdtp_fs_get_param(TYPE));
		return -1;
	}

	// Copy eFuse from image sets metadata
	mdtp_fs_set_param(VIRTUAL_FUSE, mdtp_image_sets_metadata.meta_data.params[VIRTUAL_FUSE]);

	dprintf(INFO, "mdtp: mdtp_img loaded\n");

	return 0;
}

