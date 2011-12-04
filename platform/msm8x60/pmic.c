/*
 * * Copyright (c) 2010-2011, Code Aurora Forum. All rights reserved.
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
 *  * Neither the name of Code Aurora Forum, Inc. nor the names of its
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
#include <bits.h>
#include <platform/iomap.h>
#include <platform/pmic.h>

#define TRUE  1
#define FALSE 0

/* FTS regulator PMR registers */
#define SSBI_REG_ADDR_S1_PMR		(0xA7)
#define SSBI_REG_ADDR_S2_PMR		(0xA8)
#define SSBI_REG_ADDR_S3_PMR		(0xA9)
#define SSBI_REG_ADDR_S4_PMR		(0xAA)

#define REGULATOR_PMR_STATE_MASK	0x60
#define REGULATOR_PMR_STATE_OFF		0x20

/* Regulator control registers for shutdown/reset */
#define SSBI_REG_ADDR_L22_CTRL		0x121

/* SLEEP CNTL register */
#define SSBI_REG_ADDR_SLEEP_CNTL	0x02B

#define PM8058_SLEEP_SMPL_EN_MASK	0x04
#define PM8058_SLEEP_SMPL_EN_RESET	0x04
#define PM8058_SLEEP_SMPL_EN_PWR_OFF	0x00

/* PON CNTL 1 register */
#define SSBI_REG_ADDR_PON_CNTL_1	0x01C

#define PM8058_PON_PUP_MASK		0xF0

#define PM8058_PON_WD_EN_MASK		0x08
#define PM8058_PON_WD_EN_RESET		0x08
#define PM8058_PON_WD_EN_PWR_OFF	0x00

#define PM8058_RTC_CTRL		0x1E8
#define PM8058_RTC_ALARM_ENABLE	BIT(1)

#define PM_IRQ_ID_TO_BLOCK_INDEX(id) (uint8_t)(id / 8)
#define PM_IRQ_ID_TO_BIT_MASK(id)    (uint8_t)(1 << (id % 8))

/* HDMI MPP Registers */
#define SSBI_MPP_CNTRL_BASE		0x27
#define SSBI_MPP_CNTRL(n)		(SSBI_MPP_CNTRL_BASE + (n))

#define PM8901_MPP_TYPE_MASK		0xE0
#define PM8901_MPP_CONFIG_LVL_MASK	0x1C
#define PM8901_MPP_CONFIG_CTL_MASK	0x03
#define PM8901_MPP0_CTRL_VAL		0x30
#define VREG_PMR_STATE_MASK		0x60
#define VREG_PMR_STATE_HPM		0x7F
#define VS_CTRL_USE_PMR			0xD0
#define VS_CTRL_ENABLE_MASK		0xD0
#define LDO_CTRL_VPROG_MASK		0x1F
#define REGULATOR_EN_MASK		0x80
#define PM8901_HDMI_MVS_CTRL		0x058
#define PM8901_HDMI_MVS_PMR		0x0B8
#define PM8058_HDMI_L16_CTRL		0x08A

typedef int (*pm8058_write_func) (unsigned char *, unsigned short,
				  unsigned short);
extern int pa1_ssbi2_write_bytes(unsigned char *buffer, unsigned short length,
				 unsigned short slave_addr);
extern int pa1_ssbi2_read_bytes(unsigned char *buffer, unsigned short length,
				unsigned short slave_addr);
extern int pa2_ssbi2_write_bytes(unsigned char *buffer, unsigned short length,
				 unsigned short slave_addr);
extern int pa2_ssbi2_read_bytes(unsigned char *buffer, unsigned short length,
				unsigned short slave_addr);

/* PM8058 APIs */
int pm8058_write(uint16_t addr, uint8_t * data, uint16_t length)
{
	return pa1_ssbi2_write_bytes(data, length, addr);
}

int pm8058_read(uint16_t addr, uint8_t * data, uint16_t length)
{
	return pa1_ssbi2_read_bytes(data, length, addr);
}

void pm8058_write_one(unsigned data, unsigned address)
{
	pm8058_write_func wr_function = &pa1_ssbi2_write_bytes;
	if (wr_function == NULL)
		return;
	if ((*wr_function) (&data, 1, address))
		dprintf(CRITICAL, "Error in initializing register\n");

}

int pm8058_get_irq_status(pm_irq_id_type irq, bool * rt_status)
{
	unsigned block_index, reg_data, reg_mask;
	int errFlag;

	block_index = PM_IRQ_ID_TO_BLOCK_INDEX(irq);

	/* select the irq block */
	errFlag =
	    pa1_ssbi2_write_bytes(&block_index, 1, IRQ_BLOCK_SEL_USR_ADDR);
	if (errFlag) {
		dprintf(INFO, "Device Timeout");
		return 1;
	}

	/* read real time status */
	errFlag = pa1_ssbi2_read_bytes(&reg_data, 1, IRQ_STATUS_RT_USR_ADDR);
	if (errFlag) {
		dprintf(INFO, "Device Timeout");
		return 1;
	}
	reg_mask = PM_IRQ_ID_TO_BIT_MASK(irq);

	if ((reg_data & reg_mask) == reg_mask) {
		/* The RT Status is high. */
		*rt_status = TRUE;
	} else {
		/* The RT Status is low. */
		*rt_status = FALSE;
	}
	return 0;
}

bool pm8058_gpio_get(unsigned int gpio)
{
	pm_irq_id_type gpio_irq;
	bool status;
	int ret;

	gpio_irq = gpio + PM_GPIO01_CHGED_ST_IRQ_ID;
	ret = pm8058_get_irq_status(gpio_irq, &status);

	if (ret)
		dprintf(CRITICAL, "pm8058_gpio_get failed\n");

	return status;
}

int pm8058_mwrite(uint16_t addr, uint8_t val, uint8_t mask, uint8_t * reg_save)
{
	int rc = 0;
	uint8_t reg;

	reg = (*reg_save & ~mask) | (val & mask);
	if (reg != *reg_save)
		rc = pm8058_write(addr, &reg, 1);
	if (rc)
		dprintf(CRITICAL, "pm8058_write failed; addr=%03X, rc=%d\n",
			addr, rc);
	else
		*reg_save = reg;
	return rc;
}

int pm8058_ldo_set_voltage()
{
	int ret = 0;
	unsigned vprog = 0x00000110;
	ret =
	    pm8058_mwrite(PM8058_HDMI_L16_CTRL, vprog, LDO_CTRL_VPROG_MASK, 0);
	if (ret) {
		dprintf(SPEW, "Failed to set voltage for l16 regulator\n");
	}
	return ret;
}

int pm8058_vreg_enable()
{
	int ret = 0;
	ret =
	    pm8058_mwrite(PM8058_HDMI_L16_CTRL, REGULATOR_EN_MASK,
			  REGULATOR_EN_MASK, 0);
	if (ret) {
		dprintf(SPEW, "Vreg enable failed for PM 8058\n");
	}
	return ret;
}

/* PM8901 APIs */

/*
 * Write to the control registers on PMIC via the SSBI2 interface.
 * Returns : (0) on success and (-1) on error.
 */
int pm8901_write(uint8_t * buffer, uint32_t length, uint32_t slave_addr)
{
	return pa2_ssbi2_write_bytes(buffer, length, slave_addr);
}

/*
 * Read from the control registers on PMIC via the SSBI2 interface.
 * Returns : (0) on success and (-1) on error.
 */
int pm8901_read(uint8_t * buffer, uint32_t length, uint32_t slave_addr)
{
	return pa2_ssbi2_read_bytes(buffer, length, slave_addr);
}

/*
 * PMIC 8901 LDO vreg read.
 */
int pm8901_test_bank_read(uint8_t * buffer, uint8_t bank, uint16_t addr)
{
	int ret = pm8901_write(&bank, 1, addr);
	/* if the write does not work we can't read. */
	if (ret) {
		return ret;
	}

	return pm8901_read(buffer, 1, addr);
}

/*
 * PMIC 8901 LDO vreg write.
 */
int pm8901_vreg_write(uint8_t * buffer, uint8_t mask, uint16_t addr,
		      uint8_t prev_val)
{
	uint8_t reg;

	/* Clear the bits we want to try and set. */
	reg = (prev_val & ~mask);
	/* Set the bits we want to set, before writing them to addr */
	reg |= (*buffer & mask);
	return pm8901_write(&reg, 1, addr);
}

int pm8901_reset_pwr_off(int reset)
{
	int rc = 0, i;
	uint8_t pmr;
	uint8_t pmr_addr[4] = {
		SSBI_REG_ADDR_S2_PMR,
		SSBI_REG_ADDR_S3_PMR,
		SSBI_REG_ADDR_S4_PMR,
		SSBI_REG_ADDR_S1_PMR,
	};

	/* Turn off regulators S1, S2, S3, S4 when shutting down. */
	if (!reset) {
		for (i = 0; i < 4; i++) {
			rc = pm8901_read(&pmr, 1, pmr_addr[i]);
			if (rc) {
				goto get_out;
			}

			pmr &= ~REGULATOR_PMR_STATE_MASK;
			pmr |= REGULATOR_PMR_STATE_OFF;

			rc = pm8901_write(&pmr, 1, pmr_addr[i]);
			if (rc) {
				goto get_out;
			}
		}
	}

 get_out:
	return rc;
}

int pm8058_reset_pwr_off(int reset)
{
	int rc;
	uint8_t pon, ctrl, smpl;

	/* Set regulator L22 to 1.225V in high power mode. */
	rc = pm8058_read(SSBI_REG_ADDR_L22_CTRL, &ctrl, 1);
	if (rc) {
		goto get_out3;
	}
	/* Leave pull-down state intact. */
	ctrl &= 0x40;
	ctrl |= 0x93;

	rc = pm8058_write(SSBI_REG_ADDR_L22_CTRL, &ctrl, 1);
	if (rc) {
	}

 get_out3:
	if (!reset) {
		/* Only modify the SLEEP_CNTL reg if shutdown is desired. */
		rc = pm8058_read(SSBI_REG_ADDR_SLEEP_CNTL, &smpl, 1);
		if (rc) {
			goto get_out2;
		}

		smpl &= ~PM8058_SLEEP_SMPL_EN_MASK;
		smpl |= PM8058_SLEEP_SMPL_EN_PWR_OFF;

		rc = pm8058_write(SSBI_REG_ADDR_SLEEP_CNTL, &smpl, 1);
		if (rc) {
		}
	}

 get_out2:
	rc = pm8058_read(SSBI_REG_ADDR_PON_CNTL_1, &pon, 1);
	if (rc) {
		goto get_out;
	}

	pon &= ~PM8058_PON_WD_EN_MASK;
	pon |= reset ? PM8058_PON_WD_EN_RESET : PM8058_PON_WD_EN_PWR_OFF;

	/* Enable all pullups */
	pon |= PM8058_PON_PUP_MASK;

	rc = pm8058_write(SSBI_REG_ADDR_PON_CNTL_1, &pon, 1);
	if (rc) {
		goto get_out;
	}

 get_out:
	return rc;
}

int pm8058_rtc0_alarm_irq_disable(void)
{
	int rc;
	uint8_t reg;

	rc = pm8058_read(PM8058_RTC_CTRL, &reg, 1);
	if (rc) {
		return rc;
	}
	reg = (reg & ~PM8058_RTC_ALARM_ENABLE);

	rc = pm8058_write(PM8058_RTC_CTRL, &reg, 1);
	if (rc) {
		return rc;
	}

	return rc;
}

int pm8901_mpp_enable()
{
	uint8_t prevval = 0x0;
	uint16_t mask;
	uint8_t conf;
	int ret = 0;

	conf = PM8901_MPP0_CTRL_VAL;
	mask = PM8901_MPP_TYPE_MASK | PM8901_MPP_CONFIG_LVL_MASK |
	    PM8901_MPP_CONFIG_CTL_MASK;

	if (ret = pm8901_vreg_write(&conf, mask, SSBI_MPP_CNTRL(0), prevval)) {
		dprintf(SPEW, "PM8901 MPP failed\n");
	}
	return ret;
}

int pm8901_vs_enable()
{
	uint8_t val = VREG_PMR_STATE_HPM;
	int prevval = 0x0;
	int ret = 0;

	if (ret =
	    pm8901_vreg_write(&val, VREG_PMR_STATE_HPM, PM8901_HDMI_MVS_PMR,
			      prevval)) {
		dprintf(SPEW,
			"pm8901_vreg_write failed for MVS PMR register\n");
		return ret;
	}

	val = VS_CTRL_USE_PMR;
	if (ret =
	    pm8901_vreg_write(&val, VS_CTRL_ENABLE_MASK, PM8901_HDMI_MVS_CTRL,
			      prevval)) {
		dprintf(SPEW,
			"pm8901_vreg_write failed for MVS ctrl register\n");
		return ret;
	}
	return ret;
}
