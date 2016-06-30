/* Copyright (c) 2015-2106, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of The Linux Foundation nor
 *     the names of its contributors may be used to endorse or promote
 *     products derived from this software without specific prior written
 *     permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <debug.h>
#include <mdp5.h>
#include <reg.h>
#include <bits.h>
#include <msm_panel.h>
#include <panel.h>
#include <platform/iomap.h>
#include <mipi_dsi.h>

/*
 * rc_buf_thresh = {896, 1792, 2688, 3548, 4480, 5376, 6272, 6720,
 *		7168, 7616, 7744, 7872, 8000, 8064, 8192};
 *	(x >> 6) & 0x0ff)
 */
static char dsc_rc_buf_thresh[] = {0x0e, 0x1c, 0x2a, 0x38, 0x46, 0x54,
		0x62, 0x69, 0x70, 0x77, 0x79, 0x7b, 0x7d, 0x7e};
static char dsc_rc_range_min_qp_1_1[] = {0, 0, 1, 1, 3, 3, 3, 3, 3, 3, 5,
				5, 5, 7, 13};
static char dsc_rc_range_min_qp_1_1_scr1[] = {0, 0, 1, 1, 3, 3, 3, 3, 3, 3, 5,
				5, 5, 9, 12};
static char dsc_rc_range_max_qp_1_1[] = {4, 4, 5, 6, 7, 7, 7, 8, 9, 10, 11,
			 12, 13, 13, 15};
static char dsc_rc_range_max_qp_1_1_scr1[] = {4, 4, 5, 6, 7, 7, 7, 8, 9, 10, 10,
			 11, 11, 12, 13};
static char dsc_rc_range_bpg_offset[] = {2, 0, 0, -2, -4, -6, -8, -8,
			-8, -10, -10, -12, -12, -12, -12};

#define CEIL(x, y)              (((x) + ((y)-1)) / (y))

int mdss_dsc_to_buf(struct msm_panel_info *pinfo)
{
	struct dsc_desc *dsc;
	char *bp;
	char data;
	int i, bpp;

	dsc = &pinfo->dsc;
	bp = dsc->pps_buf;

	*bp++ = 128;	/* pps length */
	*bp++ = 0;
	*bp++ = 0x0a;	/* PPS data type */
	*bp++ = 0xc0;	/* last + long pkt */

	/* pps payload */
	*bp++ = (((dsc->major & 0xf) << 4) | (dsc->minor & 0xf));	/* pps0 */
	*bp++ = (dsc->pps_id & 0xff);		/* pps1 */
	bp++;					/* pps2, reserved */

	data = dsc->line_buf_depth & 0x0f;
	data |= ((dsc->bpc & 0xf) << 4);
	*bp++ = data;				 /* pps3 */

	bpp = dsc->bpp;
	bpp <<= 4;	/* 4 fraction bits */
	data = (bpp >> 8);
	data &= 0x03;		/* upper two bits */
	data |= ((dsc->block_pred_enable & 0x1) << 5);
	data |= ((dsc->convert_rgb & 0x1) << 4);
	data |= ((dsc->enable_422 & 0x1) << 3);
	data |= ((dsc->vbr_enable & 0x1) << 2);
	*bp++ = data;				/* pps4 */
	*bp++ = bpp;				/* pps5 */

	*bp++ = ((dsc->pic_height >> 8) & 0xff); /* pps6 */
	*bp++ = (dsc->pic_height & 0x0ff);	/* pps7 */
	*bp++ = ((dsc->pic_width >> 8) & 0xff);	/* pps8 */
	*bp++ = (dsc->pic_width & 0x0ff);	/* pps9 */

	*bp++ = ((dsc->slice_height >> 8) & 0xff);/* pps10 */
	*bp++ = (dsc->slice_height & 0x0ff);	/* pps11 */
	*bp++ = ((dsc->slice_width >> 8) & 0xff); /* pps12 */
	*bp++ = (dsc->slice_width & 0x0ff);	/* pps13 */

	*bp++ = ((dsc->chunk_size >> 8) & 0xff);/* pps14 */
	*bp++ = (dsc->chunk_size & 0x0ff);	/* pps15 */

	*bp++ = (dsc->initial_xmit_delay >> 8) & 0x3; /* pps16, bit 0, 1 */
	*bp++ = (dsc->initial_xmit_delay & 0xff);/* pps17 */

	*bp++ = ((dsc->initial_dec_delay >> 8) & 0xff);	/* pps18 */
	*bp++ = (dsc->initial_dec_delay & 0xff);/* pps19 */

	bp++;					/* pps20, reserved */

	*bp++ = (dsc->initial_scale_value & 0x3f); /* pps21 */

	*bp++ = ((dsc->scale_increment_interval >> 8) & 0xff); /* pps22 */
	*bp++ = (dsc->scale_increment_interval & 0xff);	/* pps23 */

	*bp++ = ((dsc->scale_decrement_interval >> 8) & 0xf); /* pps24 */
	*bp++ = (dsc->scale_decrement_interval & 0x0ff);/* pps25 */

	bp++;			/* pps26, reserved */

	*bp++ = (dsc->first_line_bpg_offset & 0x1f);/* pps27 */

	*bp++ = ((dsc->nfl_bpg_offset >> 8) & 0xff);/* pps28 */
	*bp++ = (dsc->nfl_bpg_offset & 0x0ff);	/* pps29 */
	*bp++ = ((dsc->slice_bpg_offset >> 8) & 0xff);/* pps30 */
	*bp++ = (dsc->slice_bpg_offset & 0x0ff);/* pps31 */

	*bp++ = ((dsc->initial_offset >> 8) & 0xff);/* pps32 */
	*bp++ = (dsc->initial_offset & 0x0ff);	/* pps33 */

	*bp++ = ((dsc->final_offset >> 8) & 0xff);/* pps34 */
	*bp++ = (dsc->final_offset & 0x0ff);	/* pps35 */

	*bp++ = (dsc->min_qp_flatness & 0x1f);	/* pps36 */
	*bp++ = (dsc->max_qp_flatness & 0x1f);	/* pps37 */

	*bp++ = ((dsc->rc_model_size >> 8) & 0xff);/* pps38 */
	*bp++ = (dsc->rc_model_size & 0x0ff);	/* pps39 */

	*bp++ = (dsc->edge_factor & 0x0f);	/* pps40 */

	*bp++ = (dsc->quant_incr_limit0 & 0x1f);	/* pps41 */
	*bp++ = (dsc->quant_incr_limit1 & 0x1f);	/* pps42 */

	data = ((dsc->tgt_offset_hi & 0xf) << 4);
	data |= (dsc->tgt_offset_lo & 0x0f);
	*bp++ = data;				/* pps43 */

	for (i = 0; i < 14; i++)
		*bp++ = (dsc->buf_thresh[i] & 0xff);/* pps44 - pps57 */

	for (i = 0; i < 15; i++) {		/* pps58 - pps87 */
		data = (dsc->range_min_qp[i] & 0x1f); /* 5 bits */
		data <<= 3;
		data |= ((dsc->range_max_qp[i] >> 2) & 0x07); /* 3 bits */
		*bp++ = data;
		data = (dsc->range_max_qp[i] & 0x03); /* 2 bits */
		data <<= 6;
		data |= (dsc->range_bpg_offset[i] & 0x3f); /* 6 bits */
		*bp++ = data;
	}

	/* pps88 to pps127 are reserved */

	return 128;
}

static int mdss_dsc_initial_line_calc(int bpc, int xmit_delay,
			int slice_width, int slice_per_line)
{
	int ssm_delay;
	int total_pixels;

	ssm_delay = ((bpc < 10) ? 83 : 91);
	total_pixels = ssm_delay * 3 + 30 + xmit_delay + 6;
	total_pixels += ((slice_per_line > 1) ? (ssm_delay * 3) : 0);

	return CEIL(total_pixels, slice_width);
}

void mdss_dsc_parameters_calc(struct msm_panel_info *pinfo)
{
	struct dsc_desc *dsc;
	int bpp, bpc;
	int mux_words_size;
	int groups_per_line, groups_total;
	int min_rate_buffer_size;
	int hrd_delay;
	int pre_num_extra_mux_bits, num_extra_mux_bits;
	int slice_bits;
	int target_bpp_x16;
	int data;
	int final_value, final_scale;
	int slice_per_line, bytes_in_slice, total_bytes;
	int version;

	dsc = &pinfo->dsc;
	version = (((dsc->major & 0xf) << 4) | (dsc->minor & 0xf));

	dsc->rc_model_size = 8192;	/* rate_buffer_size */
	if (version == 0x11 && dsc->scr_rev == 0x1)
		dsc->first_line_bpg_offset = 15;
	else
		dsc->first_line_bpg_offset = 12;
	dsc->min_qp_flatness = 3;
	dsc->max_qp_flatness = 12;
	dsc->line_buf_depth = 9;

	dsc->edge_factor = 6;
	dsc->quant_incr_limit0 = 11;
	dsc->quant_incr_limit1 = 11;
	dsc->tgt_offset_hi = 3;
	dsc->tgt_offset_lo = 3;

	dsc->buf_thresh = dsc_rc_buf_thresh;
	if (version == 0x11 && dsc->scr_rev == 0x1) {
		dsc->range_min_qp = dsc_rc_range_min_qp_1_1_scr1;
		dsc->range_max_qp = dsc_rc_range_max_qp_1_1_scr1;
	} else {
		dsc->range_min_qp = dsc_rc_range_min_qp_1_1;
		dsc->range_max_qp = dsc_rc_range_max_qp_1_1;
	}
	dsc->range_bpg_offset = dsc_rc_range_bpg_offset;

	if (pinfo->mipi.dual_dsi)
		dsc->pic_width = pinfo->xres / 2;
	else
		dsc->pic_width = pinfo->xres;
	dsc->pic_height = pinfo->yres;

	bpp = dsc->bpp;
	bpc = dsc->bpc;

	if (bpp == 8)
		dsc->initial_offset = 6144;
	else
		dsc->initial_offset = 2048;	/* bpp = 12 */

	if (bpc == 8)
		mux_words_size = 48;
	else
		mux_words_size = 64;	/* bpc == 12 */

	slice_per_line = CEIL(dsc->pic_width, dsc->slice_width);

	dsc->pkt_per_line = slice_per_line / dsc->slice_per_pkt;
	if (slice_per_line % dsc->slice_per_pkt)
		dsc->pkt_per_line = 1;		/* default*/

	bytes_in_slice = CEIL(dsc->pic_width, slice_per_line);

	bytes_in_slice *= dsc->bpp;	/* bites per compressed pixel */
	bytes_in_slice = CEIL(bytes_in_slice, 8);

	dsc->bytes_in_slice = bytes_in_slice;

	dprintf(SPEW, "%s: slice_per_line=%d pkt_per_line=%d bytes_in_slice=%d\n",
		__func__, slice_per_line, dsc->pkt_per_line, bytes_in_slice);

	total_bytes = bytes_in_slice * slice_per_line;
	dsc->eol_byte_num = total_bytes % 3;
	dsc->pclk_per_line =  CEIL(total_bytes, 3);

	dsc->slice_last_group_size = 3 - dsc->eol_byte_num;

	dprintf(SPEW, "%s: pclk_per_line=%d total_bytes=%d eol_byte_num=%d\n",
		__func__, dsc->pclk_per_line, total_bytes, dsc->eol_byte_num);

	dsc->bytes_per_pkt = bytes_in_slice * dsc->slice_per_pkt;

	dsc->det_thresh_flatness = 7 + (bpc - 8);

	dsc->initial_xmit_delay = dsc->rc_model_size / (2 * bpp);

	dsc->initial_lines = mdss_dsc_initial_line_calc(bpc,
		dsc->initial_xmit_delay, dsc->slice_width, slice_per_line);

	groups_per_line = CEIL(dsc->slice_width, 3);

	dsc->chunk_size = dsc->slice_width * bpp / 8;
	if ((dsc->slice_width * bpp) % 8)
		dsc->chunk_size++;


	/* rbs-min */
	min_rate_buffer_size =  dsc->rc_model_size - dsc->initial_offset +
			dsc->initial_xmit_delay * bpp +
			groups_per_line * dsc->first_line_bpg_offset;

	hrd_delay = CEIL(min_rate_buffer_size, bpp);

	dsc->initial_dec_delay = hrd_delay - dsc->initial_xmit_delay;

	dsc->initial_scale_value = 8 * dsc->rc_model_size /
			(dsc->rc_model_size - dsc->initial_offset);

	slice_bits = 8 * dsc->chunk_size * dsc->slice_height;

	groups_total = groups_per_line * dsc->slice_height;

	data = dsc->first_line_bpg_offset * 2048;

	dsc->nfl_bpg_offset = CEIL(data, (dsc->slice_height - 1));

	pre_num_extra_mux_bits = 3 * (mux_words_size + (4 * bpc + 4) - 2);

	num_extra_mux_bits = pre_num_extra_mux_bits - (mux_words_size -
		((slice_bits - pre_num_extra_mux_bits) % mux_words_size));

	data = 2048 * (dsc->rc_model_size - dsc->initial_offset
		+ num_extra_mux_bits);
	dsc->slice_bpg_offset = CEIL(data, groups_total);

	/* bpp * 16 + 0.5 */
	data = bpp * 16;
	data *= 2;
	data++;
	data /= 2;
	target_bpp_x16 = data;

	data = (dsc->initial_xmit_delay * target_bpp_x16) / 16;
	final_value =  dsc->rc_model_size - data + num_extra_mux_bits;

	final_scale = 8 * dsc->rc_model_size /
		(dsc->rc_model_size - final_value);

	dsc->final_offset = final_value;

	data = (final_scale - 9) * (dsc->nfl_bpg_offset +
		dsc->slice_bpg_offset);
	dsc->scale_increment_interval = (2048 * dsc->final_offset) / data;

	dsc->scale_decrement_interval = groups_per_line /
		(dsc->initial_scale_value - 8);

	dprintf(SPEW, "%s: initial_xmit_delay=%d\n", __func__,
		dsc->initial_xmit_delay);

	dprintf(SPEW, "%s: bpg_offset, nfl=%d slice=%d\n", __func__,
		dsc->nfl_bpg_offset, dsc->slice_bpg_offset);

	dprintf(SPEW, "%s: groups_per_line=%d chunk_size=%d\n", __func__,
		groups_per_line, dsc->chunk_size);
	dprintf(SPEW, "%s:min_rate_buffer_size=%d hrd_delay=%d\n", __func__,
		min_rate_buffer_size, hrd_delay);
	dprintf(SPEW, "%s:initial_dec_delay=%d initial_scale_value=%d\n", __func__,
		dsc->initial_dec_delay, dsc->initial_scale_value);
	dprintf(SPEW, "%s:slice_bits=%d, groups_total=%d\n", __func__,
		slice_bits, groups_total);
	dprintf(SPEW, "%s: first_line_bgp_offset=%d slice_height=%d\n", __func__,
		dsc->first_line_bpg_offset, dsc->slice_height);
	dprintf(SPEW, "%s:final_value=%d final_scale=%d\n", __func__,
		final_value, final_scale);
	dprintf(SPEW, "%s: sacle_increment_interval=%d scale_decrement_interval=%d\n",
		__func__, dsc->scale_increment_interval,
		dsc->scale_decrement_interval);
}

void mdss_dsc_mdp_config(struct msm_panel_info *pinfo,
	unsigned int pp_base, unsigned int dsc_base,
	bool mux, bool split_mode)
{
	unsigned int data;
	unsigned int offset, off;
	struct dsc_desc *dsc;
	char *lp;
        char *cp;
        int i, bpp, lsb;

	/* dce0_sel->pp0, dce1_sel->pp1 */
	writel(0x0, MDSS_MDP_REG_DCE_SEL);

	/* dsc enable */
	writel(1, pp_base + MDSS_MDP_PP_DSC_MODE);

	data = readl(pp_base + MDSS_MDP_PP_DCE_DATA_OUT_SWAP);
	data |= BIT(18);	/* endian flip */
	writel(data, pp_base + MDSS_MDP_PP_DCE_DATA_OUT_SWAP);

	offset = dsc_base;

	data = 0;
	dsc = &pinfo->dsc;
	if (pinfo->type == MIPI_VIDEO_PANEL)
		data = BIT(2);	/* video mode */

	if (split_mode)
		data |= BIT(0);
	if (mux)
		data |= BIT(1);

	writel(data, offset + MDSS_MDP_DSC_COMMON_MODE);

	data = (dsc->initial_lines << 20);
	data |= ((dsc->slice_last_group_size - 1) << 18);

	/* bpp is 6.4 format, 4 LSBs bits are for fractional part */
	lsb = dsc->bpp % 4;
	bpp = dsc->bpp / 4;
	bpp *= 4;	/* either 8 or 12 */
	bpp <<= 4;
	bpp |= lsb;
	data |= (bpp << 8);
	data |= (dsc->block_pred_enable << 7);
	data |= (dsc->line_buf_depth << 3);
	data |= (dsc->enable_422 << 2);
	data |= (dsc->convert_rgb << 1);
	data |= dsc->input_10_bits;

	dprintf(SPEW, "%s: %d %d %d %d %d %d %d %d, data=%x\n",
		__func__,
		dsc->initial_lines , dsc->slice_last_group_size,
		dsc->bpp, dsc->block_pred_enable, dsc->line_buf_depth,
		dsc->enable_422, dsc->convert_rgb, dsc->input_10_bits, data);

	writel(data, offset + MDSS_MDP_DSC_ENC);

	data = dsc->pic_width << 16;
	data |= dsc->pic_height;
	writel(data, offset + MDSS_MDP_DSC_PICTURE);

	data = dsc->slice_width << 16;
	data |= dsc->slice_height;
	writel(data, offset + MDSS_MDP_DSC_SLICE);

	data = dsc->chunk_size << 16;
	writel(data, offset + MDSS_MDP_DSC_CHUNK_SIZE);

	dprintf(SPEW, "%s: pic_w=%d pic_h=%d, slice_h=%d slice_w=%d, chunk=%d\n",
		__func__, dsc->pic_width, dsc->pic_height,
		dsc->slice_width, dsc->slice_height, dsc->chunk_size);

	data = dsc->initial_dec_delay << 16;
	data |= dsc->initial_xmit_delay;
	writel(data, offset + MDSS_MDP_DSC_DELAY);

	data = dsc->initial_scale_value;
	writel(data, offset + MDSS_MDP_DSC_SCALE_INITIAL);

	data = dsc->scale_decrement_interval;
	writel(data, offset + MDSS_MDP_DSC_SCALE_DEC_INTERVAL);

	data = dsc->scale_increment_interval;
	writel(data, offset + MDSS_MDP_DSC_SCALE_INC_INTERVAL);

	data = dsc->first_line_bpg_offset;
	writel(data, offset + MDSS_MDP_DSC_FIRST_LINE_BPG_OFFSET);

	data = dsc->nfl_bpg_offset << 16;
	data |= dsc->slice_bpg_offset;
	writel(data, offset + MDSS_MDP_DSC_BPG_OFFSET);

	data = dsc->initial_offset << 16;
	data |= dsc->final_offset;
	writel(data, offset + MDSS_MDP_DSC_DSC_OFFSET);

	data = dsc->det_thresh_flatness << 10;
	data |= dsc->max_qp_flatness << 5;
	data |= dsc->min_qp_flatness;
	writel(data, offset + MDSS_MDP_DSC_FLATNESS);

	data = dsc->rc_model_size;	/* rate_buffer_size */
	writel(data, offset + MDSS_MDP_DSC_RC_MODEL_SIZE);

	data = dsc->tgt_offset_lo << 18;
	data |= dsc->tgt_offset_hi << 14;
	data |= dsc->quant_incr_limit1 << 9;
	data |= dsc->quant_incr_limit0 << 4;
	data |= dsc->edge_factor;
	writel(data, offset + MDSS_MDP_DSC_RC);

	lp = dsc->buf_thresh;
	off = offset + MDSS_MDP_DSC_RC_BUF_THRESH;
	for (i = 0; i < 14; i++) {
		writel(*lp++, off);
		off += 4;
	}

	cp = dsc->range_min_qp;
	off = offset + MDSS_MDP_DSC_RANGE_MIN_QP;
	for (i = 0; i < 15; i++) {
		writel(*cp++, off);
		off += 4;
	}

	cp = dsc->range_max_qp;
	off = offset + MDSS_MDP_DSC_RANGE_MAX_QP;
	for (i = 0; i < 15; i++) {
		writel(*cp++, off);
		off += 4;
	}

	cp = dsc->range_bpg_offset;
	off = offset + MDSS_MDP_DSC_RANGE_BPG_OFFSET;
	for (i = 0; i < 15; i++) {
		writel(*cp++, off);
		off += 4;
	}
}

void mdss_dsc_dsi_config(uint32_t ctl_base, int mode,
                                struct dsc_desc *dsc)
{
        unsigned int data;

	if (mode == DSI_VIDEO_MODE) {
		writel(0, ctl_base + VIDEO_COMPRESSION_MODE_CTRL_2);
                data = dsc->bytes_per_pkt << 16;
                data |= (0x0b << 8);    /*  dtype of compressed image */
                data |= (dsc->pkt_per_line - 1) << 6;
                data |= dsc->eol_byte_num << 4;
                data |= 1;      /* enable */

		writel(data, ctl_base + VIDEO_COMPRESSION_MODE_CTRL);
        } else {
                /* strem 0 */
		writel(0, ctl_base + CMD_COMPRESSION_MODE_CTRL_3);

		data = dsc->bytes_in_slice;
		writel(data, ctl_base + CMD_COMPRESSION_MODE_CTRL_2);

                data = 0x39 << 8;
                data |= (dsc->pkt_per_line - 1) << 6;
                data |= dsc->eol_byte_num << 4;
                data |= 1;      /* enable */
		writel(data, ctl_base + CMD_COMPRESSION_MODE_CTRL);
        }
}
