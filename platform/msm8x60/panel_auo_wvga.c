/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
 *  * Neither the name of Code Aurora nor
 *    the names of its contributors may be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
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
#include <dev/gpio.h>
#include <kernel/thread.h>
#include <platform/gpio.h>
#include <dev/lcdc.h>

#define GPIO_SET_OUT_VAL(gpio,val) (gpio_set(gpio,(val)<<1))

#define LCDC_SPI_GPIO_CLK		73
#define LCDC_SPI_GPIO_CS		72
#define LCDC_SPI_GPIO_MOSI		70

/*  panel cmd addresses */
#define PANEL_CMD_SLEEP_OUT		0x1100
#define PANEL_CMD_RGBCTRL		0x3B00
#define PANEL_CMD_DISP_ON		0x2900
#define PANEL_CMD_BCTRL			0x5300
#define PANEL_CMD_PWM_EN		0x6A17
#define PANEL_CMD_FORMAT		0x3A00
#define PANEL_CMD_CABC_FORCE2		0x6A18	/* backlight level */

/* timing info */
static struct lcdc_timing_parameters param = {
	.lcdc_fb_width = 480,
	.lcdc_fb_height = 800,
	.lcdc_hsync_pulse_width_dclk = 2,
	.lcdc_hsync_back_porch_dclk = 14,
	.lcdc_hsync_front_porch_dclk = 16,
	.lcdc_hsync_skew_dclk = 0,

	.lcdc_vsync_pulse_width_lines = 2,
	.lcdc_vsync_back_porch_lines = 1,
	.lcdc_vsync_front_porch_lines = 28,
};

struct lcdc_timing_parameters *auo_timing_param()
{
	return &param;
}

/* spi commands */
static void auo_spi_write_byte(uint32_t data)
{
	uint32_t bit;
	uint32_t bnum;

	bnum = 8;
	bit = 0x80;
	while (bnum--) {
		GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_CLK, 0);
		GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_MOSI, (data & bit) ? 1 : 0);
		udelay(1);
		GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_CLK, 1);
		udelay(1);
		bit >>= 1;
	}
	GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_MOSI, 0);
}

static uint32_t auo_serigo(uint32_t * input_data, uint32_t input_len)
{
	uint32_t i;

	GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_CS, 0);
	udelay(2);

	for (i = 0; i < input_len; ++i) {
		auo_spi_write_byte(input_data[i]);
		udelay(2);
	}

	GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_CS, 1);

	return 0;
}

static void auo_write_cmd(uint32_t cmd)
{
	uint32_t local_data[4];

	local_data[0] = 0x20;
	local_data[1] = (uint32_t) (cmd >> 8);
	local_data[2] = 0;
	local_data[3] = (uint32_t) cmd;
	auo_serigo(local_data, 4);
}

static void auo_write_cmd_param(uint32_t cmd, uint32_t param)
{
	uint32_t local_data[6];

	local_data[0] = 0x20;
	local_data[1] = (uint32_t) (cmd >> 8);
	local_data[2] = 0;
	local_data[3] = (uint32_t) cmd;
	local_data[4] = 0x40;
	local_data[5] = param;
	auo_serigo(local_data, 6);
}

static void auo_spi_init(void)
{
	gpio_tlmm_config(LCDC_SPI_GPIO_CLK, 0, GPIO_OUTPUT, GPIO_NO_PULL,
			 GPIO_2MA, 1);
	gpio_tlmm_config(LCDC_SPI_GPIO_CS, 0, GPIO_OUTPUT, GPIO_NO_PULL,
			 GPIO_2MA, 1);
	gpio_tlmm_config(LCDC_SPI_GPIO_MOSI, 0, GPIO_OUTPUT, GPIO_NO_PULL,
			 GPIO_2MA, 1);

	/* Set the output so that we don't disturb the slave device */
	GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_CLK, 1);
	GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_MOSI, 0);

	/* Set the Chip Select deasserted (active low)  */
	GPIO_SET_OUT_VAL(LCDC_SPI_GPIO_CS, 1);
}

/* panel initialization */
static void auo_panel_init(void)
{
	auo_write_cmd(PANEL_CMD_SLEEP_OUT);
	mdelay(150);

	/* SET_PIXEL_FORMAT: Set how many bits per pixel are used (3A00h), 18bits */
	auo_write_cmd_param(PANEL_CMD_FORMAT, 0x66);
	mdelay(1);

	/* RGBCTRL: RGB Interface Signal Control (3B00h) */
	auo_write_cmd_param(PANEL_CMD_RGBCTRL, 0x2B);
	mdelay(1);

	/* Turn on display */
	auo_write_cmd(PANEL_CMD_DISP_ON);
	mdelay(1);

	/* Turn on backlight */
	auo_write_cmd_param(PANEL_CMD_BCTRL, 0x24);
	mdelay(1);

	/* Enable PWM level */
	auo_write_cmd_param(PANEL_CMD_PWM_EN, 0x01);
	mdelay(1);

	/* Set backlight level to mid range level */
	auo_write_cmd_param(PANEL_CMD_CABC_FORCE2, 0x77);
	mdelay(10);
}

void auo_lcdc_init(void)
{
	auo_spi_init();
	auo_panel_init();
}
