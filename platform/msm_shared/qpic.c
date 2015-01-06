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

#include <debug.h>
#include <err.h>
#include <msm_panel.h>
#include <platform/iomap.h>
#include <platform/timer.h>
#include <reg.h>
#include <target.h>

#include "qpic.h"
#include "qpic_panel.h"

int mdss_qpic_panel_init(struct qpic_panel_io_desc *panel_io);

struct qpic_data_type qpic_data;
struct qpic_data_type *qpic_res = &qpic_data;
static int qpic_send_pkt_sw(uint32_t cmd, uint32_t len, uint8_t *param);

/* for debugging */
static uint32_t use_bam = false;
static uint32_t use_vsync;

/* For compilation */
void mdp_set_revision(int rev)
{
    return;
}

int qpic_on(void)
{
	int ret;
	ret = mdss_qpic_panel_on(&qpic_res->panel_io);
	return ret;
}

int qpic_off(void)
{
	int ret = NO_ERROR;

	if (!target_cont_splash_screen()) {
		ret = mdss_qpic_panel_off(&qpic_res->panel_io);
	}

	return ret;
}

void qpic_update()
{
	uint32_t fb_offset, size;

	if (use_bam)
		fb_offset = qpic_res->fb_phys + (uint32_t) qpic_res->base;
	else
		fb_offset = (uint32_t) qpic_res->fb_virt + (uint32_t) qpic_res->base;

	size = qpic_res->fb_xres * qpic_res->fb_yres * qpic_res->fb_bpp;

	qpic_send_frame(0, 0, qpic_res->fb_xres - 1, qpic_res->fb_yres - 1,
		(uint32_t *)fb_offset, size);
}

int mdss_qpic_alloc_fb_mem(struct msm_panel_info *pinfo, int base)
{
	qpic_res->fb_virt = 0;
	qpic_res->fb_phys = 0;
	qpic_res->fb_xres = pinfo->xres;
	qpic_res->fb_yres = pinfo->yres;
	qpic_res->fb_bpp = pinfo->bpp / 8;
	qpic_res->base = base;

	return 0;
}

void qpic_init(struct msm_panel_info *pinfo, int base)
{
	if (qpic_res->res_init)
		return;

	qpic_res->qpic_base = QPIC_BASE;
	mdss_qpic_panel_init(&qpic_res->panel_io);
	mdss_qpic_alloc_fb_mem(pinfo, base);
	qpic_res->res_init = true;
}

int qpic_init_sps(void)
{
	return 0;
}

void mdss_qpic_reset(void)
{
	uint32_t cnt = 0;

	QPIC_OUTP(QPIC_REG_QPIC_LCDC_RESET, 1 << 0);
	/* wait 100 us after reset as suggested by hw */
	udelay(100);
	while (((QPIC_INP(QPIC_REG_QPIC_LCDC_STTS) & (1 << 8)) == 0)) {
		if (cnt > QPIC_MAX_WAIT_CNT) {
			dprintf(CRITICAL, "%s reset not finished\n", __func__);
			break;
		}
		/* yield 100 us for next polling by experiment*/
		udelay(100);
		cnt++;
	}
}

static int qpic_send_pkt_bam(uint32_t cmd, uint32_t len, uint8_t *param)
{
	return qpic_send_pkt_sw(cmd, len, param);
}

static void qpic_dump_reg(void)
{
	dprintf(INFO, "%s\n", __func__);
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_CTRL = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_CTRL));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_CMD_DATA_CYCLE_CNT = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_CMD_DATA_CYCLE_CNT));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_CFG0 = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_CFG0));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_CFG1 = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_CFG1));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_CFG2 = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_CFG2));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_IRQ_EN = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_IRQ_EN));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_IRQ_STTS = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_IRQ_STTS));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_STTS = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_STTS));
	dprintf(INFO, "QPIC_REG_QPIC_LCDC_FIFO_SOF = %x\n",
		QPIC_INP(QPIC_REG_QPIC_LCDC_FIFO_SOF));
}

static int qpic_wait_for_fifo(void)
{
	uint32_t data, cnt = 0;
	int ret = 0;

	while (1) {
		data = QPIC_INP(QPIC_REG_QPIC_LCDC_STTS);
		data &= 0x3F;
		if (data == 0)
			break;
		/* yield 10 us for next polling by experiment*/
		udelay(10);
		if (cnt > (QPIC_MAX_WAIT_CNT * 10)) {
			dprintf(CRITICAL, "%s time out\n", __func__);
			ret = -1;
			break;
		}
		cnt++;
	}
	return ret;
}

static int qpic_wait_for_eof(void)
{
	uint32_t data, cnt = 0;
	int ret = 0;

	while (1) {
		data = QPIC_INP(QPIC_REG_QPIC_LCDC_IRQ_STTS);
		if (data & (1 << 2))
			break;
		/* yield 10 us for next polling by experiment*/
		udelay(10);
		if (cnt > (QPIC_MAX_WAIT_CNT * 10)) {
			dprintf(CRITICAL, "%s wait for eof time out\n", __func__);
			qpic_dump_reg();
			ret = -1;
			break;
		}
		cnt++;
	}
	return ret;
}

static int qpic_send_pkt_sw(uint32_t cmd, uint32_t len, uint8_t *param)
{
	uint32_t bytes_left, space, data, cfg2;
	int ret = 0;
	uint32_t i;

	if (len && !param) {
		dprintf(CRITICAL, "Null Pointer!\n");
		return 0;
	}
	if (len <= 4) {
		len = (len + 3) / 4; /* len in dwords */
		data = 0;
		for (i = 0; i < len; i++)
			data |= (uint32_t)param[i] << (8 * i);
		QPIC_OUTP(QPIC_REG_QPIC_LCDC_CMD_DATA_CYCLE_CNT, len);
		QPIC_OUTP(QPIC_REG_LCD_DEVICE_CMD0 + (4 * cmd), data);
		return 0;
	}

	if ((len & 0x1) != 0) {
		dprintf(INFO, "%s: number of bytes needs be even\n", __func__);
		len = (len + 1) & (~0x1);
	}
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_IRQ_CLR, 0xff);
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_CMD_DATA_CYCLE_CNT, 0);
	cfg2 = QPIC_INP(QPIC_REG_QPIC_LCDC_CFG2);
	if ((cmd != OP_WRITE_MEMORY_START) &&
		(cmd != OP_WRITE_MEMORY_CONTINUE))
		cfg2 |= (1 << 24); /* transparent mode */
	else
		cfg2 &= ~(1 << 24);

	cfg2 &= ~0xFF;
	cfg2 |= cmd;
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_CFG2, cfg2);
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_FIFO_SOF, 0x0);
	bytes_left = len;

	while (bytes_left > 0) {
		ret = qpic_wait_for_fifo();
		if (ret)
			goto exit_send_cmd_sw;

		space = 16;

		while ((space > 0) && (bytes_left > 0)) {
			/* write to fifo */
			if (bytes_left >= 4) {
				QPIC_OUTP(QPIC_REG_QPIC_LCDC_FIFO_DATA_PORT0,
					*(uint32_t *)param);
				param += 4;
				bytes_left -= 4;
				space--;
			} else if (bytes_left == 2) {
				QPIC_OUTPW(QPIC_REG_QPIC_LCDC_FIFO_DATA_PORT0,
					*(uint16_t *)param);
				bytes_left -= 2;
			}
		}
	}
	/* finished */
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_FIFO_EOF, 0x0);
	ret = qpic_wait_for_eof();
exit_send_cmd_sw:
	cfg2 &= ~(1 << 24);
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_CFG2, cfg2);
	return ret;
}

int qpic_send_pkt(uint32_t cmd, uint8_t *param, uint32_t len)
{
	if (!use_bam || ((cmd != OP_WRITE_MEMORY_CONTINUE) &&
		(cmd != OP_WRITE_MEMORY_START)))
		return qpic_send_pkt_sw(cmd, len, param);
	else
		return qpic_send_pkt_bam(cmd, len, param);
}

int mdss_qpic_init(void)
{
	int ret = 0;
	uint32_t data;
	mdss_qpic_reset();

	data = QPIC_INP(QPIC_REG_QPIC_LCDC_CTRL);
	/* clear vsync wait , bam mode = 0 */
	data &= ~(3 << 0);
	data &= ~(0x1f << 3);
	data |= (1 << 3); /* threshold */
	data |= (1 << 8); /* lcd_en */
	data &= ~(0x1f << 9);
	data |= (1 << 9); /* threshold */
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_CTRL, data);

	QPIC_OUTP(QPIC_REG_QPIC_LCDC_CFG0, 0x02108501);
	data = QPIC_INP(QPIC_REG_QPIC_LCDC_CFG2);
	data &= ~(0xFFF);
	data |= 0x0; /* 565 */
	data |= 0x2C;
	QPIC_OUTP(QPIC_REG_QPIC_LCDC_CFG2, data);

	/* TE enable */
	if (use_vsync) {
		data = QPIC_INP(QPIC_REG_QPIC_LCDC_CTRL);
		data |= (1 << 0);
		QPIC_OUTP(QPIC_REG_QPIC_LCDC_CTRL, data);
	}

	return ret;
}

uint32_t qpic_read_data(uint32_t cmd_index, uint32_t size)
{
	uint32_t data = 0;
	if (size <= 4) {
		QPIC_OUTP(QPIC_REG_QPIC_LCDC_CMD_DATA_CYCLE_CNT, size);
		data = QPIC_INP(QPIC_REG_LCD_DEVICE_CMD0 + (cmd_index * 4));
	}
	return data;
}

