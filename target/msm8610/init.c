/* Copyright (c) 2013-2014, 2016, The Linux Foundation. All rights reserved.
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
#include <platform/irqs.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <dload_util.h>
#include <uart_dm.h>
#include <mmc.h>
#include <platform/gpio.h>
#include <spmi.h>
#include <board.h>
#include <smem.h>
#include <baseband.h>
#include <dev/keys.h>
#include <pm8x41.h>
#include <hsusb.h>
#include <kernel/thread.h>
#include <arch/defines.h>
#include <stdlib.h>
#include <scm.h>
#include <partition_parser.h>
#include <platform/clock.h>
#include <platform/timer.h>
#include <crypto5_wrapper.h>
#include <scm.h>
#include <sdhci_msm.h>

#if LONG_PRESS_POWER_ON
#include <shutdown_detect.h>
#endif

#if PON_VIB_SUPPORT
#include <vibrator.h>
#endif

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0

#define CRYPTO_ENGINE_INSTANCE             1
#define CRYPTO_ENGINE_EE                   1
#define CRYPTO_ENGINE_FIFO_SIZE            64
#define CRYPTO_ENGINE_READ_PIPE            3
#define CRYPTO_ENGINE_WRITE_PIPE           2
#define CRYPTO_READ_PIPE_LOCK_GRP          0
#define CRYPTO_WRITE_PIPE_LOCK_GRP         0
#define CRYPTO_ENGINE_CMD_ARRAY_SIZE       20

#define TLMM_VOL_UP_BTN_GPIO    72

#if PON_VIB_SUPPORT
#define VIBRATE_TIME    250
#endif

enum target_subtype {
	HW_PLATFORM_SUBTYPE_SKUAA = 1,
	HW_PLATFORM_SUBTYPE_SKUF = 2,
	HW_PLATFORM_SUBTYPE_SKUAB = 3,
	HW_PLATFORM_SUBTYPE_NO4 = 4,
};

static void set_sdc_power_ctrl(void);

static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

struct mmc_device *dev;

void target_crypto_init_params(void);

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(2, 0, BLSP1_UART1_BASE);
#endif
}

/* Return 1 if vol_up pressed */
int target_volume_up(void)
{
	uint8_t status = 0;

	gpio_tlmm_config(TLMM_VOL_UP_BTN_GPIO, 0, GPIO_INPUT, GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);

	/* Wait for the configuration to complete.*/
	thread_sleep(1);
	/* Get status of GPIO */
	status = gpio_status(TLMM_VOL_UP_BTN_GPIO);

	/* Active low signal. */
	return !status;
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down(void)
{
	/* Volume down button tied in with PMIC RESIN. */
	return pm8x41_resin_status();
}

static void target_keystatus(void)
{
	/* Initial key status is handled in lk2nd/device/keys.c */
	if (IS_ENABLED(WITH_LK2ND_DEVICE))
		return;

	keys_init();

	if(target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if(target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);
}

void target_sdc_init(void)
{
	struct mmc_config_data config;

	/* Set drive strength & pull ctrl values */
	set_sdc_power_ctrl();

	config.bus_width = DATA_BUS_WIDTH_8BIT;
	config.max_clk_rate = MMC_CLK_200MHZ;

	/* Try slot 1*/
	config.slot = 1;
	config.sdhc_base = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base = mmc_pwrctl_base[config.slot - 1];
	config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];
	config.hs400_support = 0;

	if (!(dev = mmc_init(&config)))
	{
		/* Try slot 2 */
		config.slot = 2;
		config.sdhc_base = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base = mmc_pwrctl_base[config.slot - 1];
		config.pwr_irq     = mmc_sdc_pwrctl_irq[config.slot - 1];

		if (!(dev = mmc_init(&config)))
		{
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}

	/* MMC initialization is complete, read the partition table info */
	if (partition_read_table())
	{
		dprintf(CRITICAL, "Error reading the partition table info\n");
		ASSERT(0);
	}
}

void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	target_keystatus();

	target_sdc_init();

#if LONG_PRESS_POWER_ON
	shutdown_detect();
#endif

#if PON_VIB_SUPPORT
	/* turn on vibrator to indicate that phone is booting up to end user */
	vib_timed_turn_on(VIBRATE_TIME);
#endif

	if (target_use_signed_kernel())
		target_crypto_init_params();
}

void target_uninit(void)
{
#if PON_VIB_SUPPORT
	/* wait for the vibrator timer is expried */
	wait_vib_timeout();
#endif

	mmc_put_card_to_sleep(dev);

	if (crypto_initialized())
		crypto_eng_cleanup();

	/* Disable HC mode before jumping to kernel */
	sdhci_mode_disable(&dev->host);
}

#define SSD_CE_INSTANCE         1

void target_load_ssd_keystore(void)
{
	uint64_t ptn;
	int      index;
	uint64_t size;
	uint32_t *buffer;

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

	clock_ce_enable(SSD_CE_INSTANCE);
	scm_protect_keystore(buffer, size);
	clock_ce_disable(SSD_CE_INSTANCE);
	free(buffer);
}

/* Do any target specific intialization needed before entering fastboot mode */
void target_fastboot_init(void)
{
	/* Set the BOOT_DONE flag in PM8110 */
	pm8x41_set_boot_done();

	if (target_is_ssd_enabled()) {
		clock_ce_enable(SSD_CE_INSTANCE);
		target_load_ssd_keystore();
	}

}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	/*
	* already fill the board->target on board.c
	*/

}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;
	uint32_t platform_subtype;

	platform         = board->platform;
	platform_subtype = board->platform_subtype;

	/*
	 * Look for platform subtype if present, else
	 * check for platform type to decide on the
	 * baseband type
	 */
	switch(platform_subtype)
	{
	case HW_PLATFORM_SUBTYPE_UNKNOWN:
		break;
	case HW_PLATFORM_SUBTYPE_SKUAA:
		break;
	case HW_PLATFORM_SUBTYPE_SKUF:
		break;
	case HW_PLATFORM_SUBTYPE_SKUAB:
		break;
	case HW_PLATFORM_SUBTYPE_NO4:
		break;
	default:
		dprintf(CRITICAL, "Platform Subtype : %u is not supported\n", platform_subtype);
		ASSERT(0);
	};

	switch(platform)
	{
	case MSM8610:
	case MSM8110:
	case MSM8210:
	case MSM8810:
	case MSM8612:
	case MSM8212:
	case MSM8812:
	case MSM8510:
	case MSM8512:
		board->baseband = BASEBAND_MSM;
		break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n", platform);
		ASSERT(0);
	};
}

unsigned target_baseband(void)
{
	return board_baseband();
}

void target_serialno(unsigned char *buf)
{
	uint32_t serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

unsigned check_reboot_mode(void)
{
	uint32_t restart_reason = 0;

	/* Read reboot reason and scrub it */
	restart_reason = readl(RESTART_REASON_ADDR);
	writel(0x00, RESTART_REASON_ADDR);

	return restart_reason;
}

int set_download_mode(enum reboot_reason mode)
{
	if (mode == NORMAL_DLOAD || mode == EMERGENCY_DLOAD) {
		dload_util_write_cookie(mode == NORMAL_DLOAD ?
			DLOAD_MODE_ADDR : EMERGENCY_DLOAD_MODE_ADDR, mode);

		pm8x41_clear_pmic_watchdog();
	}

	return 0;
}

void reboot_device(unsigned reboot_reason)
{
	int ret = 0;

	/* Set cookie for dload mode */
	if(set_download_mode(reboot_reason)) {
		dprintf(CRITICAL, "HALT: set_download_mode not supported\n");
		return;
	}

	writel(reboot_reason, RESTART_REASON_ADDR);

	/* Configure PMIC for warm reset */
	pm8x41_reset_configure(PON_PSHOLD_WARM_RESET);

	ret = scm_halt_pmic_arbiter();
	if (ret)
		dprintf(CRITICAL , "Failed to halt pmic arbiter: %d\n", ret);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
}

/* Returns 1 if autopanel detection is enabled for the target. */
uint8_t target_panel_auto_detect_enabled(void)
{
	int ret = 0;

	switch(board_hardware_id())
	{
		case HW_PLATFORM_MTP:
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_QRD:
		default:
			dprintf(SPEW, "Panel auto-detection is disabled\n");
			ret = 0;
	}
	return ret;
}

static uint8_t splash_override;

/* Returns 1 if target supports continuous splash screen. */
int target_cont_splash_screen(void)
{
	uint8_t splash_screen = 0;
	if(!splash_override) {
		switch(board_hardware_id())
		{
			case HW_PLATFORM_QRD:
			case HW_PLATFORM_MTP:
			case HW_PLATFORM_SURF:
				dprintf(SPEW, "Target_cont_splash=1\n");
				splash_screen = 1;
				break;
			default:
				dprintf(SPEW, "Target_cont_splash=0\n");
				splash_screen = 0;
		}
	}
	return splash_screen;
}

void target_force_cont_splash_disable(uint8_t override)
{
	splash_override = override;
}

unsigned target_pause_for_battery_charge(void)
{
	uint8_t pon_reason = pm8x41_get_pon_reason();
	uint8_t is_cold_boot = pm8x41_get_is_cold_boot();
	dprintf(INFO, "%s : pon_reason is %d cold_boot:%d\n", __func__,
					pon_reason, is_cold_boot);
	/*In case of fastboot reboot, adb reboot or if we see the power key
	 * pressed we do not want go into charger mode.
	 * fastboot reboot is warm boot with PON hard reset bit not set
	 * adb reboot is a cold boot with PON hard reset bit set
	 */
	if (is_cold_boot &&
					(!(pon_reason & HARD_RST)) &&
					(!(pon_reason & KPDPWR_N)) &&
					((pon_reason & USB_CHG) || (pon_reason & DC_CHG)))
			return 1;
	else
			return 0;
}

void target_usb_stop(void)
{
	/* Disable VBUS mimicing in the controller. */
	ulpi_write(ULPI_MISC_A_VBUSVLDEXTSEL | ULPI_MISC_A_VBUSVLDEXT, ULPI_MISC_A_CLEAR);
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


unsigned board_machtype(void)
{
	return 0;
}

static void set_sdc_power_ctrl(void)
{

	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK, 0 },
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK, 0 },
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_6MA, TLMM_HDRV_MASK, 0 },
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK, 0 },
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK, 0 },
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK, 0 },
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
}

void *target_mmc_device(void)
{
	return (void *) dev;
}

/* Set up params for h/w CRYPTO_ENGINE. */
void target_crypto_init_params(void)
{
	struct crypto_init_params ce_params;

	/* Set up base addresses and instance. */
	ce_params.crypto_instance  = CRYPTO_ENGINE_INSTANCE;
	ce_params.crypto_base      = MSM_CE1_BASE;
	ce_params.bam_base         = MSM_CE1_BAM_BASE;

	/* Set up BAM config. */
	ce_params.bam_ee               = CRYPTO_ENGINE_EE;
	ce_params.pipes.read_pipe      = CRYPTO_ENGINE_READ_PIPE;
	ce_params.pipes.write_pipe     = CRYPTO_ENGINE_WRITE_PIPE;
	ce_params.pipes.read_pipe_grp  = CRYPTO_READ_PIPE_LOCK_GRP;
	ce_params.pipes.write_pipe_grp = CRYPTO_WRITE_PIPE_LOCK_GRP;

	/* Assign buffer sizes. */
	ce_params.num_ce           = CRYPTO_ENGINE_CMD_ARRAY_SIZE;
	ce_params.read_fifo_size   = CRYPTO_ENGINE_FIFO_SIZE;
	ce_params.write_fifo_size  = CRYPTO_ENGINE_FIFO_SIZE;

	ce_params.do_bam_init = 0;

	crypto_init_params(&ce_params);
}

/* Configure PMIC and Drop PS_HOLD for shutdown */
void shutdown_device(void)
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

crypto_engine_type board_ce_type(void)
{
	return CRYPTO_ENGINE_TYPE_HW;
}
