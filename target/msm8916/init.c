/* Copyright (c) 2014-2015,2017, The Linux Foundation. All rights reserved.
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
#include <pm8x41_regulator.h>
#include <board.h>
#include <baseband.h>
#include <hsusb.h>
#include <scm.h>
#include <platform/gpio.h>
#include <platform/gpio.h>
#include <platform/irqs.h>
#include <platform/clock.h>
#include <crypto5_wrapper.h>
#include <partition_parser.h>
#include <stdlib.h>
#include <secapp_loader.h>
#include <rpmb.h>
#include <smem.h>
#include <lk2nd-device.h>

#if LONG_PRESS_POWER_ON
#include <shutdown_detect.h>
#endif

#if PON_VIB_SUPPORT
#include <vibrator.h>
#endif

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0
#define TLMM_VOL_UP_BTN_GPIO    107
#define TLMM_SBC_USR_LED1_GPIO  21

#if PON_VIB_SUPPORT
#define VIBRATE_TIME    250
#endif

#define CE1_INSTANCE            1
#define CE_EE                   1
#define CE_FIFO_SIZE            64
#define CE_READ_PIPE            3
#define CE_WRITE_PIPE           2
#define CE_READ_PIPE_LOCK_GRP   0
#define CE_WRITE_PIPE_LOCK_GRP  0
#define CE_ARRAY_SIZE           20

static void set_sdc_power_ctrl(void);

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
	int uart = lk2nd_fdt_parse_early_uart();
	if (uart == 1)
		uart_dm_init(1, 0, BLSP1_UART0_BASE);
	else
		uart_dm_init(2, 0, BLSP1_UART1_BASE);
#endif
}

void target_sdc_init()
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

void *target_mmc_device()
{
	return (void *) dev;
}

/* Return 1 if vol_up pressed */
int target_volume_up()
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
uint32_t target_volume_down()
{
	/* Volume down button tied in with PMIC RESIN. */
	return pm8x41_resin_status();
}

static void target_keystatus()
{
	keys_init();

	if(target_volume_down())
		keys_post_event(KEY_VOLUMEDOWN, 1);

	if(target_volume_up())
		keys_post_event(KEY_VOLUMEUP, 1);
}

void target_init(void)
{
        uint32_t base_addr;
	uint8_t slot;
#if VERIFIED_BOOT
#if !VBOOT_MOTA
        int ret = 0;
#endif
#endif
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	target_keystatus();

	target_sdc_init();
	if (partition_read_table())
	{
		dprintf(CRITICAL, "Error reading the partition table info\n");
		ASSERT(0);
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

#if VERIFIED_BOOT
#if !VBOOT_MOTA
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
#endif
#endif
        /*
         * Turn on Boot LED
         */
        if (board_hardware_id() == HW_PLATFORM_SBC)
                gpio_tlmm_config(TLMM_SBC_USR_LED1_GPIO, 0, GPIO_OUTPUT,
                        GPIO_PULL_UP, GPIO_2MA, GPIO_ENABLE);
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
	case MSM8916:
	case MSM8116:
	case MSM8216:
	case MSM8616:
	case MSM8939:
	case MSM8236:
	case MSM8636:
	case MSM8936:
	case MSM8239:
	case MSM8929:
	case MSM8629:
	case MSM8229:
		board->baseband = BASEBAND_MSM;
	break;
	case APQ8016:
	case APQ8039:
	case APQ8036:
	case APQ8029:
		board->baseband = BASEBAND_APQ;
	break;
	default:
		dprintf(CRITICAL, "Platform type: %u is not supported\n", platform);
	ASSERT(0);
	};
}

unsigned target_baseband()
{
	return board_baseband();
}

int emmc_recovery_init(void)
{
	return _emmc_recovery_init();
}

static void set_sdc_power_ctrl()
{
	/* Drive strength configs for sdc pins */
	struct tlmm_cfgs sdc1_hdrv_cfg[] =
	{
		{ SDC1_CLK_HDRV_CTL_OFF,  TLMM_CUR_VAL_16MA, TLMM_HDRV_MASK },
		{ SDC1_CMD_HDRV_CTL_OFF,  TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK },
		{ SDC1_DATA_HDRV_CTL_OFF, TLMM_CUR_VAL_10MA, TLMM_HDRV_MASK },
	};

	/* Pull configs for sdc pins */
	struct tlmm_cfgs sdc1_pull_cfg[] =
	{
		{ SDC1_CLK_PULL_CTL_OFF,  TLMM_NO_PULL, TLMM_PULL_MASK },
		{ SDC1_CMD_PULL_CTL_OFF,  TLMM_PULL_UP, TLMM_PULL_MASK },
		{ SDC1_DATA_PULL_CTL_OFF, TLMM_PULL_UP, TLMM_PULL_MASK },
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
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

uint8_t target_panel_auto_detect_enabled()
{
	uint8_t ret = 0;
	uint32_t hw_subtype = board_hardware_subtype();

	switch(board_hardware_id()) {
	case HW_PLATFORM_SURF:
	case HW_PLATFORM_MTP:
		ret = 1;
		break;
	default:
		ret = 0;
		break;
	}
	return ret;
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
		case HW_PLATFORM_QRD:
		case HW_PLATFORM_SBC:
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

unsigned target_pause_for_battery_charge(void)
{
	uint8_t pon_reason = pm8x41_get_pon_reason();
	uint8_t is_cold_boot = pm8x41_get_is_cold_boot();
	dprintf(INFO, "%s : pon_reason is %d cold_boot:%d\n", __func__,
		pon_reason, is_cold_boot);
	/* In case of fastboot reboot,adb reboot or if we see the power key
	* pressed we do not want go into charger mode.
	* fastboot reboot is warm boot with PON hard reset bit not set
	* adb reboot is a cold boot with PON hard reset bit set
	*/
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

	mmc_put_card_to_sleep(dev);
	sdhci_mode_disable(&dev->host);

	if (crypto_initialized())
		crypto_eng_cleanup();

	if (target_is_ssd_enabled())
		clock_ce_disable(CE1_INSTANCE);
#if VERIFIED_BOOT
#if !VBOOT_MOTA
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
#endif
#endif
}

/* Do any target specific intialization needed before entering fastboot mode */
void target_fastboot_init(void)
{
	/* Set the BOOT_DONE flag in PM8916 */
	pm8x41_set_boot_done();

	if (target_is_ssd_enabled()) {
		clock_ce_enable(CE1_INSTANCE);
		target_load_ssd_keystore();
	}
}

int set_download_mode(enum dload_mode mode)
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

uint32_t target_get_hlos_subtype()
{
	return board_hlos_subtype();
}

void pmic_reset_configure(uint8_t reset_type)
{
	pm8x41_reset_configure(reset_type);
}

uint32_t target_get_pmic()
{
	return PMIC_IS_PM8916;
}

static struct spmi_regulator pm8916_regulators[] = {
	{"s1", 0x11400},
	{"s2", 0x11700},
	{"s3", 0x11a00},
	{"s4", 0x11d00},
	{"l1", 0x14000},
	{"l2", 0x14100},
	{"l3", 0x14200},
	{"l4", 0x14300},
	{"l5", 0x14400},
	{"l6", 0x14500},
	{"l7", 0x14600},
	{"l8", 0x14700},
	{"l9", 0x14800},
	{"l10", 0x14900},
	{"l11", 0x14a00},
	{"l12", 0x14b00},
	{"l13", 0x14c00},
	{"l14", 0x14d00},
	{"l15", 0x14e00},
	{"l16", 0x14f00},
	{"l17", 0x15000},
	{"l18", 0x15100},
	{ },
};

struct spmi_regulator* target_get_regulators()
{
	regulators_init(pm8916_regulators);
	return pm8916_regulators;
}
