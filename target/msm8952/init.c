/* Copyright (c) 2015-2016, The Linux Foundation. All rights reserved.
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
#include <platform/iomap.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <uart_dm.h>
#include <mmc.h>
#include <platform/gpio.h>
#include <dev/keys.h>
#include <spmi_v2.h>
#include <pm8x41.h>
#include <pm8x41_hw.h>
#include <board.h>
#include <baseband.h>
#include <hsusb.h>
#include <scm.h>
#include <platform/gpio.h>
#include <platform/gpio.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <crypto5_wrapper.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <rpm-smd.h>
#include <spmi.h>
#include <sdhci_msm.h>
#include <clock.h>
#include <boot_device.h>
#include <secapp_loader.h>
#include <rpmb.h>
#include <smem.h>

#include "target/display.h"

#if LONG_PRESS_POWER_ON
#include <shutdown_detect.h>
#endif

#if PON_VIB_SUPPORT
#include <vibrator.h>
#endif

#if PON_VIB_SUPPORT
#define VIBRATE_TIME    250
#endif

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0
#define TLMM_VOL_UP_BTN_GPIO    85
#define TLMM_VOL_UP_BTN_GPIO_8956 113
#define TLMM_VOL_UP_BTN_GPIO_8937 91
#define TLMM_VOL_DOWN_BTN_GPIO    128

#define FASTBOOT_MODE           0x77665500
#define RECOVERY_MODE           0x77665502
#define PON_SOFT_RB_SPARE       0x88F

#define EXT4_CMDLINE  " rootfstype=ext4 root=/dev/mmcblk0p"

#define CE1_INSTANCE            1
#define CE_EE                   1
#define CE_FIFO_SIZE            64
#define CE_READ_PIPE            3
#define CE_WRITE_PIPE           2
#define CE_READ_PIPE_LOCK_GRP   0
#define CE_WRITE_PIPE_LOCK_GRP  0
#define CE_ARRAY_SIZE           20
#define SUB_TYPE_SKUT           0x0A
#define SMBCHG_USB_RT_STS 0x21310
#define USBIN_UV_RT_STS BIT(0)

struct mmc_device *dev;

static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(2, 0, BLSP1_UART1_BASE);
#endif
}

static void set_sdc_power_ctrl()
{
	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, 0},
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, 0},
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK , 0},
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, 0},
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, 0},
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, 0},
	};

	struct tlmm_cfgs sdc1_rclk_cfg[] =
	{
		{ SDC1_RCLK_PULL_CTL_OFF, TLMM_PULL_DOWN, TLMM_PULL_MASK, 0},
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
	tlmm_set_pull_ctrl(sdc1_rclk_cfg, ARRAY_SIZE(sdc1_rclk_cfg));
}

void target_sdc_init()
{
	struct mmc_config_data config;

	/* Set drive strength & pull ctrl values */
	set_sdc_power_ctrl();

	/* Try slot 1*/
	config.slot          = 1;
	config.bus_width     = DATA_BUS_WIDTH_8BIT;
	config.max_clk_rate  = MMC_CLK_192MHZ;
	config.sdhc_base     = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base   = mmc_pwrctl_base[config.slot - 1];
	config.pwr_irq       = mmc_sdc_pwrctl_irq[config.slot - 1];
	config.hs400_support = 1;

	if (!(dev = mmc_init(&config))) {
	/* Try slot 2 */
		config.slot          = 2;
		config.max_clk_rate  = MMC_CLK_200MHZ;
		config.sdhc_base     = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base   = mmc_pwrctl_base[config.slot - 1];
		config.pwr_irq       = mmc_sdc_pwrctl_irq[config.slot - 1];
		config.hs400_support = 0;

		if (!(dev = mmc_init(&config))) {
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}
}

void *target_mmc_device()
{
	return (void *) dev;
}

/* Return 1 if vol_up pressed */
int target_volume_up()
{
	static uint8_t first_time = 0;
	uint8_t status = 0;
	uint32_t vol_up_gpio;

	if(platform_is_msm8956())
		vol_up_gpio = TLMM_VOL_UP_BTN_GPIO_8956;
	else if(platform_is_msm8937() || platform_is_msm8917())
		vol_up_gpio = TLMM_VOL_UP_BTN_GPIO_8937;
	else
		vol_up_gpio = TLMM_VOL_UP_BTN_GPIO;

	if (!first_time) {
		gpio_tlmm_config(vol_up_gpio, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);

		/* Wait for the gpio config to take effect - debounce time */
		udelay(10000);

		first_time = 1;
	}

	/* Get status of GPIO */
	status = gpio_status(vol_up_gpio);

	/* Active low signal. */
	return !status;
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down()
{
	static  bool vol_down_key_init = false;

	if ((board_hardware_id() == HW_PLATFORM_QRD) &&
			(board_hardware_subtype() == SUB_TYPE_SKUT)) {
		uint32_t status = 0;

		if (!vol_down_key_init) {
			gpio_tlmm_config(TLMM_VOL_DOWN_BTN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP,
				 GPIO_2MA, GPIO_ENABLE);
			/* Wait for the gpio config to take effect - debounce time */
			thread_sleep(10);
			vol_down_key_init = true;
		}

		/* Get status of GPIO */
		status = gpio_status(TLMM_VOL_DOWN_BTN_GPIO);

		/* Active low signal. */
		return !status;
	} else {
		/* Volume down button tied in with PMIC RESIN. */
		return pm8x41_resin_status();
	}
}

uint32_t target_is_pwrkey_pon_reason()
{
	uint8_t pon_reason = pm8950_get_pon_reason();
	bool usb_present_sts = !(USBIN_UV_RT_STS &
				pm8x41_reg_read(SMBCHG_USB_RT_STS));
	if (pm8x41_get_is_cold_boot() && ((pon_reason == KPDPWR_N) || (pon_reason == (KPDPWR_N|PON1))))
		return 1;
	else if ((pon_reason == PON1) && (!usb_present_sts))
		return 1;
	else
		return 0;
}

static void target_keystatus()
{
	keys_init();

	if(target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if(target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);
}

/* Configure PMIC and Drop PS_HOLD for shutdown */
void shutdown_device()
{
	dprintf(CRITICAL, "Going down for shutdown.\n");

	/* Configure PMIC for shutdown */
	pm8x41_reset_configure(PON_PSHOLD_SHUTDOWN);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "shutdown failed\n");

	ASSERT(0);
}


void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	if(target_is_pmi_enabled())
	{
		if(platform_is_msm8937() || platform_is_msm8917())
		{
			uint8_t pmi_rev = 0;
			uint32_t pmi_type = 0;

			pmi_type = board_pmic_target(1) & 0xffff;
			if(pmi_type == PMIC_IS_PMI8950)
			{
				/* read pmic spare register for rev */
				pmi_rev = pmi8950_get_pmi_subtype();
				if(pmi_rev)
					board_pmi_target_set(1,pmi_rev);
			}
		}
	}


	target_keystatus();

	target_sdc_init();
	if (partition_read_table())
	{
		dprintf(CRITICAL, "Error reading the partition table info\n");
		ASSERT(0);
	}

#if LONG_PRESS_POWER_ON
	if(target_is_pmi_enabled())
		shutdown_detect();
#endif

#if PON_VIB_SUPPORT
	/* turn on vibrator to indicate that phone is booting up to end user */
	if(target_is_pmi_enabled())
		vib_timed_turn_on(VIBRATE_TIME);
#endif

	if (target_use_signed_kernel())
		target_crypto_init_params();

#if VERIFIED_BOOT
	if (VB_V2 == target_get_vb_version())
	{
		clock_ce_enable(CE1_INSTANCE);

		/* Initialize Qseecom */
		if (qseecom_init() < 0)
		{
			dprintf(CRITICAL, "Failed to initialize qseecom\n");
			ASSERT(0);
		}

		/* Start Qseecom */
		if (qseecom_tz_init() < 0)
		{
			dprintf(CRITICAL, "Failed to start qseecom\n");
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
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

unsigned board_machtype(void)
{
	return LINUX_MACHTYPE_UNKNOWN;
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	/* This is already filled as part of board.c */
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;

	platform = board->platform;

	switch(platform) {
	case MSM8952:
	case MSM8956:
	case MSM8976:
	case MSM8937:
	case MSM8940:
	case MSM8917:
	case MSM8920:
	case MSM8217:
	case MSM8617:
		board->baseband = BASEBAND_MSM;
		break;
	case APQ8052:
	case APQ8056:
	case APQ8076:
	case APQ8037:
	case APQ8017:
		board->baseband = BASEBAND_APQ;
		break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n",platform);
		ASSERT(0);
	};
}

unsigned target_baseband()
{
	return board_baseband();
}

unsigned check_reboot_mode(void)
{
	uint32_t restart_reason = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);
	writel(0x00, RESTART_REASON_ADDR);

	return restart_reason;
}

unsigned check_hard_reboot_mode(void)
{
	uint8_t hard_restart_reason = 0;
	uint8_t value = 0;

	/* Read reboot reason and scrub it
	  * Bit-5, bit-6 and bit-7 of SOFT_RB_SPARE for hard reset reason
	  */
	value = pm8x41_reg_read(PON_SOFT_RB_SPARE);
	hard_restart_reason = value >> 5;
	pm8x41_reg_write(PON_SOFT_RB_SPARE, value & 0x1f);

	return hard_restart_reason;
}

int set_download_mode(enum reboot_reason mode)
{
	int ret = 0;
	ret = scm_dload_mode(mode);

	pm8x41_clear_pmic_watchdog();

	return ret;
}

int emmc_recovery_init(void)
{
	return _emmc_recovery_init();
}

void reboot_device(unsigned reboot_reason)
{
	uint8_t reset_type = 0;
	uint32_t ret = 0;

	/* Set cookie for dload mode */
	if(set_download_mode(reboot_reason)) {
		dprintf(CRITICAL, "HALT: set_download_mode not supported\n");
		return;
	}

	writel(reboot_reason, RESTART_REASON_ADDR);

	/* For Reboot-bootloader and Dload cases do a warm reset
	 * For Reboot cases do a hard reset
	 */
	if((reboot_reason == FASTBOOT_MODE) || (reboot_reason == NORMAL_DLOAD) ||
		(reboot_reason == EMERGENCY_DLOAD) || (reboot_reason == RECOVERY_MODE))
		reset_type = PON_PSHOLD_WARM_RESET;
	else
		reset_type = PON_PSHOLD_HARD_RESET;

	if(target_is_pmi_enabled())
		pm8994_reset_configure(reset_type);
	else
		pm8x41_reset_configure(reset_type);


	ret = scm_halt_pmic_arbiter();
	if (ret)
		dprintf(CRITICAL , "Failed to halt pmic arbiter: %d\n", ret);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
}

#if USER_FORCE_RESET_SUPPORT
/* Return 1 if it is a force resin triggered by user. */
uint32_t is_user_force_reset(void)
{
	uint8_t poff_reason1 = pm8x41_get_pon_poff_reason1();
	uint8_t poff_reason2 = pm8x41_get_pon_poff_reason2();

	dprintf(SPEW, "poff_reason1: %d\n", poff_reason1);
	dprintf(SPEW, "poff_reason2: %d\n", poff_reason2);
	if (pm8x41_get_is_cold_boot() && (poff_reason1 == KPDPWR_AND_RESIN ||
							poff_reason2 == STAGE3))
		return 1;
	else
		return 0;
}
#endif

unsigned target_pause_for_battery_charge(void)
{
	uint8_t pon_reason = pm8x41_get_pon_reason();
	uint8_t is_cold_boot = pm8x41_get_is_cold_boot();
	bool usb_present_sts = 1;	/* don't care by default */

	if(target_is_pmi_enabled())
		usb_present_sts = (!(USBIN_UV_RT_STS &
						 pm8x41_reg_read(SMBCHG_USB_RT_STS)));

	dprintf(INFO, "%s : pon_reason is:0x%x cold_boot:%d usb_sts:%d\n", __func__,
		pon_reason, is_cold_boot, usb_present_sts);
	/* In case of fastboot reboot,adb reboot or if we see the power key
	* pressed we do not want go into charger mode.
	* fastboot reboot is warm boot with PON hard reset bit not set
	* adb reboot is a cold boot with PON hard reset bit set
	*/
	if (is_cold_boot &&
			(!(pon_reason & HARD_RST)) &&
			(!(pon_reason & KPDPWR_N)) &&
			usb_present_sts)
		return 1;
	else
		return 0;
}

void target_uninit(void)
{
#if PON_VIB_SUPPORT
	if(target_is_pmi_enabled())
		turn_off_vib_early();
#endif
	mmc_put_card_to_sleep(dev);
	sdhci_mode_disable(&dev->host);
	if (crypto_initialized())
	{
		crypto_eng_cleanup();
		clock_ce_disable(CE1_INSTANCE);
	}

	if (target_is_ssd_enabled())
		clock_ce_disable(CE1_INSTANCE);

#if VERIFIED_BOOT
	if (VB_V2 == target_get_vb_version())
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

void target_usb_stop(void)
{
	/* Disable VBUS mimicing in the controller. */
	ulpi_write(ULPI_MISC_A_VBUSVLDEXTSEL | ULPI_MISC_A_VBUSVLDEXT, ULPI_MISC_A_CLEAR);
}

static uint8_t splash_override;
/* Returns 1 if target supports continuous splash screen. */
int target_cont_splash_screen()
{
	uint8_t splash_screen = 0;
	if (!splash_override) {
		switch (board_hardware_id()) {
		case HW_PLATFORM_MTP:
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_RCM:
		case HW_PLATFORM_QRD:
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

uint8_t target_panel_auto_detect_enabled()
{
	uint8_t ret = 0;

	switch(board_hardware_id())
	{
		case HW_PLATFORM_QRD:
			ret = platform_is_msm8956() ? 1 : 0;
			break;
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_MTP:
		default:
			ret = 0;
	}
	return ret;
}

/* Do any target specific intialization needed before entering fastboot mode */
void target_fastboot_init(void)
{
	if (target_is_ssd_enabled()) {
		clock_ce_enable(CE1_INSTANCE);
		target_load_ssd_keystore();
	}
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
void target_crypto_init_params()
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

bool target_is_pmi_enabled(void)
{
	if(platform_is_msm8917() &&
	   (board_hardware_subtype() ==	HW_PLATFORM_SUBTYPE_SAP_NOPMI))
		return 0;
	else
		return 1;
}

#if _APPEND_CMDLINE
int get_target_boot_params(const char *cmdline, const char *part, char **buf)
{
	int system_ptn_index = -1;
	uint32_t buflen;
	int ret = -1;

	if (!cmdline || !part ) {
		dprintf(CRITICAL, "WARN: Invalid input param\n");
		return -1;
	}

	if (!strstr(cmdline, "root=/dev/ram")) /* This check is to handle kdev boot */
	{
		if (target_is_emmc_boot()) {
			buflen = strlen(EXT4_CMDLINE) + sizeof(int) +1;
			*buf = (char *)malloc(buflen);
			if(!(*buf)) {
				dprintf(CRITICAL,"Unable to allocate memory for boot params\n");
				return -1;
			}
			/* Below is for emmc boot */
			system_ptn_index = partition_get_index(part) + 1; /* Adding +1 as offsets for eMMC start at 1 and NAND at 0 */
			if (system_ptn_index < 0) {
				dprintf(CRITICAL,
						"WARN: Cannot get partition index for %s\n", part);
				free(*buf);
				return -1;
			}
			snprintf(*buf, buflen, EXT4_CMDLINE"%d", system_ptn_index);
			ret = 0;
		}
	}
	/*in success case buf will be freed in the calling function of this*/
	return ret;
}
#endif

uint32_t target_get_pmic()
{
	return PMIC_IS_PMI8950;
}
