/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include "edp.h"

struct edp_aux_ctrl edpctrl;

int edp_hpd_done = 0;
int edp_video_ready = 0;

/*
 * edid
 */
static char edid_hdr[8] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};


int edp_edid_buf_error(char *buf, int len)
{
	char *bp;
	int i;
	char csum = 0;
	int ret = 0;

	bp = buf;
	if (len < 128) {
		dprintf(INFO, "%s: Error: len=%x\n", __func__, len);
		return -1;
	}

	for (i = 0; i < 128; i++)
		csum += *bp++;

	if (csum != 0) {
		dprintf(INFO, "%s: Error: csum=%x\n", __func__, csum);
		return -1;
	}

	if (buf[1] != 0xff) {
		dprintf(INFO, "%s: Error: header\n", __func__);
		return -1;
	}

	return ret;
}


void edp_extract_edid_manufacturer(struct edp_edid *edid, char *buf)
{
	char *bp;
	char data;

	bp = &buf[8];
	data = *bp & 0x7f;
	data >>= 2;
	edid->id_name[0] = 'A' + data - 1;
	data = *bp & 0x03;
	data <<= 3;
	bp++;
	data |= (*bp >> 5);
	edid->id_name[1] = 'A' + data - 1;
	data = *bp & 0x1f;
	edid->id_name[2] = 'A' + data - 1;
	edid->id_name[3] = 0;

	dprintf(SPEW, "%s: edid manufacturer = %s", __func__, edid->id_name);
}

void edp_extract_edid_product(struct edp_edid *edid, char *buf)
{
	char *bp;
	int data;

	bp = &buf[0x0a];
	data =  *bp;
	edid->id_product = *bp++;
	edid->id_product &= 0x0ff;
	data = *bp & 0x0ff;
	data <<= 8;
	edid->id_product |= data;

	dprintf(SPEW, "%s: edid product = 0x%x\n", __func__, edid->id_product);
};

void edp_extract_edid_version(struct edp_edid *edid, char *buf)
{
	edid->version = buf[0x12];
	edid->revision = buf[0x13];
	dprintf(SPEW, "%s: edid version = %d.%d", __func__,
					edid->version, edid->revision);
};

void edp_extract_edid_ext_block_cnt(struct edp_edid *edid, char *buf)
{
	edid->ext_block_cnt = buf[0x7e];
	dprintf(SPEW, "%s: edid extension = %d", __func__, edid->ext_block_cnt);
};

void edp_extract_edid_video_support(struct edp_edid *edid, char *buf)
{
	char *bp;

	bp = &buf[0x14];
	if (*bp & 0x80) {
		edid->video_intf = *bp & 0x0f;
		/* 6, 8, 10, 12, 14 and 16 bit per component */
		edid->color_depth = ((*bp & 0x70) >> 4); /* color bit depth */
		if (edid->color_depth) {
			edid->color_depth *= 2;
			edid->color_depth += 4;
		}
		dprintf(SPEW, "%s: Digital Video intf=%d color_depth=%d\n",
			  __func__, edid->video_intf, edid->color_depth);
		return;
	}
	dprintf(INFO, "%s: Error, Analog video interface", __func__);
};

void edp_extract_edid_feature(struct edp_edid *edid, char *buf)
{
	char *bp;
	char data;

	bp = &buf[0x18];
	data = *bp;
	data &= 0xe0;
	data >>= 5;
	if (data == 0x01)
		edid->dpm = 1; /* display power management */

	if (edid->video_intf) {
		if (*bp & 0x80) {
			/* RGB 4:4:4, YcrCb 4:4:4 and YCrCb 4:2:2 */
			edid->color_format = *bp & 0x18;
			edid->color_format >>= 3;
		}
	}

	dprintf(SPEW, "%s: edid dpm=%d color_format=%d",
			__func__, edid->dpm, edid->color_format);
};

void edp_extract_edid_detailed_timing_description(struct edp_edid *edid,
		char *buf)
{
	char *bp;
	int data;
	struct display_timing_desc *dp;

	dp = &edid->timing[0];

	bp = &buf[0x36];
	dp->pclk = 0;
	dp->pclk = *bp++; /* byte 0x36 */
	dp->pclk |= (*bp++ << 8); /* byte 0x37 */

	dp->h_addressable = *bp++; /* byte 0x38 */

	if (dp->pclk == 0 && dp->h_addressable == 0)
		return;	/* Not detailed timing definition */

	dp->pclk *= 10000;

	dp->h_blank = *bp++;/* byte 0x39 */
	data = *bp & 0xf0; /* byte 0x3A */
	data  <<= 4;
	dp->h_addressable |= data;

	data = *bp++ & 0x0f;
	data <<= 8;
	dp->h_blank |= data;

	dp->v_addressable = *bp++; /* byte 0x3B */
	dp->v_blank = *bp++; /* byte 0x3C */
	data = *bp & 0xf0; /* byte 0x3D */
	data  <<= 4;
	dp->v_addressable |= data;

	data = *bp++ & 0x0f;
	data <<= 8;
	dp->v_blank |= data;

	dp->h_fporch = *bp++; /* byte 0x3E */
	dp->h_sync_pulse = *bp++; /* byte 0x3F */

	dp->v_fporch = *bp & 0x0f0; /* byte 0x40 */
	dp->v_fporch  >>= 4;
	dp->v_sync_pulse = *bp & 0x0f;

	bp++;
	data = *bp & 0xc0; /* byte 0x41 */
	data <<= 2;
	dp->h_fporch |= data;

	data = *bp & 0x30;
	data <<= 4;
	dp->h_sync_pulse |= data;

	data = *bp & 0x0c;
	data <<= 2;
	dp->v_fporch |= data;

	data = *bp & 0x03;
	data <<= 4;
	dp->v_sync_pulse |= data;

	bp++;
	dp->width_mm = *bp++; /* byte 0x42 */
	dp->height_mm = *bp++; /* byte 0x43 */
	data = *bp & 0x0f0; /* byte 0x44 */
	data <<= 4;
	dp->width_mm |= data;
	data = *bp & 0x0f;
	data <<= 8;
	dp->height_mm |= data;

	bp++;
	dp->h_border = *bp++; /* byte 0x45 */
	dp->v_border = *bp++; /* byte 0x46 */

	dp->interlaced = *bp & 0x80; /* byte 0x47 */

	dp->stereo = *bp & 0x60;
	dp->stereo >>= 5;

	data = *bp & 0x1e; /* bit 4,3,2 1*/
	data >>= 1;
	dp->sync_type = data & 0x08;
	dp->sync_type >>= 3;	/* analog or digital */
	if (dp->sync_type) {
		dp->sync_separate = data & 0x04;
		dp->sync_separate >>= 2;
		if (dp->sync_separate) {
			if (data & 0x02)
				dp->vsync_pol = 1; /* positive */
			else
				dp->vsync_pol = 0;/* negative */

			if (data & 0x01)
				dp->hsync_pol = 1; /* positive */
			else
				dp->hsync_pol = 0; /* negative */
		}
	}

	dprintf(SPEW, "%s: pixel_clock = %d\n", __func__, dp->pclk);

	dprintf(SPEW, "%s: horizontal=%d, blank=%d, porch=%d, sync=%d\n",
			__func__, dp->h_addressable, dp->h_blank,
			dp->h_fporch, dp->h_sync_pulse);
	dprintf(SPEW, "%s: vertical=%d, blank=%d, porch=%d, vsync=%d\n",
			__func__, dp->v_addressable, dp->v_blank,
			dp->v_fporch, dp->v_sync_pulse);
	dprintf(SPEW, "%s: panel size in mm, width=%d height=%d\n",
			__func__, dp->width_mm, dp->height_mm);
	dprintf(SPEW, "%s: panel border horizontal=%d vertical=%d\n",
			__func__, dp->h_border, dp->v_border);
	dprintf(SPEW, "%s: interlaced=%d stereo=%d sync_type=%d sync_sep=%d\n",
			__func__, dp->interlaced, dp->stereo,
			dp->sync_type, dp->sync_separate);
	dprintf(SPEW, "%s: polarity vsync=%d, hsync=%d\n",
			__func__, dp->vsync_pol, dp->hsync_pol);
}


/*
 * EDID structure can be found in VESA standart here:
 * http://read.pudn.com/downloads110/ebook/456020/E-EDID%20Standard.pdf
 *
 * following table contains default edid
 * static char edid_raw_data[128] = {
 * 0, 255, 255, 255, 255, 255, 255, 0,
 * 6, 175, 93, 48, 0, 0, 0, 0, 0, 22,
 * 1, 4,
 * 149, 26, 14, 120, 2,
 * 164, 21,158, 85, 78, 155, 38, 15, 80, 84,
 * 0, 0, 0,
 * 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
 * 29, 54, 128, 160, 112, 56, 30, 64, 48, 32, 142, 0, 0, 144, 16,0,0,24,
 * 19, 36, 128, 160, 112, 56, 30, 64, 48, 32, 142, 0, 0, 144, 16,0,0,24,
 * 0, 0, 0, 254, 0, 65, 85, 79, 10, 32, 32, 32, 32, 32, 32, 32, 32, 32,
 * 0, 0, 0, 254, 0, 66, 49, 49, 54, 72, 65, 78, 48, 51, 46, 48, 32, 10,
 * 0, 75 };
 */

static int edp_aux_chan_ready(struct edp_aux_ctrl *ep)
{
	int cnt, ret;
	char data = 0;

	cnt = 5;
	while(cnt--) {
		ret = edp_aux_write_buf(ep, 0x50, &data, 1, 1);
		dprintf(SPEW, "%s: ret=%d\n", __func__, ret);
		if (ret >= 0)
			break;
		dprintf(INFO, "%s: failed in write\n", __func__);
		mdelay(100);
	}

	if (cnt == 0)
		return 0;

	return 1;
}

static int edp_sink_edid_read(struct edp_aux_ctrl *ep, int block)
{
	struct edp_buf *rp;
	int cnt, rlen;
	char data = 0;
	int ret = 0;

start:
	cnt = 5;
	dprintf(SPEW, "%s: cnt=%d\n", __func__, cnt);
	/* need to write a dummy byte before read edid */
	while(cnt--) {
		ret = edp_aux_write_buf(ep, 0x50, &data, 1, 1);
		if (ret >= 0)
			break;
		dprintf(INFO, "%s: failed in write\n", __func__);
		mdelay(100);
	}

	if (cnt == 0)
		return -1;

	rlen = edp_aux_read_buf(ep, 0x50, 128, 1);

	dprintf(SPEW, "%s: rlen=%d\n", rlen, __func__);

	if (rlen < 0)
		goto start;

	rp = &ep->rxp;
	if (edp_edid_buf_error(rp->data, rp->len))
		goto start;

	edp_extract_edid_manufacturer(&ep->edid, rp->data);
	edp_extract_edid_product(&ep->edid, rp->data);
	edp_extract_edid_version(&ep->edid, rp->data);
	edp_extract_edid_ext_block_cnt(&ep->edid, rp->data);
	edp_extract_edid_video_support(&ep->edid, rp->data);
	edp_extract_edid_feature(&ep->edid, rp->data);
	edp_extract_edid_detailed_timing_description(&ep->edid, rp->data);

	return 128;
}

/*
 * Converts from EDID struct to msm_panel_info
 */
void edp_edid2pinfo(struct msm_panel_info *pinfo)
{
	struct display_timing_desc *dp;

	dp = &edpctrl.edid.timing[0];

	pinfo->clk_rate = dp->pclk;

	dprintf(SPEW, "%s: pclk=%d\n", __func__, pinfo->clk_rate);

	pinfo->xres = dp->h_addressable + dp->h_border * 2;
	pinfo->yres = dp->v_addressable + dp->v_border * 2;

	pinfo->lcdc.h_back_porch = dp->h_blank - dp->h_fporch \
		- dp->h_sync_pulse;
	pinfo->lcdc.h_front_porch = dp->h_fporch;
	pinfo->lcdc.h_pulse_width = dp->h_sync_pulse;

	pinfo->lcdc.v_back_porch = dp->v_blank - dp->v_fporch \
		- dp->v_sync_pulse;
	pinfo->lcdc.v_front_porch = dp->v_fporch;
	pinfo->lcdc.v_pulse_width = dp->v_sync_pulse;

	pinfo->type = EDP_PANEL;
	pinfo->wait_cycle = 0;
	pinfo->bpp = 24;

	pinfo->lcdc.border_clr = 0;	 /* black */
	pinfo->lcdc.underflow_clr = 0xff; /* blue */
	pinfo->lcdc.hsync_skew = 0;
}

void edp_cap2pinfo(struct msm_panel_info *pinfo)
{
	struct dpcd_cap *cap;

	cap = &edpctrl.dpcd;

	pinfo->edp.max_lane_count = cap->max_lane_count;
	pinfo->edp.max_link_clk = cap->max_link_rate;

	dprintf(SPEW, "%s: clk=%d lane=%d\n", __func__,
	pinfo->edp.max_lane_count, pinfo->edp.max_link_clk);
}

static void edp_sink_capability_read(struct edp_aux_ctrl *ep,
				int len)
{
	char *bp;
	char data;
	struct dpcd_cap *cap;
	struct edp_buf *rp;
	int rlen;

	dprintf(SPEW, "%s:\n",__func__);

	rlen = edp_aux_read_buf(ep, 0, len, 0);
	if (rlen <= 0) {
		dprintf(INFO, "%s: edp aux read failed\n", __func__);
		return;
	}
	rp = &ep->rxp;
	cap = &ep->dpcd;
	bp = rp->data;

	data = *bp++; /* byte 0 */
	cap->major = (data >> 4) & 0x0f;
	cap->minor = data & 0x0f;
	if (--rlen <= 0)
		return;
	dprintf(SPEW, "%s: version: %d.%d\n", __func__,  cap->major, cap->minor);

	data = *bp++; /* byte 1 */
	/* 162, 270 and 540 MB, symbol rate, NOT bit rate */
	cap->max_link_rate = data * 27;
	if (--rlen <= 0)
		return;
	dprintf(SPEW, "%s: link_rate=%d\n",  __func__, cap->max_link_rate);

	data = *bp++; /* byte 2 */
	if (data & BIT(7))
		cap->flags |=  DPCD_ENHANCED_FRAME;
	if (data & 0x40)
		cap->flags |=  DPCD_TPS3;
	data &= 0x0f;
	cap->max_lane_count = data;
	if (--rlen <= 0)
		return;
	dprintf(SPEW, "%s: lane_count=%d\n", __func__,  cap->max_lane_count);

	data = *bp++; /* byte 3 */
	if (data & BIT(0)) {
		cap->flags |= DPCD_MAX_DOWNSPREAD_0_5;
		dprintf(SPEW, "%s: max_downspread\n", __func__);
	}

	if (data & BIT(6)) {
		cap->flags |= DPCD_NO_AUX_HANDSHAKE;
		dprintf(SPEW, "%s: NO Link Training\n", __func__);
	}
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 4 */
	cap->num_rx_port = (data & BIT(0)) + 1;
	dprintf(SPEW, "%s: rx_ports=%d", __func__,  cap->num_rx_port);
	if (--rlen <= 0)
		return;

	bp += 3;	/* skip 5, 6 and 7 */
	rlen -= 3;
	if (rlen <= 0)
		return;

	data = *bp++; /* byte 8 */
	if (data & BIT(1)) {
		cap->flags |= DPCD_PORT_0_EDID_PRESENTED;
		dprintf(SPEW, "%s: edid presented\n", __func__);
	}
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 9 */
	cap->rx_port0_buf_size = (data + 1) * 32;
	dprintf(SPEW, "%s: lane_buf_size=%d", __func__, cap->rx_port0_buf_size);
	if (--rlen <= 0)
		return;

	bp += 2; /* skip 10, 11 port1 capability */
	rlen -= 2;
	if (rlen <= 0)
		return;

	data = *bp++;	/* byte 12 */
	cap->i2c_speed_ctrl = data;
	if (cap->i2c_speed_ctrl > 0)
		dprintf(SPEW, "%s: i2c_rate=%d", __func__, cap->i2c_speed_ctrl);
	if (--rlen <= 0)
		return;

	data = *bp++;	/* byte 13 */
	cap->scrambler_reset = data & BIT(0);
	dprintf(SPEW, "%s: scrambler_reset=%d\n", __func__,
					cap->scrambler_reset);

	cap->enhanced_frame = data & BIT(1);
	dprintf(SPEW, "%s: enhanced_framing=%d\n", __func__,
					cap->enhanced_frame);
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 14 */
	if (data == 0)
		cap->training_read_interval = 100; /* us */
	else
		cap->training_read_interval = 4000 * data; /* us */
	dprintf(SPEW, "%s: training_interval=%d\n", __func__,
			 cap->training_read_interval);
}

static void edp_link_status_read(struct edp_aux_ctrl *ep, int len)
{
	char *bp;
	char data;
	struct dpcd_link_status *sp;
	struct edp_buf *rp;
	int rlen;


	/* skip byte 0x200 and 0x201 */
	rlen = edp_aux_read_buf(ep, 0x202, len, 0);
	dprintf(SPEW, "%s: rlen=%d\n", __func__, rlen);
	if (rlen <= 0) {
		dprintf(SPEW, "%s: edp aux read failed\n", __func__);
		return;
	}
	rp = &ep->rxp;
	bp = rp->data;
	sp = &ep->link_status;

	data = *bp++; /* byte 0x202 */
	sp->lane_01_status = data; /* lane 0, 1 */
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 0x203 */
	sp->lane_23_status = data; /* lane 2, 3 */
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 0x204 */
	sp->interlane_align_done = (data & BIT(0));
	sp->downstream_port_status_changed = (data & BIT(6));
	sp->link_status_updated = (data & BIT(7));
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 0x205 */
	sp->port_0_in_sync = (data & BIT(0));
	sp->port_1_in_sync = (data & BIT(1));
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 0x206 */
	sp->req_voltage_swing[0] = data & 0x03;
	data >>= 2;
	sp->req_pre_emphasis[0] = data & 0x03;
	data >>= 2;
	sp->req_voltage_swing[1] = data & 0x03;
	data >>= 2;
	sp->req_pre_emphasis[1] = data & 0x03;
	if (--rlen <= 0)
		return;

	data = *bp++; /* byte 0x207 */
	sp->req_voltage_swing[2] = data & 0x03;
	data >>= 2;
	sp->req_pre_emphasis[2] = data & 0x03;
	data >>= 2;
	sp->req_voltage_swing[3] = data & 0x03;
	data >>= 2;
	sp->req_pre_emphasis[3] = data & 0x03;

	bp = rp->data;
	dprintf(SPEW, "%s: %x %x %x %x %x %x\n", __func__, *bp,
	*(bp+1), *(bp+2), *(bp+3), *(bp+4), *(bp+5));

	dprintf(SPEW, "%s: align=%d v=%d p=%d\n", __func__,
	sp->interlane_align_done, sp->req_voltage_swing[0], sp->req_pre_emphasis[0]);
}


static int edp_cap_lane_rate_set(struct edp_aux_ctrl *ep)
{
	char buf[4];
	int len = 0;

	dprintf(SPEW, "%s: bw=%x lane=%d\n", __func__,
				ep->link_rate, ep->lane_cnt);
	buf[0] = ep->link_rate;
	buf[1] = ep->lane_cnt;
	len = edp_aux_write_buf(ep, 0x100, buf, 2, 0);

	return len;
}

static int edp_lane_set_write(struct edp_aux_ctrl *ep, int voltage_level,
		int pre_emphasis_level)
{
	int i;
	char buf[4];


	if (voltage_level >= DPCD_LINK_VOLTAGE_MAX)
		voltage_level |= 0x04;

	if (pre_emphasis_level >= DPCD_LINK_PRE_EMPHASIS_MAX)
		pre_emphasis_level |= 0x04;

	pre_emphasis_level <<= 3;

	for (i = 0; i < 4; i++)
		buf[i] = voltage_level | pre_emphasis_level;

	dprintf(SPEW, "%s: p|v=0x%x\n", __func__, voltage_level | pre_emphasis_level);
	return edp_aux_write_buf(ep, 0x103, buf, 4, 0);
}

static int edp_powerstate_write(struct edp_aux_ctrl *ep,
					char powerstate)
{
	return edp_aux_write_buf(ep, 0x600, &powerstate, 1, 0);
}

static int edp_train_pattern_set_write(struct edp_aux_ctrl *ep,
						int pattern)
{
	char buf[4];

	buf[0] = pattern;
	return edp_aux_write_buf(ep, 0x102, buf, 1, 0);
}

static int edp_sink_clock_recovery_done(struct edp_aux_ctrl *ep)
{
	int mask;
	int data;


	if (ep->lane_cnt == 1) {
		mask = 0x01;	/* lane 0 */
		data = ep->link_status.lane_01_status;
	} else if (ep->lane_cnt == 2) {
		mask = 0x011; /*B lane 0, 1 */
		data = ep->link_status.lane_01_status;
	} else {
		mask = 0x01111; /*B lane 0, 1 */
		data = ep->link_status.lane_23_status;
		data <<= 8;
		data |= ep->link_status.lane_01_status;
	}

	dprintf(SPEW, "%s: data=%x mask=%x\n", __func__, data, mask);
	data &= mask;
	if (data == mask) /* all done */
		return 1;

	return 0;
}

static int edp_sink_channel_eq_done(struct edp_aux_ctrl *ep)
{
	int mask;
	int data;


	if (!ep->link_status.interlane_align_done) /* not align */
		return 0;

	if (ep->lane_cnt == 1) {
		mask = 0x7;
		data = ep->link_status.lane_01_status;
	} else if (ep->lane_cnt == 2) {
		mask = 0x77;
		data = ep->link_status.lane_01_status;
	} else {
		mask = 0x7777;
		data = ep->link_status.lane_23_status;
		data <<= 8;
		data |= ep->link_status.lane_01_status;
	}

	dprintf(SPEW, "%s: data=%x mask=%x\n", __func__,  data, mask);

	data &= mask;
	if (data == mask)/* all done */
		return 1;

	return 0;
}

void edp_sink_train_set_adjust(struct edp_aux_ctrl *ep)
{
	int i;
	int max = 0;


	/* use the max level across lanes */
	for (i = 0; i < ep->lane_cnt; i++) {
		if (max < ep->link_status.req_voltage_swing[i])
			max = ep->link_status.req_voltage_swing[i];
	}

	ep->v_level = max;

	/* use the max level across lanes */
	max = 0;
	for (i = 0; i < ep->lane_cnt; i++) {
		if (max < ep->link_status.req_pre_emphasis[i])
			max = ep->link_status.req_pre_emphasis[i];
	}

	ep->p_level = max;
	dprintf(SPEW, "%s: v_level=%d, p_level=%d\n", __func__,
					ep->v_level, ep->p_level);
}

static void edp_host_train_set(struct edp_aux_ctrl *ep, int train)
{
	int bit, cnt;
	int data;


	bit = 1;
	bit  <<=  (train - 1);
	edp_write(EDP_BASE + EDP_STATE_CTRL, bit);

	bit = 8;
	bit <<= (train - 1);
	cnt = 10;
	while (cnt--) {
		data = edp_read(EDP_BASE + EDP_MAINLINK_READY);
		if (data & bit)
			break;
	}

	if (cnt == 0)
		dprintf(SPEW, "%s: set link_train=%d failed\n",  __func__, train);
}

char vm_pre_emphasis[4][4] = {
	{0x03, 0x06, 0x09, 0x0C},
	{0x03, 0x06, 0x09, 0xFF},
	{0x03, 0x06, 0xFF, 0xFF},
	{0x03, 0xFF, 0xFF, 0xFF}
};

char vm_voltage_swing[4][4] = {
	{0x64, 0x68, 0x6A, 0x6E},
	{0x68, 0x6A, 0x6E, 0xFF},
	{0x6A, 0x6E, 0xFF, 0xFF},
	{0x6E, 0xFF, 0xFF, 0xFF}
};

static void edp_voltage_pre_emphasise_set(struct edp_aux_ctrl *ep)
{
	int value0 = 0;
	int value1 = 0;

	dprintf(SPEW, "%s: v=%d p=%d\n", __func__, ep->v_level, ep->p_level);

	value0 = vm_pre_emphasis[(int)(ep->v_level)][(int)(ep->p_level)];
	value1 = vm_voltage_swing[(int)(ep->v_level)][(int)(ep->p_level)];

	/* Configure host and panel only if both values are allowed */
	if (value0 != 0xFF && value1 != 0xFF) {
		edp_write(EDP_BASE + EDP_PHY_EDPPHY_GLB_VM_CFG0, value0);
		edp_write(EDP_BASE + EDP_PHY_EDPPHY_GLB_VM_CFG1, value1);
		dprintf(SPEW, "%s: value0=0x%x value1=0x%x\n", __func__,
						value0, value1);
		edp_lane_set_write(ep, ep->v_level, ep->p_level);
	}

}

static int edp_start_link_train_1(struct edp_aux_ctrl *ep)
{
	int tries, old_v_level;
	int ret = 0;

	dprintf(SPEW, "%s:\n", __func__);

	edp_host_train_set(ep, 0x01); /* train_1 */
	edp_voltage_pre_emphasise_set(ep);
	edp_train_pattern_set_write(ep, 0x21); /* train_1 */

	tries = 0;
	old_v_level = ep->v_level;
	while (1) {
		udelay(ep->dpcd.training_read_interval * 10);

		edp_link_status_read(ep, 6);
		if (edp_sink_clock_recovery_done(ep)) {
			ret = 0;
			break;
		}

		if (ep->v_level == DPCD_LINK_VOLTAGE_MAX) {
			ret = -1;
			break;	/* quit */
		}

		if (old_v_level == ep->v_level) {
			tries++;
			if (tries >= 5) {
				ret = -1;
				break;	/* quit */
			}
		} else {
			tries = 0;
			old_v_level = ep->v_level;
		}

		edp_sink_train_set_adjust(ep);
		edp_voltage_pre_emphasise_set(ep);
	}

	return ret;
}

static int edp_start_link_train_2(struct edp_aux_ctrl *ep)
{
	int tries;
	int ret = 0;
	char pattern;

	dprintf(SPEW, "%s\n", __func__);

	if (ep->dpcd.flags & DPCD_TPS3)
		pattern = 0x03;
	else
		pattern = 0x02;

	edp_host_train_set(ep, pattern); /* train_2 */
	edp_voltage_pre_emphasise_set(ep);
	edp_train_pattern_set_write(ep, pattern | 0x20);/* train_2 */

	tries = 0;
	while (1) {
		udelay(ep->dpcd.training_read_interval);

		edp_link_status_read(ep, 6);

		if (edp_sink_channel_eq_done(ep)) {
			ret = 0;
			break;
		}

		tries++;
		if (tries > 5) {
			ret = -1;
			break;
		}

		edp_sink_train_set_adjust(ep);
		edp_voltage_pre_emphasise_set(ep);
	}

	return ret;
}

static int edp_link_rate_shift(struct edp_aux_ctrl *ep)
{
	/* add calculation later */
	return -1;
}

static void edp_clear_training_pattern(struct edp_aux_ctrl *ep)
{
	dprintf(SPEW, "%s:\n", __func__);
	edp_write(EDP_BASE + EDP_STATE_CTRL, 0);
	edp_train_pattern_set_write(ep, 0);
	udelay(ep->dpcd.training_read_interval);
}

static int edp_aux_link_train(struct edp_aux_ctrl *ep)
{
	int ret = 0;

	dprintf(SPEW, "%s:\n", __func__);
	ret = edp_aux_chan_ready(ep);
	if (ret == 0) {
		dprintf(INFO, "%s: Error: aux chan NOT ready\n",
							__func__);
		return ret;
	}

	/* start with max rate and lane */
	ep->lane_cnt = ep->dpcd.max_lane_count;
	ep->link_rate = ep->dpcd.max_link_rate;
	edp_write(EDP_BASE + EDP_MAINLINK_CTRL, 0x1);

train_start:
	ep->v_level = 0; /* start from default level */
	ep->p_level = 0;
	edp_cap_lane_rate_set(ep);

	edp_clear_training_pattern(ep);
	udelay(ep->dpcd.training_read_interval);
	edp_powerstate_write(ep, 1);

	ret = edp_start_link_train_1(ep);
	if (ret < 0) {
		if (edp_link_rate_shift(ep) == 0) {
			goto train_start;
		} else {
			dprintf(INFO, "%s: Training 1 failed\n", __func__);
			ret = -1;
			goto clear;
		}
	}

	dprintf(INFO, "%s: Training 1 completed successfully\n", __func__);

	edp_clear_training_pattern(ep);
	ret = edp_start_link_train_2(ep);
	if (ret < 0) {
		if (edp_link_rate_shift(ep) == 0) {
			goto train_start;
		} else {
			dprintf(INFO, "%s: Training 2 failed\n", __func__);
			ret = -1;
			goto clear;
		}
	}

	dprintf(INFO, "%s: Training 2 completed successfully\n", __func__);

clear:
	edp_clear_training_pattern(ep);

	return ret;
}

void mdss_edp_wait_for_hpd(void)
{
	while(1) {
		udelay(1000);
		edp_isr_poll();
		if (edp_hpd_done) {
			edp_hpd_done = 0;
			break;
		}
	}
}

void mdss_edp_wait_for_video_ready(void)
{
	while(1) {
		udelay(1000);
		edp_isr_poll();
		if (edp_video_ready) {
			edp_video_ready = 0;
			break;
		}
	}
}

void mdss_edp_dpcd_cap_read(void)
{
	edp_sink_capability_read(&edpctrl, 16);
}
void mdss_edp_pll_configure(void)
{
	struct display_timing_desc *dp;

	dp = &edpctrl.edid.timing[0];
	edp_pll_configure(dp->pclk);
}

void mdss_edp_lane_power_ctrl(int up)
{

	dprintf(SPEW, "%s: max_lane=%d\n", __func__, edpctrl.dpcd.max_lane_count);
	edp_lane_power_ctrl(edpctrl.dpcd.max_lane_count, up);

}

void mdss_edp_dpcd_status_read(void)
{
	edp_link_status_read(&edpctrl, 6);
}

void mdss_edp_edid_read(void)
{
	edp_sink_edid_read(&edpctrl, 0);
}

int mdss_edp_link_train(void)
{
	return edp_aux_link_train(&edpctrl);
}

void mdss_edp_aux_init(void)
{
	edp_buf_init(&edpctrl.txp, edpctrl.txbuf, sizeof(edpctrl.txbuf));
	edp_buf_init(&edpctrl.rxp, edpctrl.rxbuf, sizeof(edpctrl.rxbuf));
}
