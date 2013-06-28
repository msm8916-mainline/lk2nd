/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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

#include <err.h>
#include <assert.h>
#include <debug.h>
#include <reg.h>
#include <platform/timer.h>
#include <platform/iomap.h>
#include <mmc.h>
#include <clock.h>
#include <platform/clock.h>

void hsusb_clock_init(void)
{

}

void clock_init_mmc(uint32_t interface)
{

}

/* Configure MMC clock */
void clock_config_mmc(uint32_t interface, uint32_t freq)
{
	uint32_t reg;

	reg = 0;
	reg |= MMC_BOOT_MCI_CLK_ENABLE;
	reg |= MMC_BOOT_MCI_CLK_ENA_FLOW;
	reg |= MMC_BOOT_MCI_CLK_IN_FEEDBACK;
	writel(reg, MMC_BOOT_MCI_CLK);
}

/* Configure UART clock based on the UART block id*/
void clock_config_uart_dm(uint8_t id)
{

}

/* Function to asynchronously reset CE.
 * Function assumes that all the CE clocks are off.
 */
static void ce_async_reset(uint8_t instance)
{

}

static void clock_ce_enable(uint8_t instance)
{

}

static void clock_ce_disable(uint8_t instance)
{

}

void clock_config_ce(uint8_t instance)
{
	/* Need to enable the clock before disabling since the clk_disable()
	 * has a check to default to nop when the clk_enable() is not called
	 * on that particular clock.
	 */
	clock_ce_enable(instance);

	clock_ce_disable(instance);

	ce_async_reset(instance);

	clock_ce_enable(instance);
}
