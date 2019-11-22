/* Copyright (c) 2014, The Linux Foundation. All rights reserved.
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
 */

#include <debug.h>
#include <reg.h>
#include <platform/iomap.h>
#include <platform/gpio.h>
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
	writel(val, (uint32_t *)GPIO_CONFIG_ADDR(gpio));
	return;
}

void gpio_set_dir(uint32_t gpio, uint32_t dir)
{
	writel(dir, (uint32_t *)GPIO_IN_OUT_ADDR(gpio));
	return;
}

void gpio_set_value(uint32_t gpio, uint32_t value)
{
	/* GPIO_OUTPUT */
	if (value) {
		writel(GPIO_OUT_VAL(gpio),
			(uint32_t *)GPIO_OUT_SET_ADDR(gpio));
	} else {
		writel(GPIO_OUT_VAL(gpio),
			(uint32_t *)GPIO_OUT_CLR_ADDR(gpio));
	}
	/* GPIO_OE */
	writel(GPIO_OUT_OE_VAL(gpio),
			(uint32_t *)GPIO_OUT_OE_SET_ADDR(gpio));
	return;
}

uint32_t gpio_status(uint32_t gpio)
{
	return readl(GPIO_IN_OUT_ADDR(gpio)) & GPIO_IN;
}

/* Configure gpio for blsp uart 1 or 2 */
void gpio_config_uart_dm(uint8_t id)
{
	if (id == 1) {
		/* configure rx gpio */
		gpio_tlmm_config(1, 2, GPIO_INPUT, GPIO_NO_PULL,
					GPIO_8MA, GPIO_DISABLE);

		/* configure tx gpio */
		gpio_tlmm_config(0, 2, GPIO_OUTPUT, GPIO_NO_PULL,
					GPIO_8MA, GPIO_DISABLE);
	} else {
		/* configure rx gpio */
		gpio_tlmm_config(5, 2, GPIO_INPUT, GPIO_NO_PULL,
					GPIO_8MA, GPIO_DISABLE);

		/* configure tx gpio */
		gpio_tlmm_config(4, 2, GPIO_OUTPUT, GPIO_NO_PULL,
					GPIO_8MA, GPIO_DISABLE);
	}
}

void gpio_config_blsp_i2c(uint8_t blsp_id, uint8_t qup_id)
{
	if(blsp_id == BLSP_ID_1) {
		switch (qup_id) {
			case QUP_ID_1:
				/* configure I2C SDA gpio */
				gpio_tlmm_config(6, 3, GPIO_OUTPUT, GPIO_NO_PULL,
						GPIO_8MA, GPIO_DISABLE);

				/* configure I2C SCL gpio */
				gpio_tlmm_config(7, 3, GPIO_OUTPUT, GPIO_NO_PULL,
					GPIO_8MA, GPIO_DISABLE);
			break;
			default:
				dprintf(CRITICAL, "Incorrect QUP id %d\n",qup_id);
				ASSERT(0);
		};
	} else {
		dprintf(CRITICAL, "Incorrect BLSP id %d\n",blsp_id);
		ASSERT(0);
	}
}

int gpio_config(unsigned nr, unsigned flags)
{
	uint32_t val = 0;
	switch (flags) {
	case GPIO_INPUT:
		val &= ~BIT(9);
		break;
	case GPIO_OUTPUT:
		val |= BIT(9);
		break;
	default:
		dprintf(CRITICAL, "Flags not implemented: %d\n", flags);
		return -1;
	}
	writel(val, GPIO_CONFIG_ADDR(nr));
	return 0;
}

int gpio_get(unsigned nr)
{
	return readl(GPIO_IN_OUT_ADDR(nr)) & GPIO_IN;
}

void gpio_set(unsigned nr, unsigned on)
{
	uint32_t val;
	if (on)
		val = GPIO_OUT;
	else
		val = 0;
	writel(val, GPIO_IN_OUT_ADDR(nr));
}
