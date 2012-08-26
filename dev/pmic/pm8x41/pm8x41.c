/* Copyright (c) 2012, Code Aurora Forum. All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#include <bits.h>
#include <debug.h>
#include <reg.h>
#include <spmi.h>
#include <pm8x41_hw.h>
#include <pm8x41.h>
#include <platform/timer.h>

/* Local Macros */
#define REG_READ(_a)        pm8x41_reg_read(_a)
#define REG_WRITE(_a, _v)   pm8x41_reg_write(_a, _v)

#define REG_OFFSET(_addr)   ((_addr) & 0xFF)
#define PERIPH_ID(_addr)    (((_addr) & 0xFF00) >> 8)
#define SLAVE_ID(_addr)     ((_addr) >> 16)


/* Local functions */
static uint8_t pm8x41_reg_read(uint32_t addr)
{
	uint8_t val = 0;
	struct pmic_arb_cmd cmd;
	struct pmic_arb_param param;

	cmd.address  = PERIPH_ID(addr);
	cmd.offset   = REG_OFFSET(addr);
	cmd.slave_id = SLAVE_ID(addr);
	cmd.priority = 0;

	param.buffer = &val;
	param.size   = 1;

	pmic_arb_read_cmd(&cmd, &param);

	return val;
}

static void pm8x41_reg_write(uint32_t addr, uint8_t val)
{
	struct pmic_arb_cmd cmd;
	struct pmic_arb_param param;

	cmd.address  = PERIPH_ID(addr);
	cmd.offset   = REG_OFFSET(addr);
	cmd.slave_id = SLAVE_ID(addr);
	cmd.priority = 0;

	param.buffer = &val;
	param.size   = 1;

	pmic_arb_write_cmd(&cmd, &param);
}

/* Exported functions */

/* Set the boot done flag */
void pm8x41_set_boot_done()
{
	uint8_t val;

	val  = REG_READ(SMBB_MISC_BOOT_DONE);
	val |= BIT(BOOT_DONE_BIT);
	REG_WRITE(SMBB_MISC_BOOT_DONE, val);
}

/* Configure GPIO */
int pm8x41_gpio_config(uint8_t gpio, struct pm8x41_gpio *config)
{
	uint8_t  val;
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);

	/* Only input configuration is implemented at this time. */
	ASSERT(config->direction == PM_GPIO_DIR_IN);

	/* Disable the GPIO */
	val  = REG_READ(gpio_base + GPIO_EN_CTL);
	val &= ~BIT(PERPH_EN_BIT);
	REG_WRITE(gpio_base + GPIO_EN_CTL, val);

	/* Select the mode */
	val = config->function | (config->direction << 4);
	REG_WRITE(gpio_base + GPIO_MODE_CTL, val);

	/* Set the right pull */
	val = config->pull;
	REG_WRITE(gpio_base + GPIO_DIG_PULL_CTL, val);

	/* Select the VIN */
	val = config->vin_sel;
	REG_WRITE(gpio_base + GPIO_DIG_VIN_CTL, val);

	/* Enable the GPIO */
	val  = REG_READ(gpio_base + GPIO_EN_CTL);
	val |= BIT(PERPH_EN_BIT);
	REG_WRITE(gpio_base + GPIO_EN_CTL, val);

	return 1;
}

/* Reads the status of requested gpio */
int pm8x41_gpio_get(uint8_t gpio, uint8_t *status)
{
	uint32_t gpio_base = GPIO_N_PERIPHERAL_BASE(gpio);

	*status = REG_READ(gpio_base + GPIO_STATUS);

	/* Return the value of the GPIO pin */
	*status &= BIT(GPIO_STATUS_VAL_BIT);

	dprintf(SPEW, "GPIO %d status is %d\n", gpio, *status);

	return 1;
}

/* Prepare PON RESIN S2 reset */
void pm8x41_vol_down_key_prepare()
{
	uint8_t val;

	/* disable s2 reset */
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, 0x0);

	/* Delay needed for disable to kick in. */
	udelay(300);

	/* configure s1 timer to 0 */
	REG_WRITE(PON_RESIN_N_RESET_S1_TIMER, 0x0);

	/* configure s2 timer to 2s */
	REG_WRITE(PON_RESIN_N_RESET_S2_TIMER, PON_RESIN_N_RESET_S2_TIMER_MAX_VALUE);

	/* configure reset type */
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, S2_RESET_TYPE_WARM);

	val = REG_READ(PON_RESIN_N_RESET_S2_CTL);

	/* enable s2 reset */
	val |= BIT(S2_RESET_EN_BIT);
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, val);
}

/* Volume_Down key detect cleanup */
void pm8x41_vol_down_key_done()
{
	/* disable s2 reset */
	REG_WRITE(PON_RESIN_N_RESET_S2_CTL, 0x0);

	/* Delay needed for disable to kick in. */
	udelay(300);
}

/* Volume_Down key status */
int pm8x41_vol_down_key_status()
{
	uint8_t rt_sts = 0;

	/* Enable S2 reset so we can detect the volume down key press */
	pm8x41_vol_down_key_prepare();

	/* Delay before interrupt triggering.
	 * See PON_DEBOUNCE_CTL reg.
	 */
	mdelay(100);

	rt_sts = REG_READ(PON_INT_RT_STS);

	/* Must disable S2 reset otherwise PMIC will reset if key
	 * is held longer than S2 timer.
	 */
	pm8x41_vol_down_key_done();

	return (rt_sts & BIT(RESIN_BARK_INT_BIT));
}

void pm8x41_reset_configure(uint8_t reset_type)
{
	uint8_t val;

	/* disable PS_HOLD_RESET */
	REG_WRITE(PON_PS_HOLD_RESET_CTL, 0x0);

	/* Delay needed for disable to kick in. */
	udelay(300);

	/* configure reset type */
	REG_WRITE(PON_PS_HOLD_RESET_CTL, reset_type);

	val = REG_READ(PON_PS_HOLD_RESET_CTL);

	/* enable PS_HOLD_RESET */
	val |= BIT(S2_RESET_EN_BIT);
	REG_WRITE(PON_PS_HOLD_RESET_CTL, val);
}
