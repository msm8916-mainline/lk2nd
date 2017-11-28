/* Copyright (c) 2017-2018, The Linux Foundation. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*    * Redistributions of source code must retain the above copyright
*      notice, this list of conditions and the following disclaimer.
*    * Redistributions in binary form must reproduce the above
*      copyright notice, this list of conditions and the following
*      disclaimer in the documentation and/or other materials provided
*      with the distribution.
*    * Neither the name of The Linux Foundation nor the names of its
*      contributors may be used to endorse or promote products derived
*      from this software without specific prior written permission.
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

#ifndef _PANEL_ST7789v2_QVGA_SPI_CMD_H_
#define _PANEL_ST7789v2_QVGA_SPI_CMD_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config st7789v2_qvga_cmd_panel_data = {
	"qcom,mdss_spi_st7789v2_qvga_cmd", "spi:0:", "qcom,mdss-spi-panel",
	10, 0, "DISPLAY_1", 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution st7789v2_qvga_cmd_panel_res = {
	240, 240, 79, 59, 60, 0, 7, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info st7789v2_qvga_cmd_color = {
	16, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char st7789v2_qvga_cmd_on_cmd0[] = {
	0x11,
};

static char st7789v2_qvga_cmd_on_cmd1[] = {
	0x36, 0x00,
};

static char st7789v2_qvga_cmd_on_cmd2[] = {
	0x3A, 0x05,
};

static char st7789v2_qvga_cmd_on_cmd3[] = {
	0x35, 0x00,
};

static char st7789v2_qvga_cmd_on_cmd4[] = {
	0xB2, 0x0C, 0x0C, 0x00,
	0x33, 0x33,
};

static char st7789v2_qvga_cmd_on_cmd5[] = {
	0xB7, 0x75,
};

static char st7789v2_qvga_cmd_on_cmd6[] = {
	0xBB, 0x3D,
};

static char st7789v2_qvga_cmd_on_cmd7[] = {
	0xC2, 0x01,
};

static char st7789v2_qvga_cmd_on_cmd8[] = {
	0xC3, 0x19,
};

static char st7789v2_qvga_cmd_on_cmd9[] = {
	0x04, 0x20,
};

static char st7789v2_qvga_cmd_on_cmd10[] = {
	0xc6, 0x0F,
};

static char st7789v2_qvga_cmd_on_cmd11[] = {
	0xD0, 0xA4, 0xA1,
};

static char st7789v2_qvga_cmd_on_cmd12[] = {
	0xE0, 0x70, 0x04, 0x08,
	0x09, 0x09, 0x05, 0x2A,
	0x33, 0x41, 0x07, 0x13,
	0x13, 0x29, 0x2F,
};

static char st7789v2_qvga_cmd_on_cmd13[] = {
	0xE1, 0x70, 0x03, 0x09,
	0x0A, 0x09, 0x06, 0x2B,
	0x34, 0x41, 0x07, 0x12,
	0x14, 0x28, 0x2E
};

static char st7789v2_qvga_cmd_on_cmd14[] = {
	0x21,
};

static char st7789v2_qvga_cmd_on_cmd15[] = {
	0x29,
};

static char st7789v2_qvga_cmd_on_cmd16[] = {
	0x2A, 0x00, 0x00, 0x00, 0xEF
};

static char st7789v2_qvga_cmd_on_cmd17[] = {
	0x2B, 0x00, 0x00, 0x00, 0xEF
};

static char st7789v2_qvga_cmd_on_cmd18[] = {
	0x2c
};

static struct mdss_spi_cmd st7789v2_qvga_cmd_on_command[] = {
	{0x01, st7789v2_qvga_cmd_on_cmd0, 0x78, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd1, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd2, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd3, 0x00, 0},
	{0x06, st7789v2_qvga_cmd_on_cmd4, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd5, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd6, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd7, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd8, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd9, 0x00, 0},
	{0x02, st7789v2_qvga_cmd_on_cmd10, 0x00, 0},
	{0x03, st7789v2_qvga_cmd_on_cmd11, 0x00, 0},
	{0x0F, st7789v2_qvga_cmd_on_cmd12, 0x00, 0},
	{0x0F, st7789v2_qvga_cmd_on_cmd13, 0x00, 0},
	{0x01, st7789v2_qvga_cmd_on_cmd14, 0x00, 0},
	{0x01, st7789v2_qvga_cmd_on_cmd15, 0x78, 0},
	{0x05, st7789v2_qvga_cmd_on_cmd16, 0x00, 0},
	{0x05, st7789v2_qvga_cmd_on_cmd17, 0x00, 0},
	{0x01, st7789v2_qvga_cmd_on_cmd18, 0x00, 0},
};

#define ST7789v2_QVGA_CMD_ON_COMMAND 19


static char st7789v2_qvga_cmdoff_cmd0[] = {
	0x28,
};

static char st7789v2_qvga_cmdoff_cmd1[] = {
	0x10,
};

static struct mipi_dsi_cmd st7789v2_qvga_cmd_off_command[] = {
	{0x1, st7789v2_qvga_cmdoff_cmd0, 0x20},
	{0x1, st7789v2_qvga_cmdoff_cmd1, 0x20}
};

#define ST7789v2_QVGA_CMD_OFF_COMMAND 2

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence st7789v2_qvga_cmd_reset_seq = {
	{1, 0, 1, }, {20, 2, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight st7789v2_qvga_cmd_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

static uint8_t st7789v2_signature_addr = 0x04;

static uint8_t st7789v2_signature_len = 3;

static uint8_t st7789v2_signature[] = {
	0x85, 0x85, 0x52
};

#endif /* PANEL_ST7789v2_QVGA_SPI_CMD_H */
