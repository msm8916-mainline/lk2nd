/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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

#ifndef MDSS_QPIC_H
#define MDSS_QPIC_H

#include <msm_panel.h>
#include "qpic_panel.h"

#define QPIC_REG_QPIC_LCDC_CTRL				0x22000
#define QPIC_REG_LCDC_VERSION				0x22004
#define QPIC_REG_QPIC_LCDC_IRQ_EN			0x22008
#define QPIC_REG_QPIC_LCDC_IRQ_STTS			0x2200C
#define QPIC_REG_QPIC_LCDC_IRQ_CLR			0x22010
#define QPIC_REG_QPIC_LCDC_STTS				0x22014
#define QPIC_REG_QPIC_LCDC_CMD_DATA_CYCLE_CNT	0x22018
#define QPIC_REG_QPIC_LCDC_CFG0				0x22020
#define QPIC_REG_QPIC_LCDC_CFG1				0x22024
#define QPIC_REG_QPIC_LCDC_CFG2				0x22028
#define QPIC_REG_QPIC_LCDC_RESET			0x2202C
#define QPIC_REG_QPIC_LCDC_FIFO_SOF			0x22100
#define QPIC_REG_LCD_DEVICE_CMD0			0x23000
#define QPIC_REG_QPIC_LCDC_FIFO_DATA_PORT0	0x22140
#define QPIC_REG_QPIC_LCDC_FIFO_EOF			0x22180

#define QPIC_OUTP(off, data) \
	writel((data), qpic_res->qpic_base + (off))
#define QPIC_OUTPW(off, data) \
	writehw((data), qpic_res->qpic_base + (off))
#define QPIC_INP(off) \
	readl(qpic_res->qpic_base + (off))

#define QPIC_MAX_VSYNC_WAIT_TIME			500
#define QPIC_MAX_WAIT_CNT			1000
#define QPIC_MAX_CMD_BUF_SIZE				512

int mdss_qpic_init(void);
int qpic_send_pkt(uint32_t cmd, uint8_t *param, uint32_t len);
uint32_t qpic_read_data(uint32_t cmd_index, uint32_t size);
int mdss_qpic_panel_on(struct qpic_panel_io_desc *panel_io);
int mdss_qpic_panel_off(struct qpic_panel_io_desc *panel_io);
void qpic_init(struct msm_panel_info *pinfo, int base);
int qpic_on(void);
int qpic_off(void);
void qpic_update(void);

struct qpic_data_type {
	uint32_t rev;
	size_t qpic_reg_size;
	uint32_t qpic_phys;
	uint32_t qpic_base;
	uint32_t irq;
	uint32_t irq_ena;
	uint32_t res_init;
	void *fb_virt;
	uint32_t fb_phys;
	void *cmd_buf_virt;
	uint32_t cmd_buf_phys;
	int qpic_endpt;
	uint32_t sps_init;
	uint32_t irq_requested;
	struct qpic_panel_io_desc panel_io;
	uint32_t bus_handle;
	int fifo_eof_comp;
	int fb_xres;
	int fb_yres;
	int fb_bpp;
	int base;
};

uint32_t qpic_send_frame(
		uint32_t x_start,
		uint32_t y_start,
		uint32_t x_end,
		uint32_t y_end,
		uint32_t *data,
		uint32_t total_bytes);

#endif /* MDSS_QPIC_H */
