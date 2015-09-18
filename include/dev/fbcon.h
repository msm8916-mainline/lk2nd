/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2015, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef __DEV_FBCON_H
#define __DEV_FBCON_H

#include <stdint.h>
#define LOGO_IMG_OFFSET (12*1024*1024)
#define LOGO_IMG_MAGIC "SPLASH!!"
#define LOGO_IMG_MAGIC_SIZE sizeof(LOGO_IMG_MAGIC) - 1
#define LOGO_IMG_HEADER_SIZE 512

enum fbcon_msg_type {
	/* type for menu */
	FBCON_COMMON_MSG = 0,
	FBCON_UNLOCK_TITLE_MSG,
	FBCON_TITLE_MSG,
	FBCON_SUBTITLE_MSG,

	/* type for warning */
	FBCON_YELLOW_MSG,
	FBCON_ORANGE_MSG,
	FBCON_RED_MSG,
	FBCON_GREEN_MSG,

	/* and the select message's background */
	FBCON_SELECT_MSG_BG_COLOR,
};

typedef struct logo_img_header {
	unsigned char magic[LOGO_IMG_MAGIC_SIZE]; // "SPLASH!!"
	uint32_t width;  // logo's width, little endian
	uint32_t height; // logo's height, little endian
	uint32_t type;   // 0, Raw BGR data; 1, RLE24 Compressed data
	uint32_t blocks; // block number, compressed data size / 512
	uint32_t offset;
	uint8_t  reserved[512-28];
}logo_img_header;

struct fbimage {
	struct logo_img_header header;
	void *image;
};

#define FB_FORMAT_RGB565 0
#define FB_FORMAT_RGB666 1
#define FB_FORMAT_RGB666_LOOSE 2
#define FB_FORMAT_RGB888 3

struct fbcon_config {
	void		*base;
	unsigned	width;
	unsigned	height;
	unsigned	stride;
	unsigned	bpp;
	unsigned	format;

	void		(*update_start)(void);
	int		(*update_done)(void);
};

void fbcon_setup(struct fbcon_config *cfg);
void fbcon_putc(char c);
void fbcon_clear(void);
struct fbcon_config* fbcon_display(void);
void fbcon_extract_to_screen(logo_img_header *header, void* address);
void fbcon_putc_factor(char c, int type, unsigned scale_factor);
void fbcon_draw_msg_background(unsigned y_start, unsigned y_end,
	uint32_t paint, int update);
void fbcon_draw_line(uint32_t type);
uint32_t fbcon_get_current_line(void);
uint32_t fbcon_get_current_bg(void);
uint32_t fbcon_get_max_x(void);
#endif /* __DEV_FBCON_H */
