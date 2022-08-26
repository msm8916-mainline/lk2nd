/* Copyright (c) 2014-2018, 2020 The Linux Foundation. All rights reserved.
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
#include <ctype.h>
#include <platform/iomap.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <uart_dm.h>
#include <mmc.h>
#include <dev/keys.h>
#include <spmi.h>
#include <spmi_v2.h>
#include <pm8x41.h>
#include <board.h>
#include <baseband.h>
#include <hsusb.h>
#include <scm.h>
#include <platform/gpio.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <crypto5_wrapper.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <gpio.h>
#include <rpm-smd.h>
#include <qpic_nand.h>
#include <smem.h>
#include <secapp_loader.h>
#include <rpmb.h>
#include <boot_device.h>
#include <sdhci_msm.h>

#if LONG_PRESS_POWER_ON
#include <shutdown_detect.h>
#endif

#if PON_VIB_SUPPORT
#include <vibrator.h>
#endif

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0
#define TLMM_VOL_UP_BTN_GPIO    90
#define TLMM_VOL_DOWN_BTN_GPIO  91

#if PON_VIB_SUPPORT
#define VIBRATE_TIME    250
#endif
#define HW_SUBTYPE_APQ_NOWGR 0xA

#define CE1_INSTANCE            1
#define CE_EE                   1
#define CE_FIFO_SIZE            64
#define CE_READ_PIPE            3
#define CE_WRITE_PIPE           2
#define CE_READ_PIPE_LOCK_GRP   0
#define CE_WRITE_PIPE_LOCK_GRP  0
#define CE_ARRAY_SIZE           20
#define SUB_TYPE_SKUT           0x0A

/* Fastboot switch GPIO for Intrinsic board. */
#define USB_SW_GPIO_INTRINSIC_SOM 3

extern void smem_ptable_init(void);
extern void smem_add_modem_partitions(struct ptable *flash_ptable);
void target_sdc_init(void);

static struct ptable flash_ptable;

/* NANDc BAM pipe numbers */
#define DATA_CONSUMER_PIPE                            0
#define DATA_PRODUCER_PIPE                            1
#define CMD_PIPE                                      2

/* NANDc BAM pipe groups */
#define DATA_PRODUCER_PIPE_GRP                        0
#define DATA_CONSUMER_PIPE_GRP                        0
#define CMD_PIPE_GRP                                  1

/* NANDc EE */
#define QPIC_NAND_EE                                  0

/* NANDc max desc length. */
#define QPIC_NAND_MAX_DESC_LEN                        0x7FFF

#define LAST_NAND_PTN_LEN_PATTERN                     0xFFFFFFFF
#define UBI_CMDLINE " rootfstype=ubifs rootflags=bulk_read"

struct qpic_nand_init_config config;

struct mmc_device *dev;

static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

static void set_sdc_power_ctrl(void);
static void set_ebi2_config(void);

void update_ptable_names(void)
{
	uint32_t ptn_index;
	struct ptentry *ptentry_ptr = flash_ptable.parts;
	unsigned i;
	uint32_t len;

	/* Change all names to lower case. */
	for (ptn_index = 0; ptn_index != (uint32_t)flash_ptable.count; ptn_index++)
	{
		len = strlen(ptentry_ptr[ptn_index].name);

		for (i = 0; i < len; i++)
		{
			if (isupper(ptentry_ptr[ptn_index].name[i]))
			{
				ptentry_ptr[ptn_index].name[i] = tolower(ptentry_ptr[ptn_index].name[i]);
			}
		}

		/* SBL fills in the last partition length as 0xFFFFFFFF.
		* Update the length field based on the number of blocks on the flash.
		*/
		if ((uint32_t)(ptentry_ptr[ptn_index].length) == LAST_NAND_PTN_LEN_PATTERN)
		{
			ptentry_ptr[ptn_index].length = flash_num_blocks() - ptentry_ptr[ptn_index].start;
		}
	}
}

void target_early_init(void)
{
#if WITH_DEBUG_UART
	/* Do not intilaise UART in case the h/w
	* is RCM.
	*/
	uint32_t platform_subtype = board_hardware_subtype();

	if( board_hardware_id() == HW_PLATFORM_RCM )
		return;
	else if ( platform_subtype == HW_PLATFORM_SUBTYPE_8909_PM660_V1)
		uart_dm_init(2, 0, BLSP1_UART1_BASE);
	else
		uart_dm_init(1, 0, BLSP1_UART0_BASE);
#endif

}

int target_is_emmc_boot(void)
{
	return platform_boot_dev_isemmc();
}

void target_sdc_init(void)
{
	struct mmc_config_data config;

	/* Set drive strength & pull ctrl values */
	set_sdc_power_ctrl();

	config.bus_width = DATA_BUS_WIDTH_8BIT;
	config.max_clk_rate = MMC_CLK_177MHZ;

	/* Try slot 1*/
	config.slot         = 1;
	config.sdhc_base    = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base  = mmc_pwrctl_base[config.slot - 1];
	config.pwr_irq      = mmc_sdc_pwrctl_irq[config.slot - 1];
	config.hs400_support = 0;

	if (!(dev = mmc_init(&config))) {
	/* Try slot 2 */
		config.slot         = 2;
		config.max_clk_rate = MMC_CLK_200MHZ;
		config.sdhc_base    = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base  = mmc_pwrctl_base[config.slot - 1];
		config.pwr_irq      = mmc_sdc_pwrctl_irq[config.slot - 1];

		if (!(dev = mmc_init(&config))) {
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}
}

void *target_mmc_device(void)
{
	return (void *) dev;
}

/* Return 1 if vol_up pressed */
int target_volume_up(void)
{
	static uint8_t first_time = 0;
	uint8_t status = 0;

	if (!first_time) {
		gpio_tlmm_config(TLMM_VOL_UP_BTN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);

		/* Wait for the gpio config to take effect - debounce time */
		udelay(10000);

		first_time = 1;
	}

	/* Get status of GPIO */
	status = gpio_status(TLMM_VOL_UP_BTN_GPIO);

	/* Active low signal. */
	return !status;
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down(void)
{
	if ((board_hardware_id() == HW_PLATFORM_QRD) &&
			(board_hardware_subtype() == SUB_TYPE_SKUT)) {
		uint32_t status = 0;

		gpio_tlmm_config(TLMM_VOL_DOWN_BTN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);

		/* Wait for the gpio config to take effect - debounce time */
		thread_sleep(10);

		/* Get status of GPIO */
		status = gpio_status(TLMM_VOL_DOWN_BTN_GPIO);

		/* Active low signal. */
		return !status;
	} else {
		/* Volume down button tied in with PMIC RESIN. */
		return pm8x41_resin_status();
	}
}

static void target_keystatus(void)
{
	keys_init();

	if(target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if(target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);

}

static void set_sdc_power_ctrl(void)
{
	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, SDC1_HDRV_PULL_CTL },
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, SDC1_HDRV_PULL_CTL },
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, SDC1_HDRV_PULL_CTL },
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, SDC1_HDRV_PULL_CTL },
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, SDC1_HDRV_PULL_CTL },
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, SDC1_HDRV_PULL_CTL },
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
}

static void set_ebi2_config(void)
{
	/* Drive strength configs for ebi2 pins */
	struct tlmm_cfgs ebi2_hdrv_cfg[] =
	{
		{ EBI2_BUSY_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_WE_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_OE_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_CLE_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_ALE_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_CS_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_6MA, TLMM_HDRV_MASK, SDC1_HDRV_PULL_CTL },
	};

	/* Pull configs for ebi2 pins */
	struct tlmm_cfgs ebi2_pull_cfg[] =
	{
		{ EBI2_BUSY_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_WE_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_OE_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_CLE_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_ALE_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_CS_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, TLMM_EBI2_EMMC_GPIO_CFG },
		{ EBI2_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, SDC1_HDRV_PULL_CTL },
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(ebi2_hdrv_cfg, ARRAY_SIZE(ebi2_hdrv_cfg));
	tlmm_set_pull_ctrl(ebi2_pull_cfg, ARRAY_SIZE(ebi2_pull_cfg));

}
void target_init(void)
{
#if VERIFIED_BOOT || VERIFIED_BOOT_2
	int ret = 0;
#endif
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

#if (!WITH_LK2ND_DEVICE)
	target_keystatus();
#endif

#if BOOT_CONFIG_SUPPORT
	platform_read_boot_config();
#endif

	if (platform_boot_dev_isemmc()) {
		target_sdc_init();
		if (partition_read_table())
		{
			dprintf(CRITICAL, "Error reading the partition table info\n");
			ASSERT(0);
		}

	} else {
		set_ebi2_config();
		config.pipes.read_pipe = DATA_PRODUCER_PIPE;
		config.pipes.write_pipe = DATA_CONSUMER_PIPE;
		config.pipes.cmd_pipe = CMD_PIPE;

		config.pipes.read_pipe_grp = DATA_PRODUCER_PIPE_GRP;
		config.pipes.write_pipe_grp = DATA_CONSUMER_PIPE_GRP;
		config.pipes.cmd_pipe_grp = CMD_PIPE_GRP;

		config.bam_base = MSM_NAND_BAM_BASE;
		config.nand_base = MSM_NAND_BASE;
		config.ee = QPIC_NAND_EE;
		config.max_desc_len = QPIC_NAND_MAX_DESC_LEN;

		qpic_nand_init(&config);

		ptable_init(&flash_ptable);
		smem_ptable_init();
		smem_add_modem_partitions(&flash_ptable);

		update_ptable_names();
		flash_set_ptable(&flash_ptable);
	}

#if LONG_PRESS_POWER_ON
		shutdown_detect();
#endif

#if PON_VIB_SUPPORT

	/* turn on vibrator to indicate that phone is booting up to end user */
	vib_timed_turn_on(VIBRATE_TIME);
#endif

	if (target_use_signed_kernel())
		target_crypto_init_params();

#if VERIFIED_BOOT || VERIFIED_BOOT_2
	if (VB_M <= target_get_vb_version())
	{
		clock_ce_enable(CE1_INSTANCE);

		/* Initialize Qseecom */
		ret = qseecom_init();

		if (ret < 0)
		{
			dprintf(CRITICAL, "Failed to initialize qseecom, error: %d\n", ret);
			ASSERT(0);
		}

		/* Start Qseecom */
		ret = qseecom_tz_init();

		if (ret < 0)
		{
			dprintf(CRITICAL, "Failed to start qseecom, error: %d\n", ret);
			ASSERT(0);
		}

		if (rpmb_init() < 0)
		{
			dprintf(CRITICAL, "RPMB init failed\n");
			ASSERT(0);
		}

		/*
		 * Load the sec app for first time
		*/
		if (load_sec_app() < 0)
		{
			dprintf(CRITICAL, "Failed to load App for verified\n");
			ASSERT(0);
		}
	}
#endif

#if SMD_SUPPORT
	rpm_smd_init();
#endif
}

void target_serialno(unsigned char *buf)
{
	uint32_t serialno;
	if (target_is_emmc_boot())
		serialno = mmc_get_psn();
	else
		serialno = board_chip_serial();

	snprintf((char *)buf, SERIAL_NUMBER_LEN, "%x", serialno);

}

unsigned board_machtype(void)
{
	return LINUX_MACHTYPE_UNKNOWN;
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	/*
	* already fill the board->target on board.c
	*/
}

void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;

	platform = board->platform;
	switch(platform)
	{
	case MSM8909:
	case MSM8209:
	case MSM8208:
	case MSM8609:
	case MSM8909W:
	case MSM8905:
		board->baseband = BASEBAND_MSM;
		break;

	case MDM9209:
	case MDM9309:
	case MDM9609:
		board->baseband = BASEBAND_MDM;
		break;

	case APQ8009:
	case APQ8009W:
		if ((board->platform_hw == HW_PLATFORM_MTP) &&
			((board->platform_subtype == HW_SUBTYPE_APQ_NOWGR) ||
			 (board->platform_subtype == HW_PLATFORM_SUBTYPE_SWOC_NOWGR_CIRC)))
			board->baseband = BASEBAND_APQ_NOWGR;
		else
			board->baseband = BASEBAND_APQ;
		break;

	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n", platform);
		ASSERT(0);
	};
}
uint8_t target_panel_auto_detect_enabled(void)
{
	uint8_t ret = 0;
	uint32_t platform = board_platform_id();
	uint32_t hw_id = board_hardware_id();
	uint32_t target_id = board_target_id();
	uint32_t plat_hw_ver_major = ((target_id >> 16) & 0xFF);

	switch(platform) {
	case MSM8905:
		switch(hw_id) {
		case HW_PLATFORM_QRD:
			if(plat_hw_ver_major > 0x10 && plat_hw_ver_major < 0x13)
				ret = 1;
			break;
		default:
			break;
		}
		break;
	default:
		ret = 0;
		break;
	}
	return ret;
}

uint8_t target_is_spi(void)
{
	uint32_t platform = board_platform_id();
	uint32_t hw_id = board_hardware_id();

	if ((MSM8905 == platform) && (HW_PLATFORM_QRD == hw_id))
		return 1;
	return 0;

}

static uint8_t splash_override;
/* Returns 1 if target supports continuous splash screen. */
int target_cont_splash_screen(void)
{
	uint8_t splash_screen = 0;
	if (!splash_override) {
		switch (board_hardware_id()) {
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_MTP:
		case HW_PLATFORM_QRD:
		case HW_PLATFORM_RCM:
			splash_screen = 1;
			break;
		default:
			splash_screen = 0;
			break;
		}
		dprintf(SPEW, "Target_cont_splash=%d\n", splash_screen);
	}
	return splash_screen;
}

void target_force_cont_splash_disable(uint8_t override)
{
        splash_override = override;
}

/*Update this command line only for LE based builds*/
int get_target_boot_params(const char *cmdline, const char *part, char **buf)
{
	struct ptable *ptable;
	int system_ptn_index = -1;
	int le_based = -1;
	uint32_t buflen = 0;

	if (!cmdline || !part ) {
		dprintf(CRITICAL, "WARN: Invalid input param\n");
		return -1;
	}

	/*LE partition.xml will have recoveryfs partition*/
	if (target_is_emmc_boot())
		le_based = partition_get_index("recoveryfs");
	else
		/*Nand targets by default have this*/
		le_based = 1;

	if (le_based != -1)
	{
		if (!target_is_emmc_boot())
		{
			ptable = flash_get_ptable();
			if (!ptable)
			{
				dprintf(CRITICAL,
					"WARN: Cannot get flash partition table\n");
				return -1;
			}
			system_ptn_index = ptable_get_index(ptable, part);
		}
		else
			system_ptn_index = partition_get_index(part);
		if (system_ptn_index < 0) {
			dprintf(CRITICAL,
				"WARN: Cannot get partition index for %s\n", part);
			return -1;
		}
		/*
		* check if cmdline contains "root=" at the beginning of buffer or
		* " root=" in the middle of buffer.
		*/
		if (((!strncmp(cmdline, "root=", strlen("root="))) ||
			(strstr(cmdline, " root=")))) {
			dprintf(DEBUG, "DEBUG: cmdline has root=\n");
			return -1;
		}
		else
		/*in success case buf will be freed in the calling function of this*/
		{
			if (!target_is_emmc_boot())
			{
				buflen = strlen(UBI_CMDLINE) + strlen(" root=ubi0:rootfs ubi.mtd=") + sizeof(int) + 1; /* 1 byte for null character*/

				/* In success case, this memory is freed in calling function */
				*buf = (char *)malloc(buflen);
				if(!(*buf)) {
					dprintf(CRITICAL,"Unable to allocate memory for boot params \n");
					return -1;
				}

				/* Adding command line parameters according to target boot type */
				snprintf(*buf, buflen, UBI_CMDLINE);
				snprintf(*buf+strlen(*buf), buflen, " root=ubi0:rootfs ubi.mtd=%d", system_ptn_index);
			}
			else
			{
				/* Extra character is for Null termination */
				buflen = strlen(" root=/dev/mmcblk0p") + sizeof(int) + 1;

				/* In success case, this memory is freed in calling function */
				*buf = (char *)malloc(buflen);
				if(!(*buf)) {
					dprintf(CRITICAL,"Unable to allocate memory for boot params \n");
					return -1;
				}

				/*For Emmc case increase the ptn_index by 1*/
				snprintf(*buf, buflen, " root=/dev/mmcblk0p%d",system_ptn_index + 1);
			}
		}

		/*Return for LE based Targets.*/
		return 0;
	}
	return -1;
}

unsigned target_baseband(void)
{
	return board_baseband();
}

int emmc_recovery_init(void)
{
	return _emmc_recovery_init();
}

void target_usb_init(void)
{
	uint32_t val;

	/* Select and enable external configuration with USB PHY */
	ulpi_write(ULPI_MISC_A_VBUSVLDEXTSEL | ULPI_MISC_A_VBUSVLDEXT, ULPI_MISC_A_SET);

	/* Enable sess_vld */
	val = readl(USB_GENCONFIG_2) | GEN2_SESS_VLD_CTRL_EN;
	writel(val, USB_GENCONFIG_2);

	/* Enable external vbus configuration in the LINK */
	val = readl(USB_USBCMD);
	val |= SESS_VLD_CTRL;
	writel(val, USB_USBCMD);
}

unsigned target_pause_for_battery_charge(void)
{
	uint32_t pmic = target_get_pmic();
	uint8_t pon_reason = 0;
	uint8_t is_cold_boot = 0;

	/* In case of fastboot reboot,adb reboot or if we see the power key
	* pressed we do not want go into charger mode.
	* fastboot reboot is warm boot with PON hard reset bit not set
	* adb reboot is a cold boot with PON hard reset bit set
	*/
	if (pmic == PMIC_IS_PM660)
	{
		pon_reason = pm660_get_pon_reason();
		is_cold_boot = pm660_get_is_cold_boot();
	}
	else
	{
		pon_reason = pm8x41_get_pon_reason();
		is_cold_boot = pm8x41_get_is_cold_boot();
	}
	dprintf(INFO, "%s : pon_reason is %d cold_boot:%d\n", __func__,
		pon_reason, is_cold_boot);

	if (is_cold_boot &&
			(!(pon_reason & HARD_RST)) &&
			(!(pon_reason & KPDPWR_N)) &&
			((pon_reason & USB_CHG) || (pon_reason & DC_CHG) || (pon_reason & CBLPWR_N)))
		return 1;
	else
		return 0;
}

void target_usb_stop(void)
{
	/* Disable VBUS mimicing in the controller. */
	ulpi_write(ULPI_MISC_A_VBUSVLDEXTSEL | ULPI_MISC_A_VBUSVLDEXT, ULPI_MISC_A_CLEAR);
}


void target_uninit(void)
{
#if PON_VIB_SUPPORT
	/* wait for the vibrator timer is expried */
	wait_vib_timeout();
#endif

	if (platform_boot_dev_isemmc())
	{
		mmc_put_card_to_sleep(dev);
		sdhci_mode_disable(&dev->host);
	}

	if (crypto_initialized())
		crypto_eng_cleanup();

	if (target_is_ssd_enabled())
		clock_ce_disable(CE1_INSTANCE);

#if VERIFIED_BOOT || VERIFIED_BOOT_2
	if(VB_M <= target_get_vb_version())
	{
		if (is_sec_app_loaded())
		{
			if (send_milestone_call_to_tz() < 0)
			{
				dprintf(CRITICAL, "Failed to unload App for rpmb\n");
				ASSERT(0);
			}
		}

		if (rpmb_uninit() < 0)
		{
			dprintf(CRITICAL, "RPMB uninit failed\n");
			ASSERT(0);
		}

		clock_ce_disable(CE1_INSTANCE);
	}
#endif

#if SMD_SUPPORT
	rpm_smd_uninit();
#endif
}

/* Do any target specific intialization needed before entering fastboot mode */
void target_fastboot_init(void)
{
	uint32_t hw_id = board_hardware_id();
	uint32_t platform_subtype = board_hardware_subtype();

	/* Set the BOOT_DONE flag in PM8916 */
	pm8x41_set_boot_done();

	if (target_is_ssd_enabled()) {
		clock_ce_enable(CE1_INSTANCE);
		target_load_ssd_keystore();
	}

	if ((HW_PLATFORM_MTP == hw_id) &&
		(HW_PLATFORM_SUBTYPE_INTRINSIC_SOM == platform_subtype))
	{
		dprintf(SPEW, "Enabling PMIC GPIO for USB detection\n");

		struct pm8x41_gpio usbgpio_param = {
			.direction = PM_GPIO_DIR_OUT,
			.vin_sel = 0,
			.out_strength = PM_GPIO_OUT_DRIVE_MED,
			.function = PM_GPIO_FUNC_HIGH,
			.pull = PM_GPIO_PULLDOWN_10,
			.inv_int_pol = PM_GPIO_INVERT,
		};

		pm8x41_gpio_config(USB_SW_GPIO_INTRINSIC_SOM, &usbgpio_param);
		pm8x41_gpio_set(USB_SW_GPIO_INTRINSIC_SOM, 0);
	}

	return;
}

int set_download_mode(enum reboot_reason mode)
{
	int ret = 0;
	ret = scm_dload_mode(mode);

	pm8x41_clear_pmic_watchdog();

	return ret;
}

void target_load_ssd_keystore(void)
{
	uint64_t ptn;
	int      index;
	uint64_t size;
	uint32_t *buffer = NULL;

	if (!target_is_ssd_enabled())
		return;

	index = partition_get_index("ssd");

	ptn = partition_get_offset(index);
	if (ptn == 0){
		dprintf(CRITICAL, "Error: ssd partition not found\n");
		return;
	}

	size = partition_get_size(index);
	if (size == 0) {
		dprintf(CRITICAL, "Error: invalid ssd partition size\n");
		return;
	}

	buffer = memalign(CACHE_LINE, ROUNDUP(size, CACHE_LINE));
	if (!buffer) {
		dprintf(CRITICAL, "Error: allocating memory for ssd buffer\n");
		return;
	}
	if (mmc_read(ptn, buffer, size)) {
		dprintf(CRITICAL, "Error: cannot read data\n");
		free(buffer);
		return;
	}

	clock_ce_enable(CE1_INSTANCE);
	scm_protect_keystore(buffer, size);
	clock_ce_disable(CE1_INSTANCE);
	free(buffer);
}

crypto_engine_type board_ce_type(void)
{
	return CRYPTO_ENGINE_TYPE_HW;
}

/* Set up params for h/w CE. */
void target_crypto_init_params(void)
{
	struct crypto_init_params ce_params;

	/* Set up base addresses and instance. */
	ce_params.crypto_instance  = CE1_INSTANCE;
	ce_params.crypto_base      = MSM_CE1_BASE;
	ce_params.bam_base         = MSM_CE1_BAM_BASE;

	/* Set up BAM config. */
	ce_params.bam_ee               = CE_EE;
	ce_params.pipes.read_pipe      = CE_READ_PIPE;
	ce_params.pipes.write_pipe     = CE_WRITE_PIPE;
	ce_params.pipes.read_pipe_grp  = CE_READ_PIPE_LOCK_GRP;
	ce_params.pipes.write_pipe_grp = CE_WRITE_PIPE_LOCK_GRP;

	/* Assign buffer sizes. */
	ce_params.num_ce           = CE_ARRAY_SIZE;
	ce_params.read_fifo_size   = CE_FIFO_SIZE;
	ce_params.write_fifo_size  = CE_FIFO_SIZE;

	/* BAM is initialized by TZ for this platform.
	* Do not do it again as the initialization address space
	* is locked.
	*/
	ce_params.do_bam_init      = 0;

	crypto_init_params(&ce_params);
}

uint32_t target_get_hlos_subtype(void)
{
	return board_hlos_subtype();
}

void pmic_reset_configure(uint8_t reset_type)
{
	pm8x41_reset_configure(reset_type);
}

uint32_t target_get_pmic(void)
{
	uint32_t hw_id = board_hardware_id();
	uint32_t platform = board_platform_id();
	uint32_t platform_subtype = board_hardware_subtype();

	if ((MSM8909 == platform) &&
		(HW_PLATFORM_MTP == hw_id) &&
		(HW_PLATFORM_SUBTYPE_8909_PM8916 == platform_subtype))
		return PMIC_IS_PM8916;
	else if ((platform_subtype == HW_PLATFORM_SUBTYPE_8909_PM660) ||
		(platform_subtype == HW_PLATFORM_SUBTYPE_8909_PM660_V1) ||
		(platform_subtype == HW_PLATFORM_SUBTYPE_8909_COMPAL_ALPHA))
		return PMIC_IS_PM660;
	else
		return PMIC_IS_PM8909;
}
