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
#include <endian.h>
#include <platform/gpio.h>
#include <platform/clock.h>
#include <regulator.h>
#include <rpm-smd.h>
#include <platform/timer.h>

#include "qpic.h"
#include "qpic_panel.h"

#define GPIOMUX_FUNC_2 2
#define GPIOMUX_FUNC_GPIO 0

#define RST_GPIO_ID 23
#define CS_GPIO_ID 21
#define AD8_GPIO_ID 20
#define TE_GPIO_ID 22
#define BL_GPIO_ID 68

#define MEM_ACCESS_MODE 0x48
#define MEM_ACCESS_FORMAT 0x66

static uint32_t ldo6[][11] = {
	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_LDO_SOFTWARE_MODE, 4, SW_MODE_LDO_IPEAK,
		KEY_MICRO_VOLT, 4, 0,
	},
	{
		LDOA_RES_TYPE, 6,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_LDO_SOFTWARE_MODE, 4, SW_MODE_LDO_IPEAK,
		KEY_MICRO_VOLT, 4, 1800000,
	},
};

static uint32_t ldo12[][11] = {
	{
		LDOA_RES_TYPE, 12,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_DISABLE,
		KEY_LDO_SOFTWARE_MODE, 4, SW_MODE_LDO_IPEAK,
		KEY_MICRO_VOLT, 4, 0,
	},
	{
		LDOA_RES_TYPE, 12,
		KEY_SOFTWARE_ENABLE, 4, GENERIC_ENABLE,
		KEY_LDO_SOFTWARE_MODE, 4, SW_MODE_LDO_IPEAK,
		KEY_MICRO_VOLT, 4, 2700000,
	},
};

static void panel_io_off(struct qpic_panel_io_desc *qpic_panel_io)
{
	/* Turning off all gpios */
	gpio_tlmm_config(RST_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT, GPIO_NO_PULL,
				GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(CS_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT, GPIO_NO_PULL,
				GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(AD8_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT, GPIO_NO_PULL,
				GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(TE_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT, GPIO_NO_PULL,
				GPIO_10MA, GPIO_DISABLE);
	gpio_tlmm_config(BL_GPIO_ID, GPIOMUX_FUNC_GPIO, GPIO_INPUT,GPIO_NO_PULL,
				GPIO_10MA, GPIO_ENABLE);
	gpio_set(BL_GPIO_ID, 0x0);

	/* Disabling vdd & avdd voltage */
	rpm_send_data(&ldo6[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);
	rpm_send_data(&ldo12[GENERIC_DISABLE][0], 36, RPM_REQUEST_TYPE);

	return;
}

static int panel_io_on(struct qpic_panel_io_desc *qpic_panel_io)
{
	int rc = 0;

	/* Setting vdd & avdd voltage */
	rpm_send_data(&ldo6[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);
	rpm_send_data(&ldo12[GENERIC_ENABLE][0], 36, RPM_REQUEST_TYPE);

	/* Turning on all gpios */
	gpio_tlmm_config(RST_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT,GPIO_NO_PULL,
			GPIO_10MA, GPIO_ENABLE);
	gpio_tlmm_config(CS_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT,GPIO_NO_PULL,
			GPIO_10MA, GPIO_ENABLE);
	gpio_tlmm_config(AD8_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT,GPIO_NO_PULL,
			GPIO_10MA, GPIO_ENABLE);
	gpio_tlmm_config(TE_GPIO_ID, GPIOMUX_FUNC_2, GPIO_INPUT,GPIO_NO_PULL,
			GPIO_10MA, GPIO_ENABLE);
	gpio_tlmm_config(BL_GPIO_ID, GPIOMUX_FUNC_GPIO, GPIO_INPUT, GPIO_NO_PULL,
			GPIO_10MA, GPIO_DISABLE);
	gpio_set(BL_GPIO_ID, 0x2);
	mdelay(20);
	return rc;
}

void ili9341_off(struct qpic_panel_io_desc *qpic_panel_io)
{
	panel_io_off(qpic_panel_io);
}

int ili9341_on(struct qpic_panel_io_desc *qpic_panel_io)
{
	uint8_t param[4];
	int ret;

	ret = panel_io_on(qpic_panel_io);
	if (ret)
		return ret;
	qpic_send_pkt(OP_SOFT_RESET, NULL, 0);
	/* wait for 120 ms after reset as panel spec suggests */
	mdelay(120);
	qpic_send_pkt(OP_SET_DISPLAY_OFF, NULL, 0);
	/* wait for 20 ms after disply off */
	mdelay(20);

	/* set memory access control */
	param[0] = MEM_ACCESS_MODE;
	qpic_send_pkt(OP_SET_ADDRESS_MODE, param, 1);
	/* wait for 20 ms after command sent as panel spec suggests */
	mdelay(20);

	param[0] = MEM_ACCESS_FORMAT;
	qpic_send_pkt(OP_SET_PIXEL_FORMAT, param, 1);
	mdelay(20);

	/* set interface */
	param[0] = 1;
	param[1] = 0;
	param[2] = 0;
	qpic_send_pkt(OP_ILI9341_INTERFACE_CONTROL, param, 3);
	mdelay(20);

	qpic_send_pkt(OP_EXIT_SLEEP_MODE, NULL, 0);
	mdelay(20);

	qpic_send_pkt(OP_ENTER_NORMAL_MODE, NULL, 0);
	mdelay(20);

	qpic_send_pkt(OP_SET_DISPLAY_ON, NULL, 0);
	mdelay(20);

	param[0] = 0;
	qpic_send_pkt(OP_ILI9341_TEARING_EFFECT_LINE_ON, param, 1);

	param[0] = qpic_read_data(OP_GET_PIXEL_FORMAT, 1);

	return 0;
}

