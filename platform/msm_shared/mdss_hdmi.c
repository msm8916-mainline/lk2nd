/* Copyright (c) 2010-2016, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
#include <err.h>
#include <debug.h>
#include <reg.h>
#include <malloc.h>
#include <string.h>
#include <msm_panel.h>
#include <platform/timer.h>
#include <platform/clock.h>
#include "mdp5.h"
#include <platform/iomap.h>
#include "mdss_hdmi.h"
#include <target/display.h>

static struct msm_fb_panel_data panel;
extern int msm_display_init(struct msm_fb_panel_data *pdata);
static bool hdmi_power_enabled;
static bool hdmi_panel_clock_enabled;
static bool hdmi_pll_clock_enabled;

/* Supported HDMI Audio channels */
#define MSM_HDMI_AUDIO_CHANNEL_2		1
#define MSM_HDMI_AUDIO_CHANNEL_MAX		8

enum msm_hdmi_supported_audio_sample_rates {
	AUDIO_SAMPLE_RATE_32KHZ,
	AUDIO_SAMPLE_RATE_44_1KHZ,
	AUDIO_SAMPLE_RATE_48KHZ,
	AUDIO_SAMPLE_RATE_88_2KHZ,
	AUDIO_SAMPLE_RATE_96KHZ,
	AUDIO_SAMPLE_RATE_176_4KHZ,
	AUDIO_SAMPLE_RATE_192KHZ,
	AUDIO_SAMPLE_RATE_MAX
};

struct hdmi_msm_audio_acr {
	uint32_t n;	/* N parameter for clock regeneration */
	uint32_t cts;	/* CTS parameter for clock regeneration */
};

struct hdmi_msm_audio_arcs {
	uint32_t pclk;
	struct hdmi_msm_audio_acr lut[AUDIO_SAMPLE_RATE_MAX];
};

#define HDMI_MSM_AUDIO_ARCS(pclk, ...) { pclk, __VA_ARGS__ }

/* Audio constants lookup table for hdmi_msm_audio_acr_setup */
/* Valid Pixel-Clock rates: 25.2MHz, 27MHz, 27.03MHz, 74.25MHz, 148.5MHz */
static struct hdmi_msm_audio_arcs hdmi_audio_acr_lut[] = {
	/*  25.200MHz  */
	HDMI_MSM_AUDIO_ARCS(25200, {
		{4096, 25200}, {6272, 28000}, {6144, 25200}, {12544, 28000},
		{12288, 25200}, {25088, 28000}, {24576, 25200} }),
	/*  27.000MHz  */
	HDMI_MSM_AUDIO_ARCS(27000, {
		{4096, 27000}, {6272, 30000}, {6144, 27000}, {12544, 30000},
		{12288, 27000}, {25088, 30000}, {24576, 27000} }),
	/*  27.027MHz */
	HDMI_MSM_AUDIO_ARCS(27030, {
		{4096, 27027}, {6272, 30030}, {6144, 27027}, {12544, 30030},
		{12288, 27027}, {25088, 30030}, {24576, 27027} }),
	/*  74.250MHz */
	HDMI_MSM_AUDIO_ARCS(74250, {
		{4096, 74250}, {6272, 82500}, {6144, 74250}, {12544, 82500},
		{12288, 74250}, {25088, 82500}, {24576, 74250} }),
	/* 148.500MHz */
	HDMI_MSM_AUDIO_ARCS(148500, {
		{4096, 148500}, {6272, 165000}, {6144, 148500}, {12544, 165000},
		{12288, 148500}, {25088, 165000}, {24576, 148500} }),
	/* 297.000MHz */
	HDMI_MSM_AUDIO_ARCS(297000, {
		{3072, 222750}, {4704, 247500}, {5120, 247500}, {9408, 247500},
		{10240, 247500}, {18816, 247500}, {20480, 247500} }),
};

extern int msm_display_init(struct msm_fb_panel_data *pdata);

/* AVI INFOFRAME DATA */
#define NUM_MODES_AVI 20
#define AVI_MAX_DATA_BYTES 13

enum {
	DATA_BYTE_1,
	DATA_BYTE_2,
	DATA_BYTE_3,
	DATA_BYTE_4,
	DATA_BYTE_5,
	DATA_BYTE_6,
	DATA_BYTE_7,
	DATA_BYTE_8,
	DATA_BYTE_9,
	DATA_BYTE_10,
	DATA_BYTE_11,
	DATA_BYTE_12,
	DATA_BYTE_13,
};

#define IFRAME_PACKET_OFFSET 0x80
/*
 * InfoFrame Type Code:
 * 0x0 - Reserved
 * 0x1 - Vendor Specific
 * 0x2 - Auxiliary Video Information
 * 0x3 - Source Product Description
 * 0x4 - AUDIO
 * 0x5 - MPEG Source
 * 0x6 - NTSC VBI
 * 0x7 - 0xFF - Reserved
 */
#define AVI_IFRAME_TYPE 0x2
#define AVI_IFRAME_VERSION 0x2
#define LEFT_SHIFT_BYTE(x) ((x) << 8)
#define LEFT_SHIFT_WORD(x) ((x) << 16)
#define LEFT_SHIFT_24BITS(x) ((x) << 24)

#define MAX_AUDIO_DATA_BLOCK_SIZE       0x80
#define DBC_START_OFFSET                4
#define VIC_INDEX                       3
#define HDMI_VIC_STR_MAX                3

enum edid_data_block_type {
	RESERVED_DATA_BLOCK1 = 0,
	AUDIO_DATA_BLOCK,
	VIDEO_DATA_BLOCK,
	VENDOR_SPECIFIC_DATA_BLOCK,
	SPEAKER_ALLOCATION_DATA_BLOCK,
	VESA_DTC_DATA_BLOCK,
	RESERVED_DATA_BLOCK2,
	USE_EXTENDED_TAG
};

/* video formats defined by CEA 861D */
#define HDMI_VFRMT_UNKNOWN              0
#define HDMI_VFRMT_640x480p60_4_3       1
#define HDMI_VFRMT_1280x720p60_16_9     4
#define HDMI_VFRMT_1920x1080p60_16_9    16
#define HDMI_VFRMT_MAX                  3

#define DEFAULT_RESOLUTION HDMI_VFRMT_1920x1080p60_16_9
static uint8_t mdss_hdmi_video_fmt = HDMI_VFRMT_UNKNOWN;
static uint8_t mdss_hdmi_pref_fmt  = HDMI_VFRMT_UNKNOWN;
static uint8_t pt_scan_info;
static uint8_t it_scan_info;
static uint8_t ce_scan_info;

static uint8_t mdss_hdmi_edid_buf[0x80];

enum aspect_ratio {
	HDMI_RES_AR_INVALID,
	HDMI_RES_AR_4_3,
	HDMI_RES_AR_5_4,
	HDMI_RES_AR_16_9,
	HDMI_RES_AR_16_10,
	HDMI_RES_AR_MAX,
};

struct mdss_hdmi_timing_info {
	uint32_t	video_format;
	uint32_t	active_h;
	uint32_t	front_porch_h;
	uint32_t	pulse_width_h;
	uint32_t	back_porch_h;
	uint32_t	active_low_h;
	uint32_t	active_v;
	uint32_t	front_porch_v;
	uint32_t	pulse_width_v;
	uint32_t	back_porch_v;
	uint32_t	active_low_v;
	/* Must divide by 1000 to get the actual frequency in MHZ */
	uint32_t	pixel_freq;
	/* Must divide by 1000 to get the actual frequency in HZ */
	uint32_t	refresh_rate;
	uint32_t	interlaced;
	uint32_t	supported;
	enum            aspect_ratio ar;
};

#define HDMI_VFRMT_640x480p60_4_3_TIMING				\
	{HDMI_VFRMT_640x480p60_4_3, 640, 16, 96, 48, true,		\
	 480, 10, 2, 33, true, 25200, 60000, false, true, HDMI_RES_AR_4_3}

#define HDMI_VFRMT_1280x720p60_16_9_TIMING				\
	{HDMI_VFRMT_1280x720p60_16_9, 1280, 110, 40, 220, false,	\
	 720, 5, 5, 20, false, 74250, 60000, false, true, HDMI_RES_AR_16_9}

#define HDMI_VFRMT_1920x1080p60_16_9_TIMING				\
	{HDMI_VFRMT_1920x1080p60_16_9, 1920, 88, 44, 148, false,	\
	 1080, 4, 5, 36, false, 148500, 60000, false, true, HDMI_RES_AR_16_9}

#define MSM_HDMI_MODES_GET_DETAILS(mode, MODE) do {		\
	struct mdss_hdmi_timing_info info = MODE##_TIMING;	\
	*mode = info;						\
	} while (0)

static inline int mdss_hdmi_get_timing_info(
	struct mdss_hdmi_timing_info *mode, int id)
{
	int ret = 0;

	switch (id) {
	case HDMI_VFRMT_640x480p60_4_3:
		MSM_HDMI_MODES_GET_DETAILS(mode, HDMI_VFRMT_640x480p60_4_3);
		break;

	case HDMI_VFRMT_1280x720p60_16_9:
		MSM_HDMI_MODES_GET_DETAILS(mode, HDMI_VFRMT_1280x720p60_16_9);
		break;

	case HDMI_VFRMT_1920x1080p60_16_9:
		MSM_HDMI_MODES_GET_DETAILS(mode, HDMI_VFRMT_1920x1080p60_16_9);
		break;

	default:
		ret = ERROR;
	}

	return ret;
}


/*
 * 13 Bytes of AVI infoframe data wrt each resolution
 * Data Byte 01: 0 Y1 Y0 A0 B1 B0 S1 S0
 * Data Byte 02: C1 C0 M1 M0 R3 R2 R1 R0
 * Data Byte 03: ITC EC2 EC1 EC0 Q1 Q0 SC1 SC0
 * Data Byte 04: 0 VIC6 VIC5 VIC4 VIC3 VIC2 VIC1 VIC0
 * Data Byte 05: 0 0 0 0 PR3 PR2 PR1 PR0
 * Data Byte 06: LSB Line No of End of Top Bar
 * Data Byte 07: MSB Line No of End of Top Bar
 * Data Byte 08: LSB Line No of Start of Bottom Bar
 * Data Byte 09: MSB Line No of Start of Bottom Bar
 * Data Byte 10: LSB Pixel Number of End of Left Bar
 * Data Byte 11: MSB Pixel Number of End of Left Bar
 * Data Byte 12: LSB Pixel Number of Start of Right Bar
 * Data Byte 13: MSB Pixel Number of Start of Right Bar
 */
static uint8_t mdss_hdmi_avi_info_db[HDMI_VFRMT_MAX][AVI_MAX_DATA_BYTES] = {
	/* 480p */
	{0x10, 0x18, 0x00, 0x01, 0x00, 0x00, 0x00,
		0xE1, 0x01, 0x00, 0x00, 0x81, 0x02},
	/* 720p */
	{0x10, 0x28, 0x00, 0x04, 0x00, 0x00, 0x00,
		0xD1, 0x02, 0x00, 0x00, 0x01, 0x05},
	/* 1080p */
	{0x10, 0x28, 0x00, 0x10, 0x00, 0x00, 0x00,
		0x39, 0x04, 0x00, 0x00, 0x81, 0x07},
};

static void mdss_hdmi_audio_acr_setup(uint32_t sample_rate)
{
	/* Read first before writing */
	uint32_t acr_pck_ctrl_reg = readl(HDMI_ACR_PKT_CTRL);
	struct mdss_hdmi_timing_info tinfo = {0};
	uint32_t ret = mdss_hdmi_get_timing_info(&tinfo, mdss_hdmi_video_fmt);
	struct hdmi_msm_audio_arcs *audio_acr = &hdmi_audio_acr_lut[0];
	uint32_t lut_size = sizeof(hdmi_audio_acr_lut)
		/ sizeof(*hdmi_audio_acr_lut);
	uint32_t i, n, cts, layout, multiplier, aud_pck_ctrl_2_reg;
	uint32_t channel_num = MSM_HDMI_AUDIO_CHANNEL_2;

	if (ret || !tinfo.supported) {
		dprintf(CRITICAL, "%s: video format %d not supported\n",
			__func__, mdss_hdmi_video_fmt);
		return;
	}

	for (i = 0; i < lut_size; audio_acr = &hdmi_audio_acr_lut[++i]) {
		if (audio_acr->pclk == tinfo.pixel_freq)
			break;
	}

	if (i >= lut_size) {
		dprintf(CRITICAL, "%s: pixel clk %d not supported\n", __func__,
			tinfo.pixel_freq);
		return;
	}

	n = audio_acr->lut[sample_rate].n;
	cts = audio_acr->lut[sample_rate].cts;
	layout = (MSM_HDMI_AUDIO_CHANNEL_2 == channel_num) ? 0 : 1;

	if ((AUDIO_SAMPLE_RATE_192KHZ == sample_rate) ||
		(AUDIO_SAMPLE_RATE_176_4KHZ == sample_rate)) {
		multiplier = 4;
		n >>= 2; /* divide N by 4 and use multiplier */
	} else if ((AUDIO_SAMPLE_RATE_96KHZ == sample_rate) ||
		(AUDIO_SAMPLE_RATE_88_2KHZ == sample_rate)) {
		multiplier = 2;
		n >>= 1; /* divide N by 2 and use multiplier */
	} else {
		multiplier = 1;
	}

	dprintf(SPEW, "%s: n=%u, cts=%u, layout=%u\n", __func__, n, cts,
		layout);

	/* AUDIO_PRIORITY | SOURCE */
	acr_pck_ctrl_reg |= 0x80000100;

	/* Reset multiplier bits */
	acr_pck_ctrl_reg &= ~(7 << 16);

	/* N_MULTIPLE(multiplier) */
	acr_pck_ctrl_reg |= (multiplier & 7) << 16;

	if ((AUDIO_SAMPLE_RATE_48KHZ == sample_rate) ||
	(AUDIO_SAMPLE_RATE_96KHZ == sample_rate) ||
	(AUDIO_SAMPLE_RATE_192KHZ == sample_rate)) {
		/* SELECT(3) */
		acr_pck_ctrl_reg |= 3 << 4;
		/* CTS_48 */
		cts <<= 12;

		/* CTS: need to determine how many fractional bits */
		writel(cts, HDMI_ACR_48_0);
		/* N */
		writel(n, HDMI_ACR_48_1);
	} else if ((AUDIO_SAMPLE_RATE_44_1KHZ == sample_rate) ||
		(AUDIO_SAMPLE_RATE_88_2KHZ == sample_rate) ||
		(AUDIO_SAMPLE_RATE_176_4KHZ == sample_rate)) {
		/* SELECT(2) */
		acr_pck_ctrl_reg |= 2 << 4;
		/* CTS_44 */
		cts <<= 12;

		/* CTS: need to determine how many fractional bits */
		writel(cts, HDMI_ACR_44_0);
		/* N */
		writel(n, HDMI_ACR_44_1);
	} else {	/* default to 32k */
		/* SELECT(1) */
		acr_pck_ctrl_reg |= 1 << 4;
		/* CTS_32 */
		cts <<= 12;

		/* CTS: need to determine how many fractional bits */
		writel(cts, HDMI_ACR_32_0);
		/* N */
		writel(n, HDMI_ACR_32_1);
	}

	/* Payload layout depends on number of audio channels */
	/* LAYOUT_SEL(layout) */
	aud_pck_ctrl_2_reg = 1 | (layout << 1);
	/* override | layout */
	writel(aud_pck_ctrl_2_reg, HDMI_AUDIO_PKT_CTRL2);

	/* SEND | CONT */
	acr_pck_ctrl_reg |= 0x00000003;

	writel(acr_pck_ctrl_reg, HDMI_ACR_PKT_CTRL);
}

static void mdss_hdmi_audio_info_setup(void)
{
	uint32_t channel_count = MSM_HDMI_AUDIO_CHANNEL_2;
	uint32_t channel_allocation = 0;
	uint32_t level_shift = 0;
	uint32_t down_mix = 0;
	uint32_t check_sum, audio_info_0_reg, audio_info_1_reg;
	uint32_t audio_info_ctrl_reg;
	uint32_t aud_pck_ctrl_2_reg;
	uint32_t layout;

	layout = (MSM_HDMI_AUDIO_CHANNEL_2 == channel_count) ? 0 : 1;;
	aud_pck_ctrl_2_reg = 1 | (layout << 1);
	writel(aud_pck_ctrl_2_reg, HDMI_AUDIO_PKT_CTRL2);

	/* Read first then write because it is bundled with other controls */
	audio_info_ctrl_reg = readl(HDMI_INFOFRAME_CTRL0);

	channel_allocation = 0;	/* Default to FR,FL */

	/* Program the Channel-Speaker allocation */
	audio_info_1_reg = 0;

	/* CA(channel_allocation) */
	audio_info_1_reg |= channel_allocation & 0xff;

	/* Program the Level shifter */
	/* LSV(level_shift) */
	audio_info_1_reg |= (level_shift << 11) & 0x00007800;

	/* Program the Down-mix Inhibit Flag */
	/* DM_INH(down_mix) */
	audio_info_1_reg |= (down_mix << 15) & 0x00008000;

	writel(audio_info_1_reg, HDMI_AUDIO_INFO1);

	check_sum = 0;
	/* HDMI_AUDIO_INFO_FRAME_PACKET_HEADER_TYPE[0x84] */
	check_sum += 0x84;
	/* HDMI_AUDIO_INFO_FRAME_PACKET_HEADER_VERSION[0x01] */
	check_sum += 1;
	/* HDMI_AUDIO_INFO_FRAME_PACKET_LENGTH[0x0A] */
	check_sum += 0x0A;
	check_sum += channel_count;
	check_sum += channel_allocation;
	/* See Table 8.5 in HDMI spec */
	check_sum += (level_shift & 0xF) << 3 | (down_mix & 0x1) << 7;
	check_sum &= 0xFF;
	check_sum = (256 - check_sum);

	audio_info_0_reg = 0;
	/* CHECKSUM(check_sum) */
	audio_info_0_reg |= check_sum & 0xff;
	/* CC(channel_count) */
	audio_info_0_reg |= (channel_count << 8) & 0x00000700;

	writel(audio_info_0_reg, HDMI_AUDIO_INFO0);

	/* Set these flags */
	/* AUDIO_INFO_UPDATE | AUDIO_INFO_SOURCE | AUDIO_INFO_CONT
	 | AUDIO_INFO_SEND */
	audio_info_ctrl_reg |= 0xF0;

	/* HDMI_INFOFRAME_CTRL0[0x002C] */
	writel(audio_info_ctrl_reg, HDMI_INFOFRAME_CTRL0);
}

static uint8_t* hdmi_edid_find_block(uint32_t start_offset,
		uint8_t type, uint8_t *len)
{
	/* the start of data block collection, start of Video Data Block */
	uint8_t *in_buf = mdss_hdmi_edid_buf;
	uint32_t offset = start_offset;
	uint32_t end_dbc_offset = in_buf[2];

	*len = 0;

	/*
	 * edid buffer 1, byte 2 being 4 means no non-DTD/Data block collection
	 * present.
	 * edid buffer 1, byte 2 being 0 means no non-DTD/DATA block collection
	 * present and no DTD data present.
	 */
	if ((end_dbc_offset == 0) || (end_dbc_offset == 4))
		return NULL;

	while (offset < end_dbc_offset) {
		uint8_t block_len = in_buf[offset] & 0x1F;
		if ((in_buf[offset] >> 5) == type) {
			*len = block_len;
			dprintf(SPEW,
				"EDID: block=%d found @ %d with length=%d\n",
				type, offset, block_len);
			return in_buf + offset;
		}
		offset += 1 + block_len;
	}

	return NULL;
}

static bool mdss_hdmi_is_audio_freq_supported(uint32_t freq)
{
	uint8_t *in_buf = mdss_hdmi_edid_buf;
	const uint8_t *adb = NULL;
	uint32_t adb_size = 0;
	uint8_t len = 0, count = 0;
	uint32_t next_offset = DBC_START_OFFSET;
	uint8_t audio_data_block[MAX_AUDIO_DATA_BLOCK_SIZE];

	do {
		adb = hdmi_edid_find_block(next_offset,
			AUDIO_DATA_BLOCK, &len);

		if ((adb_size + len) > MAX_AUDIO_DATA_BLOCK_SIZE) {
			dprintf(INFO, "%s: invalid adb length\n", __func__);
			break;
		}

		if (!adb)
			break;

		memcpy((audio_data_block + adb_size), adb + 1, len);
		next_offset = (adb - in_buf) + 1 + len;

		adb_size += len;

	} while (adb);

	count = adb_size/3;
	adb = audio_data_block;

	while (count--) {
		uint8_t freq_lst = *(adb + 1) & 0xFF;

		if (freq_lst & BIT(freq))
			return true;

		adb += 3;
	}

	return false;
}

static void mdss_hdmi_audio_playback(void)
{
	char *base_addr;
	uint32_t sample_rate;

	base_addr = (char *) memalign(4096, 0x1000);
	if (base_addr == NULL) {
		dprintf(CRITICAL, "%s: Error audio buffer alloc\n", __func__);
		return;
	}

	memset(base_addr, 0, 0x1000);

	writel(0x00000010, HDMI_AUDIO_PKT_CTRL);
	writel(0x00000080, HDMI_AUDIO_CFG);

	writel(0x0000096E, LPASS_LPAIF_RDDMA_CTL0);
	writel(0x00000A6E, LPASS_LPAIF_RDDMA_CTL0);
	writel(0x00002000, HDMI_VBI_PKT_CTRL);
	writel(0x00000000, HDMI_GEN_PKT_CTRL);
	writel(0x0000096E, LPASS_LPAIF_RDDMA_CTL0);
	writel((uint32_t) base_addr,  LPASS_LPAIF_RDDMA_BASE0);
	writel(0x000005FF, LPASS_LPAIF_RDDMA_BUFF_LEN0);
	writel(0x000005FF, LPASS_LPAIF_RDDMA_PER_LEN0);
	writel(0x0000096F, LPASS_LPAIF_RDDMA_CTL0);
	writel(0x00000010, LPASS_LPAIF_DEBUG_CTL);
	writel(0x00000000, HDMI_GC);
	writel(0x00002030, HDMI_VBI_PKT_CTRL);
	writel(0x00002030, HDMI_VBI_PKT_CTRL);
	writel(0x00002030, HDMI_VBI_PKT_CTRL);

	if (mdss_hdmi_is_audio_freq_supported(AUDIO_SAMPLE_RATE_48KHZ))
		sample_rate = AUDIO_SAMPLE_RATE_48KHZ;
	else
		sample_rate = AUDIO_SAMPLE_RATE_32KHZ;

	mdss_hdmi_audio_acr_setup(sample_rate);
	mdss_hdmi_audio_info_setup();

	writel(0x00000010, HDMI_AUDIO_PKT_CTRL);
	writel(0x00000080, HDMI_AUDIO_CFG);
	writel(0x00000011, HDMI_AUDIO_PKT_CTRL);
	writel(0x00000081, HDMI_AUDIO_CFG);

	dprintf(SPEW, "audio sample rate %s\n",
		sample_rate == AUDIO_SAMPLE_RATE_48KHZ ? "48KHz" : "32KHz");
}

static uint32_t mdss_hdmi_panel_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;
	if (hdmi_panel_clock_enabled)
		return ret;

	ret = target_hdmi_panel_clock(enable, pinfo);

	hdmi_panel_clock_enabled = enable;

	return ret;
}

static uint32_t  mdss_hdmi_pll_clock(uint8_t enable, struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;

	if (hdmi_pll_clock_enabled)
		return ret;

	ret = target_hdmi_pll_clock(enable, pinfo);

	hdmi_pll_clock_enabled = enable;

	return ret;
}

static int mdss_hdmi_enable_power(uint8_t enable, struct msm_panel_info *pinfo)
{
	int ret = NO_ERROR;

	if (hdmi_power_enabled)
		return ret;

        ret = target_hdmi_regulator_ctrl(enable);
        if (ret) {
		dprintf(CRITICAL, "hdmi regulator control enable failed\n");
		goto bail_regulator_fail;
	}

	ret = target_hdmi_gpio_ctrl(enable);
	if (ret) {
		dprintf(CRITICAL, "hdmi gpio control enable failed\n");
		goto bail_gpio_fail;
	}

	hdmi_power_enabled = enable;

	dprintf(SPEW, "HDMI Panel power %s done\n", enable ? "on" : "off");

	return ret;

bail_gpio_fail:
	target_hdmi_regulator_ctrl(0);

bail_regulator_fail:
	return ret;
}

static bool mdss_hdmi_is_dvi_mode(void)
{
	uint8_t len;
	uint32_t ieee_tag;
	const uint8_t *vsd = NULL;

	vsd = hdmi_edid_find_block(DBC_START_OFFSET,
		VENDOR_SPECIFIC_DATA_BLOCK, &len);

	if (vsd == NULL || len == 0) {
		dprintf(SPEW, "%s: Invalid VSDB\n", __func__);
                return false;
	}

	ieee_tag = ((uint32_t) vsd[3] << 16) + ((uint32_t) vsd[2] << 8) +
			(uint32_t) vsd[1];

	if (ieee_tag == 0x0c03)
		return false;
	else
		return true;
}

static void mdss_hdmi_set_mode(bool on)
{
	uint32_t val = 0;

	if (on) {
		/* tx on */
		val = 0x1;

		if (!mdss_hdmi_is_dvi_mode())
			val |= 0x2;
	}

	writel(val, HDMI_CTRL);
}


static int mdss_hdmi_read_edid(void)
{
	uint8_t ndx;
	uint32_t reg_val;
	uint32_t dev_addr = 0xA0;
	uint32_t len = 0x80;
	uint32_t offset = 0x80;
	uint32_t time_out;
	uint32_t retry = 5;

	dev_addr &= 0xFE;

again:
	time_out = 10;
	writel(readl(HDMI_DDC_ARBITRATION) & ~BIT(4), HDMI_DDC_ARBITRATION);

	/* Enable DDC Interrupts */
	writel(BIT(1) | BIT(2), HDMI_DDC_INT_CTRL);

	/* config DDC to read CEA block */
	writel((10 << 16) | (2 << 0), HDMI_DDC_SPEED);
	writel(0xFF000000, HDMI_DDC_SETUP);
	writel((1 << 16) | (19 << 0), HDMI_DDC_REF);
	writel(BIT(31) | (dev_addr << 8), HDMI_DDC_DATA);
	writel(offset << 8, HDMI_DDC_DATA);
	writel((dev_addr | BIT(0)) << 8, HDMI_DDC_DATA);
	writel(BIT(12) | BIT(16), HDMI_DDC_TRANS0);
	writel(BIT(0) | BIT(12) | BIT(13) | (len << 16), HDMI_DDC_TRANS1);
	writel(BIT(0) | BIT(20), HDMI_DDC_CTRL);

	/* poll for 100ms for read to complete */
	reg_val = readl(HDMI_DDC_INT_CTRL);
	while (!(reg_val & BIT(0)) && time_out) {
		reg_val = readl(HDMI_DDC_INT_CTRL);
		time_out--;
		mdelay(10);
	}

	if (!time_out) {
		dprintf(CRITICAL, "%s: Timeout reading EDID\n", __func__);
		if (retry--)
			goto again;
		else
			return ERROR;
	}

	/* clear interrupts */
	writel(BIT(1), HDMI_DDC_INT_CTRL);

	reg_val = readl(HDMI_DDC_SW_STATUS);
	reg_val &= BIT(12) | BIT(13) | BIT(14) | BIT(15);

	/* Check if any NACK occurred */
	if (reg_val) {
		/* SW_STATUS_RESET */
		writel(BIT(3), HDMI_DDC_CTRL);

		/* SOFT_RESET */
		writel(BIT(1), HDMI_DDC_CTRL);

		dprintf(CRITICAL, "%s: NACK reading EDID\n", __func__);

		if (retry--)
			goto again;
		else
			return ERROR;
	}

	/* Write this data to DDC buffer */
	writel(BIT(0) | (3 << 16) | BIT(31), HDMI_DDC_DATA);

	/* Discard first byte */
	readl(HDMI_DDC_DATA);

	for (ndx = 0; ndx < 0x80; ndx++) {
		reg_val = readl(HDMI_DDC_DATA);
		mdss_hdmi_edid_buf[ndx] = (uint8_t)((reg_val & 0x0000FF00) >> 8);
	}

	dprintf(INFO, "%s: EDID read successful\n", __func__);

	return NO_ERROR;
}

static void mdss_hdmi_parse_res(void)
{
	uint8_t len, i;
	uint32_t video_format;
	struct mdss_hdmi_timing_info tinfo_fmt = {0};

	uint8_t *svd = hdmi_edid_find_block(DBC_START_OFFSET,
			VIDEO_DATA_BLOCK, &len);

	mdss_hdmi_video_fmt = HDMI_VFRMT_UNKNOWN;

	if (!svd) {
		mdss_hdmi_video_fmt = DEFAULT_RESOLUTION;
		return;
	}

	++svd;

	for (i = 0; i < len; ++i, ++svd) {
		struct mdss_hdmi_timing_info tinfo = {0};
		uint32_t ret = 0;

		video_format = (*svd & 0x7F);

		if (i == 0)
			mdss_hdmi_pref_fmt = video_format;

		ret = mdss_hdmi_get_timing_info(&tinfo, video_format);

		if (ret || !tinfo.supported)
			continue;

		if (!tinfo_fmt.video_format) {
			memcpy(&tinfo_fmt, &tinfo, sizeof(tinfo));
			mdss_hdmi_video_fmt = video_format;
			continue;
		}

		if (tinfo.active_v > tinfo_fmt.active_v) {
			memcpy(&tinfo_fmt, &tinfo, sizeof(tinfo));
			mdss_hdmi_video_fmt = video_format;
		}
	}

	if (mdss_hdmi_video_fmt == HDMI_VFRMT_UNKNOWN)
		mdss_hdmi_video_fmt = DEFAULT_RESOLUTION;
}

void mdss_hdmi_get_vic(char *buf)
{
	struct mdss_hdmi_timing_info tinfo = {0};
	uint32_t ret = mdss_hdmi_get_timing_info(&tinfo, mdss_hdmi_video_fmt);

	if (ret)
		snprintf(buf, HDMI_VIC_STR_MAX, "%d", HDMI_VFRMT_UNKNOWN);
	else
		snprintf(buf, HDMI_VIC_STR_MAX, "%d", tinfo.video_format);

}

static void mdss_hdmi_panel_init(struct msm_panel_info *pinfo)
{
	struct mdss_hdmi_timing_info tinfo = {0};
	uint32_t ret = mdss_hdmi_get_timing_info(&tinfo, mdss_hdmi_video_fmt);

	if (!pinfo || ret)
		return;

	pinfo->xres = tinfo.active_h;
	pinfo->yres = tinfo.active_v;
	pinfo->bpp  = 24;
	pinfo->type = HDMI_PANEL;
	pinfo->clk_rate = tinfo.pixel_freq * 1000;

	pinfo->lcdc.h_back_porch  = tinfo.back_porch_h;
	pinfo->lcdc.h_front_porch = tinfo.front_porch_h;
	pinfo->lcdc.h_pulse_width = tinfo.pulse_width_h;
	pinfo->lcdc.v_back_porch  = tinfo.back_porch_v;
	pinfo->lcdc.v_front_porch = tinfo.front_porch_v;
	pinfo->lcdc.v_pulse_width = tinfo.pulse_width_v;

	pinfo->lcdc.hsync_skew = 0;
	pinfo->lcdc.xres_pad   = 0;
	pinfo->lcdc.yres_pad   = 0;
	pinfo->lcdc.dual_pipe  = 0;
}

static uint8_t mdss_hdmi_cable_status(void)
{
	uint32_t reg_val;
	uint8_t cable_status;

	mdss_hdmi_set_mode(true);

	/* Enable USEC REF timer */
	writel(0x0001001B, HDMI_USEC_REFTIMER);

	/* set timeout to 4.1ms (max) for hardware debounce */
	reg_val = readl(HDMI_HPD_CTRL) | 0x1FFF;

	/* enable HPD circuit */
	writel(reg_val | BIT(28), HDMI_HPD_CTRL);

	writel(BIT(2) | BIT(1), HDMI_HPD_INT_CTRL);

	/* Toggle HPD circuit to trigger HPD sense */
	reg_val = readl(HDMI_HPD_CTRL) | 0x1FFF;
	writel(reg_val & ~BIT(28), HDMI_HPD_CTRL);
	writel(reg_val & BIT(28), HDMI_HPD_CTRL);

	mdelay(20);

	cable_status = (readl(HDMI_HPD_INT_STATUS) & BIT(1)) >> 1;

	dprintf(INFO, "hdmi cable %s\n",
		cable_status ? "connected" : "not connected");

	writel(BIT(0), HDMI_HPD_INT_CTRL);
	writel(reg_val & ~BIT(28), HDMI_HPD_CTRL);

	mdss_hdmi_set_mode(false);

	return cable_status;
}

static int mdss_hdmi_update_panel_info(void)
{
	mdss_hdmi_set_mode(true);

	if (!mdss_hdmi_read_edid())
		mdss_hdmi_parse_res();
	else
		mdss_hdmi_video_fmt = DEFAULT_RESOLUTION;

	mdss_hdmi_set_mode(false);

	mdss_hdmi_panel_init(&(panel.panel_info));

	panel.fb.width   = panel.panel_info.xres;
	panel.fb.height  = panel.panel_info.yres;
	panel.fb.stride  = panel.panel_info.xres;
	panel.fb.bpp     = panel.panel_info.bpp;
	panel.fb.format  = FB_FORMAT_RGB888;

	return NO_ERROR;
}

void mdss_hdmi_display_init(uint32_t rev, void *base)
{
	panel.power_func        = mdss_hdmi_enable_power;
	panel.clk_func          = mdss_hdmi_panel_clock;
	panel.update_panel_info = mdss_hdmi_update_panel_info;
	panel.pll_clk_func      = mdss_hdmi_pll_clock;

	panel.fb.base = base;
	panel.mdp_rev = rev;

	msm_display_init(&panel);
}

static int mdss_hdmi_video_setup(void)
{
	uint32_t total_v   = 0;
	uint32_t total_h   = 0;
	uint32_t start_h   = 0;
	uint32_t end_h     = 0;
	uint32_t start_v   = 0;
	uint32_t end_v     = 0;

	struct mdss_hdmi_timing_info tinfo = {0};
	uint32_t ret = mdss_hdmi_get_timing_info(&tinfo, mdss_hdmi_video_fmt);

	if (ret)
		return ERROR;

	dprintf(INFO, "hdmi resolution %dx%d@p%dHz (%d)\n",
		tinfo.active_h, tinfo.active_v, tinfo.refresh_rate/1000,
		mdss_hdmi_video_fmt);

	total_h = tinfo.active_h + tinfo.front_porch_h +
		tinfo.back_porch_h + tinfo.pulse_width_h - 1;
	total_v = tinfo.active_v + tinfo.front_porch_v +
		tinfo.back_porch_v + tinfo.pulse_width_v - 1;
	if (((total_v << 16) & 0xE0000000) || (total_h & 0xFFFFE000)) {
		dprintf(CRITICAL,
			"%s: total v=%d or h=%d is larger than supported\n",
			__func__, total_v, total_h);
		return ERROR;
	}
	writel((total_v << 16) | (total_h << 0), HDMI_TOTAL);

	start_h = tinfo.back_porch_h + tinfo.pulse_width_h;
	end_h   = (total_h + 1) - tinfo.front_porch_h;
	if (((end_h << 16) & 0xE0000000) || (start_h & 0xFFFFE000)) {
		dprintf(CRITICAL,
			"%s: end_h=%d or start_h=%d is larger than supported\n",
			__func__, end_h, start_h);
		return ERROR;
	}
	writel((end_h << 16) | (start_h << 0), HDMI_ACTIVE_H);

	start_v = tinfo.back_porch_v + tinfo.pulse_width_v - 1;
	end_v   = total_v - tinfo.front_porch_v;
	if (((end_v << 16) & 0xE0000000) || (start_v & 0xFFFFE000)) {
		dprintf(CRITICAL,
			"%s: end_v=%d or start_v=%d is larger than supported\n",
			__func__, end_v, start_v);
		return ERROR;
	}
	writel((end_v << 16) | (start_v << 0), HDMI_ACTIVE_V);

	if (tinfo.interlaced) {
		writel((total_v + 1) << 0, HDMI_V_TOTAL_F2);
		writel(((end_v + 1) << 16) | ((start_v + 1) << 0),
			HDMI_ACTIVE_V_F2);
	} else {
		writel(0, HDMI_V_TOTAL_F2);
		writel(0, HDMI_ACTIVE_V_F2);
	}

	writel(((tinfo.interlaced << 31) & 0x80000000) |
		((tinfo.active_low_h << 29) & 0x20000000) |
		((tinfo.active_low_v << 28) & 0x10000000), HDMI_FRAME_CTRL);

	return 0;
}

static void mdss_hdmi_extract_extended_data_blocks(void)
{
	uint8_t len = 0;
	uint32_t start_offset = DBC_START_OFFSET;
	uint8_t const *etag = NULL;
	uint8_t *in_buf = mdss_hdmi_edid_buf;

	do {
		/* A Tage code of 7 identifies extended data blocks */
		etag = hdmi_edid_find_block(start_offset,
			USE_EXTENDED_TAG, &len);

		start_offset = etag - in_buf + len + 1;

		/* The extended data block should at least be 2 bytes long */
		if (len < 2) {
			dprintf(SPEW, "%s: data block of len < 2 bytes\n",
				__func__);
			continue;
		}

		/*
		 * The second byte of the extended data block has the
		 * extended tag code
		 */
		switch (etag[1]) {
		case 0:
			/*
			 * Check if the sink specifies underscan
			 * support for:
			 * BIT 5: preferred video format
			 * BIT 3: IT video format
			 * BIT 1: CE video format
			 */
			pt_scan_info = (etag[2] & (BIT(4) | BIT(5))) >> 4;
			it_scan_info = (etag[2] & (BIT(3) | BIT(2))) >> 2;
			ce_scan_info = etag[2] & (BIT(1) | BIT(0));

			dprintf(INFO, "scan Info (pt|it|ce): (%d|%d|%d)\n",
				pt_scan_info, it_scan_info, ce_scan_info);
			break;
		default:
			dprintf(SPEW, "%s: Tag Code %d not supported\n",
				__func__, etag[1]);
			break;
		}
	} while (etag != NULL);
}

/*
 * If the sink specified support for both underscan/overscan then, by default,
 * set the underscan bit. Only checking underscan support for preferred
 * format and cea formats.
 */
uint8_t mdss_hdmi_get_scan_info(void)
{
	uint8_t scaninfo = 0;
	bool use_ce_scan_info = true;

	mdss_hdmi_extract_extended_data_blocks();

	if (mdss_hdmi_video_fmt == mdss_hdmi_pref_fmt) {
		use_ce_scan_info = false;

		switch (pt_scan_info) {
		case 0:
			use_ce_scan_info = true;
			break;
		case 3:
			scaninfo = BIT(1);
			break;
		default:
			break;
		}
	}

	if (use_ce_scan_info) {
		if (3 == ce_scan_info)
			scaninfo |= BIT(1);
	}

	return scaninfo;
}

void mdss_hdmi_avi_info_frame(void)
{
	uint32_t sum;
	uint32_t reg_val;
	uint8_t checksum;
	uint8_t scaninfo;
	uint32_t i, index;

	scaninfo = mdss_hdmi_get_scan_info();

	sum = IFRAME_PACKET_OFFSET + AVI_IFRAME_TYPE +
		AVI_IFRAME_VERSION + AVI_MAX_DATA_BYTES;

	for (index = 0; index < HDMI_VFRMT_MAX; index++) {
		if (mdss_hdmi_avi_info_db[index][VIC_INDEX] == mdss_hdmi_video_fmt)
			break;
	}

	if (index == VIC_INDEX)
		return;

	mdss_hdmi_avi_info_db[index][DATA_BYTE_1] |=
		scaninfo & (BIT(1) | BIT(0));

	for (i = 0; i < AVI_MAX_DATA_BYTES; i++)
		sum += mdss_hdmi_avi_info_db[index][i];

	sum &= 0xFF;
	sum = 256 - sum;
	checksum = (uint8_t) sum;

	reg_val = checksum |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[index][DATA_BYTE_1]) |
		LEFT_SHIFT_WORD(mdss_hdmi_avi_info_db[index][DATA_BYTE_2]) |
		LEFT_SHIFT_24BITS(mdss_hdmi_avi_info_db[index][DATA_BYTE_3]);
	writel(reg_val, HDMI_AVI_INFO0);

	reg_val = mdss_hdmi_avi_info_db[index][DATA_BYTE_4] |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[index][DATA_BYTE_5]) |
		LEFT_SHIFT_WORD(mdss_hdmi_avi_info_db[index][DATA_BYTE_6]) |
		LEFT_SHIFT_24BITS(mdss_hdmi_avi_info_db[index][DATA_BYTE_7]);
	writel(reg_val, HDMI_AVI_INFO1);

	reg_val = mdss_hdmi_avi_info_db[index][DATA_BYTE_8] |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[index][DATA_BYTE_9]) |
		LEFT_SHIFT_WORD(mdss_hdmi_avi_info_db[index][DATA_BYTE_10]) |
		LEFT_SHIFT_24BITS(mdss_hdmi_avi_info_db[index][DATA_BYTE_11]);
	writel(reg_val, HDMI_AVI_INFO2);

	reg_val = mdss_hdmi_avi_info_db[index][DATA_BYTE_12] |
		LEFT_SHIFT_BYTE(mdss_hdmi_avi_info_db[index][DATA_BYTE_13]) |
		LEFT_SHIFT_24BITS(AVI_IFRAME_VERSION);
	writel(reg_val, HDMI_AVI_INFO3);

	/* AVI InfFrame enable (every frame) */
	writel(readl(HDMI_INFOFRAME_CTRL0) | BIT(1) | BIT(0),
		HDMI_INFOFRAME_CTRL0);
}

int mdss_hdmi_init(void)
{
	bool is_dvi_mode = mdss_hdmi_is_dvi_mode();

	mdss_hdmi_set_mode(false);

	/* Audio settings */
	if (!is_dvi_mode)
		mdss_hdmi_audio_playback();

	/* Video settings */
	mdss_hdmi_video_setup();

	/* AVI info settings */
	if (!is_dvi_mode)
		mdss_hdmi_avi_info_frame();

	/* Enable HDMI */
	mdss_hdmi_set_mode(true);

	return 0;
}
