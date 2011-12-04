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
#include "gpio_hw.h"
#include "panel.h"
#include <dev/lcdc.h>

#define VEE_RESET              20
#define LCD_RESET              180

#define GPIO26_GPIO_CNTRL 0x169	/* backlight */

struct sharp_spi_data {
	unsigned addr;
	unsigned data;
};

static struct sharp_spi_data init_sequence[] = {
	{15, 0x01},
	{5, 0x01},
	{7, 0x10},
	{9, 0x1E},
	{10, 0x04},
	{17, 0xFF},
	{21, 0x8A},
	{22, 0x00},
	{23, 0x82},
	{24, 0x24},
	{25, 0x22},
	{26, 0x6D},
	{27, 0xEB},
	{28, 0xB9},
	{29, 0x3A},
	{49, 0x1A},
	{50, 0x16},
	{51, 0x05},
	{55, 0x7F},
	{56, 0x15},
	{57, 0x7B},
	{60, 0x05},
	{61, 0x0C},
	{62, 0x80},
	{63, 0x00},
	{92, 0x90},
	{97, 0x01},
	{98, 0xFF},
	{113, 0x11},
	{114, 0x02},
	{115, 0x08},
	{123, 0xAB},
	{124, 0x04},
	{6, 0x02},
	{133, 0x00},
	{134, 0xFE},
	{135, 0x22},
	{136, 0x0B},
	{137, 0xFF},
	{138, 0x0F},
	{139, 0x00},
	{140, 0xFE},
	{141, 0x22},
	{142, 0x0B},
	{143, 0xFF},
	{144, 0x0F},
	{145, 0x00},
	{146, 0xFE},
	{147, 0x22},
	{148, 0x0B},
	{149, 0xFF},
	{150, 0x0F},
	{202, 0x30},
	{30, 0x01},
	{4, 0x01},
	{31, 0x41}
};

static unsigned char bit_shift[8] = { (1 << 7),	/* MSB */
	(1 << 6),
	(1 << 5),
	(1 << 4),
	(1 << 3),
	(1 << 2),
	(1 << 1),
	(1 << 0)		/* LSB */
};

static unsigned vee_reset_gpio =
GPIO_CFG(VEE_RESET, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA);

static unsigned lcd_reset_gpio =
GPIO_CFG(LCD_RESET, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA);

static int sharp_display_common_power(int on)
{
	int rc = 0, flag_on = !!on;
	static int display_common_power_save_on;
	unsigned int vreg_ldo12, vreg_ldo15, vreg_ldo20, vreg_ldo16, vreg_ldo8;
	if (display_common_power_save_on == flag_on)
		return 0;

	display_common_power_save_on = flag_on;

	if (on) {
		/* set LCD reset */
		rc = gpio_tlmm_config(lcd_reset_gpio, GPIO_ENABLE);
		if (rc) {
			return rc;
		}

		gpio_set(LCD_RESET, 0);	/* bring reset line low to hold reset */

		/* set VEE reset */
		rc = gpio_tlmm_config(vee_reset_gpio, GPIO_ENABLE);

		if (rc) {
			return rc;
		}

		gpio_set(VEE_RESET, 1);
		gpio_set(VEE_RESET, 0);	/* bring reset line low to hold reset */
		mdelay(10);
	}

	/* Set LD008 to 1.8V - VEE (VCC, VDDIO, pullups) */
	pmic_write(LDO08_CNTRL, 0x06 | LDO_LOCAL_EN_BMSK);

	/* Set LD012 to 1.8V - display (VDDIO) */
	pmic_write(LDO12_CNTRL, 0x06 | LDO_LOCAL_EN_BMSK);

	/* Set LD015 to 3.0V - display (VCC), VEE (VLP) */
	pmic_write(LDO15_CNTRL, 0x1E | LDO_LOCAL_EN_BMSK);

	/* wait for power to stabilize */
	mdelay(10);

	gpio_config(VEE_RESET, 0);	/*disable VEE_RESET, rely on pullups to bring it high */
	mdelay(5);

	gpio_set(LCD_RESET, 1);	/* bring reset line high */
	mdelay(10);		/* 10 msec before IO can be accessed */

	return rc;
}

static struct msm_gpio sharp_lcd_panel_gpios[] = {
	{GPIO_CFG(45, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "spi_clk"},
	{GPIO_CFG(46, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "spi_cs0"},
	{GPIO_CFG(47, 0, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "spi_mosi"},
	{GPIO_CFG(48, 0, GPIO_INPUT, GPIO_NO_PULL, GPIO_2MA), "spi_miso"},
	{GPIO_CFG(22, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_blu2"},
	{GPIO_CFG(25, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_red2"},
	{GPIO_CFG(90, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_pclk"},
	{GPIO_CFG(91, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_en"},
	{GPIO_CFG(92, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_vsync"},
	{GPIO_CFG(93, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_hsync"},
	{GPIO_CFG(94, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_grn2"},
	{GPIO_CFG(95, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_grn3"},
	{GPIO_CFG(96, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_grn4"},
	{GPIO_CFG(97, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_grn5"},
	{GPIO_CFG(98, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_grn6"},
	{GPIO_CFG(99, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_grn7"},
	{GPIO_CFG(100, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_blu3"},
	{GPIO_CFG(101, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_blu4"},
	{GPIO_CFG(102, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_blu5"},
	{GPIO_CFG(103, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_blu6"},
	{GPIO_CFG(104, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_blu7"},
	{GPIO_CFG(105, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_red3"},
	{GPIO_CFG(106, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_red4"},
	{GPIO_CFG(107, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_red5"},
	{GPIO_CFG(108, 1, GPIO_OUTPUT, GPIO_NO_PULL, GPIO_2MA), "lcdc_red6"},
};

int sharp_lcdc_panel_power(int on)
{
	int rc, i;
	struct msm_gpio *gp;

	rc = sharp_display_common_power(on);
	if (rc < 0) {
		return rc;
	}

	if (on) {
		rc = platform_gpios_enable(sharp_lcd_panel_gpios,
					   ARRAY_SIZE(sharp_lcd_panel_gpios));
		if (rc) {
			return rc;
		}
	} else {		/* off */
		gp = sharp_lcd_panel_gpios;
		for (i = 0; i < ARRAY_SIZE(sharp_lcd_panel_gpios); i++) {
			/* ouput low */
			gpio_set(GPIO_PIN(gp->gpio_cfg), 0);
			gp++;
		}
	}
	return rc;
}

static void sharp_spi_write_byte(unsigned val)
{
	int i;

	/* Clock should be Low before entering */
	for (i = 0; i < 8; i++) {
		/* #1: Drive the Data (High or Low) */
		if (val & bit_shift[i])
			gpio_set(SPI_MOSI, 1);
		else
			gpio_set(SPI_MOSI, 0);

		/* #2: Drive the Clk High and then Low */
		gpio_set(SPI_SCLK, 1);
		gpio_set(SPI_SCLK, 0);
	}
}

static int serigo(unsigned reg, unsigned data)
{
	/* Enable the Chip Select - low */
	gpio_set(SPI_CS, 0);
	udelay(1);

	/* Transmit register address first, then data */
	sharp_spi_write_byte(reg);

	/* Idle state of MOSI is Low */
	gpio_set(SPI_MOSI, 0);
	udelay(1);
	sharp_spi_write_byte(data);

	gpio_set(SPI_MOSI, 0);
	gpio_set(SPI_CS, 1);
	return 0;
}

void sharp_lcdc_disp_on(void)
{
	unsigned i;
	gpio_set(SPI_CS, 1);
	gpio_set(SPI_SCLK, 1);
	gpio_set(SPI_MOSI, 0);
	gpio_set(SPI_MISO, 0);

	for (i = 0; i < ARRAY_SIZE(init_sequence); i++) {
		serigo(init_sequence[i].addr, init_sequence[i].data);
	}
	mdelay(10);
	serigo(31, 0xC1);
	mdelay(10);
	serigo(31, 0xD9);
	serigo(31, 0xDF);
}

void sharp_lcdc_on(void)
{
	lcdc_clock_init(27648000);
	sharp_lcdc_panel_power(1);

	/*enable backlight, open up gpio, use default for LPG */
	pmic_write(GPIO26_GPIO_CNTRL, 0x81);	/* Write, Bank0, VIN0=VPH, Mode selection enabled */
	pmic_write(GPIO26_GPIO_CNTRL, 0x99);	/* Write, Bank1, OutOn/InOff, CMOS, Invert Output (GPIO High) */
	pmic_write(GPIO26_GPIO_CNTRL, 0xAA);	/* Write, Bank2, GPIO no pull */
	pmic_write(GPIO26_GPIO_CNTRL, 0xB4);	/* Write, Bank3, high drv strength */
	pmic_write(GPIO26_GPIO_CNTRL, 0xC6);	/* Write, Bank4, Src: Special Function 2 */

	sharp_lcdc_disp_on();
}

static struct lcdc_timing_parameters param = {
	.lcdc_fb_width = 480,
	.lcdc_fb_height = 800,
	.lcdc_hsync_pulse_width_dclk = 10,
	.lcdc_hsync_back_porch_dclk = 20,
	.lcdc_hsync_front_porch_dclk = 10,
	.lcdc_hsync_skew_dclk = 0,

	.lcdc_vsync_pulse_width_lines = 2,
	.lcdc_vsync_back_porch_lines = 2,
	.lcdc_vsync_front_porch_lines = 2,
};

struct lcdc_timing_parameters *sharp_timing_param()
{
	return &param;
}
