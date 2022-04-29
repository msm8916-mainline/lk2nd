/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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

#ifndef _PANEL_GC9305_QVGA_SPI_CMD_H_
#define _PANEL_GC9305_QVGA_SPI_CMD_H_
/*---------------------------------------------------------------------------*/
/* HEADER files                                                              */
/*---------------------------------------------------------------------------*/
#include "panel.h"

/*---------------------------------------------------------------------------*/
/* Panel configuration                                                       */
/*---------------------------------------------------------------------------*/
static struct panel_config gc9305_qvga_cmd_panel_data = {
	"qcom,mdss_spi_gc9305_qvga_cmd", "spi:0:", "qcom,mdss-spi-panel",
	10, 0, "DISPLAY_1", 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, NULL
};

/*---------------------------------------------------------------------------*/
/* Panel resolution                                                          */
/*---------------------------------------------------------------------------*/
static struct panel_resolution gc9305_qvga_cmd_panel_res = {
	240, 320, 79, 59, 60, 0, 10, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel color information                                                   */
/*---------------------------------------------------------------------------*/
static struct color_info gc9305_qvga_cmd_color = {
	16, 0, 0xff, 0, 0, 0
};

/*---------------------------------------------------------------------------*/
/* Panel on/off command information                                          */
/*---------------------------------------------------------------------------*/
static char gc9305_qvga_cmd_on_cmd0[] = {
	0xFE,
};

static char gc9305_qvga_cmd_on_cmd1[] = {
	0xEF,
};

static char gc9305_qvga_cmd_on_cmd2[] = {
	0x36, 0x48,
};

static char gc9305_qvga_cmd_on_cmd3[] = {
	0x3A, 0x05,
};

static char gc9305_qvga_cmd_on_cmd4[] = {
	0x35, 0x00,
};

static char gc9305_qvga_cmd_on_cmd5[] = {
	0x44, 0x00, 0x14,
};

static char gc9305_qvga_cmd_on_cmd6[] = {
	0xA4, 0x44, 0x44,
};

static char gc9305_qvga_cmd_on_cmd7[] = {
	0xA5, 0x42, 0x42,
};

static char gc9305_qvga_cmd_on_cmd8[] = {
	0xAA, 0x88, 0x88,
};

static char gc9305_qvga_cmd_on_cmd9[] = {
	0xE8, 0x12, 0x40,
};

static char gc9305_qvga_cmd_on_cmd10[] = {
	0xE3, 0x01, 0x10,
};

static char gc9305_qvga_cmd_on_cmd11[] = {
	0xFF, 0x61,
};

static char gc9305_qvga_cmd_on_cmd12[] = {
	0xAC, 0x00,
};

static char gc9305_qvga_cmd_on_cmd13[] = {
	0xA6, 0x2A, 0x2A,
};

static char gc9305_qvga_cmd_on_cmd14[] = {
	0xA7, 0x2B, 0x2B,
};

static char gc9305_qvga_cmd_on_cmd15[] = {
	0xA8, 0x18, 0x18,
};

static char gc9305_qvga_cmd_on_cmd16[] = {
	0xA9, 0x2A, 0x2A,
};

static char gc9305_qvga_cmd_on_cmd17[] = {
	0xAD, 0x33
};

static char gc9305_qvga_cmd_on_cmd18[] = {
	0xAF, 0x55
};

static char gc9305_qvga_cmd_on_cmd19[] = {
	0xAE, 0x2B
};

static char gc9305_qvga_cmd_on_cmd20[] = {
	0x2A, 0x00, 0x00, 0x00,
	0xEF
};

static char gc9305_qvga_cmd_on_cmd21[] = {
	0x2B, 0x00, 0x00, 0x01,
	0x3F
};

static char gc9305_qvga_cmd_on_cmd22[] = {
	0x2c,
};

static char gc9305_qvga_cmd_on_cmd23[] = {
	0xF0, 0x02, 0x02, 0x00,
	0x08, 0x0C, 0x10,
};

static char gc9305_qvga_cmd_on_cmd24[] = {
	0xF1, 0x01, 0x00, 0x00,
	0x14, 0x1D, 0x0E,
};

static char gc9305_qvga_cmd_on_cmd25[] = {
	0xF2, 0x10, 0x09, 0x37,
	0x04, 0x04, 0x48,
};

static char gc9305_qvga_cmd_on_cmd26[] = {
	0xF3, 0x10, 0x0B, 0x3F,
	0x05, 0x05, 0x4E,
};

static char gc9305_qvga_cmd_on_cmd27[] = {
	0xF4, 0x0D, 0x19, 0x17,
	0x1D, 0x1E, 0x0F,

};

static char gc9305_qvga_cmd_on_cmd28[] = {
	0xF5, 0x06, 0x12, 0x13,
	0x1A, 0x1B, 0x0F,
};

static char gc9305_qvga_cmd_on_cmd29[] = {
	0x11,
};

static char gc9305_qvga_cmd_on_cmd30[] = {
	0x29,
};

static char gc9305_qvga_cmd_on_cmd31[] = {
	0x2c,
};

static char gc9305_qvga_cmd_on_cmd32[] = {
	0x2c,
};

static struct mdss_spi_cmd gc9305_qvga_cmd_on_command[] = {
	{0x01, gc9305_qvga_cmd_on_cmd0, 0x00, 0},
	{0x01, gc9305_qvga_cmd_on_cmd1, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd2, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd3, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd4, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd5, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd6, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd7, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd8, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd9, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd10, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd11, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd12, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd13, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd14, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd15, 0x00, 0},
	{0x03, gc9305_qvga_cmd_on_cmd16, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd17, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd18, 0x00, 0},
	{0x02, gc9305_qvga_cmd_on_cmd19, 0x00, 0},
	{0x05, gc9305_qvga_cmd_on_cmd20, 0x00, 0},
	{0x05, gc9305_qvga_cmd_on_cmd21, 0x00, 0},
	{0x01, gc9305_qvga_cmd_on_cmd22, 0x00, 0},
	{0x07, gc9305_qvga_cmd_on_cmd23, 0x00, 0},
	{0x07, gc9305_qvga_cmd_on_cmd24, 0x00, 0},
	{0x07, gc9305_qvga_cmd_on_cmd25, 0x00, 0},
	{0x07, gc9305_qvga_cmd_on_cmd26, 0x00, 0},
	{0x07, gc9305_qvga_cmd_on_cmd27, 0x00, 0},
	{0x07, gc9305_qvga_cmd_on_cmd28, 0x00, 0},
	{0x01, gc9305_qvga_cmd_on_cmd29, 0x78, 1},
	{0x01, gc9305_qvga_cmd_on_cmd30, 0x32, 1},
	{0x01, gc9305_qvga_cmd_on_cmd31, 0x00, 0},
	{0x01, gc9305_qvga_cmd_on_cmd32, 0x00, 1}
};

#define GC9305_QVGA_CMD_ON_COMMAND 33


static char gc9305_qvga_cmdoff_cmd0[] = {
	0x28,
};

static char gc9305_qvga_cmdoff_cmd1[] = {
	0x10,
};

static struct mipi_dsi_cmd gc9305_qvga_cmd_off_command[] __UNUSED = {
	{0x1, gc9305_qvga_cmdoff_cmd0, 0x20},
	{0x1, gc9305_qvga_cmdoff_cmd1, 0x20}
};

#define GC9305_QVGA_CMD_OFF_COMMAND 2

/*---------------------------------------------------------------------------*/
/* Panel reset sequence                                                      */
/*---------------------------------------------------------------------------*/
static struct panel_reset_sequence gc9305_qvga_cmd_reset_seq = {
	{1, 0, 1, }, {20, 2, 20, }, 2
};

/*---------------------------------------------------------------------------*/
/* Backlight setting                                                         */
/*---------------------------------------------------------------------------*/
static struct backlight gc9305_qvga_cmd_backlight = {
	1, 1, 4095, 100, 1, "PMIC_8941"
};

static uint8_t gc9305_signature_addr = 0x04;

static uint8_t gc9305_signature_len = 3;

static uint8_t gc9305_signature[] = {
	0x00, 0x93, 0x05
};
#endif /* PANEL_GC9305_QVGA_SPI_CMD_H */
