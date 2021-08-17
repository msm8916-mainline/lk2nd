// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mostly taken from LineageOS/original Xiaomi kernel source available at:
 * https://github.com/LineageOS/android_kernel_wingtech_msm8916/blob/lineage-17.1/drivers/power/bq2022a-batid.c
 * https://github.com/LineageOS/android_kernel_wingtech_msm8916/blob/lineage-17.1/drivers/power/smb1360-charger-fg-wt88047.c
 */

#include <debug.h>
#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <platform/timer.h>
#include "smb1360.h"
#include "smb1360-i2c.h"

/* BQ2022A. */
#define	ROM_COMMAND		(0xcc)
#define	CTL_COMMAND		(0xc3)
#define	WADDR_LB		(0x00)
#define	WADDR_HB		(0x00)
#define	GPIO_HIGH		1
#define	GPIO_LOW		0

#define bq2022a_bat_id		3	/* PM8916 GPIO3 */

#define pr_debug(...)	do { if (0) dprintf(SPEW, __VA_ARGS__); } while (0)
#define pr_err(...)	dprintf(CRITICAL, __VA_ARGS__)

static void gpio_direction_input(uint8_t gpio)
{
	pm8x41_gpio_direction(gpio, PM_GPIO_DIR_IN, 0);
}
static void gpio_direction_output(uint8_t gpio, uint8_t value)
{
	pm8x41_gpio_direction(gpio, PM_GPIO_DIR_OUT, value);
}
static uint8_t gpio_get_value(uint8_t gpio)
{
	uint8_t status;
	pm8x41_gpio_get(gpio, &status);
	return status;
}
static void gpio_set_value(uint8_t gpio, uint8_t value)
{
	/* Note: Usually this is not the same but in this case it is */
	gpio_direction_output(gpio, value);
}

static unsigned char bq2022a_sdq_detect(void)
{
	unsigned int PresenceTimer = 50;
	/*static volatile*/ unsigned char InputData;
	/*static volatile*/ unsigned char GotPulse;

	gpio_direction_output(bq2022a_bat_id, GPIO_LOW);

	/* Reset time should be > 480 usec */
	udelay(800);
	gpio_direction_input(bq2022a_bat_id);
	udelay(60);

	while ((PresenceTimer > 0) && (GotPulse == 0)) {
		InputData = gpio_get_value(bq2022a_bat_id);
		if (InputData == 0) {
			GotPulse = 1;
		} else {
			GotPulse = 0;
			--PresenceTimer;
		}
	}
	udelay(200);

	return GotPulse;
}

static unsigned char bq2022a_sdq_readbyte(int time)
{
	unsigned char data = 0x00;
	unsigned char mask, i;
	unsigned long flags;

	//spin_lock_irqsave(&g_bq2022a->bqlock, flags);

	for (i = 0; i < 8; i++) {
		gpio_direction_output(bq2022a_bat_id, GPIO_LOW);

		udelay(7);
		gpio_direction_input(bq2022a_bat_id);
		udelay(time);
		mask = gpio_get_value(bq2022a_bat_id);
		udelay(65);
		mask <<= i;
		data = (data | mask);
	}

	udelay(200);
	//spin_unlock_irqrestore(&g_bq2022a->bqlock, flags);

	return data;
}

static void bq2022a_sdq_writebyte(uint8_t value)
{
	unsigned char mask = 1;
	int i;
	unsigned long flags;

	//spin_lock_irqsave(&g_bq2022a->bqlock, flags);

	gpio_direction_output(bq2022a_bat_id, GPIO_HIGH);

	for (i = 0; i < 8; i++) {
		gpio_set_value(bq2022a_bat_id, GPIO_LOW);
		udelay(4);
		if (mask & value) {
			udelay(10);
			gpio_set_value(bq2022a_bat_id, GPIO_HIGH);
			udelay(100);
		} else {
			udelay(100);
			gpio_set_value(bq2022a_bat_id, GPIO_HIGH);
			udelay(10);
		}

		udelay(7);
		mask <<= 1;
	}

	//spin_unlock_irqrestore(&g_bq2022a->bqlock, flags);

}

static const unsigned char con_bat_id[] = {
	0xed, 0x21, 0x4c, 0xe5,
	0xed, 0xa9, 0x4b, 0x2e,
};

static int bq2022a_read_bat_id(int delay_time, int pimc_pin)
{
	bool is_battery_feedback = false;
	unsigned char bat_id = 0xff;
	unsigned char reset_id = 0;
	int bat_module_id;
	int i = 0;

	for (i = 0; i < 10; i++) {
		reset_id = bq2022a_sdq_detect();
		if (reset_id && pimc_pin) {
			is_battery_feedback = true;
			break;
		}
	}

	/* Attempting to read seems stupid if bq2022a is not present */
	if (!is_battery_feedback) {
		pr_err("bq2022a does not seem to be present\n");
		return 0xff;
	}

	bq2022a_sdq_writebyte(ROM_COMMAND);
	bq2022a_sdq_writebyte(CTL_COMMAND);
	bq2022a_sdq_writebyte(WADDR_LB);
	bq2022a_sdq_writebyte(WADDR_HB);
	bat_id = bq2022a_sdq_readbyte(delay_time);

	for (i = 0; i < 34; i++) {
		bat_id = bq2022a_sdq_readbyte(delay_time);
		pr_debug("reset_id:%x is_battery_feedback:%d temp ID:%x!!\n", reset_id, is_battery_feedback, bat_id);
		if ((bat_id != con_bat_id[i]) && (i < 8)) {
			pr_err("read family code Error!!\n");
			break;
		}

		if ((i == 8) && (bat_id != 0x62)) {
			bat_module_id = (bat_id & 0x0f);
			break;
		} else if (i == 33) {
			if (bat_id == 0xdb)
				bat_module_id = 17;
			else if (bat_id == 0x3c)
				bat_module_id = 0x02;
			break;
		}
	}
	bat_id = bq2022a_sdq_readbyte(delay_time);
	gpio_direction_output(bq2022a_bat_id, GPIO_HIGH);

	if (((0 < bat_module_id) && (bat_module_id < 11)) || (bat_module_id == 17))
		pr_debug("get correct ID!!\n");
	else {
		if (is_battery_feedback) {
			bat_module_id = 13; // 0; Use special ID for fallback
			pr_debug("use common ID!!\n");
		} else {
			bat_module_id = 0xff;
			pr_debug("get wrong ID!!\n");
			//return -ENODEV;
		}
	}

	//pr_err("bat_module_id= %x\n", bat_module_id);
	return /*0*/ bat_module_id;
}

static const struct smb1360_battery wt88047_batteries[] = {
	[0] = {
		.name = "0 Common",
		.capacity_mah = 2200,
		.cc_soc_coeff = 0x8373,
		.therm_coeff = 0x85D2,
	},
	[1] = {
		.name = "1 Sunwoda + Samsung",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85D2,
	},
	[2] = {
		.name = "2 Guangyu + Guangyu",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85D2,
	},
	[3] = {
		.name = "3 Sunwoda + Sony",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85D2,
	},
	[4] = {
		.name = "4 Sunwoda + Samsung (customdown)",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85D2,
	},
	[5] = {
		.name = "5 Desay + LG",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85E0,
	},
	[6] = {
		.name = "6 Feimaotui + Sony",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85D2,
	},
	[7] = {
		.name = "7 AAC",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85E0,
	},
	[8] = {
		.name = "8 Guangyu (2200)",
		.capacity_mah = 2200,
		.cc_soc_coeff = 0x8373,
		.therm_coeff = 0x85D2,
	},
	[9] = {
		.name = "9 Desai (2200)",
		.capacity_mah = 2200,
		.cc_soc_coeff = 0x8373,
		.therm_coeff = 0x85E0,
	},
	[10] = {
		.name = "10 Sunwoda (2200)",
		.capacity_mah = 2200,
		.cc_soc_coeff = 0x8373,
		.therm_coeff = 0x85D2,
	},
	[13] = {
		/*
		 * Note: This one does not exist in the Xiaomi source code.
		 * It is used to make the fallback to 0 Common more obvious,
		 * which seems to be used for fake replacement batteries.
		 */
		.name = "WARNING: Fake battery? Using 0 Common (2200 mAh)",
		.capacity_mah = 2200,
		.cc_soc_coeff = 0x8373,
		.therm_coeff = 0x85D2,
	},
	[17] = {
		.name = "17 Feimaotui + Samsung (MI2A)",
		.capacity_mah = 2000,
		.cc_soc_coeff = 0x8819,
		.therm_coeff = 0x85D2,
	},
};

static bool smb1360_wt88047_check_rsense_10mohm(const struct smb1360 *smb)
{
	status_t ret;
	uint8_t val;

	if (!smb)
		return false;

	ret = regmap_read(smb->regmap, PRE_TO_FAST_REG, &val);
	if (ret || !(val >> PRE_TO_FAST_SHIFT)) {
		dprintf(CRITICAL, "PRE_TO_FAST not set for qcom,rsense-10mohm: ret %d val %#x\n", ret, val);
		return false;
	}

	return true;
}

const struct smb1360_battery *smb1360_wt88047_detect_battery(const struct smb1360 *smb,
							     const void *fdt, int offset)
{
	int delay_time, bat_module_id;
	struct pm8x41_gpio config = {
		.direction = PM_GPIO_DIR_OUT,
		.function = PM_GPIO_FUNC_LOW,
		.output_buffer = PM_GPIO_OUT_DRAIN_NMOS,
		.pull = PM_GPIO_NO_PULL, /* none */
		.vin_sel = 0, /* VPH_PWR */
		.out_strength = PM_GPIO_OUT_DRIVE_HIGH,
	};

	pm8x41_gpio_config(bq2022a_bat_id, &config);

	for (delay_time = 5; delay_time < 10; delay_time++) {
		bat_module_id = bq2022a_read_bat_id(delay_time, 1);
		if (bat_module_id != 0xff)
			break;
		mdelay(10);
	}

	/* Failed to detect battery? */
	if (bat_module_id == 0xff)
		return NULL;

	/*
	 * For some reason, smb1360-charger-fg-wt88047.c sets rsense-10mohm
	 * if PRE_TO_FAST_REG has some value (i.e. the threshold for switching
	 * from pre to fast charging is set). This does not really make sense.
	 *
	 * Perhaps this was only used to detect some early prototypes.
	 * The mainline device tree always sets qcom,rsense-10mohm but we verify
	 * the PRE_TO_FAST_REG here for safety reasons. If it's not set correctly,
	 * pretend that we failed to detect the battery so smb1360 is not enabled.
	 */
	if (!smb1360_wt88047_check_rsense_10mohm(smb))
		return NULL;

	return &wt88047_batteries[bat_module_id];
}
