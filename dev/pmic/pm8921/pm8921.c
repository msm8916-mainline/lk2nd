/*
 * Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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
#include <assert.h>
#include <sys/types.h>
#include <dev/pm8921.h>
#include "pm8921_hw.h"


static pm8921_dev_t *dev;

static uint8_t ldo_n_voltage_mult[LDO_VOLTAGE_ENTRIES] = {
	18, /* 1.2V */
	0,
	0,
	};

static uint8_t ldo_p_voltage_mult[LDO_VOLTAGE_ENTRIES] = {
	0,
	6, /* 1.8V */
	30, /* 3.0V */
	};

/* Intialize the pmic driver */
void pm8921_init(pm8921_dev_t *pmic)
{
	ASSERT(pmic);
	ASSERT(pmic->read);
	ASSERT(pmic->write);

	dev = pmic;

	dev->initialized = 1;
}

/* Set the BOOT_DONE flag */
void pm8921_boot_done(void)
{
	uint8_t val;

	ASSERT(dev);
	ASSERT(dev->initialized);

	dev->read(&val, 1, PBL_ACCESS_2);
	val |= PBL_ACCESS_2_ENUM_TIMER_STOP;
	/* TODO: Remove next line when h/w is rewired for battery simulation.*/
	val |= (0x7 << 2);
	dev->write(&val, 1, PBL_ACCESS_2);

	dev->read(&val, 1, SYS_CONFIG_2);
	val |= (SYS_CONFIG_2_BOOT_DONE | SYS_CONFIG_2_ADAPTIVE_BOOT_DISABLE);
	dev->write(&val, 1, SYS_CONFIG_2);
}

/* Configure PMIC GPIO */
int pm8921_gpio_config(int gpio, struct pm8921_gpio *param)
{
	int ret;
	uint8_t bank[6];
	uint8_t output_buf_config;
	uint8_t output_value;

	static uint8_t dir_map[] = {
		PM_GPIO_MODE_OFF,
		PM_GPIO_MODE_OUTPUT,
		PM_GPIO_MODE_INPUT,
		PM_GPIO_MODE_BOTH,
	};

	if (param == NULL) {
	  dprintf (INFO, "pm8291_gpio struct not defined\n");
          return -1;
	}

	/* Select banks and configure the gpio */
	bank[0] = PM_GPIO_WRITE |
		((param->vin_sel << PM_GPIO_VIN_SHIFT) &
			PM_GPIO_VIN_MASK) |
		PM_GPIO_MODE_ENABLE;

	/* bank1 */
	if ((param->direction & PM_GPIO_DIR_OUT) && param->output_buffer)
		output_buf_config = PM_GPIO_OUT_BUFFER_OPEN_DRAIN;
	else
		output_buf_config = 0;

	if ((param->direction & PM_GPIO_DIR_OUT) && param->output_value)
		output_value = 1;
	else
		output_value = 0;

	bank[1] = PM_GPIO_WRITE |
		((1 << PM_GPIO_BANK_SHIFT) & PM_GPIO_BANK_MASK) |
		((dir_map[param->direction] << PM_GPIO_MODE_SHIFT)
						& PM_GPIO_MODE_MASK) |
		output_buf_config |
		output_value;

	bank[2] = PM_GPIO_WRITE |
		((2 << PM_GPIO_BANK_SHIFT) & PM_GPIO_BANK_MASK) |
		((param->pull << PM_GPIO_PULL_SHIFT) &
			PM_GPIO_PULL_MASK);

	bank[3] = PM_GPIO_WRITE |
		((3 << PM_GPIO_BANK_SHIFT) & PM_GPIO_BANK_MASK) |
		((param->out_strength << PM_GPIO_OUT_STRENGTH_SHIFT) &
			PM_GPIO_OUT_STRENGTH_MASK) |
		(param->disable_pin ? PM_GPIO_PIN_DISABLE : PM_GPIO_PIN_ENABLE);

	bank[4] = PM_GPIO_WRITE |
		((4 << PM_GPIO_BANK_SHIFT) & PM_GPIO_BANK_MASK) |
		((param->function << PM_GPIO_FUNC_SHIFT) &
			PM_GPIO_FUNC_MASK);

	bank[5] = PM_GPIO_WRITE |
		((5 << PM_GPIO_BANK_SHIFT) & PM_GPIO_BANK_MASK) |
		(param->inv_int_pol ? 0 : PM_GPIO_NON_INT_POL_INV);

	ret = dev->write(bank, 6, GPIO_CNTL(gpio));
	if (ret) {
		dprintf(INFO, "Failed to write to PM8921 ret=%d.\n", ret);
		return -1;
	}
	return 0;
}

int pm8921_ldo_set_voltage(uint32_t ldo_id, uint32_t voltage)
{
	uint8_t mult;
	uint8_t val = 0;
	uint32_t ldo_number = (ldo_id & ~LDO_P_MASK);
	int32_t ret = 0;

	/* Find the voltage multiplying factor */
	if(ldo_id & LDO_P_MASK)
		mult = ldo_p_voltage_mult[voltage];
	else
		mult = ldo_n_voltage_mult[voltage];

	/* Program the TEST reg */
	if (ldo_id & LDO_P_MASK){
		/* Bank 2, only for p ldo, use 1.25V reference */
		val = 0x0;
		val |= ( 1 << PM8921_LDO_TEST_REG_RW );
		val |= ( 2 << PM8921_LDO_TEST_REG_BANK_SEL);
		ret = dev->write(&val, 1, PM8921_LDO_TEST_REG(ldo_number));
		if (ret) {
			dprintf(CRITICAL, "Failed to write to PM8921 LDO Test Reg ret=%d.\n", ret);
			return -1;
		}

		/* Bank 4, only for p ldo, disable output range ext, normal capacitance */
		val = 0x0;
		val |= ( 1 << PM8921_LDO_TEST_REG_RW );
		val |= ( 4 << PM8921_LDO_TEST_REG_BANK_SEL);
		ret = dev->write(&val, 1, PM8921_LDO_TEST_REG(ldo_number));
		if (ret) {
			dprintf(CRITICAL, "Failed to write to PM8921 LDO Test Reg ret=%d.\n", ret);
			return -1;
		}
	}

	/* Program the CTRL reg */
	ret = dev->read(&val, 1, PM8921_LDO_CTRL_REG(ldo_number));
	if (ret == -1) {
		dprintf(CRITICAL, "Failed to read to PM8921 LDO Ctrl Reg ret=%d.\n", ret);
		return -1;
	}
	val |= ( 1 << PM8921_LDO_CTRL_REG_ENABLE);
	val |= ( 1 << PM8921_LDO_CTRL_REG_PULL_DOWN);
	val |= ( 0 << PM8921_LDO_CTRL_REG_POWER_MODE);
	val |= ( mult << PM8921_LDO_CTRL_REG_VOLTAGE);
	ret = dev->write(&val, 1, PM8921_LDO_CTRL_REG(ldo_number));
	if (ret) {
		dprintf(CRITICAL, "Failed to write to PM8921 LDO Ctrl Reg ret=%d.\n", ret);
		return -1;
	}

	return 0;
}
