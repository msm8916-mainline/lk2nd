/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
#include <platform/gpio.h>
#include <reg.h>
#include <target.h>
#include <platform.h>
#include <dload_util.h>
#include <uart_dm.h>
#include <mmc.h>
#include <spmi.h>
#include <board.h>
#include <smem.h>
#include <baseband.h>
#include <dev/keys.h>
#include <pm8x41.h>
#include <crypto5_wrapper.h>
#include <hsusb.h>
#include <clock.h>
#include <partition_parser.h>
#include <scm.h>
#include <platform/clock.h>
#include <platform/gpio.h>
#include <platform/timer.h>
#include <stdlib.h>
#include <ufs.h>

#define PMIC_ARB_CHANNEL_NUM    0
#define PMIC_ARB_OWNER_ID       0

#define FASTBOOT_MODE           0x77665500

#define BOOT_DEVICE_MASK(val)   ((val & 0x3E) >>1)

enum cdp_subtype
{
	CDP_SUBTYPE_SMB349 = 0,
	CDP_SUBTYPE_9x25_SMB349,
	CDP_SUBTYPE_9x25_SMB1357,
	CDP_SUBTYPE_9x35,
	CDP_SUBTYPE_SMB1357,
	CDP_SUBTYPE_SMB350
};

enum mtp_subtype
{
	MTP_SUBTYPE_SMB349 = 0,
	MTP_SUBTYPE_9x25_SMB349,
	MTP_SUBTYPE_9x25_SMB1357,
	MTP_SUBTYPE_9x35,
};

enum rcm_subtype
{
	RCM_SUBTYPE_SMB349 = 0,
	RCM_SUBTYPE_9x25_SMB349,
	RCM_SUBTYPE_9x25_SMB1357,
	RCM_SUBTYPE_9x35,
	RCM_SUBTYPE_SMB1357,
	RCM_SUBTYPE_SMB350
};

enum liquid_subtype
{
	LIQUID_SUBTYPE_STANDALONE = 0,
	LIQUID_SUBTYPE_9x25,
};

static void set_sdc_power_ctrl(void);
static uint32_t mmc_pwrctl_base[] =
	{ MSM_SDC1_BASE, MSM_SDC2_BASE };

static uint32_t mmc_sdhci_base[] =
	{ MSM_SDC1_SDHCI_BASE, MSM_SDC2_SDHCI_BASE };

static uint32_t  mmc_sdc_pwrctl_irq[] =
	{ SDCC1_PWRCTL_IRQ, SDCC2_PWRCTL_IRQ };

struct mmc_device *dev;
struct ufs_dev ufs_device;

extern void ulpi_write(unsigned val, unsigned reg);

void target_early_init(void)
{
#if WITH_DEBUG_UART
	uart_dm_init(7, 0, BLSP2_UART1_BASE);
#endif
}

/* Return 1 if vol_up pressed */
static int target_volume_up()
{
	uint8_t status = 0;
	struct pm8x41_gpio gpio;

	/* Configure the GPIO */
	gpio.direction = PM_GPIO_DIR_IN;
	gpio.function  = 0;
	gpio.pull      = PM_GPIO_PULL_UP_30;
	gpio.vin_sel   = 2;

	pm8x41_gpio_config(2, &gpio);

	/* Get status of P_GPIO_2 */
	pm8x41_gpio_get(2, &status);

	return !status; /* active low */
}

/* Return 1 if vol_down pressed */
uint32_t target_volume_down()
{
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

void target_uninit(void)
{
	if(target_boot_device_emmc())
		mmc_put_card_to_sleep(dev);
}

/* Do target specific usb initialization */
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
		{ SDC1_RCLK_PULL_CTL_OFF, TLMM_PULL_DOWN, TLMM_PULL_MASK },
	};

	/* Set the drive strength & pull control values */
	tlmm_set_hdrive_ctrl(sdc1_hdrv_cfg, ARRAY_SIZE(sdc1_hdrv_cfg));
	tlmm_set_pull_ctrl(sdc1_pull_cfg, ARRAY_SIZE(sdc1_pull_cfg));
}

void target_sdc_init()
{
	struct mmc_config_data config;

	/* Set drive strength & pull ctrl values */
	set_sdc_power_ctrl();

	config.bus_width = DATA_BUS_WIDTH_8BIT;

	/* Try slot 1*/
	config.slot = 1;
	config.max_clk_rate = MMC_CLK_192MHZ;
	config.sdhc_base    = mmc_sdhci_base[config.slot - 1];
	config.pwrctl_base  = mmc_pwrctl_base[config.slot - 1];
	config.pwr_irq      = mmc_sdc_pwrctl_irq[config.slot - 1];

	if (!(dev = mmc_init(&config)))
	{
		/* Try slot 2 */
		config.slot = 2;
		config.max_clk_rate = MMC_CLK_200MHZ;
		config.sdhc_base    = mmc_sdhci_base[config.slot - 1];
		config.pwrctl_base  = mmc_pwrctl_base[config.slot - 1];
		config.pwr_irq      = mmc_sdc_pwrctl_irq[config.slot - 1];

		if (!(dev = mmc_init(&config)))
		{
			dprintf(CRITICAL, "mmc init failed!");
			ASSERT(0);
		}
	}
}

static uint32_t boot_device;
static uint32_t target_read_boot_config()
{
	uint32_t val;

	val = readl(BOOT_CONFIG_REG);

	val = BOOT_DEVICE_MASK(val);

	return val;
}

uint32_t target_get_boot_device()
{
	return boot_device;
}

/*
 * Return 1 if boot from emmc else 0
 */
uint32_t target_boot_device_emmc()
{
	uint32_t boot_dev_type;

	boot_dev_type = target_get_boot_device();

	if (boot_dev_type == BOOT_EMMC || boot_dev_type == BOOT_DEFAULT)
		boot_dev_type = 1;
	else
		boot_dev_type = 0;

	return boot_dev_type;
}

void *target_mmc_device()
{
	if (target_boot_device_emmc())
		return (void *) dev;
	else
		return (void *) &ufs_device;
}

void target_init(void)
{
	dprintf(INFO, "target_init()\n");

	spmi_init(PMIC_ARB_CHANNEL_NUM, PMIC_ARB_OWNER_ID);

	target_keystatus();

	boot_device = target_read_boot_config();

	if (target_boot_device_emmc())
		target_sdc_init();
	else
	{
		ufs_device.base = UFS_BASE;
		ufs_init(&ufs_device);
	}

	/* Storage initialization is complete, read the partition table info */
	if (partition_read_table())
	{
		dprintf(CRITICAL, "Error reading the partition table info\n");
		ASSERT(0);
	}

	/* Display splash screen if enabled */
#if DISPLAY_SPLASH_SCREEN
	dprintf(INFO, "Display Init: Start\n");
	display_init();
	dprintf(INFO, "Display Init: Done\n");
#endif
}

unsigned board_machtype(void)
{
	return LINUX_MACHTYPE_UNKNOWN;
}

/* Detect the target type */
void target_detect(struct board_data *board)
{
	board->target = LINUX_MACHTYPE_UNKNOWN;
}

void set_cdp_baseband(struct board_data *board)
{

	uint32_t platform_subtype;
	platform_subtype = board->platform_subtype;

	switch(platform_subtype) {
	case CDP_SUBTYPE_9x25_SMB349:
	case CDP_SUBTYPE_9x25_SMB1357:
		board->baseband = BASEBAND_MDM;
		break;
	case CDP_SUBTYPE_9x35:
		board->baseband = BASEBAND_MDM2;
		break;
	case CDP_SUBTYPE_SMB349:
	case CDP_SUBTYPE_SMB1357:
	case CDP_SUBTYPE_SMB350:
		board->baseband = BASEBAND_APQ;
		break;
	default:
		dprintf(CRITICAL, "CDP platform subtype :%u is not supported\n",
				platform_subtype);
		ASSERT(0);
	};

}

void set_mtp_baseband(struct board_data *board)
{

	uint32_t platform_subtype;
	platform_subtype = board->platform_subtype;

	switch(platform_subtype) {
	case MTP_SUBTYPE_9x25_SMB349:
	case MTP_SUBTYPE_9x25_SMB1357:
		board->baseband = BASEBAND_MDM;
		break;
	case MTP_SUBTYPE_9x35:
		board->baseband = BASEBAND_MDM2;
		break;
	case MTP_SUBTYPE_SMB349:
		board->baseband = BASEBAND_APQ;
		break;
	default:
		dprintf(CRITICAL, "MTP platform subtype :%u is not supported\n",
				platform_subtype);
		ASSERT(0);
	};
}

void set_rcm_baseband(struct board_data *board)
{
	uint32_t platform_subtype;
	platform_subtype = board->platform_subtype;

	switch(platform_subtype) {
	case RCM_SUBTYPE_9x25_SMB349:
	case RCM_SUBTYPE_9x25_SMB1357:
		board->baseband = BASEBAND_MDM;
		break;
	case RCM_SUBTYPE_9x35:
		board->baseband = BASEBAND_MDM2;
		break;
	case RCM_SUBTYPE_SMB349:
	case RCM_SUBTYPE_SMB1357:
	case RCM_SUBTYPE_SMB350:
		board->baseband = BASEBAND_APQ;
		break;
	default:
		dprintf(CRITICAL, "RCM platform subtype :%u is not supported\n",
				platform_subtype);
		ASSERT(0);
	};
}

void set_liquid_baseband(struct board_data *board)
{
	uint32_t platform_subtype;

	platform_subtype = board->platform_subtype;

	switch(platform_subtype)
	{
		case LIQUID_SUBTYPE_STANDALONE:
			board->baseband = BASEBAND_APQ;
			break;
		case LIQUID_SUBTYPE_9x25:
			board->baseband = BASEBAND_MDM;
			break;
		default:
			dprintf(CRITICAL, "Liquid platform subtype :%u is not supported\n",platform_subtype);
			ASSERT(0);
	}
}

/* Returns 1 if target supports continuous splash screen. */
int target_cont_splash_screen()
{
	switch(board_hardware_id())
	{
		case HW_PLATFORM_SURF:
		case HW_PLATFORM_MTP:
		case HW_PLATFORM_FLUID:
		case HW_PLATFORM_LIQUID:
			dprintf(SPEW, "Target_cont_splash=1\n");
			return 1;
		default:
			dprintf(SPEW, "Target_cont_splash=0\n");
			return 0;
	}
}

/* Detect the modem type */
void target_baseband_detect(struct board_data *board)
{
	uint32_t platform;
	uint32_t platform_subtype;
	uint32_t platform_hardware;

	platform = board->platform;

	platform_hardware = board->platform_hw;

	switch(platform_hardware) {
	case HW_PLATFORM_SURF:
		set_cdp_baseband(board);
		break;
	case HW_PLATFORM_MTP:
		set_mtp_baseband(board);
		break;
	case HW_PLATFORM_RCM:
		set_rcm_baseband(board);
		break;
	case HW_PLATFORM_LIQUID:
		set_liquid_baseband(board);
		break;
	default:
		dprintf(CRITICAL, "Platform :%u is not supported\n",
				platform_hardware);
		ASSERT(0);
	};
}

unsigned target_baseband()
{
	return board_baseband();
}

void target_serialno(unsigned char *buf)
{
	unsigned int serialno;
	if (target_is_emmc_boot()) {
		serialno = mmc_get_psn();
		snprintf((char *)buf, 13, "%x", serialno);
	}
}

unsigned check_reboot_mode(void)
{
	uint32_t restart_reason = 0;
	uint32_t restart_reason_addr;

	restart_reason_addr = RESTART_REASON_ADDR;

	/* Read reboot reason and scrub it */
	restart_reason = readl(restart_reason_addr);
	writel(0x00, restart_reason_addr);

	return restart_reason;
}

void reboot_device(unsigned reboot_reason)
{
	uint8_t reset_type = 0;

	/* Write the reboot reason */
	writel(reboot_reason, RESTART_REASON_ADDR);

	if(reboot_reason == FASTBOOT_MODE)
		reset_type = PON_PSHOLD_WARM_RESET;
	else
		reset_type = PON_PSHOLD_HARD_RESET;

	pm8x41_reset_configure(reset_type);

	/* Drop PS_HOLD for MSM */
	writel(0x00, MPM2_MPM_PS_HOLD);

	mdelay(5000);

	dprintf(CRITICAL, "Rebooting failed\n");
}

/* identify the usb controller to be used for the target */
const char * target_usb_controller()
{
	return "dwc";
}

/* mux hs phy to route to dwc controller */
static void phy_mux_configure_with_jdr()
{
	uint32_t val;

	val = readl(COPSS_USB_CONTROL_WITH_JDR);

	/* Note: there are no details regarding this bit in hpg or swi. */
	val |= BIT(8);

	writel(val, COPSS_USB_CONTROL_WITH_JDR);
}

/* configure hs phy mux if using dwc controller */
void target_usb_phy_mux_configure(void)
{
	if(!strcmp(target_usb_controller(), "dwc"))
	{
		phy_mux_configure_with_jdr();
	}
}

void target_usb_phy_reset(void)
{
	uint32_t val;

	/* SS PHY reset */
	val = readl(GCC_USB3_PHY_BCR) | BIT(0);
	writel(val, GCC_USB3_PHY_BCR);
	udelay(10);
	writel(val & ~BIT(0), GCC_USB3_PHY_BCR);

	/* HS PHY reset */
	/* Note: reg/bit details are not mentioned in hpg or swi. */
	val = readl(COPSS_USB_CONTROL_WITH_JDR) | BIT(11);
	writel(val, COPSS_USB_CONTROL_WITH_JDR);
	udelay(10);
	writel(val & ~BIT(11), COPSS_USB_CONTROL_WITH_JDR);

	/* PHY_COMMON reset */
	val = readl(GCC_USB30_PHY_COM_BCR) | BIT(0);
	writel(val, GCC_USB30_PHY_COM_BCR);
	udelay(10);
	writel(val & ~BIT(0), GCC_USB30_PHY_COM_BCR);
}
