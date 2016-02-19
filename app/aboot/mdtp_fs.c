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
	int index = INVALID_PTN;
	unsigned long long ptn = 0;
	int i = 0;
	uint32_t block_size = mmc_get_device_blocksize();

	index = partition_get_index("mdtp");
	if (index == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition table not found\n");
		return 1;
	}

	ptn = partition_get_offset(index);
	mmc_set_lun(partition_get_lun(index));

	if (ptn == 0) {
		dprintf(CRITICAL, "ERROR: mdtp Partition invalid\n");
		return 1;
	}

	for(i = 0; i< MAX_PARAMS; i++) {
		mdtp_img.meta_data.params[i] = -1; //Initiate params for errors check
	}

	uint8_t *base = memalign(block_size, ROUNDUP(MDTP_HEADER_LEN, block_size));
	if (!base) {
		dprintf(CRITICAL, "ERROR: mdtp malloc failed\n");
		return 1;
	}

	// read image meta data
	if (mmc_read(ptn, (void*)base, MDTP_HEADER_LEN)) {
		dprintf(CRITICAL, "ERROR: mdtp meta data read failed\n");
		free(base);
		return 1;
	}

	uint32_t params_size = MAX_PARAMS * sizeof(uint32_t);
	uint32_t images_params_size = MAX_IMAGES*sizeof(mdtp_image_params_t);
	memscpy(mdtp_img.meta_data.params, sizeof(mdtp_img.meta_data.params), base, params_size);
	memscpy(mdtp_img.meta_data.image_params, META_DATA_PARTITION_LEN,
			base + sizeof(mdtp_img.meta_data.params), images_params_size);

	dprintf(INFO, "mdtp: mdtp_img loaded\n");

	free(base);
	return 0;
}

