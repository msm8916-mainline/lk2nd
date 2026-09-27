// SPDX-License-Identifier: GPL-2.0-only
/*
 * Mostly taken from LineageOS/original Xiaomi kernel source available at:
 * https://github.com/LineageOS/android_kernel_wingtech_msm8916/blob/lineage-17.1/drivers/power/bq2022a-batid.c
 * https://github.com/LineageOS/android_kernel_wingtech_msm8916/blob/lineage-17.1/drivers/power/smb1360-charger-fg-wt88047.c
 */

#include <debug.h>
#include <kernel/thread.h>
#include <platform/timer.h>

#include <lk2nd/hw/gpio.h>

#include "../../device.h"
#include "../smb1360.h"

/*
 * NOTE: This code is not intended to be pretty, it is copied with minimal
 * modifications from the vendor source code to ensure that it behaves exactly
 * the same.
 */

/* BQ2022A. */
#define	ROM_COMMAND		(0xcc)
#define	CTL_COMMAND		(0xc3)
#define	WADDR_LB		(0x00)
#define	WADDR_HB		(0x00)
#define	GPIO_HIGH		1
#define	GPIO_LOW		0

/* Compatibility with lk2nd's GPIO API */
#define gpio_get_value		gpiol_is_asserted
#define gpio_set_value		gpiol_set_asserted
#define gpio_direction_input	gpiol_direction_input
#define gpio_direction_output	gpiol_direction_output

#define pr_debug(...)	dprintf(SPEW, __VA_ARGS__)
#define pr_err(...)	dprintf(CRITICAL, __VA_ARGS__)

static struct gpiol_desc bq2022a_bat_id;

static unsigned char bq2022a_sdq_detect(void)
{
	unsigned int PresenceTimer = 50;
	/*static volatile*/ unsigned char InputData;
	/*static volatile*/ unsigned char GotPulse = 0;

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
	//unsigned long flags;

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
	//unsigned long flags;

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
	int bat_module_id = 0;
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
		if ((i < 8) && (bat_id != con_bat_id[i])) {
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

static int smb1360_wt88047_detect_battery(const void *dtb, int node)
{
	const struct smb1360_battery *battery = NULL;
	int delay_time, bat_module_id, ret;

	ret = gpiol_get(dtb, node, "bat-id", &bq2022a_bat_id, GPIOL_FLAGS_OUT_DEASSERTED);
	if (ret) {
		dprintf(CRITICAL, "Failed to request bat-id-gpios: %d\n", ret);
		return ret;
	}

	for (delay_time = 5; delay_time < 10; delay_time++) {
		mdelay(10);
		enter_critical_section();
		bat_module_id = bq2022a_read_bat_id(delay_time, 1);
		exit_critical_section();
		if (bat_module_id != 0xff)
			break;
	}

	if (bat_module_id != 0xff)
		battery = &wt88047_batteries[bat_module_id];

	return smb1360_battery_detected(battery);
}
LK2ND_DEVICE_INIT("wingtech,smb1360-wt88047", smb1360_wt88047_detect_battery);
