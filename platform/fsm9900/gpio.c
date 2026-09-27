/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *  * Neither the name of The Linux Foundation nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
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

#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
#include <gsbi.h>
#include <blsp_qup.h>

void gpio_tlmm_config(uint32_t gpio, uint8_t func,
		      uint8_t dir, uint8_t pull,
		      uint8_t drvstr, uint32_t enable)
{
	uint32_t val = 0;
	val |= pull;
	val |= func << 2;
	val |= drvstr << 6;
	val |= enable << 9;
	writel(val, (unsigned int *)GPIO_CONFIG_ADDR(gpio));
	return;
}

void gpio_set(uint32_t gpio, uint32_t dir)
{
	writel(dir, (unsigned int *)GPIO_IN_OUT_ADDR(gpio));
	return;
}

void gpio_config_uart_dm(uint8_t id)
{
	static struct {
		unsigned int gpio_tx;
		unsigned int gpio_rx;
	} gpio_table[] = {
		{ 0, 1 },
		{ 4, 5 },
		{ 8, 9 },
		{ 12, 13 },
		{ 16, 17 },
		{ 20, 21 },
		{ 24, 25 },
		{ 28, 29 },
		{ 32, 33 },
		{ 36, 37 },
	};

	if (id >= ARRAY_SIZE(gpio_table))
		return;

	/* configure rx gpio */
	gpio_tlmm_config(gpio_table[id].gpio_rx, 2, GPIO_INPUT, GPIO_NO_PULL,
				GPIO_8MA, GPIO_DISABLE);

	/* configure tx gpio */
	gpio_tlmm_config(gpio_table[id].gpio_tx, 2, GPIO_OUTPUT, GPIO_NO_PULL,
				GPIO_8MA, GPIO_DISABLE);
}

void gpio_config_blsp_i2c(uint8_t blsp_id, uint8_t qup_id)
{
	if (blsp_id == BLSP_ID_2) {
		switch (qup_id) {
		case QUP_ID_4:
			gpio_tlmm_config(83, 3, GPIO_OUTPUT, GPIO_NO_PULL,
						GPIO_6MA, GPIO_DISABLE);
			gpio_tlmm_config(84, 3, GPIO_OUTPUT, GPIO_NO_PULL,
						GPIO_6MA, GPIO_DISABLE);
		break;
		default:
			dprintf(CRITICAL, "Configure gpios for QUP instance: %u\n",
					  qup_id);
			ASSERT(0);
		};
	}
	else if (blsp_id == BLSP_ID_1) {
		switch (qup_id) {
		default:
			dprintf(CRITICAL, "Configure gpios for QUP instance: %u\n",
					   qup_id);
			ASSERT(0);
		};
	}
}
