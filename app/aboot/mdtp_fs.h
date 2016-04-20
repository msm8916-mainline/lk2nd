/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.

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

#ifndef MDTP_FS_H
#define MDTP_FS_H

#define MAX_IMAGES                      (40)
#define MDTP_HEADER_LEN                 (4096)
#define META_DATA_PARTITION_LEN         (2048)
#define MAX_PARAMS                      (512)
#define MDTP_PARAM_UNSET_VALUE          (111)
#define SUPPORTED_METADATA_VERSION      (1)

/*
MDTP image layout:
-The mdtp image file contains two layers that both include mdtp_img headers:
	1. The first header includes the image sets metadata.
	2. Once we decided which image set we would like to display, we read the metadata
	   of that specific image set (contains metadata of the actual images).
-The mdtp_img header is a fixed length of 4096 Bytes.
-The mdtp_img header is divided into 2 partitions:
	1.MDTP parameters (eFuse, digit-space, etc..)
	2.Images/image sets meta-data (offset, width, height)
-Each partition size is 2048 Bytes.
-Each parameter is 4 Bytes long, 512 params max.
-Each meta-data parameter (offset/width/height) is 4 Bytes long.
*/


/* Standalone parameters */
typedef struct mdtp_image_params {
	uint32_t offset;
	uint32_t width;
	uint32_t height;
} mdtp_image_params_t;

/* Image parameters */
typedef struct mdtp_meta_data {
	uint32_t params[MAX_PARAMS];
	mdtp_image_params_t image_params[MAX_IMAGES];
} mdtp_meta_data_t;

/*To make sure the header len is exactly MDTP_HEADER */
typedef union mdtp_image {
	mdtp_meta_data_t meta_data;
	uint8_t header[MDTP_HEADER_LEN];
} mdtp_image_t;


typedef enum{ACCEPTEDIT_TEXT = 0, ALERT_MESSAGE = 1, BTN_OK_OFF = 2,
	BTN_OK_ON = 3, MAINTEXT_5SECONDS = 4, MAINTEXT_ENTERPIN = 5,
	MAINTEXT_INCORRECTPIN = 6, PINTEXT = 7,
	PIN_SELECTED_0 = 8, PIN_SELECTED_1 = 9, PIN_SELECTED_2 = 10, PIN_SELECTED_3 = 11,
	PIN_SELECTED_4 = 12, PIN_SELECTED_5 = 13, PIN_SELECTED_6 = 14,
	PIN_SELECTED_7  = 15, PIN_SELECTED_8 = 16, PIN_SELECTED_9 = 17,
	PIN_UNSELECTED_0 = 18, PIN_UNSELECTED_1 = 19, PIN_UNSELECTED_2 = 20,
	PIN_UNSELECTED_3 = 21, PIN_UNSELECTED_4 = 22, PIN_UNSELECTED_5 = 23,
	PIN_UNSELECTED_6 = 24, PIN_UNSELECTED_7 = 25,
	PIN_UNSELECTED_8 = 26, PIN_UNSELECTED_9 = 27} mdtp_image_id_t;

typedef enum {VIRTUAL_FUSE = 0, DIGIT_SPACE = 1, VERSION = 2, TYPE = 3, IMAGE_SETS_NUM = 4} mdtp_parameter_id_t;

typedef enum {IMAGES = 1, IMAGE_SETS = 2} mdtp_metadata_type_t;

/*---------------------------------------------------------
 * External Functions
 *-------------------------------------------------------*/

/**
 * Returns an image offset
 */
uint32_t get_image_offset(mdtp_image_id_t img);

/**
 * Returns an image width
 */
uint32_t get_image_width(mdtp_image_id_t img);

/**
 * Returns an image height
 */
uint32_t get_image_height(mdtp_image_id_t img);

/**
 * returns a parameter value
 */
uint32_t mdtp_fs_get_param(mdtp_parameter_id_t  param);

/**
 * Loads MDTP image meta data from EMMC
 */
int mdtp_fs_init();

#endif /* MDTP_FS_H */
