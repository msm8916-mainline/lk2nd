/* Copyright (c) 2011, The Linux Foundation. All rights reserved.
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

void gpio_tlmm_config(uint32_t gpio, uint8_t func,
                      uint8_t dir, uint8_t pull,
                      uint8_t drvstr, uint32_t enable)
{
    unsigned int val = 0;
    val |= pull;
    val |= func << 2;
    val |= drvstr << 6;
    val |= enable << 9;
    unsigned int *addr = (unsigned int *)GPIO_CONFIG_ADDR(gpio);
    writel(val, addr);
    return;
}

void gpio_set(uint32_t gpio, uint32_t dir)
{
    unsigned int *addr = (unsigned int *)GPIO_IN_OUT_ADDR(gpio);
    writel(dir, addr);
    return;
}

/* Configure gpio for uart - based on gsbi id */
void gpio_config_uart_dm(uint8_t id)
{
	switch (id)
	{
	case GSBI_ID_4:
		/* Enable GSBI4 through gpio */
		/* configure rx gpio -- gsbi4(2) */
		gpio_tlmm_config(14, 1, GPIO_INPUT, GPIO_NO_PULL,
						GPIO_8MA, GPIO_DISABLE);
		/* configure tx gpio -- gsbi4(3) */
		gpio_tlmm_config(15, 1, GPIO_OUTPUT, GPIO_NO_PULL,
						GPIO_8MA, GPIO_DISABLE);
		break;
	default:
		ASSERT(0);
	}
}
