/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2015, 2018-2020 The Linux Foundation. All rights reserved.
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

#include <debug.h>
#include <err.h>
#include <stdlib.h>
#include <dev/fbcon.h>
#include <splash.h>
#include <platform.h>
#include <string.h>
#include <arch/ops.h>
#if ENABLE_WBC
#include <pm_smbchg_common.h>
#endif

#include "font5x12.h"

struct pos {
	int x;
	int y;
};

struct fb_color {
	uint32_t fg;
	uint32_t bg;
};

static struct fbcon_config *config = NULL;

#define RGB565_BLACK		0x0000
#define RGB565_WHITE		0xffff
#define RGB565_CYAN		0x07ff
#define RGB565_BLUE		0x001f
#define RGB565_SILVER		0xc618
#define RGB565_YELLOW		0xffe0
#define RGB565_ORANGE		0xfd20
#define RGB565_RED		0xf800
#define RGB565_GREEN		0x3666

#define RGB888_BLACK            0x000000
#define RGB888_WHITE            0xffffff
#define RGB888_CYAN             0x00ffff
#define RGB888_BLUE             0x0000FF
#define RGB888_SILVER           0xc0c0c0
#define RGB888_YELLOW           0xffff00
#define RGB888_ORANGE           0xffa500
#define RGB888_RED              0xff0000
#define RGB888_GREEN            0x00ff00

#define FONT_WIDTH		5
#define FONT_HEIGHT		12

#define SCALE_FACTOR		2

static uint32_t			last_scale_factor;
static uint32_t			BGCOLOR;
static uint32_t			FGCOLOR;
static uint32_t			SELECT_BGCOLOR;

static struct pos		cur_pos;
static struct pos		max_pos;
static struct fb_color		*fb_color_formats;
static struct fb_color		fb_color_formats_555[] = {
					[FBCON_COMMON_MSG] = {RGB565_WHITE, RGB565_BLACK},
					[FBCON_UNLOCK_TITLE_MSG] = {RGB565_RED, RGB565_BLACK},
					[FBCON_TITLE_MSG] = {RGB565_WHITE, RGB565_BLACK},
					[FBCON_SUBTITLE_MSG] = {RGB565_SILVER, RGB565_BLACK},
					[FBCON_YELLOW_MSG] = {RGB565_YELLOW, RGB565_BLACK},
					[FBCON_ORANGE_MSG] = {RGB565_ORANGE, RGB565_BLACK},
					[FBCON_RED_MSG] = {RGB565_RED, RGB565_BLACK},
					[FBCON_GREEN_MSG] = {RGB565_GREEN, RGB565_BLACK},
					[FBCON_SELECT_MSG_BG_COLOR] = {RGB565_WHITE, RGB565_BLUE}};

static struct fb_color		fb_color_formats_888[] = {
					[FBCON_COMMON_MSG] = {RGB888_WHITE, RGB888_BLACK},
					[FBCON_UNLOCK_TITLE_MSG] = {RGB888_RED, RGB888_BLACK},
					[FBCON_TITLE_MSG] = {RGB888_WHITE, RGB888_BLACK},
					[FBCON_SUBTITLE_MSG] = {RGB888_SILVER, RGB888_BLACK},
					[FBCON_YELLOW_MSG] = {RGB888_YELLOW, RGB888_BLACK},
					[FBCON_ORANGE_MSG] = {RGB888_ORANGE, RGB888_BLACK},
					[FBCON_RED_MSG] = {RGB888_RED, RGB888_BLACK},
					[FBCON_GREEN_MSG] = {RGB888_GREEN, RGB888_BLACK},
					[FBCON_SELECT_MSG_BG_COLOR] = {RGB888_WHITE, RGB888_BLUE}};


static void fbcon_drawglyph(char *pixels, uint32_t paint, unsigned stride,
			    unsigned bpp, unsigned *glyph, unsigned scale_factor)
{
	unsigned x, y, i, j, k;
	unsigned data, temp;
	uint32_t fg_color = paint;
	stride -= FONT_WIDTH * scale_factor;
	last_scale_factor = scale_factor;

	data = glyph[0];
	for (y = 0; y < FONT_HEIGHT / 2; ++y) {
		temp = data;
		for (i = 0; i < scale_factor; i++) {
			data = temp;
			for (x = 0; x < FONT_WIDTH; ++x) {
				if (data & 1) {
					for (j = 0; j < scale_factor; j++) {
						fg_color = paint;
						for (k = 0; k < bpp; k++) {
							*pixels = (unsigned char) fg_color;
							fg_color = fg_color >> 8;
							pixels++;
						}
					}
				}
				else
				{
					for (j = 0; j < scale_factor; j++) {
						pixels = pixels + bpp;
					}
				}
				data >>= 1;
			}
			pixels += (stride * bpp);
		}
	}

	data = glyph[1];
	for (y = 0; y < FONT_HEIGHT / 2; ++y) {
		temp = data;
		for (i = 0; i < scale_factor; i++) {
			data = temp;
			for (x = 0; x < FONT_WIDTH; ++x) {
				if (data & 1) {
					for (j = 0; j < scale_factor; j++) {
						fg_color = paint;
						for (k = 0; k < bpp; k++) {
							*pixels = (unsigned char) fg_color;
							fg_color = fg_color >> 8;
							pixels++;
						}
					}
				}
				else
				{
					for (j = 0; j < scale_factor; j++) {
						pixels = pixels + bpp;
					}
				}
				data >>= 1;
			}
			pixels += (stride * bpp);
		}
	}

}

void fbcon_draw_msg_background(unsigned y_start, unsigned y_end,
	uint32_t old_paint, int update)
{
	unsigned i, j;
	uint32_t bg_color, check_color, tmp_color, tmp1_color;
	char *pixels;
	unsigned count;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	count = config->width * (FONT_HEIGHT * (y_end - y_start) - 1);
	pixels = config->base;
	pixels += y_start * ((config->bpp / 8) * FONT_HEIGHT * config->width);

	if (update) {
		bg_color = SELECT_BGCOLOR;
		check_color = old_paint;
	} else {
		bg_color = old_paint;
		check_color = SELECT_BGCOLOR;
	}

	for (i = 0; i < count; i++) {
		tmp1_color = bg_color;
		tmp_color = 0;
		for (j = 0; j < (config->bpp / 8); j++) {
			tmp_color |= *(pixels+j) << j*8;
		}

		if (tmp_color == check_color) {
			for (j = 0; j < (config->bpp / 8); j++) {
				*pixels = (unsigned char) tmp1_color;
				tmp1_color = tmp1_color >> 8;
				pixels++;
			}
		} else {
			pixels += config->bpp / 8;
		}
	}
	fbcon_flush();
}

void fbcon_flush(void)
{
	unsigned total_x, total_y;
	unsigned bytes_per_bpp;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	if (config->update_start)
		config->update_start();
	if (config->update_done)
		while (!config->update_done());

	total_x = config->width;
	total_y = config->height;
	bytes_per_bpp = ((config->bpp) / 8);
	arch_clean_invalidate_cache_range((addr_t) config->base, (total_x * total_y * bytes_per_bpp));
}

static void fbcon_scroll_up(void)
{
	uint8_t *dst = NULL;
	uint8_t *src = NULL;
	unsigned bpp = config->bpp / 8;
	unsigned font_h = FONT_HEIGHT * last_scale_factor;
	unsigned count = 0;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	dst = config->base;
	src = dst + (config->width * font_h * bpp);
	count = config->width * (config->height - font_h) * bpp;
	memmove(dst, src, count);

	dst += count;
	count = config->width * font_h * bpp;
	memset(dst, 0, count); /* FIXME: ignores color */

	fbcon_flush();
}

void fbcon_draw_line(uint32_t type)
{
	char *pixels;
	uint32_t line_color, tmp_color;
	int i, j;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	/* set line's color via diffrent type */
	line_color = fb_color_formats[type].fg;

	pixels = config->base;
	pixels += cur_pos.y * ((config->bpp / 8) * FONT_HEIGHT * config->width);
	pixels += cur_pos.x * ((config->bpp / 8) * (FONT_WIDTH + 1));

	for (i = 0; i < (int)config->width; i++) {
		tmp_color = line_color;
		for (j = 0; j < (int)(config->bpp / 8); j++) {
			*pixels = (unsigned char) tmp_color;
			tmp_color = tmp_color >> 8;
			pixels++;
		}
	}

	cur_pos.y += 1;
	cur_pos.x = 0;
	if(cur_pos.y >= max_pos.y) {
		cur_pos.y = max_pos.y - 1;
		fbcon_scroll_up();
	} else
		fbcon_flush();
}

static void fbcon_set_colors(int type)
{
	BGCOLOR = fb_color_formats[type].bg;
	FGCOLOR = fb_color_formats[type].fg;
}

void fbcon_clear(void)
{
	unsigned long i = 0, j = 0;
	unsigned char *pixels = NULL;
	unsigned count;
	uint32_t bg_color;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	pixels = config->base;
	count =  config->width * config->height;

	fbcon_set_colors(FBCON_COMMON_MSG);
	for (i = 0; i < count; i++) {
		bg_color = BGCOLOR;
		for (j = 0; j < (config->bpp / 8); j++) {
			*pixels = (unsigned char) bg_color;
			bg_color = bg_color >> 8;
			pixels++;
		}
	}
	cur_pos.x = 0;
	cur_pos.y = 0;
}

void fbcon_clear_msg(unsigned y_start, unsigned y_end)
{
	unsigned i, j;
	uint32_t bg_color;
	char *pixels;
	unsigned count;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	count = config->width * (FONT_HEIGHT * (y_end - y_start) - 1);
	pixels = config->base;
	pixels += y_start * ((config->bpp / 8) * FONT_HEIGHT * config->width);

	fbcon_set_colors(FBCON_COMMON_MSG);
	for (i = 0; i < count; i++) {
		bg_color = BGCOLOR;
		for (j = 0; j < (config->bpp / 8); j++) {
			*pixels = (unsigned char) bg_color;
			bg_color = bg_color >> 8;
			pixels++;
		}
	}
}

void fbcon_putc_factor_xy(char c, int type, unsigned scale_factor, int x, int y)
{
	char *pixels;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	if((unsigned char)c > 127 || (unsigned char)c < 32)
		return;

	fbcon_set_colors(type);

	pixels = config->base;
	pixels += y * (config->bpp / 8) * config->width;
	pixels += x * (config->bpp / 8);

	fbcon_drawglyph(pixels, FGCOLOR, config->stride, (config->bpp / 8),
			font5x12 + (c - 32) * 2, scale_factor);
}

void fbcon_putc_factor(char c, int type, unsigned scale_factor, int y_start)
{
	char *pixels;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	if((unsigned char)c > 127)
		return;

	if((unsigned char)c < 32) {
		if(c == '\n')
			goto newline;
		else if (c == '\r') {
			cur_pos.x = 0;
			return;
		}
		else
			return;
	}

	if (cur_pos.x == 0 && (unsigned char)c == ' ' &&
		type != FBCON_SUBTITLE_MSG &&
		type != FBCON_TITLE_MSG)
		return;

	fbcon_set_colors(type);

	pixels = config->base;
	/* if y_start is null, it will start from current y loaction */
	if (y_start) {
		cur_pos.x = 0;
		cur_pos.y = y_start;
	}
	pixels += cur_pos.y * ((config->bpp / 8) * FONT_HEIGHT * config->width);
	pixels += cur_pos.x * scale_factor * ((config->bpp / 8) * (FONT_WIDTH + 1));

	fbcon_drawglyph(pixels, FGCOLOR, config->stride, (config->bpp / 8),
			font5x12 + (c - 32) * 2, scale_factor);

	cur_pos.x++;
	if (cur_pos.x >= (int)(max_pos.x / scale_factor))
		goto newline;

	return;

newline:
	cur_pos.y += scale_factor;
	cur_pos.x = 0;
	if(cur_pos.y >= max_pos.y) {
		cur_pos.y = max_pos.y - 1;
		fbcon_scroll_up();
	} else
		fbcon_flush();
}

void fbcon_putc(char c, int y_start)
{
	fbcon_putc_factor(c, FBCON_COMMON_MSG, SCALE_FACTOR, y_start);
}

uint32_t fbcon_get_current_line(void)
{
	return cur_pos.y;
}

uint32_t fbcon_get_max_x(void)
{
	return max_pos.x;
}

uint32_t fbcon_get_width(void)
{
	if (config)
		return config->width;
	else
		return 0;
}

uint32_t fbcon_get_height(void)
{
	if (config)
		return config->height;
	else
		return 0;
}

uint32_t fbcon_get_current_bg(void)
{
	return BGCOLOR;
}

void fbcon_setup(struct fbcon_config *_config)
{
	ASSERT(_config);

	config = _config;

	switch (config->format) {
	case FB_FORMAT_RGB565:
		fb_color_formats = fb_color_formats_555;
		break;
        case FB_FORMAT_RGB888:
		fb_color_formats = fb_color_formats_888;
                break;
	default:
		dprintf(CRITICAL, "unknown framebuffer pixel format\n");
		ASSERT(0);
		break;
	}

	SELECT_BGCOLOR = fb_color_formats[FBCON_SELECT_MSG_BG_COLOR].bg;
	fbcon_set_colors(FBCON_COMMON_MSG);

	cur_pos.x = 0;
	cur_pos.y = 0;
	max_pos.x = config->width / (FONT_WIDTH+1);
	max_pos.y = (config->height - 1) / FONT_HEIGHT - 1;

#if !DISPLAY_SPLASH_SCREEN
	fbcon_clear();
#endif

}

struct fbcon_config* fbcon_display(void)
{
	return config;
}

void fbcon_extract_to_screen(logo_img_header *header, void* address)
{
	const uint8_t *imagestart = (const uint8_t *)address;
	uint pos = 0, offset;
	uint count = 0;
	uint x = 0, y = 0;
	uint8_t *base, *p;

	if (!config || header->width > config->width
				|| header->height > config->height) {
		dprintf(INFO, "the logo img is too large\n");
		return;
	}

	base = (uint8_t *) config->base;

	/* put the logo to be center */
	offset = (config->height - header->height) / 2;
	if (offset)
		base += (offset * config->width) * 3;
	offset = (config->width - header->width ) / 2;

	x = offset;
	while (count < (uint)header->height * (uint)header->width) {
		uint8_t run = *(imagestart + pos);
		bool repeat_run = (run & 0x80);
		uint runlen = (run & 0x7f) + 1;
		uint runpos;

		/* consume the run byte */
		pos++;

		p = base + (y * config->width + x) * 3;

		/* start of a run */
		for (runpos = 0; runpos < runlen; runpos++) {
			*p++ = *(imagestart + pos);
			*p++ = *(imagestart + pos + 1);
			*p++ = *(imagestart + pos + 2);
			count++;

			x++;

			/* if a run of raw pixels, consume an input pixel */
			if (!repeat_run)
				pos += 3;
		}

		/* if this was a run of repeated pixels, consume the one input pixel we repeated */
		if (repeat_run)
			pos += 3;

		/* the generator will keep compressing data line by line */
		/* don't cross the lines */
		if (x == header->width + offset) {
			y++;
			x = offset;
		}
	}

}

__WEAK void display_default_image_on_screen(void)
{
	unsigned i = 0;
	unsigned total_x;
	unsigned total_y;
	unsigned bytes_per_bpp;
	unsigned image_base;
	char *image = NULL;

	if (!config) {
		dprintf(CRITICAL,"NULL configuration, image cannot be displayed\n");
		return;
	}

	fbcon_clear(); // clear screen with Black color

	total_x = config->width;
	total_y = config->height;
	bytes_per_bpp = ((config->bpp) / 8);
	image_base = ((((total_y/2) - (SPLASH_IMAGE_HEIGHT / 2) - 1) *
			(config->width)) + (total_x/2 - (SPLASH_IMAGE_WIDTH / 2)));

#if ENABLE_WBC
	image = (pm_appsbl_charging_in_progress() ? image_batt888 : imageBuffer_rgb888);
#else
	image = imageBuffer_rgb888;
#endif

	if (bytes_per_bpp == 3) {
		for (i = 0; i < SPLASH_IMAGE_HEIGHT; i++) {
			memcpy (config->base + ((image_base + (i * (config->width))) * bytes_per_bpp),
			image + (i * SPLASH_IMAGE_WIDTH * bytes_per_bpp),
			SPLASH_IMAGE_WIDTH * bytes_per_bpp);
		}
	}

	if (bytes_per_bpp == 2) {
		for (i = 0; i < SPLASH_IMAGE_HEIGHT; i++) {
			memcpy (config->base + ((image_base + (i * (config->width))) * bytes_per_bpp),
			imageBuffer + (i * SPLASH_IMAGE_WIDTH * bytes_per_bpp),
			SPLASH_IMAGE_WIDTH * bytes_per_bpp);
		}
	}

	fbcon_flush();

#if DISPLAY_MIPI_PANEL_NOVATEK_BLUE
	if(is_cmd_mode_enabled())
		mipi_dsi_cmd_mode_trigger();
#endif
}


void display_image_on_screen(void)
{
#if DISPLAY_TYPE_MIPI && !DISABLE_SPLASH_PARTITION
	if (fetch_image_from_partition() < 0) {
		display_default_image_on_screen();
	} else {
		/* data has been put into the right place */
		fbcon_flush();
	}
#else
	display_default_image_on_screen();
#endif
}
