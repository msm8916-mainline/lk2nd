/*
 * Copyright (c) 2011-2012, Linux Foundation. All rights reserved.
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
 *     * Neither the name of Linux Foundation, Inc. nor the names of its
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
#include <bits.h>

#define PBL_ACCESS_2                          0x005
#define PBL_ACCESS_2_ENUM_TIMER_STOP          (1 << 1)

#define SYS_CONFIG_2                          0x007
#define SYS_CONFIG_2_BOOT_DONE                (1 << 6)
#define SYS_CONFIG_2_ADAPTIVE_BOOT_DISABLE    (1 << 7)

#define PM8921_LDO_REG_BASE                   0x0AE
#define PM8921_LDO_CTRL_REG(id)               (PM8921_LDO_REG_BASE + (2 * (id-1)))
#define PM8921_LDO_TEST_REG(id)               (PM8921_LDO_CTRL_REG(id) + 1)

/* Bit offsets LDO CTRL register */
#define PM8921_LDO_CTRL_REG_ENABLE        7
#define PM8921_LDO_CTRL_REG_PULL_DOWN     6
#define PM8921_LDO_CTRL_REG_POWER_MODE    5
#define PM8921_LDO_CTRL_REG_VOLTAGE       0

/* Bit offsets LDO Test register */
#define PM8921_LDO_TEST_REG_BANK_SEL             4
#define PM8921_LDO_TEST_REG_RW                   7
#define PM8921_LDO_TEST_REG_BANK2_RANGE_SEL      2
#define PM8921_LDO_TEST_REG_BANK2_FINE_STEP      1
#define PM8921_LDO_TEST_REG_BANK4_RANGE_EXT      0

#define GPIO_CNTL_BASE                        0x150
#define GPIO_CNTL(n)                          (GPIO_CNTL_BASE + n)

/* GPIO Bank register programming */
#define PM_GPIO_BANK_MASK                0x70
#define PM_GPIO_BANK_SHIFT               4
#define PM_GPIO_WRITE                    0x80

/* Bank 0 */
#define PM_GPIO_VIN_MASK                 0x0E
#define PM_GPIO_VIN_SHIFT                1
#define PM_GPIO_MODE_ENABLE              0x01

/* Bank 1 */
#define PM_GPIO_MODE_MASK                0x0C
#define PM_GPIO_MODE_SHIFT               2
#define PM_GPIO_OUT_BUFFER_OPEN_DRAIN    0x02
#define PM_GPIO_OUT_INVERT               0x01

#define PM_GPIO_MODE_OFF                 3
#define PM_GPIO_MODE_OUTPUT              2
#define PM_GPIO_MODE_INPUT               0
#define PM_GPIO_MODE_BOTH                1

/* Bank 2 */
#define PM_GPIO_PULL_MASK                0x0E
#define PM_GPIO_PULL_SHIFT               1

/* Bank 3 */
#define PM_GPIO_OUT_STRENGTH_MASK        0x0C
#define PM_GPIO_OUT_STRENGTH_SHIFT       2
#define PM_GPIO_PIN_ENABLE               0x00
#define PM_GPIO_PIN_DISABLE              0x01

/* Bank 4 */
#define PM_GPIO_FUNC_MASK                0x0E
#define PM_GPIO_FUNC_SHIFT               1

/* Bank 5 */
#define PM_GPIO_NON_INT_POL_INV          0x08

/* PON CTRL 1 register */
#define PM8921_PON_CTRL_1_REG            0x01C

#define PON_CTRL_1_PULL_UP_MASK          0xE0
#define PON_CTRL_1_USB_PWR_EN            0x10

#define PON_CTRL_1_WD_EN_MASK            0x08
#define PON_CTRL_1_WD_EN_RESET           0x08
#define PON_CTRL_1_WD_EN_PWR_OFF         0x00

/* SLEEP CTRL register */
#define PM8921_SLEEP_CTRL_REG            0x10A

#define SLEEP_CTRL_SMPL_EN_MASK          0x04
#define SLEEP_CTRL_SMPL_EN_RESET         0x04
#define SLEEP_CTRL_SMPL_EN_PWR_OFF       0x00

#define IRQ_BLOCK_SEL_USR_ADDR           0x1C0
#define IRQ_STATUS_RT_USR_ADDR           0x1C3

#define PM8921_LVS_REG_BASE       0x060
#define PM8921_LVS_CTRL_REG(id)   (PM8921_LVS_REG_BASE + (2 * (id-1)))
#define PM8921_LVS_TEST_REG(id)   (PM8921_LVS_CTRL_REG(id) + 1)

#define PM8921_RTC_CTRL                  0x11D
#define PM8921_RTC_ALARM_ENABLE          (1 << 1)

#define PM8921_LVS_100_CTRL_SW_EN             (1 << 7)
#define PM8921_LVS_100_CTRL_SLEEP_B_IGNORE    (1 << 4)
#define PM8921_LVS_100_TEST_VOUT_OK           (1 << 6)

#define PM8921_MPP_REG_BASE                   0x050
#define PM8921_MPP_CTRL_REG(id)               (PM8921_MPP_REG_BASE + (id-1))

#define PM8921_MPP_CTRL_DIGITAL_OUTPUT        (1 << 5)
#define PM8921_MPP_CTRL_VIO_1                 (1 << 2)
#define PM8921_MPP_CTRL_OUTPUT_HIGH           (1 << 0)

#define PM89XX_BAT_UP_THRESH_VOL              4
#define PM89XX_BAT_ALRM_THRESH                0x23
#define PM89XX_BAT_ALRM_CTRL                  0x24
#define PM89XX_USB_OVP_CTRL                   0x21C

#define PM89XX_BAT_ALRM_ENABLE                BIT(7)
#define PM89XX_BAT_UPR_STATUS                 BIT(1)
#define PM89XX_BAT_LWR_STATUS                 BIT(0)

#define PM89XX_VBUS_INPUT_STATUS              BIT(0)

/* voltages are specified in mV */
#define PLDO_MV_VMIN                          1500
#define PLDO_MV_VMAX                          3000
#define PLDO_MV_VSTEP                         50

#define NLDO_MV_VMIN                          750
#define NLDO_MV_VMAX                          1525
#define NLDO_MV_VSTEP                         25

#define PLDO_TYPE                             0
#define NLDO_TYPE                             1

#define PM8921_MVS_5V_HDMI_SWITCH             0x70

#define LDO(_name, _type, _test_reg, _ctrl_reg) \
{\
	.name = _name,\
	.type = _type,\
	.test_reg = _test_reg,\
	.ctrl_reg = _ctrl_reg, \
}

struct pm89xx_vreg ldo_data[] = {
	LDO("LDO30", PLDO_TYPE, 0x0A3, 0x0A4),
	LDO("LDO31", PLDO_TYPE, 0x0A5, 0x0A6),
	LDO("LDO32", PLDO_TYPE, 0x0A7, 0x0A8),
	LDO("LDO33", PLDO_TYPE, 0x0C6, 0x0C7),
	LDO("LDO34", PLDO_TYPE, 0x0D2, 0x0D3),
	LDO("LDO35", PLDO_TYPE, 0x0D4, 0x0D5),
	LDO("LDO36", PLDO_TYPE, 0x0A9, 0x0AA),
};
