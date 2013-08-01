/*
 * Copyright (c) 2011-2013, Linux Foundation. All rights reserved.
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
 *     * Neither the name of  Linux Foundation, Inc. nor the names of its
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
#include <string.h>
#include <sys/types.h>
#include <err.h>
#include <dev/pm8921.h>
#include <platform/timer.h>
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

static int pm8921_masked_write(uint16_t addr,
					uint8_t mask, uint8_t val)
{
	int rc;
	uint8_t reg;

	rc = dev->read(&reg, 1, addr);
	if (rc)
	{
		return rc;
	}

	reg &= ~mask;
	reg |= val & mask;
	rc = dev->write(&reg, 1, addr);

	return rc;
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
	  dprintf (CRITICAL, "pm8291_gpio struct not defined\n");
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
		dprintf(CRITICAL, "Failed to write to PM8921 ret=%d.\n", ret);
		return -1;
	}
	return 0;
}

/* Reads the value of the irq status for the requested block */
int pm8921_irq_get_block_status(uint8_t block, uint8_t *status)
{
	int ret = 0;

	/* Select the irq block to be read */
	ret = dev->write(&block, 1, IRQ_BLOCK_SEL_USR_ADDR);

	if(!ret)
	{
		/* Read the real time irq status value for the block */
		ret = dev->read(status, 1, IRQ_STATUS_RT_USR_ADDR);
	}

	return ret;
}

/* Reads the status of requested gpio */
int pm8921_gpio_get(uint8_t gpio, uint8_t *status)
{
	int ret = 0;
	uint8_t block_status;

	ret = pm8921_irq_get_block_status(PM_GPIO_BLOCK_ID(gpio), &block_status);

	if(!ret)
	{
		if(block_status & PM_GPIO_ID_TO_BIT_MASK(gpio))
			*status = 1;
		else
			*status = 0;
	}

	return ret;
}

int pm8921_pwrkey_status(uint8_t *is_pwrkey_pressed)
{
	int ret = 0;
	uint8_t block_status;

	ret = pm8921_irq_get_block_status(PM_PWRKEY_BLOCK_ID, &block_status);

	if (!ret)
	{
		if(block_status & PM_PWRKEY_PRESS_BIT)
			*is_pwrkey_pressed = 1;
		else
			*is_pwrkey_pressed = 0;
	}
	return ret;
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
	val = 0x0;
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

/*
 * Configure PMIC for reset and power off.
 * reset = 1: Configure reset.
 * reset = 0: Configure power off.
 */
int pm8921_config_reset_pwr_off(unsigned reset)
{
	int rc;

	/* Enable SMPL(Short Momentary Power Loss) if resetting is desired. */
	rc = pm8921_masked_write(PM8921_SLEEP_CTRL_REG,
		   SLEEP_CTRL_SMPL_EN_MASK,
		   (reset ? SLEEP_CTRL_SMPL_EN_RESET : SLEEP_CTRL_SMPL_EN_PWR_OFF));
	if (rc)
	{
		goto read_write_err;
	}

	/*
	 * Select action to perform (reset or shutdown) when PS_HOLD goes low.
	 * Also ensure that KPD, CBL0, and CBL1 pull ups are enabled and that
	 * USB charging is enabled.
	 */
	rc = pm8921_masked_write(PM8921_PON_CTRL_1_REG,
		PON_CTRL_1_PULL_UP_MASK | PON_CTRL_1_USB_PWR_EN
		| PON_CTRL_1_WD_EN_MASK,
		PON_CTRL_1_PULL_UP_MASK | PON_CTRL_1_USB_PWR_EN
		| (reset ? PON_CTRL_1_WD_EN_RESET : PON_CTRL_1_WD_EN_PWR_OFF));
	if (rc)
	{
		goto read_write_err;
	}

read_write_err:
	return rc;
}

/* A wrapper function to configure PMIC PWM
 * pwm_id : Channel number to configure
 * duty_us : duty cycle for output waveform in micro seconds
 * period_us : period for output waveform in micro seconds
 */
int pm8921_set_pwm_config(uint8_t pwm_id, uint32_t duty_us, uint32_t period_us)
{
	int rc;

	rc = pm8921_pwm_config(pwm_id, duty_us, period_us, dev);

	return rc;
}

/* A wrapper function to enable PMIC PWM
 * pwm_id : Channel number to enable
 */
int pm8921_pwm_channel_enable(uint8_t pwm_id)
{
	int rc;

	rc = pm8921_pwm_enable(pwm_id, dev);

	return rc;
}

/* Configure LED's for current sinks
 * enable = 1: Configure external signal detection
 *             for the sink with the current level
 * enable = 0: Turn off external signal detection
 *
 * Values for sink are defined as follows:
 * 0 = MANUAL, turn on LED when curent [00000, 10100]
 * 1 = PWM1
 * 2 = PWM2
 * 3 = PWM3
 * 4 = DBUS1
 * 5 = DBUS2
 * 6 = DBUS3
 * 7 = DBUS4
 *
 * Current settings are calculated as per the equation:
 * [00000, 10100]: Iout = current * 2 mA
 * [10101, 11111]: invalid settings
 */

int pm8921_config_led_current(enum pm8921_leds led_num,
	uint8_t current,
	enum led_mode sink,
	int enable)
{
	uint8_t val;
	int ret;

	/* Program the CTRL reg */
	val = 0x0;

	if (enable != 0)
	{

		if (current > 0x15)
		{
			dprintf(CRITICAL, "Invalid current settings for PM8921 LED Ctrl Reg \
				current=%d.\n", current);
			return -1;
		}

		if (sink > 0x7)
		{
			dprintf(CRITICAL, "Invalid signal selection for PM8921 LED Ctrl Reg \
				sink=%d.\n", sink);
			return -1;
		}

		val |= LED_CURRENT_SET(current);
		val |= LED_SIGNAL_SELECT(sink);
	}

	ret = dev->write(&val, 1, PM8921_LED_CNTL_REG(led_num));

	if (ret)
		dprintf(CRITICAL, "Failed to write to PM8921 LED Ctrl Reg ret=%d.\n", ret);

	return ret;

}

/* Configure DRV_KEYPAD
 *drv_flash_sel:
 * 0000 = off
 * Iout = drv_flash_sel * 20 mA (300 mA driver)
 * Iout = drv_flash_sel * 40 mA (600 mA driver)
 *
 * flash_logic = 0 : flash is on when DTEST is high
 * flash_logic = 0 : flash is off when DTEST is high
 *
 * flash_ensel = 0 : manual mode, turn on flash when drv_flash_sel > 0
 * flash_ensel = 1 : DBUS1
 * flash_ensel = 2 : DBUS2
 * flash_ensel = 3 : enable flash from LPG
 */

int pm8921_config_drv_keypad(unsigned int drv_flash_sel, unsigned int flash_logic, unsigned int flash_ensel)
{
	uint8_t val;
	int ret;

	/* Program the CTRL reg */
	val = 0x0;

	if (drv_flash_sel != 0)
	{
		if (drv_flash_sel > 0x0F)
		{
			dprintf(CRITICAL, "Invalid current settings for PM8921 \
				KEYPAD_DRV Ctrl Reg drv_flash_sel=%d.\n", drv_flash_sel);
			return -1;
		}

		if (flash_logic > 1)
		{
			dprintf(CRITICAL, "Invalid signal selection for PM8921 \
				KEYPAD_DRV Ctrl Reg flash_logic=%d.\n", flash_logic);
			return -1;
		}

		if (flash_ensel > 3)
		{
			dprintf(CRITICAL, "Invalid signal selection for PM8921 \
				KEYPAD_DRV Ctrl Reg flash_ensel=%d.\n", flash_ensel);
			return -1;
		}

		val |= DRV_FLASH_SEL(drv_flash_sel);
		val |= FLASH_LOGIC_SEL(flash_logic);
		val |= FLASH_ENSEL(flash_ensel);
	}

	ret = dev->write(&val, 1, PM8921_DRV_KEYPAD_CNTL_REG);

	if (ret)
		dprintf(CRITICAL, "Failed to write to PM8921 KEYPAD_DRV Ctrl Reg ret=%d.\n", ret);

	return ret;

}

int pm8921_low_voltage_switch_enable(uint8_t lvs_id)
{
	int ret =  NO_ERROR;
	uint8_t val;

	if (lvs_id < lvs_start || lvs_id > lvs_end) {
		dprintf(CRITICAL, "Requested unsupported LVS.\n");
		return ERROR;
	}

	if (lvs_id == lvs_2) {
		dprintf(CRITICAL, "No support for LVS2 yet!\n");
		return ERROR;
	}

	/* Read LVS_TEST Reg first*/
	ret = dev->read(&val, 1, PM8921_LVS_TEST_REG(lvs_id));
	if (ret) {
		dprintf(CRITICAL, "Failed to read LVS_TEST Reg ret=%d.\n", ret);
		return ret;
	}

	/* Check if switch is already ON */
	val = val & PM8921_LVS_100_TEST_VOUT_OK;
	if (val)
		return ret;

	/* Turn on switch in normal mode */
	val = 0;
	val |= PM8921_LVS_100_CTRL_SW_EN; /* Enable Switch */
	val |= PM8921_LVS_100_CTRL_SLEEP_B_IGNORE; /* Ignore sleep mode pin */
	ret = dev->write(&val, 1, PM8921_LVS_CTRL_REG(lvs_id));
	if (ret)
		dprintf(CRITICAL, "Failed to write LVS_CTRL Reg ret=%d.\n", ret);

	return ret;
}

int pm8921_mpp_set_digital_output(uint8_t mpp_id)
{
	int ret = NO_ERROR;
	uint8_t val;

	if (mpp_id < mpp_start || mpp_id > mpp_end) {
		dprintf(CRITICAL, "Requested unsupported MPP.\n");
		return ERROR;
	}

	val = 0;
	/* Configure in digital output mode */
	val |= PM8921_MPP_CTRL_DIGITAL_OUTPUT;
	val |= PM8921_MPP_CTRL_VIO_1; /* Set input voltage to 1.8V */
	val |= PM8921_MPP_CTRL_OUTPUT_HIGH; /* Set mpp to high */

	ret = dev->write(&val, 1, PM8921_MPP_CTRL_REG(mpp_id));
	if (ret) {
		dprintf(CRITICAL, "Failed to write MPP_CTRL Reg ret=%d.\n",
			   ret);
	}

	return ret;
}

int pm8921_HDMI_Switch(void)
{
	int ret = NO_ERROR;
	uint8_t val;

	/* Value for HDMI MVS 5V Switch */
	val = 0x068;

	/* Turn on MVS 5V HDMI switch */
	ret = dev->write(&val, 1, PM8921_MVS_5V_HDMI_SWITCH);
	if (ret) {
		dprintf(CRITICAL,
		   "Failed to turn ON MVS 5V hdmi switch ret=%d.\n", ret);
	}

	return ret;
}

int pm8921_rtc_alarm_disable(void)
{
	int rc;
	uint8_t reg;

	rc = dev->read(&reg, 1, PM8921_RTC_CTRL);
	if (rc) {
		dprintf(CRITICAL,"Failed to read RTC_CTRL reg = %d\n",rc);
		return rc;
	}
	reg = (reg & ~PM8921_RTC_ALARM_ENABLE);

	rc = dev->write(&reg, 1, PM8921_RTC_CTRL);
	if (rc) {
		dprintf(CRITICAL,"Failed to write RTC_CTRL reg = %d\n",rc);
		return rc;
	}

	return rc;
}

/*
 * Set battery alarm with low & high threshold values
 */
int pm89xx_bat_alarm_set(bat_vol_t up_thresh_vol, bat_vol_t low_thresh_vol)
{
	int rc;
	uint8_t reg = 0;

	if ((up_thresh_vol > BAT_VOL_4_3) || (low_thresh_vol > BAT_VOL_4_3)) {
		dprintf(CRITICAL, "Input voltage not in permissible range\n");
		return 1;
	}

	/*
	 * Write upper & lower threshold values
	 */
	reg = (up_thresh_vol << PM89XX_BAT_UP_THRESH_VOL) | low_thresh_vol;

	rc = dev->write(&reg, 1, PM89XX_BAT_ALRM_THRESH);
	if (rc) {
		dprintf(CRITICAL, "Failed to set BAT_THRESH reg = %d\n", rc);
		return rc;
	}

	/* Read Alarm control to use the existing hysteresis values */
	rc = dev->read(&reg, 1, PM89XX_BAT_ALRM_CTRL);
	if (rc) {
		dprintf(CRITICAL, "Failed to read BAT_ALARM reg = %d\n", rc);
		return rc;
	}

	/* Enable battery alarm */
	reg |= PM89XX_BAT_ALRM_ENABLE;
	rc = dev->write(&reg, 1, PM89XX_BAT_ALRM_CTRL);
	if (rc) {
		dprintf(CRITICAL, "Failed to enable BAT_ALARM reg = %d\n", rc);
		return rc;
	}

	/* Wait for the comparator o/p to settle */
	mdelay(10);

	return rc;
}

/*
 * API to return status of battery
 * if the vbatt is below upper threshold return 0
 * if the vbatt is below lower threshold return 1
 */
int pm89xx_bat_alarm_status(uint8_t *high_status, uint8_t *low_status)
{
	int rc = 0;
	uint8_t reg = 0;

	/* Read the battery status */
	rc = dev->read(&reg, 1, PM89XX_BAT_ALRM_CTRL);
	if (rc) {
		dprintf(CRITICAL, "Failed to read BAT_ALARM reg = %d\n", rc);
		return rc;
	}

	/* Return the status if battery alarm is enabled */
	if (reg & PM89XX_BAT_ALRM_ENABLE) {
		*high_status = (reg & PM89XX_BAT_UPR_STATUS);
		*low_status = (reg & PM89XX_BAT_LWR_STATUS);
	} else {
		dprintf(CRITICAL, "Battery alarm is not enabled\n");
		return 1;
	}

	return rc;
}

/*
 * Return 1 if VBUS is connected, 0 otherwise
 */
int pm89xx_vbus_status(void)
{
	int rc;
	uint8_t reg = 0;

	rc = dev->read(&reg, 1, PM89XX_USB_OVP_CTRL);
	if (rc) {
		dprintf(CRITICAL, "Failed to read USB OVP CTRL = %d\n", rc);
		return rc;
	}

	reg &= PM89XX_VBUS_INPUT_STATUS;

	return reg;
}

static struct pm89xx_vreg *ldo_get(const char *ldo_name)
{
	uint8_t i;
	struct pm89xx_vreg *ldo = NULL;

	for (i = 0; i < ARRAY_SIZE(ldo_data); i++) {
		ldo = &ldo_data[i];
		if (!strncmp(ldo->name, ldo_name, strlen(ldo_name)))
			break;
	}

	return ldo;
}

/*
 * API takes LDO name & voltage as input
 * Input voltage is taken in mVs
 * PLDO voltage ranging from 1500mV to 3000mV
 * NLDO voltage ranging from 750mV to 1525mV
 */
int pm89xx_ldo_set_voltage(const char *ldo_name, uint32_t voltage)
{
	uint8_t mult;
	uint8_t val = 0;
	int32_t ret = 0;
	struct pm89xx_vreg *ldo;

	/* Find the LDO info from table */
	ldo = ldo_get(ldo_name);

	if (!ldo) {
		dprintf(CRITICAL, "Requested LDO is not supported : \
				%s\n", ldo_name);
		return -1;
	}

	/* Find the voltage multiplying factor */
	if (ldo->type == PLDO_TYPE) {
		if (voltage < PLDO_MV_VMIN)
			voltage = PLDO_MV_VMIN;
		else if (voltage > PLDO_MV_VMAX)
			voltage = PLDO_MV_VMAX;
		mult = (voltage - PLDO_MV_VMIN) / PLDO_MV_VSTEP;
	} else {
		if (voltage < NLDO_MV_VMIN)
			voltage = NLDO_MV_VMIN;
		else if (voltage > NLDO_MV_VMAX)
			voltage = NLDO_MV_VMAX;
		mult = (voltage - NLDO_MV_VMIN) / NLDO_MV_VSTEP;
	}

	/* Program the TEST reg */
	if (ldo->type == PLDO_TYPE) {
		/* Bank 2, only for p ldo, use 1.25V reference */
		val = 0x0;
		val |= (1 << PM8921_LDO_TEST_REG_RW);
		val |= (2 << PM8921_LDO_TEST_REG_BANK_SEL);
		ret = dev->write(&val, 1, ldo->test_reg);
		if (ret) {
			dprintf(CRITICAL, "Failed to write to PM8921 LDO Test \
					Reg ret=%d.\n", ret);
			return -1;
		}

		/*
		 * Bank 4, only for p ldo, disable output range ext,
		 * normal capacitance
		 */
		val = 0x0;
		val |= (1 << PM8921_LDO_TEST_REG_RW);
		val |= (4 << PM8921_LDO_TEST_REG_BANK_SEL);
		ret = dev->write(&val, 1, ldo->test_reg);
		if (ret) {
			dprintf(CRITICAL, "Failed to write to PM8921 LDO Test \
					Reg ret=%d.\n", ret);
			return -1;
		}
	}

	/* Program the CTRL reg */
	val = 0x0;
	val |= (1 << PM8921_LDO_CTRL_REG_ENABLE);
	val |= (1 << PM8921_LDO_CTRL_REG_PULL_DOWN);
	val |= (0 << PM8921_LDO_CTRL_REG_POWER_MODE);
	val |= (mult << PM8921_LDO_CTRL_REG_VOLTAGE);
	ret = dev->write(&val, 1, ldo->ctrl_reg);
	if (ret) {
		dprintf(CRITICAL, "Failed to write to PM8921 LDO Ctrl Reg \
				ret=%d.\n", ret);
		return -1;
	}

	return 0;
}

int pm8921_configure_wled(void)
{
	pm8921_masked_write(WLED_BOOST_CFG_REG, 0xFF, 0x47);
	pm8921_masked_write(WLED_HIGH_POLE_CAP_REG, 0xFF, 0x2c);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(2), 0xFF, 0x19);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(3), 0xFF, 0x59);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(4), 0xFF, 0x59);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(5), 0xFF, 0x66);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(6), 0xFF, 0x66);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(7), 0xFF, 0x0f);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(8), 0xFF, 0xff);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(9), 0xFF, 0x0f);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(10), 0xFF, 0xff);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(12), 0xFF, 0x16);
	pm8921_masked_write(SSBI_REG_ADDR_WLED_CTRL(13), 0xFF, 0x55);
	pm8921_masked_write(WLED_MOD_CTRL_REG, 0xFF, 0x7f);
	pm8921_masked_write(WLED_SYNC_REG, WLED_SYNC_MASK,	WLED_SYNC_VAL);
	pm8921_masked_write(WLED_SYNC_REG, WLED_SYNC_MASK,	WLED_SYNC_RESET_VAL);
}
