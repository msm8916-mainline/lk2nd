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
#include <platform/gpio.h>
#include "qpic.h"
#include "qpic_panel.h"
#include <reg.h>

static uint32_t panel_is_on;

static int (*qpic_panel_on)(struct qpic_panel_io_desc *qpic_panel_io);
static void (*qpic_panel_off)(struct qpic_panel_io_desc *qpic_panel_io);


/* write a frame of pixels to a MIPI screen */
uint32_t qpic_send_frame(uint32_t x_start,
				uint32_t y_start,
				uint32_t x_end,
				uint32_t y_end,
				uint32_t *data,
				uint32_t total_bytes)
{
	uint8_t param[4];
	uint32_t status;
	uint32_t start_0_7;
	uint32_t end_0_7;
	uint32_t start_8_15;
	uint32_t end_8_15;

	/* convert to 16 bit representation */
	x_start = x_start & 0xffff;
	y_start = y_start & 0xffff;
	x_end = x_end & 0xffff;
	y_end = y_end & 0xffff;

	/* set column/page */
	start_0_7 = x_start & 0xff;
	end_0_7 = x_end & 0xff;
	start_8_15 = (x_start >> 8) & 0xff;
	end_8_15 = (x_end >> 8) & 0xff;
	param[0] = start_8_15;
	param[1] = start_0_7;
	param[2] = end_8_15;
	param[3] = end_0_7;
	status = qpic_send_pkt(OP_SET_COLUMN_ADDRESS, param, 4);
	if (status) {
		dprintf(CRITICAL, "Failed to set column address\n");
		return status;
	}

	start_0_7 = y_start & 0xff;
	end_0_7 = y_end & 0xff;
	start_8_15 = (y_start >> 8) & 0xff;
	end_8_15 = (y_end >> 8) & 0xff;
	param[0] = start_8_15;
	param[1] = start_0_7;
	param[2] = end_8_15;
	param[3] = end_0_7;
	status = qpic_send_pkt(OP_SET_PAGE_ADDRESS, param, 4);
	if (status) {
		dprintf(CRITICAL, "Failed to set page address\n");
		return status;
	}

	status = qpic_send_pkt(OP_WRITE_MEMORY_START, (uint8_t *)data, total_bytes);
	if (status) {
		dprintf(CRITICAL, "Failed to start memory write\n");
		return status;
	}
	return 0;
}

int mdss_qpic_panel_on(struct qpic_panel_io_desc *panel_io)
{
	int rc = 0;

	if (panel_is_on)
		return 0;
	mdss_qpic_init();

	if (qpic_panel_on)
		rc = qpic_panel_on(panel_io);

	if (!rc)
		panel_is_on = true;

	return rc;
}

int mdss_qpic_panel_off(struct qpic_panel_io_desc *panel_io)
{
	if (qpic_panel_off)
		qpic_panel_off(panel_io);

	panel_is_on = false;
	return 0;
}

int mdss_qpic_panel_init(struct qpic_panel_io_desc *panel_io)
{
	qpic_panel_on = ili9341_on;
	qpic_panel_off = ili9341_off;

	return 0;
}

