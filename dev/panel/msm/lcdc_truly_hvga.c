/*
 * Copyright (c) 2012, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor
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
#include <msm_panel.h>
#include <platform/gpio.h>
#include <debug.h>
#include <err.h>
#include "lcdc_truly.h"

extern void spi_bit_bang_write(uint8_t, uint8_t, uint8_t, uint8_t);

static char data1[] = { 0xff, 0x83, 0x57, };
static char data2[] = { 0x03, };
static char data3[] = { 0x00, 0x13, 0x1C, 0x1C, 0x83, 0x48, };
static char data4[] = { 0x43, 0x06, 0x06, 0x06, };
static char data5[] = { 0x53, };
static char data6[] = { 0x02, 0x40, 0x00, 0x2a, 0x2a, 0x0d, 0x3f, };
static char data7[] = { 0x70, 0x50, 0x01, 0x3c, 0xe8, 0x08, };
static char data8[] = { 0x17, 0x0f, };
static char data9[] = { 0x60};
static char data10[] = { 0x00, 0x13, 0x1a, 0x29, 0x2d, 0x41, 0x49,
				0x52, 0x48, 0x41, 0x3c, 0x33, 0x30, 0x1c,
				0x19, 0x03, 0x00, 0x13, 0x1a, 0x29, 0x2d,
				0x41, 0x49, 0x52, 0x48, 0x41, 0x3c, 0x33,
				0x31, 0x1c, 0x19, 0x03, 0x00, 0x01,
};
static char data11[] = { 0x40, };

static void lcdc_truly_panel_power(int on)
{
	uint32_t func = 0;
	uint32_t pull = 0;
	uint32_t drv = 0;
	uint32_t dir = 0;

	dprintf(SPEW, "lcdc_truly_panel_power on = %d\n", on);

	if (on) {
		func = 0;
		dir = GPIO_OUTPUT;
		pull = GPIO_NO_PULL;
		drv = GPIO_2MA;

		/* config SPI, RESET and Backlight */
		gpio_tlmm_config(GPIO_CFG(SPI_MOSI, func, dir, pull, drv), 0);
		gpio_tlmm_config(GPIO_CFG(SPI_SCLK, func, dir, pull, drv), 0);
		gpio_tlmm_config(GPIO_CFG(SPI_CS, func, dir, pull, drv), 0);
		gpio_tlmm_config(GPIO_CFG(BACKLIGHT_EN, func, dir, pull, drv), 0);
		gpio_tlmm_config(GPIO_CFG(DISPLAY_RESET, func, dir, pull, drv), 0);
		gpio_tlmm_config(GPIO_CFG(LCD_LDO_2V8, func, dir, pull, drv), 0);
		gpio_tlmm_config(GPIO_CFG(LCD_LDO_1V8, func, dir, pull, drv), 0);

		gpio_config(SPI_MOSI, GPIO_OUTPUT);
		gpio_config(SPI_SCLK, GPIO_OUTPUT);
		gpio_config(SPI_CS, GPIO_OUTPUT);
		gpio_config(BACKLIGHT_EN, GPIO_OUTPUT);
		gpio_config(DISPLAY_RESET, GPIO_OUTPUT);
		gpio_config(LCD_LDO_2V8, GPIO_OUTPUT);
		gpio_config(LCD_LDO_1V8, GPIO_OUTPUT);

		/* turn on backlight */
		gpio_set(BACKLIGHT_EN, 1);
		/* turn on power */
		gpio_set(LCD_LDO_1V8, 1);
		gpio_set(LCD_LDO_2V8, 1);

		/* reset */
		gpio_set(DISPLAY_RESET, 1);
		/*
		 * H/w requires delay between toggle
		 * reset pin
		 */
		mdelay(20);
		gpio_set(DISPLAY_RESET, 0);
		mdelay(20);
		gpio_set(DISPLAY_RESET, 1);
		mdelay(20);
	} else {
		/* turn off backlight */
		gpio_set(BACKLIGHT_EN, 0);

		gpio_set(DISPLAY_RESET, 0);

		/* turn off power */
		gpio_set(LCD_LDO_1V8, 0);
		gpio_set(LCD_LDO_2V8, 0);
	}
	return;
}

static int truly_spi_write(char cmd, char *data, int num)
{
	int i;

	gpio_set(SPI_CS, 0); /* cs low */
	/* command byte first */
	spi_bit_bang_write(SPI_SCLK, SPI_MOSI, 0, cmd);
	/* followed by parameter bytes */
	for(i = 0; i < num; i++) {
		if(data)
			spi_bit_bang_write(SPI_SCLK, SPI_MOSI, 1, data[i]);
	}
	gpio_set(SPI_MOSI, 1); /* mosi high */
	gpio_set(SPI_CS, 1); /* cs high */
	/*
	 * H/W needs delay for gpio's to turn high
	 */
	udelay(10);
	return 0;
}

void lcdc_truly_disp_on(void)
{
	int i;

	dprintf(SPEW, "lcdc_truly_disp_on\n");

	gpio_set(SPI_CS, 1); /* cs high */
	/*
	 * This function uses delays as defined by the panel
	 * vendor before writing to registers
	 */
	truly_spi_write(0xb9, data1, sizeof(data1));
	mdelay(20);
	truly_spi_write(0xcc, data2, sizeof(data2));
	truly_spi_write(0xb1, data3, sizeof(data3));
	truly_spi_write(0xb3, data4, sizeof(data4));
	truly_spi_write(0xb6, data5, sizeof(data5));
	truly_spi_write(0xb4, data6, sizeof(data6));
	truly_spi_write(0xc0, data7, sizeof(data7));
	truly_spi_write(0xe3, data8, sizeof(data8));
	truly_spi_write(0x3a, data9, sizeof(data9));
	truly_spi_write(0xe0, data10, sizeof(data10));
	truly_spi_write(0x36, data11, sizeof(data11));
	truly_spi_write(0x11, NULL, 0);
	mdelay(150);
	truly_spi_write(0x29, NULL, 0);
	mdelay(25);
}

void lcdc_truly_disp_off(void)
{
	dprintf(SPEW, "lcdc_truly_disp_off\n");
	truly_spi_write(0x28, NULL, 0);
	mdelay(5);
	truly_spi_write(0x10, NULL, 0);
}

int lcdc_truly_panel_on(int on)
{
	dprintf(SPEW, "lcdc_truly_panel_on\n");
	if (on) {
		lcdc_truly_panel_power(1);
		lcdc_truly_disp_on();
	} else {
		lcdc_truly_panel_power(0);
		lcdc_truly_disp_off();
	}
	return NO_ERROR;
}

int lcdc_truly_hvga_on()
{
	return NO_ERROR;
}

int lcdc_truly_hvga_off()
{
	return NO_ERROR;
}

void lcdc_truly_hvga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = 320;
	pinfo->yres = 480;

	pinfo->type = LCDC_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 16;
	pinfo->clk_rate = 10240000;
	pinfo->lcdc.h_pulse_width = 8;
	pinfo->lcdc.h_back_porch = 16;
	pinfo->lcdc.h_front_porch = 4;
	pinfo->lcdc.hsync_skew = 0;
	pinfo->lcdc.v_pulse_width = 1;
	pinfo->lcdc.v_back_porch = 7;
	pinfo->lcdc.v_front_porch = 3;

	pinfo->on = lcdc_truly_hvga_on;
	pinfo->off = lcdc_truly_hvga_off;

	return;
}
