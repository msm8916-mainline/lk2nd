/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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
 *
 */

#include <stdint.h>
#include <msm_panel.h>
#include <mipi_dsi.h>
#include <sys/types.h>
#include <err.h>
#include <reg.h>
#include <debug.h>
#include <platform/iomap.h>
#include <target/display.h>
#include <dev/gpio.h>


/* Renesas Tremelo-M panel: List of commands */

static char config_sleep_out[4] = { 0x11, 0x00, 0x05, 0x80 };
static char config_CMD_MODE[4] = { 0x40, 0x01, 0x15, 0x80 };
static char config_WRTXHT[12] = {
	0x07, 0x00, 0x39, 0xC0, 0x92, 0x16, 0x08, 0x08, 0x00, 0x01, 0xe0, 0xff };
static char config_WRTXVT[12] = {
	0x07, 0x00, 0x39, 0xC0, 0x8b, 0x02, 0x02, 0x02, 0x00, 0x03, 0x60, 0xff };
static char config_PLL2NR[4] = { 0xa0, 0x24, 0x15, 0x80 };
static char config_PLL2NF1[4] = { 0xa2, 0xd0, 0x15, 0x80 };
static char config_PLL2NF2[4] = { 0xa4, 0x00, 0x15, 0x80 };
static char config_PLL2BWADJ1[4] = { 0xa6, 0xd0, 0x15, 0x80 };
static char config_PLL2BWADJ2[4] = { 0xa8, 0x00, 0x15, 0x80 };
static char config_PLL2CTL[4] = { 0xaa, 0x00, 0x15, 0x80 };
static char config_DBICBR[4] = { 0x48, 0x03, 0x15, 0x80 };
static char config_DBICTYPE[4] = { 0x49, 0x00, 0x15, 0x80 };
static char config_DBICSET1[4] = { 0x4a, 0x1c, 0x15, 0x80 };
static char config_DBICADD[4] = { 0x4b, 0x00, 0x15, 0x80 };
static char config_DBICCTL[4] = { 0x4e, 0x01, 0x15, 0x80 };

/* static char config_COLMOD_565[4] = {0x3a, 0x05, 0x15, 0x80}; */
/* static char config_COLMOD_666PACK[4] = {0x3a, 0x06, 0x15, 0x80}; */
static char config_COLMOD_888[4] = { 0x3a, 0x07, 0x15, 0x80 };
static char config_MADCTL[4] = { 0x36, 0x00, 0x15, 0x80 };
static char config_DBIOC[4] = { 0x82, 0x40, 0x15, 0x80 };
static char config_CASET[12] = {
	0x07, 0x00, 0x39, 0xC0, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x01, 0xdf, 0xff };
static char config_PASET[12] = {
	0x07, 0x00, 0x39, 0xC0, 0x2b, 0x00, 0x00, 0x00, 0x00, 0x03, 0x5f, 0xff };
static char config_TXON[4] = { 0x81, 0x00, 0x05, 0x80 };
static char config_BLSET_TM[4] = { 0xff, 0x6c, 0x15, 0x80 };

static char config_AGCPSCTL_TM[4] = { 0x56, 0x08, 0x15, 0x80 };

static char config_DBICADD70[4] = { 0x4b, 0x70, 0x15, 0x80 };
static char config_DBICSET_15[4] = { 0x4a, 0x15, 0x15, 0x80 };
static char config_DBICADD72[4] = { 0x4b, 0x72, 0x15, 0x80 };

static char config_Power_Ctrl_2a_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x10, 0xff };
static char config_Auto_Sequencer_Setting_a_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char Driver_Output_Ctrl_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x01, 0xff };
static char Driver_Output_Ctrl_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x10, 0xff };
static char config_LCD_drive_AC_Ctrl_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x02, 0xff };
static char config_LCD_drive_AC_Ctrl_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x00, 0xff };
static char config_Entry_Mode_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x03, 0xff };
static char config_Entry_Mode_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_Display_Ctrl_1_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x07, 0xff };
static char config_Display_Ctrl_1_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_Display_Ctrl_2_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x08, 0xff };
static char config_Display_Ctrl_2_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x04, 0xff };
static char config_Display_Ctrl_3_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x09, 0xff };
static char config_Display_Ctrl_3_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x0c, 0xff };
static char config_Display_IF_Ctrl_1_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x0c, 0xff };
static char config_Display_IF_Ctrl_1_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x10, 0xff };
static char config_Display_IF_Ctrl_2_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x0e, 0xff };
static char config_Display_IF_Ctrl_2_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };

static char config_Panel_IF_Ctrl_1_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x20, 0xff };
static char config_Panel_IF_Ctrl_1_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x3f, 0xff };
static char config_Panel_IF_Ctrl_3_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x22, 0xff };
static char config_Panel_IF_Ctrl_3_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x76, 0x00, 0xff };
static char config_Panel_IF_Ctrl_4_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x23, 0xff };
static char config_Panel_IF_Ctrl_4_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x1c, 0x0a, 0xff };
static char config_Panel_IF_Ctrl_5_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x24, 0xff };
static char config_Panel_IF_Ctrl_5_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x1c, 0x2c, 0xff };
static char config_Panel_IF_Ctrl_6_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x25, 0xff };
static char config_Panel_IF_Ctrl_6_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x1c, 0x4e, 0xff };
static char config_Panel_IF_Ctrl_8_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x27, 0xff };
static char config_Panel_IF_Ctrl_8_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_Panel_IF_Ctrl_9_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x28, 0xff };
static char config_Panel_IF_Ctrl_9_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x76, 0x0c, 0xff };

static char config_gam_adjust_00_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x00, 0xff };
static char config_gam_adjust_00_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_01_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x01, 0xff };
static char config_gam_adjust_01_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x05, 0x02, 0xff };
static char config_gam_adjust_02_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x02, 0xff };
static char config_gam_adjust_02_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x05, 0xff };
static char config_gam_adjust_03_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x03, 0xff };
static char config_gam_adjust_03_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_04_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x04, 0xff };
static char config_gam_adjust_04_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x00, 0xff };
static char config_gam_adjust_05_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x05, 0xff };
static char config_gam_adjust_05_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_06_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x06, 0xff };
static char config_gam_adjust_06_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x10, 0x10, 0xff };
static char config_gam_adjust_07_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x07, 0xff };
static char config_gam_adjust_07_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x02, 0xff };
static char config_gam_adjust_08_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x08, 0xff };
static char config_gam_adjust_08_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x04, 0xff };
static char config_gam_adjust_09_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x09, 0xff };
static char config_gam_adjust_09_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_0A_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0a, 0xff };
static char config_gam_adjust_0A_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_0B_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0b, 0xff };
static char config_gam_adjust_0B_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x00, 0xff };
static char config_gam_adjust_0C_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0c, 0xff };
static char config_gam_adjust_0C_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_0D_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x0d, 0xff };
static char config_gam_adjust_0D_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x10, 0x10, 0xff };
static char config_gam_adjust_10_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x10, 0xff };
static char config_gam_adjust_10_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x04, 0xff };
static char config_gam_adjust_11_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x11, 0xff };
static char config_gam_adjust_11_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x05, 0x03, 0xff };
static char config_gam_adjust_12_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x12, 0xff };
static char config_gam_adjust_12_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x04, 0xff };
static char config_gam_adjust_15_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x15, 0xff };
static char config_gam_adjust_15_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x04, 0xff };
static char config_gam_adjust_16_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x16, 0xff };
static char config_gam_adjust_16_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x1c, 0xff };
static char config_gam_adjust_17_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x17, 0xff };
static char config_gam_adjust_17_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x04, 0xff };
static char config_gam_adjust_18_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x18, 0xff };
static char config_gam_adjust_18_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x04, 0x02, 0xff };
static char config_gam_adjust_19_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x19, 0xff };
static char config_gam_adjust_19_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x05, 0xff };
static char config_gam_adjust_1C_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x1c, 0xff };
static char config_gam_adjust_1C_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x07, 0x07, 0xff };
static char config_gam_adjust_1D_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x1D, 0xff };
static char config_gam_adjust_1D_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x1f, 0xff };
static char config_gam_adjust_20_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x20, 0xff };
static char config_gam_adjust_20_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x05, 0x07, 0xff };
static char config_gam_adjust_21_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x21, 0xff };
static char config_gam_adjust_21_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x06, 0x04, 0xff };
static char config_gam_adjust_22_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x22, 0xff };
static char config_gam_adjust_22_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x04, 0x05, 0xff };
static char config_gam_adjust_27_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x27, 0xff };
static char config_gam_adjust_27_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x02, 0x03, 0xff };
static char config_gam_adjust_28_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x28, 0xff };
static char config_gam_adjust_28_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x00, 0xff };
static char config_gam_adjust_29_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0x29, 0xff };
static char config_gam_adjust_29_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x02, 0xff };

static char config_Power_Ctrl_1_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x00, 0xff };
static char config_Power_Ctrl_1b_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x36, 0x3c, 0xff };
static char config_Power_Ctrl_2_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x01, 0xff };
static char config_Power_Ctrl_2b_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x03, 0xff };
static char config_Power_Ctrl_3_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x02, 0xff };
static char config_Power_Ctrl_3a_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x01, 0xff };
static char config_Power_Ctrl_4_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x03, 0xff };
static char config_Power_Ctrl_4a_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x3c, 0x58, 0xff };
static char config_Power_Ctrl_6_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x0c, 0xff };
static char config_Power_Ctrl_6a_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x35, 0xff };

static char config_Auto_Sequencer_Setting_b_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x02, 0xff };
static char config_Panel_IF_Ctrl_10_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x29, 0xff };
static char config_Panel_IF_Ctrl_10a_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x03, 0xbf, 0xff };
static char config_Auto_Sequencer_Setting_indx[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x01, 0x06, 0xff };
static char config_Auto_Sequencer_Setting_c_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x00, 0x03, 0xff };
static char config_Power_Ctrl_2c_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4c, 0x40, 0x10, 0xff };

static char config_VIDEO[4] = { 0x40, 0x00, 0x15, 0x80 };

static char config_Panel_IF_Ctrl_10_indx_off[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4C, 0x00, 0x29, 0xff };

static char config_Panel_IF_Ctrl_10b_cmd_off[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4C, 0x00, 0x02, 0xff };

static char config_Power_Ctrl_1a_cmd[8] = {
	0x03, 0x00, 0x39, 0xC0, 0x4C, 0x30, 0x00, 0xff };

static struct mipi_dsi_cmd renesas_panel_video_mode_cmds[] = {
	{sizeof(config_sleep_out), config_sleep_out},
	{sizeof(config_CMD_MODE), config_CMD_MODE},
	{sizeof(config_WRTXHT), config_WRTXHT},
	{sizeof(config_WRTXVT), config_WRTXVT},
	{sizeof(config_PLL2NR), config_PLL2NR},
	{sizeof(config_PLL2NF1), config_PLL2NF1},
	{sizeof(config_PLL2NF2), config_PLL2NF2},
	{sizeof(config_PLL2BWADJ1), config_PLL2BWADJ1},
	{sizeof(config_PLL2BWADJ2), config_PLL2BWADJ2},
	{sizeof(config_PLL2CTL), config_PLL2CTL},
	{sizeof(config_DBICBR), config_DBICBR},
	{sizeof(config_DBICTYPE), config_DBICTYPE},
	{sizeof(config_DBICSET1), config_DBICSET1},
	{sizeof(config_DBICADD), config_DBICADD},
	{sizeof(config_DBICCTL), config_DBICCTL},
	{sizeof(config_COLMOD_888), config_COLMOD_888},
	/* Choose config_COLMOD_565 or config_COLMOD_666PACK for other modes */
	{sizeof(config_MADCTL), config_MADCTL},
	{sizeof(config_DBIOC), config_DBIOC},
	{sizeof(config_CASET), config_CASET},
	{sizeof(config_PASET), config_PASET},
	{sizeof(config_TXON), config_TXON},
	{sizeof(config_BLSET_TM), config_BLSET_TM},
	{sizeof(config_AGCPSCTL_TM), config_AGCPSCTL_TM},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_1_indx), config_Power_Ctrl_1_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_1a_cmd), config_Power_Ctrl_1a_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_2_indx), config_Power_Ctrl_2_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_2a_cmd), config_Power_Ctrl_2a_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Auto_Sequencer_Setting_indx),
	 config_Auto_Sequencer_Setting_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Auto_Sequencer_Setting_a_cmd),
	 config_Auto_Sequencer_Setting_a_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(Driver_Output_Ctrl_indx), Driver_Output_Ctrl_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(Driver_Output_Ctrl_cmd),
	 Driver_Output_Ctrl_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_LCD_drive_AC_Ctrl_indx),
	 config_LCD_drive_AC_Ctrl_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_LCD_drive_AC_Ctrl_cmd),
	 config_LCD_drive_AC_Ctrl_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Entry_Mode_indx),
	 config_Entry_Mode_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Entry_Mode_cmd),
	 config_Entry_Mode_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Display_Ctrl_1_indx),
	 config_Display_Ctrl_1_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Display_Ctrl_1_cmd),
	 config_Display_Ctrl_1_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Display_Ctrl_2_indx),
	 config_Display_Ctrl_2_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Display_Ctrl_2_cmd),
	 config_Display_Ctrl_2_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Display_Ctrl_3_indx),
	 config_Display_Ctrl_3_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Display_Ctrl_3_cmd),
	 config_Display_Ctrl_3_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Display_IF_Ctrl_1_indx),
	 config_Display_IF_Ctrl_1_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Display_IF_Ctrl_1_cmd),
	 config_Display_IF_Ctrl_1_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Display_IF_Ctrl_2_indx),
	 config_Display_IF_Ctrl_2_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Display_IF_Ctrl_2_cmd),
	 config_Display_IF_Ctrl_2_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_1_indx),
	 config_Panel_IF_Ctrl_1_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_1_cmd),
	 config_Panel_IF_Ctrl_1_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_3_indx),
	 config_Panel_IF_Ctrl_3_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_3_cmd),
	 config_Panel_IF_Ctrl_3_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_4_indx),
	 config_Panel_IF_Ctrl_4_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_4_cmd),
	 config_Panel_IF_Ctrl_4_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_5_indx),
	 config_Panel_IF_Ctrl_5_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_5_cmd),
	 config_Panel_IF_Ctrl_5_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_6_indx),
	 config_Panel_IF_Ctrl_6_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_6_cmd),
	 config_Panel_IF_Ctrl_6_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_8_indx),
	 config_Panel_IF_Ctrl_8_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_8_cmd),
	 config_Panel_IF_Ctrl_8_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_9_indx),
	 config_Panel_IF_Ctrl_9_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_9_cmd),
	 config_Panel_IF_Ctrl_9_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_00_indx),
	 config_gam_adjust_00_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_00_cmd),
	 config_gam_adjust_00_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_01_indx),
	 config_gam_adjust_01_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_01_cmd),
	 config_gam_adjust_01_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_02_indx),
	 config_gam_adjust_02_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_02_cmd),
	 config_gam_adjust_02_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_03_indx),
	 config_gam_adjust_03_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_03_cmd),
	 config_gam_adjust_03_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_04_indx), config_gam_adjust_04_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_04_cmd), config_gam_adjust_04_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_05_indx), config_gam_adjust_05_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_05_cmd), config_gam_adjust_05_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_06_indx), config_gam_adjust_06_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_06_cmd), config_gam_adjust_06_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_07_indx), config_gam_adjust_07_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_07_cmd), config_gam_adjust_07_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_08_indx), config_gam_adjust_08_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_08_cmd), config_gam_adjust_08_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_09_indx), config_gam_adjust_09_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_09_cmd), config_gam_adjust_09_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_0A_indx), config_gam_adjust_0A_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_0A_cmd), config_gam_adjust_0A_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_0B_indx), config_gam_adjust_0B_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_0B_cmd), config_gam_adjust_0B_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_0C_indx), config_gam_adjust_0C_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_0C_cmd), config_gam_adjust_0C_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_0D_indx), config_gam_adjust_0D_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_0D_cmd), config_gam_adjust_0D_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_10_indx), config_gam_adjust_10_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_10_cmd), config_gam_adjust_10_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_11_indx), config_gam_adjust_11_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_11_cmd), config_gam_adjust_11_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_12_indx), config_gam_adjust_12_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_12_cmd), config_gam_adjust_12_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_15_indx), config_gam_adjust_15_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_15_cmd), config_gam_adjust_15_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_16_indx), config_gam_adjust_16_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_16_cmd), config_gam_adjust_16_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_17_indx), config_gam_adjust_17_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_17_cmd), config_gam_adjust_17_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_18_indx), config_gam_adjust_18_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_18_cmd), config_gam_adjust_18_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_19_indx), config_gam_adjust_19_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_19_cmd), config_gam_adjust_19_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_1C_indx), config_gam_adjust_1C_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_1C_cmd), config_gam_adjust_1C_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_1D_indx), config_gam_adjust_1D_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_1D_cmd), config_gam_adjust_1D_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_20_indx), config_gam_adjust_20_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_20_cmd), config_gam_adjust_20_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_21_indx), config_gam_adjust_21_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_21_cmd), config_gam_adjust_21_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_22_indx), config_gam_adjust_22_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_22_cmd), config_gam_adjust_22_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_27_indx), config_gam_adjust_27_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_27_cmd), config_gam_adjust_27_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_28_indx), config_gam_adjust_28_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_28_cmd), config_gam_adjust_28_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_gam_adjust_29_indx), config_gam_adjust_29_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_gam_adjust_29_cmd), config_gam_adjust_29_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_1_indx), config_Power_Ctrl_1_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_1b_cmd), config_Power_Ctrl_1b_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_2_indx), config_Power_Ctrl_2_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_2b_cmd), config_Power_Ctrl_2b_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_3_indx), config_Power_Ctrl_3_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_3a_cmd), config_Power_Ctrl_3a_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_4_indx), config_Power_Ctrl_4_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_4a_cmd), config_Power_Ctrl_4a_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_6_indx), config_Power_Ctrl_6_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_6a_cmd), config_Power_Ctrl_6a_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Auto_Sequencer_Setting_indx),
	 config_Auto_Sequencer_Setting_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Auto_Sequencer_Setting_b_cmd),
	 config_Auto_Sequencer_Setting_b_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Panel_IF_Ctrl_10_indx),
	 config_Panel_IF_Ctrl_10_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Panel_IF_Ctrl_10a_cmd),
	 config_Panel_IF_Ctrl_10a_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Auto_Sequencer_Setting_indx),
	 config_Auto_Sequencer_Setting_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Auto_Sequencer_Setting_c_cmd),
	 config_Auto_Sequencer_Setting_c_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD70), config_DBICADD70},
	{sizeof(config_Power_Ctrl_2_indx),
	 config_Power_Ctrl_2_indx},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_DBICADD72), config_DBICADD72},
	{sizeof(config_Power_Ctrl_2c_cmd),
	 config_Power_Ctrl_2c_cmd},
	{sizeof(config_DBICSET_15), config_DBICSET_15},
	{sizeof(config_VIDEO), config_VIDEO}
};

/* Toggle RESET pin of the DSI Client before sending
 * panel init commands
 */
int mipi_renesas_panel_dsi_config(int on)
{
	if (on) {
		gpio_set(128, 0x1);
		mdelay(5);
		gpio_set(128, 0x0);
		gpio_set(129, 0x1);
		gpio_config(129, GPIO_OUTPUT);
		gpio_set(129, 0x0);
		gpio_set(129, 0x1);
		mdelay(10);
	} else {
		gpio_set(128, 0x1);
	}
	return 0;
}

int mipi_renesas_video_fwvga_config(void *pdata)
{
	int ret = NO_ERROR;
	/* 3 Lanes -- Enables Data Lane0, 1, 2 */
	unsigned char lane_en = 3;
	unsigned long low_pwr_stop_mode = 1;

	/* Needed or else will have blank line at top of display */
	unsigned char eof_bllp_pwr = 0x9;

	unsigned char interleav = 0;
	struct lcdc_panel_info *lcdc = NULL;
	struct msm_panel_info *pinfo = (struct msm_panel_info *) pdata;

	if (pinfo == NULL)
		return ERR_INVALID_ARGS;

	lcdc =  &(pinfo->lcdc);
	if (lcdc == NULL)
		return ERR_INVALID_ARGS;

	ret = mipi_dsi_video_mode_config((pinfo->xres),
			(pinfo->yres),
			(pinfo->xres),
			(pinfo->yres),
			(lcdc->h_front_porch),
			(lcdc->h_back_porch),
			(lcdc->v_front_porch),
			(lcdc->v_back_porch),
			(lcdc->h_pulse_width),
			(lcdc->v_pulse_width),
			pinfo->mipi.dst_format,
			pinfo->mipi.traffic_mode,
			lane_en,
			low_pwr_stop_mode,
			eof_bllp_pwr,
			interleav);
	return ret;
}

int mipi_renesas_video_fwvga_on()
{
	int ret = NO_ERROR;
	return ret;
}

int mipi_renesas_video_fwvga_off()
{
	int ret = NO_ERROR;
	return ret;
}

static struct mipi_dsi_phy_ctrl dsi_video_mode_phy_db = {
	/* DSI_BIT_CLK at 500MHz, 2 lane, RGB888 */
	{0x03, 0x01, 0x01, 0x00},	/* regulator */
	/* timing   */
	{0xb9, 0x8e, 0x1f, 0x00, 0x98, 0x9c, 0x22,
	0x90, 0x18, 0x03, 0x04},
	{0x7f, 0x00, 0x00, 0x00},	/* phy ctrl */
	{0xbb, 0x02, 0x06, 0x00},	/* strength */
	/* pll control */
	{0x00, 0xec, 0x31, 0xd2, 0x00, 0x40, 0x37, 0x62,
	0x01, 0x0f, 0x07,	/*  --> Two lane configuration */
	0x05, 0x14, 0x03, 0x0, 0x0, 0x0, 0x20, 0x0, 0x02, 0x0},
};

void mipi_renesas_video_fwvga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = REN_MIPI_FB_WIDTH;
	pinfo->yres = REN_MIPI_FB_HEIGHT;
	pinfo->lcdc.h_back_porch = MIPI_HSYNC_BACK_PORCH_DCLK;
	pinfo->lcdc.h_front_porch = MIPI_HSYNC_FRONT_PORCH_DCLK;
	pinfo->lcdc.h_pulse_width = MIPI_HSYNC_PULSE_WIDTH;
	pinfo->lcdc.v_back_porch = MIPI_VSYNC_BACK_PORCH_LINES;
	pinfo->lcdc.v_front_porch = MIPI_VSYNC_FRONT_PORCH_LINES;
	pinfo->lcdc.v_pulse_width = MIPI_VSYNC_PULSE_WIDTH;
	pinfo->mipi.num_of_lanes = 2;

	pinfo->type = MIPI_VIDEO_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;

	pinfo->mipi.mode = DSI_VIDEO_MODE;
	pinfo->mipi.traffic_mode = 2;
	pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo->mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.lane_swap = 1;
	pinfo->mipi.panel_cmds = renesas_panel_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(renesas_panel_video_mode_cmds);

	pinfo->on = mipi_renesas_video_fwvga_on;
	pinfo->off = mipi_renesas_video_fwvga_off;
	pinfo->config = mipi_renesas_video_fwvga_config;

	return;
}

void mipi_renesas_video_hvga_init(struct msm_panel_info *pinfo)
{
	if (!pinfo)
		return;

	pinfo->xres = REN_MIPI_FB_WIDTH_HVGA;
	pinfo->yres = REN_MIPI_FB_HEIGHT_HVGA;
	pinfo->lcdc.h_front_porch = MIPI_HSYNC_FRONT_PORCH_DCLK_HVGA;
	pinfo->lcdc.h_back_porch = MIPI_HSYNC_BACK_PORCH_DCLK_HVGA;
	pinfo->lcdc.v_front_porch = MIPI_VSYNC_FRONT_PORCH_LINES_HVGA;
	pinfo->lcdc.v_back_porch = MIPI_VSYNC_BACK_PORCH_LINES_HVGA;
	pinfo->lcdc.h_pulse_width = MIPI_HSYNC_PULSE_WIDTH_HVGA;
	pinfo->lcdc.v_pulse_width = MIPI_VSYNC_PULSE_WIDTH_HVGA;
	pinfo->mipi.num_of_lanes = 1;

	pinfo->type = MIPI_VIDEO_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;

	pinfo->mipi.mode = DSI_VIDEO_MODE;
	pinfo->mipi.traffic_mode = 2;
	pinfo->mipi.dst_format = DSI_VIDEO_DST_FORMAT_RGB888;
	pinfo->mipi.dsi_phy_db = &dsi_video_mode_phy_db;
	pinfo->mipi.tx_eot_append = TRUE;

	pinfo->mipi.lane_swap = 1;
	pinfo->mipi.panel_cmds = renesas_panel_video_mode_cmds;
	pinfo->mipi.num_of_panel_cmds = ARRAY_SIZE(renesas_panel_video_mode_cmds);

	pinfo->on = mipi_renesas_video_fwvga_on;
	pinfo->off = mipi_renesas_video_fwvga_off;
	pinfo->config = mipi_renesas_video_fwvga_config;

	return;
}
