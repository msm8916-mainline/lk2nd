/*
 * Copyright (c) 2008, Google Inc.
 * All rights reserved.
 *
 * Copyright (c) 2009-2011, Code Aurora Forum. All rights reserved.
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

#include "font5x12.h"

struct pos {
	int x;
	int y;
};

static struct fbcon_config *config = NULL;

#define RGB565_BLACK		0x0000
#define RGB565_WHITE		0xffff

#define RGB888_BLACK            0x000000
#define RGB888_WHITE            0xffffff

#define FONT_WIDTH		5
#define FONT_HEIGHT		12

static uint16_t			BGCOLOR;
static uint16_t			FGCOLOR;

static struct pos		cur_pos;
static struct pos		max_pos;

static void fbcon_drawglyph(uint16_t *pixels, uint16_t paint, unsigned stride,
			    unsigned *glyph)
{
	unsigned x, y, data;
	stride -= FONT_WIDTH;

	data = glyph[0];
	for (y = 0; y < (FONT_HEIGHT / 2); ++y) {
		for (x = 0; x < FONT_WIDTH; ++x) {
			if (data & 1)
				*pixels = paint;
			data >>= 1;
			pixels++;
		}
		pixels += stride;
	}

	data = glyph[1];
	for (y = 0; y < (FONT_HEIGHT / 2); y++) {
		for (x = 0; x < FONT_WIDTH; x++) {
			if (data & 1)
				*pixels = paint;
			data >>= 1;
			pixels++;
		}
		pixels += stride;
	}
}

static void fbcon_flush(void)
{
	if (config->update_start)
		config->update_start();
	if (config->update_done)
		while (!config->update_done());
}

/* TODO: Take stride into account */
static void fbcon_scroll_up(void)
{
	unsigned short *dst = config->base;
	unsigned short *src = dst + (config->width * FONT_HEIGHT);
	unsigned count = config->width * (config->height - FONT_HEIGHT);

	while(count--) {
		*dst++ = *src++;
	}

	count = config->width * FONT_HEIGHT;
	while(count--) {
		*dst++ = BGCOLOR;
	}

	fbcon_flush();
}

/* TODO: take stride into account */
void fbcon_clear(void)
{
	unsigned count = config->width * config->height;
	memset(config->base, BGCOLOR, count * ((config->bpp) / 8));
}


static void fbcon_set_colors(unsigned bg, unsigned fg)
{
	BGCOLOR = bg;
	FGCOLOR = fg;
}

void fbcon_putc(char c)
{
	uint16_t *pixels;

	/* ignore anything that happens before fbcon is initialized */
	if (!config)
		return;

	if((unsigned char)c > 127)
		return;
	if((unsigned char)c < 32) {
		if(c == '\n')
			goto newline;
		else if (c == '\r')
			cur_pos.x = 0;
		return;
	}

	pixels = config->base;
	pixels += cur_pos.y * FONT_HEIGHT * config->width;
	pixels += cur_pos.x * (FONT_WIDTH + 1);
	fbcon_drawglyph(pixels, FGCOLOR, config->stride,
			font5x12 + (c - 32) * 2);

	cur_pos.x++;
	if (cur_pos.x < max_pos.x)
		return;

newline:
	cur_pos.y++;
	cur_pos.x = 0;
	if(cur_pos.y >= max_pos.y) {
		cur_pos.y = max_pos.y - 1;
		fbcon_scroll_up();
	} else
		fbcon_flush();
}

void fbcon_setup(struct fbcon_config *_config)
{
	uint32_t bg;
	uint32_t fg;

	ASSERT(_config);

	config = _config;

	switch (config->format) {
	case FB_FORMAT_RGB565:
		fg = RGB565_WHITE;
		bg = RGB565_BLACK;
		break;
        case FB_FORMAT_RGB888:
                fg = RGB888_WHITE;
                bg = RGB888_BLACK;
                break;
	default:
		dprintf(CRITICAL, "unknown framebuffer pixel format\n");
		ASSERT(0);
		break;
	}

	fbcon_set_colors(bg, fg);

	cur_pos.x = 0;
	cur_pos.y = 0;
	max_pos.x = config->width / (FONT_WIDTH+1);
	max_pos.y = (config->height - 1) / FONT_HEIGHT;
#if !DISPLAY_SPLASH_SCREEN
	fbcon_clear();
#endif
}

struct fbcon_config* fbcon_display(void)
{
    return config;
}

void display_image_on_screen(void)
{
    unsigned i = 0;
    unsigned total_x;
    unsigned total_y;
    unsigned bytes_per_bpp;
    unsigned image_base;

    if (!config) {
       dprintf(CRITICAL,"NULL configuration, image cannot be displayed\n");
       return;
    }

    fbcon_clear();

    total_x = config->width;
    total_y = config->height;
    bytes_per_bpp = ((config->bpp) / 8);
    image_base = ((((total_y/2) - (SPLASH_IMAGE_WIDTH / 2) - 1) *
		    (config->width)) + (total_x/2 - (SPLASH_IMAGE_HEIGHT / 2)));

#if DISPLAY_TYPE_MIPI
    if (bytes_per_bpp == 3)
    {
        for (i = 0; i < SPLASH_IMAGE_WIDTH; i++)
        {
            memcpy (config->base + ((image_base + (i * (config->width))) * bytes_per_bpp),
		    imageBuffer_rgb888 + (i * SPLASH_IMAGE_HEIGHT * bytes_per_bpp),
		    SPLASH_IMAGE_HEIGHT * bytes_per_bpp);
	}
    }
    fbcon_flush();
#if DISPLAY_MIPI_PANEL_NOVATEK_BLUE
    if(is_cmd_mode_enabled())
        mipi_dsi_cmd_mode_trigger();
#endif

#else
    if (bytes_per_bpp == 2)
    {
        for (i = 0; i < SPLASH_IMAGE_WIDTH; i++)
        {
            memcpy (config->base + ((image_base + (i * (config->width))) * bytes_per_bpp),
		    imageBuffer + (i * SPLASH_IMAGE_HEIGHT * bytes_per_bpp),
		    SPLASH_IMAGE_HEIGHT * bytes_per_bpp);
	}
    }
    fbcon_flush();
#endif
}
